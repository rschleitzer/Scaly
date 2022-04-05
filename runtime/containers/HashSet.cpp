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

template<class T> struct HashSet : Object {
    Vector<List<Slot<T>>> slots;

    static HashSet<T>* from_vector(Page* _rp, Vector<T>& vector) {
        auto hash_size = get_prime(vector.length);
        Vector<List<Slot<T>>> slots = *Vector<List<Slot<T>>>::create(_rp, hash_size);
        for (int i = 0; i < hash_size; i++)
            slots.set(i, *List<Slot<T>>::create(_rp));
        auto hash_set = new(alignof(HashSet<T>), _rp) HashSet { .slots = slots };
        hash_set->initialize_from_vector(vector);
        return hash_set;
    }

    void initialize_from_vector(Vector<T>& vector) {
        for (size_t i = 0; i < vector.length; i++) {
            this->add(*vector[i]);
        }
    }

    void add(T& value) {
        auto hash_code = value.hash();
        auto slot_number = hash_code % this->slots.length;
        auto slot_list = this->slots[slot_number];
        auto iterator = slot_list->get_iterator();
        while (Slot<T>* item = iterator.next()) {
            if (value.equals(item->value)) {
                return;
            }
        }

        slot_list->add(Slot<T> {
            .value = value,
            .hash_code = hash_code,
        });
    }

    bool contains(T& value) {
        auto slot = this->slots[value.hash() % this->slots.length];
        auto iterator = slot->get_iterator();
        while (Slot<T>* item = iterator.next()) {
            if (value.equals(item->value)) {
                return true;
            }
        }

        return false;
    }
};
