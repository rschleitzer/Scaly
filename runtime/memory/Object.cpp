namespace scaly::memory {

void* Object::operator new(size_t size, size_t align, Page* page) {
    void* object = page->allocate_raw(size, align);
    if (!object)
        throw *(new std::bad_alloc());

    return object;
}

Page* Object::get_page() {
    return Page::get(this);
}

}
