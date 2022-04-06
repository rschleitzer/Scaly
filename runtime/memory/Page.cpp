namespace scaly::memory {

const int PAGE_SIZE = 0x1000;
const size_t BUCKET_PAGES = sizeof(size_t) * 8;

struct Page {
    Page* current_page;
    int next_object_offset;
    int exclusive_pages;
    void reset() {
        *(this->get_extension_page_location()) = nullptr;
        this->next_object_offset = sizeof(Page);
        this->exclusive_pages = 0;
        this->current_page = this;
    }

    bool is_oversized() {
        return this->current_page == nullptr;
    }

    Page** get_extension_page_location() {
        // Convert self to page location
        auto self_location = (char*)this;

        // Advance one page size so we are past the end of our page
        auto location_behind_page = (Page**)(self_location + PAGE_SIZE);

        // Go back one pointer size
        return location_behind_page - 1;
    }

    size_t get_next_location() {
        return ((size_t)this) + this->next_object_offset;
    }

    void set_next_location(size_t location) {
        auto self_location = (size_t)this;
        this->next_object_offset = (int)(location - self_location);
    }

    Page** get_next_exclusive_page_location() {
        // Move back one step for the extension page pointers and then by the number of exlusive pages.
        return this->get_extension_page_location() - 1 - this->exclusive_pages;
    }

    size_t get_capacity(size_t align) {
        auto location = this->get_next_location();
        auto aligned_location = (location + align - 1) & ~(align - 1);
        return (size_t)(this->get_next_exclusive_page_location()) - aligned_location;
    }

    void* allocate_raw(size_t size, size_t align) {
        if (this != this->current_page) {
            // We're already known to be full, so we delegate to the current page
            auto new_object = (this->current_page)->allocate_raw(size, align);

            // Possibly our current page was also full so we propagate back the new current page
            auto allocating_page = Page::get(new_object);
            if (allocating_page != this->current_page && (!(*allocating_page).is_oversized())) {
                this->current_page = allocating_page;
            }
            return new_object;
        }

        // Try to allocate from ourselves
        auto location = this->get_next_location();
        auto aligned_location = (location + (size_t)align - 1) & ~((size_t)align - 1);
        auto next_location = aligned_location + size;
        auto next_exclusive_page_location = this->get_next_exclusive_page_location();
        if (next_location <= (size_t)next_exclusive_page_location) {
            this->set_next_location(next_location);
            return (void*)location;
        }

        // So the space did not fit.

        // Calculate gross size to decide whether we're oversized
        auto gross_size = size + sizeof(Page);
        if (gross_size + sizeof(Page**) > PAGE_SIZE) {
            // Check whether we can allocate an exclusive page
            if (this->get_next_location() >= (size_t)(this->get_next_exclusive_page_location())) {
                // Allocate an extension page and try again with it
                return (this->allocate_extension_page())->allocate_raw(size, align);
            }

            auto page = create_oversized_page(size);
            return page + 1;
        }

        // So we're not oversized. Create extension page and let it allocate.
        return (this->allocate_extension_page())->allocate_raw(size, align);
    }

    Page* create_oversized_page(size_t size)
    {
        auto gross_size = size + sizeof(Page);

        // We allocate oversized objects directly.
        Page* page;
        posix_memalign((void**)&page, PAGE_SIZE, gross_size);

        // Oversized pages have no current_page
        page->current_page = nullptr;

        // Set the size since we will need it when deallocating
        page->next_object_offset = (int)(gross_size % 0x100000000);
        page->exclusive_pages = (int)(gross_size / 0x100000000);

        *(this->get_next_exclusive_page_location()) = page;
        this->exclusive_pages += 1;
        return page;
    }

    Page* allocate_extension_page() {
        auto page = this->allocate_page();
        // println!("Allocated extension page: {:X}", page as usize);
        *(this->get_extension_page_location()) = page;
        this->current_page = page;
        return page;
    }

    Page* allocate_page() {
        auto bucket = Bucket::get(this);
        // println!("Bucket: {:X}", bucket as usize);
        return bucket->allocate_page();
    }

    Page* allocate_exclusive_page() {
        if (this != this->current_page) {
            // We're already known to be full, so we delegate to the current page
            return this->current_page->allocate_exclusive_page();
        }

        // Check first whether we need an ordinary extension
        if (((size_t)this->get_next_location()) >= (size_t)(this->get_next_exclusive_page_location()))
        {
            // Allocate an extension page with default size
            return this->allocate_extension_page()->allocate_exclusive_page();
        }

        auto page = this->allocate_page();
        *(this->get_next_exclusive_page_location()) = page;
        this->exclusive_pages += 1;
        return page;
    }

    bool extend(size_t top, size_t size) {
        if (top != this->get_next_location()) {
            return false;
        }

        auto new_top = top + size;
        if (new_top > (size_t)this->get_next_exclusive_page_location()) {
            return false;
        }
        this->set_next_location(new_top);

        return true;
    }

    void deallocate_extensions() {
        auto page = this;
        while (page != nullptr) {
            auto extension_pointer = page->get_extension_page_location();
            for (int i = 1; i <= page->exclusive_pages; i++) {
                auto exclusive_page = *(extension_pointer - i);
                if (!exclusive_page->is_oversized()) {
                    exclusive_page->deallocate_extensions();
                }
                exclusive_page->forget();
            }

            if (page != this) {
                page->forget();
            }

            page = *extension_pointer;
        }
    }

    void forget() {
        if (this->current_page == nullptr) {
            free (this);
        } else {
            auto bucket = Bucket::get(this);
            bucket->deallocate_page(this);
        }
    }

    bool reclaim_array(Page* address) {
        auto page = this;
        while (page != nullptr) {
            if (page->deallocate_exclusive_page(address)) {
                return true;
            }
            page = *(page->get_extension_page_location());
        }

        // If we arrive here, we have a memory leak.
        return false;
    }

    static Page* get(void* address) {
        auto mask = ~(size_t)(PAGE_SIZE - 1);
        auto page_raw = (void*)((size_t)address & mask);
        auto page = (Page*)page_raw;
        return page;
    }

    bool deallocate_exclusive_page(Page* page) {
        // Find the extension Page pointer
        auto extension_pointer = this->get_extension_page_location() - 1;
        auto next_extension_page_location = this->get_next_exclusive_page_location();
        while (extension_pointer > next_extension_page_location) {
            if (*extension_pointer == page) {
                break;
            }
            extension_pointer = extension_pointer - 1;
        }

        // Report if we could not find it
        if (extension_pointer == next_extension_page_location) {
            return false;
        }

        // Shift the remaining array one position up
        while (extension_pointer > next_extension_page_location) {
            auto pp_page = extension_pointer;
            *pp_page = *(pp_page - 1);
            extension_pointer = extension_pointer - 1;
        }

        // Make room for one more extension
        this->exclusive_pages -= 1;
            page->forget();

        return true;
    }
};

}
