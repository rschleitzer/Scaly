#ifndef _PageListIterator_h
#define _PageListIterator_h
#include "../../scaly.h"
using namespace scaly::memory;

struct PageListIterator : Object {
    PageNode* current;
    PageListIterator(PageNode* head);
    Page** next();
};

#endif