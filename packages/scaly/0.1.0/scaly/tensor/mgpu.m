// Metal GPU shim (stage 6.0 spike + 6.1 whole-step encoding).
// Containment rule (a): Metal / MetalPerformanceShaders are macOS-only
// Objective-C APIs (OS-specific symbols and object types) — the
// committed seed ships one scaly.ll for all targets, so this backend
// cannot live in Scaly code. One file, one concern: encode tensor-tape
// ops (GEMM via MPS, the long tail via MSL compute kernels) into
// command buffers over storage-mode-shared MTLBuffers.
//
// NOT part of libscaly.a (decided 6.1): the shim's callers must link
// the Metal frameworks, and a libscaly.a member referencing
// scaly_mgpu_* would break every non-macOS AOT link with undefined
// symbols. tools/mgpu.sh compiles it; GPU programs link it explicitly.
// Buffers are MTLResourceStorageModeShared — on Apple Silicon that is
// plain unified memory: scaly_mgpu_contents() returns a host pointer
// the CPU can fill directly (the zero-copy path); scaly_mgpu_write /
// scaly_mgpu_read are the explicit-transfer path we measure against it.
//
// A *_reps parameter > 1 encodes that many multiplications into ONE
// command buffer before commit+wait — the whole-step-encoding probe
// (per-dispatch launch cost vs amortized), per the stage-5→6 bridge
// lesson. scaly_mgpu_last_gpu_ms() reports the last command buffer's
// pure GPU time (GPUEndTime - GPUStartTime).

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>

static id<MTLDevice> mgpu_dev;
static id<MTLCommandQueue> mgpu_queue;
static id<MTLComputePipelineState> mgpu_pso_mm4x4;
static double mgpu_last_gpu_ms;

// The extraction-path kernel: the same 4x4 register tile per thread as
// the CPU mm_tiled kernel in scaly/tensor.scaly — what a stage-6 MSL
// codegen would emit from the already-parallel-classified loop.
// Requires m % 4 == 0 and n % 4 == 0 (the bench shapes all qualify).
static NSString *const mgpu_msl_src = @""
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"kernel void mm4x4(device const float* A [[buffer(0)]],\n"
"                  device const float* B [[buffer(1)]],\n"
"                  device float* C [[buffer(2)]],\n"
"                  constant uint* dims [[buffer(3)]],\n"
"                  uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint m = dims[0], K = dims[1], n = dims[2];\n"
"    uint i0 = gid.y * 4, j0 = gid.x * 4;\n"
"    if (i0 >= m || j0 >= n) return;\n"
"    float acc[4][4] = {{0.0f}};\n"
"    for (uint x = 0; x < K; ++x) {\n"
"        float a0 = A[(i0+0)*K + x];\n"
"        float a1 = A[(i0+1)*K + x];\n"
"        float a2 = A[(i0+2)*K + x];\n"
"        float a3 = A[(i0+3)*K + x];\n"
"        float b0 = B[x*n + j0+0];\n"
"        float b1 = B[x*n + j0+1];\n"
"        float b2 = B[x*n + j0+2];\n"
"        float b3 = B[x*n + j0+3];\n"
"        acc[0][0] += a0*b0; acc[0][1] += a0*b1; acc[0][2] += a0*b2; acc[0][3] += a0*b3;\n"
"        acc[1][0] += a1*b0; acc[1][1] += a1*b1; acc[1][2] += a1*b2; acc[1][3] += a1*b3;\n"
"        acc[2][0] += a2*b0; acc[2][1] += a2*b1; acc[2][2] += a2*b2; acc[2][3] += a2*b3;\n"
"        acc[3][0] += a3*b0; acc[3][1] += a3*b1; acc[3][2] += a3*b2; acc[3][3] += a3*b3;\n"
"    }\n"
"    for (uint r = 0; r < 4; ++r)\n"
"        for (uint c = 0; c < 4; ++c)\n"
"            C[(i0+r)*n + j0+c] = acc[r][c];\n"
"}\n";

int scaly_mgpu_available(void)
{
    @autoreleasepool {
        id<MTLDevice> d = MTLCreateSystemDefaultDevice();
        return d != nil ? 1 : 0;
    }
}

// 0 on success, nonzero on failure (no device / shader compile error).
int scaly_mgpu_init(void)
{
    if (mgpu_queue != nil)
        return 0;
    mgpu_dev = MTLCreateSystemDefaultDevice();
    if (mgpu_dev == nil)
        return 1;
    mgpu_queue = [mgpu_dev newCommandQueue];
    NSError *err = nil;
    id<MTLLibrary> lib = [mgpu_dev newLibraryWithSource:mgpu_msl_src options:nil error:&err];
    if (lib == nil) {
        fprintf(stderr, "mgpu: MSL compile failed: %s\n", err.localizedDescription.UTF8String);
        return 2;
    }
    id<MTLFunction> fn = [lib newFunctionWithName:@"mm4x4"];
    mgpu_pso_mm4x4 = [mgpu_dev newComputePipelineStateWithFunction:fn error:&err];
    if (mgpu_pso_mm4x4 == nil) {
        fprintf(stderr, "mgpu: pipeline failed: %s\n", err.localizedDescription.UTF8String);
        return 3;
    }
    return 0;
}

void *scaly_mgpu_alloc(size_t bytes)
{
    id<MTLBuffer> b = [mgpu_dev newBufferWithLength:bytes options:MTLResourceStorageModeShared];
    return (__bridge_retained void *)b;
}

void scaly_mgpu_release(void *buf)
{
    id<MTLBuffer> b = (__bridge_transfer id<MTLBuffer>)buf;
    (void)b;
}

void *scaly_mgpu_contents(void *buf)
{
    return [(__bridge id<MTLBuffer>)buf contents];
}

void scaly_mgpu_write(void *buf, const void *src, size_t bytes)
{
    memcpy([(__bridge id<MTLBuffer>)buf contents], src, bytes);
}

void scaly_mgpu_read(void *buf, void *dst, size_t bytes)
{
    memcpy(dst, [(__bridge id<MTLBuffer>)buf contents], bytes);
}

// newBufferWithLength does not guarantee zeroed contents — scratch
// buffers (Adam moments, layernorm stats, row losses) start here.
void scaly_mgpu_zero(void *buf, size_t bytes)
{
    memset([(__bridge id<MTLBuffer>)buf contents], 0, bytes);
}

// C = A * B, f32 row-major (A m x k, B k x n, C m x n), via
// MPSMatrixMultiplication. Encodes `reps` multiplications into one
// command buffer, commits, blocks. Returns 0 on success.
int scaly_mgpu_mps_sgemm(void *a, void *b, void *c, int m, int k, int n, int reps)
{
    @autoreleasepool {
        if (mgpu_queue == nil)
            return 1;
        id<MTLBuffer> ab = (__bridge id<MTLBuffer>)a;
        id<MTLBuffer> bb = (__bridge id<MTLBuffer>)b;
        id<MTLBuffer> cb_ = (__bridge id<MTLBuffer>)c;
        MPSMatrixDescriptor *da = [MPSMatrixDescriptor matrixDescriptorWithRows:m columns:k rowBytes:(NSUInteger)k * 4 dataType:MPSDataTypeFloat32];
        MPSMatrixDescriptor *db = [MPSMatrixDescriptor matrixDescriptorWithRows:k columns:n rowBytes:(NSUInteger)n * 4 dataType:MPSDataTypeFloat32];
        MPSMatrixDescriptor *dc = [MPSMatrixDescriptor matrixDescriptorWithRows:m columns:n rowBytes:(NSUInteger)n * 4 dataType:MPSDataTypeFloat32];
        MPSMatrix *ma = [[MPSMatrix alloc] initWithBuffer:ab descriptor:da];
        MPSMatrix *mb = [[MPSMatrix alloc] initWithBuffer:bb descriptor:db];
        MPSMatrix *mc = [[MPSMatrix alloc] initWithBuffer:cb_ descriptor:dc];
        MPSMatrixMultiplication *mm = [[MPSMatrixMultiplication alloc] initWithDevice:mgpu_dev transposeLeft:NO transposeRight:NO resultRows:m resultColumns:n interiorColumns:k alpha:1.0 beta:0.0];
        id<MTLCommandBuffer> cmd = [mgpu_queue commandBuffer];
        for (int r = 0; r < reps; ++r)
            [mm encodeToCommandBuffer:cmd leftMatrix:ma rightMatrix:mb resultMatrix:mc];
        [cmd commit];
        [cmd waitUntilCompleted];
        mgpu_last_gpu_ms = (cmd.GPUEndTime - cmd.GPUStartTime) * 1000.0;
        return cmd.status == MTLCommandBufferStatusCompleted ? 0 : 4;
    }
}

