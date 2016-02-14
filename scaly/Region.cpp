#include "Scaly.h"
namespace scaly{

__thread _Page* __CurrentPage = 0;
    
_Region::_Region(){}

_Page* _Region::get() {
    _Page* nextPage = _Page::allocateNextPage(__CurrentPage);
    __CurrentPage = nextPage;
    return nextPage;
}

_Region::~_Region() {
    _Page* previousPage = __CurrentPage->previous();
    __CurrentPage->deallocatePageExtensions();
    _Page::forget(__CurrentPage);
    __CurrentPage = previousPage;
}

}