use scaly::containers::Ref;
use scaly::memory::Region;
use scaly::Page;
use scalyc::lexer::Lexer;

pub struct Parser {
    _lexer: Ref<Lexer>,
}

impl Parser {
    pub fn new(_p: *mut Page) -> Parser {
        Parser {
            _lexer: Ref::new(_p, Lexer::new(_p)),
        }
    }

    pub fn parse_statement(&self, _pr: &Region, _rp: *mut Page) {}
}
