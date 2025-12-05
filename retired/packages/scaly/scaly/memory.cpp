#include "../scaly.h"
namespace scaly {
namespace memory {


void test_page() {
    auto r = Region();
    auto page = r.get_page();
    if ((*page).next_page != nullptr) 
        exit(-1);
    auto location = (size_t)(*page).next_object;
    const auto answer = (int*)(*page).allocate(4, alignof(int));
    location+=4;
    if ((size_t)(*page).next_object != location) 
        exit(-4);
    *answer = 42;
    const auto another_loc = (*page).allocate(1, alignof(char));
    const auto another = (char*)another_loc;
    location = location+1;
    if ((size_t)(*page).next_object != location) 
        exit(-5);
    *another = 'A';
    const auto eau_loc = (*page).allocate(4, alignof(int));
    const auto eau = (int*)eau_loc;
    location+=7;
    if ((size_t)(*page).next_object != location) 
        exit(-6);
    *eau = 4711;
    if (Page::get(eau) != page) 
        exit(-7);
    const auto array = (size_t*)(*page).allocate(PAGE_SIZE, alignof(size_t));
    {
        int i = 0;
        while (i<=PAGE_SIZE/sizeof(size_t)-1) {
            *(array+i) = i;
            i = i+1;
        };
    };
    if (((*page).next_page != nullptr)) 
        exit(-9);
    if (((*page).current_page != nullptr)) 
        exit(-10);
    const auto oversized_page = Page::get((*(*page).exclusive_pages.head).page);
    if ((*oversized_page).next_object != nullptr) 
        exit(-15);
    if (Page::get(array) != oversized_page) 
        exit(-17);
    {
        int i = 0;
        while (i<=PAGE_SIZE) {
            (*page).allocate(1, 1);
            i = i+1;
        };
    };
    if ((*page).current_page == page) 
        exit(-18);
    if (*answer != 42) 
        exit(-19);
    if (*another != 'A') 
        exit(-20);
    if (*eau != 4711) 
        exit(-21);
    {
        int i = 0;
        while (i<=PAGE_SIZE/sizeof(size_t)-1) {
            if (*(array+i) != i) 
                exit(-22);
            i = i+1;
        };
    };
    const auto exclusive_page = (*page).allocate_exclusive_page();
}

void test() {
    test_page();
}

}

}