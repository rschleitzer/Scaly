use scaly::containers::{Array, Ref, String};
use scaly::io::{Stream};
use scaly::memory::Region;
use scaly::Page;
use scalyc::lexer::Lexer;

pub struct Parser {
    _lexer: Ref<Lexer>,
    _keywords: Ref<Array<String>>,
}

impl Parser {
    pub fn new(_pr: &Region, _rp: *mut Page, stream: *mut Stream) -> Parser {
        let _r = Region::create(_pr);
        let keywords_array = Ref::new(_r.page, Array::new());
        //(*keywords_array).add(_r.page, )
        Parser {
            _lexer: Ref::new(_rp, Lexer::new(_rp, stream)),
            _keywords: keywords_array,
        }
    }

    pub fn parse_statement(&self, _pr: &Region, _rp: *mut Page) {}
}
