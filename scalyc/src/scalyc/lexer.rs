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
        lexer.advance();
        lexer
    }

    pub fn advance(&mut self) {
        //skip_whitespace();
        self.previous_line = self.line;
        self.previous_column = self.previous_column;
    }

    fn read_character(&mut self) {
        unsafe {
            let character = (*self.stream).read_byte();
        }
    }

    // fn skip_whitespace(&self) {
    //     loop {
    //         let c = (*(self.stream)).read_byte() as char;
    //         switch (text[position])
    //         {
    //             case ' ':
    //             {
    //                 position = position + 1;
    //                 column = column + 1;
    //                 continue;
    //             }

    //             case '\t':
    //             {
    //                 position = position + 1;
    //                 column = column + 4;
    //                 continue;
    //             }

    //             case '\r':
    //             {
    //                 position = position + 1;
    //                 continue;
    //             }

    //             case '\n': {
    //                 position = position + 1;
    //                 column = 1;
    //                 line = line + 1;
    //                 continue;
    //             }

    //             case '/':
    //             {
    //                 position = position + 1;
    //                 column = column + 1;

    //                 if (position == end)
    //                     return;

    //                 if (text[position] == '/')
    //                 {
    //                     handleSingleLineComment();
    //                 }
    //                 else
    //                 {
    //                     if (text[position] == '*')
    //                         handleMultiLineComment();
    //                     else
    //                         return;
    //                 }
    //                     break;
    //             }
    //             default:
    //                 return;
    //         }
    //     }
    //     while (true);
    // }

    // void handleSingleLineComment()
    // {
    //     do
    //     {
    //         if (position == end)
    //             return;

    //         switch (text[position])
    //         {
    //             case '\t':
    //             {
    //                 position = position + 1;
    //                 column = column + 4;
    //                 continue;
    //             }

    //             case '\r':
    //             {
    //                 position = position + 1;
    //                 continue;
    //             }

    //             case '\n':
    //             {
    //                 position = position + 1;
    //                 column = 1;
    //                 line = line + 1;
    //                 return;
    //             }

    //             default:
    //             {
    //                 position = position + 1;
    //                 column = column + 1;
    //                 continue;
    //             }
    //         }
    //     }
    //     while (true);
    // }

    // void handleMultiLineComment()
    // {
    //     do
    //     {
    //         if (position == end)
    //             return;

    //         switch (text[position])
    //         {
    //             case '/':
    //             {
    //                 position = position + 1;
    //                 column = column + 1;

    //                 if (position == end)
    //                 {
    //                     return;
    //                 }
    //                 else
    //                 {
    //                     if (text[position] == '*')
    //                         handleMultiLineComment();
    //                     else
    //                         return;
    //                 }
    //                 break;
    //             }

    //             case '*':
    //             {
    //                 position = position + 1;
    //                 column = column + 1;

    //                     if (position == end)
    //                     {
    //                         return;
    //                     }
    //                     else
    //                     {
    //                         if (text[position] == '/')
    //                         {
    //                             position = position + 1;
    //                             column = column + 1;
    //                             return;
    //                         }
    //                     }
    //                 break;
    //             }

    //             case '\t':
    //             {
    //                 position = position + 1;
    //                 column = column + 4;
    //                 continue;
    //             }

    //             case '\r':
    //             {
    //                 position = position + 1;
    //                 continue;
    //             }

    //             case '\n':
    //             {
    //                 position = position + 1;
    //                 column = 1;
    //                 line = line + 1;
    //                 return;
    //             }
    //             default:
    //             {
    //                 position = position + 1;
    //                 column = column + 1;
    //                 continue;
    //             }
    //         }
    //     }
    //     while (true);
    // }

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
