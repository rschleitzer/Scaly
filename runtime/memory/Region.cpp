namespace scaly::memory {

struct Region {
    Page* page;

    static Region create_from_page(Page* page) {
        auto bucket = Bucket::get(page);
        switch (bucket->tag) {
            case Bucket::Heap:
                return Region {
                    .page = bucket->heap.allocate_page(),
                };
            case Bucket::Stack:
                auto our_page_address = StackBucket::new_page(page);
                return Region {
                    .page = our_page_address
                };
        }
    };

    static Region create(Region& region) {
        return Region::create_from_page(region.page);
    }

    void* allocate(size_t length, size_t align) {
        return this->page->allocate_raw(length, align);
    }

    ~Region() {
        page->deallocate_extensions();
        auto bucket = Bucket::get(this->page);
        switch (bucket->tag) {
            case Bucket::Stack:
                break;
            case Bucket::Heap:
                bucket->heap.deallocate_page(this->page);
                break;
        }
    }
};

}