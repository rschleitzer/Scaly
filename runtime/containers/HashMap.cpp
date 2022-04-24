namespace scaly::containers {

using namespace scaly::memory;

template<class K, class V>
struct KeyValuePair {
    K key;
    V value;
};

template<class K, class V>
struct HashMap : Object {
    size_t length;
    Vector<List<Slot<KeyValuePair<K, V>>>>* slots;
    Page* slots_page;

    static HashMap<K, V>* create(Page* _rp) {
        return new(alignof(HashMap<K, V>), _rp) HashMap<K, V> {
            .length = 0,
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
        auto hash_size = get_prime(size);
        this->slots_page = Page::get(this)->allocate_exclusive_page();
        if (hash_size < 97)
            hash_size = 97;
        Vector<List<Slot<KeyValuePair<K, V>>>>* slots = Vector<List<Slot<KeyValuePair<K, V>>>>::create(this->slots_page, hash_size);

        if (this->slots != nullptr) {
            auto vector_iterator = VectorIterator<List<Slot<KeyValuePair<K, V>>>>::create(this->slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = ListIterator<Slot<KeyValuePair<K, V>>>::create(element->head);
                while (auto item = list_iterator.next())
                {
                    auto hash_code = item->hash_code;
                    auto slot_number = hash_code % slots->length;
                    auto slot_list = slots->get(slot_number);
                    if (slot_list == nullptr)
                    {
                        slot_list = List<Slot<KeyValuePair<K, V>>>::create(Page::get(this->slots_page));
                        slots->set(slot_number, *slot_list);
                    }

                    slot_list->add(this->slots_page, *item);
                }
            }
            Page::get(this)->deallocate_exclusive_page(Page::get(this->slots));
        }
        this->slots = slots;
    }

    bool add(K key, V value) {
        auto hash_size = get_prime(this->length + 1);
        if (this->slots == nullptr || hash_size > this->slots->length)
            reallocate(this->length + 1);
        return add_internal(key, value);
    }

    bool add_internal(K key, V value) {
        auto hash_code = key.hash();
        auto slot_number = hash_code % this->slots->length;
        auto slot_list = this->slots->get(slot_number);
        if (slot_list == nullptr)
            this->slots->set(slot_number, *List<Slot<KeyValuePair<K, V>>>::create(this->slots_page));
        auto iterator = slot_list->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return false;
            }
        }

        slot_list->add(this->slots_page, Slot<KeyValuePair<K, V>> {
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

        auto hash = key.hash();
        auto slot_number = hash % this->slots->length;
        auto slot = this->slots->get(slot_number);
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

}