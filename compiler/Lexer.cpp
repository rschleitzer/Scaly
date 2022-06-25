namespace scaly {
namespace compiler {
    
using namespace scaly::containers;


struct EmptyToken {};
struct InvalidToken {};

struct IdentifierToken {
    IdentifierToken(String name) : name(name) {}
    String name;
};

struct AttributeToken {
    AttributeToken(String name) : name(name) {}
    String name;
};

struct PunctuationToken {
    PunctuationToken(String sign) : sign(sign) {}
    String sign;
};

struct StringToken {
    StringToken(String value) : value(value) {}
    String value;
};

struct FragmentToken {
    FragmentToken(String value) : value(value) {}
    String value;
};

struct IntegerToken {
    IntegerToken(String value) : value(value) {}
    String value;
};

struct BooleanToken {
    BooleanToken(bool value) : value(value) {}
    bool value;
};

struct FloatingPointToken {
    FloatingPointToken(String value) : value(value) {}
    String value;
};

struct HexToken {
    HexToken(String value) : value(value) {}
    String value;
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
    Token(ColonToken colon) : _tag(Colon) {}
    enum {
        Empty,
        Invalid,
        Identifier,
        Attribute,
        Punctuation,
        Literal,
        LineFeed,
        Colon,
    } _tag;
    union {
        EmptyToken _Empty;
        InvalidToken _Invalid;
        IdentifierToken _Identifier;
        AttributeToken _Attribute;
        PunctuationToken _Punctuation;
        LiteralToken _Literal;
        LineFeedToken _LineFeed;
        ColonToken _Colon;
    };
};

struct Lexer : Object {
    Token* token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    Lexer(String deck) {
        token = new(alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(EmptyToken());
        token->_tag = Token::Empty;
        character = nullptr;
        chars = StringIterator::create(deck);
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

    void advance(Region& _pr) {
        Region _r = Region::create(_pr);
        skip_whitespace(false);
        this->previous_position = this->position;

        if (this->character == nullptr)
            return;

        auto c = *character;

        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
            if (this->token != nullptr)
                Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
            this->token = scan_identifier(Page::get(this)->allocate_exclusive_page());
            return;
        }

        if ((c >= '1') && (c <= '9')) {
            if (this->token != nullptr)
                Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
            this->token = scan_integer_literal(Page::get(this)->allocate_exclusive_page(), c);
            return;
        }

        switch (c) {
            case '\n':
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = scan_line_feed(Page::get(this)->allocate_exclusive_page());
                break;

            case ':':
                read_character();
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(ColonToken());
                break;

            case '0':
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                {
                    auto _r_1 = Region::create(_r);
                    StringBuilder& value = *new (alignof(StringBuilder), _r_1.page) StringBuilder();
                    this->token = scan_numeric_literal(Page::get(this)->allocate_exclusive_page(), value);
                }
                break;

            case '@':
                read_character();
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = scan_attribute(Page::get(this)->allocate_exclusive_page());
                break;

            case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
            {
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = scan_operator(Page::get(this)->allocate_exclusive_page(), c);
                break;
            }

            case '\"':
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = scan_string_literal(Page::get(this)->allocate_exclusive_page());
                break;

            case '\'':
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = scan_string_identifier(Page::get(this)->allocate_exclusive_page());
                break;

            case '`':
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = scan_fragment_literal(Page::get(this)->allocate_exclusive_page());
                break;

            case '}': case ')': case ']': case '.': case '?':
            {
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                auto punctuation_string = String(Page::get(this)->allocate_exclusive_page(), c);
                this->read_character();
                this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(PunctuationToken(punctuation_string));
                break;
            }

            case '{': case '(': case '[': case ',':
            {
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                auto punctuation_string = String(Page::get(this)->allocate_exclusive_page(), c);
                this->read_character();
                this->skip_whitespace(true);
                this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(PunctuationToken(punctuation_string));
                break;
            }

            default:
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(InvalidToken());
                break;
        }
    }

    void empty() {
        if (this->token != nullptr)
            Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
        this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(EmptyToken());
    }

    Token* scan_line_feed(Page* _rp) {
        while (true) {
            read_character();
            skip_whitespace(false);
            if (this->character == nullptr) {
                auto token = new (alignof(Token), _rp) Token(LineFeedToken());
                return this->token;
            }

            if (*this->character == '\n')
                continue;

            auto token = new (alignof(Token), _rp) Token(LineFeedToken());
            token->_tag = Token::LineFeed;
            return this->token;
        }
    }

    Token* scan_identifier(Page* _rp) {
        auto r = Region(_rp);
        StringBuilder& name = *new(alignof(StringBuilder), r.page) StringBuilder();
        while (true) {
            if (character == nullptr) {
                if (name.get_length() == 0)
                    return new (alignof(Token), _rp) Token(InvalidToken());
                else
                    return new (alignof(Token), _rp) Token(IdentifierToken(name.to_string(_rp)));
            }
            else {
                char c = *character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
                {
                    name.append_character(c);
                    this->read_character();
                }
                else
                {
                    auto token = new (alignof(Token), _rp) Token(IdentifierToken(name.to_string(_rp)));
                    return token;
                }
            }
        }
    }

    Token* scan_attribute(Page* _rp) {
        auto r = Region(_rp);
        StringBuilder& name = *new(alignof(StringBuilder), r.page) StringBuilder();
        while (true) {
            if (character == nullptr) {
                if (name.get_length() == 0)
                    return new (alignof(Token), _rp) Token(InvalidToken());
                else
                    return new (alignof(Token), _rp) Token(AttributeToken(name.to_string(_rp)));
            }
            else {
                char c = *character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
                {
                    name.append_character(c);
                    this->read_character();
                }
                else
                {
                    auto token = new (alignof(Token), _rp) Token(AttributeToken(name.to_string(_rp)));
                    return token;
                }
            }
        }
    }

    Token* scan_operator(Page* _rp, char c) {
        auto r = Region(_rp);
        StringBuilder& operation = *new(alignof(StringBuilder), r.page) StringBuilder();
        operation.append_character(c);
        while (true) {
            read_character();
            if (character == nullptr) {
                if (operation.get_length() == 0)
                    return new (alignof(Token), _rp) Token(InvalidToken());
                else
                    return new (alignof(Token), _rp) Token(IdentifierToken(operation.to_string(_rp)));
            }

            auto c = *character;
            switch (c)
            {
                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                    operation.append_character(c);
                    break;

                default:{
                    auto token = new (alignof(Token), _rp) Token(IdentifierToken(operation.to_string(_rp)));
                    return token;
                }
            }
        }
    }

    Token* scan_string_literal(Page* _rp) {
        auto r = Region(_rp);
        StringBuilder& value = *new(alignof(StringBuilder), r.page) StringBuilder();
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(InvalidToken());

            auto c = *character;
            switch (c)
            {
                case '\"':
                    read_character();
                    return new (alignof(Token), _rp) Token(LiteralToken(StringToken(value.to_string(_rp))));

                case '\\': 
                    {
                        read_character();
                        if (character == nullptr)
                            return new (alignof(Token), _rp) Token(InvalidToken());

                        auto c2 = *character;
                        switch (c2) {
                            case '\"': case '\\': case '\'':
                                value.append_character(c2);
                                break;
                            case 'n':
                                value.append_character('\n');
                                break;
                            case 'r':
                                value.append_character('\r');
                                break;
                            case 't':
                                value.append_character('\t');
                                break;
                            case '0':
                                value.append_character('\0');
                                break;
                            default:
                                return new (alignof(Token), _rp) Token(InvalidToken());                            
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
        auto r = Region(_rp);
        StringBuilder& value = *new(alignof(StringBuilder), r.page) StringBuilder();
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(InvalidToken());

            auto c = *character;
            switch (c) {
                case '\'':
                    read_character();
                    {
                        auto token = new (alignof(Token), _rp) Token(IdentifierToken(value.to_string(_rp)));
                        return token;
                    }
                default:
                    value.append_character(c);
                    break;
            }
        }
    }

    Token* scan_fragment_literal(Page* _rp) {
        auto r = Region(_rp);
        StringBuilder& value = *new(alignof(StringBuilder), r.page) StringBuilder();
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(InvalidToken());

            auto c = *character;
            switch (c)
            {
                case '`':
                    read_character();
                    return new (alignof(Token), _rp) Token(LiteralToken(FragmentToken(value.to_string(_rp))));

                case '\\': 
                    {
                        read_character();
                        if (character == nullptr)
                            return new (alignof(Token), _rp) Token(InvalidToken());

                        auto c2 = *character;
                        switch (c2) {
                            case '`': case '\\': case '\'':
                                value.append_character(c2);
                                break;
                            case 'n':
                                value.append_character('\n');
                                break;
                            case 'r':
                                value.append_character('\r');
                                break;
                            case 't':
                                value.append_character('\t');
                                break;
                            case '0':
                                value.append_character('\0');
                                break;
                            default:
                                return new (alignof(Token), _rp) Token(InvalidToken());                            
                        }
                    }
                    break;

                default:
                    value.append_character(c);
                    break;
            }
        }
    }

    Token* scan_numeric_literal(Page* _rp, StringBuilder& value) {
        read_character();
        if (character == nullptr)
            return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(value.to_string(_rp))));

        auto c = *character;

        if ((c >= '0') && (c <= '9')) {
            value.append_character(c);
            return scan_integer_literal(_rp, c);
        }

        switch (c) {
            case '.':
                if (value.get_length() == 0)
                    value.append_character('0');
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
                if (value.get_length() == 0)
                    value.append_character('0');
                return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(value.to_string(_rp))));
        }
    }

    Token* scan_integer_literal(Page* _rp, char c) {
        auto r = Region(_rp);
        StringBuilder& value = *new(alignof(StringBuilder), r.page) StringBuilder();
        value.append_character(c);
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(value.to_string(_rp))));

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
                    return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(value.to_string(_rp))));
            }
        }
    }

    Token* scan_fraction(Page* _rp, StringBuilder& value) {
        while (true) {
            read_character();

            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(value.to_string(_rp))));

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
                    return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(value.to_string(_rp))));
            }
        }
    }

    Token* scan_exponent(Page* _rp, StringBuilder& value) {
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(value.to_string(_rp))));

            auto c = *character;

            if ((c >= '0') && (c <= '9')) {
                value.append_character(c);
                continue;
            }

            return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(value.to_string(_rp))));
        }
    }

    Token* scan_hex_literal(Page* _rp) {
        auto r = Region(_rp);
        StringBuilder& value = *new(alignof(StringBuilder), r.page) StringBuilder();
        while (true) {
            read_character();

            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(HexToken(value.to_string(_rp))));

            auto c = *character;

            if (((c >= '0') && (c <= '9')) || (c >= 'A') && (c <= 'F') || ((c >= 'a') && (c <= 'f'))) {
                value.append_character(c);
                continue;
            }

            return new (alignof(Token), _rp) Token(LiteralToken(HexToken(value.to_string(_rp))));
        }
    }

    Token* scan_boolean_literal(Page* _rp) {
        read_character();
        if (character == nullptr)
            return new (alignof(Token), _rp) Token(InvalidToken());

        auto c = *character;

        if (c != '0' && c != '1') {
            return new (alignof(Token), _rp) Token(InvalidToken());
        }
        else {
            read_character();
            return new (alignof(Token), _rp) Token(LiteralToken(BooleanToken(c == '1')));
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

    bool parse_keyword(Region& _pr, Page* _rp, const String& fixed_string) {
        Region _r = Region::create(_pr);
        if (token->_tag == Token::Empty)
            advance(_r);

        switch (token->_tag) {
            case Token::Identifier:
            {
                auto right_keyword = (token->_Identifier.name.equals(fixed_string));
                if (right_keyword)
                    empty();

                return right_keyword;
            }

            default:
                return false;
        }
    }

    String* parse_identifier(Region& _pr, Page* _rp, HashSet<String>& keywords) {
        Region _r = Region::create(_pr);
        if (token->_tag == Token::Empty)
            advance(_r);

        switch (token->_tag) {
            case Token::Identifier:
            {
                if (keywords.contains(token->_Identifier.name))
                    return nullptr;
                auto ret = new(alignof(String), _rp) String(_rp, token->_Identifier.name);
                empty();
                return ret;
            }
            default:
                return nullptr;
        }
    }

    String* parse_attribute(Region& _pr, Page* _rp) {
        Region _r = Region::create(_pr);
        if (token->_tag == Token::Empty)
            advance(_r);

        switch (token->_tag) {
            case Token::Attribute:
            {
                auto ret = new(alignof(String), _rp) String(_rp, token->_Attribute.name);
                empty();
                return ret;
            }
            default:
                return nullptr;
        }
    }

    bool parse_punctuation(Region& _pr, Page* _rp, const String& fixed_string) {
        Region _r = Region::create(_pr);
        if (token->_tag == Token::Empty)
            advance(_r);

        switch (token->_tag) {
            case Token::Punctuation:
            {
                bool ret = (token->_Punctuation.sign.equals(fixed_string));
                if (ret)
                    empty();

                return ret;
            }
            default:
                return false;
        }
    }

    LiteralToken* parse_literal(Region& _pr, Page* _rp) {
        Region _r = Region::create(_pr);
        if (token->_tag == Token::Empty)
            advance(_r);

        switch (token->_tag)
        {
            case Token::Literal:
                switch (token->_Literal._tag)
                {
                    case LiteralToken::String:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(StringToken(String(_rp, token->_Literal._String.value)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Integer:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(IntegerToken(String(_rp, token->_Literal._Integer.value)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::FloatingPoint:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(FloatingPointToken(String(_rp, token->_Literal._FloatingPoint.value)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Hex:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(HexToken(String(_rp, token->_Literal._Hex.value)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Boolean:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(BooleanToken(token->_Literal._Boolean.value));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Fragment:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(FragmentToken(String(_rp, token->_Literal._Fragment.value)));
                        empty();
                        return ret;
                    }

                    default:
                        return nullptr;
                }
            default:
                return nullptr;
        }
    }

    bool parse_colon(Region&_pr, Page* _rp) {
        Region _r = Region::create(_pr);
        if (token->_tag == Token::Empty)
            advance(_r);

        switch (token->_tag)
        {
            case Token::Colon: case Token::LineFeed:
                empty();
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