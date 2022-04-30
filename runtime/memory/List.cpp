namespace scaly::containers {

using namespace scaly::memory;

template<class T> struct List;

template<class T> struct Node : Object {
    T element;
    Node<T>* next;
};

template<class T> struct ListIterator {
    Node<T>* current;

    static ListIterator<T> create(Node<T>* head) {
        return ListIterator { .current = head };
    }

    T* next() {
        if (this->current != nullptr) {
            auto current = this->current;
            this->current = this->current->next;
            return &current->element;
        } else {
            return nullptr;
        }
    }
};

template<class T> struct List : Object {
    Node<T>* head;

    static List<T>* create(Page* _rp) {
        return new(alignof(List<T>), _rp) List<T> {
             .head = nullptr
        };
    }

    void add(Page* _rp, T element) {
        auto new_node = 
            new (alignof(Node<T>), _rp) Node<T> {
                .element = element,
                .next = this->head,
            };

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

    ListIterator<T> get_iterator()  {
        return ListIterator<T>::create(this->head);
    }
};

}