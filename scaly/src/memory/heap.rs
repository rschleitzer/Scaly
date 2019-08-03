use memory::bucket::Bucket;
use memory::bucket::BUCKET_PAGES;
use memory::page::Page;
use memory::pool::Pool;
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
            // println!("Heap allocated pool at: {:X}.", self.pools[index] as usize);
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
        panic!(
            "The pool pointer {:X} is not from my pools array!",
            pool as usize
        )
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

    pub fn empty(&self) {
        if self.map != MAX {
            panic! {"Heap map: {:X}", self.map};
        }

        for i in 0..BUCKET_PAGES {
            if self.pools[i] != null_mut() {
                unsafe {
                    (*self.pools[i]).deallocate();
                }
            }
        }
    }
}

// #[test]
// fn test_heap() {
//     use memory::region::Region;
//     use memory::stackbucket::StackBucket;
//     let mut heap = Heap::create();
//     unsafe {
//         let root_stack_bucket = StackBucket::create(&mut heap);
//         {
//             let root_page = Page::get(root_stack_bucket as usize);
//             let mut r = Region::create_from_page(&*root_page);

//             let mut u_start = 0usize;
//             let mut pu_previous: *mut usize = null_mut();
//             let mut _pu: *mut usize = null_mut();
//             let pointers = 133157244;
//             for i in 1..pointers + 1 {
//                 _pu = r.new(0usize);
//                 if i == 1 {
//                     u_start = _pu as usize;
//                 } else {
//                     *pu_previous = _pu as usize;
//                 }
//                 pu_previous = _pu;
//             }

//             // Check the pointer chain
//             let mut counter = 0;
//             let mut pu_check = u_start as *mut usize;
//             while pu_check != null_mut() {
//                 counter += 1;
//                 //println!("Address: {:X}", pu_check as usize);
//                 pu_check = (*pu_check) as *mut usize;
//             }
//             if counter != pointers {
//                 panic!("Check failed at {}.", pointers);
//             }

//             // Walk the page chain
//             let extension_location = r.page.get_extension_page_location();
//             // println!("Root extension page: {:X}", *extension_location as usize);
//             let mut page = *extension_location;
//             let mut page_counter = 0;
//             while !page.is_null() {
//                 let extension_page = *(*page).get_extension_page_location();
//                 // println!("Extension page: {:X}", extension_page as usize);
//                 page = extension_page;
//                 page_counter += 1;
//             }
//             println!("Pages counted: {}", page_counter);
//         }
//     }
//     heap.empty();
// }
