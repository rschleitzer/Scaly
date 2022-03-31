#include "Scaly.h"
namespace scaly{

__thread _Page* __CurrentPage = 0;

_Region::_Region(){
    __CurrentPage = (_Page*)(((char*)__CurrentPage) + _pageSize);
    __CurrentPage->reset();
}

_Page* _Region::get() {
    return __CurrentPage;
}

_Region::~_Region() {
    __CurrentPage->deallocateExtensions();
    __CurrentPage = (_Page*)(((char*)__CurrentPage) - _pageSize);
}

}