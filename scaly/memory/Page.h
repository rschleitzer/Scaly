namespace scaly {

namespace memory {

struct Page {
    void* next_object;
    Page* current_page;
    Page* next_page;
    PageList exclusive_pages;

    static Page* allocate_page();
    void reset();
    void* allocate(size_t size, size_t align);
    void* allocate_oversized(size_t size);
    static Page* get(void* address);
    size_t get_capacity(size_t align);
    Page* allocate_exclusive_page();
    void deallocate_exclusive_page(Page* page);
    void deallocate_extensions();
};

}

}