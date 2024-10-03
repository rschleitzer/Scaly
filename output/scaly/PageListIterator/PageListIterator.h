
using namespace scaly::memory;
struct PageListIterator : Object {
    PageNode* current;
    PageListIterator(PageNode* head);
};
