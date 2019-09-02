extern crate libc;
use self::libc::{
    __errno_location, c_int, c_void, close, open, read, unlink, write, O_CREAT, O_RDONLY, O_TRUNC,
    O_WRONLY,
};
use containers::String;
use io::{Disposable, IoError, Stream};
use memory::Page;

pub struct File {}
impl File {
    pub fn open_read(_rp: *mut Page, path: String) -> Result<*mut Stream, IoError> {
        unsafe {
            let file_path = path.to_c_string();
            let file_descriptor = open(file_path, O_RDONLY);
            if file_descriptor == -1 {
                return Err(IoError {
                    error_code: (*__errno_location() as i32),
                });
            }
            Ok((*_rp).allocate(FileStream {
                file_descriptor: file_descriptor,
            }))
        }
    }

    pub fn open_write(_rp: *mut Page, path: String) -> Result<*mut Stream, IoError> {
        unsafe {
            let file_path = path.to_c_string();
            let file_descriptor = open(file_path, O_CREAT | O_TRUNC | O_WRONLY, 0o644);
            if file_descriptor == -1 {
                return Err(IoError {
                    error_code: (*__errno_location() as i32),
                });
            }
            Ok((*_rp).allocate(FileStream {
                file_descriptor: file_descriptor,
            }))
        }
    }

    pub fn delete(path: String) -> Result<(), IoError> {
        unsafe {
            let file_path = path.to_c_string();
            let ret = unlink(file_path);
            if ret == -1 {
                return Err(IoError {
                    error_code: (*__errno_location() as i32),
                });
            };
            Ok(())
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
        match File::open_read(
            root_page,
            String::from_string_slice(root_page, "/tmp/0.scaly"),
        ) {
            Ok(file_in) => unsafe {
                let _file_in_disposer = Disposer { stream: file_in };
                let byte1 = (*file_in).read_byte();
                let byte2 = (*file_in).read_byte();
                let byte3 = (*file_in).read_byte();
                match File::open_write(
                    root_page,
                    String::from_string_slice(root_page, "/tmp/1.scaly"),
                ) {
                    Ok(file_out) => {
                        let _file_out_disposer = Disposer { stream: file_out };
                        (*file_out).write_byte(byte1 as u8);
                        (*file_out).write_byte(byte2 as u8);
                        (*file_out).write_byte(byte3 as u8);
                    }
                    Err(_) => (),
                }
            },
            Err(_) => (),
        };

        match File::delete(String::from_string_slice(root_page, "/tmp/1.scaly")) {
            Ok(_) => (),
            Err(_) => (),
        }
    }
}
