#ifndef _PageList_h
#define _PageList_h
#include "../../scaly.h"
using namespace scaly::memory;

struct PageList : Object {
    PageNode* head;
    PageList ();
    bool remove(Page* element);
    void add(Page* rp, Page* element);
    PageListIterator get_iterator();
};

#endif