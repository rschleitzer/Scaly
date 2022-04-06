using namespace scaly::memory;

template<class K, class V>
struct KeyValuePair {
    K key;
    V value;
};

template<class K, class V>
struct HashMap : Object {
    size_t length;
    size_t hash_size;
    Vector<List<Slot<KeyValuePair<K, V>>>>* slots;

    static HashMap<K, V>* create(Page* _rp) {
        return new(alignof(HashMap<K, V>), _rp) HashMap<K, V> {
            .length = 0,
            .hash_size = 0,
            .slots = nullptr,
        };
    }

    static HashMap<K, V>* from_vector(Page* _rp, Vector<KeyValuePair<K, V>>& vector) {
        auto hash_map = create(_rp);
        if (vector.length > 0)
        {
            hash_map->reallocate(vector.length);
            for (size_t i = 0; i < vector.length; i++) {
                hash_map->add_internal((*(vector[i])).key, (*(vector[i])).value);
            }
        }

        return hash_map;
    }

    void reallocate(size_t size)
    {
        this->hash_size = get_prime(size);
        Page* exclusive_page = Page::get(this)->allocate_exclusive_page();
        if (this->hash_size < 503)
            this->hash_size = 503;
        Vector<List<Slot<KeyValuePair<K, V>>>>* slots = Vector<List<Slot<KeyValuePair<K, V>>>>::create(exclusive_page, this->hash_size);
        if (this->slots != nullptr)
            rearrange();
        this->slots = slots;
    }

    void rearrange()
    {

    }

    bool add(K& key, V& value) {
        auto hash_size = get_prime(this->length + 1);
        if (hash_size > this->hash_size)
            reallocate(this->length + 1);
        add_internal(key, value);
    }

    bool add_internal(K& key, V& value) {
        auto hash_code = key.hash();
        auto slot_number = hash_code % this->slots->length;
        auto slot_list = this->slots->get(slot_number);
        if (slot_list == nullptr)
            this->slots->set(slot_number, *List<Slot<KeyValuePair<K, V>>>::create(Page::get(this->slots)));
        auto iterator = slot_list->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return false;
            }
        }

        slot_list->add(Slot<KeyValuePair<K, V>> {
            .value = KeyValuePair<K, V> {
                .key = key,
                .value = value,
            },
            .hash_code = hash_code,
        });

        this->length++;
        return true;
    }

    bool contains(K& key) {
        if (this->slots == nullptr)
            return false;

        auto slot = this->slots->get(key.hash() % this->slots->length);
        auto iterator = slot->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return true;
            }
        }

        return false;
    }

    V* operator [](K& key) {
        if (this->slots == nullptr)
            return nullptr;

        auto slot = this->slots->get(key.hash() % this->slots->length);
        auto iterator = slot->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return &(item->value.value);
            }
        }

        return nullptr;
    }
};
