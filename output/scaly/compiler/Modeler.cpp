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
                        return Result<Constant, ModelError>(ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))));
                    return Result<Constant, ModelError>(Constant(FloatingPointConstant(value)));
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
    return Result<Constant, ModelError>(ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))));
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

Result<Component, ModelError> model::handle_component(Page* rp, Page* ep, ComponentSyntax& component, String file) {
    auto r = Region();
    String* name = nullptr;
    auto attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
    if (component.attributes != nullptr) {
        
        auto _attribute_syntax_iterator = (*component.attributes).get_iterator();
        while (auto _attribute_syntax = _attribute_syntax_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;{
                const auto _attribute_result = handle_attribute(rp, ep, attribute_syntax, file);
                auto attribute = _attribute_result._Ok;
                if (_attribute_result._tag == Success::Error) {
                    const auto _attribute_Error = _attribute_result._Error;
                    switch (_attribute_Error._tag) {
                    default:
                        return Result<Component, ModelError>(_attribute_result._Error);

                    }
                };
                attributes.add(attribute);
            }
        };
    };
    if (component.value != nullptr) {
        if (component.operands != nullptr) {
            auto name_operands = *component.operands;
            if (name_operands.length>0) {
                auto name_operand = *name_operands.get(0);
                if (name_operands.length>1) 
                    return Result<Component, ModelError>(ModelError(ModelBuilderError(InvalidComponentName(file, Span(name_operand.start, name_operand.end)))));
                {
                    auto _result = name_operand.expression;
                    switch (_result._tag)
                    {
                        case ExpressionSyntax::Type:
                        {
                            auto type = _result._Type;
                            name = new (alignof(String), rp) String(rp, type.name.name);
                            break;
                        }
                        default:
                            {
                        };
                    }
                };
            };
        };
        const auto value = (*component.value).value;
        if (value != nullptr) {
            const auto _value_result_result = handle_operands(rp, ep, value, file);
            auto value_result = _value_result_result._Ok;
            if (_value_result_result._tag == Success::Error) {
                const auto _value_result_Error = _value_result_result._Error;
                switch (_value_result_Error._tag) {
                default:
                    return Result<Component, ModelError>(_value_result_result._Error);

                }
            };
            return Result<Component, ModelError>(Component(Span(component.start, component.end), name, value_result, Vector<Attribute>(rp, attributes)));
        }
        else {
            return Result<Component, ModelError>(Component(Span(component.start, component.end), name, Vector<Operand>(), Vector<Attribute>(rp, attributes)));
        };
    }
    else {
        if (component.operands != nullptr) {
            const auto _value_result_result = handle_operands(rp, ep, component.operands, file);
            auto value_result = _value_result_result._Ok;
            if (_value_result_result._tag == Success::Error) {
                const auto _value_result_Error = _value_result_result._Error;
                switch (_value_result_Error._tag) {
                default:
                    return Result<Component, ModelError>(_value_result_result._Error);

                }
            };
            return Result<Component, ModelError>(Component(Span(component.start, component.end), name, value_result, Vector<Attribute>(rp, attributes)));
        }
        else {
            return Result<Component, ModelError>(Component(Span(component.start, component.end), name, Vector<Operand>(), Vector<Attribute>(rp, attributes)));
        };
    };
}

Result<Tuple, ModelError> model::handle_object(Page* rp, Page* ep, ObjectSyntax& object, String file) {
    auto r = Region();
    auto components_builder = *new (alignof(List<Component>), r.get_page()) List<Component>();
    if (object.components != nullptr) {
        
        auto _component_iterator = (*object.components).get_iterator();
        while (auto _component = _component_iterator.next()) {
            auto component = *_component;{
                const auto _component_result_result = handle_component(rp, ep, component, file);
                auto component_result = _component_result_result._Ok;
                if (_component_result_result._tag == Success::Error) {
                    const auto _component_result_Error = _component_result_result._Error;
                    switch (_component_result_Error._tag) {
                    default:
                        return Result<Tuple, ModelError>(_component_result_result._Error);

                    }
                };
                components_builder.add(component_result);
            }
        };
    };
    return Result<Tuple, ModelError>(Tuple(Span(object.start, object.end), Vector<Component>(rp, components_builder)));
}

