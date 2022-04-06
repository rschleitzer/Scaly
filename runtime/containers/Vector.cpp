using namespace scaly::memory;

template<class T> struct Array;

template<class T> struct Vector : Object {
    size_t length;
    T* data;

    static Vector<T>* create(Page* _rp, size_t length) {
        return new(alignof(Vector<T>), _rp) Vector<T> {
            .length = length,
            .data = (T*) _rp->allocate_raw(length * sizeof(T), alignof(T)),
        };
    }

    static Vector<T>* create_without_buffer(Page* _rp, size_t length) {
        return new(alignof(Vector<T>), _rp) Vector<T> {
            .length = length,
        };
    }

    static Vector<T>* from_raw_array(Page* _rp, T* array, size_t length) {
        Vector<T>* vector = Vector<T>::create(_rp, length);
        memcpy(vector->data, array, length * sizeof(T));
        return vector;
    }

    static Vector<T>* from_array(Page* _rp, Array<T>& array) {
        return Vector<T>::from_raw_array(_rp, array.vector->data, array.length);
    }

    T& get(size_t i) {
        return *(*this)[i];
    }

    void set(size_t i, T item) {
        *(*this)[i] = item;
    }

    T* operator [](size_t i) {
        return data + i;
    }
};
