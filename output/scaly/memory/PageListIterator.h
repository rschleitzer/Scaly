namespace scaly {
namespace memory {
using namespace scaly::memory;
struct PageListIterator : Object {
    PageNode* current;
    PageListIterator(PageNode* head);
    PageNode next();
};
}
}