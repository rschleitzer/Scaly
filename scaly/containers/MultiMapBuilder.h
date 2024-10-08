namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class K, class V>
struct MultiMapBuilder : Object {
    size_t length;
    Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>* slots;

    MultiMapBuilder<K, V>() :length(0), slots(nullptr) {}

    void reallocate(size_t size) {
        auto hash_size = get_prime(size);
        auto slots_page = Page::get(this)->allocate_exclusive_page();
        auto slots = new(alignof(Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>), slots_page) Vector<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>(slots_page, hash_size);

        if (this->slots != nullptr) {
            auto vector_iterator = VectorIterator<BuilderList<Slot<KeyValuePair<K, Array<V>*>>>>(this->slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = BuilderListIterator<Slot<KeyValuePair<K, Array<V>*>>>(element->head);
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

    void add(K key, V value) {
        auto hash_size = get_prime(this->length + 1);
        if (this->slots == nullptr || hash_size > this->slots->length)
            reallocate(this->length + 1);
        add_internal(key, value);
    }

    void add_internal(K key, V value) {
        auto hash_code = key.hash();
        auto slot_number = hash_code % this->slots->length;
        auto slot_list = (*(this->slots))[slot_number];
        auto iterator = slot_list->get_iterator();
        Array<V>* array = nullptr;
        while (Slot<KeyValuePair<K, Array<V>*>>* item = iterator.next()) {
            if (key.equals(item->value.key)) {
                array = item->value.value;
            }
        }

        if (array == nullptr)
        {
            slot_list->add(this->slots->get_page(), Slot<KeyValuePair<K, Array<V>*>> {
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