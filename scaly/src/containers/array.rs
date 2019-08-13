extern crate libc;

use self::libc::c_void;
use self::libc::memcpy;
use containers::Vector;
use memory::Page;
use std::mem::align_of;
use std::mem::size_of;
use std::ptr::null_mut;
use std::ptr::write;

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

    pub fn add(&mut self, page: *mut Page, item: T) {
        if self.length == self.vector.length {
            self.reallocate(page);
        }

        unsafe {
            let location = self.vector.data.offset(self.length as isize);
            write(location, item);
        }

        self.length += 1;
    }

    fn reallocate(&mut self, page: *mut Page) {
        let size = size_of::<T>();
        unsafe {
            if self.vector.length == 0 {
                let exclusive_page = (*page).allocate_exclusive_page();
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
                    (*page).allocate_raw(self.vector.length * size, align_of::<T>()) as *mut T;
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
