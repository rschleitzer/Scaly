extern crate libc;
use self::libc::{
    __errno_location, c_int, c_void, close, open, read, unlink, write, O_CREAT, O_RDONLY, O_TRUNC,
    O_WRONLY,
};
use containers::{Ref, String};
use io::{Disposable, IoError, Stream};
use memory::{Page, Region};
use std::ptr::null_mut;

pub struct File {}
impl File {
    pub fn open_read(_pr: &Region, _rp: *mut Page, path: String) -> Result<*mut Stream, IoError> {
        let _r = Region::create(_pr);
        unsafe {
            let file_path = path.to_c_string(_r.page);
            let file_descriptor = open(file_path, O_RDONLY);
            if file_descriptor == -1 {
                return Err(IoError {
                    error_code: (*__errno_location() as i32),
                });
            }
            Ok((*_rp).allocate(FileStream {
                file_descriptor: file_descriptor,
                buffer: null_mut(),
                capacity: 0,
                length: 0,
                position: 0,
            }))
        }
    }

    pub fn open_write(_pr: &Region, _rp: *mut Page, path: String) -> Result<*mut Stream, IoError> {
        let _r = Region::create(_pr);
        unsafe {
            let file_path = path.to_c_string(_r.page);
            let file_descriptor = open(file_path, O_CREAT | O_TRUNC | O_WRONLY, 0o644);
            if file_descriptor == -1 {
                return Err(IoError {
                    error_code: (*__errno_location() as i32),
                });
            }
            Ok((*_rp).allocate(FileStream {
                file_descriptor: file_descriptor,
                buffer: null_mut(),
                capacity: 0,
                length: 0,
                position: 0,
            }))
        }
    }

    pub fn delete(_pr: &Region, path: String) -> Result<(), IoError> {
        let _r = Region::create(_pr);
        unsafe {
            let file_path = path.to_c_string(_r.page);
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
    buffer: *mut u8,
    capacity: usize,
    length: usize,
    position: usize,
}

impl Disposable for FileStream {
    fn dispose(&self) {
        unsafe {
            close(self.file_descriptor);
        }
    }
}

impl Stream for FileStream {
    fn read_byte(&mut self) -> Result<i32, IoError> {
        unsafe {
            if self.buffer == null_mut() {
                let exclusive_page =
                    (*Page::own(Ref::from(self as *mut FileStream))).allocate_exclusive_page();
                self.capacity = (*exclusive_page).get_capacity::<u8>();
                self.buffer = (*exclusive_page).allocate_raw(self.capacity, 1) as *mut u8;
                let bytes_read = read(
                    self.file_descriptor,
                    self.buffer as *mut c_void,
                    self.capacity,
                );
                if bytes_read == -1 {
                    return Err(IoError {
                        error_code: (*__errno_location() as i32),
                    });
                }
                if bytes_read == 0 {
                    return Ok(-1);
                }
                self.length = bytes_read as usize;
            }

            if self.position == self.length {
                let bytes_read = read(
                    self.file_descriptor,
                    self.buffer as *mut c_void,
                    self.capacity,
                );
                if bytes_read == -1 {
                    return Err(IoError {
                        error_code: (*__errno_location() as i32),
                    });
                }
                if bytes_read == 0 {
                    return Ok(-1);
                }
                self.length = bytes_read as usize;
                self.position = 0;
            }

            let the_byte = *(self.buffer.offset(self.position as isize));
            self.position += 1;
            Ok(the_byte as i32)
        }
    }

    fn write_byte(&mut self, the_byte: u8) -> Result<(), IoError> {
        unsafe {
            let bytes_written = write(
                self.file_descriptor,
                &the_byte as *const u8 as *const c_void,
                1,
            );

            if bytes_written == 1 {
                Ok(())
            } else {
                Err(IoError {
                    error_code: (*__errno_location() as i32),
                })
            }
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
        let _r = Region::create_from_page(root_page);
        match File::open_read(
            &_r,
            root_page,
            String::from_string_slice(root_page, "/tmp/0.scaly"),
        ) {
            Ok(file_in) => unsafe {
                let _file_in_disposer = Disposer { stream: file_in };
                match File::open_write(
                    &_r,
                    root_page,
                    String::from_string_slice(root_page, "/tmp/1.scaly"),
                ) {
                    Ok(file_out) => {
                        let _file_out_disposer = Disposer { stream: file_out };
                        loop {
                            match (*file_in).read_byte() {
                                Ok(the_byte) => {
                                    if the_byte == -1 {
                                        break;
                                    }
                                    match (*file_out).write_byte(the_byte as u8) {
                                        Err(_) => break,
                                        Ok(_) => (),
                                    }
                                }
                                Err(_) => break,
                            }
                        }
                    }
                    Err(_) => (),
                }
            },
            Err(_) => (),
        };

        match File::delete(&_r, String::from_string_slice(root_page, "/tmp/1.scaly")) {
            Ok(_) => (),
            Err(_) => (),
        }
    }
}
