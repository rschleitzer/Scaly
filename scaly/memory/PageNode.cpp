namespace scaly {

namespace memory {

struct PageNode : Object {
    Page* element;
    PageNode* next;
    PageNode(Page* _element, PageNode* _next)
    :   element(_element),
        next(_next) {}
};

}

}