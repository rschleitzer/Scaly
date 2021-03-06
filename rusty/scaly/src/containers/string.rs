extern crate libc;

use self::libc::{c_char, c_void};
use self::libc::{memcmp, memcpy, strlen};
use containers::hashset::hash;
use containers::hashset::Equal;
use containers::hashset::Hash;
use memory::Page;
use std::mem::size_of;
use std::ptr;
use std::ptr::write;

#[derive(Copy, Clone)]
pub struct String {
    pub data: *const u8,
}

const PACKED_SIZE: usize = size_of::<usize>() * 8 / 7;

impl String {
    pub fn create(_rp: *mut Page, data: *const u8, length: usize) -> String {
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
            let pointer = (*_rp).allocate_raw(overall_length, 1);
            ptr::copy_nonoverlapping(length_array.as_ptr(), pointer, counter + 1);
            ptr::copy_nonoverlapping(data, pointer.offset((counter + 1) as isize), length);
            String { data: pointer }
        }
    }

    pub fn new(_rp: *mut Page, string: &str) -> String {
        let length = string.len();
        String::create(_rp, string.as_ptr(), length)
    }

    pub fn copy(_rp: *mut Page, other: String) -> String {
        let mut length_array: [u8; PACKED_SIZE] = [0; PACKED_SIZE];
        let length = other.get_length();
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
            let pointer = (*_rp).allocate_raw(overall_length, 1);
            ptr::copy_nonoverlapping(length_array.as_ptr(), pointer, counter + 1);
            ptr::copy_nonoverlapping(
                other.data.offset((counter + 1) as isize),
                pointer.offset((counter + 1) as isize),
                length,
            );
            String { data: pointer }
        }
    }

    pub fn from_c_string(_rp: *mut Page, c_string: *const c_char) -> String {
        unsafe {
            let length = strlen(c_string);
            String::create(_rp, c_string as *const u8, length)
        }
    }

    pub fn from_character(_rp: *mut Page, character: char) -> String {
        String::create(_rp, &character as *const char as *const u8, 1)
    }

    #[allow(unreachable_code)]
    pub fn to_c_string(&self, _rp: *mut Page) -> *const c_char {
        let mut length: usize = 0;
        let mut bit_count = 0;
        let mut index: usize = 0;
        loop {
            if bit_count == PACKED_SIZE * 7 {
                panic!("Bad string length.");
            }

            let byte: u8 = unsafe { *(self.data.offset(index as isize)) };
            length |= ((byte & 0x7F) as usize) << bit_count;
            if (byte & 0x80) == 0 {
                break;
            }
            bit_count += 7;
            index += 1;
        }

        unsafe {
            let dest = (*_rp).allocate_raw(length + 1, 1);
            memcpy(
                dest as *mut c_void,
                self.data.offset((index + 1) as isize) as *mut c_void,
                length,
            );

            let zero = 0 as u8;
            write(dest.offset((index + 1 + length) as isize), zero);

            dest as *const c_char
        }
    }

    pub fn from_string_slice(_rp: *mut Page, s: &str) -> String {
        String::create(_rp, s.as_ptr() as *const u8, s.len())
    }

    pub fn append_character(&self, _rp: *mut Page, c: char) -> String {
        unsafe {
            let s = String::create(_rp, self.data, self.get_length() + 1);
            let data = s.data as *mut char;
            let char_pointer = data.offset((self.get_length() - 1) as isize);
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

impl Equal for String {
    fn equals(&self, other: &String) -> bool {
        let mut length: usize = 0;
        let mut bit_count = 0;
        let mut index = 0;
        loop {
            if bit_count == PACKED_SIZE * 7 {
                panic!("Bad string length.");
            }

            let byte: u8 = unsafe { *(self.data.offset(index)) };
            length |= ((byte & 0x7F) as usize) << bit_count;
            if (byte & 0x80) == 0 {
                break;
            }
            bit_count += 7;
            index += 1;
        }

        if length != other.get_length() {
            return false;
        }

        unsafe {
            memcmp(
                (self.data as *const c_void).offset(index + 1),
                (other.data as *const c_void).offset(index + 1),
                length,
            ) == 0
        }
    }
}

impl Hash for String {
    fn hash(&self) -> usize {
        hash(self.data, self.get_length())
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
        let c_string = long_string.to_c_string(root_page);
        let _string_c = String::from_c_string(root_page, c_string);

        let semi = String::new(root_page, ";");
        let dot = String::new(root_page, ".");
        assert_eq!(semi.equals(&dot), false);
        let semi2 = String::new(root_page, ";");
        assert_eq!(semi.equals(&semi2), true);
    }
}