Result<Matrix, ModelError> model::handle_vector(Page* rp, Page* ep, VectorSyntax& vector, String file) {
    auto r = Region();
    auto operations_builder = *new (alignof(List<Vector<Operand>>), r.get_page()) List<Vector<Operand>>();
    if (vector.elements != nullptr) {
        
        auto _element_iterator = (*vector.elements).get_iterator();
        while (auto _element = _element_iterator.next()) {
            auto element = *_element;{
                const auto _operation_result_result = handle_operands(rp, ep, element.operation, file);
                auto operation_result = _operation_result_result._Ok;
                if (_operation_result_result._tag == Success::Error) {
                    const auto _operation_result_Error = _operation_result_result._Error;
                    switch (_operation_result_Error._tag) {
                    default:
                        return Result<Matrix, ModelError>(_operation_result_result._Error);

                    }
                };
                operations_builder.add(operation_result);
            }
        };
    };
    return Result<Matrix, ModelError>(Matrix(Span(vector.start, vector.end), Vector<Vector<Operand>>(rp, operations_builder)));
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

Result<Block, ModelError> model::handle_block(Page* rp, Page* ep, BlockSyntax& block, String file) {
    auto r = Region();
    if (block.statements != nullptr) {
        const auto _statements_result = handle_statements(rp, ep, *block.statements, file);
        auto statements = _statements_result._Ok;
        if (_statements_result._tag == Success::Error) {
            const auto _statements_Error = _statements_result._Error;
            switch (_statements_Error._tag) {
            default:
                return Result<Block, ModelError>(_statements_result._Error);

            }
        };
        return Result<Block, ModelError>(statements);
    };
    return Result<Block, ModelError>(Block(Span(block.start, block.end), Vector<Statement>()));
}

Result<If, ModelError> model::handle_if(Page* rp, Page* ep, IfSyntax& if_syntax, String file) {
    auto r = Region();
    Property* property = nullptr;
    const auto _condition_result = handle_operands(rp, ep, if_syntax.condition, file);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
        default:
            return Result<If, ModelError>(_condition_result._Error);

        }
    };
    const auto _consequent_result = handle_command(rp, ep, if_syntax.consequent, file);
    auto consequent = _consequent_result._Ok;
    if (_consequent_result._tag == Success::Error) {
        const auto _consequent_Error = _consequent_result._Error;
        switch (_consequent_Error._tag) {
        default:
            return Result<If, ModelError>(_consequent_result._Error);

        }
    };
    Statement* alternative = nullptr;
    if (if_syntax.alternative != nullptr) {
        const auto _alternative_result_result = handle_command(rp, ep, (*if_syntax.alternative).alternative, file);
        auto alternative_result = _alternative_result_result._Ok;
        if (_alternative_result_result._tag == Success::Error) {
            const auto _alternative_result_Error = _alternative_result_result._Error;
            switch (_alternative_result_Error._tag) {
            default:
                return Result<If, ModelError>(_alternative_result_result._Error);

            }
        };
        alternative = new (alignof(Statement), rp) Statement(alternative_result);
    };
    return Result<If, ModelError>(If(Span(if_syntax.start, if_syntax.end), condition, property, consequent, alternative));
}

