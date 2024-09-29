namespace scaly {
namespace memory {
using namespace scaly::memory;
struct PageNode : Object {
    Page* page;
    PageNode* next;
    PageNode(Page* thePage, PageNode* nextNode);
};
}
}