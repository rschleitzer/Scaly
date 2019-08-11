use scaly::io::Stream;
use scaly::Page;

pub struct Lexer {
    _stream: *mut Stream,
}

impl Lexer {
    pub fn new(_p: *mut Page, stream: *mut Stream) -> Lexer {
        Lexer { _stream: stream }
    }
}
