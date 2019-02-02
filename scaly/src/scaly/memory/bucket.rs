use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::pool::Pool;
use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;

pub const BUCKET_PAGES: usize = size_of::<usize>() * 8;

pub enum Bucket<'a> {
    Stack(StackBucket<'a>),
    Heap(HeapBucket),
}

impl<'a> Bucket<'a> {
    pub fn get(address: usize) -> *mut Bucket<'a> {
        (address & !(PAGE_SIZE * BUCKET_PAGES - 1)) as *mut Bucket<'a>
    }
}

pub struct StackBucket<'a> {
    next_bucket: *mut StackBucket<'a>,
    pool: &'a mut Pool,
}

impl<'a> StackBucket<'a> {
    pub fn new_page(page: &Page) -> *mut Page {
        let page_address = page as *const Page as usize;
        let stack_bucket_page_address = page_address & !(PAGE_SIZE * BUCKET_PAGES - 1);
        let overflowed_page_address = stack_bucket_page_address + PAGE_SIZE * BUCKET_PAGES;
        let our_page_address = page_address + PAGE_SIZE;
        if overflowed_page_address == our_page_address {
            let stack_bucket = (stack_bucket_page_address + size_of::<Page>()) as *mut StackBucket;
            unsafe { (*stack_bucket).get_page_from_next_bucket() }
        } else {
            let our_page = our_page_address as *mut Page;
            unsafe {
                (*our_page).reset();
            }
            our_page
        }
    }

    fn get_page_from_next_bucket(self: &'a mut StackBucket<'a>) -> *mut Page {
        if self.next_bucket == null_mut() {
            self.next_bucket = StackBucket::create(self.pool);
        }
        Page::get(self.next_bucket as usize)
    }

    pub fn create(pool: &'a mut Pool) -> *mut StackBucket<'a> {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                PAGE_SIZE * BUCKET_PAGES,
                PAGE_SIZE * BUCKET_PAGES,
            ));
            if memory == null_mut() {
                panic!("Unable to crete a new stack bucket: Out of memory.");
            }
            //println!("bucket memory:{:X}", memory as usize);
            let page = &mut *(memory as *mut Page);
            page.reset();
            let bucket = page.allocate(StackBucket {
                next_bucket: null_mut(),
                pool: pool,
            });
            //println!("stack bucket object:{:X}", bucket as usize);
            bucket
        }
    }

    pub fn deallocate(&self) {
        let mut bucket = self as *const StackBucket;
        unsafe {
            while !(*bucket).next_bucket.is_null() {
                let next_bucket = (*bucket).next_bucket;
                let page = Page::get(bucket as usize);
                dealloc(
                    page as *mut u8,
                    Layout::from_size_align_unchecked(
                        PAGE_SIZE * BUCKET_PAGES,
                        PAGE_SIZE * BUCKET_PAGES,
                    ),
                );

                bucket = next_bucket;
            }
        }
    }
}

impl<'a> Drop for StackBucket<'a> {
    fn drop(&mut self) {
        self.deallocate();
    }
}

pub struct HeapBucket {
    _map: usize,
    _pool: *mut Pool,
}
