use containers::{Array, Ref, String};
use memory::page::Page;

#[derive(Copy, Clone)]
pub struct StringBuilder {
    buffer: Array<u8>,
}

impl StringBuilder {
    pub fn new(_rp: *mut Page) -> Ref<StringBuilder> {
        Ref::new(
            _rp,
            StringBuilder {
                buffer: Array::new(),
            },
        )
    }

    pub fn from_character(_rp: *mut Page, character: char) -> Ref<StringBuilder> {
        let mut string_builder = StringBuilder::new(_rp);
        string_builder.append_character(character);
        string_builder
    }

    pub fn append_character(&mut self, character: char) {
        self.buffer.add(character as u8);
    }

    pub fn get_length(&self) -> usize {
        return self.buffer.get_length();
    }

    pub fn to_string(&self, _rp: *mut Page) -> String {
        String::create(_rp, self.buffer.get_buffer(), self.buffer.get_length())
    }
}
