use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::heap::Heap;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;

pub struct StackBucket {
    pub heap: *mut Heap,
    next_bucket: *mut StackBucket,
}

impl StackBucket {
    pub fn allocate_page(&mut self) -> *mut Page {
        unsafe { (*self.heap).allocate_page() }
    }
    pub fn new_page(page: &Page) -> *mut Page {
        let page_address = page as *const Page as usize;
        let stack_bucket_page_address = page_address & !(PAGE_SIZE * BUCKET_PAGES - 1);
        let overflowed_page_address = stack_bucket_page_address + PAGE_SIZE * BUCKET_PAGES;
        let our_page_address = page_address + PAGE_SIZE;
        if overflowed_page_address == our_page_address {
            let bucket = (stack_bucket_page_address + size_of::<Page>()) as *mut Bucket;
            let stack_bucket = unsafe {
                match *bucket {
                    Bucket::Stack(ref mut s) => s,
                    Bucket::Heap(_) => panic!("Oops, we have a heap bucket which cannot happen."),
                }
            };
            (*stack_bucket).get_page_from_next_bucket()
        } else {
            let our_page = our_page_address as *mut Page;
            unsafe {
                (*our_page).reset();
            }
            our_page
        }
    }

    fn get_page_from_next_bucket(self: &mut StackBucket) -> *mut Page {
        // println!("get_page_from_next_bucket");
        if self.next_bucket == null_mut() {
            self.next_bucket = StackBucket::create(self.heap);
        }
        // println!("self.next_bucket: {:X}", self.next_bucket as usize);
        Page::get(self.next_bucket as usize)
    }

    pub fn create(heap: *mut Heap) -> *mut StackBucket {
        unsafe {
            // println!("BEFORE Bucket::allocate_bucket");
            let page = StackBucket::allocate_bucket();
            let bucket = (*page).allocate(Bucket::Stack(StackBucket {
                next_bucket: null_mut(),
                heap: heap,
            }));
            // println!("Bucket object:{:X}", bucket as usize);
            let stack_bucket = match *bucket {
                Bucket::Stack(ref mut s) => s,
                Bucket::Heap(_) => panic!("Oops, we have a heap bucket which cannot happen."),
            };
            let stack_bucket_pointer = stack_bucket as *const StackBucket as *mut StackBucket;
            //println!("Stack bucket object:{:X}", stack_bucket_pointer as usize);
            stack_bucket_pointer
        }
    }
    pub fn allocate_bucket() -> *mut Page {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                PAGE_SIZE * BUCKET_PAGES,
                PAGE_SIZE * BUCKET_PAGES,
            ));
            if memory == null_mut() {
                panic!("Unable to crete a new stack bucket: Out of memory.");
            }
            //println!("bucket memory:{:X}", memory as usize);
            let page = memory as *mut Page;
            (*page).reset();
            page
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
                // println!("StackBucket: dealloc {:X}", page as usize);
                bucket = next_bucket;
            }
        }
    }
}

impl Drop for StackBucket {
    fn drop(&mut self) {
        self.deallocate();
    }
}
