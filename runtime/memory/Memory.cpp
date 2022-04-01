#ifndef __Scaly_Memory__
#define __Scaly_Memory__

#include <stdlib.h>
#include <new>
#include <type_traits>

using std::size_t;
using std::align_val_t;
using std::alignment_of;

#include "Page.cpp"
#include "Object.cpp"
#include "Pool.cpp"
#include "Heap.cpp"
#include "Bucket.cpp"
#include "Region.cpp"

#endif // __Scaly_Memory__
