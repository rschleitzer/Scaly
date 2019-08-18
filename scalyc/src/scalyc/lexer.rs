use scaly::containers::Ref;
use scaly::io::Stream;
use scaly::Page;
use scaly::String;

pub struct Lexer {
    pub token: Token,
    character: char,
    stream: *mut Stream,
    is_at_end: bool,
    previous_line: usize,
    previous_column: usize,
    line: usize,
    column: usize,
}

impl Lexer {
    pub fn new(_rp: *mut Page, stream: *mut Stream) -> Ref<Lexer> {
        let mut lexer = Ref::new(
            _rp,
            Lexer {
                stream: stream,
                is_at_end: false,
                token: Token::InvalidToken,
                character: 0 as char,
                previous_line: 1,
                previous_column: 0,
                line: 1,
                column: 0,
            },
        );
        lexer.read_character();
        lexer.advance();
        lexer
    }

    fn read_character(&mut self) {
        unsafe {
            let read_result: i32 = (*self.stream).read_byte();
            if read_result == -1 {
                self.is_at_end = true;
                self.character = 0 as char;
            }
        }
    }

    pub fn advance(&mut self) {
        self.skip_whitespace();
        self.previous_line = self.line;
        self.previous_column = self.previous_column;
        if self.is_at_end {
            return
        }
    }

    fn skip_whitespace(&mut self) {
        loop {
            if self.is_at_end {
                return
            }

            match self.character {
                ' ' => {
                    self.read_character();;
                    self.column = self.column + 1;
                    continue
                }

                '\t' => {
                    self.read_character();
                    self.column = self.column + 4;
                    continue
                }

                '\r' => {
                    self.read_character();
                    continue
                }

                '\n' => {
                    self.read_character();
                    self.column = 1;
                    self.line = self.line + 1;
                    continue
                }

                '/' => {
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end {
                        return
                    }

                    if self.character == '/' {
                        self.handle_single_line_comment();
                    }
                    else {
                        if self.character == '*' {
                        self.handle_multi_line_comment();
                        }
                        else {
                            return
                        }
                    }
                }

                _ =>
                    return
            }
        }
    }

    fn handle_single_line_comment(&mut self) {
        loop {
            if self.is_at_end() {
                return
            }

            match self.character {
                '\t'=> {
                    self.read_character();
                    self.column = self.column + 4;
                    continue
                }

                '\r' => {
                    self.read_character();
                    continue
                }

                '\n' => {
                    self.read_character();
                    self.column = 1;
                    self.line = self.line + 1;
                    return
                }

                _ => {
                    self.read_character();
                    self.column = self.column + 1;
                    continue;
                }
            }
        }
    }

    fn handle_multi_line_comment(&mut self) {
        loop {
            if self.is_at_end() {
                return
            }

            match self.character {
                '/' => {
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end() {
                        return
                    }
                    else
                    {
                        if self.character == '*' {
                            self.handle_multi_line_comment();
                        }
                        else {
                            return
                        }
                    }
                }

                '*' => {
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end() {
                        return
                    }
                    else {
                        if self.character == '/' {
                            self.read_character();
                            self.column = self.column + 1;
                            return
                        }
                    }
                }

                '\t' => {
                    self.read_character();
                    self.column = self.column + 4;
                    continue
                }

                '\r' => {
                    self.read_character();
                    continue
                }

                '\n' => {
                    self.read_character();
                    self.column = 1;
                    self.line = self.line + 1;
                    return
                }

                _ => {
                    self.read_character();
                    self.column = self.column + 1;
                    continue
                }
            }
        }
    }

    pub fn get_position(&self) -> Position {
        Position {
            line: self.line,
            column: self.column,
        }
    }

    pub fn get_previous_position(&self) -> Position {
        Position {
            line: self.previous_line,
            column: self.previous_column,
        }
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
    _Identifier(String),
}
