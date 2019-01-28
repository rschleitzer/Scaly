use scaly::memory::page::_Page;
use scaly::memory::page::_PAGE_SIZE;
use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;

const _STACK_BUCKET_PAGES: usize = 0x100;

pub struct _Region<'a> {
    page: &'a mut _Page,
}

impl<'a> _Region<'a> {
    pub fn create_from_page(page: &_Page) -> _Region<'a> {
        let our_page_address = _StackBucket::new_page(page);
        _Region {
            page: unsafe { &mut *(our_page_address as *mut _Page) },
        }
    }
    pub fn create(region: &_Region<'a>) -> _Region<'a> {
        _Region::create_from_page(region.page)
    }
}

impl<'a> Drop for _Region<'a> {
    fn drop(&mut self) {
        self.page.deallocate_extensions();
    }
}

struct _StackBucket {
    next_bucket: *mut _StackBucket,
}

impl _StackBucket {
    pub fn new_page(page: &_Page) -> *mut _Page {
        let page_address = page as *const _Page as usize;
        let stack_bucket_page_address = page_address & !(_PAGE_SIZE * _STACK_BUCKET_PAGES - 1);
        _StackBucket::get_next_page(stack_bucket_page_address, page_address)
    }

    fn get_next_page(stack_bucket_page_address: usize, page_address: usize) -> *mut _Page {
        let overflowed_page_address = stack_bucket_page_address + _PAGE_SIZE * _STACK_BUCKET_PAGES;
        let our_page_address = page_address + _PAGE_SIZE;
        if overflowed_page_address == our_page_address {
            let stack_bucket =
                (stack_bucket_page_address + size_of::<_Page>()) as *mut _StackBucket;
            unsafe { (*stack_bucket).get_page_from_next_bucket() }
        } else {
            our_page_address as *mut _Page
        }
    }

    fn get_page_from_next_bucket(self: &mut _StackBucket) -> *mut _Page {
        if self.next_bucket == null_mut() {
            self.next_bucket = _StackBucket::create();
        }
        _Page::get_page(self.next_bucket as usize)
    }

    fn create() -> *mut _StackBucket {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                _PAGE_SIZE * _STACK_BUCKET_PAGES,
                _PAGE_SIZE * _STACK_BUCKET_PAGES,
            ));
            if memory == null_mut() {
                panic!("Unable to crete a new stack bucket: Out of memory.");
            }
            let page = &mut *(memory as *mut _Page);
            page.reset();
            let bucket = page.allocate(_StackBucket {
                next_bucket: null_mut(),
            });
            bucket
        }
    }

    pub fn deallocate(&self) {
        let mut bucket = self as *const _StackBucket;
        unsafe {
            while !(*bucket).next_bucket.is_null() {
                let next_bucket = (*bucket).next_bucket;
                let page = _Page::get_page(bucket as usize);
                dealloc(
                    page as *mut u8,
                    Layout::from_size_align_unchecked(
                        _PAGE_SIZE * _STACK_BUCKET_PAGES,
                        _PAGE_SIZE * _STACK_BUCKET_PAGES,
                    ),
                );

                bucket = next_bucket;
            }
        }
    }
}

impl Drop for _StackBucket {
    fn drop(&mut self) {
        self.deallocate();
    }
}

#[test]
fn test_region() {
    unsafe {
        let root_stack_bucket = _StackBucket::create();
        let r1 = _Region::create_from_page(&*_Page::get_page(root_stack_bucket as usize));
        {
            let r2 = _Region::create(&r1);
            {
                let _ = _Region::create(&r2);
            }
        }
        (*root_stack_bucket).deallocate();
    }
    unsafe {
        let root_stack_bucket = _StackBucket::create();

        let mut r = _Region::create_from_page(&*_Page::get_page(root_stack_bucket as usize));
        for _ in 1.._STACK_BUCKET_PAGES * 2 {
            r = _Region::create(&r);
        }

        (*root_stack_bucket).deallocate();
    }
}
