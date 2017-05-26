#include "scaly.h"

scaly_Chunk* scaly_Chunk_create() {

    // Allocate the raw memory of the chunk
    scaly_Page* page = 0;
    posix_memalign((void**)&page, scaly_pageSize, scaly_pageSize * scaly_numberOfPages);
    if (!page)
        return 0;

    // Our page is the first at the start of the chunk where we create the _Chunk object.
    scaly_Page_reset(page);
    return new_scaly_Chunk(page);
}

scaly_Chunk* new_scaly_Chunk(scaly_Page* page) {
    scaly_Chunk* this = (scaly_Chunk*) scaly_Page_allocateObject(page, sizeof(scaly_Chunk));

    // Allocate and initialize the allocation map which contains 4096 bits (512 bytes)
    size_t numberOfBytesInMap = scaly_numberOfPagesInBucket * sizeof(size_t);
    this->allocationMap = (size_t*)scaly_Page_allocateObject(scaly_Page_getPage(this), numberOfBytesInMap);
    memset(this->allocationMap, 0, numberOfBytesInMap);

    // Since we are sitting on the first page of the chunk, we have to mark it as already allocated
    this->allocationMap[0] = 1;

    // No bucket is full yet
    this->bucketMap = 0;
    
    return this;
 }

 int scaly_Chunk_isEmpty(scaly_Chunk* this) {

    // The first page of the first bucket is ourselves. Our own page could be abandoned.
    if (this->allocationMap[0] != 1)
        return 0;

    // All other buckets must be empty.
    for (int i = 1; i < 8 * sizeof(size_t); i++)
        if (this->allocationMap[i])
            return 0;

    return 1;
 }

scaly_Page* scaly_Chunk_allocatePage(scaly_Chunk* this) {

    // Check whether we are completely full
    if (this->bucketMap == 0xFFFFFFFFFFFFFFFF)
        return 0;

    // Find the lowest bucket which has still space available
    size_t bucket = scaly_Chunk_findLowestZeroBit64(this->bucketMap);

    // Find the lowest free page in the bucket
    size_t pagePositionInBucket = scaly_Chunk_findLowestZeroBit64(this->allocationMap[bucket]);

    // Figure out the position of the page in the chunk
    size_t pageIndex = bucket * scaly_numberOfPagesInBucket + pagePositionInBucket;

    // Set the allocation bit in the map
    this->allocationMap[bucket] |= (size_t)1 << pagePositionInBucket;
    
    // If the bucket is now full, mark it in the bucket map
    if (this->allocationMap[bucket] == 0xFFFFFFFFFFFFFFFF)
        this->bucketMap |= (size_t)1 << bucket;

    // Calculate the location of the page in memory
    size_t chunkBaseOffset = pageIndex * scaly_pageSize;
    char* page = (char*) scaly_Page_getPage(this) + chunkBaseOffset;

    // Return it as a pointer to our newly allocated page
    return (scaly_Page*)page;
}

size_t scaly_Chunk_findLowestZeroBit64(size_t map) {
    size_t bucket = 0;

    size_t lowerHalf = map & 0xFFFFFFFF;
    if (lowerHalf == 0xFFFFFFFF) {

        // The lower half is full so we have to look in the upper half
        bucket = scaly_Chunk_findLowestZeroBit32(map >> 32) + 32;
    }
    else {
        // Look in the lower half
        bucket = scaly_Chunk_findLowestZeroBit32(lowerHalf);
    }
    return bucket;
}

size_t scaly_Chunk_findLowestZeroBit32(size_t map) {
    if ((map & 0xFFFF) != 0xFFFF) {
        map &= 0xFFFF;
        if ((map & 0xFF) < 0xFF) {
            if (!(map & 0x0001))  return 0;
            if (!(map & 0x0002))  return 1;
            if (!(map & 0x0004))  return 2;
            if (!(map & 0x0008))  return 3;
            if (!(map & 0x0010))  return 4;
            if (!(map & 0x0020))  return 5;
            if (!(map & 0x0040))  return 6;
                                  return 7;
        }
        else {
            if (!(map & 0x0100)) return  8;
            if (!(map & 0x0200)) return  9;
            if (!(map & 0x0400)) return 10;
            if (!(map & 0x0800)) return 11;
            if (!(map & 0x1000)) return 12;
            if (!(map & 0x2000)) return 13;
            if (!(map & 0x4000)) return 14;
                                 return 15;
        }
    }
    else {
        map >>= 16;
        if ((map & 0xFF) < 0xFF) {
            if (!(map & 0x0001)) return 16;
            if (!(map & 0x0002)) return 17;
            if (!(map & 0x0004)) return 18;
            if (!(map & 0x0008)) return 19;
            if (!(map & 0x0010)) return 20;
            if (!(map & 0x0020)) return 21;
            if (!(map & 0x0040)) return 22;
                                 return 23;
        }
        else {
            if (!(map & 0x0100)) return 24;
            if (!(map & 0x0200)) return 25;
            if (!(map & 0x0400)) return 26;
            if (!(map & 0x0800)) return 27;
            if (!(map & 0x1000)) return 28;
            if (!(map & 0x2000)) return 29;
            if (!(map & 0x4000)) return 30;
                                 return 31;
        }
    }
}

int scaly_Chunk_deallocatePage(scaly_Chunk* this, scaly_Page* page) {

    // Check whether this page is from our chunk area
    if (((char*)page < (char*)scaly_Page_getPage(this)) || (char*)page >= ((char*)scaly_Page_getPage(this) + scaly_pageSize * scaly_numberOfPages))
        return 0;

    // Position of the page in the chunk
    size_t pageIndex = ((char*)page - (char*)scaly_Page_getPage(this)) / scaly_pageSize;
    
    // Position of the page in its bucket
    size_t pagePositionInBucket = (pageIndex & ~(scaly_numberOfPagesInBucket)) % scaly_numberOfPagesInBucket;

    // Bucket of the page
    size_t bucket = pageIndex / scaly_numberOfPagesInBucket;

    // Clear the allocation bit of the page in the allocation map
    this->allocationMap[bucket] &= ~((size_t)1 << pagePositionInBucket);

    // Clear the allocation bit in the bucket
    this->bucketMap &= ~((size_t)1 << bucket);

    return 1;
}

void scaly_Chunk_dispose(scaly_Chunk* this) {
    free(scaly_Page_getPage(this));
}