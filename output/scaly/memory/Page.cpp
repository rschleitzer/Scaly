#include "../../scaly.h"
namespace scaly {
namespace memory {

void Page::deallocate_extensions() {
    if (this->next_object == nullptr) 
        return;
    
    auto _exclusive_page_iterator = this->exclusive_pages.get_iterator();
    while (auto _exclusive_page = _exclusive_page_iterator.next()) {
        auto exclusive_page = _*exclusive_page;{
            exclusive_page*.deallocate_extensions();
            exclusive_page*.forget();
        }
    };
    auto page = this->next_page;
    while (page != nullptr) {
        const auto next_page = page.next_page;
        page*.forget();
        page = next_page;
    };
}

Page* Page::get(void* address) {
    const auto mask = PAGE_SIZE-1as~;
    const auto page =  == addressas&maskaspointer;
    page;
}

void Page::reset() {
    this->current_page = nullptr;
    this->next_page = nullptr;
    this->next_object = this+1;
    this->exclusive_pages = List();
}

void Page::deallocate_exclusive_page(Page* page) {
    page*.deallocate_extensions()if (this->exclusive_pages->removepage == false) 
        exit2;
}

void* Page::allocate_raw(size_t size, size_t align) {
    const auto location = this->next_objectas;
    const auto aligned_location = (location+align-1)&((align-1)~);
    const auto location_after_page = this+PAGE_SIZEas;
    const auto capacity = location_after_page-aligned_location;
    if (capacity<size) {
        const auto gross_size = size+sizeof(Page);
        auto page = pointer;
        if (gross_size>PAGE_SIZE) 
            return this->allocate_oversizedgross_size;
        if (this->current_page != nullptr) {
            const auto object = this->current_page->allocate_raw(size, align);
            const auto page_of_allocated_object = Page.getobject;
            if (page_of_allocated_object != this->current_page) 
                this->current_page = page_of_allocated_object;
            return object;
        };
        auto page = this->allocate_page();
        this->current_page = page;
        this->next_page = page;
        return page.allocate_raw(size, align);
    };
    const auto next_location = aligned_location+size;
    this->next_object = next_locationas;
    aligned_locationas;
}

size_t Page::get_capacity(size_t align) {
    auto location = this->next_objectas;
    auto aligned_location = location+align-1&((align-1)~);
    auto location_after_page = thisas+PAGE_SIZE;
    auto capacity = location_after_page-aligned_location;
    capacity;
}

Page* Page::allocate_page() {
    auto page = aligned_alloc(PAGE_SIZE, PAGE_SIZE)as;
    page*.reset();
    page;
}

Page* Page::allocate_exclusive_page() {
    const auto page = this->allocate_page;
    this->exclusive_pages->add(page, page);
    page;
}

void Page::forget() {
    free(this&);
}

bool Page::is_oversized() {
    this->next_object == nullptr;
}

void* Page::allocate_oversized(size_t size) {
    const auto address = aligned_alloc(PAGE_SIZE, size)aspointer;
    if (address == nullptr) 
        exit1;
    autopage == addressaspointer;
    page.next_object = nullptr;
    this->exclusive_pages*.add(this&, page);
    page+1aspointer;
}

}
}