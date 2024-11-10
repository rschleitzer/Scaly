#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;


Result<ProgramSyntax, ParserError> model::parse_program(Page* rp, Page* ep, String program) {
    auto r = Region();
    Parser& parser = *new (alignof(Parser), r.get_page()) Parser(program);
    const auto program_syntax_result = parser.parse_program(rp, ep);
    if (parser.is_at_end() == false) 
        return Result<ProgramSyntax, ParserError>(ParserError(InvalidSyntax(parser.lexer.previous_position, parser.lexer.position, String(ep, "a valid declaration"))));
    return Result<ProgramSyntax, ParserError>(program_syntax_result);
}

Result<Property, ModelError> model::handle_property(Page* rp, Page* ep, bool private_, PropertySyntax& property, String file) {
    auto r = Region();
    const auto _type_result = handle_type(rp, ep, property.annotation.type, file);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
        default:
            return Result<Property, ModelError>(_type_result._Error);

        }
    };
    Vector<Operand>* initializer = nullptr;
    if (property.initializer != nullptr) {
        const auto _i_result = handle_operands(rp, ep, (*property.initializer).operands, file);
        auto i = _i_result._Ok;
        if (_i_result._tag == Success::Error) {
            const auto _i_Error = _i_result._Error;
            switch (_i_Error._tag) {
            default:
                return Result<Property, ModelError>(_i_result._Error);

            }
        };
        initializer = new (alignof(Vector<Operand>), r.get_page()) Vector<Operand>(i);
    };
    List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
    if (property.attributes != nullptr) {
        
        auto _attribute_syntax_iterator = (*property.attributes).get_iterator();
        while (auto _attribute_syntax = _attribute_syntax_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;const auto _attribute_result = handle_attribute(rp, ep, attribute_syntax, file);
            auto attribute = _attribute_result._Ok;
            if (_attribute_result._tag == Success::Error) {
                const auto _attribute_Error = _attribute_result._Error;
                switch (_attribute_Error._tag) {
                default:
                    return Result<Property, ModelError>(_attribute_result._Error);

                }
            }
        };
    };
    return Result<Property, ModelError>(Property(Span(property.start, property.end), false, String(rp, property.name), type, initializer, Vector<Attribute>(rp, attributes)));
}

Result<Constant, ModelError> model::handle_literal(Page* rp, Page* ep, LiteralSyntax& literal, String file) {
    auto r = Region();
    {
        auto _result = literal.literal;
        switch (_result._tag)
        {
            case Literal::Boolean:
            {
                auto boolean = _result._Boolean;
                return Result<Constant, ModelError>(Constant(BooleanConstant(boolean.value)));
                break;
            }
            case Literal::Integer:
            {
                auto integer = _result._Integer;
                {
                    const auto value = (size_t)strtol(integer.value.to_c_string(r.get_page()), nullptr, 10);
                    if (errno == ERANGE) 
                        return Result<Constant, ModelError>(ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))));
                    return Result<Constant, ModelError>(Constant(IntegerConstant(value)));
                };
                break;
            }
            case Literal::Hex:
            {
                auto hex = _result._Hex;
                {
                    const auto value = (size_t)strtoul(hex.value.to_c_string(r.get_page()), nullptr, 16);
                    if (errno == ERANGE) 
                        return Result<Constant, ModelError>(ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))));
                    return Result<Constant, ModelError>(Constant(HexConstant(value)));
                };
                break;
            }
            case Literal::FloatingPoint:
            {
                auto floating_point = _result._FloatingPoint;
                {
                    const auto value = strtod(floating_point.value.to_c_string(r.get_page()), nullptr);
                    if (errno == ERANGE) 
                        ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end))));
                    return Result<Constant, ModelError>(Constant(FloatingPointConstant(floating_point)));
                };
                break;
            }
            case Literal::String:
            {
                auto string = _result._String;
                Constant(StringConstant(string.value));
                break;
            }
            case Literal::Character:
            {
                auto character = _result._Character;
                return Result<Constant, ModelError>(Constant(CharacterConstant(character.value)));
                break;
            }
            case Literal::Fragment:
            {
                auto fragment = _result._Fragment;
                return Result<Constant, ModelError>(Constant(FragmentConstant(fragment.value)));
                break;
            }
        }
    };
}

