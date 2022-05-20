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
    LiteralToken(StringToken stringToken) : tag(String), stringToken(stringToken) {}
    LiteralToken(FragmentToken fragmentToken) : tag(Fragment), fragmentToken(fragmentToken) {}
    LiteralToken(IntegerToken integerToken) : tag(Integer), integerToken(integerToken) {}
    LiteralToken(BooleanToken booleanToken) : tag(Boolean), booleanToken(booleanToken) {}
    LiteralToken(FloatingPointToken floatingPointToken) : tag(FloatingPoint), floatingPointToken(floatingPointToken) {}
    LiteralToken(HexToken hexToken) : tag(Hex), hexToken(hexToken) {}
    enum {
        String,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } tag;
    union {
        StringToken stringToken;
        FragmentToken fragmentToken;
        IntegerToken integerToken;
        BooleanToken booleanToken;
        FloatingPointToken floatingPointToken;
        HexToken hexToken;

    };
};

struct LineFeedToken {};
struct ColonToken {};

struct Token : Object {
    Token(EmptyToken emptyToken) : tag(Empty) {}
    Token(InvalidToken invalidToken) : tag(Invalid) {}
    Token(IdentifierToken identifierToken) : tag(Identifier), identifierToken(identifierToken) {}
    Token(AttributeToken attributeToken) : tag(Attribute), attributeToken(attributeToken) {}
    Token(PunctuationToken punctuationToken) : tag(Colon), punctuationToken(punctuationToken) {}
    Token(LiteralToken literalToken) : tag(Literal), literalToken(literalToken) {}
    Token(LineFeedToken lineFeedToken) : tag(LineFeed) {}
    Token(ColonToken colonToken) : tag(Colon) {}
    enum {
        Empty,
        Invalid,
        Identifier,
        Attribute,
        Punctuation,
        Literal,
        LineFeed,
        Colon,
    } tag;
    union {
        EmptyToken emptyToken;
        InvalidToken invalidToken;
        IdentifierToken identifierToken;
        AttributeToken attributeToken;
        PunctuationToken punctuationToken;
        LiteralToken literalToken;
        LineFeedToken lineFeedToken;
        ColonToken colonToken;
    };
};

struct Lexer : Object {
    Token* token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    Lexer(String& deck) {
        token = new(alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(EmptyToken());
        token->tag = Token::Empty;
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
                    StringBuilder& value = *StringBuilder::create(_r_1.page);
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
                auto punctuation_string = String::from_character(Page::get(this)->allocate_exclusive_page(), c);
                this->read_character();
                this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(PunctuationToken(*punctuation_string));
                break;
            }

            case '{': case '(': case '[': case ',':
            {
                if (this->token != nullptr)
                    Page::get(this)->deallocate_exclusive_page(Page::get(this->token));
                auto punctuation_string = String::from_character(Page::get(this)->allocate_exclusive_page(), c);
                this->read_character();
                this->skip_whitespace(true);
                this->token = new (alignof(Token), Page::get(this)->allocate_exclusive_page()) Token(PunctuationToken(*punctuation_string));
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
            token->tag = Token::LineFeed;
            return this->token;
        }
    }

    Token* scan_identifier(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& name = *StringBuilder::create(r.page);
        while (true) {
            if (character == nullptr) {
                if (name.get_length() == 0)
                    return new (alignof(Token), _rp) Token(InvalidToken());
                else
                    return new (alignof(Token), _rp) Token(IdentifierToken(*name.to_string(_rp)));
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
                    auto token = new (alignof(Token), _rp) Token(IdentifierToken(*name.to_string(_rp)));
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
                if (name.get_length() == 0)
                    return new (alignof(Token), _rp) Token(InvalidToken());
                else
                    return new (alignof(Token), _rp) Token(AttributeToken(*name.to_string(_rp)));
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
                    auto token = new (alignof(Token), _rp) Token(AttributeToken(*name.to_string(_rp)));
                    return token;
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
                if (operation.get_length() == 0)
                    return new (alignof(Token), _rp) Token(InvalidToken());
                else
                    return new (alignof(Token), _rp) Token(IdentifierToken(*operation.to_string(_rp)));
            }

            auto c = *character;
            switch (c)
            {
                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                    operation.append_character(c);
                    break;

                default:{
                    auto token = new (alignof(Token), _rp) Token(IdentifierToken(*operation.to_string(_rp)));
                    return token;
                }
            }
        }
    }