// Same contract via the hand-written MSL 4x4-tile kernel.
int scaly_mgpu_msl_matmul(void *a, void *b, void *c, int m, int k, int n, int reps)
{
    @autoreleasepool {
        if (mgpu_pso_mm4x4 == nil)
            return 1;
        id<MTLBuffer> ab = (__bridge id<MTLBuffer>)a;
        id<MTLBuffer> bb = (__bridge id<MTLBuffer>)b;
        id<MTLBuffer> cb_ = (__bridge id<MTLBuffer>)c;
        uint32_t dims[3] = { (uint32_t)m, (uint32_t)k, (uint32_t)n };
        id<MTLCommandBuffer> cmd = [mgpu_queue commandBuffer];
        id<MTLComputeCommandEncoder> enc = [cmd computeCommandEncoder];
        [enc setComputePipelineState:mgpu_pso_mm4x4];
        [enc setBuffer:ab offset:0 atIndex:0];
        [enc setBuffer:bb offset:0 atIndex:1];
        [enc setBuffer:cb_ offset:0 atIndex:2];
        [enc setBytes:dims length:sizeof(dims) atIndex:3];
        MTLSize grid = MTLSizeMake((NSUInteger)(n / 4), (NSUInteger)(m / 4), 1);
        MTLSize tg = MTLSizeMake(8, 8, 1);
        for (int r = 0; r < reps; ++r)
            [enc dispatchThreads:grid threadsPerThreadgroup:tg];
        [enc endEncoding];
        [cmd commit];
        [cmd waitUntilCompleted];
        mgpu_last_gpu_ms = (cmd.GPUEndTime - cmd.GPUStartTime) * 1000.0;
        return cmd.status == MTLCommandBufferStatusCompleted ? 0 : 4;
    }
}

double scaly_mgpu_last_gpu_ms(void)
{
    return mgpu_last_gpu_ms;
}

// ===================================================================
// 6.1 whole-step encoding: the tensor tape's op set as encodable GPU
// ops. The Scaly driver (demo program) walks its tape — the tape IS
// the graph, nodes in id order — and encodes forward, gradient-zeroing,
// backward and Adam into ONE command buffer via scaly_mgpu_begin /
// scaly_mgpu_enc_* / scaly_mgpu_commit_wait. Per-op-sync mode is the
// same API with begin/commit around each op (the step-1 measurement).
//
// GEMM goes through MPS (kernel objects cached; MPS opens its own
// internal encoders, so the shim closes the current compute encoder
// first). Everything else is an MSL kernel below. GPU answers for the
// backwards the CPU TaskPlanner classifies "blocked" (shared writes):
//   - embedding scatter-add:   one thread PER COLUMN loops the tokens
//     sequentially — writes stay column-disjoint, deterministic, no
//     atomics, no race.
//   - layernorm dgamma/dbeta:  same reshape, one thread per column
//     reduces across rows (row stats come from a stats scratch the
//     forward kernel writes: mean, inv per row).
//   - add_bcast dy:            one thread per broadcast slot sums its
//     B repeats.
//   - attention backward:      the CPU code's three passes are three
//     dispatches; each is (row, head)- resp. (key, head)-disjoint, and
//     the serial compute encoder orders them.
//   - crossentropy/loss:       per-row losses to scratch, then a
//     single-thread reduce; the seed grad is a 1-element fill.
// GPU float reductions are NOT bit-identical to the CPU (order) —
// callers gate with tolerances, never exact equality.

typedef struct { uint32_t n; float v; } mgpu_fill_args;
typedef struct { uint32_t n; } mgpu_n_args;
typedef struct { uint32_t r, n, yr; } mgpu_bcast_args;
typedef struct { uint32_t r, n; float eps; } mgpu_ln_args;
typedef struct { uint32_t tokens, dim; } mgpu_embed_args;
typedef struct { uint32_t r, n; } mgpu_ce_args;
typedef struct { uint32_t t, d, hs, win; } mgpu_attn_args;
typedef struct { uint32_t total; float lr, b1, b2, eps, bc1, bc2; } mgpu_adam_args;
typedef struct { uint32_t m, k, n; } mgpu_mm_args;

