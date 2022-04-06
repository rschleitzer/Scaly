using namespace scaly::memory;

template<class K, class V>
struct KeyValuePair {
    K key;
    V value;
};

template<class K, class V>
struct HashMap : Object {
    Vector<List<Slot<KeyValuePair<K, V>>>> slots;

    static HashMap<K, V>* from_vector(Page* _rp, Vector<KeyValuePair<K, V>>& vector) {
        auto hash_size = get_prime(vector.length);
        Vector<List<Slot<KeyValuePair<K, V>>>> slots = *Vector<List<Slot<KeyValuePair<K, V>>>>::create(_rp, hash_size);
        for (int i = 0; i < hash_size; i++)
            slots.set(i, *List<Slot<KeyValuePair<K, V>>>::create(_rp));
        auto hash_map = new(alignof(HashMap<K, V>), _rp) HashMap { .slots = slots };
        hash_map->initialize_from_vector(vector);
        return hash_map;
    }

    void initialize_from_vector(Vector<KeyValuePair<K, V>>& vector) {
        for (size_t i = 0; i < vector.length; i++) {
            this->add((*(vector[i])).key, (*(vector[i])).value);
        }
    }

    void add(K& key, V& value) {
        auto hash_code = key.hash();
        auto slot_number = hash_code % this->slots.length;
        auto slot_list = this->slots[slot_number];
        auto iterator = slot_list->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return;
            }
        }

        slot_list->add(Slot<KeyValuePair<K, V>> {
            .value = KeyValuePair<K, V> {
                .key = key,
                .value = value,
            },
            .hash_code = hash_code,
        });
    }

    bool contains(K& value) {
        auto slot = this->slots[value.hash() % this->slots.length];
        auto iterator = slot->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (value.equals(item->value.key)) {
                return true;
            }
        }

        return false;
    }
};
