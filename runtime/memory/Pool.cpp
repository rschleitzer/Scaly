namespace scaly {
namespace memory {

Pool::Pool(Heap* _heap)
:   map(std::numeric_limits<size_t>::max()),
    heap(_heap) {}
            
Pool* Pool::create(Heap* heap) {
    void* memory;
    posix_memalign(&memory, PAGE_SIZE * BUCKET_PAGES, PAGE_SIZE * BUCKET_PAGES * BUCKET_PAGES);

    if (memory == nullptr)
        // Unable to create pool: Out of memory.
        exit(7);

    Pool* pool = nullptr;

    for (int i = 0; i < BUCKET_PAGES; i++) {
        auto bucket_page = (Page*)((size_t)memory + PAGE_SIZE * BUCKET_PAGES * i);
        bucket_page->reset();
        auto bucket = new (alignof(Bucket), bucket_page) Bucket (
            HeapBucket {
                .pool = nullptr,
                // We mark the first page of the bucket as full because it cannot be reset.
                .map = 0x7FFFFFFFFFFFFFFF,
            }
        );

        if (i == 0) {
            pool = new (alignof(Pool), bucket_page) Pool(heap);
        }

        bucket->heap.pool = pool;
    }
    return pool;
}

Page* Pool::allocate_page() {
    if (this->map == 0)
        return this->heap->allocate_page();

    auto pool_page_address = (size_t)Page::get(this);
    auto position = Bucket::find_least_position(this->map);
    auto bucket_page_address = pool_page_address + PAGE_SIZE * BUCKET_PAGES * (position - 1);
    auto bucket = (Bucket*)(bucket_page_address + sizeof(Page));
    return bucket->allocate_page();
}

size_t Pool::get_allocation_bit(Page* page) {
    auto first_page_of_bucket = Page::get(this);
    auto first_bucket_address = (size_t)first_page_of_bucket;
    auto distance = ((size_t)page) - first_bucket_address;
    auto position = distance / PAGE_SIZE / BUCKET_PAGES;
    return ((size_t)1 << (BUCKET_PAGES - 1 - position));
}

void Pool::mark_as_full(Page* page) {
    auto bit = this->get_allocation_bit(page);
    this->map = this->map & ~bit;
    if (this->map == 0)
        this->heap->mark_as_full(this);
}

void Pool::mark_as_free(Page* page) {
    auto bit = this->get_allocation_bit(page);
    if (this->map == 0)
        this->heap->mark_as_free(this);
    this->map = this->map | bit;
}

void Pool::deallocate() {
    if (this->map != std::numeric_limits<size_t>::max())
        // Pool is not empty.
        exit(8);

    free(Page::get(this));
}

}

}
