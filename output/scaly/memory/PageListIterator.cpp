#include "../../scaly.h"
namespace scaly {
namespace memory {

PageListIterator::PageListIterator(PageNode* current) : current(current) {}

Page** PageListIterator::next() {
    if (current != nullptr) {
        const auto previous_current = current;
        current = (*current).next;
        return &((*previous_current).page);
    }
    else return nullptr;
}

}
}