#ifndef _hashing_h
#define _hashing_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace hashing {
static size_t HASH_PRIMES[] = { 3, 5, 11, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741, };

    bool is_prime(size_t candidate);
    size_t get_prime(size_t size);
    size_t hash(char* data, size_t length);
}
#endif