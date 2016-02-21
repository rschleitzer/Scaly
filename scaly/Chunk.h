#ifndef __Scaly__Chunk__
#define __Scaly__Chunk__
#include "Scaly.h"
namespace scaly {

class _Chunk : public Object {
public:
    _Chunk();
    _Page* allocatePage();
    void deallocatePage(_Page* page);
    void dispose();

private:
    const size_t numberOfPages = 8 * sizeof(size_t) * 4 * sizeof(size_t);
    char* chunkBase;
    char* chunkPointer;
};

}

#endif // __Scaly_Chunk__
