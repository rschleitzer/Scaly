#ifndef _Region_h
#define _Region_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Region : Object {
    Page* page;

    Region(Page* page);
    Region();
    ~Region();
    Page* get_page();
};

#endif