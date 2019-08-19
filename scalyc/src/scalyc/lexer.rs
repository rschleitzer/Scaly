use scaly::containers::Ref;
use scaly::io::Stream;
use scaly::memory::Region;
use scaly::{Page, String, StringBuilder};

pub struct Lexer {
    pub token: Ref<Token>,
    character: char,
    stream: *mut Stream,
    is_at_end: bool,
    previous_line: usize,
    previous_column: usize,
    line: usize,
    column: usize,
}

impl Lexer {
    pub fn new(_pr: &Region, _rp: *mut Page, stream: *mut Stream) -> Ref<Lexer> {
        let _r = Region::create(_pr);
        let _token_page = unsafe { (*_rp).allocate_exclusive_page() };
        unsafe { (*_token_page).reset() };
        let mut lexer = Ref::new(
            _rp,
            Lexer {
                stream: stream,
                is_at_end: false,
                token: Ref::new(_token_page, Token::InvalidToken),
                character: 0 as char,
                previous_line: 1,
                previous_column: 0,
                line: 1,
                column: 0,
            },
        );
        lexer.read_character();
        lexer.advance(&_r);
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

    pub fn advance(&mut self, _pr: &Region) {
        let _r = Region::create(_pr);
        self.skip_whitespace();
        self.previous_line = self.line;
        self.previous_column = self.previous_column;
        if self.is_at_end {
            return;
        }

        let c = self.character;

        if ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) {
            {
                let _token_page = Page::own(&self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_identifier(&_r, _token_page);
            }
            return;
        }

        if (c >= '0') && (c <= '9') {
            {
                let _token_page = Page::own(&self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_numeric_literal(&_r, _token_page);
            }
            return;
        }

        match c {
            '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                let _token_page = Page::own(&self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_operator(&_r, _token_page);
            }

            '\"' => {
                let _token_page = Page::own(&self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_string_literal(&_r, _token_page);
            }

            '\'' => {
                let _token_page = Page::own(&self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = self.scan_character_literal(&_r, _token_page);
            }

            '{' | '}' | '(' | ')' | '[' | ']' | '.' | ',' | ':' | ';' | '?' | '!' | '@' | '#'
            | '$' | '_' | '`' => {
                {
                    let _token_page = Page::own(&self.token);
                    unsafe {
                        (*_token_page).reset();
                    }
                    self.token = Ref::new(
                        _token_page,
                        Token::Punctuation(String::from_character(Page::own(self), c)),
                    );
                }
                self.read_character();
                self.column = self.column + 1;
            }

            _ => {
                let _token_page = Page::own(&self.token);
                unsafe {
                    (*_token_page).reset();
                }
                self.token = Ref::new(_token_page, Token::InvalidToken);
            }
        }
    }

    fn scan_identifier(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut name: Ref<StringBuilder> = StringBuilder::from_character(_r.page, self.character);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::Identifier(name.to_string(_rp)));
            }

            let c = self.character;
            if ((c >= 'a') && (c <= 'z'))
                || ((c >= 'A') && (c <= 'Z'))
                || ((c >= '0') && (c <= '9'))
                || (c == '_')
            {
                name.append_character(c);
            } else {
                return Ref::new(_rp, Token::Identifier(name.to_string(_rp)));
            }
        }
    }

    fn scan_operator(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut operation: Ref<StringBuilder> =
            StringBuilder::from_character(_r.page, self.character);

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::Identifier(operation.to_string(_rp)));
            }

            match self.character {
                '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                    operation.append_character(self.character)
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
                return Ref::new(_rp, Token::InvalidToken);
            }

            match self.character {
                '\"' => {
                    self.read_character();
                    self.column = self.column + 1;
                    return Ref::new(_rp, Token::StringLiteral(value.to_string(_rp)));
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
                        _ => return Ref::new(_rp, Token::InvalidToken),
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
                return Ref::new(_rp, Token::InvalidToken);
            }

            match self.character {
                '\'' => {
                    self.read_character();
                    self.column = self.column + 1;
                    return Ref::new(_rp, Token::CharacterLiteral(value.to_string(_rp)));
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
                        _ => return Ref::new(_rp, Token::InvalidToken),
                    }
                }
                _ => value.append_character(self.character),
            }
        }
    }

    fn scan_numeric_literal(&mut self, _pr: &Region, _rp: *mut Page) -> Ref<Token> {
        let _r = Region::create(_pr);
        let mut value: Ref<StringBuilder> = StringBuilder::from_character(_rp, self.character);

        self.read_character();
        self.column = self.column + 1;

        if self.is_at_end() {
            return Ref::new(_rp, Token::NumericLiteral(value.to_string(_rp)));
        }
        if self.character == 'x' {
            return self.scan_hex_literal(&_r, _rp);
        } else {
            self.read_character();
            self.column = self.column + 1;
        }

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Ref::new(_rp, Token::NumericLiteral(value.to_string(_rp)));
            }

            let c = self.character;
            if (c >= '0') && (c <= '9') {
                value.append_character(self.character)
            } else {
                return Ref::new(_rp, Token::NumericLiteral(value.to_string(_rp)));
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
                return Ref::new(_rp, Token::HexLiteral(value.to_string(_rp)));
            }

            let c = self.character;
            if ((c >= '0') && (c <= '9'))
                || ((c >= 'a') && (c <= 'f'))
                || ((c >= 'A') && (c <= 'F'))
            {
                value.append_character(self.character)
            } else {
                return Ref::new(_rp, Token::HexLiteral(value.to_string(_rp)));
            }
        }
    }

    fn skip_whitespace(&mut self) {
        loop {
            if self.is_at_end {
                return;
            }

            match self.character {
                ' ' => {
                    self.read_character();;
                    self.column = self.column + 1;
                    continue;
                }

                '\t' => {
                    self.read_character();
                    self.column = self.column + 4;
                    continue;
                }

                '\r' => {
                    self.read_character();
                    continue;
                }

                '\n' => {
                    self.read_character();
                    self.column = 1;
                    self.line = self.line + 1;
                    continue;
                }

                '/' => {
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
            if self.is_at_end() {
                return;
            }

            match self.character {
                '\t' => {
                    self.read_character();
                    self.column = self.column + 4;
                    continue;
                }

                '\r' => {
                    self.read_character();
                    continue;
                }

                '\n' => {
                    self.read_character();
                    self.column = 1;
                    self.line = self.line + 1;
                    return;
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
                return;
            }

            match self.character {
                '/' => {
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
                    self.read_character();
                    self.column = self.column + 1;

                    if self.is_at_end() {
                        return;
                    } else {
                        if self.character == '/' {
                            self.read_character();
                            self.column = self.column + 1;
                            return;
                        }
                    }
                }

                '\t' => {
                    self.read_character();
                    self.column = self.column + 4;
                    continue;
                }

                '\r' => {
                    self.read_character();
                    continue;
                }

                '\n' => {
                    self.read_character();
                    self.column = 1;
                    self.line = self.line + 1;
                    return;
                }

                _ => {
                    self.read_character();
                    self.column = self.column + 1;
                    continue;
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
        self.is_at_end
    }
}

#[derive(Copy, Clone)]
pub struct Position {
    pub line: usize,
    pub column: usize,
}

#[derive(Copy, Clone)]
pub enum Token {
    InvalidToken,
    Identifier(String),
    StringLiteral(String),
    CharacterLiteral(String),
    NumericLiteral(String),
    HexLiteral(String),
    Punctuation(String),
}