Result<Match, ModelError> model::handle_match(Page* rp, Page* ep, MatchSyntax& match_syntax, String file) {
    auto r = Region();
    const auto _condition_result = handle_operands(rp, ep, match_syntax.scrutinee, file);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
        default:
            return Result<Match, ModelError>(_condition_result._Error);

        }
    };
    List<Branch>& branches_builder = *new (alignof(List<Branch>), r.get_page()) List<Branch>();
    if (match_syntax.branches != nullptr) {
        
        auto _branch_iterator = (*match_syntax.branches).get_iterator();
        while (auto _branch = _branch_iterator.next()) {
            auto branch = *_branch;{
                List<Case>& cases_builder = *new (alignof(List<Case>), r.get_page()) List<Case>();
                
                auto _case_syntax_iterator = (*branch.cases).get_iterator();
                while (auto _case_syntax = _case_syntax_iterator.next()) {
                    auto case_syntax = *_case_syntax;{
                        const auto _condition_result = handle_operands(rp, ep, case_syntax.condition, file);
                        auto condition = _condition_result._Ok;
                        if (_condition_result._tag == Success::Error) {
                            const auto _condition_Error = _condition_result._Error;
                            switch (_condition_Error._tag) {
                            default:
                                return Result<Match, ModelError>(_condition_result._Error);

                            }
                        };
                        cases_builder.add(Case(Span(case_.start, case_.end), condition));
                    }
                };
                const auto _consequent_result = handle_statement(rp, ep, branch.consequent, file);
                auto consequent = _consequent_result._Ok;
                if (_consequent_result._tag == Success::Error) {
                    const auto _consequent_Error = _consequent_result._Error;
                    switch (_consequent_Error._tag) {
                    default:
                        return Result<Match, ModelError>(_consequent_result._Error);

                    }
                };
                branches_builder.add(Branch(Span(branch.start, branch.end), Vector<Case>(rp, cases_builder), consequent));
            }
        };
    };
    auto alternative = Statement*nullptr;
    if (match_.alternative != nullptr) {
        const auto _alternative_result_result = handle_command(rp, ep, (*match_syntax.alternative).alternative, file);
        auto alternative_result = _alternative_result_result._Ok;
        if (_alternative_result_result._tag == Success::Error) {
            const auto _alternative_result_Error = _alternative_result_result._Error;
            switch (_alternative_result_Error._tag) {
            default:
                return Result<Match, ModelError>(_alternative_result_result._Error);

            }
        };
        alternative = new (alignof(Statement), rp) Statement(alternative_result);
    };
    return Result<Match, ModelError>(Match(Span(match_syntax.start, match_syntax.end), condition, Vector<Branch>(rp, branches_builder), alternative));
}

Result<When, ModelError> model::handle_when(Page* rp, Page* ep, WhenSyntax& when_syntax, String file) {
    auto r = Region();
    List<String>& name_builder = *new (alignof(List<String>), r.get_page()) List<String>();
    name_builder.add(when_syntax.variant.name);
    if (when_syntax.variant.extensions != nullptr) {
        
        auto _name_iterator = (*when_syntax.variant.extensions).get_iterator();
        while (auto _name = _name_iterator.next()) {
            auto name = *_name;{
                name_builder.add(name.name);
            }
        };
    };
    const auto _consequent_result = handle_command(rp, ep, when_syntax.command, file);
    auto consequent = _consequent_result._Ok;
    if (_consequent_result._tag == Success::Error) {
        const auto _consequent_Error = _consequent_result._Error;
        switch (_consequent_Error._tag) {
        default:
            return Result<When, ModelError>(_consequent_result._Error);

        }
    };
    return Result<When, ModelError>(When(Span(when_syntax.start, when_syntax.end), when_syntax.name, Vector<String>(rp, name_builder), consequent));
}

Result<Choose, ModelError> model::handle_choose(Page* rp, Page* ep, ChooseSyntax& choose_syntax, String file) {
    auto r = Region();
    const auto _condition_result = handle_operands(rp, ep, choose_syntax.condition, file);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
        default:
            return Result<Choose, ModelError>(_condition_result._Error);

        }
    };
    List<When>& cases_builder = *new (alignof(List<When>), r.get_page()) List<When>();
    if (choose_syntax.cases != nullptr) {
        
        auto _case_syntax_iterator = (*choose_syntax.cases).get_iterator();
        while (auto _case_syntax = _case_syntax_iterator.next()) {
            auto case_syntax = *_case_syntax;{
                const auto _case_result_result = handle_when(rp, ep, case_syntax, file);
                auto case_result = _case_result_result._Ok;
                if (_case_result_result._tag == Success::Error) {
                    const auto _case_result_Error = _case_result_result._Error;
                    switch (_case_result_Error._tag) {
                    default:
                        return Result<Choose, ModelError>(_case_result_result._Error);

                    }
                };
                cases_builder.add(case_result);
            }
        };
    };
    const Statement* alternative = nullptr;
    if (choose_syntax.alternative != nullptr) {
        const auto _alternative_result_result = handle_command(rp, ep, choose_.alternative::alternative, file);
        auto alternative_result = _alternative_result_result._Ok;
        if (_alternative_result_result._tag == Success::Error) {
            const auto _alternative_result_Error = _alternative_result_result._Error;
            switch (_alternative_result_Error._tag) {
            default:
                return Result<Choose, ModelError>(_alternative_result_result._Error);

            }
        };
        alternative = new (alignof(Statement), rp) Statement(alternative_result);
    };
    return Result<Choose, ModelError>(Choose(Span(choose_syntax.start, choose_syntax.end), condition, Vector<When>(rp, cases_builder), alternative));
}

