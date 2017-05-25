#ifndef __Scaly__Array__
#define __Scaly__Array__
#include "scaly.h"

typedef struct scaly_Array scaly_Array; struct scaly_Array {
    size_t _size;
    size_t _capacity;
    void** _rawArray;
};

scaly_Array* new_scaly_Array();
scaly_Array* new_scaly_Array_With_Capacity(scaly_Page* _page, size_t capacity);
scaly_Array* new_scaly_Array_From_Array(scaly_Page* _page, scaly_Array* array);
void** scaly_Array_elementAt(scaly_Array* this, size_t i);
void scaly_Array_push(scaly_Array* this, void* item);
void scaly_Array_remove(scaly_Array* this, void* t);
void scaly_Array_reAllocate(scaly_Array* this, size_t newCapacity);
void scaly_Array_allocate(scaly_Array* this);

#endif//__Scaly_Array__
