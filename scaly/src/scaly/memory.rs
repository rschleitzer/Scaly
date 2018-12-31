const _PAGE_SIZE: usize = 0x1000;

pub struct _Page<'a> {
    current_page: *mut _Page<'a>,
    next_object_offset: i32,
    exclusive_pages: i32
}

impl<'a> _Page<'a> {

    pub unsafe fn reset(&mut self) {
        *(self.get_extension_page_location() as *mut *const _Page) = std::ptr::null();
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
        location_behind_page - std::mem::size_of::<*const _Page>()
    }

    unsafe fn get_next_location(&self) -> usize {
        self as *const _Page as usize + self.next_object_offset as usize
    }

    unsafe fn set_next_location(&mut self, location: usize) {
        let self_location = self as *const _Page as usize;
        self.next_object_offset = (location - self_location as usize) as i32;
    }

    unsafe fn get_next_exclusive_page_location(&self) -> usize  {
        self.get_extension_page_location() - (self.exclusive_pages as usize + 1) * std::mem::size_of::<*const _Page>()
    }

    pub unsafe fn allocate_raw(&mut self, size: usize, align: usize) -> *mut u8 {
        if !std::ptr::eq(self, self.current_page) {

            // We're already known to be full, so we delegate to the current page
            let new_object = (*self.current_page).allocate_raw(size, align);

            // Possibly our current page was also full so we propagate back the new current page
            let allocating_page = _Page::get_page(new_object);
            if !std::ptr::eq(allocating_page, self.current_page) && (!allocating_page.is_oversized()) {
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
        let gross_size = std::mem::size_of::<_Page>() + size + std::mem::size_of::<*const _Page>();
        if gross_size > _PAGE_SIZE {
            if self.get_next_location() >= self.get_extension_page_location() {

                // Allocate an extension page with default size
                let extension_page = self.allocate_extension_page();

                // Try again with the new extension page
                return extension_page.allocate_raw(size, align);
            }

            // We allocate oversized objects directly.
            let memory = std::alloc::alloc(std::alloc::Layout::from_size_align_unchecked(size + std::mem::size_of::<_Page>(), _PAGE_SIZE) );

            // Initialize a _Page object at the page start
            let page = memory as *mut _Page<'a>;
            (*page).reset();

            // Oversized pages have no current_page
            (*page).current_page = std::ptr::null_mut();
            
            *(self.get_next_exclusive_page_location() as *mut *const _Page) = page;
            self.exclusive_pages = self.exclusive_pages + 1;
            return page.offset(1) as *mut u8;
        }

        // So we're not oversized. Create extension page and let it allocate.
        self.allocate_extension_page().allocate_raw(size, align)
    }

    unsafe fn allocate_extension_page(&mut self) -> &'a mut _Page<'a> {

        // Allocate a page
        let memory = std::alloc::alloc(std::alloc::Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE) );

        // Initialize a _Page object at the page start
        let page = memory as *mut _Page<'a>;
        (*page).reset();
        *(self.get_extension_page_location() as *mut *const _Page) = page;
        self.current_page = page;
        &mut *page
    }

    pub unsafe fn get_page(address: *mut u8) -> &'a mut _Page<'a> {
        &mut *((address as usize & !_PAGE_SIZE - 1) as *mut _Page)
    }

    unsafe fn deallocate_extensions(&mut self) {
        unimplemented!()
    }
}

#[test]
fn test_page() {

    unsafe {

        // Allocate a page
        let memory = std::alloc::alloc(std::alloc::Layout::from_size_align_unchecked(_PAGE_SIZE, _PAGE_SIZE) );
        assert_ne!(memory, std::ptr::null_mut());

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