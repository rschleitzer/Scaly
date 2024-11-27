#include "../../scaly.h"
namespace scaly {
namespace json {
using namespace scaly::containers;


IdentifierToken::IdentifierToken(Vector<char> name) : name(name) {}

AttributeToken::AttributeToken(Vector<char> name) : name(name) {}

PunctuationToken::PunctuationToken(char sign) : sign(sign) {}

StringToken::StringToken(Vector<char> value) : value(value) {}

CharacterToken::CharacterToken(Vector<char> value) : value(value) {}

FragmentToken::FragmentToken(Vector<char> value) : value(value) {}

IntegerToken::IntegerToken(Vector<char> value) : value(value) {}

BooleanToken::BooleanToken(bool value) : value(value) {}

FloatingPointToken::FloatingPointToken(Vector<char> value) : value(value) {}

HexToken::HexToken(Vector<char> value) : value(value) {}
LiteralToken::LiteralToken(struct StringToken _String) : _tag(String), _String(_String) {}

LiteralToken::LiteralToken(struct CharacterToken _Character) : _tag(Character), _Character(_Character) {}

LiteralToken::LiteralToken(struct FragmentToken _Fragment) : _tag(Fragment), _Fragment(_Fragment) {}

LiteralToken::LiteralToken(struct IntegerToken _Integer) : _tag(Integer), _Integer(_Integer) {}

LiteralToken::LiteralToken(struct BooleanToken _Boolean) : _tag(Boolean), _Boolean(_Boolean) {}

LiteralToken::LiteralToken(struct FloatingPointToken _FloatingPoint) : _tag(FloatingPoint), _FloatingPoint(_FloatingPoint) {}

LiteralToken::LiteralToken(struct HexToken _Hex) : _tag(Hex), _Hex(_Hex) {}

Token::Token(struct EmptyToken _Empty) : _tag(Empty), _Empty(_Empty) {}

Token::Token(struct InvalidToken _Invalid) : _tag(Invalid), _Invalid(_Invalid) {}

Token::Token(struct IdentifierToken _Identifier) : _tag(Identifier), _Identifier(_Identifier) {}

Token::Token(struct AttributeToken _Attribute) : _tag(Attribute), _Attribute(_Attribute) {}

Token::Token(struct PunctuationToken _Punctuation) : _tag(Punctuation), _Punctuation(_Punctuation) {}

Token::Token(struct LiteralToken _Literal) : _tag(Literal), _Literal(_Literal) {}

Token::Token(struct ColonToken _Colon) : _tag(Colon), _Colon(_Colon) {}


Lexer::Lexer(Token token, char* character, StringIterator chars, size_t previous_position, size_t position) : token(token), character(character), chars(chars), previous_position(previous_position), position(position) {}

Lexer::Lexer() : token(EmptyToken()), character(nullptr), chars(StringIterator(String())), previous_position(0), position(0) {}

Lexer::Lexer(String deck) : Lexer(){
    chars = deck.get_iterator();
    read_character();
    skip_whitespace(true);
}

void Lexer::read_character() {
    character = chars.next();
    if (character != nullptr) 
        position = position+1;
}

void Lexer::advance() {
    previous_position = position;
    if (character == nullptr) 
        return;
    const auto c = *character;
    if (((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))) {
        token = scan_identifier();
        skip_whitespace(false);
        return;
    };
    if ((c>='1')&&(c<='9')) {
        token = scan_integer_literal(character, 0);
        skip_whitespace(false);
        return;
    };
    switch (c)
    {
        case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '~': case '<': case '>': 
            {
                token = scan_operator();
                skip_whitespace(false);
                return;
            };
            break;
        case '}': case ')': case ']': case '.': case '?': case '!': case '$': case '#': case '^': 
            {
                token = Token(PunctuationToken(*character));
                read_character();
                skip_whitespace(false);
                return;
            };
            break;
        case '{': case '(': case '[': case ',': 
            {
                token = Token(PunctuationToken(*character));
                read_character();
                skip_whitespace(true);
                return;
            };
            break;
        case '\n': 
            token = scan_line_feed();
            break;
        case ':': 
            {
                read_character();
                token = Token(ColonToken());
            };
            break;
        case '0': 
            token = scan_numeric_literal();
            break;
        case '@': 
            {
                read_character();
                token = scan_attribute();
            };
            break;
        case '\"': 
            token = scan_string_literal();
            break;
        case '\'': 
            token = scan_string_identifier();
            break;
        case '`': 
            token = scan_fragment_literal();
            break;
        default:
            token = Token(InvalidToken());
    };
    skip_whitespace(false);
}

void Lexer::empty() {
    token = Token(EmptyToken());
}

Token Lexer::scan_line_feed() {
    while (true) {
        read_character();
        skip_whitespace(false);
        if (character == nullptr) 
            return Token(ColonToken());
        if (*character == '\n') 
            continue;;
        return Token(ColonToken());
    };
}

Token Lexer::scan_identifier() {
    const auto start = character;
    size_t length = 0;
    while (true) {
        if (character == nullptr) {
            if (length == 0) {
                return Token(InvalidToken());
            }
            else {
                return Token(IdentifierToken(Vector<char>(start, length)));
            };
        }
        else {
            const auto c = *character;
            if (((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))||((c>='0')&&(c<='9'))||(c == '_')) {
                read_character();
                length = length+1;
            }
            else {
                return Token(IdentifierToken(Vector<char>(start, length)));
            };
        };
    };
}

Token Lexer::scan_attribute() {
    const auto start = character;
    size_t length = 0;
    while (true) {
        if (character == nullptr) {
            if (length == 0) {
                return Token(InvalidToken());
            }
            else {
                return Token(AttributeToken(Vector<char>(start, length)));
            };
        }
        else {
            const auto c = *character;
            if (((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))||((c>='0')&&(c<='9'))||(c == '_')) {
                read_character();
                length = length+1;
            }
            else {
                return Token(AttributeToken(Vector<char>(start, length)));
            };
        };
    };
}

Token Lexer::scan_operator() {
    const auto start = character;
    size_t length = 0;
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) {
            if (length == 1) {
                return Token(InvalidToken());
            }
            else {
                return Token(IdentifierToken(Vector<char>(start, length)));
            };
        };
        switch (*character)
        {
            case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>': 
                continue;;
                break;
            default:
                return Token(IdentifierToken(Vector<char>(start, length)));
        };
    };
}

