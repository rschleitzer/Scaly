namespace scaly::memory {

struct Region {
    Page* page;

    static Region create_from_page(Page* page) {
        auto our_page_address = StackBucket::new_page(page);
        //println!("our_page_address:{:X}", our_page_address as usize);
        return Region {
            .page = our_page_address
        };
    };

    static Region create(Region& region) {
        return Region::create_from_page(region.page);
    }

    void* allocate(size_t length, size_t align) {
        return this->page->allocate_raw(length, align);
    }

    ~Region() {
        page->deallocate_extensions();
    }
};

}