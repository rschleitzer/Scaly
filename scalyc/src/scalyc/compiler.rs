use scaly::containers::Ref;
use scaly::io::Console;
use scaly::memory::Region;
use scaly::Page;
use scaly::String;
use scalyc::parser::Parser;

pub struct Compiler {}

impl Compiler {
    pub fn new(_p: *mut Page) -> Compiler {
        Compiler {}
    }
    pub fn compile(&self, _pr: &Region) {
        let _r = Region::create(_pr);
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
            let _statement = parser.parse_statement(&_r_1, _r_1.page);
            break;
        }
    }
}
