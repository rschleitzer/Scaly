use scaly::containers::Ref;
use scaly::io::{Console, File};
use scaly::memory::Region;
use scaly::Page;
use scaly::String;
use scaly::Vector;
use scalyc::options::Options;
use scalyc::parser::Parser;

pub struct Compiler {
    arguments: Ref<Vector<String>>,
}

impl Compiler {
    pub fn new(_p: *mut Page, arguments: Ref<Vector<String>>) -> Compiler {
        Compiler {
            arguments: arguments,
        }
    }

    pub fn compile(&self, _pr: &Region, _rp: *mut Page, _ep: *mut Page) {
        let _r = Region::create(_pr);
        let options = Options::parse_arguments(&_r, _rp, _ep, self.arguments);

        for file in options.files.iter() {
            let _r_1 = Region::create(&_r);
            let file_result = File::open_read(&_r_1, _r_1.page, *file);
            match file_result {
                Ok(file_stream) => {
                    let mut parser =
                        Ref::new(_r_1.page, Parser::new(&_r_1, _r_1.page, *file, file_stream));
                    let _file_syntax = parser.parse_file(&_r_1, _r_1.page, _r_1.page);
                }
                Err(_) => continue,
            }
        }

        if let None = options.output_name {
            loop {
                let _r_1 = Region::create(&_r);
                Console::write(&_r_1, String::from_string_slice(_r_1.page, "Scaly>"));
                let mut parser = Ref::new(
                    _r_1.page,
                    Parser::new(
                        &_r_1,
                        _r_1.page,
                        String::from_string_slice(_r_1.page, "<stdin>"),
                        Console::open_standard_output(_r_1.page),
                    ),
                );
                let _file_syntax = parser.parse_statement(&_r_1, _r_1.page, _r_1.page);
            }
        }
    }
}
