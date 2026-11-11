gpu-wgsl: tests/gpu-plan/fixture.scaly:49:5: kernel
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read> b0: array<i32>;
@group(0) @binding(3) var<storage, read_write> o0: array<i32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[4];
    if (gid >= U[3]) { return; }
    var F: u32 = 0u;
    let u0: i32 = bitcast<i32>(U[0]);
    let n0: u32 = U[1];
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        var t0: i32 = 0;
        if ((0u != 0u) || (v0 >= n0)) { F = 1u; } else { t0 = b0[v0]; }
        let t2: i32 = i32(u32(t0) + u32(u0));
        o0[gid] = t2;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/fixture.scaly:61:1: kernel
fn f0(p0: i32, p1: i32, F: ptr<function, u32>) -> i32
{
    var v0: i32 = p0;
    var v1: i32 = 0;
    while ((*F) == 0u)
    {
        let t0: i32 = v1;
        let t1: bool = i32(t0) < i32(3);
        if (!t1) { break; }
        let t2: i32 = v0;
        let t3: i32 = i32(u32(t2) + u32(p1));
        v0 = t3;
        let t4: i32 = v1;
        let t5: i32 = i32(u32(t4) + u32(1));
        v1 = t5;
    }
    let t6: i32 = v0;
    return t6;
    return 0;
}
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read_write> o0: array<i32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[2];
    if (gid >= U[1]) { return; }
    var F: u32 = 0u;
    let u0: i32 = bitcast<i32>(U[0]);
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        let t0: i32 = f0(i32(v0), u0, &F);
        o0[gid] = t0;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/fixture.scaly:66:1: kernel
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read> b0: array<i32>;
@group(0) @binding(3) var<storage, read_write> o0: array<i32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[3];
    if (gid >= U[2]) { return; }
    var F: u32 = 0u;
    let n0: u32 = U[0];
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        var v1: i32 = 0;
        if ((0u != 0u) && ((0u & 2147483648u) == 0u)) { F = 1u; }
        for (var v2: u32 = 0u; (F == 0u) && (0u == 0u) && (v2 < 4u); v2++)
        {
            let t0: i32 = v1;
            var t1: i32 = 0;
            if ((0u != 0u) || (v0 >= n0)) { F = 1u; } else { t1 = b0[v0]; }
            let t3: i32 = i32(u32(t1) * u32(i32(v2)));
            let t4: i32 = i32(u32(t0) + u32(t3));
            v1 = t4;
        }
        let t5: i32 = v1;
        o0[gid] = t5;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/fixture.scaly:69:5: no kernel: reduces into s
gpu-wgsl: tests/gpu-plan/fixture.scaly:76:1: no kernel for webgpu: kernel (metal): out c; in big; calls wide; not webgpu: the counter i, used outside a cast, is a 64-bit integer
gpu-wgsl: tests/gpu-plan/fixture.scaly:81:1: no kernel for webgpu: kernel (metal): out d; in scale; calls half; not webgpu: floating point, and the loop is not relaxed
gpu-wgsl: tests/gpu-plan/fixture.scaly:86:1: no kernel: e is a Vector[double]
gpu-wgsl: tests/gpu-plan/fixture.scaly:91:1: no kernel: down is recursive
gpu-wgsl: tests/gpu-plan/fixture.scaly:96:1: no kernel: blocked: extern call toupper
gpu-wgsl: tests/gpu-plan/fixture.scaly:101:1: no kernel: text_of allocates or throws
gpu-wgsl: tests/gpu-plan/fixture.scaly:109:1: no kernel: builds a Pair
gpu-wgsl: tests/gpu-plan/fixture.scaly:117:1: kernel
fn f0(p0: i32, F: ptr<function, u32>) -> i32
{
    let t0: bool = i32(p0) > i32(9);
    if (t0)
    {
        return 9;
    }
    return p0;
    return 0;
}
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read> b0: array<i32>;
@group(0) @binding(3) var<storage, read_write> o0: array<i32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[6];
    if (gid >= U[5]) { return; }
    var F: u32 = 0u;
    let u0: i32 = bitcast<i32>(U[0]);
    let u1: u32 = U[1];
    let u1_h: u32 = U[2];
    let n0: u32 = U[3];
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        var t0: i32 = 0;
        var t1: i32 = 0;
        if ((0u != 0u) || (v0 >= n0)) { F = 1u; } else { t1 = b0[v0]; }
        let t3: bool = i32(t1) > i32(9);
        if (t3)
        {
            let t4: i32 = f0(u0, &F);
            t0 = t4;
        }
        else
        {
            t0 = 1;
        }
        let v1: i32 = t0;
        let t5: i32 = i32(u32(v1) + u32(i32(u1)));
        o0[gid] = t5;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/fixture.scaly:125:1: no kernel: reduces into total
gpu-wgsl: tests/gpu-plan/fixture.scaly:130:1: no kernel: a put at another index than the loop's counter
gpu-wgsl: tests/gpu-plan/fixture.scaly:134:1: no kernel: reads and writes m
gpu-wgsl: tests/gpu-plan/fixture.scaly:138:1: no kernel: blocked: transitively impure call print
gpu-wgsl: tests/gpu-plan/relaxed.scaly:15:1: no kernel for webgpu: kernel (metal): out a; calls half; not webgpu: floating point, and the loop is not relaxed
gpu-wgsl: tests/gpu-plan/relaxed.scaly:17:9: kernel
fn f0(p0: f32, F: ptr<function, u32>) -> f32
{
    let t0: f32 = p0 * 0.5f;
    return t0;
    return 0.0f;
}
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read_write> o0: array<f32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[1];
    if (gid >= U[0]) { return; }
    var F: u32 = 0u;
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        let t0: f32 = f32(v0);
        let t1: f32 = f0(t0, &F);
        o0[gid] = t1;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/relaxed.scaly:21:5: kernel
fn f0(p0: f32, F: ptr<function, u32>) -> f32
{
    let t0: f32 = p0 * 0.5f;
    return t0;
    return 0.0f;
}
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read_write> o0: array<f32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[1];
    if (gid >= U[0]) { return; }
    var F: u32 = 0u;
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        let t0: f32 = f32(v0);
        let t1: f32 = f0(t0, &F);
        let t2: f32 = t1 + 1.0f;
        o0[gid] = t2;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/relaxed.scaly:23:5: kernel
@group(0) @binding(0) var<storage, read> U: array<u32>;
@group(0) @binding(1) var<storage, read_write> FLT: array<atomic<u32>>;
@group(0) @binding(2) var<storage, read_write> o0: array<i32>;
@compute @workgroup_size(64)
fn k(@builtin(global_invocation_id) id: vec3<u32>)
{
    let gid: u32 = id.x + id.y * U[1];
    if (gid >= U[0]) { return; }
    var F: u32 = 0u;
    let v0: u32 = gid;
    for (var once: u32 = 0u; once < 1u; once++)
    {
        let t0: i32 = i32(u32(i32(v0)) * u32(2));
        o0[gid] = t0;
    }
    if (F != 0u) { atomicStore(&FLT[0], 1u); }
}

gpu-wgsl: tests/gpu-plan/relaxed.scaly:25:5: no kernel for webgpu: kernel (metal): out e; not webgpu: the counter i, used outside a cast, is a 64-bit integer
gpu-wgsl: tests/gpu-plan/relaxed.scaly:28:1: no kernel for webgpu: kernel (metal): out c; calls half; not webgpu: floating point, and the loop is not relaxed
