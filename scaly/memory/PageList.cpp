#include "Object.h"
#include "PageNode.h"
#include "PageListIterator.h"
#include "PageList.h"

namespace scaly {
namespace memory {

PageList::PageList() : head(nullptr) {}

Page** PageList::get_head() {
    if (this->head == nullptr)
        return nullptr;
    return &this->head->page;
}

size_t PageList::count() {
    auto list_iterator = this->get_iterator();
    size_t i = 0;
    while (auto item = list_iterator.next())
        i++;
    return i;
}

bool PageList::remove(Page* element) {
    auto node = this->head;
    PageNode* previous_node = nullptr;
    while (node != nullptr) {
        if (node->page == element) {
            if (previous_node != nullptr)
                previous_node->next = node->next;
            if (node == head)
                head = nullptr;
            return true;
        }
        previous_node = node;
        node = node->next;
    }

    return false;
}

PageListIterator PageList::get_iterator()  {
    return PageListIterator(this->head);
}

void PageList::add(Page* _rp, Page* element) {
    auto new_node = 
        new (alignof(PageNode), _rp) PageNode(element, this->head);

    this->head = new_node;
}

}

}