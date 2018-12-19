use std::cell::Cell;

pub struct _Page {
    current_page: Cell<*mut u8>,
    next_object_offset: i32,
    exclusive_pages: i32
}

impl _Page {

    pub fn reset(&mut self) {
        // *getExtensionPageLocation() = 0;
        self.next_object_offset = 0;
        self.exclusive_pages = 0;
        //self.current_page = Cell::new(self);
    }

    pub fn clear() {
    }

    pub fn is_oversized(&self) -> bool {
        self.current_page.as_ptr().is_null()
    }
}