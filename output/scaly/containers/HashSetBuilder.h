#ifndef _HashSetBuilder_h
#define _HashSetBuilder_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class T>
struct Slot : Object {
    T value;
    size_t hash_code;

    Slot(T value, size_t hash_code) : value(value), hash_code(hash_code) {}
};

template<class T>
struct HashSetBuilder : Object {
    size_t length;
    Vector<BuilderList<Slot<T>>>* slots;

    HashSetBuilder(size_t length, Vector<BuilderList<Slot<T>>>* slots) : length(length), slots(slots) {}

    HashSetBuilder() : length(0), slots(nullptr) {}

    HashSetBuilder(Vector<T> vector) : HashSetBuilder<T>(){
        if (vector.length>0) {
            reallocate(vector.length);
            auto vector_iterator = VectorIterator<T>(&vector);
            while (auto element = vector_iterator.next()) {
                add_internal(*element);
            };
        };
    }

    bool add_internal(T value) {
        const auto hash_code = value.hash();
        const auto slot_number = hash_code%(*slots).length;
        const auto slot_list = (*slots).get(slot_number);
        auto iterator = (*slot_list).get_iterator();
        while (auto item = iterator.next()) {
            if (value.equals((*item).value)) {
                return false;
            };
        };
        (*slot_list).add((*slots).get_page(), Slot<T>(value, hash_code));
        length = length+1;
        return true;
    };

    bool contains(T value) {
        if (slots == nullptr) 
            return false;
        const auto hash = value.hash();
        const auto slot_number = hash%(*slots).length;
        const auto slot = (*slots).get(slot_number);
        auto iterator = (*slot).get_iterator();
        while (auto item = iterator.next()) if (value.equals((*item).value)) 
            return true;
        return false;
    };

    void reallocate(size_t size) {
        const auto hash_size = hashing::get_prime(size);
        const auto slots_page = (*Page::get(this)).allocate_exclusive_page();
        const auto new_slots = new (alignof(Vector<BuilderList<Slot<T>>>), slots_page) Vector<BuilderList<Slot<T>>>(slots_page, hash_size);
        if (slots != nullptr) {
            auto vector_iterator = VectorIterator<BuilderList<Slot<T>>>(slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = BuilderListIterator<Slot<T>>((*element).head);
                while (auto item = list_iterator.next()) {
                    const auto hash_code = (*item).hash_code;
                    const auto slot_number = hash_code%(*new_slots).length;
                    const auto slot_list = (*(new_slots))[slot_number];
                    (*slot_list).add(slots_page, *item);
                };
            };
            (*Page::get(this)).deallocate_exclusive_page(Page::get(slots));
        };
        slots = new_slots;
    };

    bool add(T value) {
        const auto hash_size = hashing::get_prime(length+1);
        if (slots == nullptr||hash_size>(*slots).length) 
            reallocate(length+1);
        return add_internal(value);
    };
};

#endif