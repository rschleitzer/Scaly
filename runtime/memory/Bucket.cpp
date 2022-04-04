namespace scaly::memory {

Bucket* Bucket::get(void* address) {
    auto mask = ~(size_t)(PAGE_SIZE * BUCKET_PAGES - 1);
    auto bucket_location = (size_t)address & mask;
    // The bucket is the first object of the first page of the bucket
    auto bucket_object_location = bucket_location + sizeof(Page);
    auto bucket = (Bucket*)bucket_object_location;
    return bucket;
}

Page* Bucket::allocate_page() {
    switch (this->tag) {
        case Bucket::Stack:
            return this->stack.allocate_page();
        case Bucket::Heap:
            return this->heap.allocate_page();
        default:
            return nullptr;
    }
}

void Bucket::deallocate_page(Page* page) {
    switch (this->tag) {
        case Bucket::Heap:
            this->heap.deallocate_page(page);
            return;
        case Bucket::Stack:
            // Tried to deallocate a page from a StackBucket."
            exit(5);
    }
}

size_t Bucket::find_least_position(size_t n) {
    switch (BUCKET_PAGES) {
        case 64: return Bucket::find_least_position_64(n);
        case 32: return Bucket::find_least_position_32(n);
        default:
            // Scaly currently does not support this architecture."
            exit(6);
    }
}

// from http://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
size_t Bucket::find_least_position_64(size_t n) {
    static size_t NUMBERS[64] = {
        0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20,
        55, 30, 34, 11, 43, 14, 22, 4, 62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10,
        13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63,
    };

    n = n | n >> 1;
    n = n | n >> 2;
    n = n | n >> 4;
    n = n | n >> 8;
    n = n | n >> 16;
    n = n | n >> 32;

    auto product = (__int128_t)n * 0x03f6eaf2cd271461;

    auto log = NUMBERS[((size_t)product) >> 58];
    return BUCKET_PAGES - log;
}

// from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
size_t Bucket::find_least_position_32(size_t n) {
    static size_t NUMBERS[32] = {
        0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24,
        7, 19, 27, 23, 6, 26, 5, 4, 31,
    };

    n = n | n >> 1;
    n = n | n >> 2;
    n = n | n >> 4;
    n = n | n >> 8;
    n = n | n >> 16;

    auto log = NUMBERS[(n * 0x07C4ACDD) >> 27];
    return BUCKET_PAGES - log;
}


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

Page* StackBucket::allocate_page() {
    return heap->allocate_page();
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
    auto stack_bucket_page_address = page_address & ~(PAGE_SIZE * BUCKET_PAGES - 1);
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

Page* HeapBucket::allocate_page() {
    if (this->map == 0)
        return this->pool->allocate_page();

    auto position = Bucket::find_least_position(this->map);
    auto bit = ((size_t)1) << (BUCKET_PAGES - position);
    this->map = this->map & ~bit;
    if (this->map == 0)
        this->pool->mark_as_full(Page::get(this));

    auto page = (Page*)(((size_t)Page::get(this)) + (position - 1) * PAGE_SIZE);

    // The first page of the bucket was already initialized by the pool.
    if (position > 1)
        page->reset();

    return page;
}

void HeapBucket::deallocate_page(Page* page) {
    auto base_page = Page::get(this);
    auto distance = ((size_t)page) - ((size_t)base_page);
    auto index = distance / PAGE_SIZE;
    if (index > BUCKET_PAGES - 1)
        // Position invalid for page
        exit(9);
    
    auto position = index + 1;
    auto bit = ((size_t)1) << (BUCKET_PAGES - position);
    if ((this->map & bit) != 0)
        // Page was not allocated
        exit(10);

    if (this->map == 0)
        this->pool->mark_as_free(base_page);

    this->map = this->map | bit;
}

};
