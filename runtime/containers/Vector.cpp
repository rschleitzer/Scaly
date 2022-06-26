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

    Vector(T* data, size_t length)
    : length(length), data(data) {}

    Vector<T>(Page* _rp, size_t length) : length(length) {
        if (this->length > 0) {
            this->data = (T*) _rp->allocate_raw(length * sizeof(T), alignof(T));
            std::memset(this->data, 0, length * sizeof(T));
        } else {
            this->data = nullptr;
        }
    }

    Vector<T>(Page* _rp, T* array, size_t length) : length(length) {
        if (this->length > 0) {
            this->data = (T*) _rp->allocate_raw(length * sizeof(T), alignof(T));
            memcpy(this->data, array, length * sizeof(T));
        } else {
            this->data = nullptr;
        }
    }

    Vector<T>(Page* _rp, Array<T>& array) : length(array.length) {
        if (this->length > 0) {
            this->data = (T*) _rp->allocate_raw(length * sizeof(T), alignof(T));
            memcpy(this->data, array.vector->data, length * sizeof(T));
        } else {
            this->data = nullptr;
        }
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