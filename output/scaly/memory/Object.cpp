#include "../../scaly.h"
namespace scaly {
namespace memory {

Page* Object::get_page() {
    return Page::get(this);
}

void* Object::operator new(size_t size, size_t align, Page* page) {
    const auto object = (*page).allocate(size, align);
    if (object == nullptr) 
        exit(16);
    return object;
}

}
}