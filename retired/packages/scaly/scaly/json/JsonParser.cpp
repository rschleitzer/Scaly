#include "../../scaly.h"
namespace scaly {
namespace json {
using namespace scaly::containers;


IdentifierLiteral::IdentifierLiteral(String name) : name(name) {}

StringLiteral::StringLiteral(String value) : value(value) {}

CharacterLiteral::CharacterLiteral(String value) : value(value) {}

FragmentLiteral::FragmentLiteral(String value) : value(value) {}

IntegerLiteral::IntegerLiteral(String value) : value(value) {}

BooleanLiteral::BooleanLiteral(bool value) : value(value) {}

FloatingPointLiteral::FloatingPointLiteral(String value) : value(value) {}

HexLiteral::HexLiteral(String value) : value(value) {}
Literal::Literal(struct StringLiteral _String) : _tag(String), _String(_String) {}

Literal::Literal(struct CharacterLiteral _Character) : _tag(Character), _Character(_Character) {}

Literal::Literal(struct FragmentLiteral _Fragment) : _tag(Fragment), _Fragment(_Fragment) {}

Literal::Literal(struct IntegerLiteral _Integer) : _tag(Integer), _Integer(_Integer) {}

Literal::Literal(struct BooleanLiteral _Boolean) : _tag(Boolean), _Boolean(_Boolean) {}

Literal::Literal(struct FloatingPointLiteral _FloatingPoint) : _tag(FloatingPoint), _FloatingPoint(_FloatingPoint) {}

Literal::Literal(struct HexLiteral _Hex) : _tag(Hex), _Hex(_Hex) {}


NullSyntax::NullSyntax(size_t start, size_t end) : start(start), end(end) {}

FalseSyntax::FalseSyntax(size_t start, size_t end) : start(start), end(end) {}

TrueSyntax::TrueSyntax(size_t start, size_t end) : start(start), end(end) {}

LiteralSyntax::LiteralSyntax(size_t start, size_t end, Literal literal) : start(start), end(end), literal(literal) {}

ArraySyntax::ArraySyntax(size_t start, size_t end, Vector<ConstituentSyntax>* values) : start(start), end(end), values(values) {}

ObjectSyntax::ObjectSyntax(size_t start, size_t end, Vector<PropertySyntax>* properties) : start(start), end(end), properties(properties) {}
ValueSyntax::ValueSyntax(struct ObjectSyntax _Object) : _tag(Object), _Object(_Object) {}

ValueSyntax::ValueSyntax(struct ArraySyntax _Array) : _tag(Array), _Array(_Array) {}

ValueSyntax::ValueSyntax(struct LiteralSyntax _Literal) : _tag(Literal), _Literal(_Literal) {}

ValueSyntax::ValueSyntax(struct TrueSyntax _True) : _tag(True), _True(_True) {}

ValueSyntax::ValueSyntax(struct FalseSyntax _False) : _tag(False), _False(_False) {}

ValueSyntax::ValueSyntax(struct NullSyntax _Null) : _tag(Null), _Null(_Null) {}


ConstituentSyntax::ConstituentSyntax(size_t start, size_t end, ValueSyntax value) : start(start), end(end), value(value) {}

PropertySyntax::PropertySyntax(size_t start, size_t end, LiteralSyntax name, ValueSyntax value) : start(start), end(end), name(name), value(value) {}

Parser::Parser(Lexer lexer, Vector<String> keywords_index, HashSet<String> keywords) : lexer(lexer), keywords_index(keywords_index), keywords(keywords) {}

Parser::Parser(String text) : lexer(Lexer(text)), keywords_index(initialize_keywords_index()), keywords(initialize_keywords()){
}

Vector<String> Parser::initialize_keywords_index() {
    auto r = Region();
    auto keywords_builder = new (alignof(Array<String>), r.get_page()) Array<String>();
    auto p = Page::get(this);
    (*keywords_builder).add(String(p, "true"));
    (*keywords_builder).add(String(p, "false"));
    (*keywords_builder).add(String(p, "null"));
    return Vector<String>(p, *keywords_builder);
}

HashSet<String> Parser::initialize_keywords() {
    auto r = Region();
    auto hash_set_builder = new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>(keywords_index);
    return HashSet<String>(Page::get(this), *hash_set_builder);
}

Result<Literal, ParserError> Parser::parse_literal_token(Page* rp, Page* ep) {
    {
        auto _result = lexer.token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                lexer.advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = lexer.token;
        switch (_result._tag)
        {
            case Token::Literal:
            {
                auto literal = _result._Literal;
                {
                    {
                        auto _result = literal;
                        switch (_result._tag)
                        {
                            case LiteralToken::String:
                            {
                                auto string = _result._String;
                                {
                                    const auto ret = Literal(StringLiteral(String(rp, string.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Character:
                            {
                                auto character = _result._Character;
                                {
                                    const auto ret = Literal(CharacterLiteral(String(rp, character.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Integer:
                            {
                                auto integer = _result._Integer;
                                {
                                    const auto ret = Literal(IntegerLiteral(String(rp, integer.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::FloatingPoint:
                            {
                                auto fp = _result._FloatingPoint;
                                {
                                    const auto ret = Literal(FloatingPointLiteral(String(rp, fp.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Hex:
                            {
                                auto hex = _result._Hex;
                                {
                                    const auto ret = Literal(HexLiteral(String(rp, hex.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Boolean:
                            {
                                auto boolean = _result._Boolean;
                                {
                                    const auto ret = Literal(BooleanLiteral(boolean.value));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Fragment:
                            {
                                auto fragment = _result._Fragment;
                                {
                                    const auto ret = Literal(FragmentLiteral(String(rp, fragment.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                        }
                    };
                };
                break;
            }
            default:
                return Result<Literal, ParserError>(Result<Literal, ParserError>(ParserError(DifferentSyntax())));
        }
    };
}

Result<Vector<PropertySyntax>*, ParserError> Parser::parse_property_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<PropertySyntax>), r.get_page()) List<PropertySyntax>();
    while (true) {
        const auto _node_result = parse_property(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<PropertySyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<PropertySyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<PropertySyntax>*, ParserError>(new (alignof(Vector<PropertySyntax>), rp) Vector<PropertySyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }(*list).add(node);
    };
}

Result<PropertySyntax, ParserError> Parser::parse_property(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto name_start = lexer.position;
    const auto _name_result = parse_literal(rp, ep);
    auto name = _name_result._Ok;
    if (_name_result._tag == Success::Error) {
        const auto _name_Error = _name_result._Error;
        switch (_name_Error._tag) {
            case ParserError::Different: {
                const auto d = _name_Error._Different;
                return Result<PropertySyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _name_Error._Invalid;
                return Result<PropertySyntax, ParserError>(i);
                break;
            }
        }
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    if (success_colon_2 == false) {
        return Result<PropertySyntax, ParserError>(ParserError(InvalidSyntax(start_colon_2, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto value_start = lexer.position;
    const auto _value_result = parse_value(rp, ep);
    auto value = _value_result._Ok;
    if (_value_result._tag == Success::Error) {
        const auto _value_Error = _value_result._Error;
        switch (_value_Error._tag) {
            case ParserError::Different: {
                const auto d = _value_Error._Different;
                return Result<PropertySyntax, ParserError>(InvalidSyntax(value_start, lexer.position, String(ep, "a valid Value syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _value_Error._Invalid;
                return Result<PropertySyntax, ParserError>(i);
                break;
            }
        }
    };
    const auto start_comma_4 = lexer.previous_position;
    const auto success_comma_4 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<PropertySyntax, ParserError>(PropertySyntax(start, end, name, value));
}

Result<Vector<ConstituentSyntax>*, ParserError> Parser::parse_constituent_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ConstituentSyntax>), r.get_page()) List<ConstituentSyntax>();
    while (true) {
        const auto _node_result = parse_constituent(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ConstituentSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ConstituentSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ConstituentSyntax>*, ParserError>(new (alignof(Vector<ConstituentSyntax>), rp) Vector<ConstituentSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }(*list).add(node);
    };
}

Result<ConstituentSyntax, ParserError> Parser::parse_constituent(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto value_start = lexer.position;
    const auto _value_result = parse_value(rp, ep);
    auto value = _value_result._Ok;
    if (_value_result._tag == Success::Error) {
        const auto _value_Error = _value_result._Error;
        switch (_value_Error._tag) {
            case ParserError::Different: {
                const auto d = _value_Error._Different;
                return Result<ConstituentSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _value_Error._Invalid;
                return Result<ConstituentSyntax, ParserError>(i);
                break;
            }
        }
    };
    const auto start_comma_2 = lexer.previous_position;
    const auto success_comma_2 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(start, end, value));
}

Result<ValueSyntax, ParserError> Parser::parse_value(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_object(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ValueSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ValueSyntax, ParserError>(ValueSyntax(ObjectSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_array(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ValueSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ValueSyntax, ParserError>(ValueSyntax(ArraySyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_literal(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ValueSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ValueSyntax, ParserError>(ValueSyntax(LiteralSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_true(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ValueSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ValueSyntax, ParserError>(ValueSyntax(TrueSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_false(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ValueSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ValueSyntax, ParserError>(ValueSyntax(FalseSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_null(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ValueSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ValueSyntax, ParserError>(ValueSyntax(NullSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ValueSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<ObjectSyntax, ParserError> Parser::parse_object(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_curly_1 = lexer.previous_position;
    const auto success_left_curly_1 = lexer.parse_punctuation('{');
    if (success_left_curly_1 == false) {
        return Result<ObjectSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto properties_start = lexer.position;
    const auto _properties_result = parse_property_list(rp, ep);
    auto properties = _properties_result._Ok;
    if (_properties_result._tag == Success::Error) {
        const auto _properties_Error = _properties_result._Error;
        switch (_properties_Error._tag) {
            case ParserError::Different: {
                const auto d = _properties_Error._Different;
                return Result<ObjectSyntax, ParserError>(InvalidSyntax(properties_start, lexer.position, String(ep, "a valid Property syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _properties_Error._Invalid;
                return Result<ObjectSyntax, ParserError>(i);
                break;
            }
        }
    };
    const auto start_right_curly_3 = lexer.previous_position;
    const auto success_right_curly_3 = lexer.parse_punctuation('}');
    if (success_right_curly_3 == false) {
        return Result<ObjectSyntax, ParserError>(ParserError(InvalidSyntax(start_right_curly_3, lexer.position, String(ep, "}"))));
    };
    const auto end = lexer.position;
    return Result<ObjectSyntax, ParserError>(ObjectSyntax(start, end, properties));
}

Result<ArraySyntax, ParserError> Parser::parse_array(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_bracket_1 = lexer.previous_position;
    const auto success_left_bracket_1 = lexer.parse_punctuation('[');
    if (success_left_bracket_1 == false) {
        return Result<ArraySyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto values_start = lexer.position;
    Vector<ConstituentSyntax>* values = nullptr;
    {
        auto _result = parse_constituent_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ArraySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                values = success;
                break;
            }
        }
    };
    const auto start_right_bracket_3 = lexer.previous_position;
    const auto success_right_bracket_3 = lexer.parse_punctuation(']');
    if (success_right_bracket_3 == false) {
        return Result<ArraySyntax, ParserError>(ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(ep, "]"))));
    };
    const auto end = lexer.position;
    return Result<ArraySyntax, ParserError>(ArraySyntax(start, end, values));
}

Result<LiteralSyntax, ParserError> Parser::parse_literal(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto _literal_result = parse_literal_token(rp, ep);
    auto literal = _literal_result._Ok;
    if (_literal_result._tag == Success::Error) {
        const auto _literal_Error = _literal_result._Error;
        switch (_literal_Error._tag) {
            case ParserError::Different: {
                const auto d = _literal_Error._Different;
                return Result<LiteralSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _literal_Error._Invalid;
                return Result<LiteralSyntax, ParserError>(i);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<LiteralSyntax, ParserError>(LiteralSyntax(start, end, literal));
}

Result<TrueSyntax, ParserError> Parser::parse_true(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_true_1 = lexer.previous_position;
    const auto success_true_1 = lexer.parse_keyword(*keywords_index.get(0));
    if (success_true_1 == false) {
        return Result<TrueSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<TrueSyntax, ParserError>(TrueSyntax(start, end));
}

Result<FalseSyntax, ParserError> Parser::parse_false(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_false_1 = lexer.previous_position;
    const auto success_false_1 = lexer.parse_keyword(*keywords_index.get(1));
    if (success_false_1 == false) {
        return Result<FalseSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<FalseSyntax, ParserError>(FalseSyntax(start, end));
}

Result<NullSyntax, ParserError> Parser::parse_null(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_null_1 = lexer.previous_position;
    const auto success_null_1 = lexer.parse_keyword(*keywords_index.get(2));
    if (success_null_1 == false) {
        return Result<NullSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<NullSyntax, ParserError>(NullSyntax(start, end));
}

bool Parser::is_at_end() {
    return lexer.is_at_end();
}

}
}