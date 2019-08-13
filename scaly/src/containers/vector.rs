use memory::page::Page;
use std::mem::{align_of, size_of};
use std::ptr::write;

pub struct Vector<T> {
    pub data: *mut T,
    pub length: usize,
}

impl<T> Vector<T> {
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
                //write(address, item);
            }
        }
        vector
    }
}

#[test]
fn test_hash_map() {
    use containers::Ref;
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
    let _vector = Vector::from_raw_array(
        _r_1.page,
        &[
            Ref::new(_r_1.page, String::from_string_slice(_r_1.page, "using")),
            Ref::new(_r_1.page, String::from_string_slice(_r_1.page, "namespace")),
            Ref::new(_r_1.page, String::from_string_slice(_r_1.page, "typedef")),
        ],
    );
}
