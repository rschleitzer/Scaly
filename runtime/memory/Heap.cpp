namespace scaly::memory {

struct Heap {
    size_t map;
    Pool* pools[BUCKET_PAGES];

    static Heap create() {
        return Heap {
            .map = (size_t)-1L,
            .pools = {},
        };
    };
};

}
