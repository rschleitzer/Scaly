#include "../../scaly.h"
namespace scaly {
namespace memory {

PageListIterator::PageListIterator(PageNode* head) {};

PageNode* PageListIterator::next() {
    if (this->current != nullptr) {
        const auto current = this->current;
        this->current = *(this->current).next;
        return current.element;
    }
    else {
        return nullptr;
    };
}

}
}