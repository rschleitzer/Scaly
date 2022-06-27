namespace scaly {
namespace memory {

struct Region {
    Page* page;

    Region(Heap& heap) {
        auto root_stack_bucket = StackBucket::create(&heap);
        page = Page::get(root_stack_bucket);
    }

    Region(const Region& region) {
        auto bucket = Bucket::get(region.page);
        switch (bucket->tag) {
            case Bucket::Heap:
                this->page = bucket->heap.allocate_page();
                break;
            case Bucket::Stack:
                auto our_page_address = StackBucket::new_page(region.page);
                this->page = our_page_address;
        }
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