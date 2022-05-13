#ifndef __Scaly_Memory__
#define __Scaly_Memory__

#include <stdlib.h>
#include <new>
#include <type_traits>
#include <limits>

using std::size_t;
using std::alignment_of;

#include "Object.h"
#include "Pool.h"
#include "Bucket.h"
#include "List.cpp"
#include "Page.cpp"
#include "Object.cpp"
#include "Heap.cpp"
#include "Pool.cpp"
#include "Bucket.cpp"
#include "Region.cpp"

#endif // __Scaly_Memory__
