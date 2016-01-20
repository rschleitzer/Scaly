#include "Scaly.h"

namespace scaly {

void* Object::operator new(size_t size, _Page* page) {
    void* object = page->allocateObject(size);
    if (!object)
        throw *(new std::bad_alloc());

    return object;
}

_Page* Object::getPage() {
    return _Page::getPage(this);
}

}
