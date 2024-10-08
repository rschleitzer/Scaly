namespace scaly {

namespace memory {

struct PageList : Object {
    PageNode* head;
    PageList();
    void add(Page* _rp, Page* page);
    bool remove(Page* page);
    PageListIterator get_iterator();
};

}

}