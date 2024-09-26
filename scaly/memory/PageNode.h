namespace scaly {
namespace memory {
struct PageNode : Object {
    Page* page;
    PageNode* next;
    PageNode(Page* thePage, PageNode* nextNode);
};
}
}