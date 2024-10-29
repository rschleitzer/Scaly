extern "C" void exit(int);

const int PAGE_SIZE = 0x1000;

#include "Object.h"
#include "PageNode.h"
#include "PageListIterator.h"
#include "PageList.h"
#include "Page.h"

namespace scaly {
namespace memory {

Page* Object::get_page() {
    return Page::get(this);
}

void* Object::operator new(size_t size, size_t align, Page* page) {
    void* object = page->allocate(size, align);
    if (!object)
        exit(16);

    return object;
}

}

}