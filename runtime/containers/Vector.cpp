namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct Array;

template<class T> struct Vector;

template<class T> struct VectorIterator {
    Vector<T>& vector;
    size_t position;

    VectorIterator<T>(Vector<T>& vector) : vector(vector), position(0) {}

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

    Vector(size_t length, T* data)
    : length(length), data(data) {}

    Vector<T>(Page* _rp, size_t length) : length(length) {
        if (this->length > 0) {
            this->data = (T*) _rp->allocate_raw(length * sizeof(T), alignof(T));
            std::memset(this->data, 0, length * sizeof(T));
        } else {
            this->data = nullptr;
        }
    }

    static Vector<T>* from_raw_array(Page* _rp, T* array, size_t length) {
        Vector<T>* vector = new(alignof(Vector<T>), _rp) Vector<T>(_rp, length);
        memcpy(vector->data, array, length * sizeof(T));
        return vector;
    }

    static Vector<T>* from_array(Page* _rp, Array<T>& array) {
        if (array.length == 0)
            return new(alignof(Vector<T>), _rp) Vector<T>(0, nullptr);
        return Vector<T>::from_raw_array(_rp, array.vector->data, array.length);
    }

    T* get(size_t i) {
        if (i >= this->length)
            return nullptr;
        return (*this)[i];
    }

    bool set(size_t i, T item) {
        if (i >= length)
            return false;
        *(*this)[i] = item;
        return true;
    }

    T* operator [](size_t i) {
        if (i >= length)
            return nullptr;

        return data + i;
    }
};

}
}