namespace scaly::memory {

const int PAGE_SIZE = 0x1000;
const size_t BUCKET_PAGES = sizeof(size_t) * 8;

struct Page;

struct Allocator : Object {
    Page* current_page;
    Allocator* previous;
    Allocator* next;

    void* allocate(size_t size, size_t align);
    Page* allocate_page();
    void register_allocator(Allocator* allocator);
    void* allocate_oversized(size_t size);
    Page* allocate_exclusive_page();
    void deallocate();
    static void deallocate_page(Page* page);
};

struct Page {
    Allocator* allocator;
    void* next_object;

    void reset() {
        this->allocator = nullptr;
        this->next_object = this + 1;
    }

    bool is_oversized() {
        return this->next_object == nullptr;
    }

    void allocate_allocator()
    {
        auto location = (size_t)this->next_object;
        auto aligned_location = (location + alignof(Allocator) - 1) & ~(alignof(Allocator) - 1);
        auto location_after_page = (size_t)this + PAGE_SIZE;
        auto next_location = aligned_location + sizeof(Allocator);
        this->allocator = (Allocator*)aligned_location;
        this->next_object = (void*)next_location;
        this->allocator->current_page = nullptr;
        this->allocator->previous = nullptr;
        this->allocator->next = nullptr;
    }

    size_t get_capacity(size_t align)
    {
        auto location = (size_t)this->next_object;
        auto aligned_location = (location + align - 1) & ~(align - 1);
        auto location_after_page = (size_t)this + PAGE_SIZE;
        auto capacity = location_after_page - aligned_location;

        if (!this->allocator)
            capacity -= sizeof(Allocator);

        return capacity;
    }

    void* allocate_raw(size_t size, size_t align) {
        auto location = (size_t)this->next_object;
        auto aligned_location = (location + align - 1) & ~(align - 1);
        auto location_after_page = (size_t)this + PAGE_SIZE;
        auto capacity = location_after_page - aligned_location;

        if (!this->allocator)
            capacity -= sizeof(Allocator);

        if (capacity < size) {
            // We are too full for this size, so we delegate to the allocator
            if (this->allocator == nullptr)
                this->allocate_allocator();
            return this->allocator->allocate(size, align);
        }

        // Allocate from ourselves
        auto next_location = aligned_location + size;
        this->next_object = (void*)next_location;
        return (void*)aligned_location;
    }

    Page* allocate_page() {
        auto bucket = Bucket::get(this);
        // println!("Bucket: {:X}", bucket as usize);
        return bucket->allocate_page();
    }

    Page* allocate_exclusive_page() {
        if (this->allocator == nullptr)
            allocate_allocator();
        return allocator->allocate_exclusive_page();
    }

    void deallocate_extensions() {
        if (this->allocator == nullptr)
            return;

        allocator->deallocate();
    }

    void forget() {
        if (this->next_object == nullptr) {
            free (this);
        } else {
            auto bucket = Bucket::get(this);
            bucket->deallocate_page(this);
        }
    }

    static Page* get(void* address) {
        auto mask = ~(size_t)(PAGE_SIZE - 1);
        auto page_raw = (void*)((size_t)address & mask);
        auto page = (Page*)page_raw;
        return page;
    }

    void deallocate_exclusive_page(Page* page) {        
        this->allocator->deallocate_page(page);
    }
};

void* Allocator::allocate(size_t size, size_t align)
{
    auto gross_size = size + sizeof(Page) + sizeof(Allocator);
    Page* page;
    if (gross_size > PAGE_SIZE)
        return allocate_oversized(gross_size);

    if (this->current_page != nullptr) {
        auto object = this->current_page->allocate_raw(size, align);
        auto page_of_allocated_object = Page::get(object);
        if (page_of_allocated_object != this->current_page)
        {
            this->current_page = page_of_allocated_object;
            auto allocator = (Allocator*)(page_of_allocated_object + 1);
            this->register_allocator(allocator);
        }
        return object;
    }

    page = this->allocate_page();
    this->current_page = page;
    return page->allocate_raw(size, align);
}


Page* Allocator::allocate_page()
{
    auto page = Bucket::get(this)->allocate_page();
    page->allocator = new(alignof(Allocator), page) Allocator {
    };
    return page;
}

void Allocator::register_allocator(Allocator* allocator)
{
    allocator->previous = this;
    if (this->next != nullptr) {
        this->next->previous = allocator;
        allocator->next = this->next;
    }
    this->next = allocator;
}


void* Allocator::allocate_oversized(size_t size)
{
    // We allocate oversized objects directly.
    Page* page;
    posix_memalign((void**)&page, PAGE_SIZE, size);

    // Oversized pages have no current_page
    page->next_object = nullptr;

    // Set the size since we will need it when deallocating
    page->allocator = (Allocator*)(size);

    // Create an allocator at the start of the page
    auto allocator = (Allocator*)(page + 1);
    allocator->current_page = nullptr;

    allocator = (Allocator*)(page + 1);
    this->register_allocator(allocator);
    return allocator + 1;
}

Page* Allocator::allocate_exclusive_page()
{
    auto page = this->allocate_page();
    this->register_allocator(page->allocator);
    return page;
}

void Allocator::deallocate()
{
    auto allocator = this; 
    while (true)
    {
        auto next_allocator = allocator->next;
        Page::get(allocator)->forget();
        if (next_allocator == nullptr)
            break;
        else
            allocator = next_allocator;
    }
}

void Allocator::deallocate_page(Page* page)
{
    Allocator* allocator;
    if (page->next_object == nullptr)
        allocator = (Allocator*)(page + 1);
    else
        allocator = page->allocator;

    if (allocator->previous != nullptr)
        allocator->previous->next = allocator->next;

    if (allocator->next != nullptr)
        allocator->next->previous = allocator->previous;
}

}
