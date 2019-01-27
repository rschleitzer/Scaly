use scaly::memory::page::_Page;
use scaly::memory::page::_PAGE_SIZE;

pub const _STACK_PAGES: isize = 0x100;

pub struct _Region<'a> {
    page: &'a mut _Page,
}

impl<'a> _Region<'a> {
    pub fn create(page: & _Page) -> _Region<'a> {
        let our_page_address = page as *const _Page as usize + _PAGE_SIZE;
        let base_stack_bucket_address = our_page_address & !(_PAGE_SIZE * _STACK_PAGES as usize - 1);
        let overflowed_page_address = base_stack_bucket_address + _PAGE_SIZE * _STACK_PAGES as usize;
        if overflowed_page_address == our_page_address {
            panic!("Page stack overflow.");
        }
        _Region {
            page: unsafe { &mut *(our_page_address as *mut _Page) }
        }
    }
}

impl<'a>  Drop for  _Region<'a> {
    fn drop(&mut self) {
        self.page.deallocate_extensions();
    }
}

#[test]
fn test_region() {
    use std::ptr::null_mut;
    use std::alloc::alloc;
    use std::alloc::Layout;
    use scaly::memory::page::_PAGE_SIZE;

    unsafe {
        let memory = alloc(Layout::from_size_align_unchecked(_PAGE_SIZE * _STACK_PAGES as usize, _PAGE_SIZE));
        assert_ne!(memory, null_mut());
        let r1 = _Region::create(& *(memory as *const _Page));
        {
            let r2 = _Region::create(r1.page);
            {
                let _ = _Region::create(r2.page);
                
            }
        }
    }
}
