#ifndef _Test_h
#define _Test_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Test : Object {
    String path;
    String name;

    Test(String path, String name);
};

#endif