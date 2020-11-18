use std::collections::HashSet;
use std::str::Chars;

pub struct Position {
    pub line: usize,
    pub column: usize,
}

pub enum Token {
    Empty,
    Invalid,
    Identifier(String),
    Attribute(String),
    Literal(Literal),
    Punctuation(String),
    LineFeed,
    Colon,
    Semicolon,
}

pub enum Literal {
    StringLiteral(String),
    Fragment(String),
    Integer(String),
    FloatingPoint(String),
    Hex(String),
}

pub struct Lexer<'a> {
    pub token: Token,
    character: Option<char>,
    chars: Chars<'a>,
    previous_line: usize,
    previous_column: usize,
    pub line: usize,
    pub column: usize,
}

impl<'a> Lexer<'a> {
    pub fn new(deck: &'a str) -> Lexer {
        let mut lexer = Lexer {
            chars: deck.chars(),
            token: Token::Empty,
            character: None,
            previous_line: 1,
            previous_column: 0,
            line: 1,
            column: 0,
        };
        lexer.read_character();
        lexer
    }

    fn read_character(&mut self) {
        self.character = self.chars.next();
        match self.character {
            None => (),
            Some(c) => match c {
                '\r' => (),
                '\n' => {
                    self.column = 0;
                    self.line = self.line + 1;
                }
                '\t' => self.column = self.column + 4,
                _ => self.column = self.column + 1,
            },
        }
    }

