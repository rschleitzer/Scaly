#ifndef __Scaly__Array__
#define __Scaly__Array__
#include "Scaly.h"
namespace scaly {

template<class T> class _Array : public Object {
public:
    _Array<T>()
    :_size(0), _capacity(0), _rawArray(0) {}

    _Array<T>(size_t capacity)
    : _size(0), _capacity(capacity) {
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

    T** getRawArray() {
        return _rawArray;
    }

    // Append a value to the array
    void push(T* item) {
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

    // Take away a value from the array's end
    T* pop() {
        if (!_size) {
            return 0;
        }
        else {
            _Page* page = *(*this)[_size - 1];
            _size--;
            return page;
        }
    }

private:

    void reAllocate(size_t newCapacity) {
        T** oldArray = _rawArray;
        _capacity = newCapacity;
        allocate();
        memcpy(_rawArray, oldArray, _size * sizeof(T*));

        // Reclaim the page if it was oversized, i.e., exclusively allocated
        if (((Object*)oldArray)->getPage() == ((_Page*)oldArray))
            getPage()->reclaimArray(oldArray);
    }

    void allocate() {
        _rawArray = (T**) getPage()->allocateObject(_capacity * sizeof(T*));
    }

    size_t _size;
    size_t _capacity;
    T** _rawArray;
};

}
#endif//__Scaly_Array__
