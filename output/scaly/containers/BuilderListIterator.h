#ifndef _BuilderListIterator_h
#define _BuilderListIterator_h
#include "../../scaly.h"
using namespace scaly::memory;

template<class T>
struct BuilderListIterator : Object {
    Node<T>* current;

    BuilderListIterator(Node<T>* head) : current(head){
    }

    T* next() {
        if (current != nullptr) {
            const auto next_node = current;
            current = (*current).next;
            return &(*next_node).element;
        }
        else {
            return nullptr;
        };
    };
};

#endif