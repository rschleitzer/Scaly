const _PAGE_SIZE: usize = 0x1000;

pub struct _Page<'a> {
    current_page: Option<&'a _Page<'a>>,
    next_object_offset: i32,
    exclusive_pages: i32
}

impl<'a> _Page<'a> {

    pub unsafe fn reset(&mut self) {
        *self.get_extension_page_location() = Option::None;
        self.next_object_offset = 0;
        self.exclusive_pages = 0;
        
        // currentPage = this;
        self.current_page = std::mem::transmute::<&mut _Page, Option<&'a _Page<'a>>>(self);
    }

    pub unsafe fn clear(&mut self) {
        self.deallocate_extensions();
        self.reset();
    }

    pub fn is_oversized(&self) -> bool {
        match self.current_page { None => true, _ => false }
    }

    unsafe fn get_extension_page_location(&self) -> *mut Option<&'a _Page<'a>> {

        // Convert self to u8 pointer
        let u8_ptr = std::mem::transmute::<*const _Page, *const u8>(&*self);
        
        // Advance one page size so we are past the end of our page
        let ptr_advanced_by_page_size = u8_ptr.offset(_PAGE_SIZE as isize);

        // Convert to *_Page?
        let ptr_next_page = std::mem::transmute::<*const u8, *mut Option<&'a _Page<'a>>>(ptr_advanced_by_page_size);

        // Go back one pointer size
        let ptr_at_end_of_page = ptr_next_page.offset(-1);

        // Convert to mutable pointer to the optional extension page
        &mut *ptr_at_end_of_page
    }

    unsafe fn _get_next_object(&self) -> *mut u8 {
        let u8_ptr = std::mem::transmute::<*const _Page, *const u8>(&*self);
        let offset_pointer = u8_ptr.offset(self.next_object_offset as isize);
        offset_pointer as *mut u8
    }

    unsafe fn _set_next_object(&mut self, object: *const u8) {
        let self_ptr = std::mem::transmute::<*const _Page, *const u8>(&*self);
        self.next_object_offset = (object as usize - self_ptr as usize) as i32;
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
        let page = &mut*std::mem::transmute::<*mut u8, *mut _Page>(memory);

        assert_ne!(page.next_object_offset, 0);
        assert_ne!(page.exclusive_pages, 0);

        page.reset();

        assert_eq!(page.next_object_offset, 0);
        assert_eq!(page.exclusive_pages, 0);
        assert_eq!(page.current_page.unwrap().next_object_offset, 0);
        assert_eq!(page.current_page.unwrap().exclusive_pages, 0);
        assert_eq!(page.is_oversized(), false);

    }
}