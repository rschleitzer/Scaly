#ifndef _Console_h
#define _Console_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
using namespace scaly::containers;
namespace Console {

    void print(const_char* message);
    void print(String message);
}
#endif