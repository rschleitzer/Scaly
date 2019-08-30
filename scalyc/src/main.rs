extern crate libc;
extern crate scaly;

use libc::c_char;
use libc::c_int;

use scaly::containers::{Array, Ref, String, Vector};
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
    let _r = Region::create_from_page(Page::get(StackBucket::create(&mut Heap::create()) as usize));
    _scalyc_main(&_r, {
        let _r_1 = Region::create(&_r);
        let mut arguments: Ref<Array<String>> = Ref::new(_r_1.page, Array::new());
        for n in 0..argc {
            if n == 0 {
                continue;
            }
            unsafe {
                let arg = argv.offset(n as isize);
                let s = String::from_c_string(_r.page, *arg);
                (*arguments).add(s);
            }
        }

        Ref::new(_r.page, Vector::from_array(_r.page, arguments))
    });
}

fn _scalyc_main(_pr: &Region, arguments: Ref<Vector<String>>) {
    use scalyc::compiler::Compiler;
    let _r = Region::create(_pr);

    let compiler = Ref::new(_r.page, Compiler::new(_r.page, arguments));
    (*compiler).compile(&_r, _r.page, _r.page);
}
