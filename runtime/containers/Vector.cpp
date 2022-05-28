namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct Array;

template<class T> struct Vector;

template<class T> struct VectorIterator {
    Vector<T>& vector;
    size_t position;

    static VectorIterator<T> create(Vector<T>& vector) {
        return VectorIterator { .vector = vector, .position = 0 };
    }

    T* next() {
        if (position == vector.length)
            return nullptr;

        position += 1;
        return vector.get(position - 1);
    }
};

template<class T> struct Vector : Object {
    size_t length;
    T* data;

    Vector(size_t length)
    : length(length) {}

    static Vector<T>* create(Page* _rp, size_t length) {
        auto vector = allocate(_rp, length);
        if (length > 0)
            std::memset(vector->data, 0, length * sizeof(T));
        return vector;
    }

    static Vector<T>* allocate(Page* _rp, size_t length) {
        auto vector = create_without_buffer(_rp, length);
        if (length > 0)
            vector->data = (T*) _rp->allocate_raw(length * sizeof(T), alignof(T));
        return vector;
    }

    static Vector<T>* create_without_buffer(Page* _rp, size_t length) {
        return new(alignof(Vector<T>), _rp) Vector<T> (length);
    }

    static Vector<T>* from_raw_array(Page* _rp, T* array, size_t length) {
        Vector<T>* vector = Vector<T>::create(_rp, length);
        memcpy(vector->data, array, length * sizeof(T));
        return vector;
    }

    static Vector<T>* from_array(Page* _rp, Array<T>& array) {
        if (array.length == 0)
            return new(alignof(Vector<T>), _rp) Vector<T>(0);
        return Vector<T>::from_raw_array(_rp, array.vector->data, array.length);
    }

    T* get(size_t i) {
        return (*this)[i];
    }

    void set(size_t i, T item) {
        if (i < length)
            *(*this)[i] = item;
    }

    T* operator [](size_t i) {
        if (i >= length)
            return nullptr;

        return data + i;
    }
};

}
}