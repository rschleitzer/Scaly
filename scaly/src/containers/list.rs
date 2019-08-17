use memory::page::Page;
use std::ptr::null_mut;

pub struct List<T: Copy> {
    head: *mut Node<T>,
}

struct Node<T: Copy> {
    element: T,
    next: *mut Node<T>,
}

impl<T: Copy> List<T> {
    pub fn new() -> Self {
        List { head: null_mut() }
    }

    pub fn add(&mut self, _page: *mut Page, element: T) {
        let new_node = unsafe {
            (*_page).allocate(Node {
                element: element,
                next: self.head,
            })
        };

        self.head = new_node;
    }
    pub fn get_iterator(&self) -> ListIterator<T> {
        ListIterator::new(self.head)
    }
}

pub struct ListIterator<T: Copy> {
    current: *mut Node<T>,
}

impl<T: Copy> ListIterator<T> {
    fn new(head: *mut Node<T>) -> ListIterator<T> {
        ListIterator { current: head }
    }
}

impl<T: Copy> Iterator for ListIterator<T> {
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        if self.current != null_mut() {
            let current = self.current;
            self.current = unsafe { (*(self.current)).next };
            Some(unsafe { (*(current)).element })
        } else {
            None
        }
    }
}

#[test]
fn test_list() {
    use containers::Ref;
    use memory::{Heap, Page, Region, StackBucket};
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let root_page = Page::get(root_stack_bucket as usize);
    let r = Region::create_from_page(root_page);
    let _r_1 = Region::create(&r);
    let mut list = Ref::new(_r_1.page, List::new());

    list.add(_r_1.page, 1);
    list.add(_r_1.page, 2);
    list.add(_r_1.page, 3);

    for i in list.get_iterator() {
        println!("Item: {}", i)
    }
}
