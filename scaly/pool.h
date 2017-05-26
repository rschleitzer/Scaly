#ifndef __scaly__pool__
#define __scaly__pool__
#include "scaly.h"

typedef struct scaly_Pool scaly_Pool; struct scaly_Pool {
    scaly_Array* chunks;
};

scaly_Pool* scaly_Pool_new(scaly_Page* _page);
scaly_Page* scaly_Pool_allocatePage(scaly_Pool* this);
int scaly_Pool_deallocatePage(scaly_Pool* this, scaly_Page* page);
void scaly_Pool_dispose(scaly_Pool* this);
scaly_Chunk* scaly_Pool_getContainingChunk(scaly_Pool* this, scaly_Page* page);

#endif // __Scaly__Pool__
