use scaly::containers::Vector;
use std::ptr::null_mut;

pub struct Array<T> {
    _vector: Vector<T>,
    length: usize,
}

impl<T> Array<T> {
    pub fn new() -> Array<T> {
        Array {
            _vector: Vector {
                data: null_mut(),
                capacity: 0,
            },
            length: 0,
        }
    }

    pub fn get_length(&self) -> usize {
        self.length
    }
}
