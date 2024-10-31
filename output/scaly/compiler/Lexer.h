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

#endif