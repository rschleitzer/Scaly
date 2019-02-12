use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::heapbucket::HeapBucket;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;
use std::usize::MAX;

pub struct Pool {
    map: usize,
}

impl Pool {
    pub fn create() -> *mut Pool {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                PAGE_SIZE * BUCKET_PAGES * BUCKET_PAGES,
                PAGE_SIZE * BUCKET_PAGES,
            ));
            // println!("Pool memory: {:X}.", memory as usize);
            if memory == null_mut() {
                panic!("Unable to crete pool: Out of memory.");
            }
            let mut pool: *mut Pool = null_mut();

            for i in 0..BUCKET_PAGES - 1 {
                let bucket_page = (memory as usize + PAGE_SIZE * BUCKET_PAGES * i) as *mut Page;
                // println!("Bucket page: {:X}.", bucket_page as usize);
                (*bucket_page).reset();
                let bucket = (*bucket_page).allocate(Bucket::Heap(HeapBucket {
                    pool: null_mut(),
                    map: MAX,
                }));
                // println!("Bucket: {:X}.", bucket as usize);
                if i == 0 {
                    let page = memory as *mut Page;
                    pool = (*page).allocate(Pool { map: MAX });
                    // println!("Pool object: {:X}.", pool as usize);
                }
                (*bucket).set_pool(pool);
            }
            pool
        }
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        //println!("Pool allocating page.");
        if self.map == 0 {
            panic!("Not more than one pool supported currently.");
        }
        let pool_page_address = Page::get(self as *const Pool as usize) as usize;
        let bucket_page_address = if self.map == MAX {
            Page::get(self as *const Pool as usize) as usize
        } else {
            let position = Bucket::find_least_position(self.map);
            pool_page_address + PAGE_SIZE * BUCKET_PAGES * position
        };
        let bucket = (bucket_page_address + size_of::<Page>()) as *mut Bucket;
        unsafe { (*bucket).allocate_page() }
    }

    fn get_allocation_bit(&self, page: usize) -> usize {
        let first_bucket_address = Page::get(self as *const Pool as usize) as usize;
        let distance = page - first_bucket_address;
        let position = distance / PAGE_SIZE / BUCKET_PAGES;
        // println!("Pool position to be marked as full: {}", position);
        if position == 0 {
            1
        } else {
            1 << (BUCKET_PAGES - position)
        }
    }

    pub fn mark_as_full(&mut self, page: usize) {
        let bit = self.get_allocation_bit(page);
        // println!("Pool bit to be marked as full: {":X"}", bit);
        self.map = self.map & !bit;
    }

    pub fn mark_as_free(&mut self, page: usize) {
        let bit = self.get_allocation_bit(page);
        self.map = self.map | bit;
    }
}

impl Drop for Pool {
    fn drop(&mut self) {
        if self.map != 0 {
            panic!("Pool is not empty!")
        }
        unsafe {
            //println!("Pool: dealloc {:X}", self.pointers as *const u8 as usize);
            dealloc(
                &*self as *const Pool as *mut u8,
                Layout::from_size_align_unchecked(
                    PAGE_SIZE * BUCKET_PAGES * BUCKET_PAGES,
                    PAGE_SIZE * BUCKET_PAGES,
                ),
            );
        }
    }
}

#[test]
fn test_pool() {
    use scaly::memory::region::Region;
    use scaly::memory::stackbucket::StackBucket;
    let pool = Pool::create();
    unsafe {
        let root_stack_bucket = StackBucket::create(pool);
        {
            let mut r = Region::create_from_page(&*Page::get(root_stack_bucket as usize));
            for i in 1..60000 {
                r.new(i);
            }
        }
    }
}
