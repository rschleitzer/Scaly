// Page allocator runtime.
//
// Two bucket types live in statically-decided address spaces, differentiated
// only by the calling API — never by a runtime tag.
//
//   StackBucket: strict LIFO. Allocated only for "root" pages — i.e., pages
//     pushed at function entry and at `try` entry. Release is a pointer
//     decrement; no bitmap state to maintain.
//
//   HeapBucket:  bitmap-managed. Allocated for "extension" pages (when a
//     root page overflows) and "exclusive" pages (HashMap rehash etc.).
//     Release clears a bit; bucket rejoins the free list if it was full.
//
// Each bucket is a BUCKET_SIZE-aligned memory region of BUCKET_PAGES pages.
// Page 0 holds the bucket's header struct; pages 1..BUCKET_PAGES-1 are
// usable. Given any page address, the bucket's base is just `addr & ~MASK`.
//
// Global state: three pointers. Single-threaded for now; thread-local is
// the obvious next step.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

constexpr size_t PAGE_SIZE = 0x1000;
constexpr size_t BUCKET_PAGES = 64;
constexpr size_t BUCKET_SIZE = PAGE_SIZE * BUCKET_PAGES;
constexpr uintptr_t BUCKET_MASK = BUCKET_SIZE - 1;

// All pages usable except page 0 (header). Bits 0..62 map to pages 1..63.
// Bit set = page free.
constexpr uint64_t BITMAP_ALL_FREE = (1ULL << (BUCKET_PAGES - 1)) - 1;

struct Page {
    void* next_object;
    Page* current_page;
    Page* next_page;
    struct { void* head; } exclusive_pages;
};

struct StackBucket {
    StackBucket* prev;
    StackBucket* next;
    // rest of page 0 unused
};

struct HeapBucket {
    HeapBucket* prev;
    HeapBucket* next;
    uint64_t bitmap;  // bit i = page (i+1) is free
    // rest of page 0 unused
};

// Global allocator state.
StackBucket* g_stack_head = nullptr;
Page*        g_stack_top  = nullptr;  // most recently allocated root page
HeapBucket*  g_heap_head  = nullptr;  // head of buckets with free pages

void reset_page(Page* p) {
    p->next_object = reinterpret_cast<void*>(p + 1);
    p->current_page = nullptr;
    p->next_page = nullptr;
    p->exclusive_pages.head = nullptr;
}

void* aligned_bucket_alloc() {
    void* mem = nullptr;
#if defined(_WIN32)
    mem = _aligned_malloc(BUCKET_SIZE, BUCKET_SIZE);
#else
    if (posix_memalign(&mem, BUCKET_SIZE, BUCKET_SIZE) != 0) mem = nullptr;
#endif
    if (!mem) {
        fprintf(stderr, "scaly page runtime: out of memory allocating bucket\n");
        abort();
    }
    return mem;
}

StackBucket* create_stack_bucket(StackBucket* prev) {
    void* mem = aligned_bucket_alloc();
    auto* sb = static_cast<StackBucket*>(mem);
    sb->prev = prev;
    sb->next = nullptr;
    if (prev) prev->next = sb;
    return sb;
}

HeapBucket* create_heap_bucket() {
    void* mem = aligned_bucket_alloc();
    auto* hb = static_cast<HeapBucket*>(mem);
    hb->prev = nullptr;
    hb->next = nullptr;
    hb->bitmap = BITMAP_ALL_FREE;
    return hb;
}

inline StackBucket* stack_bucket_of(const Page* p) {
    return reinterpret_cast<StackBucket*>(reinterpret_cast<uintptr_t>(p) & ~BUCKET_MASK);
}

inline HeapBucket* heap_bucket_of(const Page* p) {
    return reinterpret_cast<HeapBucket*>(reinterpret_cast<uintptr_t>(p) & ~BUCKET_MASK);
}

inline Page* first_usable_page(const void* bucket_base) {
    return reinterpret_cast<Page*>(reinterpret_cast<uintptr_t>(bucket_base) + PAGE_SIZE);
}

