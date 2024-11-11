#ifndef _HashMapBuilder_h
#define _HashMapBuilder_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class K, class V>
struct KeyValuePair : Object {
    K key;
    V value;

    KeyValuePair(K key, V value) : key(key), value(value) {}
};

template<class K, class V>
struct HashMapBuilder : Object {
    size_t length;
    Vector<BuilderList<Slot<KeyValuePair<K, V>>>>* slots;

    HashMapBuilder(size_t length, Vector<BuilderList<Slot<KeyValuePair<K, V>>>>* slots) : length(length), slots(slots) {}

    HashMapBuilder() : length(0), slots(nullptr) {}

    HashMapBuilder(Page* rp, Vector<KeyValuePair<K, V>> vector) : HashMapBuilder<K, V>(){
        if (vector.length>0) {
            reallocate(vector.length);
            auto vector_iterator = VectorIterator<KeyValuePair<K, V>>(&vector);
            while (auto element = vector_iterator.next()) add_internal((*element).key, (*element).value);
        };
    }

    void reallocate(size_t size) {
        const auto hash_size = hashing::get_prime(size);
        const auto slots_page = (*Page::get(this)).allocate_exclusive_page();
        const auto new_slots = new (alignof(Vector<BuilderList<Slot<KeyValuePair<K, V>>>>), slots_page) Vector<BuilderList<Slot<KeyValuePair<K, V>>>>(slots_page, hash_size);
        if (slots != nullptr) {
            auto vector_iterator = VectorIterator<BuilderList<Slot<KeyValuePair<K, V>>>>(slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = BuilderListIterator<Slot<KeyValuePair<K, V>>>((*element).head);
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

    bool add(K key, V value) {
        const auto hash_size = hashing::get_prime(length+1);
        if (slots == nullptr||hash_size>(*slots).length) 
            reallocate(length+1);
        return add_internal(key, value);
    };

    bool add_internal(K key, V value) {
        const auto hash_code = key.hash();
        const auto slot_number = hash_code%(*slots).length;
        const auto slot_list = (*(slots))[slot_number];
        auto iterator = (*slot_list).get_iterator();
        while (auto item = iterator.next()) {
            if ((key.equals((*item).value.key))) 
                return false;
        };
        (*slot_list).add((*slots).get_page(), Slot<KeyValuePair<K, V>>(KeyValuePair<K, V>(key, value), hash_code));
        length = length+1;
        return true;
    };

    bool contains(K key) {
        if (slots == nullptr) 
            return false;
        const auto hash = key.hash();
        const auto slot_number = hash%(*slots).length;
        const auto slot = (*slots).get(slot_number);
        auto iterator = (*slot).get_iterator();
        while (auto item = iterator.next()) {
            if (key.equals((*item).value.key)) 
                return true;
        };
        return false;
    };

    V* get(K key) {
        if (slots == nullptr) 
            return nullptr;
        const auto slot = (*slots).get(key.hash()%(*slots).length);
        auto iterator = (*slot).get_iterator();
        while (auto item = iterator.next()) {
            if ((key.equals((*item).value.key))) 
                return &((*item).value.value);
        };
        return nullptr;
    };

    V* operator [](K key){
        return get(key);
    }
};

#endif