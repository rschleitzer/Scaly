#ifndef __Scaly__Chunk__
#define __Scaly__Chunk__
#include "Scaly.h"
namespace scaly {

class _Chunk : public Object {
public:
    _Chunk();
    _Page* allocatePage();
    bool deallocatePage(_Page* page);
    void dispose();

private:
    static size_t findLowestZeroBit64(size_t index);
    static unsigned findLowestZeroBit32(unsigned index);

    const size_t numberOfPagesInBucket = 8 * sizeof(size_t);
    const size_t numberOfPages = numberOfPagesInBucket * numberOfPagesInBucket;
    char* chunkBase;
    size_t* allocationMap;
    size_t allocationIndex;
};

}

#endif // __Scaly_Chunk__
