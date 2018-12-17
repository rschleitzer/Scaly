mod scalyc;

fn main() {
    _scalyc_main(std::env::args())
}

use scalyc::options::*;
use scalyc::compiler::*;

pub fn _scalyc_main(arguments: std::env::Args) {
    match Options::parse_arguments(arguments) {
        Err(error) => match error {
            OptionsError::NullLengthArgument => println!("Null length argument!"),
            OptionsError::EmptyOption => println!("Empty option!"),
            OptionsError::InvalidOption(option) => println!("Invalid option {}", option),
            OptionsError::UnknownOption(option) => println!("Unknown option {}", option),
        }
        Ok(options) => Compiler::compile(options)
    }
}
