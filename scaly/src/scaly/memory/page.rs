use scaly::memory::Bucket;
use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::align_of;
use std::mem::size_of;
use std::ptr::null_mut;
use std::ptr::write;

pub const PAGE_SIZE: usize = 0x1000;

pub struct Page {
    current_page: *mut Page,
    next_object_offset: i32,
    exclusive_pages: i32,
}

impl Page {
    pub fn reset(&mut self) {
        unsafe {
            *(self.get_extension_page_location()) = null_mut();
        }
        self.next_object_offset = size_of::<Page>() as i32;
        self.exclusive_pages = 0;
        self.current_page = &mut *self;
    }

    pub fn clear(&mut self) {
        self.deallocate_extensions();
        self.reset();
    }

    pub fn is_oversized(&self) -> bool {
        self.current_page.is_null()
    }

    fn get_extension_page_location(&self) -> *mut *mut Page {
        // Convert self to page location
        let self_location = self as *const Page as usize;

        // Advance one page size so we are past the end of our page
        let location_behind_page = (self_location + PAGE_SIZE) as *mut *mut Page;

        // Go back one pointer size
        unsafe { location_behind_page.offset(-1) }
    }

    fn get_next_location(&self) -> usize {
        self as *const Page as usize + self.next_object_offset as usize
    }

    fn set_next_location(&mut self, location: usize) {
        let self_location = self as *const Page as usize;
        self.next_object_offset = (location - self_location as usize) as i32;
    }

    fn get_next_exclusive_page_location(&self) -> *mut *mut Page {
        unsafe {
            self.get_extension_page_location()
                .offset(-(self.exclusive_pages as isize + 1))
        }
    }

    pub fn allocate<T>(&mut self, object: T) -> *mut T {
        let memory = self.allocate_raw(size_of::<T>(), align_of::<T>()) as *mut T;

        unsafe {
            // Write into uninitialized memory.
            write(memory, object);
            memory
        }
    }

    pub fn allocate_raw(&mut self, size: usize, align: usize) -> *mut u8 {
        if self as *mut Page != self.current_page {
            unsafe {
                // We're already known to be full, so we delegate to the current page
                let new_object = (*self.current_page).allocate_raw(size, align);

                // Possibly our current page was also full so we propagate back the new current page
                let allocating_page = Page::get(new_object as usize);
                if allocating_page != self.current_page && (!(*allocating_page).is_oversized()) {
                    self.current_page = allocating_page;
                }
                return new_object;
            }
        }

        // Try to allocate from ourselves
        let location = self.get_next_location();
        let aligned_location = (location + align - 1) & !(align - 1);
        let next_location = aligned_location + size;
        if next_location <= self.get_next_exclusive_page_location() as usize {
            self.set_next_location(next_location);
            return location as *mut u8;
        }

        // So the space did not fit.

        // Calculate gross size to decide whether we're oversized
        if size_of::<Page>() + size + size_of::<*mut *mut Page>() > PAGE_SIZE {
            if self.get_next_location() >= self.get_next_exclusive_page_location() as usize {
                // Allocate an extension page and try again with it
                unsafe {
                    return (*self.allocate_extension_page()).allocate_raw(size, align);
                }
            }

            unsafe {
                let gross_size = size + size_of::<Page>();
                // We allocate oversized objects directly.
                let memory = alloc(Layout::from_size_align_unchecked(gross_size, PAGE_SIZE));

                // Initialize a Page object at the page start
                let page = memory as *mut Page;

                // Oversized pages have no current_page
                (*page).current_page = null_mut();

                // Set the size since we will need it when deallocating
                (*page).next_object_offset = (gross_size % 0x100000000) as i32;
                (*page).exclusive_pages = (gross_size / 0x100000000) as i32;

                *(self.get_next_exclusive_page_location()) = page;
                self.exclusive_pages += 1;
                return page.offset(1) as *mut u8;
            }
        }

        // So we're not oversized. Create extension page and let it allocate.
        unsafe { (*self.allocate_extension_page()).allocate_raw(size, align) }
    }

    fn allocate_extension_page(&mut self) -> *mut Page {
        unsafe {
            let page = self.allocate_page();
            *(self.get_extension_page_location()) = page;
            self.current_page = page;
            &mut *page
        }
    }

    fn allocate_page(&mut self) -> *mut Page {
        unsafe {
            let bucket = Bucket::get(self as *const Page as usize);
            //println!("Bucket: {:X}", bucket as usize);
            let page = (*bucket).allocate_page();
            (*page).reset();
            page
        }
    }

    pub fn allocate_exclusive_page(&mut self) -> *mut Page {
        unsafe {
            if self as *mut Page != self.current_page {
                // We're already known to be full, so we delegate to the current page
                return (*self.current_page).allocate_exclusive_page();
            }

            // Check first whether we need an ordinary extension
            if self.get_next_location() as usize >= self.get_next_exclusive_page_location() as usize
            {
                // Allocate an extension page with default size
                return (*self.allocate_extension_page()).allocate_exclusive_page();
            }

            let page = self.allocate_page();
            *(self.get_next_exclusive_page_location()) = page;
            self.exclusive_pages += 1;
            &mut *page
        }
    }

    pub fn extend(&mut self, top: usize, size: usize) -> bool {
        if top != self.get_next_location() {
            return false;
        }

        let new_top = top + size;
        if new_top > self.get_next_exclusive_page_location() as usize {
            return false;
        }
        self.set_next_location(new_top);

        true
    }

