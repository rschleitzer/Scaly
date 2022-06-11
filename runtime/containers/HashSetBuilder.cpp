namespace scaly {
namespace containers {

using namespace scaly::memory;

// https://planetmath.org/goodhashtableprimes
static size_t HASH_PRIMES[] = {
    3, 5, 11, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613,
    393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611,
    402653189, 805306457, 1610612741,
};

bool is_prime(size_t candidate) {
    if ((candidate & 1) != 0) {
        auto limit = (size_t)std::sqrt((double)candidate);
        size_t divisor = 3;
        while (divisor <= limit) {
            divisor += 2;
            if ((candidate % divisor) == 0) {
                return false;
            }
        }
        return true;
    }

    return candidate == 2;
}

size_t get_prime(size_t size) {
    for (int i = 0; i < 30; i++) {
        if (HASH_PRIMES[i] >= size) {
            return HASH_PRIMES[i];
        }
    }

    size_t i = size | 1;
    while (i < std::numeric_limits<size_t>::max()) {
        if (is_prime(i)) {
            return i;
        }
        i += 2;
    }
    return size;
}

// FNV-1a hash
inline size_t hash(char* data, size_t length) {
    size_t hash = 0xcbf29ce484222325;
    size_t prime = 0x100000001b3;
    for(int i = 0; i < length; ++i) {
        char value = data[i];
        hash = hash ^ value;
        hash *= prime;
    }
    return hash;
}

// template<class T> struct List;
template<class T> struct Slot {
    T value;
    size_t hash_code;
};

template<class T>
struct HashSetBuilder : Object {
    size_t length;
    Vector<List<Slot<T>>>* slots;
    Page* slots_page;

    static HashSetBuilder<T>* create(Page* _rp) {
        return new(alignof(HashSetBuilder<T>), _rp) HashSetBuilder<T>();
    }

    static HashSetBuilder<T>* from_vector(Page* _rp, Vector<T>& vector) {
        auto hash_set_builder = create(_rp);
        if (vector.length > 0)
        {
            hash_set_builder->reallocate(vector.length);
            for (size_t i = 0; i < vector.length; i++) {
                hash_set_builder->add_internal(*(vector[i]));
            }
        }

        return hash_set_builder;
    }

    void reallocate(size_t size)
    {
        auto hash_size = get_prime(size);
        this->slots_page = Page::get(this)->allocate_exclusive_page();
        Vector<List<Slot<T>>>* slots = Vector<List<Slot<T>>>::create(this->slots_page, hash_size);

        if (this->slots != nullptr) {
            auto vector_iterator = VectorIterator<List<Slot<T>>>::create(*this->slots);
            while (auto element = vector_iterator.next()) {
                auto list_iterator = ListIterator<Slot<T>>::create(element->head);
                while (auto item = list_iterator.next())
                {
                    auto hash_code = item->hash_code;
                    auto slot_number = hash_code % slots->length;
                    auto slot_list = slots->get(slot_number);
                    if (slot_list == nullptr)
                    {
                        slot_list = List<Slot<T>>::create(Page::get(this->slots_page));
                        slots->set(slot_number, *slot_list);
                    }

                    slot_list->add(this->slots_page, *item);
                }
            }
            Page::get(this)->deallocate_exclusive_page(Page::get(this->slots));
        }
        this->slots = slots;
    }

    bool add(T value) {
        auto hash_size = get_prime(this->length + 1);
        if (this->slots == nullptr || hash_size > this->slots->length)
            reallocate(this->length + 1);
        return add_internal(value);
    }

    bool add_internal(T value) {
        auto hash_code = value.hash();
        auto slot_number = hash_code % this->slots->length;
        auto slot_list = this->slots->get(slot_number);
        if (slot_list == nullptr)
            this->slots->set(slot_number, *List<Slot<T>>::create(this->slots_page));
        auto iterator = slot_list->get_iterator();
        while (Slot<T>* item = iterator.next()) {
            if (value.equals(item->value)) {
                return false;
            }
        }

        slot_list->add(this->slots_page, Slot<T> {
            .value = value,
            .hash_code = hash_code,
        });

        this->length++;
        return true;
    }

    bool contains(const T& value) {
        if (this->slots == nullptr)
            return false;

        auto hash = value.hash();
        auto slot_number = hash % this->slots->length;
        auto slot = this->slots->get(slot_number);
        auto iterator = slot->get_iterator();
        while (Slot<T>* item = iterator.next()) {
            if (value.equals(item->value)) {
                return true;
            }
        }

        return false;
    }
};

}

}