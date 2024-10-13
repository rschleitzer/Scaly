#include "../../scaly.h"
namespace scaly {
namespace memory {

Region::Region() {};
Region::~Region() {};

Page* Region::get_page() {
    if (this->page == nullptr) 
        this->page = Page.allocate_page();
    return this->page;
}

}
}