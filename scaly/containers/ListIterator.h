namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct List;

template<class T> struct ListIterator : Object {
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

}

}