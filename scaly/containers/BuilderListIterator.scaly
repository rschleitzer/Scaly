namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct BuilderList;

template<class T> struct BuilderListIterator : Object {
    Node<T>* current;

    BuilderListIterator<T>(Node<T>* head) :current(head) {}

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