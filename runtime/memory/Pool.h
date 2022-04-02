namespace scaly::memory {

struct Heap;

struct Pool : Object {
    size_t map;
    Heap* heap;

    static Pool* create(Heap* heap);
    Page* allocate_page();
    size_t get_allocation_bit(size_t page);
    void mark_as_full(size_t page);
    void mark_as_free(size_t page);
    void deallocate();
};

}
