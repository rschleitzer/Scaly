namespace scaly {

namespace memory {

using namespace scaly::containers;

const int PAGE_SIZE = 0x1000;
const size_t BUCKET_PAGES = sizeof(size_t) * 8;

struct Page {
    void* next_object;
    Page* current_page;
    Page* next_page;
    List<Page*> exclusive_pages;

    void reset() {
        this->current_page = nullptr;
        this->next_page = nullptr;
        this->next_object = this + 1;
        this->exclusive_pages = List<Page*>();
    }

    void clear() {
        this->deallocate_extensions();
        this->reset();
    }

    bool is_oversized() {
        return this->next_object == nullptr;
    }

    size_t get_capacity(size_t align)
    {
        auto location = (size_t)this->next_object;
        auto aligned_location = (location + align - 1) & ~(align - 1);
        auto location_after_page = (size_t)this + PAGE_SIZE;
        auto capacity = location_after_page - aligned_location;
        return capacity;
    }

    void* allocate_raw(size_t size, size_t align) {
        auto location = (size_t)this->next_object;
        auto aligned_location = (location + align - 1) & ~(align - 1);
        auto location_after_page = (size_t)this + PAGE_SIZE;
        auto capacity = location_after_page - aligned_location;

        if (capacity < size) {
            // We need to reserve the space for the page object
            auto gross_size = size + sizeof(Page);
            Page* page;
            if (gross_size > PAGE_SIZE)
                return allocate_oversized(gross_size);

            if (this->current_page != nullptr) {
                auto object = this->current_page->allocate_raw(size, align);
                auto page_of_allocated_object = Page::get(object);
                if (page_of_allocated_object != this->current_page)
                    this->current_page = page_of_allocated_object;
                return object;
            }

            page = this->allocate_page();
            this->current_page = page;
            this->next_page = page;
            return page->allocate_raw(size, align);
        }

        // Allocate from ourselves
        auto next_location = aligned_location + size;
        this->next_object = (void*)next_location;
        return (void*)aligned_location;
    }

    void* allocate_oversized(size_t size)
    {
        // We allocate oversized objects directly.
        auto page = (Page*)aligned_alloc(PAGE_SIZE, size);

        // Oversized pages have no next_object
        page->next_object = nullptr;

        // An oversized page is always exclusive
        this->exclusive_pages.add(this, page);

        // The page offset by the null pointer for next_object
        return (void*)(page + 1);
    }

    static Page* allocate_page() {
        // auto bucket = Bucket::get(this);
        auto page = (Page*)aligned_alloc(PAGE_SIZE, PAGE_SIZE);
        page->reset();
        return page;
    }

    Page* allocate_exclusive_page() {
        auto page = this->allocate_page();
        this->exclusive_pages.add(page, page);
        return page;
    }

    void deallocate_extensions() {
        auto i = this->exclusive_pages.get_iterator();
        while (auto exclusive_page = i.next())
        {
            // Oversized pages cannot have extnsions
            if ((*exclusive_page)->next_object == nullptr)
                continue;
            (*exclusive_page)->deallocate_extensions();
            (*exclusive_page)->forget();
        }

        if (this->next_object == nullptr)
            return;

        auto page = this->next_page; 
        while (page != nullptr)
        {
            auto next_page = page->next_page;
            page->forget();
            page = next_page;
        }
    }

    void forget() {
        free(this);
    }

    static Page* get(void* address) {
        auto mask = ~(size_t)(PAGE_SIZE - 1);
        auto page_raw = (void*)((size_t)address & mask);
        auto page = (Page*)page_raw;
        return page;
    }

    void deallocate_exclusive_page(Page* page) {
        page->deallocate_extensions();
        page->forget();
        if (!this->exclusive_pages.remove(page))
            exit(15);
    }
};

}

}