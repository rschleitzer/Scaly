#include "../../scaly.h"
namespace scaly {
namespace containers {
namespace hashing {


bool is_prime(size_t candidate) {
    if ((candidate&1) != 0) {
        const auto limit = (size_t)sqrt((double)candidate);
        size_t divisor = 3;
        while (divisor<=limit) {
            divisor = divisor+2;
            if ((candidate%divisor) == 0) 
                return false;
        };
        return true;
    };
    return candidate == 2;
}

size_t get_prime(size_t size) {
    {
        int i = 0;
        while (i<30) {
            if (*(hashing::HASH_PRIMES+i)>=size) 
                return *(hashing::HASH_PRIMES+i);
            i = i+1;
        };
    };
    {
        auto i = size|1;
        while (i<SIZE_MAX) {
            if (is_prime(i)) 
                return i;
            i = i+2;
        };
    };
    return size;
}

size_t hash(char* data, size_t length) {
    size_t hash = 0xcbf29ce484222325;
    size_t prime = 0x100000001b3;
    int i = 0;
    while (i<length) {
        const auto value = *(data+i);
        hash = hash ^ value;
        hash = hash*prime;
        i = i+1;
    };
    return hash;
}

}

}
}