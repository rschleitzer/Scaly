namespace scaly {
namespace containers {
using namespace scaly::memory;
template<class T> struct ListIterator : Object {
    Node<T>* current;
    ListIterator(Node<T>* head);
    void next() {};
};
}
}