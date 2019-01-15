use scaly::memory::page::_Page;
use scaly::memory::context::_Context;

pub struct _Region<'a> {
    context: &'a mut _Context,
}

impl<'a> _Region<'a> {
    pub fn create(context: &mut _Context) -> _Region {
        _Region {
            context: context,
        }
    }
}

impl<'a>  Drop for  _Region<'a> {
    fn drop(&mut self) {
        self.context.deallocate_page();
    }
}

#[test]
fn test_region() {
    use std::ptr::null_mut;
    use std::alloc::alloc;
    use std::alloc::Layout;
    use scaly::memory::page::_PAGE_SIZE;
    use scaly::memory::context::_STACK_PAGES;

    unsafe {
        let memory = alloc(Layout::from_size_align_unchecked(_PAGE_SIZE * _STACK_PAGES as usize, _PAGE_SIZE));
        assert_ne!(memory, null_mut());
        let mut context = _Context::initialize(memory as *mut _Page);
        region_function(&mut context);
    }
}

fn region_function(_c: &mut _Context) {
    let mut r = _Region::create(_c);
    {
        
    }
}