Token Lexer::scan_string_literal() {
    const auto start = character+1;
    size_t length = 0;
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(InvalidToken());
        const auto c = *character;
        switch (c)
        {
            case '\"': 
                {
                    read_character();
                    return Token(LiteralToken(StringToken(Vector<char>(start, length-1))));
                };
                break;
            case '\\': 
                {
                    read_character();
                    length = length+1;
                    if (character == nullptr) 
                        return Token(InvalidToken());
                    switch (*character)
                    {
                        case '\"': case '\\': case '\'': case 'n': case 'r': case 't': case '0': 
                            {
                            };
                            break;
                        default:
                            return Token(InvalidToken());
                    };
                };
                break;
        };
    };
}

Token Lexer::scan_string_identifier() {
    const auto start = character+1;
    size_t length = 0;
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(InvalidToken());
        const auto c = *character;
        switch (c)
        {
            case '\'': 
                {
                    read_character();
                    return Token(LiteralToken(CharacterToken(Vector<char>(start, length-1))));
                };
                break;
            case '\\': 
                {
                    read_character();
                    length = length+1;
                    if (character == nullptr) 
                        return Token(InvalidToken());
                    switch (*character)
                    {
                        case '\"': case '\\': case '\'': case 'n': case 'r': case 't': case '0': 
                            {
                            };
                            break;
                        default:
                            return Token(InvalidToken());
                    };
                };
                break;
            default:
                break;
        };
    };
}

