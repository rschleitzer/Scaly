#include "Object.h"
#include "PageNode.h"
#include "PageListIterator.h"
#include "PageList.h"
#include "Page.h"
#include "Region.h"

namespace scaly {
namespace memory {

Region::Region() : page(nullptr) {}

Region::~Region() {
    if (page != nullptr) {
        page->deallocate_extensions();
        page->forget();
    }
}

Page* Region::get_page() {
    if (page == nullptr)
        page = Page::allocate_page();
    return page;
}

}

}