#ifndef __Scaly_Task__
#define __Scaly_Task__
#include "Scaly.h"
namespace scaly {

class _Task : public Object {
public:
    _Task();
    static _Page* allocatePage();
    _Page* recyclePage();
    void disposePage(_Page* page);
    void dispose();

    static const size_t maxPagePoolSize = 0x100;
    
private:
    _Array<_Page>& pagePool;
};

}

#endif // __Scaly_Task__
