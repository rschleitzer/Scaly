mod scalyc;

use scalyc::options::*;
use scalyc::compiler::*;

fn main() -> Result<(), std::io::Error> {
    let args = std::env::args();
    match Options::parse_arguments(args) {
        Ok(options) => compile(options),
        Err(error) => match error {
            OptionsError::NullLengthArgument => println!("Null length argument!"),
            OptionsError::EmptyOption => println!("Empty option!"),
            OptionsError::InvalidOption(option) => println!("Invalid option {}", option),
            OptionsError::UnknownOption(option) => println!("Unknown option {}", option),
        }
    }
    Ok(())
}
