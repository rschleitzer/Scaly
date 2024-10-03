
using namespace scaly::memory;
template<class T> struct Node : Object {
    T element;
    Node<T>* next;
};
