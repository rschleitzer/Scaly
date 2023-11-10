namespace scaly {

namespace memory {

struct PageList : Object {
    PageNode* head;

    PageList() : head(nullptr) {}

    void add(Page* _rp, Page* element) {
        auto new_node = 
            new (alignof(PageNode), _rp) PageNode(element, this->head);

        this->head = new_node;
    }

    bool remove(Page* element) {
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

    Page** get_head() {
        if (this->head == nullptr)
            return nullptr;
        return &this->head->page;
    }

    size_t count() {
        auto list_iterator = this->get_iterator();
        size_t i = 0;
        while (auto item = list_iterator.next())
            i++;
        return i;
    }

    PageListIterator get_iterator()  {
        return PageListIterator(this->head);
    }
};

}

}