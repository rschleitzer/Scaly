use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::page::Page;
use scaly::memory::pool::Pool;
use std::ptr::null_mut;
use std::usize::MAX;

pub struct Heap {
    map: usize,
    pools: [*mut Pool; BUCKET_PAGES],
}

impl Heap {
    pub fn create() -> Heap {
        Heap {
            map: MAX,
            pools: [null_mut(); BUCKET_PAGES],
        }
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        // println!("Heap map before allocation: {:X}.", self.map);
        if self.map == 0 {
            panic!("Not more than one heap supported currently.");
        }

        let index = Bucket::find_least_position(self.map) - 1;
        // println!("Heap index: {}.", index);
        if self.pools[index] == null_mut() {
            self.pools[index] = Pool::create(self);
            println!("Heap allocated pool at: {:X}.", self.pools[index] as usize);
        }
        // println!("Heap allocated pool has a map: {:X}.", unsafe {
        //     (*self.pools[index]).map
        // });
        unsafe { (*self.pools[index]).allocate_page() }
    }

    pub fn get_allocation_position(&self, pool: *mut Pool) -> usize {
        for i in 0..BUCKET_PAGES {
            if pool == self.pools[i] {
                // println!("Heap bit to be marked as full: {:X}", bit);
                return i;
            }
        }
        panic!("The pool pointer {:X} is not from my pools array!")
    }

    pub fn get_allocation_bit(&self, pool: *mut Pool) -> usize {
        1 << (BUCKET_PAGES - 1 - self.get_allocation_position(pool))
    }

    pub fn mark_as_full(&mut self, pool: *mut Pool) {
        let bit = self.get_allocation_bit(pool);
        // println!("Heap bit to be marked as full: {:X}", bit);
        self.map = self.map & !bit;
        // println!("Heap map after mark as full: {:X}.", self.map);
    }

    pub fn mark_as_free(&mut self, pool: *mut Pool) {
        let bit = self.get_allocation_bit(pool);
        // println!("Heap bit to be marked as free: {:X}", bit);
        self.map = self.map | bit;
    }

    pub fn deallocate(&mut self, pool: *mut Pool) {
        let position = self.get_allocation_position(pool);
        self.pools[position] = null_mut();
        unsafe {
            (*pool).deallocate();
        }
    }
}

#[test]
fn test_heap() {
    use scaly::memory::region::Region;
    use scaly::memory::stackbucket::StackBucket;
    let mut heap = Heap::create();
    unsafe {
        let root_stack_bucket = StackBucket::create(&mut heap);
        {
            let root_page = Page::get(root_stack_bucket as usize);
            let mut r = Region::create_from_page(&*root_page);

            let mut u: usize = 0;
            let mut _pu_start: usize = 0;
            // let mut ppu_next: *mut *mut usize = &mut pu;
            // let mut ppu_start: *mut *mut usize = null_mut();
            for i in 1..8323313 {
                let ru = r.new(u);
                if i == 1 {
                    _pu_start = ru as *mut usize as usize;
                }
                u = ru as *mut usize as usize;
                // **ppu_next = ppu as usize;
                // ppu_next = ppu;
            }
            let extension_location = r.page.get_extension_page_location();
            println!("Root extension page: {:X}", *extension_location as usize);
            let mut page = *extension_location;
            let mut page_counter = 1;
            while !page.is_null() {
                let extension_page = *(*page).get_extension_page_location();
                println!("Extension page: {:X}", extension_page as usize);
                page = extension_page;
                page_counter += 1;
                println!("Pages counted: {}", page_counter);
            }
        }
    }
    if heap.map != MAX {
        panic!("Heap not empty at the end.")
    }
}
