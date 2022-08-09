#include "Memory.cpp"
using namespace scaly::memory;

void test_page()
{
    Region _r;

    Page& page = *_r.get_page();

    // We have no next page yet
    if (page.next_page != nullptr)
        exit (-1);

    // And we are not oversized
    if (page.is_oversized())
        exit (-2);

    auto location = (size_t)page.next_object;

    // Allocate an int. Our location moves by bytes.
    auto answer = (int*)page.allocate_raw(4, alignof(int));
    location += 4;
    if ((size_t)page.next_object != location)
        exit (-4);
    *answer = 42;

    // Allocate a byte character and set it to A.
    auto another_loc = page.allocate_raw(1, alignof(char));
    auto another = (char*)another_loc;
    location += 1;
    if ((size_t)page.next_object != location)
        exit (-5);
    *another = 'A';

    // We allocate another int and move 7 bytes forward because of the 4 byte alignment.
    auto eau_loc = page.allocate_raw(4, alignof(int));
    auto eau = (int*)eau_loc;
    location += 7;
    if ((size_t)page.next_object != location)
        exit (-6);
    *eau = 4711;

    // Ask the int for its page which shall be our own.
    if (Page::get(eau) != &page)
        exit (-7);

    // Allocate an oversized object which should cause allocating an oversized page
    auto array = (size_t*)page.allocate_raw(PAGE_SIZE, alignof(size_t));
    for (int i = 0; i <= PAGE_SIZE / sizeof(size_t) - 1; i++) {
        array[i] = i;
    }

    // Since an oversized page is always exclusive, we should not yet have a next page
    if (page.next_page != nullptr)
        exit (-9);

    // Our allocator should not yet point to a current page
    if (page.current_page != nullptr)
        exit (-10);

    // Now we look at our oversized page.
    Page& oversized_page = *Page::get(page.exclusive_pages.head->element);

    // A null next_object indicates an oversized page.
    if (oversized_page.next_object != nullptr)
        exit (-15);

    // The page of our array pointer shall be our oversized page.
    if (Page::get(array) != &oversized_page)
        exit (-17);

    // Overflow the page
    for (int i = 0; i <= PAGE_SIZE; i++) {
        page.allocate_raw(1, 1);
    }

    // Our allocator should now point to a current page
    if (page.current_page == &page)
        exit (-18);

    // The data shall be still intact.
    if (*answer != 42)
        exit (-19);

    if (*another != 'A')
        exit (-20);

    if (*eau  != 4711)
        exit (-21);
 
    for (int i = 0; i <= PAGE_SIZE / sizeof(size_t) - 1; i++) {
        if (array[i] != i)
            exit (-22);
    }

    auto exclusive_page = page.allocate_exclusive_page();
 }

void test_allocate_raw()
{
    Region r;
    Page& page = *r.get_page();

    size_t* start = nullptr;
    size_t* p_previous = nullptr;
    size_t* p = nullptr;
    auto pointers = 128000000;
    for (int i = 1; i <= pointers; i++) {
        p = (size_t*)page.allocate_raw(sizeof(size_t), alignof(size_t));
        *p = 0;
        if (i == 1) {
            start = p;
        } else {
            *p_previous = (size_t)p;
        }
        p_previous = p;
    }

    // Check the pointer chain
    auto counter = 0;
    p = start;
    while (p != nullptr) {
        counter += 1;
        p = (size_t*)*p;
    }
    if (counter != pointers)
        exit (-28);
}


int main(int argc, char** argv)
{
    test_page();
    test_allocate_raw();
}
