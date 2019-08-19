use containers::{Array, Ref, String};
use memory::page::Page;

#[derive(Copy, Clone)]
pub struct StringBuilder {
    buffer: Array<u8>,
}

impl StringBuilder {
    pub fn from_character(_rp: *mut Page, character: char) -> Ref<StringBuilder> {
        let mut string_builder = Ref::new(_rp, StringBuilder{ buffer: Array::new() });
        string_builder.append_character(character);
        string_builder
    }

    pub fn append_character(&mut self, character: char) {
        self.buffer.add(character as u8);
    }

    pub fn to_string(&self, _rp: *mut Page) -> String {
        String::create(_rp, self.buffer.get_buffer(), self.buffer.get_length())
    }
}