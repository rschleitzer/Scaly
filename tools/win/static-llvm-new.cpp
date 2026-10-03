// tools/win/static-llvm-new.cpp -- part of the static-LLVM EXPERIMENT only
// (tools/win-link.sh with SCALY_STATIC_LLVM_DIR set AND
// SCALY_STATIC_LLVM_HEAP=rpmalloc; tests/win32/WINDOWS-BOX.md §10). The
// replaceable C++ allocation operators, routed to rpmalloc. ★Measured: correct,
// and worth 1-2 % of a large compile at most -- not the default.
//
// Why: LLVM's release libraries carry rpmalloc to REPLACE malloc, which needs
// the static C library. A JIT host takes the C library from ucrtbase.dll, so
// malloc/free stay the UCRT's (one heap for strdup, the runtime's shims and
// the JIT's code). But all the C++ in such a
// program IS LLVM, and C++ pairs new with delete: these operators are a second
// heap that never meets the first. What LLVM takes with malloc directly
// (SmallVector growth, StringMap tables, the C API's strdup'ed messages) stays
// on the UCRT heap, and so does everything a C caller frees.
//
// No header: the three names below are all <new> would give, and the file
// compiles for either architecture without an INCLUDE path.
typedef decltype(sizeof 0) size_t;
namespace std {
enum class align_val_t : size_t {};
struct nothrow_t;
}
extern "C" {
void* rpmalloc(size_t size);
void rpfree(void* ptr);
void* rpaligned_alloc(size_t alignment, size_t size);
}

// LLVM is built without exceptions; an allocation that fails ends the program
// as LLVM's own bad-alloc handler would.
static inline void* scaly_new(size_t n)
{
    void* p = rpmalloc(n ? n : 1);
    if (!p)
        __builtin_trap();
    return p;
}

static inline void* scaly_new_aligned(size_t n, std::align_val_t a)
{
    void* p = rpaligned_alloc((size_t)a, n ? n : 1);
    if (!p)
        __builtin_trap();
    return p;
}

void* operator new(size_t n) { return scaly_new(n); }
void* operator new[](size_t n) { return scaly_new(n); }
void* operator new(size_t n, const std::nothrow_t&) noexcept { return rpmalloc(n ? n : 1); }
void* operator new[](size_t n, const std::nothrow_t&) noexcept { return rpmalloc(n ? n : 1); }
void* operator new(size_t n, std::align_val_t a) { return scaly_new_aligned(n, a); }
void* operator new[](size_t n, std::align_val_t a) { return scaly_new_aligned(n, a); }
void* operator new(size_t n, std::align_val_t a, const std::nothrow_t&) noexcept { return rpaligned_alloc((size_t)a, n ? n : 1); }
void* operator new[](size_t n, std::align_val_t a, const std::nothrow_t&) noexcept { return rpaligned_alloc((size_t)a, n ? n : 1); }

void operator delete(void* p) noexcept { rpfree(p); }
void operator delete[](void* p) noexcept { rpfree(p); }
void operator delete(void* p, size_t) noexcept { rpfree(p); }
void operator delete[](void* p, size_t) noexcept { rpfree(p); }
void operator delete(void* p, const std::nothrow_t&) noexcept { rpfree(p); }
void operator delete[](void* p, const std::nothrow_t&) noexcept { rpfree(p); }
void operator delete(void* p, std::align_val_t) noexcept { rpfree(p); }
void operator delete[](void* p, std::align_val_t) noexcept { rpfree(p); }
void operator delete(void* p, size_t, std::align_val_t) noexcept { rpfree(p); }
void operator delete[](void* p, size_t, std::align_val_t) noexcept { rpfree(p); }
void operator delete(void* p, std::align_val_t, const std::nothrow_t&) noexcept { rpfree(p); }
void operator delete[](void* p, std::align_val_t, const std::nothrow_t&) noexcept { rpfree(p); }