    pub fn deallocate_extensions(&mut self) {
        let mut page: *mut Page = self;
        unsafe {
            while !page.is_null() {
                let extension_pointer = (*page).get_extension_page_location();
                for i in 1..(*page).exclusive_pages {
                    let mut exclusive_page = &mut **(extension_pointer.offset(-(i as isize)));
                    if !exclusive_page.is_oversized() {
                        exclusive_page.deallocate_extensions();
                    }
                    exclusive_page.forget();
                }

                if page != self as *mut Page {
                    (*page).forget();
                }

                page = *extension_pointer;
            }
        }
    }

    pub fn forget(&self) {
        //println!("Page: dealloc {:X}", self as *const Page as usize);
        unsafe {
            if self.current_page == null_mut() {
                dealloc(
                    self as *const Page as *mut u8,
                    Layout::from_size_align_unchecked(
                        self.next_object_offset as usize
                            + self.exclusive_pages as usize * 0x100000000,
                        PAGE_SIZE,
                    ),
                );
            } else {
                let bucket = Bucket::get(self as *const Page as usize);
                //println!("Bucket: {:X}", bucket as usize);
                (*bucket).deallocate_page(self);
            }
        };
    }

    pub fn reclaim_array(&mut self, address: *mut Page) -> bool {
        unsafe {
            let mut page: *mut Page = self;
            while !page.is_null() {
                if (*page).deallocate_exclusive_page(address) {
                    return true;
                }
                page = *(*page).get_extension_page_location();
            }
        }

        // If we arrive here, we have a memory leak.
        false
    }

    pub fn get(address: usize) -> *mut Page {
        (address & !(PAGE_SIZE - 1)) as *mut Page
    }

    fn deallocate_exclusive_page(&mut self, page: *mut Page) -> bool {
        unsafe {
            // Find the extension Page pointer
            let mut extension_pointer = self.get_extension_page_location().offset(-1);
            let next_extension_page_location = self.get_next_exclusive_page_location();
            while extension_pointer > next_extension_page_location {
                if *extension_pointer == page {
                    break;
                }
                extension_pointer = extension_pointer.offset(-1);
            }

            // Report if we could not find it
            if extension_pointer == next_extension_page_location {
                return false;
            }

            // Shift the remaining array one position up
            while extension_pointer > next_extension_page_location {
                let pp_page = extension_pointer;
                *pp_page = *(pp_page.offset(-1));
                extension_pointer = extension_pointer.offset(-1);
            }
        }

        // Make room for one more extension
        self.exclusive_pages -= 1;
        unsafe {
            (*page).forget();
        }

        true
    }
}

#[test]
fn test_page() {
    use scaly::memory::stackbucket::StackBucket;
    use scaly::memory::Pool;
    use scaly::memory::Region;
    unsafe {
        // Allocate a page
        let mut pool = Pool::create();
        let root_stack_bucket = StackBucket::create(&mut pool);
        let r = Region::create_from_page(&*Page::get(root_stack_bucket as usize));

        assert_eq!(r.page.next_object_offset, size_of::<Page>() as i32);
        assert_eq!(r.page.exclusive_pages, 0);
        assert_eq!(r.page.is_oversized(), false);
        assert_eq!(r.page as *mut Page, r.page.current_page);

        {
            let extension_page_location = r.page.get_extension_page_location();
            assert_eq!(
                extension_page_location as usize,
                r.page as *const Page as usize + PAGE_SIZE - size_of::<*mut *mut Page>()
            );
        }

        {
            let mut location = r.page.get_next_location();
            assert_eq!(
                location as usize,
                r.page as *const Page as usize + size_of::<Page>()
            );

            let _answer = r.page.allocate(42);
            location += 4;
            assert_eq!(r.page.get_next_location(), location);

            let another = r.page.allocate_raw(1, 2);
            location += 1;
            assert_eq!(r.page.get_next_location(), location);
            *another = 43;

            let eau = r.page.allocate_raw(4, 4) as *mut i32;
            location += 7;
            assert_eq!(r.page.get_next_location(), location);
            *eau = 4711;
            assert_eq!(Page::get(eau as usize), r.page as *mut Page);

            // Allocate an oversized page which should cause allocating an exclusive page
            let array = r.page.allocate_raw(PAGE_SIZE, 8) as *mut usize;
            for i in 0..PAGE_SIZE / size_of::<usize>() - 1 {
                *(array.offset(i as isize)) = i;
            }
            assert_eq!(r.page.get_next_location(), location);
            assert_eq!(r.page as *mut Page, r.page.current_page);

            // Overflow the page
            for _ in 0..PAGE_SIZE {
                r.page.allocate_raw(1, 1);
            }

            //     assert_ne!(r.page as *mut Page, r.page.current_page);
            //     assert_eq!(*(r.page.get_extension_page_location()), r.page.current_page);
            //     assert_eq!((*r.page.current_page).exclusive_pages, 0);
            //     assert_eq!((*r.page.current_page).current_page, r.page.current_page);

            //     assert_eq!(r.page.exclusive_pages, 1);
            //     assert_eq!(*answer, 42);
            //     assert_eq!(*another, 43);
            //     assert_eq!(*eau, 4711);

            //     let exclusive_page = r.page.get_next_exclusive_page_location().offset(1);
            //     assert_eq!((**exclusive_page).current_page, null_mut());
            //     assert_eq!((**exclusive_page).exclusive_pages, 0);
            //     assert_eq!(
            //         (**exclusive_page).next_object_offset as usize,
            //         size_of::<Page>() + PAGE_SIZE
            //     );
            //     assert_eq!(Page::get(array as usize), *exclusive_page);

            //     let success = r.page.reclaim_array(Page::get(array as usize));
            //     assert_eq!(success, true);
            //     assert_eq!(r.page.exclusive_pages, 0);

            //     r.page.clear();
            //     r.page.forget();
        }
    }
}
