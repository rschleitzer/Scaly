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

        let position = Bucket::find_least_position(self.map);
        if self.pools[position] == null_mut() {
            self.pools[position] = Pool::create();
            println!(
                "Heap allocated pool at: {:X}.",
                self.pools[position] as usize
            );
        }
        unsafe { (*self.pools[position]).allocate_page() }
    }
}

#[test]
fn test_heap() {
    let mut heap = Heap::create();
    let page = heap.allocate_page();
    for i in 0..1000 {
        unsafe {
            (*page).allocate(i);
        }
    }
}
