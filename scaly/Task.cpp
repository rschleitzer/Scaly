#include "Scaly.h"
namespace scaly{

// Some statistics
size_t pagesAllocated;
size_t pagesDeallocated;

__thread _Task* __CurrentTask = 0;

_Task::_Task() {}

_Page* _Task::getExtensionPage() {
    _Page* page = allocatePage();
    if (!page)
        return 0;
    return page; }

_Page* _Task::allocatePage() {
    _Page* page = 0;
    posix_memalign((void**)&page, _pageSize, _pageSize);
    pagesAllocated++;
    return page; }

void _Task::releaseExtensionPage(_Page* page) {
    pagesDeallocated++;
    free(page);
}

void _Task::dispose() {}

}