#include "Memory.cpp"
using namespace scaly::memory;

int main(int argc, char** argv)
{
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto r = Region::create_from_page(Page::get(root_stack_bucket));

    auto page = r.page;

    // Our next object is located behind the data of our page
    if (page->next_object_offset != sizeof(Page))
        return 1;

    // We have no exclusive pages yet
    if (page->exclusive_pages != 0)
        return 1;

    // And we are not oversized
    if (page->is_oversized())
        return 3;

    // The current page is our page itself
    if (page != page->current_page)
        return 4;

    // The first extension page pointer is one pointer behind the last positin
    auto extension_page_location = page->get_extension_page_location();
    if ((extension_page_location != (Page**)((size_t)page + PAGE_SIZE - sizeof(Page**))))
        return 5;

    // The next possible location is behind our data
    auto location = page->get_next_location();
    if (location != (size_t)((char*)page + sizeof(Page)))
        return 6;

    // Allocate an int. Our location moves by bytes.
    auto answer = (int*)page->allocate_raw(4, alignof(int));
    location += 4;
    if (page->get_next_location() != location)
        return 7;
    *answer = 42;

    // Allocate a byte character and set it to A.
    auto another_loc = page->allocate_raw(1, alignof(char));
    auto another = (char*)another_loc;
    location += 1;
    if (page->get_next_location() != location)
        return 8;
    *another = 'A';

    // We allocate another int and move 7 bytes forward because of the 4 byte alignment.
    auto eau_loc = page->allocate_raw(4, alignof(int));
    auto eau = (int*)eau_loc;
    location += 7;
    if (page->get_next_location() != location)
        return 9;
    *eau = 4711;

    // Ask the int for its page which shall be our own.
    if (Page::get(eau) != page)
        return 10;

    // Allocate an oversized object which should cause allocating an exclusive page
    auto array = (size_t*)page->allocate_raw(PAGE_SIZE, alignof(size_t));
    for (int i = 0; i <= PAGE_SIZE / sizeof(size_t) - 1; i++) {
        array[i] = i;
    }
    
    // After allocating the oversized object, the location shall be the same
    if (page->get_next_location() != location)
        return 11;

    // The current page should be still us
    if (page != page->current_page)
        return 12;

    // Overflow the page
    for (int i = 0; i <= PAGE_SIZE; i++) {
        page->allocate_raw(1, 1);
    }

    // Now the current page shall have switched to a new one
    if (page == page->current_page)
        return 13;

    // Since we have one extension page now, our extension page shall point to our current page.
    if (*(page->get_extension_page_location()) != page->current_page)
        return 14;

    // Our new current page shall not have exclusive pages yet.
    if (page->current_page->exclusive_pages != 0)
        return 15;

    // The current page of our new current page shall point to itself.
    if (page->current_page->current_page != page->current_page)
        return 16;

    if (page->exclusive_pages != 1)
        return 17;

    // The data shall be still intact.
    if (*answer != 42)
        return 18;

    if (*another != 'A')
        return 19;

    if (*eau  != 4711)
        return 20;
 
   for (int i = 0; i <= PAGE_SIZE / sizeof(size_t) - 1; i++) {
        if (array[i] != i)
            return 21;
    }
 
    // Now we look at our exclusive page which is one pointer above the location of our next exclusive page.
    auto exclusive_page = *(page->get_next_exclusive_page_location() + 1);
    
    // The exclusive page shall not have a current page.
    if (exclusive_page->current_page != nullptr)
        return 22;

    // The upper part of the allocated size was stored in exclusive_pages which shall be zero since we did a rather small allocation.
    if (exclusive_page->exclusive_pages != 0)
        return 23;

    // The lower part of the allocated size was stored in next_object_offset.
    if (exclusive_page->next_object_offset != sizeof(Page) + PAGE_SIZE)
        return 24;

    // The page of our array pointer shall be our exclusive page.
    if (Page::get(array) != exclusive_page)
        return 25;

    // Free the array storage.
    if (!page->reclaim_array(Page::get(array)))
        return 26;

    // Our page shall have no more exclusive pages now.
    if (page->exclusive_pages != 0)
        return 27;

    return 0;
}