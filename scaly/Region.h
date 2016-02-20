#ifndef __Scaly_Region__
#define __Scaly_Region__
#include "Scaly.h"
namespace scaly {

class _Region {
public:
    _Region();
    _Page* get();
    ~_Region();
private:
    _Page* page;
};

}

#endif // __Scaly_Region__
