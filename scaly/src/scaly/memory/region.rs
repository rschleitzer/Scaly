use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use std::alloc::alloc;
use std::alloc::alloc_zeroed;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;

const STACK_BUCKET_PAGES: usize = 0x100;

pub struct Region<'a> {
    page: &'a mut Page,
}

impl<'a> Region<'a> {
    pub fn create_from_page(page: &Page) -> Region<'a> {
        let our_page_address = StackBucket::new_page(page);
        Region {
            page: unsafe { &mut *(our_page_address as *mut Page) },
        }
    }
    pub fn create(region: &Region<'a>) -> Region<'a> {
        Region::create_from_page(region.page)
    }
}

impl<'a> Drop for Region<'a> {
    fn drop(&mut self) {
        self.page.deallocate_extensions();
    }
}

pub enum Bucket<'a> {
    Stack(StackBucket<'a>),
    Heap(HeapBucket),
}

pub struct HeapBucket {
    map: usize,
}

pub struct Pool {
    root_map: usize,
    maps: *mut usize,
    pointers: *mut HeapBucket,
}

const MAPS_SIZE: usize = size_of::<usize>() * 8 * size_of::<usize>();
const POINTERS_SIZE: usize = MAPS_SIZE * 8 * size_of::<usize>();

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
            Pool {
                root_map: 0,
                maps: maps_memory as *mut usize,
                pointers: pointers_memory as *mut HeapBucket,
            }
        }
    }
}

impl Drop for Pool {
    fn drop(&mut self) {
        if self.root_map != 0 {
            panic!("Pool is not empty!")
        }
        unsafe {
            dealloc(
                self.maps as *mut u8,
                Layout::from_size_align_unchecked(MAPS_SIZE, PAGE_SIZE),
            );
            dealloc(
                self.pointers as *mut u8,
                Layout::from_size_align_unchecked(POINTERS_SIZE, PAGE_SIZE),
            );
        }
    }
}

pub struct StackBucket<'a> {
    next_bucket: *mut StackBucket<'a>,
    pool: &'a mut Pool,
}

impl<'a> StackBucket<'a> {
    pub fn new_page(page: &Page) -> *mut Page {
        let page_address = page as *const Page as usize;
        let stack_bucket_page_address = page_address & !(PAGE_SIZE * STACK_BUCKET_PAGES - 1);
        let overflowed_page_address = stack_bucket_page_address + PAGE_SIZE * STACK_BUCKET_PAGES;
        let our_page_address = page_address + PAGE_SIZE;
        if overflowed_page_address == our_page_address {
            let stack_bucket = (stack_bucket_page_address + size_of::<Page>()) as *mut StackBucket;
            unsafe { (*stack_bucket).get_page_from_next_bucket() }
        } else {
            our_page_address as *mut Page
        }
    }

    fn get_page_from_next_bucket(self: &'a mut StackBucket<'a>) -> *mut Page {
        if self.next_bucket == null_mut() {
            self.next_bucket = StackBucket::create(self.pool);
        }
        Page::get_page(self.next_bucket as usize)
    }

    fn create(pool: &'a mut Pool) -> *mut StackBucket<'a> {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                PAGE_SIZE * STACK_BUCKET_PAGES,
                PAGE_SIZE * STACK_BUCKET_PAGES,
            ));
            if memory == null_mut() {
                panic!("Unable to crete a new stack bucket: Out of memory.");
            }
            let page = &mut *(memory as *mut Page);
            page.reset();
            let bucket = page.allocate(StackBucket {
                next_bucket: null_mut(),
                pool: pool,
            });
            bucket
        }
    }

    pub fn deallocate(&self) {
        let mut bucket = self as *const StackBucket;
        unsafe {
            while !(*bucket).next_bucket.is_null() {
                let next_bucket = (*bucket).next_bucket;
                let page = Page::get_page(bucket as usize);
                dealloc(
                    page as *mut u8,
                    Layout::from_size_align_unchecked(
                        PAGE_SIZE * STACK_BUCKET_PAGES,
                        PAGE_SIZE * STACK_BUCKET_PAGES,
                    ),
                );

                bucket = next_bucket;
            }
        }
    }
}

impl<'a> Drop for StackBucket<'a> {
    fn drop(&mut self) {
        self.deallocate();
    }
}

#[test]
fn test_region() {
    unsafe {
        let mut pool = Pool::create();
        let root_stack_bucket = StackBucket::create(&mut pool);
        let r1 = Region::create_from_page(&*Page::get_page(root_stack_bucket as usize));
        {
            let r2 = Region::create(&r1);
            {
                let _ = Region::create(&r2);
            }
        }
        (*root_stack_bucket).deallocate();
    }
    unsafe {
        let mut pool = Pool::create();
        let root_stack_bucket = StackBucket::create(&mut pool);

        let mut r = Region::create_from_page(&*Page::get_page(root_stack_bucket as usize));
        for _ in 1..STACK_BUCKET_PAGES * 2 {
            r = Region::create(&r);
        }

        (*root_stack_bucket).deallocate();
    }
}