    Token* scan_string_literal(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(InvalidToken());

            auto c = *character;
            switch (c)
            {
                case '\"':
                    read_character();
                    return new (alignof(Token), _rp) Token(LiteralToken(StringToken(*value.to_string(_rp))));

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
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(InvalidToken());

            auto c = *character;
            switch (c) {
                case '\'':
                    read_character();
                    {
                        auto token = new (alignof(Token), _rp) Token(IdentifierToken(*value.to_string(_rp)));
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
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(InvalidToken());

            auto c = *character;
            switch (c)
            {
                case '`':
                    read_character();
                    return new (alignof(Token), _rp) Token(LiteralToken(FragmentToken(*value.to_string(_rp))));

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
            auto token = new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(*value.to_string(_rp))));

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
                return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(*value.to_string(_rp))));
        }
    }

    Token* scan_integer_literal(Page* _rp, char c) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        value.append_character(c);
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(*value.to_string(_rp))));

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
                    return new (alignof(Token), _rp) Token(LiteralToken(IntegerToken(*value.to_string(_rp))));
            }
        }
    }

    Token* scan_fraction(Page* _rp, StringBuilder& value) {
        while (true) {
            read_character();

            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(*value.to_string(_rp))));

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
                    return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(*value.to_string(_rp))));
            }
        }
    }

    Token* scan_exponent(Page* _rp, StringBuilder& value) {
        while (true) {
            read_character();
            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(*value.to_string(_rp))));

            auto c = *character;

            if ((c >= '0') && (c <= '9')) {
                value.append_character(c);
                continue;
            }

            return new (alignof(Token), _rp) Token(LiteralToken(FloatingPointToken(*value.to_string(_rp))));
        }
    }

    Token* scan_hex_literal(Page* _rp) {
        auto r = Region::create_from_page(_rp);
        StringBuilder& value = *StringBuilder::create(r.page);
        while (true) {
            read_character();

            if (character == nullptr)
                return new (alignof(Token), _rp) Token(LiteralToken(HexToken(*value.to_string(_rp))));

            auto c = *character;

            if (((c >= '0') && (c <= '9')) || (c >= 'A') && (c <= 'F') || ((c >= 'a') && (c <= 'f'))) {
                value.append_character(c);
                continue;
            }

            return new (alignof(Token), _rp) Token(LiteralToken(HexToken(*value.to_string(_rp))));
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

    bool parse_keyword(Region& _pr, Page* _rp, String& fixed_string) {
        Region _r = Region::create(_pr);
        if (token->tag == Token::Empty)
            advance(_r);

        switch (token->tag) {
            case Token::Identifier:
            {
                auto right_keyword = (token->identifierToken.name.equals(fixed_string));
                if (right_keyword)
                    empty();

                return right_keyword;
            }

            default:
                return false;
        }
    }

    String* parse_identifier(Region& _pr, Page* _rp, HashSetBuilder<String>& keywords) {
        Region _r = Region::create(_pr);
        if (token->tag == Token::Empty)
            advance(_r);

        switch (token->tag) {
            case Token::Identifier:
            {
                if (keywords.contains(token->identifierToken.name))
                    return nullptr;
                auto ret = token->identifierToken.name.copy(_rp);
                empty();
                return ret;
            }
            default:
                return nullptr;
        }
    }

    String* parse_attribute(Region& _pr, Page* _rp) {
        Region _r = Region::create(_pr);
        if (token->tag == Token::Empty)
            advance(_r);

        switch (token->tag) {
            case Token::Attribute:
            {
                auto ret = token->attributeToken.name.copy(_rp);
                empty();
                return ret;
            }
            default:
                return nullptr;
        }
    }

    bool parse_punctuation(Region& _pr, Page* _rp, String& fixed_string) {
        Region _r = Region::create(_pr);
        if (token->tag == Token::Empty)
            advance(_r);

        switch (token->tag) {
            case Token::Punctuation:
            {
                bool ret = (token->punctuationToken.sign.equals(fixed_string));
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
        if (token->tag == Token::Empty)
            advance(_r);

        switch (token->tag)
        {
            case Token::Literal:
                switch (token->literalToken.tag)
                {
                    case LiteralToken::String:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(StringToken(*token->literalToken.stringToken.value.copy(_rp)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Integer:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(IntegerToken(*token->literalToken.integerToken.value.copy(_rp)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::FloatingPoint:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(FloatingPointToken(*token->literalToken.floatingPointToken.value.copy(_rp)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Hex:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(HexToken(*token->literalToken.hexToken.value.copy(_rp)));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Boolean:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(BooleanToken(token->literalToken.booleanToken.value));
                        empty();
                        return ret;
                    }

                    case LiteralToken::Fragment:
                    {
                        auto ret = new (alignof(LiteralToken), _rp) LiteralToken(FragmentToken(*token->literalToken.fragmentToken.value.copy(_rp)));
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
        if (token->tag == Token::Empty)
            advance(_r);

        switch (token->tag)
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