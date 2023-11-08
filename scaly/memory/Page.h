namespace scaly {

namespace memory {

struct Page {
    void* next_object;
    Page* current_page;
    Page* next_page;
    PageList exclusive_pages;

    void reset();
    bool is_oversized();
    size_t get_capacity(size_t align);
    void* allocate_raw(size_t size, size_t align);
    void* allocate_oversized(size_t size);
    static Page* allocate_page();
    Page* allocate_exclusive_page();
    void deallocate_extensions();
    void forget();
    static Page* get(void* address);
    void deallocate_exclusive_page(Page* page);
};

}

}