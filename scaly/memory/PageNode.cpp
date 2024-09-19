#include "Object.h"
#include "PageNode.h"

scaly::memory::PageNode::PageNode(Page* thePage, PageNode* nextNode)
    :   page(thePage),
        next(nextNode) {};
