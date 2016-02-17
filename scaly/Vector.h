#ifndef __Scaly__Vector__
#define __Scaly__Vector__
#include "Scaly.h"
namespace scaly {

template<class T> class _Array;

template<class T> class _Vector : public Object {
public:

    static _Vector<T>& createUninitialized(_Page* page, size_t theSize)
    {
        _Vector<T>& vector = *(_Vector<T>*)page->allocateObject(sizeof(_Vector) + theSize * sizeof(T*));
        vector.size = theSize;
        return vector;
    }

    static _Vector<T>& create(_Page* page, _Array<T>& array)
    {
        size_t length = array.length();
        _Vector<T>& vector = createUninitialized(page, length);
        memcpy((T**)(&vector + 1), array.getRawArray(), length * sizeof(T**));
        return vector;
    }

    T** operator [](size_t i) {
        if (i < size)
            return (T**)(this + 1) + i;

        return 0;
    }

    int length() {
        return size;
    }

private:
    size_t size;
};

}
#endif//__Scaly_Vector__
