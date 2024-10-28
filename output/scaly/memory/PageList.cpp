#include "../../scaly.h"
namespace scaly {
namespace memory {

PageList::PageList(PageNode* head) : head(head) {}

PageList::PageList() : head(nullptr) {}

bool PageList::remove(Page* element) {
    auto node = head;
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

void PageList::add(Page* rp, Page* element) {
    const auto new_node = new (alignof(PageNode), rp) PageNode(element, head);
    head = new_node;
}

PageListIterator PageList::get_iterator() {
    return PageListIterator(head);
}

}
}