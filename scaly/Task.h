#ifndef __Scaly_Task__
#define __Scaly_Task__
#include "Scaly.h"
namespace scaly {

class _Task {
public:
    _Task();
    _Page* recycle();
    void dispose(_Page* page);
    ~_Task();
};

}

#endif // __Scaly_Task__
