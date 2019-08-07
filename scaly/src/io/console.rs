extern crate libc;

use self::libc::c_char;
use self::libc::fdopen;
use self::libc::fprintf;
use self::libc::STDERR_FILENO;
use containers::String;
use memory::region::Region;

pub struct Console {}
impl Console {
    pub fn write(_pr: &Region, s: String) {
        let _r = Region::create(_pr);
        let scaly_string = s.append_character(_r.page, '\0');
        unsafe {
            let f = fdopen(STDERR_FILENO, "w\0".as_ptr() as *const c_char);
            fprintf(f, scaly_string.to_c_string());
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
