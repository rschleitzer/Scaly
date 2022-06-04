namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V>
struct MultiMap : Object {
    Vector<Vector<KeyValuePair<K, Vector<V>>>>* slots;

    static MultiMap<K, V>* create(Page* _rp) {
        return new(alignof(MultiMap<K, V>), _rp) MultiMap<K, V> ();
    }

    static MultiMap<K, V>* from_multi_map_builder(Region& _pr, Page* _rp, MultiMapBuilder<K, V>& multi_map_builder) {
        auto _r = Region::create(_pr);
        auto multi_map = create(_rp);
        if (multi_map_builder.length == 0)
            return multi_map;

        multi_map->slots = Vector<Vector<KeyValuePair<K, Vector<V>>>>::create(_rp, multi_map_builder.slots->length);
        auto length = multi_map_builder.slots->length;
        for (size_t i = 0; i < length; i++) {
            auto _r_1 = Region::create(_r);
            auto array = Array<KeyValuePair<K, Vector<V>>>::create(_r_1.page);
            auto list_iterator = ListIterator<Slot<KeyValuePair<K, Array<V>*>>>::create(multi_map_builder.slots->get(i)->head);
            while (auto item = list_iterator.next()) {
                array->add(
                    KeyValuePair<K, Vector<V>> {
                        .key = item->value.key,
                        .value = *Vector<V>::from_array(_rp, *item->value.value),
                    }                
                );
            }
            if (array->length > 0)
                multi_map->slots->set(i, *Vector<KeyValuePair<K, Vector<V>>>::from_array(_rp, *array));
        }

        return multi_map;
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