static NSString *const mgpu_msl_ops_src = @""
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct FillArgs { uint n; float v; };\n"
"struct NArgs { uint n; };\n"
"struct BcastArgs { uint r; uint n; uint yr; };\n"
"struct LnArgs { uint r; uint n; float eps; };\n"
"struct EmbedArgs { uint tokens; uint dim; };\n"
"struct CeArgs { uint r; uint n; };\n"
"struct AttnArgs { uint t; uint d; uint hs; uint win; };\n"
"struct AdamArgs { uint total; float lr; float b1; float b2; float eps; float bc1; float bc2; };\n"
"struct MmArgs { uint m; uint k; uint n; };\n"
"\n"
"kernel void fill_f32(device float* buf [[buffer(0)]],\n"
"                     constant FillArgs& a [[buffer(1)]],\n"
"                     uint gid [[thread_position_in_grid]])\n"
"{ if (gid < a.n) buf[gid] = a.v; }\n"
"\n"
"kernel void acc_f32(device float* dst [[buffer(0)]],\n"
"                    device const float* src [[buffer(1)]],\n"
"                    constant NArgs& a [[buffer(2)]],\n"
"                    uint gid [[thread_position_in_grid]])\n"
"{ if (gid < a.n) dst[gid] += src[gid]; }\n"
"\n"
"kernel void add_f32(device const float* x [[buffer(0)]],\n"
"                    device const float* y [[buffer(1)]],\n"
"                    device float* out [[buffer(2)]],\n"
"                    constant NArgs& a [[buffer(3)]],\n"
"                    uint gid [[thread_position_in_grid]])\n"
"{ if (gid < a.n) out[gid] = x[gid] + y[gid]; }\n"
"\n"
"kernel void gelu_fwd(device const float* x [[buffer(0)]],\n"
"                     device float* out [[buffer(1)]],\n"
"                     constant NArgs& a [[buffer(2)]],\n"
"                     uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid >= a.n) return;\n"
"    float v = x[gid];\n"
"    float inner = 0.7978845608f * (v + 0.044715f * v * v * v);\n"
"    float t = precise::tanh(inner);\n"
"    out[gid] = 0.5f * v * (1.0f + t);\n"
"}\n"
"\n"
"kernel void gelu_bwd(device const float* x [[buffer(0)]],\n"
"                     device const float* dout [[buffer(1)]],\n"
"                     device float* dx [[buffer(2)]],\n"
"                     constant NArgs& a [[buffer(3)]],\n"
"                     uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid >= a.n) return;\n"
"    float v = x[gid];\n"
"    float inner = 0.7978845608f * (v + 0.044715f * v * v * v);\n"
"    float t = precise::tanh(inner);\n"
"    float dinner = 0.7978845608f * (1.0f + 0.134145f * v * v);\n"
"    float dg = 0.5f * (1.0f + t) + 0.5f * v * (1.0f - t * t) * dinner;\n"
"    dx[gid] += dout[gid] * dg;\n"
"}\n"
"\n"
"kernel void add_bcast_fwd(device const float* x [[buffer(0)]],\n"
"                          device const float* y [[buffer(1)]],\n"
"                          device float* out [[buffer(2)]],\n"
"                          constant BcastArgs& a [[buffer(3)]],\n"
"                          uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid >= a.r * a.n) return;\n"
"    uint i = gid / a.n, j = gid - i * a.n;\n"
"    out[gid] = x[gid] + y[(i % a.yr) * a.n + j];\n"
"}\n"
"\n"
"// dy accumulation: one thread per broadcast slot sums its repeats\n"
"// (the shared-write reshape; deterministic order over b).\n"
"kernel void add_bcast_bwd_dy(device const float* dout [[buffer(0)]],\n"
"                             device float* dy [[buffer(1)]],\n"
"                             constant BcastArgs& a [[buffer(2)]],\n"
"                             uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid >= a.yr * a.n) return;\n"
"    uint iy = gid / a.n, j = gid - iy * a.n;\n"
"    uint reps = a.r / a.yr;\n"
"    float s = 0.0f;\n"
"    for (uint b = 0; b < reps; ++b)\n"
"        s += dout[(b * a.yr + iy) * a.n + j];\n"
"    dy[gid] += s;\n"
"}\n"
"\n"
"// row thread: layernorm forward + (mean, inv) into stats[2*i] — the\n"
"// backward kernels read them instead of recomputing per column.\n"
"kernel void ln_fwd(device const float* x [[buffer(0)]],\n"
"                   device const float* gamma [[buffer(1)]],\n"
"                   device const float* beta [[buffer(2)]],\n"
"                   device float* out [[buffer(3)]],\n"
"                   device float* stats [[buffer(4)]],\n"
"                   constant LnArgs& a [[buffer(5)]],\n"
"                   uint i [[thread_position_in_grid]])\n"
"{\n"
"    if (i >= a.r) return;\n"
"    uint n = a.n;\n"
"    float sum = 0.0f;\n"
"    for (uint j = 0; j < n; ++j) sum += x[i * n + j];\n"
"    float mean = sum / (float)n;\n"
"    float vsum = 0.0f;\n"
"    for (uint j = 0; j < n; ++j) { float d = x[i * n + j] - mean; vsum += d * d; }\n"
"    float inv = 1.0f / precise::sqrt(vsum / (float)n + a.eps);\n"
"    for (uint j = 0; j < n; ++j)\n"
"        out[i * n + j] = (x[i * n + j] - mean) * inv * gamma[j] + beta[j];\n"
"    stats[2 * i] = mean; stats[2 * i + 1] = inv;\n"
"}\n"
"\n"
"kernel void ln_bwd_dx(device const float* x [[buffer(0)]],\n"
"                      device const float* gamma [[buffer(1)]],\n"
"                      device const float* dout [[buffer(2)]],\n"
"                      device float* dx [[buffer(3)]],\n"
"                      device const float* stats [[buffer(4)]],\n"
"                      constant LnArgs& a [[buffer(5)]],\n"
"                      uint i [[thread_position_in_grid]])\n"
"{\n"
"    if (i >= a.r) return;\n"
"    uint n = a.n;\n"
"    float mean = stats[2 * i], inv = stats[2 * i + 1];\n"
"    float m1 = 0.0f, m2 = 0.0f;\n"
"    for (uint j = 0; j < n; ++j) {\n"
"        float xhat = (x[i * n + j] - mean) * inv;\n"
"        float dyhat = dout[i * n + j] * gamma[j];\n"
"        m1 += dyhat; m2 += dyhat * xhat;\n"
"    }\n"
"    m1 /= (float)n; m2 /= (float)n;\n"
"    for (uint j = 0; j < n; ++j) {\n"
"        float xhat = (x[i * n + j] - mean) * inv;\n"
"        float dyhat = dout[i * n + j] * gamma[j];\n"
"        dx[i * n + j] += inv * (dyhat - m1 - xhat * m2);\n"
"    }\n"
"}\n"
"\n"
"// column thread: dgamma/dbeta reduce across rows (shared-write\n"
"// reshape: writes are column-disjoint, loop over rows sequential).\n"
"kernel void ln_bwd_dgb(device const float* x [[buffer(0)]],\n"
"                       device const float* dout [[buffer(1)]],\n"
"                       device float* dgamma [[buffer(2)]],\n"
"                       device float* dbeta [[buffer(3)]],\n"
"                       device const float* stats [[buffer(4)]],\n"
"                       constant LnArgs& a [[buffer(5)]],\n"
"                       uint j [[thread_position_in_grid]])\n"
"{\n"
"    if (j >= a.n) return;\n"
"    float dg = 0.0f, db = 0.0f;\n"
"    for (uint i = 0; i < a.r; ++i) {\n"
"        float xhat = (x[i * a.n + j] - stats[2 * i]) * stats[2 * i + 1];\n"
"        float d = dout[i * a.n + j];\n"
"        dg += d * xhat; db += d;\n"
"    }\n"
"    dgamma[j] += dg; dbeta[j] += db;\n"
"}\n"
"\n"
"kernel void embed_fwd(device const float* table [[buffer(0)]],\n"
"                      device const long* tok [[buffer(1)]],\n"
"                      device float* out [[buffer(2)]],\n"
"                      constant EmbedArgs& a [[buffer(3)]],\n"
"                      uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid >= a.tokens * a.dim) return;\n"
"    uint t = gid / a.dim, j = gid - t * a.dim;\n"
"    out[gid] = table[(uint)tok[t] * a.dim + j];\n"
"}\n"
"\n"
"// scatter-add reshaped column-disjoint: one thread per embedding\n"
"// column walks the tokens sequentially (duplicate ids collide only\n"
"// within the thread) — deterministic, no atomics.\n"
"kernel void embed_bwd(device const float* dout [[buffer(0)]],\n"
"                      device const long* tok [[buffer(1)]],\n"
"                      device float* dtable [[buffer(2)]],\n"
"                      constant EmbedArgs& a [[buffer(3)]],\n"
"                      uint j [[thread_position_in_grid]])\n"
"{\n"
"    if (j >= a.dim) return;\n"
"    for (uint t = 0; t < a.tokens; ++t)\n"
"        dtable[(uint)tok[t] * a.dim + j] += dout[t * a.dim + j];\n"
"}\n"
"\n"
"kernel void ce_fwd_rows(device const float* x [[buffer(0)]],\n"
"                        device const long* tok [[buffer(1)]],\n"
"                        device float* rowloss [[buffer(2)]],\n"
"                        constant CeArgs& a [[buffer(3)]],\n"
"                        uint i [[thread_position_in_grid]])\n"
"{\n"
"    if (i >= a.r) return;\n"
"    uint n = a.n;\n"
"    float mx = x[i * n];\n"
"    for (uint j = 1; j < n; ++j) mx = max(mx, x[i * n + j]);\n"
"    float sum = 0.0f;\n"
"    for (uint j = 0; j < n; ++j) sum += precise::exp(x[i * n + j] - mx);\n"
"    rowloss[i] = mx + precise::log(sum) - x[i * n + (uint)tok[i]];\n"
"}\n"
"\n"
"kernel void ce_reduce(device const float* rowloss [[buffer(0)]],\n"
"                      device float* out [[buffer(1)]],\n"
"                      constant CeArgs& a [[buffer(2)]],\n"
"                      uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid != 0) return;\n"
"    float s = 0.0f;\n"
"    for (uint i = 0; i < a.r; ++i) s += rowloss[i];\n"
"    out[0] = s / (float)a.r;\n"
"}\n"
"\n"
"kernel void ce_bwd(device const float* x [[buffer(0)]],\n"
"                   device const long* tok [[buffer(1)]],\n"
"                   device const float* dout [[buffer(2)]],\n"
"                   device float* dx [[buffer(3)]],\n"
"                   constant CeArgs& a [[buffer(4)]],\n"
"                   uint i [[thread_position_in_grid]])\n"
"{\n"
"    if (i >= a.r) return;\n"
"    uint n = a.n;\n"
"    float scale = dout[0] / (float)a.r;\n"
"    float mx = x[i * n];\n"
"    for (uint j = 1; j < n; ++j) mx = max(mx, x[i * n + j]);\n"
"    float sum = 0.0f;\n"
"    for (uint j = 0; j < n; ++j) sum += precise::exp(x[i * n + j] - mx);\n"
"    float inv = 1.0f / sum;\n"
"    uint t = (uint)tok[i];\n"
"    for (uint j = 0; j < n; ++j) {\n"
"        float p = precise::exp(x[i * n + j] - mx) * inv;\n"
"        if (j == t) p -= 1.0f;\n"
"        dx[i * n + j] += scale * p;\n"
"    }\n"
"}\n"
"\n"
"// (query row, head) thread: windowed causal attention forward; the\n"
"// probabilities land in aux exactly like the CPU op (stride win).\n"
"kernel void attn_fwd(device const float* q [[buffer(0)]],\n"
"                     device const float* k [[buffer(1)]],\n"
"                     device const float* v [[buffer(2)]],\n"
"                     device float* out [[buffer(3)]],\n"
"                     device float* aux [[buffer(4)]],\n"
"                     constant AttnArgs& a [[buffer(5)]],\n"
"                     uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint h = gid.x, i = gid.y;\n"
"    if (h >= a.hs || i >= a.t) return;\n"
"    uint d = a.d, w = a.win, dh = d / a.hs;\n"
"    uint ws = (i / w) * w;\n"
"    float scale = 1.0f / precise::sqrt((float)dh);\n"
"    uint base = h * a.t * w + i * w;\n"
"    float mx = 0.0f;\n"
"    for (uint j = ws; j <= i; ++j) {\n"
"        float acc = 0.0f;\n"
"        for (uint c = 0; c < dh; ++c)\n"
"            acc += q[i * d + h * dh + c] * k[j * d + h * dh + c];\n"
"        float sc = acc * scale;\n"
"        aux[base + (j - ws)] = sc;\n"
"        if (j == ws || sc > mx) mx = sc;\n"
"    }\n"
"    float sum = 0.0f;\n"
"    for (uint j = ws; j <= i; ++j) {\n"
"        float e = precise::exp(aux[base + (j - ws)] - mx);\n"
"        aux[base + (j - ws)] = e; sum += e;\n"
"    }\n"
"    float inv = 1.0f / sum;\n"
"    for (uint j = ws; j <= i; ++j) aux[base + (j - ws)] *= inv;\n"
"    for (uint c = 0; c < dh; ++c) {\n"
"        float acc = 0.0f;\n"
"        for (uint j = ws; j <= i; ++j)\n"
"            acc += aux[base + (j - ws)] * v[j * d + h * dh + c];\n"
"        out[i * d + h * dh + c] = acc;\n"
"    }\n"
"}\n"
"\n"
"// backward pass 1: ds into the aux scratch half (rows disjoint)\n"
"kernel void attn_bwd1(device const float* v [[buffer(0)]],\n"
"                      device const float* dout [[buffer(1)]],\n"
"                      device float* aux [[buffer(2)]],\n"
"                      constant AttnArgs& a [[buffer(3)]],\n"
"                      uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint h = gid.x, i = gid.y;\n"
"    if (h >= a.hs || i >= a.t) return;\n"
"    uint d = a.d, w = a.win, dh = d / a.hs;\n"
"    uint ws = (i / w) * w;\n"
"    uint base = h * a.t * w + i * w, dsoff = a.hs * a.t * w;\n"
"    float ci = 0.0f;\n"
"    for (uint j = ws; j <= i; ++j) {\n"
"        float dp = 0.0f;\n"
"        for (uint c = 0; c < dh; ++c)\n"
"            dp += dout[i * d + h * dh + c] * v[j * d + h * dh + c];\n"
"        aux[dsoff + base + (j - ws)] = dp;\n"
"        ci += dp * aux[base + (j - ws)];\n"
"    }\n"
"    for (uint j = ws; j <= i; ++j) {\n"
"        float ds = aux[base + (j - ws)] * (aux[dsoff + base + (j - ws)] - ci);\n"
"        aux[dsoff + base + (j - ws)] = ds;\n"
"    }\n"
"}\n"
"\n"
"// backward pass 2: dQ (query rows disjoint)\n"
"kernel void attn_bwd2(device const float* k [[buffer(0)]],\n"
"                      device const float* aux [[buffer(1)]],\n"
"                      device float* dq [[buffer(2)]],\n"
"                      constant AttnArgs& a [[buffer(3)]],\n"
"                      uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint h = gid.x, i = gid.y;\n"
"    if (h >= a.hs || i >= a.t) return;\n"
"    uint d = a.d, w = a.win, dh = d / a.hs;\n"
"    uint ws = (i / w) * w;\n"
"    uint dsoff = a.hs * a.t * w, base = h * a.t * w + i * w;\n"
"    float scale = 1.0f / precise::sqrt((float)dh);\n"
"    for (uint c = 0; c < dh; ++c) {\n"
"        float acc = 0.0f;\n"
"        for (uint j = ws; j <= i; ++j)\n"
"            acc += aux[dsoff + base + (j - ws)] * k[j * d + h * dh + c];\n"
"        dq[i * d + h * dh + c] += scale * acc;\n"
"    }\n"
"}\n"
"\n"
"// backward pass 3: dK and dV (key rows disjoint)\n"
"kernel void attn_bwd3(device const float* q [[buffer(0)]],\n"
"                      device const float* dout [[buffer(1)]],\n"
"                      device const float* aux [[buffer(2)]],\n"
"                      device float* dk [[buffer(3)]],\n"
"                      device float* dv [[buffer(4)]],\n"
"                      constant AttnArgs& a [[buffer(5)]],\n"
"                      uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint h = gid.x, j = gid.y;\n"
"    if (h >= a.hs || j >= a.t) return;\n"
"    uint d = a.d, w = a.win, dh = d / a.hs;\n"
"    uint wsj = (j / w) * w, wej = wsj + w, jw = j - wsj;\n"
"    uint dsoff = a.hs * a.t * w;\n"
"    float scale = 1.0f / precise::sqrt((float)dh);\n"
"    for (uint c = 0; c < dh; ++c) {\n"
"        float acck = 0.0f, accv = 0.0f;\n"
"        for (uint i = j; i < wej; ++i) {\n"
"            acck += aux[dsoff + h * a.t * w + i * w + jw] * q[i * d + h * dh + c];\n"
"            accv += aux[h * a.t * w + i * w + jw] * dout[i * d + h * dh + c];\n"
"        }\n"
"        dk[j * d + h * dh + c] += scale * acck;\n"
"        dv[j * d + h * dh + c] += accv;\n"
"    }\n"
"}\n"
"\n"
"// Adam with CPU-precomputed bias corrections; mv holds m at [0,total)\n"
"// and v at [total,2*total) like the CPU op's aux layout.\n"
"kernel void adam_step(device float* p [[buffer(0)]],\n"
"                      device const float* g [[buffer(1)]],\n"
"                      device float* mv [[buffer(2)]],\n"
"                      constant AdamArgs& a [[buffer(3)]],\n"
"                      uint gid [[thread_position_in_grid]])\n"
"{\n"
"    if (gid >= a.total) return;\n"
"    float gr = g[gid];\n"
"    float m = a.b1 * mv[gid] + (1.0f - a.b1) * gr;\n"
"    float v = a.b2 * mv[a.total + gid] + (1.0f - a.b2) * gr * gr;\n"
"    mv[gid] = m; mv[a.total + gid] = v;\n"
"    p[gid] -= a.lr * (m / a.bc1) / (precise::sqrt(v / a.bc2) + a.eps);\n"
"}\n"
"\n"
"// MSL matmul family for the single-encoder variant (same 4x4 register\n"
"// tile as the spike kernel; each requires its tiled dims % 4 == 0):\n"
"// mm_nt_acc: dA += dC * B^T   (dC m x n, B k x n, dA m x k)\n"
"// mm_tn_acc: dB += A^T * dC   (A m x k, dC m x n, dB k x n)\n"
"kernel void mm_nt_acc(device const float* dc [[buffer(0)]],\n"
"                      device const float* b [[buffer(1)]],\n"
"                      device float* da [[buffer(2)]],\n"
"                      constant MmArgs& dims [[buffer(3)]],\n"
"                      uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint m = dims.m, K = dims.k, n = dims.n;\n"
"    uint i0 = gid.y * 4, x0 = gid.x * 4;\n"
"    if (i0 >= m || x0 >= K) return;\n"
"    float acc[4][4] = {{0.0f}};\n"
"    for (uint j = 0; j < n; ++j) {\n"
"        float d0 = dc[(i0+0)*n + j], d1 = dc[(i0+1)*n + j];\n"
"        float d2 = dc[(i0+2)*n + j], d3 = dc[(i0+3)*n + j];\n"
"        float k0 = b[(x0+0)*n + j], k1 = b[(x0+1)*n + j];\n"
"        float k2 = b[(x0+2)*n + j], k3 = b[(x0+3)*n + j];\n"
"        acc[0][0]+=d0*k0; acc[0][1]+=d0*k1; acc[0][2]+=d0*k2; acc[0][3]+=d0*k3;\n"
"        acc[1][0]+=d1*k0; acc[1][1]+=d1*k1; acc[1][2]+=d1*k2; acc[1][3]+=d1*k3;\n"
"        acc[2][0]+=d2*k0; acc[2][1]+=d2*k1; acc[2][2]+=d2*k2; acc[2][3]+=d2*k3;\n"
"        acc[3][0]+=d3*k0; acc[3][1]+=d3*k1; acc[3][2]+=d3*k2; acc[3][3]+=d3*k3;\n"
"    }\n"
"    for (uint r = 0; r < 4; ++r)\n"
"        for (uint c = 0; c < 4; ++c)\n"
"            da[(i0+r)*K + x0+c] += acc[r][c];\n"
"}\n"
"\n"
"kernel void mm_tn_acc(device const float* a [[buffer(0)]],\n"
"                      device const float* dc [[buffer(1)]],\n"
"                      device float* db [[buffer(2)]],\n"
"                      constant MmArgs& dims [[buffer(3)]],\n"
"                      uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint m = dims.m, K = dims.k, n = dims.n;\n"
"    uint x0 = gid.y * 4, j0 = gid.x * 4;\n"
"    if (x0 >= K || j0 >= n) return;\n"
"    float acc[4][4] = {{0.0f}};\n"
"    for (uint i = 0; i < m; ++i) {\n"
"        float r0 = a[i*K + x0+0], r1 = a[i*K + x0+1];\n"
"        float r2 = a[i*K + x0+2], r3 = a[i*K + x0+3];\n"
"        float d0 = dc[i*n + j0+0], d1 = dc[i*n + j0+1];\n"
"        float d2 = dc[i*n + j0+2], d3 = dc[i*n + j0+3];\n"
"        acc[0][0]+=r0*d0; acc[0][1]+=r0*d1; acc[0][2]+=r0*d2; acc[0][3]+=r0*d3;\n"
"        acc[1][0]+=r1*d0; acc[1][1]+=r1*d1; acc[1][2]+=r1*d2; acc[1][3]+=r1*d3;\n"
"        acc[2][0]+=r2*d0; acc[2][1]+=r2*d1; acc[2][2]+=r2*d2; acc[2][3]+=r2*d3;\n"
"        acc[3][0]+=r3*d0; acc[3][1]+=r3*d1; acc[3][2]+=r3*d2; acc[3][3]+=r3*d3;\n"
"    }\n"
"    for (uint r = 0; r < 4; ++r)\n"
"        for (uint c = 0; c < 4; ++c)\n"
"            db[(x0+r)*n + j0+c] += acc[r][c];\n"
"}\n"
"\n"
"kernel void mm_nn(device const float* A [[buffer(0)]],\n"
"                  device const float* B [[buffer(1)]],\n"
"                  device float* C [[buffer(2)]],\n"
"                  constant MmArgs& dims [[buffer(3)]],\n"
"                  uint2 gid [[thread_position_in_grid]])\n"
"{\n"
"    uint m = dims.m, K = dims.k, n = dims.n;\n"
"    uint i0 = gid.y * 4, j0 = gid.x * 4;\n"
"    if (i0 >= m || j0 >= n) return;\n"
"    float acc[4][4] = {{0.0f}};\n"
"    for (uint x = 0; x < K; ++x) {\n"
"        float a0 = A[(i0+0)*K + x], a1 = A[(i0+1)*K + x];\n"
"        float a2 = A[(i0+2)*K + x], a3 = A[(i0+3)*K + x];\n"
"        float b0 = B[x*n + j0+0], b1 = B[x*n + j0+1];\n"
"        float b2 = B[x*n + j0+2], b3 = B[x*n + j0+3];\n"
"        acc[0][0]+=a0*b0; acc[0][1]+=a0*b1; acc[0][2]+=a0*b2; acc[0][3]+=a0*b3;\n"
"        acc[1][0]+=a1*b0; acc[1][1]+=a1*b1; acc[1][2]+=a1*b2; acc[1][3]+=a1*b3;\n"
"        acc[2][0]+=a2*b0; acc[2][1]+=a2*b1; acc[2][2]+=a2*b2; acc[2][3]+=a2*b3;\n"
"        acc[3][0]+=a3*b0; acc[3][1]+=a3*b1; acc[3][2]+=a3*b2; acc[3][3]+=a3*b3;\n"
"    }\n"
"    for (uint r = 0; r < 4; ++r)\n"
"        for (uint c = 0; c < 4; ++c)\n"
"            C[(i0+r)*n + j0+c] = acc[r][c];\n"
"}\n";

