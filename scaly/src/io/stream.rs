pub trait Disposable {
    fn dispose(&self);
}

pub trait Stream: Disposable {
    fn read_byte(&self) -> i32;
    fn write_byte(&self, u8);
}

pub struct Disposer {
    pub stream: *mut Stream
}

impl Drop for Disposer {
    fn drop(&mut self) {
        unsafe { (*self.stream).dispose() }
    }
}

