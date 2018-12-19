pub struct Options<'a> {
    pub files: Vec<&'a str>,
    pub output_name: &'a str,
    pub directory: &'a str,
}

impl<'a> Options<'a> {
    pub fn parse_arguments(arguments: &Vec<&'a str>) -> Result<Options<'a>, OptionsError<'a>> {
        let mut output_name = "";
        let mut directory = "";
        let mut files: Vec<&'a str> = vec![];
        let mut first_argument: bool = true;

        let mut args = arguments.iter();
        
        loop {
            let arg = args.next();
            if first_argument {
                first_argument = false;
                continue;
            }

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
                        files.push(arg);
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
                                None => return Err(OptionsError::InvalidOption(arg)),
                            }
                        }

                        'd' => {
                            directory = match args.next() {
                                Some(o) => o,
                                None => return Err(OptionsError::InvalidOption(arg)),
                            }
                        }
                        _ => return Err(OptionsError::UnknownOption(arg))
                    };

                }
            }
        }
    }
}

pub enum OptionsError<'a> {
    NullLengthArgument,
    EmptyOption,
    InvalidOption(&'a str),
    UnknownOption(&'a str),
}