Result<Type*, ModelError> model::handle_type(Page* rp, Page* ep, TypeSyntax& type_syntax, String file) {
    auto r = Region();
    List<String>& path = *new (alignof(List<String>), r.get_page()) List<String>();
    path.add(String(rp, type_syntax.name.name));
    if (type_syntax.name.extensions != nullptr) {
        
        auto _extension_iterator = (*type_syntax.name.extensions).get_iterator();
        while (auto _extension = _extension_iterator.next()) {
            auto extension = *_extension;{
                path.add(extension.name);
            }
        };
    };
    Vector<Type>* generics = nullptr;
    auto lifetime = Lifetime(Unspecified());
    List<Type>& generics_builder = *new (alignof(List<Type>), r.get_page()) List<Type>();
    if (type_syntax.generics != nullptr) {
        const auto generic_arguments = (*type_syntax.generics).generics;
        if (generic_arguments != nullptr) {
            
            auto _generic_iterator = (*generic_arguments).get_iterator();
            while (auto _generic = _generic_iterator.next()) {
                auto generic = *_generic;{
                    const auto _type_result = handle_type(rp, ep, generic.type, file);
                    auto type = _type_result._Ok;
                    if (_type_result._tag == Success::Error) {
                        const auto _type_Error = _type_result._Error;
                        switch (_type_Error._tag) {
                        default:
                            return Result<Type*, ModelError>(_type_result._Error);

                        }
                    };
                    generics_builder.add(*type);
                }
            };
            generics = new (alignof(Vector<Type>), rp) Vector<Type>(rp, generics_builder);
        }
        else {
            generics = new (alignof(Vector<Type>), rp) Vector<Type>();
        };
    };
    if (type_syntax.lifetime != nullptr) {
        const auto lifetime_syntax = *type_syntax.lifetime;
        {
            auto _result = lifetime_syntax;
            switch (_result._tag)
            {
                case LifetimeSyntax::Call:
                {
                    auto c = _result._Call;
                    lifetime = Lifetime(Call());
                    break;
                }
                case LifetimeSyntax::Local:
                {
                    auto l = _result._Local;
                    lifetime = Lifetime(Local());
                    break;
                }
                case LifetimeSyntax::Reference:
                {
                    auto r = _result._Reference;
                    lifetime = Lifetime(Reference(String(rp, r.location)));
                    break;
                }
                case LifetimeSyntax::Thrown:
                {
                    auto t = _result._Thrown;
                    lifetime = Lifetime(Thrown());
                    break;
                }
            }
        };
    };
    return Result<Type*, ModelError>(new (alignof(Type), rp) Type(Span(type_syntax.start, type_syntax.end), Vector<String>(rp, path), generics, lifetime));
}

