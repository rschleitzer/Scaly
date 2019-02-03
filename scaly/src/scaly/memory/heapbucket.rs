use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::Bucket::Heap;
use scaly::memory::bucket::Bucket::Stack;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::pool::Pool;
use std::alloc::alloc;
use std::alloc::Layout;

pub struct HeapBucket {
    _pool: *mut Pool,
    _map: usize,
}

impl HeapBucket {
    pub fn create(pool: *mut Pool) -> *mut HeapBucket {
        unsafe {
            let page = Bucket::allocate_bucket();
            let bucket = (*page).allocate(Bucket::Heap(HeapBucket {
                _pool: pool,
                _map: 0,
            }));
            //println!("Bucket object:{:X}", bucket as usize);
            let heap_bucket = match *bucket {
                Heap(ref mut h) => h,
                Stack(_) => panic!("Oops, we have a heap bucket which cannot happen."),
            };
            let heap_bucket_pointer = heap_bucket as *const HeapBucket as *mut HeapBucket;
            //println!("Stack bucket object:{:X}", stack_bucket_pointer as usize);
            heap_bucket_pointer
        }
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        let page =
            unsafe { alloc(Layout::from_size_align_unchecked(PAGE_SIZE, PAGE_SIZE)) as *mut Page };
        page
    }
}
