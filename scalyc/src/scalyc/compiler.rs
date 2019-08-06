use scaly::containers::Ref;
use scaly::memory::Region;
use scaly::Page;
use scalyc::parser::Parser;

pub struct Compiler {
    parser: Ref<Parser>,
}

impl Compiler {
    pub fn new(_p: *mut Page) -> Compiler {
        Compiler {
            parser: Ref::new(_p, Parser::new(_p)),
        }
    }
    pub fn compile(&self, _pr: &Region) {
        let _r = Region::create(_pr);
        (*self.parser).parse_statement(&_r, _r.page);
    }
}