Token Lexer::scan_fragment_literal() {
    const auto start = character+1;
    size_t length = 0;
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(InvalidToken());
        switch (*character)
        {
            case '`': 
                {
                    read_character();
                    return Token(LiteralToken(FragmentToken(Vector<char>(start, length-1))));
                };
                break;
            case '\\': 
                {
                    read_character();
                    length = length+1;
                    if (character == nullptr) 
                        return Token(InvalidToken());
                    switch (*character)
                    {
                        case '`': case '\\': case '\'': case 'n': case 'r': case 't': case '0': 
                            break;
                            break;
                        default:
                            return Token(InvalidToken());
                    };
                };
                break;
            default:
                break;
        };
    };
}

Token Lexer::scan_numeric_literal() {
    const auto start = character;
    size_t length = 0;
    read_character();
    length = length+1;
    if (character == nullptr) 
        return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));
    const auto c = *character;
    if ((c>='0')&&(c<='9')) 
        return scan_integer_literal(start, length);
    switch (c)
    {
        case '.': 
            return scan_fraction(start, length);
            break;
        case 'E': 
            return scan_exponent(start, length);
            break;
        case 'e': 
            return scan_exponent(start, length);
            break;
        case 'x': 
            return scan_hex_literal(start, length);
            break;
        case 'B': 
            return scan_boolean_literal();
            break;
        default:
            return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));
    };
}

Token Lexer::scan_integer_literal(char* start, size_t length) {
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));
        const auto c = *character;
        if ((c>='0')&&(c<='9')) 
            continue;switch (c)
        {
            case '.': 
                return scan_fraction(start, length);
                break;
            case 'E': 
                return scan_exponent(start, length);
                break;
            case 'e': 
                return scan_exponent(start, length);
                break;
            default:
                return Token(LiteralToken(IntegerToken(Vector<char>(start, length))));
        };
    };
}

Token Lexer::scan_fraction(char* start, size_t length) {
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));
        const auto c = *character;
        if ((c>='0')&&(c<='9')) 
            continue;switch (c)
        {
            case 'E': 
                return scan_exponent(start, length);
                break;
            case 'e': 
                return scan_exponent(start, length);
                break;
            default:
                return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));
        };
    };
}

Token Lexer::scan_exponent(char* start, size_t length) {
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));
        const auto c = *character;
        if (((c>='0')&&(c<='9'))) 
            continue;;
        return Token(LiteralToken(FloatingPointToken(Vector<char>(start, length))));
    };
}

Token Lexer::scan_hex_literal(char* start, size_t length) {
    while (true) {
        read_character();
        length = length+1;
        if (character == nullptr) 
            return Token(LiteralToken(HexToken(Vector<char>(start, length))));
        const auto c = *character;
        if (((c>='0')&&(c<='9'))||(c>='A')&&(c<='F')||((c>='a')&&(c<='f'))) 
            continue;;
        return Token(LiteralToken(HexToken(Vector<char>(start, length))));
    };
}

Token Lexer::scan_boolean_literal() {
    read_character();
    if (character == nullptr) 
        return Token(InvalidToken());
    const auto c = *character;
    if (c != '0'&&c != '1') {
        return Token(InvalidToken());
    }
    else {
        read_character();
        return Token(LiteralToken(BooleanToken(c == '1')));
    };
}

void Lexer::skip_whitespace(bool skip_line_feed) {
    while (true) {
        switch ((size_t)character)
        {
            case 0: 
                return;
                break;
            default:
                {
                    switch (*character)
                    {
                        case ' ': 
                            {
                                read_character();
                                continue;;
                            };
                            break;
                        case '\t': 
                            {
                                read_character();
                                continue;;
                            };
                            break;
                        case '\r': 
                            {
                                read_character();
                                continue;;
                            };
                            break;
                        case '\n': 
                            if ((skip_line_feed)) {
                                read_character();
                                continue;;
                            }
                            else {
                                return;
                            };
                            break;
                        case '\\': 
                            {
                                read_character();
                                switch ((size_t)character)
                                {
                                    case 0: 
                                        return;
                                        break;
                                    default:
                                        {
                                            switch (*character)
                                            {
                                                case '\r': 
                                                    {
                                                        read_character();
                                                        read_character();
                                                        continue;;
                                                    };
                                                    break;
                                                case '\n': 
                                                    {
                                                        read_character();
                                                        continue;;
                                                    };
                                                    break;
                                            };
                                        };
                                };
                            };
                            break;
                        case ';': 
                            {
                                read_character();
                                switch ((size_t)character)
                                {
                                    case 0: 
                                        return;
                                        break;
                                    default:
                                        {
                                            if (*character != '*') {
                                                read_character();
                                                handle_single_line_comment();
                                            }
                                            else {
                                                read_character();
                                                handle_multi_line_comment();
                                            };
                                        };
                                };
                            };
                            break;
                        default:
                            return;
                    };
                };
        };
    };
}

