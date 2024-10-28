#ifndef _List_h
#define _List_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class T>
struct List : Object {
    Node<T>* head;

    List(Node<T>* head) : head(head) {}

    List() : head(nullptr) {}

    T get_head() {
        if (head == nullptr) 
            return nullptr;
        return &((*head).element);
    };

    size_t count() {
        auto list_iterator = get_iterator();
        size_t i = 0;
        while (list_iterator.next() != nullptr) i = i+1;
        return i;
    };

    bool remove(T element) {
        auto node = head;
        Node<T>* previous_node = nullptr;
        while (node != nullptr) {
            if ((*node).element == element) {
                if (previous_node != nullptr) 
                    (*previous_node).next = (*node).next;
                if (node == head) 
                    head = nullptr;
                return true;
            };
            previous_node = node;
            node = (*node).next;
        };
        return false;
    };

    ListIterator<T> get_iterator() {
        return ListIterator<T>(head); };

    void add(T element) {
        const auto lp = get_page();
        const auto new_node = new (alignof(Node<T>), lp) Node<T>(element, head);
        head = new_node;
    };
};

#endif