#ifndef _Vector_h
#define _Vector_h
#include "../../scaly.h"
using namespace scaly::memory;

template<class T>
struct VectorIterator : Object {
    Vector<T>* vector;
    size_t position;

    VectorIterator(Vector<T>* vector) : vector(vector), position(0){
    }

    T* next() {
        if (vector == nullptr) 
            return nullptr;
        if (position == (*vector).length) 
            return nullptr;
        position = position+1;
        return (*vector).get(position-1);
    };
};

template<class T>
struct Vector : Object {
    size_t length;
    T* data;

    Vector() : length(0), data(nullptr){
    }

    Vector(T* data, size_t length) : length(length), data(data){
    }

    Vector(Page* rp, size_t length) : length(length){
        if (length>0) {
            data = (T*)(*rp).allocate_raw(length*sizeof(T), alignof(T));
            memset(data, 0, length*sizeof(T));
        }
        else {
            data = nullptr;
        };
    }

    Vector(Page* rp, T* array, size_t legnth) : length(length){
        if (length>0) {
            data = (T*)(*rp).allocate_raw(length*sizeof(T), alignof(T));
            memcpy(data, array, length*sizeof(T));
        }
        else {
            data = nullptr;
        };
    }

    Vector(Page* rp, Vector<T> other) : length(other.length){
        if (length>0) {
            data = (T*)(*rp).allocate_raw(length*sizeof(T), alignof(T));
            memcpy(data, other.data, length*sizeof(T));
        }
        else {
            data = nullptr;
        };
    }

    Vector(Page* rp, Array<T> array) : length(array.length){
        if (length>0) {
            data = (T*)(*rp).allocate_raw(length*sizeof(T), alignof(T));
            memcpy(data, (*array.vector).data, length*sizeof(T));
        }
        else {
            data = nullptr;
        };
    }

    Vector(Page* rp, List<T> list) : length(list.count()){
        if (length>0) {
            data = (T*)(*rp).allocate_raw(length*sizeof(T), alignof(T));
            auto list_iterator = list.get_iterator();
            size_t i = length;
            while (auto item = list_iterator.next()) {
                i = i-1;
                *(*this)[i] = *item;
            };
        }
        else {
            data = nullptr;
        };
    }

    T* operator [](size_t i){
        if (i>=length) 
            return nullptr;
        return data+i;
    }

    bool put(size_t i, T item) {
        if (i>=length) 
            return false;
        *(*this)[i] = item;
        return true;
    };

    T* get(size_t i) {
        if (i>=length) 
            return nullptr;
        return (*this)[i];
    };
};

#endif