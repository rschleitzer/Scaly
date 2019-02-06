use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::Bucket::Heap;
use scaly::memory::bucket::Bucket::Stack;
use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::pool::Pool;
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
            // println!("HeapBucket allocated first page:{:X}", page as usize);
            return page;
        }

        if self.map == 0 {
            unsafe {
                // println!("self.pool: {:X}", self.pool as usize);
                if self as *mut HeapBucket == (*self.pool).current_bucket {
                    // println!("Let the pool allocate a new one.");
                    (*self.pool).allocate_bucket();
                }
                return (*(*self.pool).current_bucket).allocate_page();
            }
        }

        let position = HeapBucket::find_least_position(self.map);
        // println!("HeapBucket position: {}", position);
        let bit = 1 << (BUCKET_PAGES - position);
        // println!("Bit for allocation: {:X}", bit);
        self.map = self.map & !bit;

        let page = (Page::get(self as *const HeapBucket as usize) as usize + position * PAGE_SIZE)
            as *mut Page;
        // println!("HeapBucket allocated page:{:X}", page as usize);
        unsafe { (*page).reset() }
        page
    }

    pub fn deallocate_page(&mut self, page: &Page) {
        let base_page = Page::get(self as *const HeapBucket as usize) as usize;
        // println!("HeapBucket about to de-allocate a page:{:X}", base_page);
        let distance = page as *const Page as usize - base_page;
        let position = distance / PAGE_SIZE;
        // println!("Position to de-allocate: {}", position);
        if position > BUCKET_PAGES - 1 {
            panic!("Position invalid for page {:X}", base_page)
        };
        let bit = if position == 0 {
            1
        } else {
            1 << (BUCKET_PAGES - position)
        };
        // println!("Bit for deallocation: {:X}", bit);
        if self.map & bit != 0 {
            panic!("Page {:X} was not allocated!", base_page)
        }
        // println!("self.map before deallocation: {:X}", self.map);
        self.map = self.map | bit;
        // println!("self.map after deallocation: {:X}", self.map);
    }

    fn find_least_position(n: usize) -> usize {
        match BUCKET_PAGES {
            64 => HeapBucket::find_least_position_64(n),
            32 => HeapBucket::find_least_position_32(n),
            _ => panic!(
                "Scaly currently does not support {}bit architecture.",
                BUCKET_PAGES
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
        // println!("Log: {}", log);
        BUCKET_PAGES - log
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
        BUCKET_PAGES - log
    }
}

#[test]
fn test_heapbucket() {
    unsafe {
        let mut pool = Pool::create();
        //println!("pool: {:X}", &pool as *const Pool as usize);
        let bucket = HeapBucket::create(&mut pool);
        let _page00 = (*bucket).allocate_page();
        let _page01 = (*bucket).allocate_page();
        let page02 = (*bucket).allocate_page();
        let page03 = (*bucket).allocate_page();
        let page04 = (*bucket).allocate_page();
        let page05 = (*bucket).allocate_page();
        let _page06 = (*bucket).allocate_page();
        let _page07 = (*bucket).allocate_page();
        let _page08 = (*bucket).allocate_page();
        let _page09 = (*bucket).allocate_page();
        let _page10 = (*bucket).allocate_page();
        let _page11 = (*bucket).allocate_page();
        let _page12 = (*bucket).allocate_page();
        let _page13 = (*bucket).allocate_page();
        let _page14 = (*bucket).allocate_page();
        let _page15 = (*bucket).allocate_page();
        let page16 = (*bucket).allocate_page();
        let _page17 = (*bucket).allocate_page();
        let page18 = (*bucket).allocate_page();
        let _page19 = (*bucket).allocate_page();
        let _page20 = (*bucket).allocate_page();
        let _page21 = (*bucket).allocate_page();
        let _page22 = (*bucket).allocate_page();
        let _page23 = (*bucket).allocate_page();
        let _page24 = (*bucket).allocate_page();
        let _page25 = (*bucket).allocate_page();
        let _page26 = (*bucket).allocate_page();
        let _page27 = (*bucket).allocate_page();
        let _page28 = (*bucket).allocate_page();
        let _page29 = (*bucket).allocate_page();
        let _page30 = (*bucket).allocate_page();
        let _page31 = (*bucket).allocate_page();
        let _page32 = (*bucket).allocate_page();
        let _page33 = (*bucket).allocate_page();
        let _page34 = (*bucket).allocate_page();
        let _page35 = (*bucket).allocate_page();
        let _page36 = (*bucket).allocate_page();
        let _page37 = (*bucket).allocate_page();
        let _page38 = (*bucket).allocate_page();
        let _page39 = (*bucket).allocate_page();
        let _page40 = (*bucket).allocate_page();
        let _page41 = (*bucket).allocate_page();
        let _page42 = (*bucket).allocate_page();
        let _page43 = (*bucket).allocate_page();
        let _page44 = (*bucket).allocate_page();
        let _page45 = (*bucket).allocate_page();
        let _page46 = (*bucket).allocate_page();
        let _page47 = (*bucket).allocate_page();
        let _page48 = (*bucket).allocate_page();
        let _page49 = (*bucket).allocate_page();
        let _page50 = (*bucket).allocate_page();
        let _page51 = (*bucket).allocate_page();
        let _page52 = (*bucket).allocate_page();
        let _page53 = (*bucket).allocate_page();
        let _page54 = (*bucket).allocate_page();
        let _page55 = (*bucket).allocate_page();
        let _page56 = (*bucket).allocate_page();
        let _page57 = (*bucket).allocate_page();
        let _page58 = (*bucket).allocate_page();
        let _page59 = (*bucket).allocate_page();
        let _page60 = (*bucket).allocate_page();
        let _page61 = (*bucket).allocate_page();
        let page62 = (*bucket).allocate_page();
        let page63 = (*bucket).allocate_page();
        assert_eq!((*bucket).map, 0);
        (*bucket).deallocate_page(&*page62);
        let page62a = (*bucket).allocate_page();
        assert_eq!(page62, page62a);
        assert_eq!((*bucket).map, 0);
        (*bucket).deallocate_page(&*page02);
        (*bucket).deallocate_page(&*page03);
        (*bucket).deallocate_page(&*page04);
        (*bucket).deallocate_page(&*page05);
        let page02a = (*bucket).allocate_page();
        let page03a = (*bucket).allocate_page();
        let page04a = (*bucket).allocate_page();
        let page05a = (*bucket).allocate_page();
        assert_eq!(page02, page02a);
        assert_eq!(page03, page03a);
        assert_eq!(page04, page04a);
        assert_eq!(page05, page05a);
        assert_eq!((*bucket).map, 0);
        (*bucket).deallocate_page(&*page16);
        (*bucket).deallocate_page(&*page18);
        let page16a = (*bucket).allocate_page();
        let page18a = (*bucket).allocate_page();
        assert_eq!(page16, page16a);
        assert_eq!(page18, page18a);
        assert_eq!((*bucket).map, 0);
        (*bucket).deallocate_page(&*page63);
        let page63a = (*bucket).allocate_page();
        assert_eq!(page63, page63a);
        assert_eq!((*bucket).map, 0);
    }
}
