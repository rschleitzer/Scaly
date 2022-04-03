#include "Memory.cpp"
using namespace scaly::memory;

void test_page()
{
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto r = Region::create_from_page(Page::get(root_stack_bucket));

    auto page = r.page;

    // Our next object is located behind the data of our page
    if (page->next_object_offset != sizeof(Page))
        exit (-1);

    // We have no exclusive pages yet
    if (page->exclusive_pages != 0)
        exit (-2);

    // And we are not oversized
    if (page->is_oversized())
        exit (-3);

    // The current page is our page itself
    if (page != page->current_page)
        exit (-4);

    // The first extension page pointer is one pointer behind the last positin
    auto extension_page_location = page->get_extension_page_location();
    if ((extension_page_location != (Page**)((size_t)page + PAGE_SIZE - sizeof(Page**))))
        exit (-5);

    // The next possible location is behind our data
    auto location = page->get_next_location();
    if (location != (size_t)((char*)page + sizeof(Page)))
        exit (-6);

    // Allocate an int. Our location moves by bytes.
    auto answer = (int*)page->allocate_raw(4, alignof(int));
    location += 4;
    if (page->get_next_location() != location)
        exit (-7);
    *answer = 42;

    // Allocate a byte character and set it to A.
    auto another_loc = page->allocate_raw(1, alignof(char));
    auto another = (char*)another_loc;
    location += 1;
    if (page->get_next_location() != location)
        exit (-8);
    *another = 'A';

    // We allocate another int and move 7 bytes forward because of the 4 byte alignment.
    auto eau_loc = page->allocate_raw(4, alignof(int));
    auto eau = (int*)eau_loc;
    location += 7;
    if (page->get_next_location() != location)
        exit (-9);
    *eau = 4711;

    // Ask the int for its page which shall be our own.
    if (Page::get(eau) != page)
        exit (-10);

    // Allocate an oversized object which should cause allocating an exclusive page
    auto array = (size_t*)page->allocate_raw(PAGE_SIZE, alignof(size_t));
    for (int i = 0; i <= PAGE_SIZE / sizeof(size_t) - 1; i++) {
        array[i] = i;
    }
    
    // After allocating the oversized object, the location shall be the same
    if (page->get_next_location() != location)
        exit (-11);

    // The current page should be still us
    if (page != page->current_page)
        exit (-12);

    // Overflow the page
    for (int i = 0; i <= PAGE_SIZE; i++) {
        page->allocate_raw(1, 1);
    }

    // Now the current page shall have switched to a new one
    if (page == page->current_page)
        exit (-13);

    // Since we have one extension page now, our extension page shall point to our current page.
    if (*(page->get_extension_page_location()) != page->current_page)
        exit (-14);

    // Our new current page shall not have exclusive pages yet.
    if (page->current_page->exclusive_pages != 0)
        exit (-15);

    // The current page of our new current page shall point to itself.
    if (page->current_page->current_page != page->current_page)
        exit (-16);

    if (page->exclusive_pages != 1)
        exit (-17);

    // The data shall be still intact.
    if (*answer != 42)
        exit (-18);

    if (*another != 'A')
        exit (-19);

    if (*eau  != 4711)
        exit (-20);
 
   for (int i = 0; i <= PAGE_SIZE / sizeof(size_t) - 1; i++) {
        if (array[i] != i)
            exit (-21);
    }
 
    // Now we look at our exclusive page which is one pointer above the location of our next exclusive page.
    auto exclusive_page = *(page->get_next_exclusive_page_location() + 1);
    
    // The exclusive page shall not have a current page.
    if (exclusive_page->current_page != nullptr)
        exit (-22);

    // The upper part of the allocated size was stored in exclusive_pages which shall be zero since we did a rather small allocation.
    if (exclusive_page->exclusive_pages != 0)
        exit (-23);

    // The lower part of the allocated size was stored in next_object_offset.
    if (exclusive_page->next_object_offset != sizeof(Page) + PAGE_SIZE)
        exit (-24);

    // The page of our array pointer shall be our exclusive page.
    if (Page::get(array) != exclusive_page)
        exit (-25);

    // Free the array storage.
    if (!page->reclaim_array(Page::get(array)))
        exit (-26);

    // Our page shall have no more exclusive pages now.
    if (page->exclusive_pages != 0)
        exit (-27);
}

