#ifndef _BuilderList_h
#define _BuilderList_h
#include "../../scaly.h"
using namespace scaly::memory;

template<class T>
struct BuilderList : Object {
    Node<T>* head;

    BuilderList() : head(nullptr){
    }

    T* get_head() {
        if (head == nullptr) 
            return nullptr;
        return &(*head).element;
    };

    size_t count() {
        auto list_iterator = get_iterator();
        size_t i = 0;
        while (auto item = list_iterator.next()) i = i+1;
        return i;
    };

    bool remove(T element) {
        const auto node = head;
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
    };

    BuilderListIterator<T> get_iterator() {
        return BuilderListIterator<T>(head); };

    void add(Page* rp, T element) {
        const auto new_node = new (alignof(Node<T>), rp) Node<T>(element, head);
        head = new_node;
    };
};

#endif