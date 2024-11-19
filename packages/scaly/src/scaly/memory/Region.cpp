#include "../../scaly.h"
namespace scaly {
namespace memory {

Region::Region(Page* page) : page(page) {}

Region::Region() {
    page = nullptr;
}
Region::~Region() {
    if (page != nullptr) {
        (*page).deallocate_extensions();
        free(page);
    };
};

Page* Region::get_page() {
    if (page == nullptr) 
        page = Page::allocate_page();
    return page;
}

}
}