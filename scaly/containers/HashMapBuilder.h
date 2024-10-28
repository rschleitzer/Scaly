namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V>
struct KeyValuePair {
    K key;
    V value;
};

template<class K, class V>
struct HashMapBuilder : Object {
    size_t length;
    Vector<BuilderList<Slot<KeyValuePair<K, V>>>>* slots;

    HashMapBuilder<K, V>() :length(0), slots(nullptr) {}

    HashMapBuilder<K, V>(Page* _rp, Vector<KeyValuePair<K, V>>& vector) : HashMapBuilder<K, V>() {
        if (vector.length > 0) {
            this->reallocate(vector.length);
            auto vector_iterator = VectorIterator<KeyValuePair<K, V>>(vector);
            while (auto element = vector_iterator.next()) {
                this->add_internal(element->key, element->value);
            }
        }
    }

    void reallocate(size_t size) {
        auto hash_size = get_prime(size);
        auto slots_page = Page::get(this)->allocate_exclusive_page();
        auto slots = new(alignof(Vector<BuilderList<Slot<KeyValuePair<K, V>>>>), slots_page) Vector<BuilderList<Slot<KeyValuePair<K, V>>>>(slots_page, hash_size);

        if (this->slots != nullptr) {
            auto vector_iterator = VectorIterator<BuilderList<Slot<KeyValuePair<K, V>>>>(*this->slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = BuilderListIterator<Slot<KeyValuePair<K, V>>>(element->head);
                while (auto item = list_iterator.next())
                {
                    auto hash_code = item->hash_code;
                    auto slot_number = hash_code % slots->length;
                    auto slot_list = (*(slots))[slot_number];
                    slot_list->add(slots_page, *item);
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
        auto slot_list = (*(this->slots))[slot_number];
        auto iterator = slot_list->get_iterator();
        while (Slot<KeyValuePair<K, V>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return false;
            }
        }

        slot_list->add(this->slots->get_page(), Slot<KeyValuePair<K, V>> {
            .value = KeyValuePair<K, V> {
                .key = key,
                .value = value,
            },
            .hash_code = hash_code,
        });

        this->length++;
        return true;
    }

    bool contains(const K& key) {
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

    V* operator [](const K& key) {
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
}