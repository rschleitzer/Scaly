extern crate libc;
use self::libc::{c_int, c_void, close, open, read, write, O_RDONLY};
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
            let bytes_read = read(
                self.file_descriptor,
                &mut the_byte as *mut u8 as *mut c_void,
                1,
            );
            if bytes_read == 0 {
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
fn test_file() {
    use io::stream::Disposer;
    use memory::Heap;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    {
        let root_page = Page::get(root_stack_bucket as usize);
        let file = File::open_read(
            root_page,
            String::from_string_slice(root_page, "/tmp/0.scaly"),
        );
        let _file_disposer = Disposer { stream: file };
        unsafe {
            let byte1 = (*file).read_byte();
            let byte2 = (*file).read_byte();
            let byte3 = (*file).read_byte();
            let byte4 = (*file).read_byte();
            (*file).write_byte(byte1 as u8);
            (*file).write_byte(byte2 as u8);
            (*file).write_byte(byte3 as u8);
            (*file).write_byte(byte4 as u8);
        }
    }
}
