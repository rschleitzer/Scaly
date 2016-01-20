#include "Scaly.h"
namespace scaly {
    
char* align(char* unalignedPointer) {
    if (!(((long long) unalignedPointer) & (_alignment - 1)))
        return unalignedPointer;

    long long unalignedPointerAsNumber = (long long )unalignedPointer;
    unalignedPointerAsNumber += _alignment;
    unalignedPointerAsNumber &= ~(_alignment - 1);

    return (char*)unalignedPointerAsNumber;
}

}
