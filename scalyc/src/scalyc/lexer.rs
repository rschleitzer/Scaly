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

        let c = self.character;

        if ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) {
            unsafe {(*(Page::own(&self.token))).reset();}
            self.token = self.scan_identifier();
            return
        }

        if (c >= '0') && (c <= '9') {
            // token = self.scan_numeric_literal();
            return
        }

        match c {
            '+' | '-' | '*' | '/' | '=' | '%' | '&' | '|' | '^' | '~' | '<' | '>' => {
                unsafe {(*(Page::own(&self.token))).reset();}
                self.token = self.scan_operator();
            }

            '\"' => {
                unsafe {(*(Page::own(&self.token))).reset();}
                self.token = self.scan_string_literal();
            }

            '\'' => {
                unsafe {(*(Page::own(&self.token))).reset();}
                self.token = self.scan_character_literal();
            }

            '{' | '}' | '(' | ')' | '[' | ']' | '.' | ',' | ':' | ';' | '?' | '!' | '@' | '#' | '$' | '_' | '`' => {
                unsafe {(*(Page::own(&self.token))).reset();}
                self.token = Token::_Punctuation(String::from_character(Page::own(self), c));
                self.read_character();
                self.column = self.column + 1;
            }

            _ => {
                self.token = Token::InvalidToken
            }
        }
    }

    fn scan_identifier(&mut self) -> Identifier {
        string name = new string(text[position], 1);

        do
        {
            position = position + 1;
            column = column + 1;

            if (position == end)
                return (new Identifier(name));

            char c = text[position];
            if (((c >= 'a') && (c <= 'z')) ||
                ((c >= 'A') && (c <= 'Z')) ||
                ((c >= '0') && (c <= '9')) ||
                    (c == '_'))
                name = name + c;
            else
                return (new Identifier(name));
        }
        while (true);
    }

    Identifier scanOperator()
    {
        string operation = new string(text[position], 1);

        do
        {
            position = position + 1;
            column = column + 1;

            if (position == end)
                return (new Identifier(operation));

            switch (text[position])
            {
                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                    operation = operation + text[position];
                    break;

                default:
                    return (new Identifier(operation));
            }
        }
        while (true);
    }

    public Token scanStringLiteral()
    {
        var value = "";

        do
        {
            position = position + 1;
            column = column + 1;

            if (position == end)
                return (new InvalidToken());

            switch (text[position])
            {
                case '\"':
                {
                    position = position + 1;
                    column = column + 1;
                    return (new StringLiteral(value));
                }
                case '\\':
                {
                    position = position + 1;
                    column = column + 1;
                    switch (text[position])
                    {
                        case '\"': case '\\': case '\'':
                        {
                            value = value + '\\';
                            value = value + text[position];
                            break;
                        }
                        case 'n': value = value + "\\n"; break;
                        case 'r': value = value + "\\r"; break;
                        case 't': value = value + "\\t"; break;
                        case '0': value = value + "\\0"; break;
                        default: return (new InvalidToken());
                    }
                    break;
                }
                default:
                    value = value + text[position];
                    break;
            }
        }
        while (true);
    }

    Token scanCharacterLiteral()
    {
        var value = "";

        do
        {
            position = position + 1;
            column = column + 1;

            if (position == end)
                return (new InvalidToken());

            switch (text[position])
            {
                case '\'':
                {
                    position = position + 1;
                    column = column + 1;
                    return (new CharacterLiteral(value));
                }
                case '\\':
                {
                    position = position + 1;
                    column = column + 1;
                    switch (text[position])
                    {
                        case '\"': case '\\': case '\'':
                        {
                            value = value + '\\';
                            value = value + text[position];
                            break;
                        }
                        case 'n': value = value + "\\n"; break;
                        case 'r': value = value + "\\r"; break;
                        case 't': value = value + "\\t"; break;
                        case '0': value = value + "\\0"; break;
                        default: return (new InvalidToken());
                    }
                    break;
                }
                default:
                    value = value + text[position];
                    break;
            }
        }
        while (true);
    }

    NumericLiteral scanNumericLiteral()
    {
        var value = new string(text[position], 1);

        position = position + 1;
        column = column + 1;

        if (position == end)
            return (new NumericLiteral(value));
        
        var x = text[position];
        if (x == 'x')
        {
            return scanHexLiteral();
        }
        else
        {
            position = position - 1;
            column = column - 1;
        }

        do
        {
            position = position + 1;
            column = column + 1;

            if (position == end)
                return (new NumericLiteral(value));

            var c = text[position];
            if ((c >= '0') && (c <= '9'))
                value = value + text[position];
            else
                return (new NumericLiteral(value));
        }
        while (true);
    }

    HexLiteral scanHexLiteral()
    {
        var value = new string(text[position], 1);

        do
        {
            position = position + 1;
            column = column + 1;

            if (position == end)
                return (new HexLiteral(value));

            var c = text[position];
            if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
                value = value + text[position];
            else
                return (new HexLiteral(value));
        }
        while (true);
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
    _Punctuation(String),
}
