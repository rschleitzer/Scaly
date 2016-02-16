#include "Scaly.h"
namespace scaly{

// Some statistics
static int pagesAllocated;
static int pagesDeallocated;
static int pagesCached;
static int pagesFromCache;

__thread _Task* __CurrentTask = 0;
    
_Task::_Task()
: pagePool (*new(getPage()) _Array<_Page>(256)) {
}

_Page* _Task::allocatePage() {
    void* pageLocation;
    
    if (posix_memalign(&pageLocation, _Page::pageSize, _Page::pageSize)) {
        return 0;
    }
    else {
        pagesAllocated++;
        return (_Page*)pageLocation;
    }
}

_Page* _Task::recyclePage() {
    size_t length = pagePool.length();
    if (length) {
        pagesFromCache++;
        return pagePool.pop();
    }
    else {
        return allocatePage();
    }
}

void _Task::disposePage(_Page* page) {
    size_t length = pagePool.length();
    if (length >= maxPagePoolSize) {
        free(page);
        pagesDeallocated++;
    }
    else {
        pagesCached++;
        pagePool.push(page);
    }
}

void _Task::dispose() {

    // Free the page pool
    size_t _pagePool_length = pagePool.length();
    for (size_t _i = 0; _i < _pagePool_length; _i++) {
        free(pagePool.pop());
        pagesDeallocated++;
    }

    // Since our own page might have extended itself...
    getPage()->deallocatePageExtensions();
    
    // ...and the deallocated pages are pooled again
    _pagePool_length = pagePool.length();
    for (size_t _i = 0; _i < _pagePool_length; _i++) {
        free(*pagePool[_i]);
        pagesDeallocated++;
    }
    
    // Free our root page and that's it
    free(getPage());
    pagesDeallocated++;
}

}