namespace scaly {
namespace memory {

struct Heap;
struct Page;

struct StackBucket {
    Heap* heap;
    StackBucket* next_bucket;

    static StackBucket* create(Heap* heap);
    Page* allocate_page();
    static Page* allocate_bucket();
    static Page* new_page(Page* page);
    Page* get_page_from_next_bucket();
    void deallocate();
};

struct HeapBucket{
    Pool* pool;
    size_t map;

    Page* allocate_page();
    void deallocate_page(Page* page);
};

struct Bucket : Object {
    enum { Stack, Heap } tag;
    union {
        StackBucket stack;
        HeapBucket heap;
    };

    Bucket(HeapBucket heap);
    Bucket(StackBucket stack);
    static Bucket* get(void* address);
    Page* allocate_page();
    void deallocate_page(Page* page);
    static size_t find_least_position(size_t n);
    static size_t find_least_position_64(size_t n);
    static size_t find_least_position_32(size_t n);
};

}

}