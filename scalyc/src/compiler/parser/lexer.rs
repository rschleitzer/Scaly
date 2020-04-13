use std::str::Chars;

pub struct Position {
    pub line: usize,
    pub column: usize,
}

pub enum Token {
    Empty,
    Invalid,
    Identifier(String),
    Literal(Literal),
    Punctuation(String),
}

pub enum Literal {
    String(String),
    Character(String),
    Numeric(String),
    Hex(String),
}

pub struct Lexer<'a> {
    pub token: Token,
    character: char,
    chars: Chars<'a>,
    is_at_end: bool,
    previous_line: usize,
    previous_column: usize,
    pub line: usize,
    pub column: usize,
}

impl<'a> Lexer<'a> {
    pub fn new(deck: &'a str) -> Lexer {
        Lexer {
            chars: deck.chars(),
            is_at_end: false,
            token: Token::Empty,
            character: 0 as char,
            previous_line: 1,
            previous_column: 0,
            line: 1,
            column: 0,
        }
    }

    fn read_character(&mut self) {
        if self.character == 0 as char {
            match self.chars.next() {
                Some(read_result) => {
                    self.character = read_result;
                }
                None => {
                    self.is_at_end = true;
                    self.character = 0 as char;
                }
            }
        }
    }

    pub fn advance(&mut self) {
        self.skip_whitespace();
        self.previous_line = self.line;
        self.previous_column = self.column;
        if self.is_at_end {
            return;
        }

        let c = self.character;

        if ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) {
            self.token = self.scan_identifier();
            return;
        }

        if (c >= '0') && (c <= '9') {
            self.token = self.scan_numeric_literal();
            return;
        }

        match c {
            '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                self.token = self.scan_operator()
            }
            '\"' => self.token = self.scan_string_literal(),
            '\'' => self.token = self.scan_character_literal(),
            '{' | '}' | '(' | ')' | '[' | ']' | '.' | ',' | ':' | ';' | '?' | '!' | '@' | '#'
            | '$' | '_' | '`' => {
                let mut punctuation_string = String::new();
                punctuation_string.push(c);
                self.token = Token::Punctuation(punctuation_string);
                self.character = 0 as char;
                self.column = self.column + 1;
            }
            _ => self.token = Token::Invalid,
        }
    }

    pub fn empty(&mut self) {
        self.token = Token::Empty;
    }

    fn scan_identifier(&mut self) -> Token {
        let mut name = String::new();

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                if name.len() == 0 {
                    return Token::Invalid;
                } else {
                    return Token::Identifier(name);
                }
            }

            let c = self.character;
            if ((c >= 'a') && (c <= 'z'))
                || ((c >= 'A') && (c <= 'Z'))
                || ((c >= '0') && (c <= '9'))
                || (c == '_')
            {
                name.push(c);
                self.character = 0 as char
            } else {
                return Token::Identifier(name);
            }
        }
    }

    fn scan_operator(&mut self) -> Token {
        let mut operation = String::new();

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                if operation.len() == 0 {
                    return Token::Invalid;
                } else {
                    return Token::Identifier(operation);
                }
            }

            match self.character {
                '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                    operation.push(self.character);
                    self.character = 0 as char
                }

                _ => return Token::Identifier(operation),
            }
        }
    }

    fn scan_string_literal(&mut self) -> Token {
        let mut value = String::new();

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Token::Invalid;
            }

            match self.character {
                '\"' => {
                    self.read_character();
                    self.column = self.column + 1;
                    return Token::Literal(Literal::String(value));
                }

                '\\' => {
                    self.read_character();
                    self.column = self.column + 1;
                    match self.character {
                        '\"' | '\\' | '\'' => {
                            value.push('\\');
                            value.push(self.character);
                        }
                        'n' => {
                            value.push('\\');
                            value.push('n');
                        }
                        'r' => {
                            value.push('\\');
                            value.push('r');
                        }
                        't' => {
                            value.push('\\');
                            value.push('t');
                        }
                        '0' => {
                            value.push('\\');
                            value.push('0');
                        }
                        _ => return Token::Invalid,
                    }
                }
                _ => value.push(self.character),
            }
        }
    }

    fn scan_character_literal(&mut self) -> Token {
        let mut value = String::new();

        loop {
            self.read_character();
            self.column = self.column + 1;

            if self.is_at_end() {
                return Token::Invalid;
            }

            match self.character {
                '\'' => {
                    self.read_character();
                    self.column = self.column + 1;
                    return Token::Literal(Literal::Character(value));
                }

                '\\' => {
                    self.read_character();
                    self.column = self.column + 1;
                    match self.character {
                        '\"' | '\\' | '\'' => {
                            value.push('\\');
                            value.push(self.character);
                        }
                        'n' => {
                            value.push('\\');
                            value.push('n');
                        }
                        'r' => {
                            value.push('\\');
                            value.push('r');
                        }
                        't' => {
                            value.push('\\');
                            value.push('t');
                        }
                        '0' => {
                            value.push('\\');
                            value.push('0');
                        }
                        _ => return Token::Invalid,
                    }
                }
                _ => {
                    value.push(self.character);
                    self.character = 0 as char
                }
            }
        }
    }

    fn scan_numeric_literal(&mut self) -> Token {
        let c = self.character;
        self.character = 0 as char;
        self.read_character();
        self.column = self.column + 1;

        if self.is_at_end() {
            let mut char_string = String::new();
            char_string.push(c);
            return Token::Literal(Literal::Numeric(char_string));
        }

        if c == '0' && self.character == 'x' {
            return self.scan_hex_literal();
        }

        let mut value = String::new();

        value.push(c);

        loop {
            if (self.character >= '0') && (self.character <= '9') {
                value.push(self.character);
                self.character = 0 as char;
                self.read_character();
                self.column = self.column + 1;
            } else {
                return Token::Literal(Literal::Numeric(value));
            }
        }
    }

    fn scan_hex_literal(&mut self) -> Token {
        let mut value = String::new();
        value.push('0');
        value.push('x');

        self.character = 0 as char;
        self.read_character();

        if self.is_at_end() {
            value.push('0');
            return Token::Literal(Literal::Hex(value));
        }

        loop {
            let c = self.character;
            if ((c >= '0') && (c <= '9'))
                || ((c >= 'a') && (c <= 'f'))
                || ((c >= 'A') && (c <= 'F'))
            {
                value.push(c);
            } else {
                return Token::Literal(Literal::Hex(value));
            }

            self.character = 0 as char;
            self.read_character();
            self.column = self.column + 1;
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

    pub fn _parse_keyword(&mut self, fixed_string: String) -> bool {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Identifier(name) => {
                let right_keyword = *name == fixed_string;
                self.empty();
                return right_keyword;
            }
            _ => return false,
        }
    }

    pub fn _parse_identifier(&mut self) -> Option<String> {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Identifier(name) => {
                let ret = String::from(name);
                self.empty();
                return Some(ret);
            }
            _ => return None,
        }
    }

    pub fn parse_punctuation(&mut self, fixed_string: String) -> bool {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Punctuation(name) => {
                let ret = *name == fixed_string;
                if ret {
                    self.empty();
                }
                return ret;
            }
            _ => return false,
        }
    }

    pub fn parse_literal(&mut self) -> Option<Literal> {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Literal(Literal::String(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::String(ret))
            }
            Token::Literal(Literal::Character(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::Character(ret))
            }
            Token::Literal(Literal::Numeric(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::Numeric(ret))
            }
            Token::Literal(Literal::Hex(name)) => {
                let ret = String::from(name);
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
