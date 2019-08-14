extern crate libc;

use self::libc::c_void;
use self::libc::{read, write, STDIN_FILENO, STDOUT_FILENO};
use containers::String;
use io::{Disposable, Stream};
use memory::region::Region;
use memory::Page;

pub struct Console {}
impl Console {
    pub fn write(_pr: &Region, s: String) {
        let _r = Region::create(_pr);
        unsafe {
            write(
                STDOUT_FILENO,
                s.to_c_string() as *const c_void,
                s.get_length(),
            );
        }
    }

    pub fn open_standard_output(_rp: *mut Page) -> *mut Stream {
        unsafe { (*_rp).allocate(ConsoleStream {}) }
    }
}

pub struct ConsoleStream {}

impl Disposable for ConsoleStream {
    fn dispose(&self) {}
}

impl Stream for ConsoleStream {
    fn read_byte(&self) -> u8 {
        let mut the_byte: u8 = 0;
        unsafe {
            read(STDIN_FILENO, &mut the_byte as *mut u8 as *mut c_void, 1);
        }
        the_byte
    }

    fn write_byte(&self, the_byte: u8) {
        unsafe {
            write(STDOUT_FILENO, &the_byte as *const u8 as *const c_void, 1);
        }
    }
}

#[test]
fn test_console() {
    use memory::Heap;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let r1 = Region::create_from_page(Page::get(root_stack_bucket as usize));
    {
        let root_page = Page::get(root_stack_bucket as usize);
        Console::write(&r1, String::from_string_slice(root_page, "Scaly>"));
        // let stdout = Console::open_standard_output(root_page);
        // unsafe {
        //     let byte1 = (*stdout).read_byte();
        //     let byte2 = (*stdout).read_byte();
        //     let byte3 = (*stdout).read_byte();
        //     (*stdout).write_byte(byte1);
        //     (*stdout).write_byte(byte2);
        //     (*stdout).write_byte(byte3);
        // }
    }
}