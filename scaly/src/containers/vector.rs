use std::mem::{align_of,size_of};
use memory::page::Page;

pub struct Vector<T> {
    pub data: *mut T,
    pub length: usize,
}

impl<T> Vector<T> {
    pub fn new(_page: *mut Page, length: usize) -> Vector<T> {
        unsafe { Vector {
            data: (*_page).allocate_raw(length * size_of::<T>(), align_of::<T>()) as *mut T,
            length: length,
        }}
    }
}