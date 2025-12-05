#ifndef _PageList_h
#define _PageList_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct PageList : Object {
    PageNode* head;

    PageList(PageNode* head);
    PageList ();
    void add(Page* rp, Page* element);
    bool remove(Page* element);
    PageListIterator get_iterator();
};

#endif