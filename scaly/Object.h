#ifndef __Scaly_Object__
#define __Scaly_Object__
#include "Scaly.h"
namespace scaly {

class Object {
public:
    void* operator new(size_t size, _Page* page);
    _Page* _getPage();
};

}

#endif//__Scaly_Object__
