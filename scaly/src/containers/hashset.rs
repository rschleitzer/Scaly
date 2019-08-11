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
        HashMap {
            _buckets: Vector::new(_page, size),
            _slots: Vector::new(_page, size),
        }
    }
}

struct Slot<T> {
    _hash_code: i32,
    _next: usize,
    _value: T,
}
