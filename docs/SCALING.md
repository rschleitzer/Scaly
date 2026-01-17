# Scaly Self-Scaling Runtime Architecture

> **Scaly: The Self-Scaling Programming Language**
> Write traditional code. It scales automatically.

## Vision

Scaly programs scale transparently across available hardware without requiring the programmer to write parallel or async code. No `async/await`, no thread management, no synchronization primitives. The runtime observes, adapts, and optimizes automatically.

The programmer writes sequential, traditional-style code:

```scaly
function render(pixels: Slice[Pixel], scene: Scene)
{
    for i in 0..pixels.length()
        pixels[i] = raytrace(scene, i)
}
```

Scaly does the rest.

---

## Two Dimensions of Scaling

| Dimension | Purpose | Mechanism |
|-----------|---------|-----------|
| **Supercomputing** | Utilize all CPU cores (and GPU) | Green threads, work-stealing, JIT tiering |
| **Superserving** | Handle massive concurrent I/O | Async I/O backends (io_uring, kqueue, IOCP) |

These are orthogonal and can be combined. A web server that handles 100k connections while parallelizing request processing uses both.

---

## Architecture Overview

```
┌───────────────────────────────────────────────────────────────────────┐
│                           SCALY PROGRAM                               │
├───────────────────────────────────────────────────────────────────────┤
│  User Code                                                            │
│      │                                                                │
│      │  Traditional for-loops, function calls                         │
│      │  No async/await, no thread annotations                         │
│      ▼                                                                │
├───────────────────────────────────────────────────────────────────────┤
│  Runtime (statically linked)                                          │
│  ┌──────────────┬──────────────┬──────────────┬────────────────────┐  │
│  │  Scheduler   │  I/O         │  Profiler    │  JIT               │  │
│  │              │              │              │                    │  │
│  │  Green       │  io_uring    │  Burn-in     │  LLVM ORC          │  │
│  │  Threads     │  kqueue      │  Hotspot     │  Tier generation   │  │
│  │  Work-       │  IOCP        │  Detection   │  Caching           │  │
│  │  Stealing    │              │              │                    │  │
│  └──────┬───────┴───────┬──────┴───────┬──────┴─────────┬──────────┘  │
│         │               │              │                │             │
│  ┌──────┴───────────────┴──────────────┴────────────────┴──────────┐  │
│  │  Hardware Abstraction                                           │  │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────────────────┐  │  │
│  │  │  Cores   │ │   GPU    │ │   I/O    │ │  Cluster (future)  │  │  │
│  │  └──────────┘ └──────────┘ └──────────┘ └────────────────────┘  │  │
│  └─────────────────────────────────────────────────────────────────┘  │
├───────────────────────────────────────────────────────────────────────┤
│  Syscall Layer (libc-free)                                            │
│  ┌──────────────┬──────────────┬──────────────┐                       │
│  │  Linux       │  macOS       │  Windows     │                       │
│  │  syscall     │  syscall     │  NtXxx       │                       │
│  └──────────────┴──────────────┴──────────────┘                       │
└───────────────────────────────────────────────────────────────────────┘
```

---

## Core Design Decisions

### 1. Statically Linked, Single Binary

No dynamic libraries, no DLL hell, no version conflicts.

```bash
$ scalyc raytracer.scaly -o raytracer
$ scp raytracer server:
$ ssh server ./raytracer   # Just works
```

### 2. libc-Free Runtime

The Scaly runtime makes direct syscalls on each platform:

| Platform | Syscall Mechanism |
|----------|-------------------|
| Linux    | `syscall` instruction |
| macOS    | `syscall` instruction |
| Windows  | `NtXxx` functions |

This eliminates the 1970s-era constraints of libc (no async, no threading model). The compiler (`scalyc`) still links against libc because LLVM requires it, but compiled Scaly programs are libc-free.

### 3. Opt-In Scaling

For first contact with new developers, Scaly is small, fast, and simple:

