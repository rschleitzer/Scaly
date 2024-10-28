#ifndef _Node_h
#define _Node_h
#include "../../scaly.h"
using namespace scaly::memory;

template<class T>
struct Node : Object {
    T element;
    Node<T>* next;

    Node(T element, Node<T>* next) : element(element), next(next){
    }
};

#endif