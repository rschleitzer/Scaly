extern crate libc;

use self::libc::c_char;
use self::libc::strlen;
use memory::Page;
use std::mem::size_of;
use std::ptr;
use std::ptr::write;

pub struct String {
    pub data: *const u8,
}

const PACKED_SIZE: usize = size_of::<usize>() * 8 / 7;

impl String {
    pub fn create(page: *mut Page, data: *const u8, length: usize) -> String {
        let mut length_array: [u8; PACKED_SIZE] = [0; PACKED_SIZE];
        let mut rest = length;
        let mut counter: usize = 0;
        while rest >= 0x80 {
            length_array[counter] = rest as u8 | 0x80;
            rest >>= 7;
            counter += 1;
        }
        length_array[counter] = rest as u8;
        let overall_length = counter + 1 + length;
        unsafe {
            let pointer = (*page).allocate_raw(overall_length, 1);
            ptr::copy_nonoverlapping(length_array.as_ptr(), pointer, counter + 1);
            ptr::copy_nonoverlapping(data, pointer.offset((counter + 1) as isize), length);
            String { data: pointer }
        }
    }

    pub fn new(page: *mut Page, string: &str) -> String {
        let length = string.len();
        String::create(page, string.as_ptr(), length)
    }

    pub fn from_c_string(page: *mut Page, c_string: *const c_char) -> String {
        unsafe {
            let length = strlen(c_string);
            String::create(page, c_string as *const u8, length)
        }
    }

    pub fn to_c_string(&self) -> *const c_char {
        self.data as *const c_char
    }

    pub fn from_string_slice(page: *mut Page, s: &str) -> String {
        String::create(page, s.as_ptr() as *const u8, s.len())
    }

    pub fn append_character(&self, page: *mut Page, c: char) -> String {
        unsafe {
            let length = self.get_length() + 1;
            let s = String::create(page, self.data, length);
            let data = s.data as *mut char;
            let char_pointer = data.offset(length as isize);
            write(char_pointer, c);
            s
        }
    }

    pub fn get_length(&self) -> usize {
        let mut result: usize = 0;
        let mut bit_count = 0;
        let mut index = 0;
        loop {
            if bit_count == PACKED_SIZE * 7 {
                panic!("Bad string length.");
            }

            let byte: u8 = unsafe { *(self.data.offset(index)) };
            result |= ((byte & 0x7F) as usize) << bit_count;
            if (byte & 0x80) == 0 {
                break;
            }
            bit_count += 7;
            index += 1;
        }
        result
    }
}

#[test]
fn test_string() {
    use memory::Heap;
    use memory::Page;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    {
        let root_page = Page::get(root_stack_bucket as usize);
        let string = String::new(root_page, "Hello world!");
        let length = string.get_length();
        assert_eq!(length, 12);
        let long_string = String::new(root_page, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
        assert_eq!(long_string.get_length(), 130);
    }
}