```bash
$ time scalyc hello.scaly -o hello
real    0.1s

$ ls -lh hello
1.2M    hello
```

Scaling features are opt-in:

```bash
scalyc hello.scaly               # Minimal binary (~2 MB)
scalyc raytracer.scaly --scale   # Full runtime + JIT (~30-40 MB)
```

The compiler can also detect when scaling would be beneficial and suggest it.

---

## Supercomputing: CPU Parallelism

### Tiered Execution

The compiler generates multiple versions of parallelizable loops:

| Tier | When Used | Generated | Description |
|------|-----------|-----------|-------------|
| **0** | Burn-in | AOT | Sequential + instrumentation |
| **1** | Small loops | AOT | Sequential, no overhead |
| **2** | CPU hotspots | JIT | Work-stealing across cores |
| **3** | Massive parallelism | JIT | GPU kernel |
| **4** | Distributed (future) | JIT | Cluster distribution |

### Burn-in and Profiling

Static analysis cannot determine:
- How many iterations a loop will have
- How expensive each iteration is
- Current hardware utilization

Therefore, the runtime uses **burn-in profiling**:

```
Execution Timeline:

0.0s    Loop starts with Tier 0 (instrumented)
0.1s    Iteration 1 measured
0.2s    Iteration 2 measured
...
1.0s    Profiler: "This is a hotspot!"
1.1s    Background: JIT compiling Tier 2...
1.2s    Main thread: Still running Tier 0
1.5s    JIT complete!
1.6s    → Atomic switch to Tier 2
1.7s    4 iterations in parallel
        🚀 Liftoff
```

The unoptimized code continues running while JIT compiles in the background. No blocking, no cold-start penalty after the first few seconds.

### JIT with LLVM ORC

The runtime includes LLVM's ORC JIT. When a hotspot is detected:

1. Load loop template (LLVM IR stored in binary)
2. Generate work-stealing wrapper
3. Compile to native code
4. Atomic pointer swap for future invocations

```scaly
define TierCache
{
    var compiled: HashMap[int, pointer[void]]
    var jit: LLVMOrcJIT

    function get_or_compile(loop_id: int, tier: Tier) returns pointer[void]
    {
        choose compiled.get(loop_id * 10 + tier)
        {
            when Some(ptr)
                ptr
            when None
            {
                let ir generate_tier_ir(loop_id, tier)
                let ptr jit.compile(ir)
                compiled.set(loop_id * 10 + tier, ptr)
                ptr
            }
        }
    }
}
```

### Green Threads and Work-Stealing

Inspired by Oliver Kowalke's Boost.Context/Boost.Fiber:

- **Stackful coroutines**: Each fiber has its own stack
- **Work-stealing scheduler**: Idle workers steal from busy workers
- **Continuation Passing Style**: Compiler transforms suspend points (invisible to programmer)

```
┌───────────────────────────────────────────┐
│  Fiber Pool                               │
│  ┌───┐ ┌───┐ ┌───┐ ┌───┐                  │
│  │ F │ │ F │ │ F │ │ F │  ...             │
│  └─┬─┘ └─┬─┘ └─┬─┘ └─┬─┘                  │
│    └─────┴─────┴─────┘                    │
│            │                              │
│     ┌──────▼──────┐                       │
│     │ Work Queue  │                       │
│     └──────┬──────┘                       │
│            │                              │
│     ┌──────▼───────────────────────────┐  │
│     │ Worker Threads (1 per core)      │  │
│     │ ┌────┐ ┌────┐ ┌────┐ ┌────┐     │  │
│     │ │ W0 │ │ W1 │ │ W2 │ │ W3 │     │  │
│     │ └────┘ └────┘ └────┘ └────┘     │  │
│     │         Work-Stealing            │  │
│     └──────────────────────────────────┘  │
└───────────────────────────────────────────┘
```

---

## Superserving: Async I/O

### Platform Backends

| Platform | API | Characteristics |
|----------|-----|-----------------|
| Linux | io_uring | Shared ring buffer, true async, zero-copy capable |
| macOS | kqueue | Event-based, efficient for sockets |
| Windows | IOCP | Completion ports, native async |

