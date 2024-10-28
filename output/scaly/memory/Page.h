#ifndef _Page_h
#define _Page_h
#include "../../scaly.h"
using namespace scaly::memory;
const int PAGE_SIZE = 0x1000;

struct Page : Object {
    void* next_object;
    Page* current_page;
    Page* next_page;
    PageList exclusive_pages;
    static Page* get(void* address);
    void deallocate_extensions();
    void reset();
    void deallocate_exclusive_page(Page* page);
    size_t get_capacity(size_t align);
    void* allocate_raw(size_t size, size_t align);
    static Page* allocate_page();
    void forget();
    Page* allocate_exclusive_page();
    bool is_oversized();
    void* allocate_oversized(size_t size);
};

#endif