#include <stdlib.h>
#include <new>
#include <type_traits>
#include <limits>

using std::size_t;

const int PAGE_SIZE = 0x1000;

#include "Object.h"
#include "PageNode.h"

scaly::memory::PageNode::PageNode(Page* thePage, PageNode* nextNode)
    :   page(thePage),
        next(nextNode) {};
