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
    static const size_t numberOfPagesInBucket = 8 * sizeof(size_t);
    static const size_t numberOfPages = numberOfPagesInBucket * numberOfPagesInBucket;

private:
    static size_t findLowestZeroBit64(size_t index);
    static size_t findLowestZeroBit32(size_t index);

    size_t* allocationMap;
    size_t allocationIndex;
};

}

#endif // __Scaly_Chunk__
