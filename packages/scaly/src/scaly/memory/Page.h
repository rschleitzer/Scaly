#ifndef _Page_h
#define _Page_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
const int PAGE_SIZE = 0x1000;

struct Page : Object {
    void* next_object;
    Page* current_page;
    Page* next_page;
    PageList exclusive_pages;

    Page(void* next_object, Page* current_page, Page* next_page, PageList exclusive_pages);
    static Page* allocate_page();
    void reset();
    void* allocate(size_t size, size_t align);
    void* allocate_oversized(size_t size);
    static Page* get(void* address);
    size_t get_capacity(size_t align);
    Page* allocate_exclusive_page();
    void deallocate_extensions();
    void deallocate_exclusive_page(Page* page);
};

#endif