Result<For, ModelError> model::handle_for(Page* rp, Page* ep, ForSyntax& for_syntax, String file) {
    auto r = Region();
    const auto _expression_result = handle_operands(rp, ep, for_syntax.operation, file);
    auto expression = _expression_result._Ok;
    if (_expression_result._tag == Success::Error) {
        const auto _expression_Error = _expression_result._Error;
        switch (_expression_Error._tag) {
        default:
            return Result<For, ModelError>(_expression_result._Error);

        }
    };
    if (for_syntax.name != nullptr) 
        return Result<For, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Label in For"), Span((*for_syntax.name).start, (*for_syntax.name).end)))));
    const auto _action_result = handle_action(rp, ep, for_syntax.action, file);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
        default:
            return Result<For, ModelError>(_action_result._Error);

        }
    };
    return Result<For, ModelError>(For(Span(for_syntax.start, for_syntax.end), String(rp, for_syntax.variable), expression, action));
}

Result<Binding, ModelError> model::handle_condition(Page* rp, Page* ep, ConditionSyntax& condition, String file) {
    auto r = Region();
    {
        auto _result = condition;
        switch (_result._tag)
        {
            case ConditionSyntax::Operation:
            {
                auto operation_syntax = _result._Operation;
                {
                    const auto _operation_result_result = handle_operation(rp, ep, operation_syntax, file);
                    auto operation_result = _operation_result_result._Ok;
                    if (_operation_result_result._tag == Success::Error) {
                        const auto _operation_result_Error = _operation_result_result._Error;
                        switch (_operation_result_Error._tag) {
                        default:
                            return Result<Binding, ModelError>(_operation_result_result._Error);

                        }
                    };
                    return Result<Binding, ModelError>(Binding(String(rp, "const"), Item(Span(operation_syntax.start, operation_syntax.end), false, nullptr, nullptr, Vector<Attribute>()), operation_result));
                };
                break;
            }
            case ConditionSyntax::Operation:
            {
                auto let_syntax = _result._Operation;
                {
                    Type* type = nullptr;
                    if (let_syntax.binding.annotation != nullptr) {
                        const auto _type_result_result = handle_binding_annotation(rp, ep, *let_syntax.binding.annotation, file);
                        auto type_result = _type_result_result._Ok;
                        if (_type_result_result._tag == Success::Error) {
                            const auto _type_result_Error = _type_result_result._Error;
                            switch (_type_result_Error._tag) {
                            default:
                                return Result<Binding, ModelError>(_type_result_result._Error);

                            }
                        };
                        type = type_result;
                    };
                    const auto _operation_result_result = handle_operands(rp, ep, let_syntax.binding.operation, file);
                    auto operation_result = _operation_result_result._Ok;
                    if (_operation_result_result._tag == Success::Error) {
                        const auto _operation_result_Error = _operation_result_result._Error;
                        switch (_operation_result_Error._tag) {
                        default:
                            return Result<Binding, ModelError>(_operation_result_result._Error);

                        }
                    };
                    return Result<Binding, ModelError>(Binding(String(rp, "const"), Item(Span(let_syntax.start, let_syntax.end), false, new (alignof(String), rp) String(rp, let_syntax.binding.name), nullptr, Vector<Attribute>()), operation_result));
                };
                break;
            }
        }
    };
}

Result<While, ModelError> handle_while(Page* rp, Page* ep, WhileSyntax& while_syntax, String file) {
    auto r = Region();
    const auto _condition_result = handle_condition(rp, ep, while_syntax.condition, file);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
        default:
            return Result<While, ModelError>(_condition_result._Error);

        }
    };
    if (while_syntax.name != nullptr) 
        return Result<While, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Label in While"), Span((*while_syntax.name).start, (*while_syntax.name).end)))));
    const auto _action_result = handle_action(rp, ep, while_syntax.action, file);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
        default:
            return Result<While, ModelError>(_action_result._Error);

        }
    };
    return Result<While, ModelError>(While(Span(while_syntax.start, while_syntax.end), condition, action));
}

