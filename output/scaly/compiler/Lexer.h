#ifndef _Lexer_h
#define _Lexer_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

struct EmptyToken : Object {

};

struct InvalidToken : Object {

};

struct ColonToken : Object {

};

struct IdentifierToken : Object {
    Vector<char> name;

    IdentifierToken(Vector<char> name);
};

struct AttributeToken : Object {
    Vector<char> name;

    AttributeToken(Vector<char> name);
};

struct PunctuationToken : Object {
    char sign;

    PunctuationToken(char sign);
};

struct StringToken : Object {
    Vector<char> value;

    StringToken(Vector<char> value);
};

struct CharacterToken : Object {
    Vector<char> value;

    CharacterToken(Vector<char> value);
};

struct FragmentToken : Object {
    Vector<char> value;

    FragmentToken(Vector<char> value);
};

struct IntegerToken : Object {
    Vector<char> value;

    IntegerToken(Vector<char> value);
};

struct BooleanToken : Object {
    bool value;

    BooleanToken(bool value);
};

struct FloatingPointToken : Object {
    Vector<char> value;

    FloatingPointToken(Vector<char> value);
};

struct HexToken : Object {
    Vector<char> value;

    HexToken(Vector<char> value);
};

struct LiteralToken {
    LiteralToken(StringToken _StringToken);
    LiteralToken(CharacterToken _CharacterToken);
    LiteralToken(FragmentToken _FragmentToken);
    LiteralToken(IntegerToken _IntegerToken);
    LiteralToken(BooleanToken _BooleanToken);
    LiteralToken(FloatingPointToken _FloatingPointToken);
    LiteralToken(HexToken _HexToken);
    enum {
        String,
        Character,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } tag;
    union {
        StringToken _String;
        CharacterToken _Character;
        FragmentToken _Fragment;
        IntegerToken _Integer;
        BooleanToken _Boolean;
        FloatingPointToken _FloatingPoint;
        HexToken _Hex;
    };
};
struct Token {
    Token(EmptyToken);
    Token(InvalidToken);
    Token(IdentifierToken);
    Token(AttributeToken);
    Token(PunctuationToken);
    Token(LiteralToken);
    Token(ColonToken);
    enum {
        Empty,
        Invalid,
        Identifier,
        Attribute,
        Punctuation,
        Literal,
        Colon,
    } tag;
    union {
        EmptyToken Empty_;
        InvalidToken Invalid_;
        IdentifierToken Identifier_;
        AttributeToken Attribute_;
        PunctuationToken Punctuation_;
        LiteralToken Literal_;
        ColonToken Colon_;
    };
};
struct Lexer : Object {
    Token token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    Lexer(Token token, char* character, StringIterator chars, size_t previous_position, size_t position);
    Lexer ();
    Lexer(String deck);
    void read_character();
    void advance();
    void empty();
    Token scan_line_feed();
    Token scan_identifier();
    Token scan_attribute();
    Token scan_operator();
    Token scan_string_literal();
    Token scan_string_identifier();
    Token scan_fragment_literal();
    Token scan_numeric_literal();
    Token scan_integer_literal(char* start, size_t length);
    Token scan_fraction(char* start, size_t length);
    Token scan_exponent(char* start, size_t length);
    Token scan_hex_literal(char* start, size_t length);
    Token scan_boolean_literal();
    void skip_whitespace(bool skip_line_feed);
    void handle_single_line_comment();
    void handle_multi_line_comment();
    bool parse_keyword(String fixed_string);
    String* parse_identifier(Page* rp, HashSet<String> keywords);
    String* parse_attribute(Page* rp);
    bool parse_punctuation(char character);
    bool parse_colon();
    bool is_at_end();
};

#endif