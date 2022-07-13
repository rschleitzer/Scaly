namespace scaly {
namespace memory {

struct Region {
    Page* page;

    Region() {
        page = Page::allocate_page();
    }

    ~Region() {
        page->deallocate_extensions();
        page->forget();
    }

    private: Region(const Region& region) {
    }
};

}

}