static id<MTLLibrary> mgpu_ops_lib;
static id<MTLComputePipelineState> mgpu_pso_fill, mgpu_pso_acc, mgpu_pso_add,
    mgpu_pso_gelu_fwd, mgpu_pso_gelu_bwd, mgpu_pso_bcast_fwd, mgpu_pso_bcast_dy,
    mgpu_pso_ln_fwd, mgpu_pso_ln_dx, mgpu_pso_ln_dgb,
    mgpu_pso_embed_fwd, mgpu_pso_embed_bwd,
    mgpu_pso_ce_rows, mgpu_pso_ce_red, mgpu_pso_ce_bwd,
    mgpu_pso_attn_fwd, mgpu_pso_attn_b1, mgpu_pso_attn_b2, mgpu_pso_attn_b3,
    mgpu_pso_adam, mgpu_pso_mm_nn, mgpu_pso_mm_nt, mgpu_pso_mm_tn;

static id<MTLCommandBuffer> mgpu_cmd;
static id<MTLComputeCommandEncoder> mgpu_enc;
static NSMutableDictionary *mgpu_gemm_cache;
static double mgpu_gpu_ms_sum;

static id<MTLComputePipelineState> mgpu_pso(id<MTLLibrary> lib, NSString *name)
{
    NSError *err = nil;
    id<MTLFunction> fn = [lib newFunctionWithName:name];
    id<MTLComputePipelineState> p = [mgpu_dev newComputePipelineStateWithFunction:fn error:&err];
    if (p == nil)
        fprintf(stderr, "mgpu: pipeline %s failed: %s\n", name.UTF8String,
                err.localizedDescription.UTF8String);
    return p;
}

