#include "../../scaly.h"
namespace scaly {
namespace compiler {
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
LiteralToken::LiteralToken(StringToken _StringToken) : _tag(String) { _String = _StringToken; }

LiteralToken::LiteralToken(CharacterToken _CharacterToken) : _tag(Character) { _Character = _CharacterToken; }

LiteralToken::LiteralToken(FragmentToken _FragmentToken) : _tag(Fragment) { _Fragment = _FragmentToken; }

LiteralToken::LiteralToken(IntegerToken _IntegerToken) : _tag(Integer) { _Integer = _IntegerToken; }

LiteralToken::LiteralToken(BooleanToken _BooleanToken) : _tag(Boolean) { _Boolean = _BooleanToken; }

LiteralToken::LiteralToken(FloatingPointToken _FloatingPointToken) : _tag(FloatingPoint) { _FloatingPoint = _FloatingPointToken; }

LiteralToken::LiteralToken(HexToken _HexToken) : _tag(Hex) { _Hex = _HexToken; }

Token::Token(EmptyToken _EmptyToken) : _tag(Empty) { _Empty = _EmptyToken; }

Token::Token(InvalidToken _InvalidToken) : _tag(Invalid) { _Invalid = _InvalidToken; }

Token::Token(IdentifierToken _IdentifierToken) : _tag(Identifier) { _Identifier = _IdentifierToken; }

Token::Token(AttributeToken _AttributeToken) : _tag(Attribute) { _Attribute = _AttributeToken; }

Token::Token(PunctuationToken _PunctuationToken) : _tag(Punctuation) { _Punctuation = _PunctuationToken; }

Token::Token(LiteralToken _LiteralToken) : _tag(Literal) { _Literal = _LiteralToken; }

Token::Token(ColonToken _ColonToken) : _tag(Colon) { _Colon = _ColonToken; }


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
    if (c == '+'||c == '-'||c == '*'||c == '/'||c == '='||c == '%'||c == '&'||c == '|'||c == '~'||c == '<'||c == '>') {
        token = scan_operator();
        break;
    };
    if (c == '}'||c == ')'||c == ']'||c == '.'||c == '?'||c == '!'||c == '$'||c == '#'||c == '^') {
        token = Token(PunctuationToken(*character));
        read_character();
        skip_whitespace(false);
        return;
    };
    if (c == '{'||c == '('||c == '['||c == ',') {
        token = Token(PunctuationToken(*character));
        read_character();
        skip_whitespace(true);
        return;
    };
    switch (c)
    {
        case '\n':
            token = scan_line_feed();
        case ':':
            {
                read_character();
                token = Token(ColonToken());
            };
        case '0':
            {
                auto r_1 = Region();
                StringBuilder&value == *new(alignof(StringBuilder), r_1.get_page())StringBuilder();
                token = scan_numeric_literal();
            };
        case '@':
            {
                read_character();
                token = scan_attribute();
            };
        case '\"':
            token = scan_string_literal();
        case '`':
            token = scan_fragment_literal();
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
        if ((*character=='\n')) 
            continue;
        return Token(ColonToken());
    };
}

}
}