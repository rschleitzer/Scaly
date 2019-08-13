use memory::page::Page;
use std::mem::{align_of, size_of};
use std::ops::Index;
use std::ptr::write;

#[derive(Copy, Clone)]
pub struct Vector<T: Copy> {
    pub data: *mut T,
    pub length: usize,
}

impl<T: Copy> Vector<T> {
    pub fn new(_page: *mut Page, length: usize) -> Vector<T> {
        unsafe {
            Vector {
                data: (*_page).allocate_raw(length * size_of::<T>(), align_of::<T>()) as *mut T,
                length: length,
            }
        }
    }

    pub fn from_raw_array(_page: *mut Page, array: &[T]) -> Vector<T> {
        let vector = Vector::new(_page, array.len());
        let mut address = vector.data;
        for item in array {
            unsafe {
                write(address, *item);
                address = address.offset(1);
            }
        }
        vector
    }
}

impl<T: Copy> Index<usize> for Vector<T> {
    type Output = T;
    fn index(&self, offset: usize) -> &Self::Output {
        unsafe { &*self.data.offset(offset as isize) }
    }
}

#[test]
fn test_hash_map() {
    use containers::{Ref, String};
    use io::Console;
    use memory::{Heap, Page, Region, StackBucket};
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let root_page = Page::get(root_stack_bucket as usize);
    let r = Region::create_from_page(root_page);
    let _r_1 = Region::create(&r);
    let vector = Ref::new(
        _r_1.page,
        Vector::from_raw_array(
            _r_1.page,
            &[
                String::from_string_slice(_r_1.page, "using"),
                String::from_string_slice(_r_1.page, "namespace"),
                String::from_string_slice(_r_1.page, "typedef"),
            ],
        ),
    );
    Console::write(&_r_1, vector[0]);
    Console::write(&_r_1, vector[1]);
    Console::write(&_r_1, vector[2]);
}
