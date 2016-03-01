#include "Scaly.h"
namespace scaly{

_Pool::_Pool() {
    chunk = new(getPage()) _Chunk(); }

_Page* _Pool::allocatePage() {
    _Page* page = chunk->allocatePage();
    if (!page)
        return 0;
    return page; }

bool _Pool::deallocatePage(_Page* page) {
    return chunk->deallocatePage(page); }

void _Pool::dispose() {
    chunk->dispose(); }

}