// Compile the op kernels; call once after scaly_mgpu_init. 0 = success.
int scaly_mgpu_ops_init(void)
{
    if (mgpu_ops_lib != nil)
        return 0;
    if (mgpu_dev == nil)
        return 1;
    NSError *err = nil;
    mgpu_ops_lib = [mgpu_dev newLibraryWithSource:mgpu_msl_ops_src options:nil error:&err];
    if (mgpu_ops_lib == nil) {
        fprintf(stderr, "mgpu: ops MSL compile failed: %s\n", err.localizedDescription.UTF8String);
        return 2;
    }
    mgpu_pso_fill = mgpu_pso(mgpu_ops_lib, @"fill_f32");
    mgpu_pso_acc = mgpu_pso(mgpu_ops_lib, @"acc_f32");
    mgpu_pso_add = mgpu_pso(mgpu_ops_lib, @"add_f32");
    mgpu_pso_gelu_fwd = mgpu_pso(mgpu_ops_lib, @"gelu_fwd");
    mgpu_pso_gelu_bwd = mgpu_pso(mgpu_ops_lib, @"gelu_bwd");
    mgpu_pso_bcast_fwd = mgpu_pso(mgpu_ops_lib, @"add_bcast_fwd");
    mgpu_pso_bcast_dy = mgpu_pso(mgpu_ops_lib, @"add_bcast_bwd_dy");
    mgpu_pso_ln_fwd = mgpu_pso(mgpu_ops_lib, @"ln_fwd");
    mgpu_pso_ln_dx = mgpu_pso(mgpu_ops_lib, @"ln_bwd_dx");
    mgpu_pso_ln_dgb = mgpu_pso(mgpu_ops_lib, @"ln_bwd_dgb");
    mgpu_pso_embed_fwd = mgpu_pso(mgpu_ops_lib, @"embed_fwd");
    mgpu_pso_embed_bwd = mgpu_pso(mgpu_ops_lib, @"embed_bwd");
    mgpu_pso_ce_rows = mgpu_pso(mgpu_ops_lib, @"ce_fwd_rows");
    mgpu_pso_ce_red = mgpu_pso(mgpu_ops_lib, @"ce_reduce");
    mgpu_pso_ce_bwd = mgpu_pso(mgpu_ops_lib, @"ce_bwd");
    mgpu_pso_attn_fwd = mgpu_pso(mgpu_ops_lib, @"attn_fwd");
    mgpu_pso_attn_b1 = mgpu_pso(mgpu_ops_lib, @"attn_bwd1");
    mgpu_pso_attn_b2 = mgpu_pso(mgpu_ops_lib, @"attn_bwd2");
    mgpu_pso_attn_b3 = mgpu_pso(mgpu_ops_lib, @"attn_bwd3");
    mgpu_pso_adam = mgpu_pso(mgpu_ops_lib, @"adam_step");
    mgpu_pso_mm_nn = mgpu_pso(mgpu_ops_lib, @"mm_nn");
    mgpu_pso_mm_nt = mgpu_pso(mgpu_ops_lib, @"mm_nt_acc");
    mgpu_pso_mm_tn = mgpu_pso(mgpu_ops_lib, @"mm_tn_acc");
    if (mgpu_pso_fill == nil || mgpu_pso_adam == nil || mgpu_pso_attn_b3 == nil)
        return 3;
    mgpu_gemm_cache = [NSMutableDictionary new];
    return 0;
}

