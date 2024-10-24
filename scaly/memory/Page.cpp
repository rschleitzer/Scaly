const int PAGE_SIZE = 0x1000;

#include "Object.h"
#include "PageNode.h"
#include "PageListIterator.h"
#include "PageList.h"
#include "Page.h"

extern "C" void* aligned_alloc(size_t, size_t);
extern "C" void  free(void*);
extern "C" void exit(int);

namespace scaly {

namespace memory {

void Page::deallocate_extensions() {
    // Oversized pages cannot have extensions
    if (this->next_object == nullptr)
        return;

    auto _exclusive_pages_iterator = this->exclusive_pages.get_iterator();
    while (auto _exclusive_page = _exclusive_pages_iterator.next()) {
        auto exclusive_page = *_exclusive_page;
        exclusive_page->deallocate_extensions();
        exclusive_page->forget();
    }

    auto page = this->next_page; 
    while (page != nullptr) {
        auto next_page = page->next_page;
        page->forget();
        page = next_page;
    }
}

Page* Page::get(void* address) {
    auto mask = ~(size_t)(PAGE_SIZE - 1);
    auto page = (Page*)((size_t)address & mask);
    return page;
}

void Page::reset() {
    this->current_page = nullptr;
    this->next_page = nullptr;
    this->next_object = this + 1;
    this->exclusive_pages = PageList();
}

void Page::deallocate_exclusive_page(Page* page) {
    page->deallocate_extensions();
    page->forget();
    if (!this->exclusive_pages.remove(page))
        exit(2);
}

void* Page::allocate_raw(size_t size, size_t align) {
    auto location = (size_t)this->next_object;
    auto aligned_location = (location + align - 1) & ~(align - 1);
    auto location_after_page = (size_t)this + PAGE_SIZE;
    auto capacity = location_after_page - aligned_location;

    if (capacity < size) {
        // We need to reserve the space for the page object
        auto gross_size = size + sizeof(Page);
        if (gross_size > PAGE_SIZE)
            return allocate_oversized(gross_size);

        if (this->current_page != nullptr) {
            auto object = this->current_page->allocate_raw(size, align);
            auto page_of_allocated_object = Page::get(object);
            if (page_of_allocated_object != this->current_page)
                this->current_page = page_of_allocated_object;
            return object;
        }

        auto page = this->allocate_page();
        this->current_page = page;
        this->next_page = page;
        return page->allocate_raw(size, align);
    }

    // Allocate from ourselves
    auto next_location = aligned_location + size;
    this->next_object = (void*)next_location;
    return (void*)aligned_location;
}

size_t Page::get_capacity(size_t align) {
    auto location = (size_t)this->next_object;
    auto aligned_location = (location + align - 1) & ~(align - 1);
    auto location_after_page = (size_t)this + PAGE_SIZE;
    auto capacity = location_after_page - aligned_location;
    return capacity;
}

Page* Page::allocate_page() {
    auto page = (Page*)aligned_alloc(PAGE_SIZE, PAGE_SIZE);
    page->reset();
    return page;
}

Page* Page::allocate_exclusive_page() {
    auto page = this->allocate_page();
    this->exclusive_pages.add(page, page);
    return page;
}

void Page::forget() {
    free(this);
}

bool Page::is_oversized() {
    return this->next_object == nullptr;
}

void* Page::allocate_oversized(size_t size)
{
    // We allocate oversized objects directly.
    auto address = aligned_alloc(PAGE_SIZE, (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    if (address == nullptr)
        exit(1);
    auto page = (Page*)address;

    // Oversized pages have no next_object
    page->next_object = nullptr;

    // An oversized page is always exclusive
    this->exclusive_pages.add(this, page);

    // The page offset by the null pointer for next_object
    return (void*)(page + 1);
}

}

}