#ifndef _JsonParser_h
#define _JsonParser_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

struct IdentifierLiteral : Object {
    String name;

    IdentifierLiteral(String name);
};

struct StringLiteral : Object {
    String value;

    StringLiteral(String value);
};

struct CharacterLiteral : Object {
    String value;

    CharacterLiteral(String value);
};

struct FragmentLiteral : Object {
    String value;

    FragmentLiteral(String value);
};

struct IntegerLiteral : Object {
    String value;

    IntegerLiteral(String value);
};

struct BooleanLiteral : Object {
    bool value;

    BooleanLiteral(bool value);
};

struct FloatingPointLiteral : Object {
    String value;

    FloatingPointLiteral(String value);
};

struct HexLiteral : Object {
    String value;

    HexLiteral(String value);
};

struct Literal : Object {
    Literal(StringLiteral);
    Literal(CharacterLiteral);
    Literal(FragmentLiteral);
    Literal(IntegerLiteral);
    Literal(BooleanLiteral);
    Literal(FloatingPointLiteral);
    Literal(HexLiteral);
    enum {
        String,
        Character,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } _tag;
    union {
        struct StringLiteral _String;
        struct CharacterLiteral _Character;
        struct FragmentLiteral _Fragment;
        struct IntegerLiteral _Integer;
        struct BooleanLiteral _Boolean;
        struct FloatingPointLiteral _FloatingPoint;
        struct HexLiteral _Hex;
    };
};
struct NullSyntax : Object {
    size_t start;
    size_t end;

    NullSyntax(size_t start, size_t end);
};

struct FalseSyntax : Object {
    size_t start;
    size_t end;

    FalseSyntax(size_t start, size_t end);
};

struct TrueSyntax : Object {
    size_t start;
    size_t end;

    TrueSyntax(size_t start, size_t end);
};

struct LiteralSyntax : Object {
    size_t start;
    size_t end;
    Literal literal;

    LiteralSyntax(size_t start, size_t end, Literal literal);
};

struct ArraySyntax : Object {
    size_t start;
    size_t end;
    Vector<ConstituentSyntax>* values;

    ArraySyntax(size_t start, size_t end, Vector<ConstituentSyntax>* values);
};

struct ObjectSyntax : Object {
    size_t start;
    size_t end;
    Vector<PropertySyntax>* properties;

    ObjectSyntax(size_t start, size_t end, Vector<PropertySyntax>* properties);
};

struct ValueSyntax : Object {
    ValueSyntax(ObjectSyntax);
    ValueSyntax(ArraySyntax);
    ValueSyntax(LiteralSyntax);
    ValueSyntax(TrueSyntax);
    ValueSyntax(FalseSyntax);
    ValueSyntax(NullSyntax);
    enum {
        Object,
        Array,
        Literal,
        True,
        False,
        Null,
    } _tag;
    union {
        struct ObjectSyntax _Object;
        struct ArraySyntax _Array;
        struct LiteralSyntax _Literal;
        struct TrueSyntax _True;
        struct FalseSyntax _False;
        struct NullSyntax _Null;
    };
};
struct ConstituentSyntax : Object {
    size_t start;
    size_t end;
    ValueSyntax value;

    ConstituentSyntax(size_t start, size_t end, ValueSyntax value);
};

struct PropertySyntax : Object {
    size_t start;
    size_t end;
    LiteralSyntax name;
    ValueSyntax value;

    PropertySyntax(size_t start, size_t end, LiteralSyntax name, ValueSyntax value);
};

struct Parser : Object {
    Lexer lexer;
    Vector<String> keywords_index;
    HashSet<String> keywords;

    Parser(Lexer lexer, Vector<String> keywords_index, HashSet<String> keywords);
    Parser(String text);
    Vector<String> initialize_keywords_index();
    HashSet<String> initialize_keywords();
    Result<Literal, ParserError> parse_literal_token(Page* rp, Page* ep);
    Result<Vector<PropertySyntax>*, ParserError> parse_property_list(Page* rp, Page* ep);
    Result<PropertySyntax, ParserError> parse_property(Page* rp, Page* ep);
    Result<Vector<ConstituentSyntax>*, ParserError> parse_constituent_list(Page* rp, Page* ep);
    Result<ConstituentSyntax, ParserError> parse_constituent(Page* rp, Page* ep);
    Result<ValueSyntax, ParserError> parse_value(Page* rp, Page* ep);
    Result<ObjectSyntax, ParserError> parse_object(Page* rp, Page* ep);
    Result<ArraySyntax, ParserError> parse_array(Page* rp, Page* ep);
    Result<LiteralSyntax, ParserError> parse_literal(Page* rp, Page* ep);
    Result<TrueSyntax, ParserError> parse_true(Page* rp, Page* ep);
    Result<FalseSyntax, ParserError> parse_false(Page* rp, Page* ep);
    Result<NullSyntax, ParserError> parse_null(Page* rp, Page* ep);
    bool is_at_end();
};

#endif