int scaly_mgpu_begin(void)
{
    if (mgpu_queue == nil)
        return 1;
    @autoreleasepool {
        mgpu_cmd = [mgpu_queue commandBuffer];
    }
    mgpu_enc = nil;
    return 0;
}

static id<MTLComputeCommandEncoder> mgpu_ensure_enc(void)
{
    if (mgpu_enc == nil) {
        @autoreleasepool {
            mgpu_enc = [mgpu_cmd computeCommandEncoder];
        }
    }
    return mgpu_enc;
}

int scaly_mgpu_commit_wait(void)
{
    @autoreleasepool {
    if (mgpu_cmd == nil)
        return 1;
    if (mgpu_enc != nil) {
        [mgpu_enc endEncoding];
        mgpu_enc = nil;
    }
    [mgpu_cmd commit];
    [mgpu_cmd waitUntilCompleted];
    mgpu_last_gpu_ms = (mgpu_cmd.GPUEndTime - mgpu_cmd.GPUStartTime) * 1000.0;
    mgpu_gpu_ms_sum += mgpu_last_gpu_ms;
    int ok = mgpu_cmd.status == MTLCommandBufferStatusCompleted ? 0 : 4;
    mgpu_cmd = nil;
    return ok;
    }
}

void scaly_mgpu_ms_reset(void) { mgpu_gpu_ms_sum = 0.0; }
double scaly_mgpu_ms_sum(void) { return mgpu_gpu_ms_sum; }

static void mgpu_dispatch1(id<MTLComputePipelineState> pso, NSUInteger n)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:pso];
    NSUInteger tg = MIN((NSUInteger)256, pso.maxTotalThreadsPerThreadgroup);
    if (tg > n && n > 0) tg = n;
    [e dispatchThreads:MTLSizeMake(n, 1, 1) threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
}

static void mgpu_dispatch2(id<MTLComputePipelineState> pso, NSUInteger w, NSUInteger h)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:pso];
    NSUInteger tgx = MIN(w, (NSUInteger)8);
    NSUInteger tgy = MIN(h, (NSUInteger)32);
    [e dispatchThreads:MTLSizeMake(w, h, 1) threadsPerThreadgroup:MTLSizeMake(tgx, tgy, 1)];
}

#define MGPU_BUF(x) ((__bridge id<MTLBuffer>)(x))

// Buffer-slot registry: Scaly has no natural pointer-array container,
// so the driver addresses every MTLBuffer by an int slot (the demo
// uses node_id*4 + {0 value, 1 grad, 2 aux, 3 tok}, scratch buffers
// appended after). All enc_* functions below take slots.
static void **mgpu_slots;
static int mgpu_nslots;

static id<MTLBuffer> SB(int slot)
{
    return (__bridge id<MTLBuffer>)mgpu_slots[slot];
}

int scaly_mgpu_slots_init(int n)
{
    if (mgpu_slots != NULL) {
        for (int i = 0; i < mgpu_nslots; ++i)
            if (mgpu_slots[i] != NULL)
                scaly_mgpu_release(mgpu_slots[i]);
        free(mgpu_slots);
    }
    mgpu_slots = calloc((size_t)n, sizeof(void *));
    mgpu_nslots = n;
    return mgpu_slots == NULL ? 1 : 0;
}

// Allocate a shared buffer into a slot, zero-filled.
int scaly_mgpu_slot_alloc(int slot, size_t bytes)
{
    void *b = scaly_mgpu_alloc(bytes);
    if (b == NULL)
        return 1;
    memset(scaly_mgpu_contents(b), 0, bytes);
    mgpu_slots[slot] = b;
    return 0;
}

void *scaly_mgpu_slot_contents(int slot)
{
    return scaly_mgpu_contents(mgpu_slots[slot]);
}

void scaly_mgpu_slot_write(int slot, const void *src, size_t bytes)
{
    memcpy(scaly_mgpu_contents(mgpu_slots[slot]), src, bytes);
}

