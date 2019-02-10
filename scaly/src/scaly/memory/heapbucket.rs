use scaly::memory::bucket::Bucket;
use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::pool::Pool;
use std::usize::MAX;

pub struct HeapBucket {
    pub pool: *mut Pool,
    pub map: usize,
}

impl HeapBucket {
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
                (*self.pool).mark_as_full(Page::get(self as *const HeapBucket as usize) as usize);
                (*self.pool).allocate_page();
            }
        }

        let position = Bucket::find_least_position(self.map);
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
        if self.map == MAX {
            unsafe {
                (*self.pool).mark_as_free(base_page);
            }
        }
    }
}

#[test]
fn test_heapbucket() {
    use scaly::memory::bucket::Bucket::Heap;
    use scaly::memory::bucket::Bucket::Stack;
    unsafe {
        let pool = Pool::create();
        println!("pool: {:X}", pool as usize);
        let bucket = Bucket::get(pool as usize);
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
        assert_eq!(
            match *bucket {
                Stack(ref _s) => 0,
                Heap(ref h) => h.map,
            },
            0
        );
        (*bucket).deallocate_page(&*page62);
        let page62a = (*bucket).allocate_page();
        assert_eq!(page62, page62a);
        assert_eq!(
            match *bucket {
                Stack(ref _s) => 0,
                Heap(ref h) => h.map,
            },
            0
        );
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
        assert_eq!(
            match *bucket {
                Stack(ref _s) => 0,
                Heap(ref h) => h.map,
            },
            0
        );
        (*bucket).deallocate_page(&*page16);
        (*bucket).deallocate_page(&*page18);
        let page16a = (*bucket).allocate_page();
        let page18a = (*bucket).allocate_page();
        assert_eq!(page16, page16a);
        assert_eq!(page18, page18a);
        assert_eq!(
            match *bucket {
                Stack(ref _s) => 0,
                Heap(ref h) => h.map,
            },
            0
        );
        (*bucket).deallocate_page(&*page63);
        let page63a = (*bucket).allocate_page();
        assert_eq!(page63, page63a);
        assert_eq!(
            match *bucket {
                Stack(ref _s) => 0,
                Heap(ref h) => h.map,
            },
            0
        );
    }
}
