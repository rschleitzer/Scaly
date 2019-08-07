extern crate libc;
extern crate scaly;

use libc::c_char;
use libc::c_int;

use scaly::containers::Array;
use scaly::containers::Ref;
use scaly::containers::String;
use scaly::memory::Heap;
use scaly::memory::Page;
use scaly::memory::Region;
use scaly::memory::StackBucket;

use std::ffi::CString;

mod scalyc;

// Rust's main which converts args back to C's main convention (used here for the Rust backend)
fn main() {
    let args = std::env::args()
        .map(|arg| CString::new(arg).unwrap())
        .collect::<Vec<CString>>();
    let c_args = args
        .iter()
        .map(|arg| arg.as_ptr())
        .collect::<Vec<*const c_char>>();
    _main(c_args.len() as c_int, c_args.as_ptr());
}

// C style main function which converts args to Scaly's main convention (would be provided by the LLVM backend)
fn _main(argc: c_int, argv: *const *const c_char) {
    let mut heap = Heap::create();
    let root_stack_bucket = StackBucket::create(&mut heap);
    let root_page = Page::get(root_stack_bucket as usize);
    let r = Region::create_from_page(root_page);
    unsafe {
        let _r_1 = Region::create(&r);
        let mut arguments: Ref<Array<String>> = Ref::new(_r_1.page, Array::new());
        for n in 0..argc {
            if n == 0 {
                continue;
            }

            let arg = argv.offset(n as isize);
            let page = arguments.get_page();
            let s = String::from_c_string(page, *arg);
            (*arguments).add(page, s);
        }

        _scalyc_main(&_r_1, arguments);
    }
}

fn _scalyc_main(_pr: &Region, _arguments: Ref<Array<String>>) {
    use scalyc::compiler::Compiler;
    let _r = Region::create(_pr);

    let compiler = Ref::new(_r.page, Compiler::new(_r.page));
    (*compiler).compile(&_r);
}
