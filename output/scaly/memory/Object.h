#ifndef _Object_h
#define _Object_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Object {
    void* operator new(size_t size, size_t align, Page* page);
    Page* get_page();
};

#endif