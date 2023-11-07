#ifndef __Scaly_Memory__
#define __Scaly_Memory__

#include <stdlib.h>
#include <new>
#include <type_traits>
#include <limits>

using std::size_t;
using std::alignment_of;

const int PAGE_SIZE = 0x1000;

#include "Object.h"
#include "PageNode.cpp"
#include "PageListIterator.cpp"
#include "PageList.cpp"
#include "Page.cpp"
#include "Region.cpp"
#include "Object.cpp"

#endif // __Scaly_Memory__
