namespace scaly {

namespace memory {

PageListIterator::PageListIterator(PageNode* head) :current(head) {}

Page** PageListIterator::next() {
    if (this->current != nullptr) {
        auto current = this->current;
        this->current = this->current->next;
        return &current->page;
    } else {
        return nullptr;
    }
}

}

}