mod scalyc;

fn main() {
    let argvec = std::env::args().collect::<Vec<std::string::String>>();
    let arguments: Vec<&str> = argvec.iter().map(AsRef::as_ref).collect();
    _scalyc_main(&arguments)
}

use scalyc::compiler::Compiler;
use scalyc::options::{Options, OptionsError};

pub fn _scalyc_main(arguments: &Vec<&str>) {
    match Options::parse_arguments(arguments) {
        Err(error) => match error {
            OptionsError::NullLengthArgument => println!("Null length argument!"),
            OptionsError::EmptyOption => println!("Empty option!"),
            OptionsError::InvalidOption(option) => println!("Invalid option {}", option),
            OptionsError::UnknownOption(option) => println!("Unknown option {}", option),
        },
        Ok(options) => Compiler::compile(options),
    }
}
