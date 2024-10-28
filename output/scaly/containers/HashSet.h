#ifndef _HashSet_h
#define _HashSet_h
#include "../../scaly.h"
using namespace scaly::memory;

template<class T>
struct HashSet : Object {
    Vector<Vector<T>>* slots;

    HashSet(Page* rp, HashSetBuilder<T> hash_set_builder) {
        auto r = Region();
        if (hash_set_builder.length == 0) {
            slots = nullptr;
            return;
        };
        slots = new (alignof(Vector<Vector<T>>), rp) Vector<Vector<T>>(rp, (*hash_set_builder.slots).length);
        auto length = (*hash_set_builder.slots).length;
        size_t i = 0;
        while (i<length) {
            auto array = new (alignof(Array<T>), r.get_page()) Array<T>();
            auto list_iterator = ListIterator<Slot<T>>((*(*hash_set_builder.slots).get(i)).head);
            while (auto item = list_iterator.next()) (*array).add((*item).value);
            if ((*array).length>0) 
                (*slots).put(i, Vector<T>(rp, *array));
            i = i+1;
        };
    }

    bool contains(T value) {
        if (slots == nullptr) 
            return false;
        const auto hash = value.hash();
        const auto slot_number = hash%(*slots).length;
        const auto slot = (*slots).get(slot_number);
        const auto length = (*slot).length;
        size_t i = 0;
        while (i<length) {
            if ((value.equals(*(*slot).get(i)))) 
                return true;
            i = i+1;
        };
        return false;
    };
};

#endif