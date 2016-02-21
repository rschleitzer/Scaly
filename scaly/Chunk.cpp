#include "Scaly.h"
namespace scaly{

_Chunk::_Chunk() {
    posix_memalign((void**)&chunkBase, _pageSize, _pageSize * numberOfPages);
    size_t numberOfBytesInMap = numberOfPagesInBucket * sizeof(size_t);
    allocationMap = (size_t*)getPage()->allocateObject(numberOfBytesInMap);
    memset(allocationMap, 0, numberOfBytesInMap);
    allocationIndex = 0;
 }

_Page* _Chunk::allocatePage() {
    // Look up the page in the page index
    size_t bucket = 0;
    bucket = findLowestZeroBit64(allocationIndex);
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
    char* ret = chunkBase + chunkBaseOffset;
    return (_Page*)ret; }

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

unsigned _Chunk::findLowestZeroBit32(unsigned index) {
    unsigned bucket = 0;
    unsigned short lowerHalf = index & 0xFFFF;
    if (lowerHalf == 0xFFFF) {
        // The lower half part is full so we have to look in the upper part
        bucket = 16;
        bucket += findLowestZeroBit16(index >> 16); }
    else {
        bucket = findLowestZeroBit16(lowerHalf); }
    return bucket; }

unsigned short _Chunk::findLowestZeroBit16(unsigned short index) {
    unsigned char bucket = 0;
    unsigned char lowerHalf = index & 0xFF;
    if (lowerHalf == 0xFF) {
        // The lower half part is full so we have to look in the upper part
        bucket = 8;
        bucket += findLowestZeroBit8(index >> 8); }
    else {
        bucket = findLowestZeroBit8(lowerHalf); }
    return bucket; }

unsigned char _Chunk::findLowestZeroBit8(unsigned char index) {
    if (!(index &  1)) return 0;
    if (!(index &  2)) return 1;
    if (!(index &  4)) return 2;
    if (!(index &  8)) return 3;
    if (!(index & 16)) return 4;
    if (!(index & 32)) return 5;
    if (!(index & 64)) return 6;
                else return 7; }

void _Chunk::deallocatePage(_Page* page) {
}

void _Chunk::dispose() {
    free(chunkBase); }
}