void test_heap()
{
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);
    auto r = Region::create_from_page(&*root_page);
    auto page = r.page;

    size_t u_start = 0;
    size_t* pu_previous = nullptr;
    size_t* pu = nullptr;
    auto pointers = 133157244;
    for (int i = 1; i <= pointers; i++) {
        pu = (size_t*)page->allocate_raw(sizeof(size_t), alignof(size_t));
        *pu = 0;
        if (i == 1) {
            u_start = (size_t)pu;
        } else {
            *pu_previous = (size_t)pu;
        }
        pu_previous = pu;
    }

    // Check the pointer chain
    auto counter = 0;
    auto pu_check = (size_t*)u_start;
    while (pu_check != nullptr) {
        counter += 1;
        pu_check = (size_t*)(*pu_check) ;
    }
    if (counter != pointers)
        exit (-28);

    // Walk the page chain
    auto extension_location = page->get_extension_page_location();
    // println!("Root extension page: {:X}", *extension_location as usize);
    page = *extension_location;
    auto page_counter = 0;
    while (page) {
        auto extension_page = *page->get_extension_page_location();
        page = extension_page;
        page_counter += 1;
    }
}

void test_heapbucket() {
    auto heap = Heap::create();
    auto pool = Pool::create(&heap);
    auto bucket = Bucket::get(pool);
    auto _page00 = bucket->allocate_page();
    auto _page01 = bucket->allocate_page();
    auto page02 = bucket->allocate_page();
    auto page03 = bucket->allocate_page();
    auto page04 = bucket->allocate_page();
    auto page05 = bucket->allocate_page();
    auto _page06 = bucket->allocate_page();
    auto _page07 = bucket->allocate_page();
    auto _page08 = bucket->allocate_page();
    auto _page09 = bucket->allocate_page();
    auto _page10 = bucket->allocate_page();
    auto _page11 = bucket->allocate_page();
    auto _page12 = bucket->allocate_page();
    auto _page13 = bucket->allocate_page();
    auto _page14 = bucket->allocate_page();
    auto _page15 = bucket->allocate_page();
    auto page16 = bucket->allocate_page();
    auto _page17 = bucket->allocate_page();
    auto page18 = bucket->allocate_page();
    auto _page19 = bucket->allocate_page();
    auto _page20 = bucket->allocate_page();
    auto _page21 = bucket->allocate_page();
    auto _page22 = bucket->allocate_page();
    auto _page23 = bucket->allocate_page();
    auto _page24 = bucket->allocate_page();
    auto _page25 = bucket->allocate_page();
    auto _page26 = bucket->allocate_page();
    auto _page27 = bucket->allocate_page();
    auto _page28 = bucket->allocate_page();
    auto _page29 = bucket->allocate_page();
    auto _page30 = bucket->allocate_page();
    auto _page31 = bucket->allocate_page();
    auto _page32 = bucket->allocate_page();
    auto _page33 = bucket->allocate_page();
    auto _page34 = bucket->allocate_page();
    auto _page35 = bucket->allocate_page();
    auto _page36 = bucket->allocate_page();
    auto _page37 = bucket->allocate_page();
    auto _page38 = bucket->allocate_page();
    auto _page39 = bucket->allocate_page();
    auto _page40 = bucket->allocate_page();
    auto _page41 = bucket->allocate_page();
    auto _page42 = bucket->allocate_page();
    auto _page43 = bucket->allocate_page();
    auto _page44 = bucket->allocate_page();
    auto _page45 = bucket->allocate_page();
    auto _page46 = bucket->allocate_page();
    auto _page47 = bucket->allocate_page();
    auto _page48 = bucket->allocate_page();
    auto _page49 = bucket->allocate_page();
    auto _page50 = bucket->allocate_page();
    auto _page51 = bucket->allocate_page();
    auto _page52 = bucket->allocate_page();
    auto _page53 = bucket->allocate_page();
    auto _page54 = bucket->allocate_page();
    auto _page55 = bucket->allocate_page();
    auto _page56 = bucket->allocate_page();
    auto _page57 = bucket->allocate_page();
    auto _page58 = bucket->allocate_page();
    auto _page59 = bucket->allocate_page();
    auto _page60 = bucket->allocate_page();
    auto _page61 = bucket->allocate_page();
    auto page62 = bucket->allocate_page();
    auto page63 = bucket->allocate_page();

    if (bucket->tag != Bucket::Heap)
        exit(-29);
    if (bucket->heap.map != 0)
        exit(-30);

    bucket->deallocate_page(&*page62);
    auto page62a = bucket->allocate_page();

    if (page62 != page62a)
        exit(-31);

    if (bucket->tag != Bucket::Heap)
        exit(-32);
    if (bucket->heap.map != 0)
        exit(-33);

    bucket->deallocate_page(&*page02);
    bucket->deallocate_page(&*page03);
    bucket->deallocate_page(&*page04);
    bucket->deallocate_page(&*page05);
    auto page02a = bucket->allocate_page();
    auto page03a = bucket->allocate_page();
    auto page04a = bucket->allocate_page();
    auto page05a = bucket->allocate_page();

    if (page02 != page02a)
        exit(-34);
    if (page03 != page03a)
        exit(-35);
    if (page04 != page04a)
        exit(-36);
    if (page05 != page05a)
        exit(-37);

    if (bucket->tag != Bucket::Heap)
        exit(-38);
    if (bucket->heap.map != 0)
        exit(-39);

    bucket->deallocate_page(&*page16);
    bucket->deallocate_page(&*page18);
    auto page16a = bucket->allocate_page();
    auto page18a = bucket->allocate_page();

    if (page16 != page16a)
        exit(-40);
    if (page18 != page18a)
        exit(-41);

    if (bucket->tag != Bucket::Heap)
        exit(-42);
    if (bucket->heap.map != 0)
        exit(-43);

    bucket->deallocate_page(&*page63);
    auto page63a = bucket->allocate_page();
    if (page63 != page63a)
        exit(-44);

    if (bucket->tag != Bucket::Heap)
        exit(-38);
    if (bucket->heap.map != 0)
        exit(-39);
}

void test_region() {
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto r1 = Region::create_from_page(Page::get(root_stack_bucket));

    auto one = (int*)r1.allocate(sizeof(int), alignof(int));
    *one = 1;

    auto two = (int*)r1.allocate(sizeof(int), alignof(int));
    *two = 2;

    {
        auto r2a = Region::create(r1);

        auto three = (int*)r2a.allocate(sizeof(int), alignof(int));
        *three = 3;

        auto four = (int*)r2a.allocate(sizeof(int), alignof(int));
        *four = 4;

        auto five = (int*)r2a.allocate(sizeof(int), alignof(int));
        *five = 5;

        if  (*five != 5)
            exit(40);
        if  (*four != 4)
            exit(41);
        if  (*three != 3)
            exit(42);
    }
    {
        auto r2b = Region::create(r1);

        auto six = (int*)r2b.allocate(sizeof(int), alignof(int));
        *six = 6;

        auto seven = (int*)r2b.allocate(sizeof(int), alignof(int));
        *seven = 7;
    }

    if  (*two != 2)
        exit(43);
    if  (*one != 1)
        exit(44);
}

int main(int argc, char** argv)
{
    test_page();
    test_heap();
    test_heapbucket();
    test_region();
}
