namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V>
struct MultiMapBuilder : Object {
    size_t length;
    Vector<List<Slot<KeyValuePair<K, Array<V>*>>>>* slots;
    Page* slots_page;

    static MultiMapBuilder<K, V>* create(Page* _rp) {
        return new(alignof(MultiMapBuilder<K, V>), _rp) MultiMapBuilder<K, V> ();
    }

    void reallocate(size_t size)
    {
        auto hash_size = get_prime(size);
        this->slots_page = Page::get(this)->allocate_exclusive_page();
        auto slots = new(alignof(Vector<List<Slot<KeyValuePair<K, Array<V>*>>>>), this->slots_page) Vector<List<Slot<KeyValuePair<K, Array<V>*>>>>(this->slots_page, hash_size);

        if (this->slots != nullptr) {
            auto vector_iterator = VectorIterator<List<Slot<KeyValuePair<K, Array<V>*>>>>(*this->slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = ListIterator<Slot<KeyValuePair<K, Array<V>*>>>(element->head);
                while (auto item = list_iterator.next())
                {
                    auto hash_code = item->hash_code;
                    auto slot_number = hash_code % slots->length;
                    auto slot_list = slots->get(slot_number);
                    if (slot_list == nullptr)
                    {
                        slot_list = List<Slot<KeyValuePair<K, Array<V>*>>>::create(Page::get(this->slots_page));
                        slots->set(slot_number, *slot_list);
                    }

                    slot_list->add(this->slots_page, *item);
                }
            }
            Page::get(this)->deallocate_exclusive_page(Page::get(this->slots));
        }
        this->slots = slots;
    }

    void add(K key, V value) {
        auto hash_code = key.hash();
        if (this->slots == nullptr)
            reallocate(this->length + 1);
        auto slot_number = hash_code % this->slots->length;
        auto slot_list = this->slots->get(slot_number);
        if (slot_list == nullptr)
            this->slots->set(slot_number, *List<Slot<KeyValuePair<K, Array<V>*>>>::create(this->slots_page));
        auto iterator = slot_list->get_iterator();
        Array<V>* array = nullptr;
        while (Slot<KeyValuePair<K, Array<V>*>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                array = item->value.value;
            }
        }

        if (array == nullptr)
        {
            auto hash_size = get_prime(this->length + 1);
            if (hash_size > this->slots->length)
                reallocate(this->length + 1);

            slot_number = hash_code % this->slots->length;
            slot_list = this->slots->get(slot_number);
            slot_list->add(this->slots_page, Slot<KeyValuePair<K, Array<V>*>> {
                .value = KeyValuePair<K, Array<V>*> {
                    .key = key,
                    .value = new (alignof(Array<V>), Page::get(this)) Array<V>(),
                },
                .hash_code = hash_code,
            });

            this->length++;
            array = slot_list->head->element.value.value;
        }

        array->add(value);
    }

    bool contains(K& key) {
        if (this->slots == nullptr)
            return false;

        auto hash = key.hash();
        auto slot_number = hash % this->slots->length;
        auto slot = this->slots->get(slot_number);
        auto iterator = slot->get_iterator();
        while (Slot<KeyValuePair<K, Array<V>*>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return true;
            }
        }

        return false;
    }

    Array<V>* operator [](K& key) {
        if (this->slots == nullptr)
            return nullptr;

        auto slot = this->slots->get(key.hash() % this->slots->length);
        auto iterator = slot->get_iterator();
        while (Slot<KeyValuePair<K, Array<V>*>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                return item->value.value;
            }
        }

        return nullptr;
    }
};

}
}