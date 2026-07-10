// Metal GPU matmul shim (stage 6.0 spike). Containment rule (a):
// Metal / MetalPerformanceShaders are macOS-only Objective-C APIs
// (OS-specific symbols and object types) — the committed seed ships one
// scaly.ll for all targets, so this backend cannot live in Scaly code.
// One file, one concern: get f32 matrices onto the GPU, multiply them
// (vendor MPS path = the ceiling; hand-written MSL 4x4-tile kernel =
// the kernel-extraction path), get the result back, and expose timing.
//
// NOT part of libscaly.a yet: tests/tensor/bench/run_gpu.sh compiles it
// (tools/mgpu.sh) and links it explicitly with the Metal frameworks.
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
