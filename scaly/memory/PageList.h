namespace scaly {

namespace memory {

struct PageList : Object {
    PageNode* head;
    PageList();
    void add(Page* _rp, Page* element);
    bool remove(Page* element);
    Page** get_head();
    size_t count();
    PageListIterator get_iterator();
};

}

}