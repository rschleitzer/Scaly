extern crate libc;

use self::libc::printf;
use containers::String;
use memory::region::Region;

pub struct Console {}
impl Console {
    pub fn write(_pr: &Region, s: String) {
        let _r = Region::create(_pr);
        let scaly_string = s.append_character(_r.page, '\0');
        unsafe {
            printf(scaly_string.to_c_string());
        }
    }
}

#[test]
fn test_console() {
    use memory::Heap;
    use memory::Page;
    use memory::StackBucket;
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let r1 = Region::create_from_page(Page::get(root_stack_bucket as usize));
    {
        let root_page = Page::get(root_stack_bucket as usize);
        Console::write(&r1, String::from_string_slice(root_page, "Scaly>"));
    }
}
