#include "scaly.h"

scaly_Array* new_scaly_Array(scaly_Page* _page) {
    scaly_Array* this = (scaly_Array*) scaly_Page_allocateObject(_page, sizeof(scaly_Array));

    this->_size = 0;
    this->_capacity = 0;
    this->_rawArray = 0;

    return this;
}

scaly_Array* new_scaly_Array_With_Capacity(scaly_Page* _page, size_t capacity) {
    scaly_Array* this = (scaly_Array*) scaly_Page_allocateObject(_page, sizeof(scaly_Array));

    this->_size = 0;
    this->_capacity = capacity;
    scaly_Array_allocate(this);

    return this;
}

scaly_Array* new_scaly_Array_From_Array(scaly_Page* _page, scaly_Array* array) {
    scaly_Array* this = (scaly_Array*) scaly_Page_allocateObject(_page, sizeof(scaly_Array));

    this->_size = array->_size;
    this->_capacity = array->_size;
    scaly_Array_allocate(this);
    memcpy(this->_rawArray, array->_rawArray, this->_size * sizeof(void*));
    
    return this;
}
    
void** scaly_Array_elementAt(scaly_Array* this, size_t i) {
    if (i < this->_size)
        return this->_rawArray + i;

    return 0;
}


// Append a value to the array
void scaly_Array_push(scaly_Array* this, void* item) {
    if (!this->_rawArray) {
        this->_capacity = 1;
        this->_size = 0;
        scaly_Array_allocate(this);
    }
    else {
        if (this->_size == this->_capacity)
            scaly_Array_reAllocate(this, this->_capacity * 2);
    }

    *(this->_rawArray + this->_size) = item;
    this->_size += 1;
}

// Remove a value from the array
void scaly_Array_remove(scaly_Array* this, void* t) {
    for (size_t i = 0; i < this->_size; i++) {
        if (scaly_Array_elementAt(this, i) == t) {
            for (size_t j = i + 1; j < this->_size; j++)
                *(scaly_Array_elementAt(this, j - 1)) = *(scaly_Array_elementAt(this, j));
            this->_size--;
            return;
        }
    }
}


void scaly_Array_reAllocate(scaly_Array* this, size_t newCapacity) {
    void** oldArray = this->_rawArray;
    this->_capacity = newCapacity;
    scaly_Array_allocate(this);
    memcpy(this->_rawArray, oldArray, this->_size * sizeof(void*));

    // Reclaim the page if it was oversized, i.e., exclusively allocated
    if (scaly_Page_isOversized(scaly_Page_getPage(oldArray)))
        scaly_Page_reclaimArray(scaly_Page_getPage(this), oldArray);
}

void scaly_Array_allocate(scaly_Array* this) {
    this->_rawArray = (void**) scaly_Page_allocateObject(scaly_Page_getPage(this), this->_capacity * sizeof(void*));
}
