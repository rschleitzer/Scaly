use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::mem::align_of;
use std::ptr::null_mut;
use std::ptr::write;

const _PAGE_SIZE: usize = 0x1000;

pub struct _Page {
    current_page: *mut _Page,
    next_object_offset: i32,
    exclusive_pages: i32,
}

impl _Page {
    pub fn reset(&mut self) {
        unsafe {
            *(self.get_extension_page_location()) = null_mut();
        }
        self.next_object_offset = size_of::<_Page>() as i32;
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

    fn get_extension_page_location(&self) -> *mut *mut _Page {
        // Convert self to page location
        let self_location = self as *const _Page as usize;

        // Advance one page size so we are past the end of our page
        let location_behind_page = (self_location + _PAGE_SIZE) as *mut *mut _Page;

        // Go back one pointer size
        unsafe { location_behind_page.offset(-1) }
    }

    fn get_next_location(&self) -> usize {
        self as *const _Page as usize + self.next_object_offset as usize
    }

    fn set_next_location(&mut self, location: usize) {
        let self_location = self as *const _Page as usize;
        self.next_object_offset = (location - self_location as usize) as i32;
    }

    fn get_next_exclusive_page_location(&self) -> *mut *mut _Page {
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
        if self as *mut _Page != self.current_page {
            unsafe {
                // We're already known to be full, so we delegate to the current page
                let new_object = (*self.current_page).allocate_raw(size, align);

                // Possibly our current page was also full so we propagate back the new current page
                let allocating_page = _Page::get_page(new_object as usize);
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
        if size_of::<_Page>() + size + size_of::<*mut *mut _Page>() > _PAGE_SIZE {
            if self.get_next_location() >= self.get_next_exclusive_page_location() as usize {
                // Allocate an extension page and try again with it
                unsafe {
                    return (*self.allocate_extension_page()).allocate_raw(size, align);
                }
            }

            unsafe {
                let gross_size = size + size_of::<_Page>();
                // We allocate oversized objects directly.
                let memory = alloc(Layout::from_size_align_unchecked(
                    gross_size,
                    _PAGE_SIZE,
                ));

                // Initialize a _Page object at the page start
                let page = memory as *mut _Page;

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

    fn allocate_extension_page(&mut self) -> *mut _Page {
        unsafe {
            let page = _Page::allocate_page();
            *(self.get_extension_page_location()) = page;
            self.current_page = page;
            &mut *page
        }
    }

    fn allocate_page() -> *mut _Page {
        unsafe {
            let page =
                alloc(Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE)) as *mut _Page;
            (*page).reset();
            page
        }
    }

    pub fn allocate_exclusive_page(&mut self) -> *mut _Page {
        unsafe {
            if self as *mut _Page != self.current_page {
                // We're already known to be full, so we delegate to the current page
                return (*self.current_page).allocate_exclusive_page();
            }

            // Check first whether we need an ordinary extension
            if self.get_next_location() as usize >= self.get_next_exclusive_page_location() as usize
            {
                // Allocate an extension page with default size
                return (*self.allocate_extension_page()).allocate_exclusive_page();
            }

            let page = _Page::allocate_page();
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

    fn deallocate_extensions(&mut self) {
        let mut page: *mut _Page = self;
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

                if page != self as *mut _Page {
                    (*page).forget();
                }

                page = *extension_pointer;
            }
        }
    }

    pub fn forget(&self) {
        let size = if self.current_page == null_mut() {
            self.next_object_offset as usize + self.exclusive_pages as usize * 0x100000000
        } else {
            _PAGE_SIZE
        };
        unsafe {
            dealloc(
                self as *const _Page as *mut u8,
                Layout::from_size_align_unchecked(size, _PAGE_SIZE),
            )
        }
    }

    pub fn reclaim_array(&mut self, address: *mut _Page) -> bool {
        unsafe {
            let mut page: *mut _Page = self;
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

    pub fn get_page(address: usize) -> *mut _Page {
        (address & !(_PAGE_SIZE - 1)) as *mut _Page
    }

    fn deallocate_exclusive_page(&mut self, page: *mut _Page) -> bool {
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
    unsafe {
        // Allocate a page
        let memory = alloc(Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE));
        assert_ne!(memory, null_mut());

        // Write some numbers to the page memory
        for i in 0.._PAGE_SIZE {
            let ptr = memory.offset(i as isize);
            *ptr = i as u8
        }

        // Initialize a _Page object at the page start
        let page = &mut *(memory as *mut _Page);

        assert_ne!(page.next_object_offset, 0);
        assert_ne!(page.exclusive_pages, 0);

        page.reset();

        assert_eq!(page.next_object_offset, size_of::<_Page>() as i32);
        assert_eq!(page.exclusive_pages, 0);
        assert_eq!(page.is_oversized(), false);
        assert_eq!(page as *mut _Page, page.current_page);

        {
            let extension_page_location = page.get_extension_page_location();
            assert_eq!(
                extension_page_location as usize,
                page as *const _Page as usize + _PAGE_SIZE - size_of::<*mut *mut _Page>()
            );
        }

        {
            let mut location = page.get_next_location();
            assert_eq!(
                location as usize,
                page as *const _Page as usize + size_of::<_Page>()
            );

            let answer = page.allocate(42);
            location += 4;
            assert_eq!(page.get_next_location(), location);

            let another = page.allocate_raw(1, 2);
            location += 1;
            assert_eq!(page.get_next_location(), location);
            *another = 43;

            let eau = page.allocate_raw(4, 4) as *mut i32;
            location += 7;
            assert_eq!(page.get_next_location(), location);
            *eau = 4711;
            assert_eq!(_Page::get_page(eau as usize), page as *mut _Page);

            // Allocate an oversized page which should cause allocating an exclusive page
            let array = page.allocate_raw(_PAGE_SIZE, 8) as *mut usize;
            for i in 0.._PAGE_SIZE / size_of::<usize>() - 1 {
                *(array.offset(i as isize)) = i;
            }
            assert_eq!(page.get_next_location(), location);
            assert_eq!(page as *mut _Page, page.current_page);

            // Overflow the page
            for _ in 0.._PAGE_SIZE {
                page.allocate_raw(1, 1);
            }

            assert_ne!(page as *mut _Page, page.current_page);
            assert_eq!(*(page.get_extension_page_location()), page.current_page);
            assert_eq!((*page.current_page).exclusive_pages, 0);
            assert_eq!((*page.current_page).current_page, page.current_page);

            assert_eq!(page.exclusive_pages, 1);
            assert_eq!(*answer, 42);
            assert_eq!(*another, 43);
            assert_eq!(*eau, 4711);

            let exclusive_page = page.get_next_exclusive_page_location().offset(1);
            assert_eq!((**exclusive_page).current_page, null_mut());
            assert_eq!((**exclusive_page).exclusive_pages, 0);
            assert_eq!((**exclusive_page).next_object_offset as usize, size_of::<_Page>() + _PAGE_SIZE );
            assert_eq!(_Page::get_page(array as usize), *exclusive_page);

            let success = page.reclaim_array(_Page::get_page(array as usize));
            assert_eq!(success, true);
            assert_eq!(page.exclusive_pages, 0);

            page.clear();
            page.forget();
        }
    }
}
