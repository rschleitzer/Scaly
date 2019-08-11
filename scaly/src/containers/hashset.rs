use containers::reference::Ref;
use containers::vector::Vector;
use memory::page::Page;

pub struct HashSet<T> {
    _map: Option<Ref<HashMap<T>>>,
}

impl<T> HashSet<T> {
    pub fn from_raw_array(_page: *mut Page, _array: *const T, length: usize) -> HashSet<T> {
        HashSet {
            _map: Some(Ref::new(_page, HashMap::new(_page, length))),
        }
    }
}

struct HashMap<T> {
    _buckets: Vector<i32>,
    _slots: Vector<Slot<T>>,
}

impl<T> HashMap<T> {
    pub fn new(_page: *mut Page, size: usize) -> HashMap<T> {
        let hash_size = HashPrimeHelper::get_prime(size);
        HashMap {
            _buckets: Vector::new(_page, hash_size),
            _slots: Vector::new(_page, hash_size),
        }
    }
}

struct Slot<T> {
    _hash_code: usize,
    _next: usize,
    _value: T,
}

struct HashPrimeHelper {}

// https://planetmath.org/goodhashtableprimes
static HASH_PRIMES: &'static [usize] = &[
    3, 5, 11, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613,
    393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611,
    402653189, 805306457, 1610612741,
];

impl HashPrimeHelper {
    pub fn get_prime(size: usize) -> usize {
        for i in HASH_PRIMES {
            if *i > size {
                return *i;
            }
        }

        let mut i = size | 1;
        while i < std::usize::MAX {
            if HashPrimeHelper::is_prime(i) {
                return i;
            }
            i += 2;
        }
        size
    }

    fn is_prime(candidate: usize) -> bool {
        if (candidate & 1) != 0 {
            let limit = (candidate as f64).sqrt() as usize;
            let mut divisor: usize = 3;
            while divisor <= limit {
                divisor += 2;
                if (candidate % divisor) == 0 {
                    return false;
                }
            }
            return true;
        }

        candidate == 2
    }
}
