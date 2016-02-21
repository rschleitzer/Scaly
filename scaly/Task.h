#ifndef __Scaly_Task__
#define __Scaly_Task__
#include "Scaly.h"
namespace scaly {

class _Task : public Object {
public:
    _Task();
    _Page* getExtensionPage();
    _Page* releaseStackPage();
    void releaseExtensionPage(_Page* page);
    void dispose();

private:
    _Page* allocatePage();
};

}

#endif // __Scaly_Task__
