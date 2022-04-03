namespace scaly::memory {

struct Heap;

struct Pool : Object {
    size_t map;
    Heap* heap;

    static Pool* create(Heap* heap);
    Page* allocate_page();
    size_t get_allocation_bit(Page* page);
    void mark_as_full(Page* page);
    void mark_as_free(Page* page);
    void deallocate();
};

}
