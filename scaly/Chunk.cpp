#include "Scaly.h"
namespace scaly{

_Chunk::_Chunk() {
    posix_memalign((void**)&chunkBase, _pageSize, _pageSize * numberOfPages);
    chunkPointer = chunkBase;
 }

_Page* _Chunk::allocatePage() {
    if (chunkPointer > chunkBase + numberOfPages * _pageSize)
        return 0;
    _Page* ret = (_Page*)chunkPointer;
    chunkPointer += _pageSize;
    return ret;
}
void _Chunk::deallocatePage(_Page* page) {
}

void _Chunk::dispose() {
    free(chunkBase); }

}