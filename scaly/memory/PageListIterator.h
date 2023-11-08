namespace scaly {

namespace memory {

struct PageListIterator {
    PageNode* current;
    PageListIterator(PageNode* head);
    Page** next();
};

}

}