inline Page* last_usable_page(const void* bucket_base) {
    return reinterpret_cast<Page*>(reinterpret_cast<uintptr_t>(bucket_base) + (BUCKET_PAGES - 1) * PAGE_SIZE);
}

}  // anonymous namespace

extern "C" {

// -- StackBucket (root pages) ------------------------------------------------

Page* scaly_alloc_root_page() {
    if (!g_stack_top) {
        if (!g_stack_head) g_stack_head = create_stack_bucket(nullptr);
        g_stack_top = first_usable_page(g_stack_head);
        reset_page(g_stack_top);
        return g_stack_top;
    }

    StackBucket* bucket = stack_bucket_of(g_stack_top);
    Page* next = reinterpret_cast<Page*>(reinterpret_cast<uintptr_t>(g_stack_top) + PAGE_SIZE);

    if (next <= last_usable_page(bucket)) {
        g_stack_top = next;
    } else {
        if (!bucket->next) bucket->next = create_stack_bucket(bucket);
        g_stack_top = first_usable_page(bucket->next);
    }
    reset_page(g_stack_top);
    return g_stack_top;
}

void scaly_release_root_page(Page* p) {
    // Strict LIFO is the intended invariant. A few emitter paths in the
    // Scaly stdlib still produce exit flows whose cleanup doesn't match a
    // preceding push (tracked in issue queue). Tolerate the mismatch here
    // by skipping the decrement: the orphaned slot remains inside its
    // bucket (backing memory is still valid) and is reclaimed wholesale
    // when the bucket is freed at process exit. This preserves
    // correctness at the cost of not recycling the slot.
    if (p != g_stack_top) {
        return;
    }
    StackBucket* bucket = stack_bucket_of(p);
    Page* first = first_usable_page(bucket);

    if (p == first) {
        // Popped the bucket's first page; unwind to previous bucket (if any)
        if (!bucket->prev) {
            g_stack_top = nullptr;
        } else {
            g_stack_top = last_usable_page(bucket->prev);
        }
    } else {
        g_stack_top = reinterpret_cast<Page*>(reinterpret_cast<uintptr_t>(p) - PAGE_SIZE);
    }
}

// -- HeapBucket (extension and exclusive pages) ------------------------------

Page* scaly_alloc_page() {
    if (!g_heap_head) {
        g_heap_head = create_heap_bucket();
    }

    HeapBucket* b = g_heap_head;
    // g_heap_head always has bitmap != 0 (we remove full buckets below).
    int slot = __builtin_ctzll(b->bitmap);   // 0..62
    b->bitmap &= ~(1ULL << slot);

    Page* page = reinterpret_cast<Page*>(
        reinterpret_cast<uintptr_t>(b) + (slot + 1) * PAGE_SIZE);
    reset_page(page);

    if (b->bitmap == 0) {
        // Bucket is now full; unlink from free-list.
        g_heap_head = b->next;
        if (g_heap_head) g_heap_head->prev = nullptr;
        b->next = nullptr;
        b->prev = nullptr;
    }

    return page;
}

void scaly_release_page(Page* p) {
    // Oversized pages (allocated directly via aligned_alloc in Page.scaly's
    // allocate_oversized) carry next_object == null. They go back via free().
    if (p->next_object == nullptr) {
        free(p);
        return;
    }

    HeapBucket* b = heap_bucket_of(p);
    int slot = static_cast<int>(
        (reinterpret_cast<uintptr_t>(p) - reinterpret_cast<uintptr_t>(b)) / PAGE_SIZE) - 1;

    bool was_full = (b->bitmap == 0);
    b->bitmap |= (1ULL << slot);

    if (was_full) {
        // Rejoin the free-list at the head.
        b->next = g_heap_head;
        b->prev = nullptr;
        if (g_heap_head) g_heap_head->prev = b;
        g_heap_head = b;
    }
}

}  // extern "C"
