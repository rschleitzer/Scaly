#ifndef _MultiMap_h
#define _MultiMap_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class K, class V>
struct MultiMapIterator : Object {
    VectorIterator<Vector<KeyValuePair<K, Vector<V>>>> slot_iterator;
    VectorIterator<KeyValuePair<K, Vector<V>>> element_iterator;

    MultiMapIterator(VectorIterator<Vector<KeyValuePair<K, Vector<V>>>> slot_iterator, VectorIterator<KeyValuePair<K, Vector<V>>> element_iterator) : slot_iterator(slot_iterator), element_iterator(element_iterator) {}

    MultiMapIterator(MultiMap<K, V> multimap) : slot_iterator(VectorIterator<Vector<KeyValuePair<K, Vector<V>>>>(multimap.slots)), element_iterator(VectorIterator<KeyValuePair<K, Vector<V>>>(slot_iterator.next())){
    }

    Vector<V>* next() {
        while (true) {
            const auto ret = element_iterator.next();
            if (ret != nullptr) 
                return &(*ret).value;
            const auto next_slot = slot_iterator.next();
            if (next_slot == nullptr) 
                return nullptr;
            element_iterator = VectorIterator<KeyValuePair<K, Vector<V>>>(next_slot);
        };
    };
};

template<class K, class V>
struct MultiMap : Object {
    Vector<Vector<KeyValuePair<K, Vector<V>>>>* slots;

    MultiMap(Vector<Vector<KeyValuePair<K, Vector<V>>>>* slots) : slots(slots) {}

    MultiMap() : slots(nullptr) {}

    MultiMap(Page* rp, MultiMapBuilder<K, V> multi_map_builder) {
        auto r = Region();
        if (multi_map_builder.length == 0) {
            slots = nullptr;
            return;
        };
        slots = new (alignof(Vector<Vector<KeyValuePair<K, Vector<V>>>>), rp) Vector<Vector<KeyValuePair<K, Vector<V>>>>(rp, (*multi_map_builder.slots).length);
        const auto length = (*multi_map_builder.slots).length;
        size_t i = 0;
        while (i<length) {
            auto array = new (alignof(Array<KeyValuePair<K, Vector<V>>>), r.get_page()) Array<KeyValuePair<K, Vector<V>>>();
            auto list_iterator = ListIterator<Slot<KeyValuePair<K, Array<V>*>>>((*(*multi_map_builder.slots).get(i)).head);
            while (auto item = list_iterator.next()) (*array).add(KeyValuePair<K, Vector<V>>((*item).value.key, Vector<V>(rp, *(*item).value.value)));
            if ((*array).length>0) 
                (*slots).put(i, Vector<KeyValuePair<K, Vector<V>>>(rp, *array));
            i = i+1;
        };
    }

    Vector<V>* operator [](K key){
        if (slots == nullptr) 
            return nullptr;
        const auto hash = key.hash();
        const auto slot_number = hash%(*slots).length;
        const auto slot = (*slots).get(slot_number);
        const auto length = (*slot).length;
        size_t i = 0;
        while (i<length) {
            if ((key.equals((*(*slot).get(i)).key))) 
                return &((*(*slot).get(i)).value);
            i = i+1;
        };
        return nullptr;
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
};

#endif