// State + I/O shim for the Scaly-implemented page allocator.
//
// The allocator itself lives in packages/scaly/0.1.0/scaly/memory/
// root_pages.scaly (StackBucket/HeapBucket logic + SCALY_TRACE_ROOT
// diagnostic counters). This shim exists because:
//
//   1. Scaly has no mutable module-level globals yet, so the three state
//      pointers + trace array live in a C static.
//   2. Scaly has no format-string / stderr-print facility, so the
//      diagnostic dump and the LIFO-violation abort are done in C.
//   3. Scaly has no function-pointer-to-atexit syntax, so the atexit
//      registration also stays here.
//
// The shim exposes three entry points. Everything else — bucket walks,
// bitmap scans, trace counting — is Scaly code.
//
// Layout of ScalyRuntimeState mirrors the Scaly-side struct exactly. If
// either side changes, update both.

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

constexpr int64_t TRACE_MAX = 8192;

struct TraceEntry {
    const char* name;
    int64_t     push_count;
    int64_t     pop_count;
};

struct ScalyRuntimeState {
    void*        stack_head;
    void*        stack_top;
    void*        heap_head;
    int64_t      trace_enabled;
    int64_t      trace_atexit_registered;
    int64_t      trace_count;
    TraceEntry   trace_entries[TRACE_MAX];
};

ScalyRuntimeState g_state{};

void trace_atexit_dump_cb() {
    bool any = false;
    for (int64_t i = 0; i < g_state.trace_count; i++) {
        const TraceEntry& e = g_state.trace_entries[i];
        if (e.push_count != e.pop_count) {
            fprintf(stderr,
                "scaly_trace_root: UNBALANCED %s push=%lld pop=%lld leak=%lld\n",
                e.name,
                (long long)e.push_count,
                (long long)e.pop_count,
                (long long)(e.push_count - e.pop_count));
            any = true;
        }
    }
    if (!any) {
        fprintf(stderr, "scaly_trace_root: all root-page push/pop balanced\n");
    }
}

}  // namespace

extern "C" {

ScalyRuntimeState* scaly_rt_state() {
    return &g_state;
}

// Called by the Scaly trace layer on first push/pop. Reads $SCALY_TRACE_ROOT
// and arms the atexit dump exactly once per process.
void scaly_rt_register_trace() {
    if (g_state.trace_atexit_registered) return;
    g_state.trace_atexit_registered = 1;
    if (std::getenv("SCALY_TRACE_ROOT")) {
        g_state.trace_enabled = 1;
        std::atexit(trace_atexit_dump_cb);
    }
}

// Called by scaly_release_root_page when LIFO invariant is violated.
// Prints the mismatch and aborts. Must stay in C because the Scaly
// allocator cannot call into anything that might itself need a root page.
void scaly_rt_abort_lifo(const void* release, const void* top) {
    fprintf(stderr,
        "scaly_release_root_page: LIFO violation — release=%p top=%p\n",
        release, top);
    abort();
}

// Diagnostic push/pop counters, emitted by the Emitter at every root-page
// push/pop site. Each entry is keyed by the emitting function's name; the
// atexit dump reports any name whose push and pop counts don't balance.
// Linear lookup keeps the hot path allocation-free at the cost of O(N)
// per update — acceptable for a diagnostic that is off by default.

static TraceEntry* trace_find_or_add(const char* name) {
    if (!name) name = "<null>";
    for (int64_t i = 0; i < g_state.trace_count; i++) {
        if (strcmp(g_state.trace_entries[i].name, name) == 0)
            return &g_state.trace_entries[i];
    }
    if (g_state.trace_count >= TRACE_MAX) return nullptr;
    TraceEntry& e = g_state.trace_entries[g_state.trace_count++];
    e.name = strdup(name);
    e.push_count = 0;
    e.pop_count = 0;
    return &e;
}

void scaly_trace_root_push(const char* name) {
    scaly_rt_register_trace();
    if (!g_state.trace_enabled) return;
    if (TraceEntry* e = trace_find_or_add(name)) e->push_count++;
}

void scaly_trace_root_pop(const char* name) {
    scaly_rt_register_trace();
    if (!g_state.trace_enabled) return;
    if (TraceEntry* e = trace_find_or_add(name)) e->pop_count++;
}

}  // extern "C"
