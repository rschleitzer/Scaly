#ifndef __Scaly_Task__
#define __Scaly_Task__
#include "Scaly.h"
namespace scaly {

class _Task : public Object {
public:
    _Task();
    _Page* getStackPage();
    _Page* getExtensionPage();
    _Page* releaseStackPage();
    void releaseExtensionPage(_Page* page);
    void dispose();

private:
    _Page* allocatePage();
    void* extensionPagesBase;
    size_t lowestBucket;
    size_t highestBucket;
    _Array<size_t>* extensionPages;
};

}

#endif // __Scaly_Task__
