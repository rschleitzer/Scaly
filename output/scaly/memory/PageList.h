namespace scaly {
namespace memory {
using namespace scaly::memory;
struct PageList : Object {
    PageNode* head;
    PageList();
    Page get_head();
    size_t count();
    bool remove(Page element);
    PageListIterator get_iterator();
    void add(Page* element);
};
}
}