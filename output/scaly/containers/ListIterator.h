#ifndef _ListIterator_h
#define _ListIterator_h
#include "../../scaly.h"
using namespace scaly::memory;

template<class T>
struct ListIterator : Object {
    Node<T>* current;

    ListIterator(Node<T>* head) {
        current = head;
    }

    T* next() {
        if (current != nullptr) {
            auto old_current = (*this).current;
            current = (*current).next;
            return &((*old_current).element);
        }
        else {
            return nullptr;
        };
    };
};

#endif