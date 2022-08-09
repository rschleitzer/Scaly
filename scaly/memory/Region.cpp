namespace scaly {
namespace memory {

struct Region {

    Region() : page(nullptr) {}

    ~Region() {
        if (page != nullptr) {
            page->deallocate_extensions();
            page->forget();
        }
    }

    Page* get_page() {
        if (page == nullptr)
            page = Page::allocate_page();
        return page;
    }

private:
    Region(const Region& region) {
    }
    Page* page;
};

}

}