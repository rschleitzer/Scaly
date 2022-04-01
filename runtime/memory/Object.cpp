namespace scaly::memory {

struct Object {

    void* operator new(size_t size, size_t align, Page* page) {
        void* object = page->allocate_raw(size, align);
        if (!object)
            throw *(new std::bad_alloc());

        return object;
    }

    Page* get_page() {
        return Page::get(this);
    }
};

}
