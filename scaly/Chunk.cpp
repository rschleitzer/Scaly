#include "Scaly.h"
namespace scaly{

_Chunk* _Chunk::create() {

    // Allocate the raw memory of the chunk
    _Page* page = 0;
    posix_memalign((void**)&page, _pageSize, _pageSize * numberOfPages);
    if (!page)
        return 0;

    // Our page is the first at the start of the chunk where we create the _Chunk object.
    page->reset();
    new(page) _Chunk();
}

_Chunk::_Chunk() {

    // Allocate and initialize the allocation map which contains 4096 bits (512 bytes)
    size_t numberOfBytesInMap = numberOfPagesInBucket * sizeof(size_t);
    allocationMap = (size_t*)_getPage()->allocateObject(numberOfBytesInMap);
    memset(allocationMap, 0, numberOfBytesInMap);

    // Since we are sitting on the first page of the chunk, we have to mark it as already allocated
    allocationMap[0] = 1;

    // No bucket is full yet
    bucketMap = 0;
 }

 bool _Chunk::isEmpty() {

    // The first page of the first bucket is ourselves. Our own page could be abandoned.
    if (allocationMap[0] != 1)
        return false;

    // All other buckets must be empty.
    for (int i = 1; i < 8 * sizeof(size_t); i++)
        if (allocationMap[i])
            return false;

    return true;
 }

_Page* _Chunk::allocatePage() {

    // Check whether we are completely full
    if (bucketMap == 0xFFFFFFFFFFFFFFFF)
        return 0;

    // Find the lowest bucket which has still space available
    size_t bucket = findLowestZeroBit64(bucketMap);

    // Find the lowest free page in the bucket
    size_t pagePositionInBucket = findLowestZeroBit64(allocationMap[bucket]);

    // Figure out the position of the page in the chunk
    size_t pageIndex = bucket * numberOfPagesInBucket + pagePositionInBucket;

    // Set the allocation bit in the map
    allocationMap[bucket] |= (size_t)1 << pagePositionInBucket;
    
    // If the bucket is now full, mark it in the bucket map
    if (allocationMap[bucket] == 0xFFFFFFFFFFFFFFFF)
        bucketMap |= (size_t)1 << bucket;

    // Calculate the location of the page in memory
    size_t chunkBaseOffset = pageIndex * _pageSize;
    char* page = (char*) _getPage() + chunkBaseOffset;

    // Return it as a pointer to our newly allocated page
    return (_Page*)page;
}

size_t _Chunk::findLowestZeroBit64(size_t map) {
    size_t bucket = 0;

    size_t lowerHalf = map & 0xFFFFFFFF;
    if (lowerHalf == 0xFFFFFFFF) {

        // The lower half is full so we have to look in the upper half
        bucket = findLowestZeroBit32(map >> 32) + 32;
    }
    else {
        // Look in the lower half
        bucket = findLowestZeroBit32(lowerHalf);
    }
    return bucket;
}

size_t _Chunk::findLowestZeroBit32(size_t map) {
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

bool _Chunk::deallocatePage(_Page* page) {

    // Check whether this page is from our chunk area
    if (((char*)page < (char*)_getPage()) || (char*)page >= ((char*)_getPage() + _pageSize * numberOfPages))
        return false;

    // Position of the page in the chunk
    size_t pageIndex = ((char*)page - (char*)_getPage()) / _pageSize;
    
    // Position of the page in its bucket
    size_t pagePositionInBucket = (pageIndex & ~(numberOfPagesInBucket)) % numberOfPagesInBucket;

    // Bucket of the page
    size_t bucket = pageIndex / numberOfPagesInBucket;

    // Clear the allocation bit of the page in the allocation map
    allocationMap[bucket] &= ~((size_t)1 << pagePositionInBucket);

    // Clear the allocation bit in the bucket
    bucketMap &= ~((size_t)1 << bucket);

    return true;
}

void _Chunk::dispose() {
    free(_getPage()); }
}