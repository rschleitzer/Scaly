namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct List;

template<class T> struct Node : Object {
    T element;
    Node<T>* next;
    Node(T _element, Node<T>* _next)
    :   element(_element),
        next(_next) {}
};

template<class T> struct ListIterator {
    Node<T>* current;

    ListIterator<T>(Node<T>* head) :current(head) {}

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

    List() : head(nullptr) {}
    static List<T>* create(Page* _rp) {
        return new(alignof(List<T>), _rp) List<T>();
    }

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

    ListIterator<T> get_iterator()  {
        return ListIterator<T>(this->head);
    }
};

}

}