Result<Try, ModelError> handle_try(Page* rp, Page* ep, TrySyntax& try_syntax, String file) {
    auto r = Region();
    const auto _condition_result = handle_condition(rp, ep, try_syntax.condition, file);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
        default:
            return Result<Try, ModelError>(_condition_result._Error);

        }
    };
    List<When>& catches_builder = *new (alignof(List<When>), r.get_page()) List<When>();
    if (try_syntax.cases != nullptr) {
        
        auto _catch_syntax_iterator = (*try_syntax.cases).get_iterator();
        while (auto _catch_syntax = _catch_syntax_iterator.next()) {
            auto catch_syntax = *_catch_syntax;{
                const auto _catch_result_result = handle_when(rp, ep, catch_syntax, file);
                auto catch_result = _catch_result_result._Ok;
                if (_catch_result_result._tag == Success::Error) {
                    const auto _catch_result_Error = _catch_result_result._Error;
                    switch (_catch_result_Error._tag) {
                    default:
                        return Result<Try, ModelError>(_catch_result_result._Error);

                    }
                };
                catches_builder.add(catch_result);
            }
        };
    };
    const Statement* dropper = nullptr;
    if (try_syntax.dropper != nullptr) {
        const auto drop_syntax = *try_syntax.dropper;
        const auto _drop_result_result = handle_command(rp, ep, drop_syntax.alternative, file);
        auto drop_result = _drop_result_result._Ok;
        if (_drop_result_result._tag == Success::Error) {
            const auto _drop_result_Error = _drop_result_result._Error;
            switch (_drop_result_Error._tag) {
            default:
                return Result<Try, ModelError>(_drop_result_result._Error);

            }
        };
        dropper = new (alignof(Statement), rp) Statement(drop_result);
    };
    return Result<Try, ModelError>(Try(Span(try_syntax.start, try_syntax.end), condition, Vector<When>(rp, catches_builder), dropper));
}

Result<SizeOf, ModelError> handle_size_of(Page* rp, Page* ep, SizeOfSyntax& size_of_syntax, String file) {
    auto r = Region();
    const auto _type_result = handle_type(rp, ep, size_of_syntax.type, file);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
        default:
            return Result<SizeOf, ModelError>(_type_result._Error);

        }
    };
    return Result<SizeOf, ModelError>(SizeOf(Span(size_of_syntax.start, size_of_syntax.end), *type));
}

