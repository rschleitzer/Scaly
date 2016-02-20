#include "Scaly.h"
namespace scaly{

// Some statistics

__thread _Task* __CurrentTask = 0;

_Task::_Task() {}

_Page* _Task::getExtensionPage() {
    _Page* page = allocatePage();
    if (!page)
        return 0;
    pagesAllocated++;
    return page; }

_Page* _Task::allocatePage() {
    _Page* page = 0;
    posix_memalign((void**)&page, _pageSize, _pageSize);
    return page; }

void _Task::releaseExtensionPage(_Page* page) {
    pagesDeallocated++;
    free(page);
}

void _Task::dispose() {}

}