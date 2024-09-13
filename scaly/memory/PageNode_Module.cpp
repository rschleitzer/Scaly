#ifndef __Scaly_Memory__
#define __Scaly_Memory__

#include <stdlib.h>
#include <new>
#include <type_traits>
#include <limits>

using std::size_t;

const int PAGE_SIZE = 0x1000;

#include "Object.h"
#include "PageNode.h"
#include "PageNode.cpp"

#endif // __Scaly_Memory__