Result<Expression, ModelError> handle_expression(Page* rp, Page* ep, ExpressionSyntax& expression, String file) {
    auto r = Region();
    {
        auto _result = expression;
        switch (_result._tag)
        {
            case ExpressionSyntax::Literal:
            {
                auto literal_syntax = _result._Literal;
                {
                    const auto _constant_result = handle_literal(rp, ep, literal_syntax, file);
                    auto constant = _constant_result._Ok;
                    if (_constant_result._tag == Success::Error) {
                        const auto _constant_Error = _constant_result._Error;
                        switch (_constant_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_constant_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(constant);
                };
                break;
            }
            case ExpressionSyntax::Type:
            {
                auto type_syntax = _result._Type;
                {
                    const auto _type_result_result = handle_type(rp, ep, type_syntax, file);
                    auto type_result = _type_result_result._Ok;
                    if (_type_result_result._tag == Success::Error) {
                        const auto _type_result_Error = _type_result_result._Error;
                        switch (_type_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_type_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(*type_result);
                };
                break;
            }
            case ExpressionSyntax::Object:
            {
                auto object_syntax = _result._Object;
                {
                    const auto _object_result_result = handle_object(rp, ep, object_syntax, file);
                    auto object_result = _object_result_result._Ok;
                    if (_object_result_result._tag == Success::Error) {
                        const auto _object_result_Error = _object_result_result._Error;
                        switch (_object_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_object_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(object_result);
                };
                break;
            }
            case ExpressionSyntax::Vector:
            {
                auto vector_syntax = _result._Vector;
                {
                    const auto _vector_result_result = handle_vector(rp, ep, vector_syntax, file);
                    auto vector_result = _vector_result_result._Ok;
                    if (_vector_result_result._tag == Success::Error) {
                        const auto _vector_result_Error = _vector_result_result._Error;
                        switch (_vector_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_vector_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(vector_result);
                };
                break;
            }
            case ExpressionSyntax::Block:
            {
                auto block_syntax = _result._Block;
                {
                    const auto _block_result_result = handle_block(rp, ep, block_syntax, file);
                    auto block_result = _block_result_result._Ok;
                    if (_block_result_result._tag == Success::Error) {
                        const auto _block_result_Error = _block_result_result._Error;
                        switch (_block_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_block_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(block_result);
                };
                break;
            }
            case ExpressionSyntax::If:
            {
                auto if_syntax = _result._If;
                {
                    const auto _if_result_result = handle_if(rp, ep, if_syntax, file);
                    auto if_result = _if_result_result._Ok;
                    if (_if_result_result._tag == Success::Error) {
                        const auto _if_result_Error = _if_result_result._Error;
                        switch (_if_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_if_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(if_result);
                };
                break;
            }
            case ExpressionSyntax::Match:
            {
                auto match_syntax = _result._Match;
                {
                    const auto _match_result_result = handle_match(rp, ep, match_syntax, file);
                    auto match_result = _match_result_result._Ok;
                    if (_match_result_result._tag == Success::Error) {
                        const auto _match_result_Error = _match_result_result._Error;
                        switch (_match_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_match_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(match_result);
                };
                break;
            }
            case ExpressionSyntax::Lambda:
            {
                auto lambda_syntax = _result._Lambda;
                return Result<Expression, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Lambda"), Span(lambda_syntax.start, lambda_syntax.end)))));
                break;
            }
            case ExpressionSyntax::For:
            {
                auto for_syntax = _result._For;
                {
                    const auto _for_result_result = handle_for(rp, ep, for_syntax, file);
                    auto for_result = _for_result_result._Ok;
                    if (_for_result_result._tag == Success::Error) {
                        const auto _for_result_Error = _for_result_result._Error;
                        switch (_for_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_for_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(for_result);
                };
                break;
            }
            case ExpressionSyntax::While:
            {
                auto while_syntax = _result._While;
                {
                    const auto _while_result_result = handle_while(rp, ep, while_syntax, file);
                    auto while_result = _while_result_result._Ok;
                    if (_while_result_result._tag == Success::Error) {
                        const auto _while_result_Error = _while_result_result._Error;
                        switch (_while_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_while_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(while_result);
                };
                break;
            }
            case ExpressionSyntax::Choose:
            {
                auto choose_syntax = _result._Choose;
                {
                    const auto _choose_result_result = handle_choose(rp, ep, choose_syntax, file);
                    auto choose_result = _choose_result_result._Ok;
                    if (_choose_result_result._tag == Success::Error) {
                        const auto _choose_result_Error = _choose_result_result._Error;
                        switch (_choose_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_choose_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(choose_result);
                };
                break;
            }
            case ExpressionSyntax::Try:
            {
                auto try_syntax = _result._Try;
                {
                    const auto _try_result_result = handle_try(rp, ep, try_syntax, file);
                    auto try_result = _try_result_result._Ok;
                    if (_try_result_result._tag == Success::Error) {
                        const auto _try_result_Error = _try_result_result._Error;
                        switch (_try_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_try_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(try_result);
                };
                break;
            }
            case ExpressionSyntax::Repeat:
            {
                auto repeat_syntax = _result._Repeat;
                return Result<Expression, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Repeat"), Span(repeat_syntax.start, repeat_syntax.end)))));
                break;
            }
            case ExpressionSyntax::SizeOf:
            {
                auto sizeof_syntax = _result._SizeOf;
                {
                    const auto _size_of_result_result = handle_size_of(rp, ep, sizeof_syntax, file);
                    auto size_of_result = _size_of_result_result._Ok;
                    if (_size_of_result_result._tag == Success::Error) {
                        const auto _size_of_result_Error = _size_of_result_result._Error;
                        switch (_size_of_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_size_of_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(size_of_result);
                };
                break;
            }
        }
    };
}

Result<Operand, ModelError> handle_operand(Page* rp, Page* ep, OperandSyntax& operand, String file) {
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

Result<Vector<Operand>, ModelError> handle_operands(Page* rp, Page* ep, Vector<OperandSyntax>* operands, String file) {
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