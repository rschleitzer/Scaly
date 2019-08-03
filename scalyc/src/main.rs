extern crate libc;
extern crate scaly;

use libc::c_char;
use libc::c_int;

use scaly::scaly::memory::StackBucket;

use std::ffi::CString;

mod scalyc;

// Rust's main which converts args back to C's main convention (used here for the Rust backend)
fn main() {
    let args = std::env::args().map(|arg| CString::new(arg).unwrap()).collect::<Vec<CString>>();
    let c_args = args
        .iter()
        .map(|arg| arg.as_ptr())
        .collect::<Vec<*const c_char>>();
    _main(c_args.len() as c_int, c_args.as_ptr());
}

// C style main function which converts args to Scaly's main convention (would be provided by the LLVM backend)
fn _main(argc: c_int, argv: *const *const c_char)
{
    for n in 0..argc {
        unsafe {
            if n == 0 {
                continue;
            }

            let arg = argv.offset(n as isize);
            println!("{}", arg as usize);
        }
    }
}

// use scalyc::compiler::Compiler;
// use scalyc::options::{Options, OptionsError};

// pub fn _scalyc_main(arguments: &Vec<&str>) {
//     match Options::parse_arguments(arguments) {
//         Err(error) => match error {
//             OptionsError::NullLengthArgument => println!("Null length argument!"),
//             OptionsError::EmptyOption => println!("Empty option!"),
//             OptionsError::InvalidOption(option) => println!("Invalid option {}", option),
//             OptionsError::UnknownOption(option) => println!("Unknown option {}", option),
//         },
//         Ok(options) => Compiler::compile(options),
//     }
// }
