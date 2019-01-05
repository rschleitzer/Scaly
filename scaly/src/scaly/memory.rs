use std::alloc::alloc;
use std::alloc::dealloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::eq;
use std::ptr::null;
use std::ptr::null_mut;

const _PAGE_SIZE: usize = 0x1000;

pub struct _Page<'a> {
    current_page: *mut _Page<'a>,
    next_object_offset: i32,
    exclusive_pages: i32
}

impl<'a> _Page<'a> {

    pub unsafe fn reset(&mut self) {
        *(self.get_extension_page_location() as *mut *const _Page) = null();
        self.next_object_offset = 0;
        self.exclusive_pages = 0;
        
        // currentPage = this;
        self.current_page = &mut *self;
    }

    pub unsafe fn clear(&mut self) {
        self.deallocate_extensions();
        self.reset();
    }

    pub fn is_oversized(&self) -> bool {
        self.current_page.is_null()
    }

    unsafe fn get_extension_page_location(&self) -> usize {

        // Convert self to page location
        let self_location = self as *const _Page as usize;
        
        // Advance one page size so we are past the end of our page
        let location_behind_page = self_location + _PAGE_SIZE;

        // Go back one pointer size
        location_behind_page - size_of::<*const _Page>()
    }

    unsafe fn get_next_location(&self) -> usize {
        self as *const _Page as usize + self.next_object_offset as usize
    }

    unsafe fn set_next_location(&mut self, location: usize) {
        let self_location = self as *const _Page as usize;
        self.next_object_offset = (location - self_location as usize) as i32;
    }

    unsafe fn get_next_exclusive_page_location(&self) -> usize  {
        self.get_extension_page_location() - (self.exclusive_pages as usize + 1) * size_of::<*const _Page>()
    }

    pub unsafe fn allocate_raw(&mut self, size: usize, align: usize) -> *mut u8 {
        if !eq(self, self.current_page) {

            // We're already known to be full, so we delegate to the current page
            let new_object = (*self.current_page).allocate_raw(size, align);

            // Possibly our current page was also full so we propagate back the new current page
            let allocating_page = _Page::get_page(new_object);
            if !eq(allocating_page, self.current_page) && (!allocating_page.is_oversized()) {
                self.current_page = allocating_page;
            }
            return new_object;
        }

        // Try to allocate from ourselves
        let location = self.get_next_location();
        let next_location = (location + align - 1) & !(align - 1);
        if next_location <= self.get_next_exclusive_page_location() as usize {
            self.set_next_location(next_location);
            return location as *mut u8
        }

        // So the space did not fit.

        // Calculate gross size to decide whether we're oversized
        let gross_size = size_of::<_Page>() + size + size_of::<*const _Page>();
        if gross_size > _PAGE_SIZE {
            if self.get_next_location() >= self.get_extension_page_location() {

                // Allocate an extension page with default size
                let extension_page = self.allocate_extension_page();

                // Try again with the new extension page
                return extension_page.allocate_raw(size, align);
            }

            // We allocate oversized objects directly.
            let memory = alloc(Layout::from_size_align_unchecked(size + size_of::<_Page>(), _PAGE_SIZE) );

            // Initialize a _Page object at the page start
            let page = memory as *mut _Page<'a>;
            (*page).reset();

            // Oversized pages have no current_page
            (*page).current_page = null_mut();
            
            *(self.get_next_exclusive_page_location() as *mut *const _Page) = page;
            self.exclusive_pages += 1;
            return page.offset(1) as *mut u8;
        }

        // So we're not oversized. Create extension page and let it allocate.
        self.allocate_extension_page().allocate_raw(size, align)
    }

    unsafe fn allocate_extension_page(&mut self) -> &'a mut _Page<'a> {
        let page = _Page::allocate_page();
        *(self.get_extension_page_location() as *mut *const _Page) = page;
        self.current_page = page;
        &mut *page
    }

    unsafe fn allocate_page() -> *mut _Page<'a> {
        let memory = alloc(Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE));
        let page = memory as *mut _Page<'a>;
        (*page).reset();
        page
    }

    pub unsafe fn allocate_exclusive_page(&mut self) -> &'a mut _Page<'a> {

        if !eq(self, self.current_page) {
            // We're already known to be full, so we delegate to the current page
            return (*self.current_page).allocate_exclusive_page();
        }

        // Check first whether we need an ordinary extension
        if self.get_next_location() >= self.get_next_exclusive_page_location() {

            // Allocate an extension page with default size
            return self.allocate_extension_page().allocate_exclusive_page();
        }

        let page = _Page::allocate_page();
        *(self.get_next_exclusive_page_location() as *mut *const _Page) = page;
        self.exclusive_pages += 1;
        &mut *page
    }

    pub fn extend(&mut self, top: usize, size: usize) -> bool {
        unsafe {
            if top != self.get_next_location() {
                return false;
            }

            let new_top = top + size;
            if new_top > self.get_next_exclusive_page_location() {
                return false;
            }
            self.set_next_location(new_top);
        }
        true 
    }

    pub fn deallocate_extensions(&mut self) {
        let mut page: *mut _Page<'a> = self;
        unsafe {
            while !page.is_null() {
                let extension_location = (*page).get_extension_page_location();
                for i in 1..(*page).exclusive_pages {
                    let mut exclusive_page = &mut *((extension_location - (i as usize * size_of::<*const _Page> as usize)) as *mut _Page<'a>);
                    if !exclusive_page.is_oversized() {
                        exclusive_page.deallocate_extensions();
                    }
                    exclusive_page.forget();
                }

                if !eq(page, self) {
                    (*page).forget();
                }

                page = extension_location as *mut _Page<'a>; 
            }
        }
    }

    pub fn forget(&self) {
        unsafe {
            dealloc(self as *const _Page as *mut u8, Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE))
        }
    }

    pub unsafe fn get_page(address: *mut u8) -> &'a mut _Page<'a> {
        &mut *((address as usize & !_PAGE_SIZE - 1) as *mut _Page)
    }
}

#[test]
fn test_page() {

    unsafe {

        // Allocate a page
        let memory = alloc(Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE) );
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

        assert_eq!(page.next_object_offset, 0);
        assert_eq!(page.exclusive_pages, 0);
        assert_eq!((*(page.current_page)).next_object_offset, 0);
        assert_eq!((*(page.current_page)).exclusive_pages, 0);
        assert_eq!(page.is_oversized(), false);

    }
}