#include "../../scaly.h"
namespace scaly {
namespace memory {

Page::Page(void* next_object, Page* current_page, Page* next_page, PageList exclusive_pages) : next_object(next_object), current_page(current_page), next_page(next_page), exclusive_pages(exclusive_pages) {}

Page* Page::allocate_page() {
    auto page = (Page*)aligned_alloc(PAGE_SIZE, PAGE_SIZE);
    (*page).reset();
    return page;
}

void Page::reset() {
    current_page = nullptr;
    next_page = nullptr;
    next_object = this+1;
    exclusive_pages = PageList();
}

void* Page::allocate_raw(size_t size, size_t align) {
    const auto location = (size_t)next_object;
    const auto aligned_location = (location+align-1)&~(align-1);
    const auto location_after_page = (size_t)this+PAGE_SIZE;
    const auto capacity = location_after_page-aligned_location;
    if (capacity<size) {
        const auto gross_size = size+sizeof(Page);
        if (gross_size>PAGE_SIZE) 
            return allocate_oversized(gross_size);
        if (current_page != nullptr) {
            const auto object = (*current_page).allocate_raw(size, align);
            const auto page_of_allocated_object = get(object);
            if (page_of_allocated_object != current_page) 
                current_page = page_of_allocated_object;
            return object;
        };
        auto page = allocate_page();
        current_page = page;
        next_page = page;
        return (*page).allocate_raw(size, align);
    };
    const auto next_location = aligned_location+size;
    next_object = (void*)next_location;
    return (void*)aligned_location;
}

void Page::deallocate_extensions() {
    if (next_object == nullptr) 
        return;
    
    auto _exclusive_page_iterator = exclusive_pages.get_iterator();
    while (auto _exclusive_page = _exclusive_page_iterator.next()) {
        auto exclusive_page = *_exclusive_page;{
            (*exclusive_page).deallocate_extensions();
            (*exclusive_page).forget();
        }
    };
    auto page = next_page;
    while (page != nullptr) {
        const auto next_page = (*page).next_page;
        (*page).forget();
        page = next_page;
    };
}

Page* Page::get(void* address) {
    const auto mask = ~(size_t)(PAGE_SIZE-1);
    const auto page = (Page*)((size_t)address&mask);
    return page;
}

void Page::deallocate_exclusive_page(Page* page) {
    (*page).deallocate_extensions();
    (*page).forget();
    if ((exclusive_pages).remove(page) == false) 
        exit(2);
}

size_t Page::get_capacity(size_t align) {
    auto location = (size_t)next_object;
    const auto aligned_location = (location+align-1)&~(align-1);
    auto location_after_page = (size_t)this+PAGE_SIZE;
    auto capacity = location_after_page-aligned_location;
    return capacity;
}

Page* Page::allocate_exclusive_page() {
    const auto page = allocate_page();
    (exclusive_pages).add(page, page);
    return page;
}

void Page::forget() {
    free(this);
}

bool Page::is_oversized() {
    return next_object == nullptr;
}

void* Page::allocate_oversized(size_t size) {
    const auto address = aligned_alloc(PAGE_SIZE, (size+PAGE_SIZE-1)&~(PAGE_SIZE-1));
    if (address == nullptr) 
        exit(1);
    auto page = (Page*)address;
    (*page).next_object = nullptr;
    (exclusive_pages).add(this, page);
    return (void*)(page+1);
}

}
}