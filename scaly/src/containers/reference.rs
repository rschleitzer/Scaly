use containers::hashset::{Equal, Hash};
use memory::Page;
use std::ops::Deref;
use std::ops::DerefMut;

#[derive(Copy, Clone)]
pub struct Ref<T> {
    pub data: *mut T,
}

impl<T> Ref<T> {
    pub fn new(_rp: *mut Page, object: T) -> Ref<T> {
        Ref {
            data: unsafe { (*_rp).allocate(object) },
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

impl<T: Equal<T>> Equal<T> for Ref<T> {
    fn equals(&self, other: &T) -> bool {
        unsafe { (*self.data).equals(other) }
    }
}

impl<T: Hash<T>> Hash<T> for Ref<T> {
    fn hash(&self) -> usize {
        unsafe { (*self.data).hash() }
    }
}
