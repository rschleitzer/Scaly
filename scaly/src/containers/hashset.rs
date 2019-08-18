use containers::array::Array;
use containers::list::List;
use containers::reference::Ref;
use containers::vector::Vector;
use memory::page::Page;

#[derive(Copy, Clone)]
pub struct HashSet<T: Hash<T> + Copy> {
    slots: Ref<Vector<List<Slot<T>>>>,
}

impl<T: Hash<T> + Copy> HashSet<T> {
    pub fn from_vector(_page: *mut Page, vector: Ref<Vector<T>>) -> Ref<HashSet<T>> {
        let hash_size = HashPrimeHelper::get_prime(vector.length);
        let mut array: Ref<Array<List<Slot<T>>>> = Ref::new(_page, Array::new());
        for _ in 0..hash_size {
            array.add(_page, List::new());
        }
        let slots = Ref::new(_page, Vector::from_array(_page, array));
        let mut hash_set = Ref::new(_page, HashSet { slots: slots });
        hash_set.initialize_from_vector(_page, vector);
        hash_set
    }

    fn initialize_from_vector(&mut self, _page: *mut Page, vector: Ref<Vector<T>>) {
        for value in vector.iter() {
            self.add(_page, value);
        }
    }

    fn add(&mut self, _page: *mut Page, value: &T) {
        let hash_code = value.hash();
        let mut slot_list = self.slots[hash_code % self.slots.length];
        for slot in slot_list.get_iterator() {
            if value.equals(&slot.value) {
                return;
            }
        }

        slot_list.add(
            _page,
            Slot {
                hash_code: hash_code,
                value: *value,
            },
        );
    }

    pub fn contains(&self, value: T) -> bool {
        for slot in self.slots[value.hash() % self.slots.length].get_iterator() {
            if value.equals(&slot.value) {
                return true;
            }
        }

        false
    }
}

#[derive(Copy, Clone)]
struct Slot<T: Copy> {
    value: T,
    hash_code: usize,
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
            if *i >= size {
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
    use containers::String;
    use memory::Heap;
    use memory::Page;
    use memory::Region;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let root_page = Page::get(root_stack_bucket as usize);
    let r = Region::create_from_page(root_page);
    let _r_1 = Region::create(&r);
    let keywords = HashSet::from_vector(
        _r_1.page,
        Ref::new(
            _r_1.page,
            Vector::from_raw_array(
                _r_1.page,
                &[
                    String::from_string_slice(_r_1.page, "using"),
                    String::from_string_slice(_r_1.page, "namespace"),
                    String::from_string_slice(_r_1.page, "typedef"),
                ],
            ),
        ),
    );

    assert_eq!(
        (*keywords).contains(String::from_string_slice(_r_1.page, "using")),
        true
    );
    assert_eq!(
        (*keywords).contains(String::from_string_slice(_r_1.page, "namespace")),
        true
    );
    assert_eq!(
        (*keywords).contains(String::from_string_slice(_r_1.page, "typedef")),
        true
    );
    assert_eq!(
        (*keywords).contains(String::from_string_slice(_r_1.page, "nix")),
        true
    );
}
