use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::heapbucket::HeapBucket;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use std::alloc::alloc_zeroed;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;

pub struct Pool {
    root_map: usize,
    maps: *mut usize,
    pointers: *mut HeapBucket,
    pub current_bucket: *mut HeapBucket,
}

const MAPS_SIZE: usize = BUCKET_PAGES * BUCKET_PAGES / size_of::<u8>();
const POINTERS_SIZE: usize = BUCKET_PAGES * BUCKET_PAGES * size_of::<*const Bucket>();

impl Pool {
    pub fn create() -> Pool {
        unsafe {
            let maps_memory = alloc_zeroed(Layout::from_size_align_unchecked(MAPS_SIZE, PAGE_SIZE));
            if maps_memory == null_mut() {
                panic!("Unable to allocate maps memory: Out of memory.");
            }
            let pointers_memory =
                alloc_zeroed(Layout::from_size_align_unchecked(POINTERS_SIZE, PAGE_SIZE));
            if pointers_memory == null_mut() {
                panic!("Unable to allocate maps memory: Out of memory.");
            }
            // println!(
            //     "pool.maps:{:X}, pool.pointers:{:X}",
            //     maps_memory as usize, pointers_memory as usize
            // );
            Pool {
                root_map: 0,
                maps: maps_memory as *mut usize,
                pointers: pointers_memory as *mut HeapBucket,
                current_bucket: null_mut(),
            }
        }
    }

    pub fn allocate_bucket(&mut self) {
        println!("Pool allocating bucket.");
        self.current_bucket = HeapBucket::create(self as *mut Pool);
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        if self.current_bucket == null_mut() {
            self.allocate_bucket();
        };
        unsafe {
            //println!("Pool current_bucket {:X}", self.current_bucket as usize);
            let page = (*self.current_bucket).allocate_page();
            //println!("Pool allocated page at {:X}", page as usize);
            page
        }
    }
}

impl Drop for Pool {
    fn drop(&mut self) {
        if self.root_map != 0 {
            panic!("Pool is not empty!")
        }
        unsafe {
            //println!("Pool: dealloc {:X}", self.maps as *const u8 as usize);
            dealloc(
                self.maps as *mut u8,
                Layout::from_size_align_unchecked(MAPS_SIZE, PAGE_SIZE),
            );
            //println!("Pool: dealloc {:X}", self.pointers as *const u8 as usize);
            dealloc(
                self.pointers as *mut u8,
                Layout::from_size_align_unchecked(POINTERS_SIZE, PAGE_SIZE),
            );
        }
    }
}
