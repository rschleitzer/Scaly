#ifndef _HashMap_h
#define _HashMap_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class K, class V>
struct HashMapIterator : Object {
    VectorIterator<Vector<KeyValuePair<K, V>>> slot_iterator;
    VectorIterator<KeyValuePair<K, V>> element_iterator;

    HashMapIterator(VectorIterator<Vector<KeyValuePair<K, V>>> slot_iterator, VectorIterator<KeyValuePair<K, V>> element_iterator) : slot_iterator(slot_iterator), element_iterator(element_iterator) {}

    HashMapIterator(HashMap<K, V> hash_map) : slot_iterator(VectorIterator<Vector<KeyValuePair<K, V>>>(hash_map.slots)), element_iterator(VectorIterator<KeyValuePair<K, V>>(slot_iterator.next())){
    }

    V* next() {
        while (true) {
            const auto ret = element_iterator.next();
            if (ret != nullptr) 
                return &(*ret).value;
            const auto next_slot = slot_iterator.next();
            if (next_slot == nullptr) 
                return nullptr;
            element_iterator = VectorIterator<KeyValuePair<K, V>>(next_slot);
        };
    };
};

template<class K, class V>
struct HashMap : Object {
    Vector<Vector<KeyValuePair<K, V>>>* slots;

    HashMap(Vector<Vector<KeyValuePair<K, V>>>* slots) : slots(slots) {}

    HashMap() : slots(nullptr) {}

    HashMap(Page* rp, HashMapBuilder<K, V> hash_map_builder) {
        auto r = Region();
        if (hash_map_builder.length == 0) {
            slots = nullptr;
            return;
        };
        slots = new (alignof(Vector<Vector<KeyValuePair<K, V>>>), rp) Vector<Vector<KeyValuePair<K, V>>>(rp, (*hash_map_builder.slots).length);
        const auto length = (*hash_map_builder.slots).length;
        size_t i = 0;
        while (i<length) {
            auto array = new (alignof(Array<KeyValuePair<K, V>>), r.get_page()) Array<KeyValuePair<K, V>>();
            auto list_iterator = ListIterator<Slot<KeyValuePair<K, V>>>((*(*hash_map_builder.slots).get(i)).head);
            while (auto item = list_iterator.next()) (*array).add((*item).value);
            if ((*array).length>0) 
                (*slots).put(i, Vector<KeyValuePair<K, V>>(rp, *array));
            i = i+1;
        };
    }

    bool contains(K key) {
        if (slots == nullptr) 
            return false;
        const auto hash = key.hash();
        const auto slot_number = hash%(*slots).length;
        const auto slot = (*slots).get(slot_number);
        const auto length = (*slot).length;
        size_t i = 0;
        while (i<length) {
            if ((key.equals((*(*slot).get(i)).key))) 
                return true;
            i = i+1;
        };
        return false;
    };

    V* operator [](K key){
        return get(key);
    }

    V* get(K key) {
        if (slots == nullptr) 
            return nullptr;
        const auto hash = key.hash();
        const auto slot_number = hash%(*slots).length;
        const auto slot = (*slots).get(slot_number);
        const auto length = (*slot).length;
        size_t i = 0;
        while (i<length) {
            if (key.equals((*(*slot).get(i)).key)) 
                return &((*(*slot).get(i)).value);
            i = i+1;
        };
        return nullptr;
    };
};

#endif