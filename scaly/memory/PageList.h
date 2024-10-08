namespace scaly {

namespace memory {

struct PageList : Object {
    PageNode* head;
    PageList();
    Page** get_head();
    void add(Page* _rp, Page* page);
    bool remove(Page* page);
    size_t count();
    PageListIterator get_iterator();
};

}

}