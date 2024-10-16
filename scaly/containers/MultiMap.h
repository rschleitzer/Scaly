namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V> struct MultiMap;

template<class K, class V>
struct MultiMapIterator {
    VectorIterator<Vector<KeyValuePair<K, Vector<V>>>> slot_iterator;
    VectorIterator<KeyValuePair<K, Vector<V>>> element_iterator;

    MultiMapIterator(const MultiMap<K, V>& multi_map) 
        : slot_iterator(VectorIterator<Vector<KeyValuePair<K, Vector<V>>>>(multi_map.slots)), 
          element_iterator(VectorIterator<KeyValuePair<K, Vector<V>>>(slot_iterator.next())) {}

    Vector<V>* next() {
        while (true) {
            auto ret = element_iterator.next();
            if (ret != nullptr)
                return &(*ret).value;
            
            auto next_slot = slot_iterator.next();
            if (next_slot == nullptr)
                return nullptr;
            
            element_iterator = VectorIterator<KeyValuePair<K, Vector<V>>>(next_slot);
        }
    }
};

template<class K, class V>
struct MultiMap : Object {
    Vector<Vector<KeyValuePair<K, Vector<V>>>>* slots;

    MultiMap() {
        this->slots = nullptr;
    };

    MultiMap(Page* _rp, MultiMapBuilder<K, V>& multi_map_builder) {
        Region _r;
        if (multi_map_builder.length == 0) {
            this->slots = nullptr;
            return;
        }

        this->slots = new(alignof(Vector<Vector<KeyValuePair<K, Vector<V>>>>), _rp) Vector<Vector<KeyValuePair<K, Vector<V>>>>(_rp, multi_map_builder.slots->length);
        auto length = multi_map_builder.slots->length;
        for (size_t i = 0; i < length; i++) {
            Region _r_1;
            Array<KeyValuePair<K, Vector<V>>>& array = *new(alignof(Array<KeyValuePair<K, Vector<V>>>), _r_1.get_page()) Array<KeyValuePair<K, Vector<V>>>();
            auto list_iterator = ListIterator<Slot<KeyValuePair<K, Array<V>*>>>(multi_map_builder.slots->get(i)->head);
            while (auto item = list_iterator.next()) {
                array.add(
                    KeyValuePair<K, Vector<V>> {
                        .key = item->value.key,
                        .value = Vector<V>(_rp, *item->value.value),
                    }                
                );
            }
            if (array.length > 0)
                this->slots->put(i, Vector<KeyValuePair<K, Vector<V>>>(_rp, array));
        }
    }

    bool contains(K& key) {
        if (this->slots == nullptr)
            return false;

        auto hash = key.hash();
        auto slot_number = hash % this->slots->length;
        auto slot = this->slots->get(slot_number);
        auto length = slot->length;
        for (size_t i = 0; i < length; i++) {
            if (key.equals(slot->get(i)->key)) {
                return true;
            }
        }

        return false;
    }

    Vector<V>* operator [](K& key) {
        if (this->slots == nullptr)
            return nullptr;

        auto hash = key.hash();
        auto slot_number = hash % this->slots->length;
        auto slot = this->slots->get(slot_number);
        auto length = slot->length;
        for (size_t i = 0; i < length; i++) {
            if (key.equals(slot->get(i)->key)) {
                return &(slot->get(i)->value);
            }
        }

        return nullptr;
    }
};

}
}