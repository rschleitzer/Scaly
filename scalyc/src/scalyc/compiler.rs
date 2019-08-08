use scaly::containers::Ref;
use scaly::io::{Console, ConsoleStream};
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Page;
use scaly::String;
use scalyc::parser::Parser;

pub struct Compiler {
    parser: Ref<Parser>,
}

impl Compiler {
    pub fn new(_p: *mut Page) -> Compiler {
        let parser = Ref::new(_p, Parser::new(_p));
        Compiler { parser: parser }
    }
    pub fn compile(&self, _pr: &Region) {
        let _r = Region::create(_pr);
        loop {
            let _r_1 = Region::create(&_r);
            Console::write(&_r_1, String::from_string_slice(_r_1.page, "Scaly>"));
            let parser = Ref::new(_p, Parser::new(_p, Ref::new(_r_1.page, ConsoleStream::));
            let _statement = (*self.parser).parse_statement(&_r_1, _r_1.page);
            break;
        }
    }
}
