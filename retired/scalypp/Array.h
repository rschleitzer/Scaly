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
    
    _Array<T>(_Array<T>* array)
    : _size(array->length()), _capacity(array->length()) {
        allocate();
        memcpy(_rawArray, array->getRawArray(), _size * sizeof(T**));
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
            T* item = *(*this)[_size - 1];
            _size--;
            return item;
        }
    }

    // Remove a value from the array
    void remove(T* t) {
        for (size_t i = 0; i < _size; i++) {
            if (*(*this)[i] == t) {
                for (size_t j = i + 1; j < _size; j++)
                    *(*this)[j - 1] = *(*this)[j];
                _size--;
                return;
            }
        }
    }

private:

    void reAllocate(size_t newCapacity) {
        T** oldArray = _rawArray;
        _capacity = newCapacity;
        allocate();
        memcpy(_rawArray, oldArray, _size * sizeof(T*));

        // Reclaim the page if it was oversized, i.e., exclusively allocated
        if (((Object*)oldArray)->_getPage()->isOversized())
            _getPage()->reclaimArray(oldArray);
    }

    void allocate() {
        _rawArray = (T**) _getPage()->allocateObject(_capacity * sizeof(T*));
    }

    size_t _size;
    size_t _capacity;
    T** _rawArray;
};

}
#endif//__Scaly_Array__
