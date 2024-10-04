namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct BuilderList : Object {
    Node<T>* head;

    BuilderList() :head(nullptr) {}

    void add(Page* _rp, T element) {
        auto new_node = 
            new (alignof(Node<T>), _rp) Node<T>(element, this->head);

        this->head = new_node;
    }

    bool remove(T element) {
        auto node = this->head;
        Node<T>* previous_node = nullptr;
        while (node != nullptr) {
            if (node->element == element) {
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

    T* get_head() {
        if (this->head == nullptr)
            return nullptr;
        return &this->head->element;
    }

    size_t count() {
        auto list_iterator = this->get_iterator();
        size_t i = 0;
        while (auto item = list_iterator.next())
            i++;
        return i;
    }

    BuilderListIterator<T> get_iterator()  {
        return BuilderListIterator<T>(this->head);
    }
};

}

}