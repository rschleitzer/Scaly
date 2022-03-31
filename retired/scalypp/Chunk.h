#ifndef __Scaly__Chunk__
#define __Scaly__Chunk__
#include "Scaly.h"
namespace scaly {

class _Chunk : public Object {
public:
    static _Chunk* create();
    _Chunk();
    _Page* allocatePage();
    bool deallocatePage(_Page* page);
    bool isEmpty();
    void dispose();
    
    // 64 pages in a bucket
    static const size_t numberOfPagesInBucket = 8 * sizeof(size_t);
    
    // The whole chunk contains is 16 Meg large and contains 4096 pages in 64 buckets
    static const size_t numberOfPages = numberOfPagesInBucket * numberOfPagesInBucket;

private:
    static size_t findLowestZeroBit64(size_t index);
    static size_t findLowestZeroBit32(size_t index);

    // A 512 bytes long map whose bits indicate which of our 4096 pages are currently allocated
    size_t* allocationMap;
    
    // 64 bits which indicate which buckets are completely full
    size_t bucketMap;
};

}

#endif // __Scaly_Chunk__
