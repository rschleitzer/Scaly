namespace scaly::compiler {
    
using namespace scaly::containers;


struct EmptyToken {};
struct InvalidToken {};

struct IdentifierToken {
    String name;
};

struct AttributeToken {
    String name;
};

struct PunctuationToken {
    String sign;
};

struct StringToken {
    String value;
};

struct FragmentToken {
    String value;
};

struct IntegerToken {
    String value;
};

struct BooleanToken {
    bool value;
};

struct FloatingPointToken {
    String value;
};

struct HexToken {
    String value;
};

struct LiteralToken {
    enum {
        String,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } tag;
    union {
        StringToken string;
        FragmentToken fragment;
        IntegerToken integer;
        BooleanToken boolean;
        FloatingPointToken floatingPoint;
        HexToken hex;

    };
};

struct LineFeedToken {};
struct ColonToken {};
struct SemicolonToken {};

struct Token : Object {
    enum {
        Empty,
        Invalid,
        Identifier,
        Attribute,
        Punctuation,
        Literal,
        LineFeed,
        Colon,
        Semicolon,
    } tag;
    union {
        EmptyToken empty;
        InvalidToken invalid;
        IdentifierToken identifier;
        AttributeToken attribute;
        PunctuationToken punctuation;
        LiteralToken literal;
        LineFeedToken lineFeed;
        ColonToken colon;
        SemicolonToken semicolon;
    };
};

struct Lexer : Object {
    Token* token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    static Lexer* create(Page* _rp, String& deck) {
        auto lexer = new(alignof(Lexer), _rp) Lexer();
        lexer->token = new(alignof(Token), Page::get(lexer)->allocate_exclusive_page()) Token();
        lexer->token->tag = Token::Empty;
        lexer->character = nullptr;
        lexer->chars = StringIterator::create(deck);
        lexer->previous_position = 0,
        lexer->position = 0,
        lexer->read_character();
        lexer->skip_whitespace(true);
        return lexer;
    }

    void read_character() {
        this->character = chars.next();
        if (this->character != nullptr)
            this->position++;        
    }

    void advance(Page* _rp) {
        skip_whitespace(false);
        this->previous_position = this->position;

        if (this->character == nullptr)
            return;

        auto c = *character;

        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
            if (this->token != nullptr)
                Page::get(this->token)->clear();
            this->token = scan_identifier(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token));
            return;
        }

        if ((c >= '1') && (c <= '9')) {
            if (this->token != nullptr)
                Page::get(this->token)->clear();
            this->token = scan_integer_literal(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token), c);
            return;
        }

        switch (c) {
            case '\n':
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = scan_line_feed(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token));
                break;

            case ':':
                read_character();
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
                this->token->tag = Token::Colon;
                break;

            case ';':
                read_character();
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
                this->token->tag = Token::Semicolon;
                break;

            case '0':
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                {
                    auto r = Region::create_from_page(_rp);
                    StringBuilder& value = *StringBuilder::create(r.page);
                    this->token = scan_numeric_literal(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token), value);
                }
                break;

            case '@':
                read_character();
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = scan_attribute(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token));
                break;

            case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
            {
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = scan_operator(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token), c);
                break;
            }

            case '\"':
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = scan_string_literal(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token));
                break;

            case '\'':
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = scan_string_identifier(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token));
                break;

            case '`':
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = scan_fragment_literal(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token));
                break;

            case '}': case ')': case ']': case '.': case '?':
            {
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                auto punctuation_string = String::from_character(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token), c);
                this->read_character();
                this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
                this->token->tag = Token::Punctuation;
                this->token->punctuation = PunctuationToken { .sign = *punctuation_string };
                break;
            }

            case '{': case '(': case '[': case ',':
            {
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                auto punctuation_string = String::from_character(this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token), c);
                this->read_character();
                this->skip_whitespace(true);
                this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
                this->token->tag = Token::Punctuation;
                this->token->punctuation = PunctuationToken { .sign = *punctuation_string };
                break;
            }

            default:
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
                this->token->tag = Token::Invalid;
                break;
        }
    }

    void empty() {
            if (this->token != nullptr)
                Page::get(this->token)->clear();
            this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
            this->token->tag = Token::Empty;
    }

    Token* scan_line_feed(Page* _rp) {
        while (true) {
            read_character();
            skip_whitespace(false);
            if (this->character == nullptr) {
                if (this->token != nullptr)
                    Page::get(this->token)->clear();
                this->token = new (alignof(Token), this->token == nullptr ? Page::get(this)->allocate_exclusive_page() : Page::get(this->token)) Token();
                this->token->tag = Token::Invalid;
                return this->token;
            }

            if (*this->character == '\n')
                continue;

            auto token = new (alignof(Token), _rp) Token();
            token->tag = Token::LineFeed;
        }
    }

    Token* scan_identifier(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& name = *StringBuilder::create(r.page);
        while (true) {
            if (character == nullptr) {
                if (name.get_length() == 0) {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Invalid;
                    return token;
                }
                else {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Identifier;
                    token->identifier = IdentifierToken { .name = *name.to_string(_rp) };
                    return token;
                }
            }
            else
            {
                char c = *character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
                {
                    name.append_character(c);
                    this->read_character();
                }
                else
                {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Identifier;
                    token->identifier = IdentifierToken { .name = *name.to_string(_rp) };
                    return token;
                }
            }
        }
    }

    Token* scan_attribute(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& name = *StringBuilder::create(r.page);
        while (true) {
            if (character == nullptr) {
                if (name.get_length() == 0) {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Invalid;
                    return token;
                }
                else {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Attribute;
                    token->attribute = AttributeToken { .name = *name.to_string(_rp) };
                    return token;
                }
            }
            else
            {
                char c = *character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
                {
                    name.append_character(c);
                    this->read_character();
                }
                else
                {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Attribute;
                    token->attribute = AttributeToken { .name = *name.to_string(_rp) };
                }
            }
        }
    }

    Token* scan_operator(Page* _rp, char c) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& operation = *StringBuilder::create(r.page);
        operation.append_character(c);
        while (true) {
            read_character();
            if (character == nullptr) {
                if (operation.get_length() == 0) {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Invalid;
                    return token;
                }
                else {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Identifier;
                    token->identifier = IdentifierToken { .name = *operation.to_string(_rp) };
                    return token;
                }
            }

            auto c = *character;
            switch (c)
            {
                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                    operation.append_character(c);
                    break;

                default:{
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Identifier;
                    token->identifier = IdentifierToken { .name = *operation.to_string(_rp) };
                }
            }
        }
    }

    Token* scan_string_literal(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();
            if (character == nullptr) {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Invalid;
                return token;
            }

            auto c = *character;
            switch (c)
            {
                case '\"':
                    {
                        read_character();
                        auto token = new (alignof(Token), _rp) Token();
                        token->tag = Token::Literal;
                        token->literal = LiteralToken {
                            .tag = LiteralToken::String,
                            .string = StringToken {
                                .value = *value.to_string(_rp),
                            }
                        };
                        return token;
                    }

                case '\\': 
                    {
                        read_character();
                        if (character == nullptr) {
                            auto token = new (alignof(Token), _rp) Token();
                            token->tag = Token::Invalid;
                            return token;
                        }
                        auto c2 = *character;
                        switch (c2) {
                            case '\"': case '\\': case '\'':
                                value.append_character('\\');
                                value.append_character(c2);
                                break;
                            case 'n':
                                value.append_character('\\');
                                value.append_character('n');
                                break;
                            case 'r':
                                value.append_character('\\');
                                value.append_character('r');
                                break;
                            case 't':
                                value.append_character('\\');
                                value.append_character('t');
                                break;
                            case '0':
                                value.append_character('\\');
                                value.append_character('0');
                                break;
                            default:{
                                auto token = new (alignof(Token), _rp) Token();
                                token->tag = Token::Invalid;
                                return token;
                            }
                            
                        }
                    }
                    break;

                default:
                    value.append_character(c);
                    break;
            }
        }
    }

    Token* scan_string_identifier(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();
            if (character == nullptr){
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Invalid;
                return token;
            }
            auto c = *character;
            switch (c) {
                case '\'':
                    read_character();
                    {
                        auto token = new (alignof(Token), _rp) Token();
                        token->tag = Token::Identifier;
                        token->identifier = IdentifierToken { .name = *value.to_string(_rp) };
                        return token;
                    }
                default:
                    value.append_character(c);
                    break;
            }
        }
    }

    Token* scan_fragment_literal(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();
            if (character == nullptr){
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Invalid;
                return token;
            }
            auto c = *character;
            switch (c) {
                case '`':
                    read_character();
                    {
                        auto token = new (alignof(Token), _rp) Token();
                        token->tag = Token::Literal;
                        token->literal = LiteralToken {
                            .tag = LiteralToken::Fragment,
                            .fragment = FragmentToken {
                                .value = *value.to_string(_rp),
                            }
                        };
                        return token;
                    }
                case '\\':
                    read_character();
                    if (character == nullptr){
                        auto token = new (alignof(Token), _rp) Token();
                        token->tag = Token::Invalid;
                        return token;
                    }
                    {
                        auto c2 = *character;
                        switch (c2)
                        {
                            case '`':
                                value.append_character(c2);
                                break;
                            default:
                                value.append_character('\\');
                                value.append_character(c2);
                                break;
                        }
                        break;
                    }

                default:
                    value.append_character(c);
                    break;
            }
        }
    }

    Token* scan_numeric_literal(Page* _rp, StringBuilder& value) {
        read_character();
        if (character == nullptr) {
            auto token = new (alignof(Token), _rp) Token();
            token->tag = Token::Literal;
            token->literal = LiteralToken {
                .tag = LiteralToken::String,
                .integer = IntegerToken {
                    .value = *value.to_string(_rp),
                }
            };
            return token;
        }

        auto c = *character;

        if ((c >= '0') && (c <= '9')) {
            value.append_character(c);
            return scan_integer_literal(_rp, c);
        }

        switch (c) {
            case '.':
                value.append_character(c);
                return scan_fraction(_rp, value);

            case 'E':
            case 'e':
                value.append_character(c);
                return scan_exponent(_rp, value);

            case 'x':
                return scan_hex_literal(_rp);

            case 'B':
                return scan_boolean_literal(_rp);

            default:
            {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::String,
                    .integer = IntegerToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }
        }
    }

    Token* scan_integer_literal(Page* _rp, char c) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        value.append_character(c);
        while (true) {
            read_character();
            if (character == nullptr) {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::String,
                    .integer = IntegerToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }

            auto c = *character;

            if ((c >= '0') && (c <= '9')) {
                value.append_character(c);
                continue;
            }

            switch (c) {
                case '.':
                    value.append_character(c);
                    return scan_fraction(_rp, value);

                case 'E':
                case 'e':
                    value.append_character(c);
                    return scan_exponent(_rp, value);

                default:
                {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Literal;
                    token->literal = LiteralToken {
                        .tag = LiteralToken::Integer,
                        .integer = IntegerToken {
                            .value = *value.to_string(_rp),
                        }
                    };
                    return token;
                }
            }
        }
    }

    Token* scan_fraction(Page* _rp, StringBuilder& value) {
        while (true) {
            read_character();

            if (character == nullptr) {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::FloatingPoint,
                    .floatingPoint = FloatingPointToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }

            auto c = *character;

            if ((c >= '0') && (c <= '9')) {
                value.append_character(c);
                continue;
            }

            switch (c)
            {
                case 'E':
                case 'e':
                    value.append_character(c);
                    return scan_exponent(_rp, value);

                default:
                {
                    auto token = new (alignof(Token), _rp) Token();
                    token->tag = Token::Literal;
                    token->literal = LiteralToken {
                        .tag = LiteralToken::FloatingPoint,
                        .floatingPoint = FloatingPointToken {
                            .value = *value.to_string(_rp),
                        }
                    };
                    return token;
                }
            }
        }
    }

    Token* scan_exponent(Page* _rp, StringBuilder& value) {
        while (true) {
            read_character();
            if (character == nullptr) {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::FloatingPoint,
                    .floatingPoint = FloatingPointToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }

            auto c = *character;

            if ((c >= '0') && (c <= '9'))
            {
                value.append_character(c);
                continue;
            }

            {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::FloatingPoint,
                    .floatingPoint = FloatingPointToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }
        }
    }

    Token* scan_hex_literal(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();

            if (character == nullptr) {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::Hex,
                    .hex = HexToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }

            auto c = *character;

            if (((c >= '0') && (c <= '9')) || (c >= 'A') && (c <= 'F') || ((c >= 'a') && (c <= 'f'))) {
                value.append_character(c);
                continue;
            }

            {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::Hex,
                    .hex = HexToken {
                        .value = *value.to_string(_rp),
                    }
                };
                return token;
            }
        }
    }

    Token* scan_boolean_literal(Page* _rp) {
        while (true) {
            read_character();
            if (character == nullptr){
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Invalid;
                return token;
            }

            auto c = *character;

            if (c != '0' && c != '1') {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Invalid;
                return token;
            }

            {
                auto token = new (alignof(Token), _rp) Token();
                token->tag = Token::Literal;
                token->literal = LiteralToken {
                    .tag = LiteralToken::Boolean,
                    .boolean = BooleanToken {
                        .value = c == '1',
                    }
                };
                return token;
            }
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
};

}