// GEMM via MPS: C = alpha * op(A) * op(B) + beta * C. Stored shapes:
// op(A) is ra x ca after the optional transpose, likewise op(B); the
// caller passes the STORED row/col counts. MPS opens its own encoder,
// so close ours first. Kernel objects are cached by shape/flags (the
// graph is static — a training run reuses a handful).
void scaly_mgpu_enc_gemm(int a, int b, int c,
                         int arows, int acols, int brows, int bcols,
                         int ta, int tb, float alpha, float beta)
{
    @autoreleasepool {
    if (mgpu_enc != nil) {
        [mgpu_enc endEncoding];
        mgpu_enc = nil;
    }
    int m = ta ? acols : arows;
    int k = ta ? arows : acols;
    int n = tb ? brows : bcols;
    NSString *key = [NSString stringWithFormat:@"%d_%d_%d_%d_%d_%g_%g",
                     m, k, n, ta, tb, alpha, beta];
    MPSMatrixMultiplication *mm = mgpu_gemm_cache[key];
    if (mm == nil) {
        mm = [[MPSMatrixMultiplication alloc] initWithDevice:mgpu_dev
              transposeLeft:(ta != 0) transposeRight:(tb != 0)
              resultRows:m resultColumns:n interiorColumns:k
              alpha:alpha beta:beta];
        mgpu_gemm_cache[key] = mm;
    }
    MPSMatrixDescriptor *da = [MPSMatrixDescriptor matrixDescriptorWithRows:arows columns:acols
                               rowBytes:(NSUInteger)acols * 4 dataType:MPSDataTypeFloat32];
    MPSMatrixDescriptor *db = [MPSMatrixDescriptor matrixDescriptorWithRows:brows columns:bcols
                               rowBytes:(NSUInteger)bcols * 4 dataType:MPSDataTypeFloat32];
    MPSMatrixDescriptor *dc = [MPSMatrixDescriptor matrixDescriptorWithRows:m columns:n
                               rowBytes:(NSUInteger)n * 4 dataType:MPSDataTypeFloat32];
    MPSMatrix *ma = [[MPSMatrix alloc] initWithBuffer:SB(a) descriptor:da];
    MPSMatrix *mb = [[MPSMatrix alloc] initWithBuffer:SB(b) descriptor:db];
    MPSMatrix *mc = [[MPSMatrix alloc] initWithBuffer:SB(c) descriptor:dc];
    [mm encodeToCommandBuffer:mgpu_cmd leftMatrix:ma rightMatrix:mb resultMatrix:mc];
    }
}

// MSL matmul trio (single-encoder path); tiled dims must be % 4.
void scaly_mgpu_enc_mm_nn(int a, int b, int c, int m, int k, int n)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_mm_args args = { (uint32_t)m, (uint32_t)k, (uint32_t)n };
    [e setComputePipelineState:mgpu_pso_mm_nn];
    [e setBuffer:SB(a) offset:0 atIndex:0];
    [e setBuffer:SB(b) offset:0 atIndex:1];
    [e setBuffer:SB(c) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:MTLSizeMake((NSUInteger)(n / 4), (NSUInteger)(m / 4), 1)
        threadsPerThreadgroup:MTLSizeMake(8, 8, 1)];
}

void scaly_mgpu_enc_mm_nt_acc(int dc, int b, int da, int m, int k, int n)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_mm_args args = { (uint32_t)m, (uint32_t)k, (uint32_t)n };
    [e setComputePipelineState:mgpu_pso_mm_nt];
    [e setBuffer:SB(dc) offset:0 atIndex:0];
    [e setBuffer:SB(b) offset:0 atIndex:1];
    [e setBuffer:SB(da) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:MTLSizeMake((NSUInteger)(k / 4), (NSUInteger)(m / 4), 1)
        threadsPerThreadgroup:MTLSizeMake(8, 8, 1)];
}

void scaly_mgpu_enc_mm_tn_acc(int a, int dc, int db, int m, int k, int n)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_mm_args args = { (uint32_t)m, (uint32_t)k, (uint32_t)n };
    [e setComputePipelineState:mgpu_pso_mm_tn];
    [e setBuffer:SB(a) offset:0 atIndex:0];
    [e setBuffer:SB(dc) offset:0 atIndex:1];
    [e setBuffer:SB(db) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:MTLSizeMake((NSUInteger)(n / 4), (NSUInteger)(k / 4), 1)
        threadsPerThreadgroup:MTLSizeMake(8, 8, 1)];
}

void scaly_mgpu_enc_fill(int buf, int count, float v)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_fill_args args = { (uint32_t)count, v };
    [e setComputePipelineState:mgpu_pso_fill];
    [e setBuffer:SB(buf) offset:0 atIndex:0];
    [e setBytes:&args length:sizeof(args) atIndex:1];
    NSUInteger tg = MIN((NSUInteger)256, (NSUInteger)count);
    [e dispatchThreads:MTLSizeMake((NSUInteger)count, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
}

void scaly_mgpu_enc_acc(int dst, int src, int count)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_n_args args = { (uint32_t)count };
    [e setComputePipelineState:mgpu_pso_acc];
    [e setBuffer:SB(dst) offset:0 atIndex:0];
    [e setBuffer:SB(src) offset:0 atIndex:1];
    [e setBytes:&args length:sizeof(args) atIndex:2];
    NSUInteger tg = MIN((NSUInteger)256, (NSUInteger)count);
    [e dispatchThreads:MTLSizeMake((NSUInteger)count, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
}

void scaly_mgpu_enc_add(int x, int y, int out, int count)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_n_args args = { (uint32_t)count };
    [e setComputePipelineState:mgpu_pso_add];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(y) offset:0 atIndex:1];
    [e setBuffer:SB(out) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    NSUInteger tg = MIN((NSUInteger)256, (NSUInteger)count);
    [e dispatchThreads:MTLSizeMake((NSUInteger)count, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
}

void scaly_mgpu_enc_gelu_fwd(int x, int out, int count)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_n_args args = { (uint32_t)count };
    [e setComputePipelineState:mgpu_pso_gelu_fwd];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(out) offset:0 atIndex:1];
    [e setBytes:&args length:sizeof(args) atIndex:2];
    NSUInteger tg = MIN((NSUInteger)256, (NSUInteger)count);
    [e dispatchThreads:MTLSizeMake((NSUInteger)count, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
}

void scaly_mgpu_enc_gelu_bwd(int x, int dout, int dx, int count)
{
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    mgpu_n_args args = { (uint32_t)count };
    [e setComputePipelineState:mgpu_pso_gelu_bwd];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(dout) offset:0 atIndex:1];
    [e setBuffer:SB(dx) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    NSUInteger tg = MIN((NSUInteger)256, (NSUInteger)count);
    [e dispatchThreads:MTLSizeMake((NSUInteger)count, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
}

void scaly_mgpu_enc_add_bcast_fwd(int x, int y, int out, int r, int n, int yr)
{
    mgpu_bcast_args args = { (uint32_t)r, (uint32_t)n, (uint32_t)yr };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_bcast_fwd];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(y) offset:0 atIndex:1];
    [e setBuffer:SB(out) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    NSUInteger total = (NSUInteger)r * (NSUInteger)n;
    [e dispatchThreads:MTLSizeMake(total, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, total), 1, 1)];
}

void scaly_mgpu_enc_add_bcast_bwd_dy(int dout, int dy, int r, int n, int yr)
{
    mgpu_bcast_args args = { (uint32_t)r, (uint32_t)n, (uint32_t)yr };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_bcast_dy];
    [e setBuffer:SB(dout) offset:0 atIndex:0];
    [e setBuffer:SB(dy) offset:0 atIndex:1];
    [e setBytes:&args length:sizeof(args) atIndex:2];
    NSUInteger total = (NSUInteger)yr * (NSUInteger)n;
    [e dispatchThreads:MTLSizeMake(total, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, total), 1, 1)];
}

void scaly_mgpu_enc_ln_fwd(int x, int gamma, int beta, int out, int stats,
                           int r, int n, float eps)
{
    mgpu_ln_args args = { (uint32_t)r, (uint32_t)n, eps };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_ln_fwd];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(gamma) offset:0 atIndex:1];
    [e setBuffer:SB(beta) offset:0 atIndex:2];
    [e setBuffer:SB(out) offset:0 atIndex:3];
    [e setBuffer:SB(stats) offset:0 atIndex:4];
    [e setBytes:&args length:sizeof(args) atIndex:5];
    [e dispatchThreads:MTLSizeMake((NSUInteger)r, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)r), 1, 1)];
}