    pub fn advance(&mut self) {
        self.skip_whitespace(false);
        self.previous_line = self.line;
        self.previous_column = self.column;
        match self.character {
            None => return,
            Some(c) => {
                if ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) {
                    self.token = self.scan_identifier();
                    return;
                }

                if (c >= '1') && (c <= '9') {
                    let mut value = String::new();
                    value.push(c);
                    self.token = self.scan_numeric_literal(value);
                    return;
                }

                match c {
                    '\n' => {
                        self.token = self.scan_line_feed();
                    }
                    ':' => {
                        self.read_character();
                        self.token = Token::Colon;
                    }
                    ';' => {
                        self.read_character();
                        self.token = Token::Semicolon;
                    }
                    '0' => {
                        let mut value = String::new();
                        value.push(c);
                        self.token = self.scan_numeric_or_hex_literal(value);
                    }
                    '@' => {
                        self.read_character();
                        self.token = self.scan_attribute();
                        return;
                    }
                    '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                        let mut operation = String::new();
                        operation.push(c);
                        self.token = self.scan_operator(operation)
                    }
                    '\"' => self.token = { self.scan_string_literal() },
                    '\'' => self.token = { self.scan_string_identifier() },
                    '`' => self.token = { self.scan_fragment_literal() },
                    '}' | ')' | ']' | '.' | '?' => {
                        let mut punctuation_string = String::new();
                        punctuation_string.push(c);
                        self.read_character();
                        self.token = Token::Punctuation(punctuation_string);
                    }
                    '{' | '(' | '[' | ',' => {
                        let mut punctuation_string = String::new();
                        punctuation_string.push(c);
                        self.read_character();
                        self.skip_whitespace(true);
                        self.token = Token::Punctuation(punctuation_string);
                    }
                    _ => self.token = Token::Invalid,
                }
            }
        }
    }

    pub fn empty(&mut self) {
        self.token = Token::Empty;
    }

    fn scan_line_feed(&mut self) -> Token {
        loop {
            self.read_character();
            self.skip_whitespace(false);
            match self.character {
                None => {
                    return Token::LineFeed;
                }

                Some(c) => match c {
                    '\n' => continue,
                    _ => return Token::LineFeed,
                },
            }
        }
    }

    fn scan_identifier(&mut self) -> Token {
        let mut name = String::new();
        loop {
            match self.character {
                None => {
                    if name.len() == 0 {
                        return Token::Invalid;
                    } else {
                        return Token::Identifier(name);
                    }
                }

                Some(c) => match c {
                    'a'..='z' | 'A'..='Z' | '0'..='9' | '_' => {
                        name.push(c);
                        self.read_character();
                    }
                    _ => return Token::Identifier(name),
                },
            }
        }
    }

    fn scan_attribute(&mut self) -> Token {
        let mut name = String::new();
        loop {
            match self.character {
                None => {
                    if name.len() == 0 {
                        return Token::Invalid;
                    } else {
                        return Token::Attribute(name);
                    }
                }

                Some(c) => match c {
                    'a'..='z' | 'A'..='Z' | '0'..='9' | '_' => {
                        name.push(c);
                        self.read_character();
                    }
                    _ => return Token::Attribute(name),
                },
            }
        }
    }

    fn scan_operator(&mut self, mut operation: String) -> Token {
        loop {
            match self.character {
                None => {
                    if operation.len() == 0 {
                        return Token::Invalid;
                    } else {
                        return Token::Identifier(operation);
                    }
                }
                Some(c) => match c {
                    '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                        operation.push(c);
                        self.read_character();
                    }

                    _ => return Token::Identifier(operation),
                },
            }
        }
    }

    fn scan_string_literal(&mut self) -> Token {
        let mut value = String::new();

        loop {
            self.read_character();

            match self.character {
                None => return Token::Invalid,
                Some(c) => match c {
                    '\"' => {
                        self.read_character();
                        return Token::Literal(Literal::StringLiteral(value));
                    }

                    '\\' => {
                        self.read_character();
                        match self.character {
                            None => return Token::Invalid,
                            Some(c) => match c {
                                '\"' | '\\' | '\'' => {
                                    value.push('\\');
                                    value.push(c);
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
                            },
                        }
                    }

                    _ => {
                        value.push(c);
                    }
                },
            }
        }
    }

    fn scan_string_identifier(&mut self) -> Token {
        let mut value = String::new();

        loop {
            self.read_character();

            match self.character {
                None => return Token::Invalid,
                Some(c) => match c {
                    '\'' => {
                        self.read_character();
                        return Token::Identifier(value);
                    }

                    _ => {
                        value.push(c);
                    }
                },
            }
        }
    }

    fn scan_fragment_literal(&mut self) -> Token {
        let mut value = String::new();

        loop {
            self.read_character();

            match self.character {
                None => return Token::Invalid,
                Some(c) => match c {
                    '`' => {
                        self.read_character();
                        return Token::Literal(Literal::Fragment(value));
                    }

                    '\\' => {
                        self.read_character();
                        match self.character {
                            None => return Token::Invalid,
                            Some(c) => match c {
                                '`' => {
                                    value.push(c);
                                }
                                _ => {
                                    value.push('\\');
                                    value.push(c);
                                }
                            },
                        }
                    }

                    _ => {
                        value.push(c);
                    }
                },
            }
        }
    }

    fn scan_numeric_or_hex_literal(&mut self, mut value: String) -> Token {
        self.read_character();
        match self.character {
            None => return Token::Literal(Literal::Integer(value)),
            Some(c) => match c {
                '0'..='9' => {
                    value.push(c);
                    return self.scan_numeric_literal(value);
                }
                '.' => {
                    value.push(c);
                    return self.scan_fraction(value);
                }
                'E' | 'e' => {
                    value.push(c);
                    return self.scan_exponent(value);
                }
                'x' => {
                    return self.scan_hex_literal();
                }
                _ => return Token::Literal(Literal::Integer(value)),
            },
        }
    }

    fn scan_numeric_literal(&mut self, mut value: String) -> Token {
        loop {
            self.read_character();
            match self.character {
                None => {
                    return Token::Literal(Literal::Integer(value));
                }
                Some(c) => match c {
                    '0'..='9' => {
                        value.push(c);
                    }
                    '.' => {
                        value.push(c);
                        return self.scan_fraction(value);
                    }
                    'E' | 'e' => {
                        value.push(c);
                        return self.scan_exponent(value);
                    }
                    _ => return Token::Literal(Literal::Integer(value)),
                },
            }
        }
    }

    fn scan_fraction(&mut self, mut value: String) -> Token {
        loop {
            self.read_character();
            match self.character {
                None => {
                    return Token::Literal(Literal::FloatingPoint(value));
                }
                Some(c) => match c {
                    '0'..='9' => {
                        value.push(c);
                    }
                    'E' | 'e' => {
                        value.push(c);
                        return self.scan_exponent(value);
                    }
                    _ => return Token::Literal(Literal::FloatingPoint(value)),
                },
            }
        }
    }

    fn scan_exponent(&mut self, mut value: String) -> Token {
        loop {
            self.read_character();
            match self.character {
                None => {
                    return Token::Literal(Literal::FloatingPoint(value));
                }
                Some(c) => match c {
                    '0'..='9' => {
                        value.push(c);
                    }
                    _ => return Token::Literal(Literal::FloatingPoint(value)),
                },
            }
        }
    }

    fn scan_hex_literal(&mut self) -> Token {
        let mut value = String::new();
        loop {
            self.read_character();
            match self.character {
                None => {
                    return Token::Literal(Literal::FloatingPoint(value));
                }
                Some(c) => match c {
                    '0'..='9' | 'a'..='f' | 'A'..='F' => {
                        value.push(c);
                    }
                    _ => return Token::Literal(Literal::Hex(value)),
                },
            }
        }
    }

    fn skip_whitespace(&mut self, line_feed: bool) {
        loop {
            match self.character {
                None => return,
                Some(c) => match c {
                    ' ' => {
                        self.read_character();
                        continue
                    }
                    '\t' => {
                        self.read_character();
                        continue
                    }
                    '\r' => {
                        self.read_character();
                        continue
                    }
                    '\n' => if line_feed {
                        self.read_character();
                        continue
                    }
                    else {
                        return
                    }
                    '/' => {
                        self.read_character();
                        match self.character {
                            None => return,
                            Some(c) => match c {
                                '/' => {
                                    self.read_character();
                                    self.handle_single_line_comment()
                                }
                                '*' => {
                                    self.read_character();
                                    self.handle_multi_line_comment()
                                }
                                _ => return,
                            },
                        }
                    }

                    _ => {
                        return
                    }
                },
            }
        }
    }

    fn handle_single_line_comment(&mut self) {
        loop {
            match self.character {
                None => return,
                Some(c) => match c {
                    '\t' => {
                        self.read_character();
                        continue;
                    }
                    '\r' => {
                        continue;
                    }
                    '\n' => {
                        self.read_character();
                        return;
                    }
                    _ => {
                        self.read_character();
                        continue;
                    }
                },
            }
        }
    }

    fn handle_multi_line_comment(&mut self) {
        loop {
            match self.character {
                None => return,
                Some(c) => match c {
                    '/' => {
                        self.read_character();
                        match self.character {
                            None => return,
                            Some(c) => match c {
                                '*' => self.handle_multi_line_comment(),
                                _ => return,
                            },
                        }
                    }

                    '*' => {
                        self.read_character();
                        match self.character {
                            None => return,
                            Some(c) => match c {
                                '/' => {
                                    self.read_character();
                                    return;
                                }
                                _ => (),
                            },
                        }
                    }

                    '\t' => {
                        self.read_character();
                        continue;
                    }

                    '\r' => {
                        continue;
                    }

                    '\n' => {
                        self.read_character();
                        continue;
                    }

                    _ => {
                        self.read_character();
                        continue;
                    }
                },
            }
        }
    }

    pub fn parse_keyword(&mut self, fixed_string: String) -> bool {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Identifier(name) => {
                let right_keyword = *name == fixed_string;
                if right_keyword {
                    self.empty();
                }
                return right_keyword;
            }
            _ => return false,
        }
    }

    pub fn parse_identifier(&mut self, keywords: &HashSet<String>) -> Option<String> {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Identifier(name) => {
                if keywords.contains(name) {
                    return None;
                }
                let ret = String::from(name);
                self.empty();
                return Some(ret);
            }
            _ => return None,
        }
    }

    pub fn parse_attribute(&mut self) -> Option<String> {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Attribute(name) => {
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
            Token::Literal(Literal::StringLiteral(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::StringLiteral(ret))
            }
            Token::Literal(Literal::Integer(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::Integer(ret))
            }
            Token::Literal(Literal::FloatingPoint(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::FloatingPoint(ret))
            }
            Token::Literal(Literal::Hex(name)) => {
                let ret = String::from(name);
                self.empty();
                Some(Literal::Hex(ret))
            }
            _ => None,
        }
    }

    pub fn parse_colon(&mut self) -> bool {
        match &self.token {
            Token::Empty => self.advance(),
            _ => (),
        }
        match &self.token {
            Token::Colon | Token::LineFeed => {
                self.empty();
                true
            }
            _ => false,
        }
    }

    pub fn is_at_end(&mut self) -> bool {
        match self.character {
            None => true,
            Some(_) => false,
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
}
