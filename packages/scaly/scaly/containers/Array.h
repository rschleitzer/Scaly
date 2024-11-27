#ifndef _Array_h
#define _Array_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class T>
struct Array : Object {
    size_t length;
    Vector<T>* vector;

    Array(size_t length, Vector<T>* vector) : length(length), vector(vector) {}

    Array() : length(0), vector(nullptr){
    }

    Array(size_t size) : length(0), vector(nullptr){
        const auto exclusive_page = (*Page::get(this)).allocate_exclusive_page();
        vector = new (alignof(Vector<T>), exclusive_page) Vector<T>(exclusive_page, size);
    }

    T* get_buffer() {
        if (vector == nullptr) 
            return nullptr;
        return (*vector).data;
    };

    size_t get_length() {
        return length; };

    size_t get_capacity() {
        if (vector == nullptr) 
            return 0;
        return (*vector).length;
    };

    void add(T item) {
        if (vector == nullptr||length == (*vector).length) 
            reallocate();
        const auto location = (*vector).data+length;
        *location = item;
        length = length+1;
    };

    void add(Vector<T> items) {
        auto new_length = length+items.length;
        const auto size = sizeof(T);
        if (vector == nullptr||new_length>(*vector).length) 
            reallocate();
        if (new_length>(*vector).length) {
            auto own_page = Page::get(this);
            const auto data = (T*)(*own_page).allocate(length*size, alignof(T));
            const auto bytes_to_copy = (*vector).length*size;
            if ((*vector).length>0) 
                memcpy(data, (*vector).data, bytes_to_copy);
            const auto old_exclusive_page = Page::get((*vector).data);
            (*own_page).deallocate_exclusive_page(old_exclusive_page);
            (*vector).data = data;
            (*vector).length = new_length;
        };
        if (items.length>0) 
            memcpy((*vector).data+length*size, items.data, items.length*size);
        length = length+items.length;
    };

    T* get(size_t i) {
        if (i>=length) 
            return nullptr;
        return (*this)[i];
    };

    T* operator [](size_t i){
        if (length<i+1) 
            return nullptr;
        return (*vector).data+i;
    }

    void reallocate() {
        const auto own_page = Page::get(this);
        const auto size = sizeof(T);
        size_t length = 0;
        if (vector == nullptr) {
            const auto exclusive_page = (*own_page).allocate_exclusive_page();
            const auto capacity = (*exclusive_page).get_capacity(alignof(T))-sizeof(Vector<T>);
            length = capacity/size;
            vector = new (alignof(Vector<T>), exclusive_page) Vector<T>(exclusive_page, length);
        }
        else {
            length = (*vector).length*2;
            const auto new_vector = new (alignof(Vector<T>), own_page) Vector<T>(own_page, length);
            const auto bytes_to_copy = (*vector).length*size;
            memcpy((*new_vector).data, (*vector).data, bytes_to_copy);
            const auto old_exclusive_page = Page::get((*vector).data);
            (*own_page).deallocate_exclusive_page(old_exclusive_page);
            vector = new_vector;
        };
    };
};

#endif