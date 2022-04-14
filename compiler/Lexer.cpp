using namespace scaly::containers;

namespace scaly::compiler {

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

struct Token {
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
        AttributeToken attribute;
        PunctuationToken punctuation;
        LiteralToken literal;
        LineFeedToken lineFeed;
        ColonToken colon;
        SemicolonToken semicolon;
    };
};

struct Lexer {
    Token token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    Lexer create(String deck) {
        auto lexer = Lexer {
            .token = Token {
                .tag = Token::Empty,
            },
            .character = nullptr,
            .chars = StringIterator::create(deck),
            .previous_position = 0,
            .position = 0,
        };

        lexer.read_character();
        skip_whitespace(true);

        return lexer;
    }

    void read_character() {
        this->character = chars.next();
        if (this->character != nullptr)
            this->position++;        
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
                                        switch (*this->character) {
                                            case '*':
                                                read_character();
                                                handle_multi_line_comment();
                                                break;
                                            default:
                                                read_character();
                                                handle_single_line_comment();
                                                break;
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
                    switch (*this->character)
                    {
                        case '\t':
                            read_character();
                            continue;
                        case '\r':
                            read_character();
                            continue;
                        case '\n':
                            read_character();
                            return;
                        default:
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