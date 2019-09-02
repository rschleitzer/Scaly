use io::{IoError};

pub trait Disposable {
    fn dispose(&self);
}

pub trait Stream: Disposable {
    fn read_byte(&mut self) -> i32;
    fn write_byte(&mut self, u8) -> Result<(), IoError>;
}

pub struct Disposer {
    pub stream: *mut Stream
}

impl Drop for Disposer {
    fn drop(&mut self) {
        unsafe { (*self.stream).dispose() }
    }
}

