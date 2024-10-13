namespace scaly {
namespace compiler {
    
using namespace scaly::containers;


struct EmptyToken {};
struct InvalidToken {};

struct IdentifierToken {
    IdentifierToken(Vector<char> name) : name(name) {}
    Vector<char> name;
};

struct AttributeToken {
    AttributeToken(Vector<char> name) : name(name) {}
    Vector<char> name;
};

struct PunctuationToken {
    PunctuationToken(char sign) : sign(sign) {}
    char sign;
};

struct StringToken {
    StringToken(Vector<char> value) : value(value) {}
    Vector<char> value;
};

struct FragmentToken {
    FragmentToken(Vector<char> value) : value(value) {}
    Vector<char> value;
};

struct IntegerToken {
    IntegerToken(Vector<char> value) : value(value) {}
    Vector<char> value;
};

struct BooleanToken {
    BooleanToken(bool value) : value(value) {}
    bool value;
};

struct FloatingPointToken {
    FloatingPointToken(Vector<char> value) : value(value) {}
    Vector<char> value;
};

struct HexToken {
    HexToken(Vector<char> value) : value(value) {}
    Vector<char> value;
};

struct LiteralToken : Object {
    LiteralToken(StringToken string) : _tag(String), _String(string) {}
    LiteralToken(FragmentToken fragment) : _tag(Fragment), _Fragment(fragment) {}
    LiteralToken(IntegerToken integer) : _tag(Integer), _Integer(integer) {}
    LiteralToken(BooleanToken boolean) : _tag(Boolean), _Boolean(boolean) {}
    LiteralToken(FloatingPointToken floating_point) : _tag(FloatingPoint), _FloatingPoint(floating_point) {}
    LiteralToken(HexToken hex) : _tag(Hex), _Hex(hex) {}
    enum {
        String,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } _tag;
    union {
        StringToken _String;
        FragmentToken _Fragment;
        IntegerToken _Integer;
        BooleanToken _Boolean;
        FloatingPointToken _FloatingPoint;
        HexToken _Hex;

    };
};

struct LineFeedToken {};
struct ColonToken {};

struct Token : Object {
    Token(EmptyToken empty) : _tag(Empty) {}
    Token(InvalidToken invalid) : _tag(Invalid) {}
    Token(IdentifierToken identifier) : _tag(Identifier), _Identifier(identifier) {}
    Token(AttributeToken attribute) : _tag(Attribute), _Attribute(attribute) {}
    Token(PunctuationToken punctuation) : _tag(Punctuation), _Punctuation(punctuation) {}
    Token(LiteralToken literal) : _tag(Literal), _Literal(literal) {}
    Token(LineFeedToken line_feed) : _tag(LineFeed) {}
    enum {
        Empty,
        Invalid,
        Identifier,
        Attribute,
        Punctuation,
        Literal,
        LineFeed,
    } _tag;
    union {
        EmptyToken _Empty;
        InvalidToken _Invalid;
        IdentifierToken _Identifier;
        AttributeToken _Attribute;
        PunctuationToken _Punctuation;
        LiteralToken _Literal;
        LineFeedToken _LineFeed;
    };
};

struct Lexer : Object {
    Token token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    Lexer(String deck)
      : chars(StringIterator(deck)),
        token(Token(EmptyToken()))
    {
        character = nullptr;
        previous_position = 0,
        position = 0,
        read_character();
        skip_whitespace(true);
    }

    void read_character() {
        this->character = chars.next();
        if (this->character != nullptr)
            this->position++;        
    }

    void advance() {
        this->previous_position = this->position;

        if (this->character == nullptr)
            return;

        auto c = *character;

        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
            this->token = scan_identifier();
            skip_whitespace(false);
            return;
        }

        if ((c >= '1') && (c <= '9')) {
            this->token = scan_integer_literal(this->character, 0);
            skip_whitespace(false);
            return;
        }

        switch (c) {
            case '\n':
                this->token = scan_line_feed(Page::get(this)->allocate_exclusive_page());
                break;

            case ':':
                read_character();
                this->token = Token(LineFeedToken());
                break;

            case '0':
                {
                    Region _r_1;
                    StringBuilder& value = *new (alignof(StringBuilder), _r_1.get_page()) StringBuilder();
                    this->token = scan_numeric_literal();
                }
                break;

            case '@':
                read_character();
                this->token = scan_attribute();
                break;

            case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '~': case '<': case '>':
            {
                this->token = scan_operator();
                break;
            }

            case '\"':
                this->token = scan_string_literal();
                break;

            case '\'':
                this->token = scan_string_identifier();
                break;

            case '`':
                this->token = scan_fragment_literal();
                break;

            case '}': case ')': case ']': case '.': case '?': case '!': case '$': case '#': case '^':
            {
                this->token = Token(PunctuationToken(*this->character));
                this->read_character();
                break;
            }

            case '{': case '(': case '[': case ',':
            {
                this->token = Token(PunctuationToken(*this->character));
                this->read_character();
                this->skip_whitespace(true);
                break;
            }

            default:
                this->token = Token(InvalidToken());
                break;
        }
        skip_whitespace(false);
    }

    void empty() {
        this->token = Token(EmptyToken());
    }

    Token scan_line_feed(Page* _rp) {
        while (true) {
            read_character();
            skip_whitespace(false);
            if (this->character == nullptr)
                return Token(LineFeedToken());

            if (*this->character == '\n')
                continue;

            return Token(LineFeedToken());
        }
    }

    Token scan_identifier() {
        char* start = this->character;
        size_t length = 0;
        while (true) {
            if (character == nullptr) {
                if (length == 0)
                    return Token(InvalidToken());
                else
                    return Token(IdentifierToken(Vector<char>(start, length)));
            }
            else {
                char c = *character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_')) {
                    this->read_character();
                    length++;
                }
                else {
                    return Token(IdentifierToken(Vector<char>(start, length)));
                }
            }
        }
    }

    Token scan_attribute() {
        char* start = this->character;
        size_t length = 0;
        while (true) {
            if (character == nullptr) {
                if (length == 0)
                    return Token(InvalidToken());
                else
                    return Token(AttributeToken(Vector<char>(start, length)));
            }
            else {
                char c = *character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_')) {
                    this->read_character();
                    length++;
                }
                else {
                    return Token(AttributeToken(Vector<char>(start, length)));
                }
            }
        }
    }

    Token scan_operator() {
        char* start = this->character;
        size_t length = 0;
        while (true) {
            read_character();
            length++;
            if (character == nullptr) {
                if (length == 1)
                    return Token(InvalidToken());
                else
                    return Token(IdentifierToken(Vector<char>(start, length)));
            }

            auto c = *character;
            switch (c)
            {
                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                    break;

                default:{
                    return Token(IdentifierToken(Vector<char>(start, length)));
                }
            }
        }
    }

    Token scan_string_literal() {
        char* start = this->character + 1;
        size_t length = 0;
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(InvalidToken());

            switch (*character) {
                case '\"':
                    read_character();
                    return Token(LiteralToken(StringToken(Vector<char>(start, length - 1))));

                case '\\': 
                    read_character();
                    length++;
                    if (character == nullptr)
                        return Token(InvalidToken());

                    switch (*character) {
                        case '\"': case '\\': case '\'':
                            break;
                        case 'n':
                            break;
                        case 'r':
                            break;
                        case 't':
                            break;
                        case '0':
                            break;
                        default:
                            return Token(InvalidToken());                            
                    }
                    break;

                default:
                    break;
            }
        }
    }

    Token scan_string_identifier() {
        char* start = this->character + 1;
        size_t length = 0;
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(InvalidToken());

            auto c = *character;
            switch (c) {
                case '\'':
                    read_character();
                        return Token(IdentifierToken(Vector<char>(start, length - 1)));
                default:
                    break;
            }
        }
    }

    Token scan_fragment_literal() {
        char* start = this->character + 1;
        size_t length = 0;
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(InvalidToken());

            switch (*character)
            {
                case '`':
                    read_character();
                    return Token(LiteralToken(FragmentToken(Vector<char>(start, length - 1))));

                case '\\': 
                    read_character();
                    length++;
                    if (character == nullptr)
                        return Token(InvalidToken());

                    switch (*character) {
                        case '`': case '\\': case '\'':
                            break;
                        case 'n':
                            break;
                        case 'r':
                            break;
                        case 't':
                            break;
                        case '0':
                            break;
                        default:
                            return Token(InvalidToken());                            
                    }
                    break;

                default:
                    break;
            }
        }
    }

    Token scan_numeric_literal() {
        char* start = this->character;
        size_t length = 0;
        read_character();
        length++;
        if (character == nullptr)
            return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));

        auto c = *character;

        if ((c >= '0') && (c <= '9')) {
            return scan_integer_literal(start, length);
        }

        switch (c) {
            case '.':
                return scan_fraction(start, length);

            case 'E':
            case 'e':
                return scan_exponent(start, length);

            case 'x':
                return scan_hex_literal(start, length);

            case 'B':
                return scan_boolean_literal();

            default:
                return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));
        }
    }

    Token scan_integer_literal(char* start, size_t length) {
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));

            auto c = *character;

            if ((c >= '0') && (c <= '9')) {
                continue;
            }

            switch (c) {
                case '.':
                    return scan_fraction(start, length);

                case 'E':
                case 'e':
                    return scan_exponent(start, length);

                default:
                    return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));
            }
        }
    }

    Token scan_fraction(char* start, size_t length) {
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));

            auto c = *character;

            if ((c >= '0') && (c <= '9'))
                continue;

            switch (c) {
                case 'E':
                case 'e':
                    return scan_exponent(start, length);

                default:
                    return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));
            }
        }
    }

    Token scan_exponent(char* start, size_t length) {
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));

            auto c = *character;

            if ((c >= '0') && (c <= '9'))
                continue;

            return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));
        }
    }

    Token scan_hex_literal(char* start, size_t length) {
        while (true) {
            read_character();
            length++;
            if (character == nullptr)
                return Token(LiteralToken(HexToken(Vector<char>(start, length))));

            auto c = *character;

            if (((c >= '0') && (c <= '9')) || (c >= 'A') && (c <= 'F') || ((c >= 'a') && (c <= 'f')))
                continue;

            return Token(LiteralToken(HexToken(Vector<char>(start, length))));
        }
    }

    Token scan_boolean_literal() {
        read_character();
        if (character == nullptr)
            return Token(InvalidToken());

        auto c = *character;

        if (c != '0' && c != '1') {
            return Token(InvalidToken());
        }
        else {
            read_character();
            return Token(LiteralToken(BooleanToken(c == '1')));
        }
    }

    void skip_whitespace(bool line_feed) {
        while (true) {
            switch ((size_t)this->character) {
                case 0:
                    return;
                default:
                {
                    switch(*this->character) {
                        case ' ':
                            read_character();
                            continue;
                        case '\t':
                            read_character();
                            continue;
                        case '\r':
                            read_character();
                            continue;
                        case '\n':
                            if (line_feed) {
                                read_character();
                                continue;
                            }
                            else {
                                return;
                            }

                        case ';':
                            {
                                read_character();
                                switch ((size_t)this->character) {
                                    case 0:
                                        return;
                                    default:
                                    {
                                        if (*this->character != '*') {
                                            read_character();
                                            handle_single_line_comment();
                                        }
                                        else {
                                            read_character();
                                            handle_multi_line_comment();
                                        }
                                    }
                                }
                            }
                            break;
                        default:
                            return;
                    };
                }
            }
        }
    }

    void handle_single_line_comment() {
        while (true) {
            switch ((size_t)this->character) {
                case 0:
                    return;
                default:
                {
                    if (*this->character == '\n')
                    {
                        read_character();
                        return;
                    }
                    else
                    {
                        read_character();
                        continue;
                    }
                }
            }
        }
    }

    void handle_multi_line_comment() {
        while (true) {
            switch ((size_t)this->character) {
                case 0:
                    return;
                default:
                {
                    switch (*this->character)
                    {
                        case ';':
                            read_character();
                            switch ((size_t)this->character) {
                                case 0:
                                    return;
                                default:
                                    switch (*this->character)
                                    {
                                        case '*':
                                            handle_multi_line_comment();
                                        default:
                                            continue;
                                    }
                            }
                            break;

                        case '*':
                            read_character();
                            switch ((size_t)this->character) {
                                case 0:
                                    return;
                                default:
                                    switch (*this->character)
                                    {
                                        case ';':
                                            read_character();
                                            return;
                                    }
                            }
                            break;

                        default:
                            read_character();
                            continue;
                    }
                }
            }
        }
    }

    bool parse_keyword(Page* _rp, const String& fixed_string) {
        if (token._tag == Token::Empty)
            advance();

        switch (token._tag) {
            case Token::Identifier:
            {
                auto right_keyword = (fixed_string.equals(token._Identifier.name));
                if (right_keyword)
                    empty();

                return right_keyword;
            }

            default:
                return false;
        }
    }

    String* parse_identifier(Page* _rp, HashSet<String>& keywords) {
        if (token._tag == Token::Empty)
            advance();

        switch (token._tag) {
            case Token::Identifier: {
                Region _r_1;
                if (keywords.contains(String(_r_1.get_page(), token._Identifier.name)))
                    return nullptr;
                auto ret = new(alignof(String), _rp) String(_rp, token._Identifier.name);
                empty();
                return ret;
            }
            default:
                return nullptr;
        }
    }

    String* parse_attribute(Page* _rp) {
        if (token._tag == Token::Empty)
            advance();

        switch (token._tag) {
            case Token::Attribute:
            {
                auto ret = new(alignof(String), _rp) String(_rp, token._Attribute.name);
                empty();
                return ret;
            }
            default:
                return nullptr;
        }
    }

    bool parse_punctuation(char character) {
        if (token._tag == Token::Empty)
            advance();

        switch (token._tag) {
            case Token::Punctuation:
            {
                bool ret = (character == token._Punctuation.sign);
                if (ret)
                    empty();

                return ret;
            }
            default:
                return false;
        }
    }

    bool parse_colon(Page* _rp) {
        if (token._tag == Token::Empty)
            advance();

        switch (token._tag)
        {
            case Token::LineFeed:
                empty();
                return true;

            case Token::Empty:
                return true;

            default:
                return false;
        }
    }

    bool is_at_end() {
        if (character == nullptr)
            return true;

        return false;
    }

};

}
}