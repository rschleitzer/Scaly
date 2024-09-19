typedef __SIZE_TYPE__ size_t;

namespace scaly {
namespace containers {

// https://planetmath.org/goodhashtableprimes
static size_t HASH_PRIMES[] = {
    3, 5, 11, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613,
    393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611,
    402653189, 805306457, 1610612741,
};

extern "C" double sqrt(double);

bool is_prime(size_t candidate) {
    if ((candidate & 1) != 0) {
        auto limit = (size_t)sqrt((double)candidate);
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
    while (i < __SIZE_MAX__) {
        if (is_prime(i)) {
            return i;
        }
        i += 2;
    }
    return size;
}

// FNV-1a hash
size_t hash(char* data, size_t length) {
    size_t hash = 0xcbf29ce484222325;
    size_t prime = 0x100000001b3;
    for(int i = 0; i < length; ++i) {
        char value = data[i];
        hash = hash ^ value;
        hash *= prime;
    }
    return hash;
}

}

}