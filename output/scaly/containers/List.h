namespace scaly {
namespace containers {
using namespace scaly::memory;
template<class T> struct List : Object {
    Node<T>* head;
    List();
    T get_head() {};
    size_t count() {};
    bool remove(T element) {};
    ListIterator<T> get_iterator() {};
    void add(T element) {};
};
}
}