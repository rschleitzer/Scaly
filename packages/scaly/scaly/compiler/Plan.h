#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Plan : Object {
    String path;
    String name;

    Plan(String path, String name);
};

#endif