#include "Scaly.h"
namespace scaly{

__thread _Page* __CurrentPage = 0;
extern __thread _Task* __CurrentTask;

_Region::_Region(){
    page = __CurrentPage;
    page = (_Page*)(((char*)page) + _pageSize);
    new(page) _Page();
    __CurrentPage = page; }

_Page* _Region::get() {
    return page; }

_Region::~_Region() {
    _Page* page = __CurrentPage;
    page->deallocateExtensions();
    page = (_Page*)(((char*)page) - _pageSize);
    __CurrentPage = page; }

}