Result<Model, ModelError> model::handle_model(Page* rp, Page* ep, ModelSyntax& model, String file) {
    {
        auto _result = model;
        switch (_result._tag)
        {
            case ModelSyntax::Literal:
            {
                auto literal = _result._Literal;
                {
                    const auto _constant_result = handle_literal(rp, ep, literal, file);
                    auto constant = _constant_result._Ok;
                    if (_constant_result._tag == Success::Error) {
                        const auto _constant_Error = _constant_result._Error;
                        switch (_constant_Error._tag) {
                        default:
                            return Result<Model, ModelError>(_constant_result._Error);

                        }
                    };
                    return Result<Model, ModelError>(constant);
                };
                break;
            }
            case ModelSyntax::Name:
            {
                auto name = _result._Name;
                {
                    return Result<Model, ModelError>(String(rp, name.name));
                };
                break;
            }
            case ModelSyntax::Object:
            {
                auto object_syntax = _result._Object;
                {
                    const auto _tuple_result = handle_object(rp, ep, object_syntax, file);
                    auto tuple = _tuple_result._Ok;
                    if (_tuple_result._tag == Success::Error) {
                        const auto _tuple_Error = _tuple_result._Error;
                        switch (_tuple_Error._tag) {
                        default:
                            return Result<Model, ModelError>(_tuple_result._Error);

                        }
                    };
                    return Result<Model, ModelError>(tuple);
                };
                break;
            }
            case ExpressionSyntax::Vector:
            {
                auto vector = _result._Vector;
                {
                    const auto _matrix_result = handle_vector(rp, ep, vector, file);
                    auto matrix = _matrix_result._Ok;
                    if (_matrix_result._tag == Success::Error) {
                        const auto _matrix_Error = _matrix_result._Error;
                        switch (_matrix_Error._tag) {
                        default:
                            return Result<Model, ModelError>(_matrix_result._Error);

                        }
                    };
                    return Result<Model, ModelError>(matrix);
                };
                break;
            }
        }
    };
}

Result<Attribute, ModelError> model::handle_attribute(Page* rp, Page* ep, AttributeSyntax& attribute, String file) {
    const auto _model_result = handle_model(rp, ep, attribute.model, file);
    auto model = _model_result._Ok;
    if (_model_result._tag == Success::Error) {
        const auto _model_Error = _model_result._Error;
        switch (_model_Error._tag) {
        default:
            return Result<Attribute, ModelError>(_model_result._Error);

        }
    };
    return Result<Attribute, ModelError>(Attribute(Span(attribute.start, attribute.end), attribute.name, model));
}

Result<Operand, ModelError> model::handle_operand(Page* rp, Page* ep, OperandSyntax& operand, String file) {
    auto r = Region();
    Vector<String>* member_access = nullptr;
    if (operand.members != nullptr) {
        List<String>& path = *new (alignof(List<String>), r.get_page()) List<String>();
        
        auto _member_iterator = (*operand.members).get_iterator();
        while (auto _member = _member_iterator.next()) {
            auto member = *_member;{
                path.add(String(rp, member.name.name));
                if (member.name.extensions != nullptr) {
                    
                    auto _extension_iterator = (*member.name.extensions).get_iterator();
                    while (auto _extension = _extension_iterator.next()) {
                        auto extension = *_extension;{
                            path.add(extension.name);
                        }
                    };
                };
            }
        };
        member_access = new (alignof(Vector<String>), rp) Vector<String>(rp, path);
    };
    const auto _expression_result = handle_expression(rp, ep, operand.expression, file);
    auto expression = _expression_result._Ok;
    if (_expression_result._tag == Success::Error) {
        const auto _expression_Error = _expression_result._Error;
        switch (_expression_Error._tag) {
        default:
            return Result<Operand, ModelError>(_expression_result._Error);

        }
    };
    return Result<Operand, ModelError>(Operand(Span(operand.start, operand.end), expression, member_access));
}

Result<Vector<Operand>, ModelError> model::handle_operands(Page* rp, Page* ep, Vector<OperandSyntax>* operands, String file) {
    auto r = Region();
    List<Operand>& operands_builder = *new (alignof(List<Operand>), r.get_page()) List<Operand>();
    if (operands != nullptr) {
        
        auto _operand_syntax_iterator = (*operands).get_iterator();
        while (auto _operand_syntax = _operand_syntax_iterator.next()) {
            auto operand_syntax = *_operand_syntax;{
                const auto _operand_result = handle_operand(rp, ep, operand_syntax, file);
                auto operand = _operand_result._Ok;
                if (_operand_result._tag == Success::Error) {
                    const auto _operand_Error = _operand_result._Error;
                    switch (_operand_Error._tag) {
                    default:
                        return Result<Vector<Operand>, ModelError>(_operand_result._Error);

                    }
                };
                operands_builder.add(operand);
            }
        };
    };
    return Result<Vector<Operand>, ModelError>(Vector<Operand>(rp, operands_builder));
}

}
}