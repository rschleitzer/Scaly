#include "Scaly.h"
namespace scaly{

// Some statistics
static int pagesAllocated;
static int pagesDeallocated;

__thread _Task* __CurrentTask = 0;
    
_Task::_Task(){}

_Page* _Task::recycle() {
    void* pageLocation;
    
    if (posix_memalign(&pageLocation, _Page::pageSize, _Page::pageSize)) {
        return 0;
    }
    else {
        pagesAllocated++;
        return (_Page*)pageLocation;
    }
}

void _Task::dispose(_Page* page) {
    free(page);
    pagesDeallocated++;
}

_Task::~_Task() {
}

}