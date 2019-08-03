use memory::bucket::Bucket;
use memory::bucket::BUCKET_PAGES;
use memory::heap::Heap;
use memory::heapbucket::HeapBucket;
use memory::page::Page;
use memory::page::PAGE_SIZE;
use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;
use std::usize::MAX;

pub struct Pool {
    pub map: usize,
    heap: *mut Heap,
}

impl Pool {
    pub fn create(heap: *mut Heap) -> *mut Pool {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                PAGE_SIZE * BUCKET_PAGES * BUCKET_PAGES,
                PAGE_SIZE * BUCKET_PAGES,
            ));
            // println!("Pool memory: {:X}.", memory as usize);
            if memory == null_mut() {
                panic!("Unable to create pool: Out of memory.");
            }
            let mut pool: *mut Pool = null_mut();

            for i in 0..BUCKET_PAGES {
                let bucket_page = (memory as usize + PAGE_SIZE * BUCKET_PAGES * i) as *mut Page;
                // println!("Bucket page: {}, {:X}.", i, bucket_page as usize);
                (*bucket_page).reset();
                let bucket = (*bucket_page).allocate(Bucket::Heap(HeapBucket {
                    pool: null_mut(),
                    map: MAX,
                }));
                // println!("Bucket initialized: {:X}.", bucket as usize);
                if i == 0 {
                    pool = (*bucket_page).allocate(Pool {
                        map: MAX,
                        heap: heap,
                    });
                    // println!("Pool object: {:X}.", pool as usize);
                    // println!("Pool map after creation: {:X}.", (*pool).map);
                }
                match *bucket {
                    Bucket::Stack(ref _s) => (),
                    Bucket::Heap(ref mut h) => h.pool = pool,
                }
            }
            pool
        }
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        // println!("Pool map before allocation: {:X}.", self.map);
        if self.map == 0 {
            unsafe {
                (*self.heap).mark_as_full(self);
                return (*self.heap).allocate_page();
            };
        }
        let pool_page_address = Page::get(self as *const Pool as usize) as usize;
        // println!("Pool page address: {:X}.", pool_page_address);
        let bucket_page_address = {
            let position = Bucket::find_least_position(self.map);
            // println!("Pool least position: {}.", position);
            pool_page_address + PAGE_SIZE * BUCKET_PAGES * (position - 1)
        };
        // println!("Bucket page address: {:X}.", bucket_page_address);
        let bucket = (bucket_page_address + size_of::<Page>()) as *mut Bucket;
        // println!("Bucket address: {:X}.", bucket as usize);
        unsafe { (*bucket).allocate_page() }
    }

    fn get_allocation_bit(&self, page: usize) -> usize {
        let first_bucket_address = Page::get(self as *const Pool as usize) as usize;
        let distance = page - first_bucket_address;
        let position = distance / PAGE_SIZE / BUCKET_PAGES;
        1 << (BUCKET_PAGES - 1 - position)
    }

    pub fn mark_as_full(&mut self, page: usize) {
        let bit = self.get_allocation_bit(page);
        // println!("Pool bit to be marked as full: {:X}", bit);
        self.map = self.map & !bit;
    }

    pub fn mark_as_free(&mut self, page: usize) {
        let bit = self.get_allocation_bit(page);
        // println!("Pool bit to be marked as free: {:X}", bit);
        let old_map = self.map;
        self.map = self.map | bit;
        if old_map == 0 {
            unsafe {
                (*self.heap).mark_as_free(self);
            }
        }
    }

    pub fn deallocate(&mut self) {
        if self.map != MAX {
            panic!("Pool is not empty!")
        }
        unsafe {
            // println!(
            //     "Pool: dealloc {:X}",
            //     Page::get(self as *const Pool as usize) as usize
            // );
            dealloc(
                Page::get(self as *const Pool as usize) as *mut u8,
                Layout::from_size_align(
                    PAGE_SIZE * BUCKET_PAGES * BUCKET_PAGES,
                    PAGE_SIZE * BUCKET_PAGES,
                )
                .unwrap(),
            );
        }
    }
}
