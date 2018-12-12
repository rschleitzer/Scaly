pub struct Options {
    pub files: Vec<String>,
    pub output_name: String,
    pub directory: String,
}

pub enum OptionsError {
    NullLengthArgument,
    EmptyOption,
    InvalidOption(String),
    UnknownOption(String),
}

impl Options {
    pub fn parse_arguments(mut args: std::env::Args) -> Result<Options, OptionsError> {
        let mut output_name = String::new();
        let mut directory = String::new();
        let mut files: Vec<String> = vec![];

        loop {
            let arg = args.next();
            match arg {
                None => return Ok(Options {
                    output_name: output_name,
                    directory: directory,
                    files: files
                }),
                Some(arg) => {
                    let mut char_iterator = arg.chars();
                    let first_char = match char_iterator.next() {
                        Some(c) => c,
                        None => return Err(OptionsError::NullLengthArgument)
                    };

                    if first_char != '-' {
                        files.push(arg.clone());
                        continue
                    }

                    let second_char = match char_iterator.next() {
                        Some(c) => c,
                        None => return Err(OptionsError::EmptyOption)
                    };

                    match second_char {

                        'o' => {
                            output_name = match args.next() {
                                Some(o) => o,
                                None => return Err(OptionsError::InvalidOption(arg.clone())),
                            }
                        }

                        'd' => {
                            directory = match args.next() {
                                Some(o) => o,
                                None => return Err(OptionsError::InvalidOption(arg.clone())),
                            }
                        }
                        _ => return Err(OptionsError::UnknownOption(arg.clone()))
                    };

                }
            }
        }
    }
}