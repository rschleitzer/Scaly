
using namespace scaly::memory;
template<class T> struct List : Object {
    Node<T>* head;
    List();
};
