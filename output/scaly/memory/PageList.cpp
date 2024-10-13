#include "../../scaly.h"
namespace scaly {
namespace memory {

PageList::PageList() : head(nullptr) {}

void PageList::add(Page* _rp, Page* element) {
    const auto new_node = new (alignof(PageNode), _rp) PageNode(element, this->head);
    this->head = new_node;
}

bool PageList::remove(Page* element) {
    auto node = this->head;
    PageNode* previous_node = nullptr;
    while (node != nullptr) {
        if ((*node).page == element) {
            if (previous_node != nullptr) 
                (*previous_node).next = (*node).next;
            if (node == head) 
                head = nullptr;
            return true;
        };
        previous_node = node;
        node = (*node).next;
    };
    return false;
}

PageListIterator PageList::get_iterator() {
    return PageListIterator(this->head);
}

}
}