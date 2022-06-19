namespace scaly {
namespace memory {

struct Region {
    Page* page;

    Region(Page* page) {
        auto bucket = Bucket::get(page);
        switch (bucket->tag) {
            case Bucket::Heap:
                this->page = bucket->heap.allocate_page();
                break;
            case Bucket::Stack:
                auto our_page_address = StackBucket::new_page(page);
                this->page = our_page_address;
        }
    };

    static Region create(Region& region) {
        return Region(region.page);
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

}