namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V>
struct HashMap : Object {
    size_t length;
    Vector<Vector<KeyValuePair<K, V>>>* slots;

    HashMap<K, V>(Region& _pr, Page* _rp, HashMapBuilder<K, V>& hash_map_builder) {
        Region _r(_pr);
        if (hash_map_builder.length == 0)
            return;

        this->slots = new(alignof(Vector<Vector<KeyValuePair<K, V>>>), _rp) Vector<Vector<KeyValuePair<K, V>>>(_rp, hash_map_builder.slots->length);
        auto length = hash_map_builder.slots->length;
        for (size_t i = 0; i < length; i++) {
            Region _r_1(_r);
            Array<KeyValuePair<K, V>>& array = *new(alignof(Array<KeyValuePair<K, V>>), _r_1.page) Array<KeyValuePair<K, V>>();
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
};

}
}