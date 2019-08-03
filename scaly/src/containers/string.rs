use memory::Region;
use std::mem::size_of;
use std::ptr;

pub struct String {
    pub data: *const u8,
}

const PACKED_SIZE: usize = size_of::<usize>() * 8 / 7;

impl String {
    pub fn create(region: &mut Region, data: *const u8, length: usize) -> String {
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
        let pointer = region.allocate(overall_length);
        unsafe {
            ptr::copy_nonoverlapping(length_array.as_ptr(), pointer, counter + 1);
            ptr::copy_nonoverlapping(data, pointer.offset((counter + 1) as isize), length);
        }
        String { data: pointer }
    }

    pub fn new(region: &mut Region, string: &str) -> String {
        let length = string.len();
        String::create(region, string.as_ptr(), length)
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
    unsafe {
        let root_stack_bucket = StackBucket::create(&mut heap);
        {
            let root_page = Page::get(root_stack_bucket as usize);
            let mut r = Region::create_from_page(&*root_page);
            let string = String::new(&mut r, "Hello world!");
            let length = string.get_length();
            assert_eq!(length, 12);
            let long_string = String::new(&mut r, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
            assert_eq!(long_string.get_length(), 130);
        }
    }
}
