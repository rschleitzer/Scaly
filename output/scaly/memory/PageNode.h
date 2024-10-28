#ifndef _PageNode_h
#define _PageNode_h
#include "../../scaly.h"
using namespace scaly::memory;

struct PageNode : Object {
    Page* page;
    PageNode* next;
    PageNode(Page* thePage, PageNode* nextNode);
};

#endif