namespace scaly {
namespace memory {

struct Heap {
    size_t map;
    Pool* pools[BUCKET_PAGES];

    static Heap create() {
        return Heap {
            .map = (size_t)-1L,
            .pools = {},
        };
    };

    Page* allocate_page() {
        if (this->map == 0) {
            exit(2);
        }

        auto index = Bucket::find_least_position(this->map) - 1;
        if (this->pools[index] == nullptr) {
            this->pools[index] = Pool::create(this);
        }

        return this->pools[index]->allocate_page();
    }

    size_t get_allocation_position(Pool* pool) {
        for (int i = 0; i < BUCKET_PAGES; i++) {
            if (pool == this->pools[i]) {
                return i;
            }
        }
        // The pool pointer {:X} is not from the pools array
        exit(3);
    }

    size_t get_allocation_bit(Pool* pool) {
        return ((size_t)1) << (BUCKET_PAGES - 1 - this->get_allocation_position(pool));
    }

    void mark_as_full(Pool* pool) {
        auto bit = this->get_allocation_bit(pool);
        this->map = this->map & ~bit;
    }

    void mark_as_free(Pool* pool) {
        auto bit = this->get_allocation_bit(pool);
        this->map = this->map | bit;
    }

    void deallocate(Pool* pool) {
        auto position = this->get_allocation_position(pool);
        this->pools[position] = nullptr;
        pool->deallocate();
    }
};

}

}
