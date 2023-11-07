namespace scaly {

namespace memory {

struct PageListIterator {
    PageNode* current;

    PageListIterator(PageNode* head) :current(head) {}

    Page** next() {
        if (this->current != nullptr) {
            auto current = this->current;
            this->current = this->current->next;
            return &current->element;
        } else {
            return nullptr;
        }
    }
};

}

}