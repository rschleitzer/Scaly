#ifndef __Scaly_Array__
#define __Scaly_Array__
#include "Scaly.h"
namespace scaly {

template<class T> class Array : public Object {
public:
    Array<T>()
    :_size(0), _capacity(0), _rawArray(0) {}

    Array<T>(size_t size)
    : _size(size), _capacity(size) {
        allocate();
    }

    T** operator [](size_t i) {
        if (i < _size)
            return _rawArray + i;

        return 0;
    }

    int length() {
        return _size;
    }
    
    // Append a value to the array
    void append(T* item) {
        if (!_rawArray) {
            _capacity = 1;
            _size = 0;
            allocate();
        }
        else {
            if (_size == _capacity)
                reAllocate(_capacity * 2);
        }

        *(_rawArray + _size) = item;
        _size += 1;
    }

private:

    void reAllocate(size_t newCapacity) {
        T** oldArray = _rawArray;
        size_t oldCapacity = _capacity;
        _capacity = newCapacity;
        allocate();
        memcpy(_rawArray, oldArray, _size * sizeof(T*));
        
        // Reclaim the page if it was oversized, i.e., exclusively allocated
        if (oldCapacity > _Page::pageSize)
            _Page::reclaimArray(oldArray);
    }

    void allocate() {
        size_t length = _capacity * sizeof(T*);
        _rawArray = (T**) getPage()->allocateObject(length);
    }

    size_t _size;
    size_t _capacity;
    T** _rawArray;
};

}
#endif//__Scaly_Array__
