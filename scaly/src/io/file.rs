extern crate libc;

use self::libc::{__errno_location, close, open, read, write, O_RDONLY};
use self::libc::{c_int, c_void};
use containers::String;
use io::{Disposable, Stream};
use memory::Page;

pub struct File {}
impl File {
    pub fn open_read(_rp: *mut Page, path: String) -> *mut Stream {
        unsafe {
            let file_path = path.to_c_string();
            let file_descriptor = open(file_path, O_RDONLY);
            (*_rp).allocate(FileStream {
                file_descriptor: file_descriptor,
            })
        }
    }
}

pub struct FileStream {
    file_descriptor: c_int,
}

impl Disposable for FileStream {
    fn dispose(&self) {
        unsafe {
            close(self.file_descriptor);
        }
    }
}

impl Stream for FileStream {
    fn read_byte(&self) -> i32 {
        let mut the_byte: u8 = 0;
        unsafe {
            read(
                self.file_descriptor,
                &mut the_byte as *mut u8 as *mut c_void,
                1,
            );
            if *__errno_location() != 0 {
                return -1;
            }
        }
        the_byte as i32
    }

    fn write_byte(&self, the_byte: u8) {
        unsafe {
            write(
                self.file_descriptor,
                &the_byte as *const u8 as *const c_void,
                1,
            );
        }
    }
}

#[test]
fn test_console() {
    use io::Console;
    use memory::region::Region;
    use memory::Heap;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let r1 = Region::create_from_page(Page::get(root_stack_bucket as usize));
    {
        let root_page = Page::get(root_stack_bucket as usize);
        let stdout = File::open_read(root_page, String::from_string_slice(root_page, "Scaly>"));
        unsafe {
            let byte1 = (*stdout).read_byte();
            let byte2 = (*stdout).read_byte();
            let byte3 = (*stdout).read_byte();
            let stdout = Console::open_standard_output(root_page);
            unsafe {
                (*stdout).write_byte(byte1);
                (*stdout).write_byte(byte2);
                (*stdout).write_byte(byte3);
            }
        }
    }
}