### Invisible Async

The programmer writes blocking-style code:

```scaly
function handle_request(conn: Connection) returns Response
{
    let body File.read(conn.path)      ; ← Fiber yields here
    let parsed Parser.parse(body)
    Response.ok(parsed)
}
```

Under the hood:

1. `File.read()` submits request to io_uring/kqueue/IOCP
2. Fiber yields to scheduler
3. Scheduler runs other fibers
4. Completion arrives
5. Fiber resumes with result

No `async` keyword, no `.await`, no callback hell.

---

## The Scaling Cascade

Scaly automatically chooses the appropriate scaling level:

```
Code runs on...

    ┌────────────────────────────────────────────────┐
    │ 1. Cores   (Green Threads, Work-Stealing)      │
    ├────────────────────────────────────────────────┤
    │ 2. GPU     (Compute Shaders via LLVM)          │
    ├────────────────────────────────────────────────┤
    │ 3. Cluster (Distributed Runtime - future)      │
    └────────────────────────────────────────────────┘

    ...and the programmer just writes:

    for item in items
        process(item)
```

---

## Why This Works for Scaly

Scaly's language design enables transparent scaling:

| Feature | Benefit for Scaling |
|---------|---------------------|
| **Function vs Procedure** | Compiler knows what's pure → safe to parallelize |
| **RBMM (Region-Based Memory)** | No shared mutable state → no synchronization needed |
| **No Exceptions** | Clear control flow → reliable CPS transformation |
| **Immutable by Default** | Data races impossible in pure functions |

---

## Binary Sizes

| Configuration | Size | Use Case |
|---------------|------|----------|
| Minimal (`--no-scale`) | ~2 MB | Tutorials, small tools |
| Standard (`--scale`) | ~30-40 MB | Production, CPU scaling |
| Full (`--scale --gpu`) | ~50-60 MB | GPU compute included |

The size increase comes from LLVM ORC JIT. For users who need small binaries, `--no-scale` provides a lean runtime with only AOT-compiled tiers.

---

## Showcase: Cinebench-Style Demo

The self-scaling effect is best demonstrated visually with a raytracer:

```
Second 1-5:    Tier 0, sequential, burn-in
               Image builds slowly, pixel by pixel

Second 5:      Profiler detects hotspot
               JIT compiles Tier 2 in background

Second 6+:     Tier 2 active, work-stealing
               Image fills in dramatically faster

User sees the acceleration live.
```

This demonstrates Scaly's promise without any code changes, flags, or annotations.

---

## Implementation Roadmap

### Phase 1: Foundation (Post Self-Hosting)
- [ ] libc-free syscall layer (Linux first)
- [ ] Basic fiber/green thread implementation
- [ ] Work-stealing scheduler

### Phase 2: CPU Scaling
- [ ] Tiered loop compilation (Tier 0, 1 AOT)
- [ ] Burn-in profiler
- [ ] JIT integration (Tier 2)
- [ ] Cinebench-style demo raytracer

### Phase 3: I/O Scaling
- [ ] io_uring backend (Linux)
- [ ] kqueue backend (macOS)
- [ ] IOCP backend (Windows)
- [ ] Fiber-aware I/O wrappers

### Phase 4: GPU Scaling
- [ ] LLVM NVPTX backend integration
- [ ] GPU-capable loop detection
- [ ] Tier 3 JIT compilation

### Phase 5: Cluster (Future)
- [ ] Peer discovery
- [ ] Work distribution protocol
- [ ] Tier 4 implementation

---

## Summary

Scaly's self-scaling runtime delivers on the language's core promise: **the programmer writes simple, traditional code, and Scaly makes it scale**. Through burn-in profiling, JIT compilation, and platform-native async I/O, programs automatically utilize available hardware from a single core to a GPU cluster.

No async/await. No thread pools. No manual parallelization.

Just code that scales itself.
