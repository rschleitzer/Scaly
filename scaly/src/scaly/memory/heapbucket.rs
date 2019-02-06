use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::Bucket::Heap;
use scaly::memory::bucket::Bucket::Stack;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::pool::Pool;
use std::mem::size_of;
use std::usize::MAX;

pub struct HeapBucket {
    pool: *mut Pool,
    map: usize,
}

impl HeapBucket {
    pub fn create(pool: *mut Pool) -> *mut HeapBucket {
        unsafe {
            let page = Bucket::allocate_bucket();
            let bucket = (*page).allocate(Bucket::Heap(HeapBucket {
                pool: pool,
                map: MAX,
            }));
            //println!("Bucket object:{:X}", bucket as usize);
            let heap_bucket = match *bucket {
                Heap(ref mut h) => h,
                Stack(_) => panic!("Oops, we have a heap bucket which cannot happen."),
            };
            (*pool).current_bucket = heap_bucket;
            let heap_bucket_pointer = heap_bucket as *const HeapBucket as *mut HeapBucket;
            //println!("Stack bucket object:{:X}", stack_bucket_pointer as usize);
            heap_bucket_pointer
        }
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        if self.map == MAX {
            self.map = self.map - 1;
            let page = Page::get(self as *const HeapBucket as usize);
            println!("HeapBucket allocated first page:{:X}", page as usize);
            return page;
        }

        if self.map == 0 {
            unsafe {
                //println!("self.pool: {:X}", self.pool as usize);
                if self as *mut HeapBucket == (*self.pool).current_bucket {
                    //println!("Let the pool allocate a new one.");
                    (*self.pool).allocate_bucket();
                }
                return (*(*self.pool).current_bucket).allocate_page();
            }
        }

        let position = HeapBucket::find_least_position(self.map);
        println!("HeapBucket position: {}", position);
        self.map = self.map & !(1 << size_of::<usize>() * 8 - position);

        let page = (Page::get(self as *const HeapBucket as usize) as usize + position * PAGE_SIZE)
            as *mut Page;
        println!("HeapBucket allocated page:{:X}", page as usize);
        unsafe { (*page).reset() }
        page
    }

    pub fn deallocate_page(&mut self, page: &Page) {
        let base_page = Page::get(self as *const HeapBucket as usize) as usize;
        println!("HeapBucket about to de-allocate a page:{:X}", base_page);
        let distance = page as *const Page as usize - base_page;
        let position = distance / PAGE_SIZE;
        println!("Position to de-allocate: {}", position);
        if position > size_of::<usize>() * 8 - 1 {
            panic!("Position invalid for page {:X}", base_page)
        };
        let bit = 1 << size_of::<usize>() * 8 - position - 1;
        println!("Bit: {:X}", bit);
        if self.map & bit != 0 {
            panic!("Page {:X} was not allocated!", base_page)
        }
        self.map = self.map & bit;
    }

    fn find_least_position(n: usize) -> usize {
        match size_of::<usize>() * 8 {
            64 => HeapBucket::find_least_position_64(n),
            32 => HeapBucket::find_least_position_32(n),
            _ => panic!(
                "Scaly currently does not support {}bit architecture.",
                size_of::<usize>() * 8
            ),
        }
    }

    // from http://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
    fn find_least_position_64(n: usize) -> usize {
        static NUMBERS: &'static [usize] = &[
            0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20,
            55, 30, 34, 11, 43, 14, 22, 4, 62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10,
            13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63,
        ];

        let n = n | n >> 1;
        let n = n | n >> 2;
        let n = n | n >> 4;
        let n = n | n >> 8;
        let n = n | n >> 16;
        let n = n | n >> 32;

        let product = n as u128 * 0x03f6eaf2cd271461;

        let log = NUMBERS[product as usize >> 58];
        size_of::<usize>() * 8 - log
    }

    // from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
    fn find_least_position_32(n: usize) -> usize {
        static NUMBERS: &'static [usize] = &[
            0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24,
            7, 19, 27, 23, 6, 26, 5, 4, 31,
        ];

        let n = n | n >> 1;
        let n = n | n >> 2;
        let n = n | n >> 4;
        let n = n | n >> 8;
        let n = n | n >> 16;

        let log = NUMBERS[(n * 0x07C4ACDD) >> 27];
        size_of::<usize>() * 8 - log
    }
}

#[test]
fn test_heapbucket() {
    unsafe {
        let mut pool = Pool::create();
        //println!("pool: {:X}", &pool as *const Pool as usize);
        let bucket = HeapBucket::create(&mut pool);
        let page0 = (*bucket).allocate_page();
        let page1 = (*bucket).allocate_page();
        let page2 = (*bucket).allocate_page();
        let page3 = (*bucket).allocate_page();
        let page4 = (*bucket).allocate_page();
        let page5 = (*bucket).allocate_page();
        let page6 = (*bucket).allocate_page();
        let page7 = (*bucket).allocate_page();
        let page8 = (*bucket).allocate_page();
        let page9 = (*bucket).allocate_page();
        (*page1).forget();
        (*page5).forget();
        (*page4).forget();
        (*page3).forget();
        (*page8).forget();
        (*page0).forget();
        (*page7).forget();
        (*page6).forget();
        (*page2).forget();
        (*page9).forget();
        assert_eq!((*bucket).map, 0);
    }
}
