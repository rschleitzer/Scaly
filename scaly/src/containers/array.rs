extern crate libc;

use self::libc::c_void;
use self::libc::memcpy;
use containers::Vector;
use memory::Page;
use std::mem::align_of;
use std::mem::size_of;
use std::ptr::null_mut;
use std::ptr::write;
use std::ops::Index;
use std::ops::{Deref, DerefMut};

pub struct Array<T: Copy> {
    vector: Vector<T>,
    length: usize,
}

impl<T: Copy> Array<T> {
    pub fn new() -> Array<T> {
        Array {
            vector: Vector {
                data: null_mut(),
                length: 0,
            },
            length: 0,
        }
    }

    pub fn get_length(&self) -> usize {
        self.length
    }

    pub fn add(&mut self, item: T) {
        if self.length == self.vector.length {
            self.reallocate();
        }

        unsafe {
            let location = self.vector.data.offset(self.length as isize);
            write(location, item);
        }

        self.length += 1;
    }

    fn reallocate(&mut self) {
        let _own_page = Page::own(self);
        let size = size_of::<T>();
        unsafe {
            if self.vector.length == 0 {
                let exclusive_page = (*_own_page).allocate_exclusive_page();
                let capacity = (*exclusive_page).get_capacity::<T>();
                self.vector.length = capacity / size;
                self.vector.data = (*exclusive_page)
                    .allocate_raw(self.vector.length * size, align_of::<T>())
                    as *mut T;
            } else {
                let old_length = self.vector.length;
                self.vector.length *= 2;
                let old_data = self.vector.data;
                self.vector.data =
                    (*_own_page).allocate_raw(self.vector.length * size, align_of::<T>()) as *mut T;
                memcpy(
                    self.vector.data as *mut c_void,
                    old_data as *mut c_void,
                    old_length * size,
                );
                let old_exclusive_page = Page::get(old_data as usize);
                (*old_exclusive_page).forget();
            }
        }
    }
}

impl<T: Copy> Index<usize> for Array<T> {
    type Output = T;
    fn index(&self, offset: usize) -> &Self::Output {
        unsafe { &*self.vector.data.offset(offset as isize) }
    }
}

impl<T: Copy> Deref for Array<T> {
    type Target = [T];
    fn deref(&self) -> &[T] {
        unsafe { ::std::slice::from_raw_parts(self.vector.data, self.length) }
    }
}

impl<T: Copy> DerefMut for Array<T> {
    fn deref_mut(&mut self) -> &mut [T] {
        unsafe { ::std::slice::from_raw_parts_mut(self.vector.data, self.length) }
    }
}
