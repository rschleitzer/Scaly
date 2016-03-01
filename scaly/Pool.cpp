#include "Scaly.h"
namespace scaly{

_Pool::_Pool() {
    chunks = new(getPage()) _Array<_Chunk>(); }

_Page* _Pool::allocatePage() {
    size_t chunksLength = chunks->length();
    for (int i = 0; i < chunksLength; i++) {
        _Page* page = (*(*chunks)[i])->allocatePage();
        if (page)
            return page; }

    _Chunk* chunk = _Chunk::create();
    if (!chunk)
        return 0;
    chunks->push(chunk);
    return chunk->allocatePage(); }

bool _Pool::deallocatePage(_Page* page) {
    _Chunk* chunk = getContainingChunk(page);
    if (!chunk)
        return false;

    bool deallocated = chunk->deallocatePage(page);
    if (!deallocated)
        return false;
    if (chunk->isEmpty()) {
        chunk->dispose();
        chunks->remove(chunk);
    }
    return true; }

_Chunk* _Pool::getContainingChunk(_Page* page) {
    size_t chunksLength = chunks->length();
    for (int i = 0; i < chunksLength; i++) {
        _Chunk* chunk = *(*chunks)[i];
        _Page* basePage = chunk->getPage();
        _Page* upperBound = (_Page*)((char*)basePage + _pageSize * _Chunk::numberOfPages);
        if ((page > basePage) && (page < upperBound))
            return chunk;
    }
    return 0;
}

void _Pool::dispose() {
    size_t chunksLength = chunks->length();
    for (int i = 0; i < chunksLength; i++)
        (*(*chunks)[i])->dispose(); }
}

