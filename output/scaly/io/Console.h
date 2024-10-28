#ifndef _Console_h
#define _Console_h
#include "../../scaly.h"
using namespace scaly::memory;
using namespace scaly::containers;
namespace Console {

    void print(Page* ep, const_char* message);
    void print(Page* ep, String message);
}
#endif