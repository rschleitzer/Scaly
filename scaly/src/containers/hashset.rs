use containers::array::Array;

pub struct HashSet<T> {
    _buckets: Array<i32>,
    _slots: Array<Slot<T>>,
}

impl<T> HashSet<T> {
    pub fn new() -> HashSet<T> {
        HashSet {
            _buckets: Array::new(),
            _slots: Array::new(),
        }
    }

    pub fn add(_value: T) {}
}

struct Slot<T> {
    _hash_code: i32,
    _next: i32,
    _value: T,
}
