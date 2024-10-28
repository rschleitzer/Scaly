#ifndef _MultiMapBuilder_h
#define _MultiMapBuilder_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

template<class K, class V>
struct MultiMapBuilder : Object {
    size_t length;
    Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>* slots;

    MultiMapBuilder(size_t length, Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>* slots) : length(length), slots(slots) {}

    MultiMapBuilder() : length(0), slots(nullptr) {}

    Array<V>* operator [](K key){
        if (slots == nullptr) 
            return nullptr;
        const auto slot = (*slots).get(key.hash()%(*slots).length);
        auto iterator = (*slot).get_iterator();
        while (auto item = iterator.next()) {
            if (key.equals((*item).value.key)) 
                return (*item).value.value;
        };
        return nullptr;
    }

    void add_internal(K key, V value) {
        const auto hash_code = key.hash();
        const auto slot_number = hash_code%(*slots).length;
        const auto slot_list = (*slots)[slot_number];
        auto iterator = (*slot_list).get_iterator();
        Array<V>* array = nullptr;
        while (auto item = iterator.next()) {
            if ((key.equals((*item).value.key))) 
                array = (*item).value.value;
        };
        if (array == nullptr) {
            const auto page = get_page();
            (*slot_list).add((*slots).get_page(), Slot<KeyValuePair<K, Array<V>*>>(KeyValuePair<K, Array<V>*>(key, new (alignof(Array<V>), page) Array<V>()), hash_code));
            length = length+1;
            array = (*(*slot_list).head).element.value.value;
        };
        (*array).add(value);
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

    void reallocate(size_t size) {
        const auto hash_size = hashing::get_prime(size);
        const auto slots_page = (*Page::get(this)).allocate_exclusive_page();
        auto new_slots = new (alignof(Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>), slots_page) Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>(slots_page, hash_size);
        if (slots != nullptr) {
            auto vector_iterator = VectorIterator<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>(slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = BuilderListIterator<Slot<KeyValuePair<K, Array<V>*>>>((*element).head);
                while (auto item = list_iterator.next()) {
                    const auto hash_code = (*item).hash_code;
                    const auto slot_number = hash_code%(*new_slots).length;
                    const auto slot_list = (*new_slots)[slot_number];
                    (*slot_list).add(slots_page, *item);
                };
            };
            (*Page::get(this)).deallocate_exclusive_page(Page::get(slots));
        };
        slots = new_slots;
    };

    void add(K key, V value) {
        const auto hash_size = hashing::get_prime(length+1);
        if ((slots == nullptr||hash_size>(*slots).length)) 
            reallocate(length+1);
        add_internal(key, value);
    };
};

#endif