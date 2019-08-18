use scaly::io::Stream;
use scaly::Page;
use scaly::String;

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

    pub fn is_at_end(&self) -> bool {
        match self.token {
            Token::_EofToken => true,
            _ => false,    
        }
    }
}

#[derive(Copy, Clone)]
pub struct Position {
    pub line: usize,
    pub column: usize,
}

#[derive(Copy, Clone)]
pub enum Token {
    _EofToken,
    InvalidToken,
    _Identifier (String),
}