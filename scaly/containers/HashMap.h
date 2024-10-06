namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V> struct HashMap;

template<class K, class V>
struct HashMapIterator {
    VectorIterator<Vector<KeyValuePair<K, V>>> slot_iterator;
    VectorIterator<KeyValuePair<K, V>> element_iterator;

    HashMapIterator<K, V>(const HashMap<K, V>& hash_map) 
        : slot_iterator(VectorIterator<Vector<KeyValuePair<K, V>>>(hash_map.slots)), element_iterator(VectorIterator<KeyValuePair<K, V>>(slot_iterator.next())) {}

    V* next() {
        while (true) {
            auto ret = element_iterator.next();
            if (ret != nullptr)
                return &(*ret).value;
            
            auto next_slot = slot_iterator.next();
            if (next_slot == nullptr)
                return nullptr;
            
            element_iterator = VectorIterator<KeyValuePair<K, V>>(next_slot);
        }
    }
};

template<class K, class V>
struct HashMap : Object {
    Vector<Vector<KeyValuePair<K, V>>>* slots;

    HashMap<K, V>() {
        this->slots = nullptr;
    };

    HashMap<K, V>(Page* _rp, HashMapBuilder<K, V>& hash_map_builder) {
        Region _r;
        if (hash_map_builder.length == 0) {
            this->slots = nullptr;
            return;
        }

        this->slots = new(alignof(Vector<Vector<KeyValuePair<K, V>>>), _rp) Vector<Vector<KeyValuePair<K, V>>>(_rp, hash_map_builder.slots->length);
        auto length = hash_map_builder.slots->length;
        for (size_t i = 0; i < length; i++) {
            Region _r_1;
            Array<KeyValuePair<K, V>>& array = *new(alignof(Array<KeyValuePair<K, V>>), _r_1.get_page()) Array<KeyValuePair<K, V>>();
            auto list_iterator = ListIterator<Slot<KeyValuePair<K, V>>>(hash_map_builder.slots->get(i)->head);
            while (auto item = list_iterator.next())
                array.add(item->value);
            if (array.length > 0)
                this->slots->set(i, Vector<KeyValuePair<K, V>>(_rp, array));
        }
    }

    bool contains(const K& key) {
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

    V* operator [](const K& key) {
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

    Vector<V*>& get_values(Page* _rp) const {
        Region _r;
        auto array = *new(alignof(Array<V*>), _r.get_page()) Array<V*>();
        if (this->slots) {
            auto slot_iterator = VectorIterator<Vector<KeyValuePair<K, V>>>(this->slots);
            while (auto slot = slot_iterator.next()) {
                auto element_iterator = VectorIterator<KeyValuePair<K, V>>(slot);
                while (auto element = element_iterator.next()) {
                    array.add(&element->value);
                }
            }
        }

        return *new(alignof(Vector<V*>), _rp) Vector<V*>(_rp, array);
    }
};

}
}