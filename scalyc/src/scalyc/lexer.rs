use scaly::containers::Ref;
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::Equal;
use scaly::{Page, String, StringBuilder};

pub struct Lexer {
    pub token: Ref<Token>,
    character: char,
    stream: *mut Stream,
    is_at_end: bool,
    previous_line: usize,
    previous_column: usize,
    pub line: usize,
    pub column: usize,
}

impl Lexer {
    pub fn new(_pr: &Region, _rp: *mut Page, stream: *mut Stream) -> Ref<Lexer> {
        let _r = Region::create(_pr);
        let _token_page = unsafe { (*_rp).allocate_exclusive_page() };
        unsafe { (*_token_page).reset() };
        let lexer = Ref::new(
            _rp,
            Lexer {
                stream: stream,
                is_at_end: false,
                token: Ref::new(_token_page, Token::Empty),
                character: 0 as char,
                previous_line: 1,
                previous_column: 0,
                line: 1,
                column: 0,
            },
        );
        lexer
    }

    fn read_character(&mut self) {
        if self.character == 0 as char {
            unsafe {
                let read_result: i32 = (*self.stream).read_byte();
                if read_result == -1 {
                    self.is_at_end = true;
                    self.character = 0 as char;
                } else {
                    self.character = read_result as u8 as char;
                }
            }
        }
    }