void scaly_mgpu_enc_ln_bwd_dx(int x, int gamma, int dout, int dx, int stats,
                              int r, int n)
{
    mgpu_ln_args args = { (uint32_t)r, (uint32_t)n, 0.0f };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_ln_dx];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(gamma) offset:0 atIndex:1];
    [e setBuffer:SB(dout) offset:0 atIndex:2];
    [e setBuffer:SB(dx) offset:0 atIndex:3];
    [e setBuffer:SB(stats) offset:0 atIndex:4];
    [e setBytes:&args length:sizeof(args) atIndex:5];
    [e dispatchThreads:MTLSizeMake((NSUInteger)r, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)r), 1, 1)];
}

void scaly_mgpu_enc_ln_bwd_dgb(int x, int dout, int dgamma, int dbeta, int stats,
                               int r, int n)
{
    mgpu_ln_args args = { (uint32_t)r, (uint32_t)n, 0.0f };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_ln_dgb];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(dout) offset:0 atIndex:1];
    [e setBuffer:SB(dgamma) offset:0 atIndex:2];
    [e setBuffer:SB(dbeta) offset:0 atIndex:3];
    [e setBuffer:SB(stats) offset:0 atIndex:4];
    [e setBytes:&args length:sizeof(args) atIndex:5];
    [e dispatchThreads:MTLSizeMake((NSUInteger)n, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)n), 1, 1)];
}

void scaly_mgpu_enc_embed_fwd(int table, int tok, int out, int tokens, int dim)
{
    mgpu_embed_args args = { (uint32_t)tokens, (uint32_t)dim };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_embed_fwd];
    [e setBuffer:SB(table) offset:0 atIndex:0];
    [e setBuffer:SB(tok) offset:0 atIndex:1];
    [e setBuffer:SB(out) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    NSUInteger total = (NSUInteger)tokens * (NSUInteger)dim;
    [e dispatchThreads:MTLSizeMake(total, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, total), 1, 1)];
}

void scaly_mgpu_enc_embed_bwd(int dout, int tok, int dtable, int tokens, int dim)
{
    mgpu_embed_args args = { (uint32_t)tokens, (uint32_t)dim };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_embed_bwd];
    [e setBuffer:SB(dout) offset:0 atIndex:0];
    [e setBuffer:SB(tok) offset:0 atIndex:1];
    [e setBuffer:SB(dtable) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:MTLSizeMake((NSUInteger)dim, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)dim), 1, 1)];
}

// Encodes both the per-row losses and the single-thread mean reduce.
void scaly_mgpu_enc_ce_fwd(int x, int tok, int rowloss, int out, int r, int n)
{
    mgpu_ce_args args = { (uint32_t)r, (uint32_t)n };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_ce_rows];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(tok) offset:0 atIndex:1];
    [e setBuffer:SB(rowloss) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:MTLSizeMake((NSUInteger)r, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)r), 1, 1)];
    [e setComputePipelineState:mgpu_pso_ce_red];
    [e setBuffer:SB(rowloss) offset:0 atIndex:0];
    [e setBuffer:SB(out) offset:0 atIndex:1];
    [e setBytes:&args length:sizeof(args) atIndex:2];
    [e dispatchThreads:MTLSizeMake(1, 1, 1) threadsPerThreadgroup:MTLSizeMake(1, 1, 1)];
}

void scaly_mgpu_enc_ce_bwd(int x, int tok, int dout, int dx, int r, int n)
{
    mgpu_ce_args args = { (uint32_t)r, (uint32_t)n };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_ce_bwd];
    [e setBuffer:SB(x) offset:0 atIndex:0];
    [e setBuffer:SB(tok) offset:0 atIndex:1];
    [e setBuffer:SB(dout) offset:0 atIndex:2];
    [e setBuffer:SB(dx) offset:0 atIndex:3];
    [e setBytes:&args length:sizeof(args) atIndex:4];
    [e dispatchThreads:MTLSizeMake((NSUInteger)r, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)r), 1, 1)];
}

void scaly_mgpu_enc_attn_fwd(int q, int k, int v, int out, int aux,
                             int t, int d, int hs, int win)
{
    mgpu_attn_args args = { (uint32_t)t, (uint32_t)d, (uint32_t)hs, (uint32_t)win };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_attn_fwd];
    [e setBuffer:SB(q) offset:0 atIndex:0];
    [e setBuffer:SB(k) offset:0 atIndex:1];
    [e setBuffer:SB(v) offset:0 atIndex:2];
    [e setBuffer:SB(out) offset:0 atIndex:3];
    [e setBuffer:SB(aux) offset:0 atIndex:4];
    [e setBytes:&args length:sizeof(args) atIndex:5];
    [e dispatchThreads:MTLSizeMake((NSUInteger)hs, (NSUInteger)t, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)hs, (NSUInteger)8), 32, 1)];
}

// Encodes the three backward passes; the serial encoder orders them.
void scaly_mgpu_enc_attn_bwd(int q, int k, int v, int dout, int aux,
                             int dq, int dk, int dv,
                             int t, int d, int hs, int win)
{
    mgpu_attn_args args = { (uint32_t)t, (uint32_t)d, (uint32_t)hs, (uint32_t)win };
    NSUInteger tgx = MIN((NSUInteger)hs, (NSUInteger)8);
    MTLSize grid = MTLSizeMake((NSUInteger)hs, (NSUInteger)t, 1);
    MTLSize tg = MTLSizeMake(tgx, 32, 1);
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_attn_b1];
    [e setBuffer:SB(v) offset:0 atIndex:0];
    [e setBuffer:SB(dout) offset:0 atIndex:1];
    [e setBuffer:SB(aux) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:grid threadsPerThreadgroup:tg];
    [e setComputePipelineState:mgpu_pso_attn_b2];
    [e setBuffer:SB(k) offset:0 atIndex:0];
    [e setBuffer:SB(aux) offset:0 atIndex:1];
    [e setBuffer:SB(dq) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:grid threadsPerThreadgroup:tg];
    [e setComputePipelineState:mgpu_pso_attn_b3];
    [e setBuffer:SB(q) offset:0 atIndex:0];
    [e setBuffer:SB(dout) offset:0 atIndex:1];
    [e setBuffer:SB(aux) offset:0 atIndex:2];
    [e setBuffer:SB(dk) offset:0 atIndex:3];
    [e setBuffer:SB(dv) offset:0 atIndex:4];
    [e setBytes:&args length:sizeof(args) atIndex:5];
    [e dispatchThreads:grid threadsPerThreadgroup:tg];
}

void scaly_mgpu_enc_adam(int p, int g, int mv, int total,
                         float lr, float b1, float b2, float eps, float bc1, float bc2)
{
    mgpu_adam_args args = { (uint32_t)total, lr, b1, b2, eps, bc1, bc2 };
    id<MTLComputeCommandEncoder> e = mgpu_ensure_enc();
    [e setComputePipelineState:mgpu_pso_adam];
    [e setBuffer:SB(p) offset:0 atIndex:0];
    [e setBuffer:SB(g) offset:0 atIndex:1];
    [e setBuffer:SB(mv) offset:0 atIndex:2];
    [e setBytes:&args length:sizeof(args) atIndex:3];
    [e dispatchThreads:MTLSizeMake((NSUInteger)total, 1, 1)
        threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)256, (NSUInteger)total), 1, 1)];
}
