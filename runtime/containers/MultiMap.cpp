namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V>
struct MultiMap : Object {
    size_t length;
    Vector<Vector<KeyValuePair<K, V>>>* slots;

    static MultiMap<K, V>* create(Page* _rp) {
        return new(alignof(MultiMap<K, V>), _rp) MultiMap<K, V> ();
    }

    static MultiMap<K, V>* from_hash_map_builder(Region& _pr, Page* _rp, MultiMapBuilder<K, V>& hash_map_builder) {
        auto _r = Region::create(_pr);
        auto hash_map = create(_rp);
        if (hash_map_builder.length == 0)
            return hash_map;

        hash_map->slots = Vector<Vector<KeyValuePair<K, V>>>::create(_rp, hash_map_builder.slots->length);
        auto length = hash_map_builder.slots->length;
        for (size_t i = 0; i < length; i++) {
            auto _r_1 = Region::create(_r);
            auto array = Array<KeyValuePair<K, V>>::create(_r_1.page);
            auto list_iterator = ListIterator<Slot<KeyValuePair<K, V>>>::create(hash_map_builder.slots->get(i)->head);
            while (auto item = list_iterator.next())
                array->add(item->value);
            if (array->length > 0)
                hash_map->slots->set(i, *Vector<KeyValuePair<K, V>>::from_array(_rp, *array));
        }

        return hash_map;
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

    V* operator [](K& key) {
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