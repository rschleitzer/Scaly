#ifndef __scaly__Chunk__
#define __scaly__Chunk__
#include "scaly.h"

// 64 pages in a bucket
static const size_t scaly_numberOfPagesInBucket = 8 * sizeof(size_t);

// The whole chunk contains is 16 Meg large and contains 4096 pages in 64 buckets
static const size_t scaly_numberOfPages = 8 * sizeof(size_t) * 8 * sizeof(size_t);

typedef struct scaly_Chunk scaly_Chunk; struct scaly_Chunk {

    // A 512 bytes long map whose bits indicate which of our 4096 pages are currently allocated
    size_t* allocationMap;
    
    // 64 bits which indicate which buckets are completely full
    size_t bucketMap;
};

scaly_Chunk* scaly_Chunk_create();
scaly_Chunk* new_scaly_Chunk(scaly_Page* page);
scaly_Page* scaly_Chunk_allocatePage(scaly_Chunk* this);
int scaly_Chunk_deallocatePage(scaly_Chunk* this, scaly_Page* page);
int scaly_Chunk_isEmpty(scaly_Chunk* this);
void scaly_Chunk_dispose(scaly_Chunk* this);
static size_t scaly_Chunk_findLowestZeroBit64(size_t index);
static size_t scaly_Chunk_findLowestZeroBit32(size_t index);

#endif // __scaly_Chunk__
