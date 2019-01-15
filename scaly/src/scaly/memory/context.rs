use scaly::memory::page::_Page;
use scaly::memory::page::_PAGE_SIZE;

pub const _STACK_PAGES: isize = 0x1000;

pub struct _Context {
    current_page: *mut _Page,
    top_page: *mut _Page,
}

impl _Context {
    pub fn initialize(base_page: *mut _Page) -> _Context {
        _Context {
            current_page: base_page,
            top_page: (base_page as usize + _STACK_PAGES as usize * _PAGE_SIZE as usize) as *mut _Page,
        }
    }

    pub fn allocate_page(&mut self) -> *mut _Page {
        let page = self.current_page;
        self.current_page = (self.current_page as usize + _PAGE_SIZE) as *mut _Page;
        if self.current_page == self.top_page {
            panic!("Page stack overflow.");
        }
        page
    }

    pub fn deallocate_page(&mut self) {
        self.current_page = (self.current_page as usize - _PAGE_SIZE) as *mut _Page;
    }
}

#[test]
fn test_context() {
    use std::ptr::null_mut;
    use std::alloc::alloc;
    use std::alloc::Layout;

    unsafe {
        let memory = alloc(Layout::from_size_align_unchecked(_PAGE_SIZE * _STACK_PAGES as usize, _PAGE_SIZE));
        assert_ne!(memory, null_mut());
        let mut context = _Context::initialize(memory as *mut _Page);

        for _ in 0.._STACK_PAGES - 1 {
            let _ = context.allocate_page();
        }

        for _ in 0.._STACK_PAGES - 1 {
            let _ = context.deallocate_page();
        }
        let first_page = context.allocate_page();
        assert_eq!(first_page as usize, memory as usize);
    }
}