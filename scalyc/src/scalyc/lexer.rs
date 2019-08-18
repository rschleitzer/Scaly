use scaly::io::Stream;
use scaly::Page;

pub struct Lexer {
    pub token: Token,
    _stream: *mut Stream,
    previous_line: usize,
    previous_column: usize,
    line: usize,
    column: usize,
}

impl Lexer {
    pub fn new(_p: *mut Page, stream: *mut Stream) -> Lexer {
        Lexer { _stream: stream, token: Token::InvalidToken, previous_line: 1, previous_column: 0, line: 1, column: 0, }
    }

    pub fn get_position(&self) -> Position {
        Position { line: self.line, column: self.column }
    }

    pub fn get_previous_position(&self) -> Position {
        Position { line: self.previous_line, column: self.previous_column }
    }
}

#[derive(Copy, Clone)]
pub struct Position {
    pub line: usize,
    pub column: usize,
}

pub enum Token {
    _EofToken,
    InvalidToken,
    _Identifier (String),
}