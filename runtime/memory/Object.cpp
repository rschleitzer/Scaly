namespace scaly {
namespace memory {

void* Object::operator new(size_t size, size_t align, Page* page) {
    void* object = page->allocate_raw(size, align);
    if (!object)
        exit(16);

    return object;
}

void* Object::operator new(size_t size, size_t align, Region& region) {
    auto page = region.get_page();
    void* object = page->allocate_raw(size, align);
    if (!object)
        exit(16);

    return object;
}

Page* Object::get_page() {
    return Page::get(this);
}

}

}