void Lexer::handle_single_line_comment() {
    while (true) {
        switch ((size_t)character)
        {
            case 0: 
                return;
                break;
            default:
                {
                    if (*character == '\n') {
                        read_character();
                        return;
                    }
                    else {
                        read_character();
                        continue;;
                    };
                };
        };
    };
}

void Lexer::handle_multi_line_comment() {
    while (true) {
        switch ((size_t)character)
        {
            case 0: 
                return;
                break;
            default:
                {
                    switch (*character)
                    {
                        case ';': 
                            {
                                read_character();
                                switch ((size_t)character)
                                {
                                    case 0: 
                                        return;
                                        break;
                                    default:
                                        switch (*character)
                                        {
                                            case '*': 
                                                {
                                                    read_character();
                                                    handle_multi_line_comment();
                                                    continue;;
                                                };
                                                break;
                                            default:
                                                continue;;
                                        };
                                };
                            };
                            break;
                        case '*': 
                            {
                                read_character();
                                switch ((size_t)character)
                                {
                                    case 0: 
                                        return;
                                        break;
                                    default:
                                        switch (*character)
                                        {
                                            case ';': 
                                                {
                                                    read_character();
                                                    return;
                                                };
                                                break;
                                            default:
                                                read_character();
                                        };
                                };
                            };
                            break;
                        default:
                            {
                                read_character();
                                continue;;
                            };
                    };
                };
        };
    };
}

bool Lexer::parse_keyword(String fixed_string) {
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Identifier:
            {
                auto identifier = _result._Identifier;
                {
                    const auto right_keyword = fixed_string.equals(identifier.name);
                    if (right_keyword) 
                        empty();
                    return right_keyword;
                };
                break;
            }
            default:
                return false;
        }
    };
}

String* Lexer::parse_identifier(Page* rp, HashSet<String> keywords) {
    auto r = Region();
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Identifier:
            {
                auto identifier = _result._Identifier;
                {
                    if (keywords.contains(*new (alignof(String), r.get_page()) String(r.get_page(), identifier.name))) 
                        return nullptr;
                    const auto ret = new (alignof(String), rp) String(rp, identifier.name);
                    empty();
                    return ret;
                };
                break;
            }
            default:
                return nullptr;
        }
    };
}

String* Lexer::parse_attribute(Page* rp) {
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Attribute:
            {
                auto attribute = _result._Attribute;
                {
                    const auto ret = new (alignof(String), rp) String(rp, attribute.name);
                    empty();
                    return ret;
                };
                break;
            }
            default:
                return nullptr;
        }
    };
}

bool Lexer::parse_punctuation(char character) {
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Punctuation:
            {
                auto punctuation = _result._Punctuation;
                {
                    const auto ret = character == punctuation.sign;
                    if (ret) 
                        empty();
                    return ret;
                };
                break;
            }
            default:
                return false;
        }
    };
}

bool Lexer::parse_colon() {
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = token;
        switch (_result._tag)
        {
            case Token::Colon:
            {
                auto c = _result._Colon;
                {
                    empty();
                    return true;
                };
                break;
            }
            case Token::Empty:
            {
                auto e = _result._Empty;
                {
                    return true;
                };
                break;
            }
            default:
                return false;
        }
    };
}

bool Lexer::is_at_end() {
    if (character == nullptr) 
        return true;
    return false;
}

}
}