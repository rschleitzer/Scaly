#include "Scaly.h"
namespace scaly{

_Chunk* _Chunk::create() {
    _Page* page = 0;
    posix_memalign((void**)&page, _pageSize, _pageSize * numberOfPages);
    if (!page)
        return 0;
    page->reset();
    new(page) _Chunk();
}

_Chunk::_Chunk() {
    chunkBase = (char*) getPage();
    size_t numberOfBytesInMap = numberOfPagesInBucket * sizeof(size_t);
    allocationMap = (size_t*)getPage()->allocateObject(numberOfBytesInMap);
    memset(allocationMap, 0, numberOfBytesInMap);
    // for our own page
    allocationMap[0] = 1;
    allocationIndex = 0;
 }

 bool _Chunk::isEmpty() {
    if (allocationMap[0] != 1)
        return false;
    for (int i = 1; i < 8 * sizeof(size_t); i++)
        if (allocationMap[i])
            return false;

    return true;
 }

_Page* _Chunk::allocatePage() {
    // Look up the page in the page index
    if (allocationIndex == 0xFFFFFFFFFFFFFFFF)
        return 0;

    size_t bucket = findLowestZeroBit64(allocationIndex);
    // Get the map of the bucket
    size_t bucketMap = allocationMap[bucket];
    // Look up the page in the bucket map
    size_t pagePositionInBucket = findLowestZeroBit64(bucketMap);
    size_t pageIndex = bucket * numberOfPagesInBucket + pagePositionInBucket;
    if (pageIndex == numberOfPages)
        return 0;

    // Set the allocation bit in the map
    allocationMap[bucket] |= (size_t)1 << pagePositionInBucket;
    if (allocationMap[bucket] == 0xFFFFFFFFFFFFFFFF)
        allocationIndex |= (size_t)1 << bucket;
    size_t chunkBaseOffset = pageIndex * _pageSize;
    char* page = chunkBase + chunkBaseOffset;
    return (_Page*)page; }

size_t _Chunk::findLowestZeroBit64(size_t index) {
    size_t bucket = 0;
    size_t lowerHalf = index & 0xFFFFFFFF;
    if (lowerHalf == 0xFFFFFFFF) {
        // The lower half part is full so we have to look in the upper part
        bucket = 32;
        bucket += findLowestZeroBit32(index >> 32); }
    else {
        bucket = findLowestZeroBit32(lowerHalf); }
    return bucket; }

size_t _Chunk::findLowestZeroBit32(size_t index) {
    if ((index & 0xFFFF) != 0xFFFF) {
        index &= 0xFFFF;
        if ((index & 0xFF) < 0xFF) {
            if (!(index & 0x0001))  return 0;
            if (!(index & 0x0002))  return 1;
            if (!(index & 0x0004))  return 2;
            if (!(index & 0x0008))  return 3;
            if (!(index & 0x0010))  return 4;
            if (!(index & 0x0020))  return 5;
            if (!(index & 0x0040))  return 6;
                                    return 7; }
        else {
            if (!(index & 0x0100)) return  8;
            if (!(index & 0x0200)) return  9;
            if (!(index & 0x0400)) return 10;
            if (!(index & 0x0800)) return 11;
            if (!(index & 0x1000)) return 12;
            if (!(index & 0x2000)) return 13;
            if (!(index & 0x4000)) return 14;
                                   return 15; } }
    else {
        index >>= 16;
        if ((index & 0xFF) < 0xFF) {
            if (!(index & 0x0001)) return 16;
            if (!(index & 0x0002)) return 17;
            if (!(index & 0x0004)) return 18;
            if (!(index & 0x0008)) return 19;
            if (!(index & 0x0010)) return 20;
            if (!(index & 0x0020)) return 21;
            if (!(index & 0x0040)) return 22;
                                   return 23; }
        else {
            if (!(index & 0x0100)) return 24;
            if (!(index & 0x0200)) return 25;
            if (!(index & 0x0400)) return 26;
            if (!(index & 0x0800)) return 27;
            if (!(index & 0x1000)) return 28;
            if (!(index & 0x2000)) return 29;
            if (!(index & 0x4000)) return 30;
                                   return 31; } } }

bool _Chunk::deallocatePage(_Page* page) {
    // Check whether this page is from us
    if (((char*)page < chunkBase) || (char*)page >= (chunkBase + _pageSize * numberOfPages))
        return false;

    size_t pageIndex = ((char*)page - chunkBase) / _pageSize;
    size_t pagePositionInBucket = (pageIndex & ~(numberOfPagesInBucket)) % numberOfPagesInBucket;
    size_t bucket = pageIndex / numberOfPagesInBucket;
    // Clear the allocation bit in the map
    allocationMap[bucket] &= ~((size_t)1 << pagePositionInBucket);
    if (allocationMap[bucket] < 0xFFFFFFFFFFFFFFFF)
        allocationIndex &= ~((size_t)1 << bucket);
    return true;
}

void _Chunk::dispose() {
    free(chunkBase); }
}