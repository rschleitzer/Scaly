namespace scaly::memory {

struct StackBucket {
    Heap* heap;
    StackBucket* next_bucket;

    static StackBucket* create(Heap* heap);
    static Page* allocate_bucket();
    static Page* new_page(Page* page);
    Page* get_page_from_next_bucket();
    void deallocate();
};

struct HeapBucket{

};

struct Bucket : Object {
    enum { Stack, Heap } tag;
    union {
        StackBucket stack;
        HeapBucket heap;
    };
};

StackBucket* StackBucket::create(Heap* heap) {
    auto page = StackBucket::allocate_bucket();
    auto bucket = new(alignof(Bucket), page) Bucket {
        .tag = Bucket::Stack,
        .stack = StackBucket {
            .heap = heap,
            .next_bucket = nullptr
        }
    };

    return &(bucket->stack);
}

Page* StackBucket::allocate_bucket() {
    Page* page;
    posix_memalign((void**)&page, PAGE_SIZE * BUCKET_PAGES, PAGE_SIZE * BUCKET_PAGES);
    if (page == nullptr)
        exit(1);

    page->reset();
    return page;
}

Page* StackBucket::new_page(Page* page) {
    auto page_address = (size_t)page;
    auto stack_bucket_page_address = page_address & !(PAGE_SIZE * BUCKET_PAGES - 1);
    auto overflowed_page_address = stack_bucket_page_address + PAGE_SIZE * BUCKET_PAGES;
    auto our_page_address = page_address + PAGE_SIZE;
    if (overflowed_page_address == our_page_address) {
        auto bucket = (Bucket*)(stack_bucket_page_address + sizeof(Page));
        auto stack_bucket = bucket->stack;
        return stack_bucket.get_page_from_next_bucket();
    }
    else {
        auto our_page = (Page*)our_page_address;
        our_page->reset();
        return our_page;
    }
}

Page* StackBucket::get_page_from_next_bucket() {
    // println!("get_page_from_next_bucket");
    if (this->next_bucket == nullptr) {
        this->next_bucket = StackBucket::create(this->heap);
    }
    // println!("self.next_bucket: {:X}", self.next_bucket as usize);
    return Page::get(this->next_bucket);
}

void StackBucket::deallocate() {
    auto bucket = this;
    while (bucket->next_bucket) {
        auto next_bucket = bucket->next_bucket;
        auto page = Page::get(bucket);
        free(page);
        // println!("StackBucket: dealloc {:X}", page as usize);
        bucket = next_bucket;
    }
}

};
