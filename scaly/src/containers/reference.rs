use memory::Page;
use std::ops::Deref;
use std::ops::DerefMut;

pub struct Ref<T> {
    pub data: *mut T,
}

impl<T> Ref<T> {
    pub fn new(page: *mut Page, object: T) -> Ref<T> {
        Ref {
            data: unsafe { (*page).allocate(object) },
        }
    }

    pub fn get_page(&self) -> *mut Page {
        Page::get(self.data as usize)
    }
}

impl<T> Deref for Ref<T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.data }
    }
}

impl<T> DerefMut for Ref<T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.data }
    }
}
