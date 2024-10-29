namespace scaly {

namespace memory {

struct Page {
    void* next_object;
    Page* current_page;
    Page* next_page;
    PageList exclusive_pages;

    static Page* allocate_page();
    void reset();
    void* allocate_raw(size_t size, size_t align);
    void deallocate_extensions();
    static Page* get(void* address);
    void deallocate_exclusive_page(Page* page);
    size_t get_capacity(size_t align);
    Page* allocate_exclusive_page();
    void forget();
    bool is_oversized();
    void* allocate_oversized(size_t size);
};

}

}