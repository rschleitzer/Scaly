#include "scaly.h"

scaly_Pool* scaly_Pool_new(scaly_Page* _page) {
    scaly_Pool* this = (scaly_Pool*) scaly_Page_allocateObject(_page, sizeof(scaly_Pool));

    this->chunks = scaly_Array_new(scaly_Page_getPage(this));

    return this;
}

scaly_Page* scaly_Pool_allocatePage(scaly_Pool* this) {
    size_t chunksLength = this->chunks->_size;
    for (int i = 0; i < chunksLength; i++) {
        scaly_Page* page = scaly_Chunk_allocatePage((scaly_Chunk*)scaly_Array_elementAt(this->chunks, i));
        if (page)
            return page;
    }

    scaly_Chunk* chunk = scaly_Chunk_create();
    if (!chunk)
        return 0;

    scaly_Array_push(this->chunks, chunk);
    return scaly_Chunk_allocatePage(chunk);
}

int scaly_Pool_deallocatePage(scaly_Pool* this, scaly_Page* page) {
    scaly_Chunk* chunk = scaly_Pool_getContainingChunk(this, page);
    if (!chunk)
        return 0;

    int deallocated = scaly_Chunk_deallocatePage(chunk, page);
    if (!deallocated)
        return 0;
    if (scaly_Chunk_isEmpty(chunk)) {
        scaly_Chunk_dispose(chunk);
        scaly_Array_remove(this->chunks, chunk);
    }
    return 1;
}

scaly_Chunk* scaly_Pool_getContainingChunk(scaly_Pool* this, scaly_Page* page) {
    size_t chunksLength = this->chunks->_size;
    for (int i = 0; i < chunksLength; i++) {
        scaly_Chunk* chunk = (scaly_Chunk*)scaly_Array_elementAt(this->chunks, i);
        scaly_Page* basePage = scaly_Page_getPage(chunk);
        scaly_Page* upperBound = (scaly_Page*)((char*)basePage + scaly_pageSize * scaly_numberOfPages);
        if ((page > basePage) && (page < upperBound))
            return chunk;
    }
    return 0;
}

void scaly_Pool_dispose(scaly_Pool* this) {
    size_t chunksLength = this->chunks->_size;
    for (int i = 0; i < chunksLength; i++)
        scaly_Chunk_dispose((scaly_Chunk*)scaly_Array_elementAt(this->chunks, i));
}

