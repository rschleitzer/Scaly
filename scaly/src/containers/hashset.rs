use containers::reference::Ref;
use containers::vector::Vector;
use memory::page::Page;

pub struct HashSet<T: Hash<T>> {
    _map: Option<Ref<HashMap<T>>>,
}

impl<T: Hash<T>> HashSet<T> {
    pub fn from_vector(_page: *mut Page, vector: Ref<Vector<T>>) -> Ref<HashSet<T>> {
        let mut hash_set = Ref::new(_page, HashSet { _map: None });
        (*hash_set).initialize_from_vector(vector);
        hash_set
    }

    fn initialize_from_vector(&mut self, vector: Ref<Vector<T>>) {
        let map = Ref::new(
            Page::own(self),
            HashMap::new(Page::own(self), (*vector).length),
        );
        self._map = Some(map);
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

pub trait Equal<T: ?Sized = Self> {
    fn equals(&self, other: &T) -> bool;
}

pub trait Hash<T: ?Sized = Self>: Equal<T> {
    fn hash(&self) -> usize;
}

// FNV-1a hash
pub fn hash(data: *const u8, length: usize) -> usize {
    let bytes = unsafe { std::slice::from_raw_parts(data, length) };
    let mut hash: u64 = 0xcbf29ce484222325;
    for byte in bytes.iter() {
        hash = hash ^ (*byte as u64);
        hash = hash.wrapping_mul(0x100000001b3);
    }
    hash as usize
}

#[test]
fn test_hash_map() {
    use containers::Ref;
    use memory::Heap;
    use memory::Page;
    use memory::Region;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let root_page = Page::get(root_stack_bucket as usize);
    let r = Region::create_from_page(root_page);
    unsafe {
        let _r_1 = Region::create(&r);
        // let keywords: HashSet<Ref<String>> = HashSet::from_vector(
        //     _r_1.page,
        //     Vector::from_raw_array(_r_1.page, ["using", "namespace", "typedef"]),
        // );
    }
}
