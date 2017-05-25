#include "scaly.h"
    
char* scaly_align(char* unalignedPointer) {
    if (!(((long long) unalignedPointer) & (scaly_alignment - 1)))
        return unalignedPointer;

    long long unalignedPointerAsNumber = (long long )unalignedPointer;
    unalignedPointerAsNumber += scaly_alignment;
    unalignedPointerAsNumber &= ~(scaly_alignment - 1);

    return (char*)unalignedPointerAsNumber;
}
