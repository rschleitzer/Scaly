use containers::Vector;
use std::ptr::null_mut;

pub struct Array<T> {
    _vector: Vector<T>,
    capacity: usize,
}

impl<T> Array<T> {
    pub fn new() -> Array<T> {
        Array {
            _vector: Vector {
                data: null_mut(),
                length: 0,
            },
            capacity: 0,
        }
    }

    pub fn get_length(&self) -> usize {
        self.capacity
    }
}
