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
        initializer = i;
    };
    List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
    if (property.attributes != nullptr) 
        
    auto _attribute_syntax_iterator = property.attributes.get_iterator();
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
    return Result<Property, ModelError>(Property(Span(property.start, property.end), false, String(rp, property.name), type, initializer, Vector<Attribute>(rp, attributes)));
}

Result<Type*, ModelError> model::handle_type(Page* rp, Page* ep, TypeSyntax& type_syntax, String file) {
    auto r = Region();
    List<String>& path = path*new (alignof(List<String>), r.get_page()) List<String>();
    path.add(String(rp, type_syntax.name.name));
    if (type_syntax.name.extensions != nullptr) 
        
    auto _extension_iterator = type_syntax.name.extensions.get_iterator();
    while (auto _extension = _extension_iterator.next()) {
        auto extension = *_extension;path.add(extension.name)
    };
    Vector<Type>* generics = nullptr;
    auto lifetime = Lifetime(Unspecified());
    List<Type>& generics_builder = *new (alignof(List<Type>), r.get_page()) List<Type>();
    if (type_syntax.generics != nullptr) {
        const auto generic_arguments = (*type_syntax.generics).generics;
        if (generic_arguments != nullptr) {
            
            auto _generic_iterator = generic_arguments.get_iterator();
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
            generics == new (alignof(Vector<Type>), rp) Vector<Type>();
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
                    lifetime = Lifetime(Reference(String(rp, rlocation)));
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

Result<Operand, ModelError> model::handle_operand(Page* rp, Page* ep, OperandSyntax& operand, String file) {
    auto r = Region();
    Vector<String>* member_access = nullptr;
    if (operand.members != nullptr) {
        const List<String>& path = pathnew (alignof(List<String>), r.get_page()) List<String>();
        
        auto _member_iterator = *operand.members.get_iterator();
        while (auto _member = _member_iterator.next()) {
            auto member = *_member;{
                path.add(String(rp, member.name.name));
                if (member.name.extensions != nullptr) 
                    
                auto _extension_iterator = *member.name.extensions.get_iterator();
                while (auto _extension = _extension_iterator.next()) {
                    auto extension = *_extension;path.add(extension.name)
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
        
        auto _operand_syntax_iterator = operands.get_iterator();
        while (auto _operand_syntax = _operand_syntax_iterator.next()) {
            auto operand_syntax = *_operand_syntax;{
                const auto _operand_result = handle_operand(rp, ep, *operand, file);
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