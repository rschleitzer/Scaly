#ifndef __Scaly_Containers__
#define __Scaly_Containers__

#include "../memory/Memory.cpp"
#include <cstring>

using std::memcpy;

namespace scaly::containers {
#include "List.cpp"
#include "String.cpp"
#include "Vector.cpp"
#include "Array.cpp"
#include "StringBuilder.cpp"
}

#endif // __Scaly_Containers__