    pub fn advance(&mut self, _pr: &Region) {
        let _r = Region::create(_pr);
        self.skip_whitespace();
        self.previous_line = self.line;
        self.previous_column = self.column;
        if self.is_at_end {
            return;
        }

        let c = self.character;

        if ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) {
            {
                let _token_page = Page::own(self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_identifier(&_r, _token_page);
            }
            return;
        }

        if (c >= '0') && (c <= '9') {
            {
                let _token_page = Page::own(self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_numeric_literal(&_r, _token_page);
            }
            return;
        }

        match c {
            '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                let _token_page = Page::own(self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_operator(&_r, _token_page);
            }

            '\"' => {
                let _token_page = Page::own(self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_string_literal(&_r, _token_page);
            }

            '\'' => {
                let _token_page = Page::own(self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_character_literal(&_r, _token_page);
            }

            '{' | '}' | '(' | ')' | '[' | ']' | '.' | ',' | ':' | ';' | '?' | '!' | '@' | '#'
            | '$' | '_' | '`' => {
                {
                    let _token_page = Page::own(self.token);
                    unsafe {
                        (*_token_page).reset();
                    }
                    self.token = Ref::new(
                        _token_page,
                        Token::Punctuation(String::from_character(_token_page, c)),
                    );
                }
                self.character = 0 as char;
                self.column = self.column + 1;
            }

            _ => {
                let _token_page = Page::own(self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = Ref::new(_token_page, Token::Invalid);
            }
        }
    }

    pub fn empty(&mut self) {
        let _token_page = Page::own(self.token);
        unsafe {
            (*_token_page).reset();
        }
        self.token = Ref::new(_token_page, Token::Empty);
    }

    fn scan_identifier(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut name: Ref<StringBuilder> = StringBuilder::new(_r.page);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                // if name.get_length() == 0 {
                //     return Ref::new(_rp, Token::Invalid);
                // }
                // else {
                    return Ref::new(_rp, Token::Identifier(name.to_string(_rp)));
                // }
            }

            let c = self.character;
            if ((c >= 'a') && (c <= 'z'))
                || ((c >= 'A') && (c <= 'Z'))
                || ((c >= '0') && (c <= '9'))
                || (c == '_')
            {
                name.append_character(c);
                self.character = 0 as char
            } else {
                return Ref::new(_rp, Token::Identifier(name.to_string(_rp)));
            }
        }
    }

    fn scan_operator(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut operation: Ref<StringBuilder> = StringBuilder::new(_r.page);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                // if operation.get_length() == 0 {
                //     return Ref::new(_rp, Token::Invalid);
                // }
                // else {
                    return Ref::new(_rp, Token::Identifier(operation.to_string(_rp)));
                // }
            }

            match self.character {
                '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                    operation.append_character(self.character);
                    self.character = 0 as char
                }

                _ => return Ref::new(_rp, Token::Identifier(operation.to_string(_rp))),
            }
        }
    }

    fn scan_string_literal(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut value: Ref<StringBuilder> = StringBuilder::new(_r.page);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::Invalid);
            }

            match self.character {
                '\"' => {
                    self.read_character();
                    self.column = self.column + 1;
                    return Ref::new(_rp, Token::Literal(Literal::String(value.to_string(_rp))));
                }

                '\\' => {
                    self.read_character();
                    self.column = self.column + 1;
                    match self.character {
                        '\"' | '\\' | '\'' => {
                            value.append_character('\\');
                            value.append_character(self.character);
                        }
                        'n' => {
                            value.append_character('\\');
                            value.append_character('n');
                        }
                        'r' => {
                            value.append_character('\\');
                            value.append_character('r');
                        }
                        't' => {
                            value.append_character('\\');
                            value.append_character('t');
                        }
                        '0' => {
                            value.append_character('\\');
                            value.append_character('0');
                        }
                        _ => return Ref::new(_rp, Token::Invalid),
                    }
                }
                _ => value.append_character(self.character),
            }
        }
    }

    fn scan_character_literal(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut value: Ref<StringBuilder> = StringBuilder::new(_r.page);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::Invalid);
            }

            match self.character {
                '\'' => {
                    self.read_character();
                    self.column = self.column + 1;
                    return Ref::new(
                        _rp,
                        Token::Literal(Literal::Character(value.to_string(_rp))),
                    );
                }

                '\\' => {
                    self.read_character();
                    self.column = self.column + 1;
                    match self.character {
                        '\"' | '\\' | '\'' => {
                            value.append_character('\\');
                            value.append_character(self.character);
                        }
                        'n' => {
                            value.append_character('\\');
                            value.append_character('n');
                        }
                        'r' => {
                            value.append_character('\\');
                            value.append_character('r');
                        }
                        't' => {
                            value.append_character('\\');
                            value.append_character('t');
                        }
                        '0' => {
                            value.append_character('\\');
                            value.append_character('0');
                        }
                        _ => return Ref::new(_rp, Token::Invalid),
                    }
                }
                _ => {
                    value.append_character(self.character);
                    self.character = 0 as char
                }
            }
        }
    }

    fn scan_numeric_literal(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut value: Ref<StringBuilder> = StringBuilder::from_character(_rp, self.character);

        self.read_character();
        self.column = self.column + 1;

        if self.is_at_end() {
            return Ref::new(_rp, Token::Literal(Literal::Numeric(value.to_string(_rp))));
        }
        if self.character == 'x' {
            return self.scan_hex_literal(&_r, _rp);
        }

        loop {
            let c = self.character;
            if (c >= '0') && (c <= '9') {
                value.append_character(self.character);
                self.character = 0 as char
            } else {
                return Ref::new(_rp, Token::Literal(Literal::Numeric(value.to_string(_rp))));
            }
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::Literal(Literal::Numeric(value.to_string(_rp))));
            }
        }
    }

    fn scan_hex_literal(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut value: Ref<StringBuilder> = StringBuilder::from_character(_rp, self.character);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::Literal(Literal::Hex(value.to_string(_rp))));
            }

            let c = self.character;
            if ((c >= '0') && (c <= '9'))
                || ((c >= 'a') && (c <= 'f'))
                || ((c >= 'A') && (c <= 'F'))
            {
                value.append_character(self.character);
                self.character = 0 as char
            } else {
                return Ref::new(_rp, Token::Literal(Literal::Hex(value.to_string(_rp))));
            }
        }
    }

    fn skip_whitespace(&mut self) {
        loop {
            self.read_character();
            if self.is_at_end {
                return;
            }

            match self.character {
                ' ' => {
                    self.character = 0 as char;
                    self.column = self.column + 1;
                    continue;
                }

                '\t' => {
                    self.character = 0 as char;
                    self.column = self.column + 4;
                    continue;
                }

                '\r' => {
                    self.character = 0 as char;
                    continue;
                }

                '\n' => {
                    self.character = 0 as char;
                    self.column = 1;
                    self.line = self.line + 1;
                    continue;
                }

                '/' => {
                    self.character = 0 as char;
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end {
                        return;
                    }

                    if self.character == '/' {
                        self.handle_single_line_comment();
                    } else {
                        if self.character == '*' {
                            self.handle_multi_line_comment();
                        } else {
                            return;
                        }
                    }
                }

                _ => return,
            }
        }
    }

    fn handle_single_line_comment(&mut self) {
        loop {
            self.read_character();
            if self.is_at_end() {
                return;
            }

            match self.character {
                '\t' => {
                    self.character = 0 as char;
                    self.column = self.column + 4;
                    continue;
                }

                '\r' => {
                    self.character = 0 as char;
                    continue;
                }

                '\n' => {
                    self.character = 0 as char;
                    self.column = 1;
                    self.line = self.line + 1;
                    return;
                }

                _ => {
                    self.character = 0 as char;
                    self.column = self.column + 1;
                    continue;
                }
            }
        }
    }

    fn handle_multi_line_comment(&mut self) {
        loop {
            self.read_character();
            if self.is_at_end() {
                return;
            }

            match self.character {
                '/' => {
                    self.character = 0 as char;
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end() {
                        return;
                    } else {
                        if self.character == '*' {
                            self.handle_multi_line_comment();
                        } else {
                            return;
                        }
                    }
                }

                '*' => {
                    self.character = 0 as char;
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end() {
                        return;
                    } else {
                        if self.character == '/' {
                            self.character = 0 as char;
                            self.read_character();
                            self.column = self.column + 1;
                            return;
                        }
                    }
                }

                '\t' => {
                    self.character = 0 as char;
                    self.column = self.column + 4;
                    continue;
                }

                '\r' => {
                    self.character = 0 as char;
                    continue;
                }

                '\n' => {
                    self.character = 0 as char;
                    self.column = 1;
                    self.line = self.line + 1;
                    return;
                }

                _ => {
                    self.character = 0 as char;
                    self.column = self.column + 1;
                    continue;
                }
            }
        }
    }

    pub fn parse_keyword(&mut self, _pr: &Region, fixed_string: String) -> bool {
        let _r = Region::create(_pr);
        match *self.token {
            Token::Empty => self.advance(&_r),
            _ => (),
        }
        match *self.token {
            Token::Identifier(name) => {
                let right_keyword = name.equals(&fixed_string);
                self.empty();
                return right_keyword;
            }
            _ => return false,
        }
    }

    pub fn parse_identifier(&mut self, _pr: &Region, _rp: *mut Page) -> Option<String> {
        let _r = Region::create(_pr);
        match *self.token {
            Token::Empty => self.advance(&_r),
            _ => (),
        }
        match *self.token {
            Token::Identifier(name) => {
                let ret = String::copy(_rp, name);
                self.empty();
                return Some(ret);
            }
            _ => return None,
        }
    }

    pub fn parse_punctuation(&mut self, _pr: &Region, fixed_string: String) -> bool {
        let _r = Region::create(_pr);
        match *self.token {
            Token::Empty => self.advance(&_r),
            _ => (),
        }
        match *self.token {
            Token::Punctuation(name) => {
                let ret = name.equals(&fixed_string);
                if ret {
                    self.empty();
                }
                return ret;
            }
            _ => return false,
        }
    }

    pub fn parse_literal(&mut self, _pr: &Region, _rp: *mut Page) -> Option<Literal> {
        let _r = Region::create(_pr);
        match *self.token {
            Token::Empty => self.advance(&_r),
            _ => (),
        }
        match *self.token {
            Token::Literal(Literal::String(name)) => {
                let ret = String::copy(_rp, name);
                self.empty();
                Some(Literal::String(ret))
            }
            Token::Literal(Literal::Character(name)) => {
                let ret = String::copy(_rp, name);
                self.empty();
                Some(Literal::Character(ret))
            }
            Token::Literal(Literal::Numeric(name)) => {
                let ret = String::copy(_rp, name);
                self.empty();
                Some(Literal::Numeric(ret))
            }
            Token::Literal(Literal::Hex(name)) => {
                let ret = String::copy(_rp, name);
                self.empty();
                Some(Literal::Hex(ret))
            }
            _ => None,
        }
    }

    pub fn is_at_end(&self) -> bool {
        self.is_at_end
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
}

#[derive(Copy, Clone)]
pub struct Position {
    pub line: usize,
    pub column: usize,
}

#[derive(Copy, Clone)]
pub enum Token {
    Empty,
    Invalid,
    Identifier(String),
    Literal(Literal),
    Punctuation(String),
}

#[derive(Copy, Clone)]
pub enum Literal {
    String(String),
    Character(String),
    Numeric(String),
    Hex(String),
}
