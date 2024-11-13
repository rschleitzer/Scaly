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
        initializer = new (alignof(Vector<Operand>), rp) Vector<Operand>(i);
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

Result<Item, ModelError> model::handle_item(Page* rp, Page* ep, bool private_, ItemSyntax& item, String file) {
    auto r = Region();
    Type* type = nullptr;
    if (item.annotation != nullptr) {
        const auto _type_result_result = handle_type(rp, ep, (*item.annotation).type, file);
        auto type_result = _type_result_result._Ok;
        if (_type_result_result._tag == Success::Error) {
            const auto _type_result_Error = _type_result_result._Error;
            switch (_type_result_Error._tag) {
            default:
                return Result<Item, ModelError>(_type_result_result._Error);

            }
        };
        type = type_result;
    };
    List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
    if (item.attributes != nullptr) {
        
        auto _attribute_syntax_iterator = (*item.attributes).get_iterator();
        while (auto _attribute_syntax = _attribute_syntax_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;{
                const auto _attribute_result = handle_attribute(rp, ep, attribute_syntax, file);
                auto attribute = _attribute_result._Ok;
                if (_attribute_result._tag == Success::Error) {
                    const auto _attribute_Error = _attribute_result._Error;
                    switch (_attribute_Error._tag) {
                    default:
                        return Result<Item, ModelError>(_attribute_result._Error);

                    }
                };
                attributes.add(attribute);
            }
        };
    };
    return Result<Item, ModelError>(Item(Span(item.start, item.end), false, new (alignof(String), rp) String(rp, item.name), type, Vector<Attribute>(rp, attributes)));
}

Result<Vector<Item>, ModelError> model::handle_parameterset(Page* rp, Page* ep, ParameterSetSyntax& parameterSetSyntax, String file) {
    auto r = Region();
    List<Item>& items = *new (alignof(List<Item>), r.get_page()) List<Item>();
    {
        auto _result = parameterSetSyntax;
        switch (_result._tag)
        {
            case ParameterSetSyntax::Parameters:
            {
                auto parameters_syntax = _result._Parameters;
                {
                    if (parameters_syntax.items != nullptr) {
                        
                        auto _item_syntax_iterator = (*parameters_syntax.items).get_iterator();
                        while (auto _item_syntax = _item_syntax_iterator.next()) {
                            auto item_syntax = *_item_syntax;{
                                const auto _item_result = handle_item(rp, ep, false, item_syntax, file);
                                auto item = _item_result._Ok;
                                if (_item_result._tag == Success::Error) {
                                    const auto _item_Error = _item_result._Error;
                                    switch (_item_Error._tag) {
                                    default:
                                        return Result<Vector<Item>, ModelError>(_item_result._Error);

                                    }
                                };
                                items.add(item);
                            }
                        };
                    };
                };
                break;
            }
            case ParameterSetSyntax::Type:
            {
                auto type_syntax = _result._Type;
                {
                    const auto _type_result = handle_type(rp, ep, type_syntax, file);
                    auto type = _type_result._Ok;
                    if (_type_result._tag == Success::Error) {
                        const auto _type_Error = _type_result._Error;
                        switch (_type_Error._tag) {
                        default:
                            return Result<Vector<Item>, ModelError>(_type_result._Error);

                        }
                    };
                    items.add(Item(Span(type_syntax.start, type_syntax.end), false, nullptr, type, Vector<Attribute>()));
                };
                break;
            }
        }
    };
    return Result<Vector<Item>, ModelError>(Vector<Item>(rp, items));
}

Result<DeInitializer*, ModelError> model::handle_body(Page* rp, Page* ep, String name, String path, BodySyntax& body, List<Use>& uses, HashSetBuilder<String>& modules_checker, List<Module>& modules, List<Initializer>& initializers_builder, MultiMapBuilder<String, Function>& functions_builder, Array<Member>& members_builder, HashMapBuilder<String, Nameable>& symbols_builder, String file) {
    auto r = Region();
    if (body.uses != nullptr) {
        
        auto _use_syntax_iterator = (*body.uses).get_iterator();
        while (auto _use_syntax = _use_syntax_iterator.next()) {
            auto use_syntax = *_use_syntax;{
                const auto _use_result_result = handle_use(rp, ep, use_syntax);
                auto use_result = _use_result_result._Ok;
                if (_use_result_result._tag == Success::Error) {
                    const auto _use_result_Error = _use_result_result._Error;
                    switch (_use_result_Error._tag) {
                    default:
                        return Result<DeInitializer*, ModelError>(_use_result_result._Error);

                    }
                };
                uses.add(use_result);
            }
        };
    };
    if ((body.inits != nullptr)) {
        
        auto _initializer_iterator = (*body.inits).get_iterator();
        while (auto _initializer = _initializer_iterator.next()) {
            auto initializer = *_initializer;{
                const auto _initializer_result_result = handle_initializer(rp, ep, initializer, false, file);
                auto initializer_result = _initializer_result_result._Ok;
                if (_initializer_result_result._tag == Success::Error) {
                    const auto _initializer_result_Error = _initializer_result_result._Error;
                    switch (_initializer_result_Error._tag) {
                    default:
                        return Result<DeInitializer*, ModelError>(_initializer_result_result._Error);

                    }
                };
                initializers_builder.add(initializer_result);
            }
        };
    };
    DeInitializer* de_initializer = nullptr;
    if (body.deInit != nullptr) {
        const auto _de_initializer_result_result = handle_deinitializer(rp, ep, *body.deInit, file);
        auto de_initializer_result = _de_initializer_result_result._Ok;
        if (_de_initializer_result_result._tag == Success::Error) {
            const auto _de_initializer_result_Error = _de_initializer_result_result._Error;
            switch (_de_initializer_result_Error._tag) {
            default:
                return Result<DeInitializer*, ModelError>(_de_initializer_result_result._Error);

            }
        };
        de_initializer = de_initializer_result;
    };
    if (body.members != nullptr) {
        
        auto _member_iterator = (*body.members).get_iterator();
        while (auto _member = _member_iterator.next()) {
            auto member = *_member;{
                {
                    auto _result = member.constituent;
                    switch (_result._tag)
                    {
                        case ConstituentSyntax::Definition:
                        {
                            auto definition_syntax = _result._Definition;
                            {
                                const auto _concept_result = handle_definition(rp, ep, path, definition_syntax, false, file);
                                auto concept = _concept_result._Ok;
                                if (_concept_result._tag == Success::Error) {
                                    const auto _concept_Error = _concept_result._Error;
                                    switch (_concept_Error._tag) {
                                    default:
                                        return Result<DeInitializer*, ModelError>(_concept_result._Error);

                                    }
                                };
                                if ((functions_builder.contains(concept.name))) 
                                    return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(definition_syntax.start, definition_syntax.end)))));
                                if (symbols_builder.add(concept.name, Nameable(concept)) == false) 
                                    return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(DuplicateName(concept.name, Span(definition_syntax.start, definition_syntax.end)))));
                                members_builder.add(Member(concept));
                            };
                            break;
                        }
                        case ConstituentSyntax::Function:
                        {
                            auto function_syntax = _result._Function;
                            {
                                const auto _function_result_result = build_function(rp, ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file);
                                auto function_result = _function_result_result._Ok;
                                if (_function_result_result._tag == Success::Error) {
                                    const auto _function_result_Error = _function_result_result._Error;
                                    switch (_function_result_Error._tag) {
                                    default:
                                        return Result<DeInitializer*, ModelError>(_function_result_result._Error);

                                    }
                                };
                                const auto symbol_with_function_name = symbols_builder.get(function_result.name);
                                if (symbol_with_function_name != nullptr) 
                                    return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(function_syntax.start, function_syntax.end)))));
                                functions_builder.add(function_result.name, function_result);
                                members_builder.add(Member(function_result));
                            };
                            break;
                        }
                        case ConstituentSyntax::Procedure:
                        {
                            auto procedure_syntax = _result._Procedure;
                            {
                                const auto _procedure_result_result = build_function(rp, ep, procedure_syntax.start, procedure_syntax.end, procedure_syntax.target, false, false, file);
                                auto procedure_result = _procedure_result_result._Ok;
                                if (_procedure_result_result._tag == Success::Error) {
                                    const auto _procedure_result_Error = _procedure_result_result._Error;
                                    switch (_procedure_result_Error._tag) {
                                    default:
                                        return Result<DeInitializer*, ModelError>(_procedure_result_result._Error);

                                    }
                                };
                                const auto symbol_with_function_name = symbols_builder.get(procedure_result.name);
                                if (symbol_with_function_name != nullptr) 
                                    return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(procedure_syntax.start, procedure_syntax.end)))));
                                functions_builder.add(procedure_result.name, procedure_result);
                                members_builder.add(Member(procedure_result));
                            };
                            break;
                        }
                        case ConstituentSyntax::Operator:
                        {
                            auto operator_syntax = _result._Operator;
                            {
                                const auto _operator_result_result = handle_operator(rp, ep, operator_syntax, false, file);
                                auto operator_result = _operator_result_result._Ok;
                                if (_operator_result_result._tag == Success::Error) {
                                    const auto _operator_result_Error = _operator_result_result._Error;
                                    switch (_operator_result_Error._tag) {
                                    default:
                                        return Result<DeInitializer*, ModelError>(_operator_result_result._Error);

                                    }
                                };
                                if (symbols_builder.add(operator_result.name, operator_result) == false) 
                                    return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))));
                                members_builder.add(Member(operator_result));
                            };
                            break;
                        }
                        case ConstituentSyntax::Implement:
                        {
                            auto implement_syntax = _result._Implement;
                            return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Implement"), Span(implement_syntax.start, implement_syntax.end)))));
                            break;
                        }
                        case ConstituentSyntax::Trait:
                        {
                            auto trait_syntax = _result._Trait;
                            return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Trait"), Span(trait_syntax.start, trait_syntax.end)))));
                            break;
                        }
                        case ConstituentSyntax::Macro:
                        {
                            auto macro_syntax = _result._Macro;
                            return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Macro"), Span(macro_syntax.start, macro_syntax.end)))));
                            break;
                        }
                        case ConstituentSyntax::Module:
                        {
                            auto module_syntax = _result._Module;
                            {
                                const auto _module_result_result = handle_module(rp, ep, path, module_syntax, false);
                                auto module_result = _module_result_result._Ok;
                                if (_module_result_result._tag == Success::Error) {
                                    const auto _module_result_Error = _module_result_result._Error;
                                    switch (_module_result_Error._tag) {
                                    default:
                                        return Result<DeInitializer*, ModelError>(_module_result_result._Error);

                                    }
                                };
                                if (modules_checker.add(module_result.name) == false) 
                                    return Result<DeInitializer*, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))));
                                modules.add(module_result);
                            };
                            break;
                        }
                    }
                };
            }
        };
    };
    auto multi_map = MultiMap<String, Function>(r.get_page(), functions_builder);
    
    auto _functions_iterator = multi_map.get_iterator();
    while (auto _functions = _functions_iterator.next()) {
        auto functions = *_functions;{
            const auto name = (*functions.get(0)).name;
            symbols_builder.add(String(name), Nameable(Vector<Function>(rp, functions)));
        }
    };
    return Result<DeInitializer*, ModelError>(de_initializer);
}

Result<Structure, ModelError> model::handle_class(Page* rp, Page* ep, String name, String path, ClassSyntax& class_syntax, bool private_, String file) {
    auto r = Region();
    HashMapBuilder<String, Nameable>& symbols_builder = *new (alignof(HashMapBuilder<String, Nameable>), r.get_page()) HashMapBuilder<String, Nameable>();
    Array<Property>& properties_builder = *new (alignof(Array<Property>), r.get_page()) Array<Property>();
    if (class_syntax.structure.parts != nullptr) {
        
        auto _part_iterator = (*class_syntax.structure.parts).get_iterator();
        while (auto _part = _part_iterator.next()) {
            auto part = *_part;{
                {
                    auto _result = part;
                    switch (_result._tag)
                    {
                        case PartSyntax::Field:
                        {
                            auto field_syntax = _result._Field;
                            {
                                const auto _property_result = handle_property(rp, ep, true, field_syntax.property, file);
                                auto property = _property_result._Ok;
                                if (_property_result._tag == Success::Error) {
                                    const auto _property_Error = _property_result._Error;
                                    switch (_property_Error._tag) {
                                    default:
                                        return Result<Structure, ModelError>(_property_result._Error);

                                    }
                                };
                                if (symbols_builder.add(property.name, property) == false) 
                                    return Result<Structure, ModelError>(ModelError(ModelBuilderError(DuplicateName(property.name, Span(field_syntax.start, field_syntax.end)))));
                                properties_builder.add(property);
                            };
                            break;
                        }
                        case PartSyntax::Property:
                        {
                            auto property_syntax = _result._Property;
                            {
                                const auto _property_result = handle_property(rp, ep, false, property_syntax, file);
                                auto property = _property_result._Ok;
                                if (_property_result._tag == Success::Error) {
                                    const auto _property_Error = _property_result._Error;
                                    switch (_property_Error._tag) {
                                    default:
                                        return Result<Structure, ModelError>(_property_result._Error);

                                    }
                                };
                                if (symbols_builder.add(property.name, property) == false) 
                                    return Result<Structure, ModelError>(ModelError(ModelBuilderError(DuplicateName(property.name, Span(property_syntax.start, property_syntax.end)))));
                                properties_builder.add(property);
                            };
                            break;
                        }
                    }
                };
            }
        };
    };
    HashSetBuilder<String>& modules_checker = *new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new (alignof(List<Module>), r.get_page()) List<Module>();
    List<Use>& uses = *new (alignof(List<Use>), r.get_page()) List<Use>();
    MultiMapBuilder<String, Function>& functions_builder = *new (alignof(MultiMapBuilder<String, Function>), r.get_page()) MultiMapBuilder<String, Function>();
    List<Initializer>& initializers_builder = *new (alignof(List<Initializer>), r.get_page()) List<Initializer>();
    Array<Member>& members_builder = *new (alignof(Array<Member>), r.get_page()) Array<Member>();
    DeInitializer* de_initializer = nullptr;
    if (class_syntax.body != nullptr) {
        const auto _de_initializer_result_result = handle_body(rp, ep, name, path, *class_syntax.body, uses, modules_checker, modules, initializers_builder, functions_builder, members_builder, symbols_builder, file);
        auto de_initializer_result = _de_initializer_result_result._Ok;
        if (_de_initializer_result_result._tag == Success::Error) {
            const auto _de_initializer_result_Error = _de_initializer_result_result._Error;
            switch (_de_initializer_result_Error._tag) {
            default:
                return Result<Structure, ModelError>(_de_initializer_result_result._Error);

            }
        };
        de_initializer = de_initializer_result;
    };
    return Result<Structure, ModelError>(Structure(Span(class_syntax.start, class_syntax.end), private_, Vector<Property>(rp, properties_builder), Vector<Module>(rp, modules), Vector<Use>(rp, uses), Vector<Initializer>(rp, initializers_builder), de_initializer, Vector<Member>(rp, members_builder), HashMap<String, Nameable>(rp, symbols_builder)));
}

Result<Namespace, ModelError> model::handle_namespace(Page* rp, Page* ep, String name, String path, NamespaceSyntax& namespace_syntax, bool private_, String file) {
    auto r = Region();
    HashMapBuilder<String, Nameable>& symbols_builder = *new (alignof(HashMapBuilder<String, Nameable>), r.get_page()) HashMapBuilder<String, Nameable>();
    Array<Member>& members_builder = *new (alignof(Array<Member>), r.get_page()) Array<Member>();
    MultiMapBuilder<String, Function>& functions_builder = *new (alignof(MultiMapBuilder<String, Function>), r.get_page()) MultiMapBuilder<String, Function>();
    HashSetBuilder<String>& modules_checker = *new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new (alignof(List<Module>), r.get_page()) List<Module>();
    if (namespace_syntax.declarations != nullptr) {
        
        auto _declaration_iterator = (*namespace_syntax.declarations).get_iterator();
        while (auto _declaration = _declaration_iterator.next()) {
            auto declaration = *_declaration;{
                {
                    auto _result = declaration.symbol;
                    switch (_result._tag)
                    {
                        case SymbolSyntax::Private:
                        {
                            auto private_syntax = _result._Private;
                            {
                                const auto export_syntax = private_syntax.export_;
                                {
                                    auto _result = export_syntax;
                                    switch (_result._tag)
                                    {
                                        case ExportSyntax::Definition:
                                        {
                                            auto definition_syntax = _result._Definition;
                                            {
                                                const auto _concept_result = handle_definition(rp, ep, path, definition_syntax, true, file);
                                                auto concept = _concept_result._Ok;
                                                if (_concept_result._tag == Success::Error) {
                                                    const auto _concept_Error = _concept_result._Error;
                                                    switch (_concept_Error._tag) {
                                                    default:
                                                        return Result<Namespace, ModelError>(_concept_result._Error);

                                                    }
                                                };
                                                if (functions_builder.contains(concept.name)) 
                                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(definition_syntax.start, definition_syntax.end)))));
                                                if (symbols_builder.add(concept.name, Nameable(concept)) == false) 
                                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(DuplicateName(concept.name, Span(definition_syntax.start, definition_syntax.end)))));
                                                members_builder.add(Member(concept));
                                            };
                                            break;
                                        }
                                        case ExportSyntax::Function:
                                        {
                                            auto function_syntax = _result._Function;
                                            {
                                                const auto _function_result_result = build_function(rp, ep, function_syntax.start, function_syntax.end, function_syntax.target, true, true, file);
                                                auto function_result = _function_result_result._Ok;
                                                if (_function_result_result._tag == Success::Error) {
                                                    const auto _function_result_Error = _function_result_result._Error;
                                                    switch (_function_result_Error._tag) {
                                                    default:
                                                        return Result<Namespace, ModelError>(_function_result_result._Error);

                                                    }
                                                };
                                                const auto symbol_with_function_name = symbols_builder.get(function_result.name);
                                                if (symbol_with_function_name != nullptr) 
                                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(function_syntax.start, function_syntax.end)))));
                                                functions_builder.add(function_result.name, function_result);
                                                members_builder.add(Member(function_result));
                                            };
                                            break;
                                        }
                                        case ExportSyntax::Operator:
                                        {
                                            auto operator_syntax = _result._Operator;
                                            {
                                                const auto _operator_result_result = handle_operator(rp, ep, operator_syntax, true, file);
                                                auto operator_result = _operator_result_result._Ok;
                                                if (_operator_result_result._tag == Success::Error) {
                                                    const auto _operator_result_Error = _operator_result_result._Error;
                                                    switch (_operator_result_Error._tag) {
                                                    default:
                                                        return Result<Namespace, ModelError>(_operator_result_result._Error);

                                                    }
                                                };
                                                if (symbols_builder.add(operator_result.name, Nameable(operator_result)) == false) 
                                                    return Result<Namespace, ModelError>(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end))));
                                                members_builder.add(Member(operator_result));
                                            };
                                            break;
                                        }
                                        case ExportSyntax::Trait:
                                        {
                                            auto trait_syntax = _result._Trait;
                                            return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Private Trait"), Span(trait_syntax.start, trait_syntax.end)))));
                                            break;
                                        }
                                        case ExportSyntax::Module:
                                        {
                                            auto module_syntax = _result._Module;
                                            {
                                                const auto _module_result_result = handle_module(rp, ep, path, module_syntax, true);
                                                auto module_result = _module_result_result._Ok;
                                                if (_module_result_result._tag == Success::Error) {
                                                    const auto _module_result_Error = _module_result_result._Error;
                                                    switch (_module_result_Error._tag) {
                                                    default:
                                                        return Result<Namespace, ModelError>(_module_result_result._Error);

                                                    }
                                                };
                                                if (modules_checker.add(module_result.name) == false) 
                                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))));
                                                modules.add(module_result);
                                            };
                                            break;
                                        }
                                    }
                                };
                            };
                            break;
                        }
                        case SymbolSyntax::Definition:
                        {
                            auto definition_syntax = _result._Definition;
                            {
                                const auto _concept_result = handle_definition(rp, ep, path, definition_syntax, true, file);
                                auto concept = _concept_result._Ok;
                                if (_concept_result._tag == Success::Error) {
                                    const auto _concept_Error = _concept_result._Error;
                                    switch (_concept_Error._tag) {
                                    default:
                                        return Result<Namespace, ModelError>(_concept_result._Error);

                                    }
                                };
                                if (functions_builder.contains(concept.name)) 
                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(definition_syntax.start, definition_syntax.end)))));
                                if (symbols_builder.add(concept.name, Nameable(concept)) == false) 
                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(DuplicateName(concept.name, Span(definition_syntax.start, definition_syntax.end)))));
                                members_builder.add(Member(concept));
                            };
                            break;
                        }
                        case SymbolSyntax::Function:
                        {
                            auto function_syntax = _result._Function;
                            {
                                const auto _function_result_result = build_function(rp, ep, function_syntax.start, function_syntax.end, function_syntax.target, true, true, file);
                                auto function_result = _function_result_result._Ok;
                                if (_function_result_result._tag == Success::Error) {
                                    const auto _function_result_Error = _function_result_result._Error;
                                    switch (_function_result_Error._tag) {
                                    default:
                                        return Result<Namespace, ModelError>(_function_result_result._Error);

                                    }
                                };
                                const auto symbol_with_function_name = symbols_builder.get(function_result.name);
                                if (symbol_with_function_name != nullptr) 
                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(function_syntax.start, function_syntax.end)))));
                                functions_builder.add(function_result.name, function_result);
                                members_builder.add(Member(function_result));
                            };
                            break;
                        }
                        case SymbolSyntax::Operator:
                        {
                            auto operator_syntax = _result._Operator;
                            {
                                const auto _operator_result_result = handle_operator(rp, ep, operator_syntax, true, file);
                                auto operator_result = _operator_result_result._Ok;
                                if (_operator_result_result._tag == Success::Error) {
                                    const auto _operator_result_Error = _operator_result_result._Error;
                                    switch (_operator_result_Error._tag) {
                                    default:
                                        return Result<Namespace, ModelError>(_operator_result_result._Error);

                                    }
                                };
                                if (symbols_builder.add(operator_result.name, Nameable(operator_result)) == false) 
                                    return Result<Namespace, ModelError>(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end))));
                                members_builder.add(Member(operator_result));
                            };
                            break;
                        }
                        case SymbolSyntax::Trait:
                        {
                            auto trait_syntax = _result._Trait;
                            return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Private Trait"), Span(trait_syntax.start, trait_syntax.end)))));
                            break;
                        }
                        case SymbolSyntax::Macro:
                        {
                            auto macro_syntax = _result._Macro;
                            return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Macro"), Span(macro_syntax.start, macro_syntax.end)))));
                            break;
                        }
                        case SymbolSyntax::Module:
                        {
                            auto module_syntax = _result._Module;
                            {
                                const auto _module_result_result = handle_module(rp, ep, path, module_syntax, true);
                                auto module_result = _module_result_result._Ok;
                                if (_module_result_result._tag == Success::Error) {
                                    const auto _module_result_Error = _module_result_result._Error;
                                    switch (_module_result_Error._tag) {
                                    default:
                                        return Result<Namespace, ModelError>(_module_result_result._Error);

                                    }
                                };
                                if (modules_checker.add(module_result.name) == false) 
                                    return Result<Namespace, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))));
                                modules.add(module_result);
                            };
                            break;
                        }
                    }
                };
            }
        };
    };
    auto multi_map = MultiMap<String, Function>(r.get_page(), functions_builder);
    
    auto _functions_iterator = multi_map.get_iterator();
    while (auto _functions = _functions_iterator.next()) {
        auto functions = *_functions;{
            const auto name = (*functions.get(0)).name;
            symbols_builder.add(String(name), Nameable(Vector<Function>(rp, functions)));
        }
    };
    Span(namespace_syntax.start, namespace_syntax.end);
    return Result<Namespace, ModelError>(Namespace(Span(namespace_syntax.start, namespace_syntax.end), Vector<Module>(rp, modules), Vector<Member>(rp, members_builder), HashMap<String, Nameable>(rp, symbols_builder)));
}

Result<Union, ModelError> model::handle_union(Page* rp, Page* ep, String name, String path, UnionSyntax& union_syntax, bool private_, String file) {
    auto r = Region();
    Array<Variant>& variants_builder = *new (alignof(Array<Variant>), r.get_page()) Array<Variant>();
    HashMapBuilder<String, Nameable>& symbols_builder = *new (alignof(HashMapBuilder<String, Nameable>), r.get_page()) HashMapBuilder<String, Nameable>();
    if (union_syntax.variants != nullptr) {
        
        auto _variant_syntax_iterator = (*union_syntax.variants).get_iterator();
        while (auto _variant_syntax = _variant_syntax_iterator.next()) {
            auto variant_syntax = *_variant_syntax;{
                Type* type = nullptr;
                if (variant_syntax.annotation != nullptr) {
                    const auto _type_result_result = handle_type(rp, ep, (*variant_syntax.annotation).type, file);
                    auto type_result = _type_result_result._Ok;
                    if (_type_result_result._tag == Success::Error) {
                        const auto _type_result_Error = _type_result_result._Error;
                        switch (_type_result_Error._tag) {
                        default:
                            return Result<Union, ModelError>(_type_result_result._Error);

                        }
                    };
                    type = type_result;
                };
                List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
                if (variant_syntax.attributes != nullptr) {
                    
                    auto _attribute_syntax_iterator = (*variant_syntax.attributes).get_iterator();
                    while (auto _attribute_syntax = _attribute_syntax_iterator.next()) {
                        auto attribute_syntax = *_attribute_syntax;{
                            const auto _attribute_result = handle_attribute(rp, ep, attribute_syntax, file);
                            auto attribute = _attribute_result._Ok;
                            if (_attribute_result._tag == Success::Error) {
                                const auto _attribute_Error = _attribute_result._Error;
                                switch (_attribute_Error._tag) {
                                default:
                                    return Result<Union, ModelError>(_attribute_result._Error);

                                }
                            };
                            attributes.add(attribute);
                        }
                    };
                };
                const auto variant = Variant(Span(variant_syntax.start, variant_syntax.end), variant_syntax.name, type, Vector<Attribute>(rp, attributes));
                if (symbols_builder.add(variant_syntax.name, Nameable(variant)) == false) 
                    return Result<Union, ModelError>(ModelError(ModelBuilderError(DuplicateName(variant_syntax.name, Span(variant_syntax.start, variant_syntax.end)))));
                variants_builder.add(variant);
            }
        };
    };
    HashSetBuilder<String>& modules_checker = *new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new (alignof(List<Module>), r.get_page()) List<Module>();
    List<Use>& uses = *new (alignof(List<Use>), r.get_page()) List<Use>();
    MultiMapBuilder<String, Function>& functions_builder = *new (alignof(MultiMapBuilder<String, Function>), r.get_page()) MultiMapBuilder<String, Function>();
    List<Initializer>& initializers_builder = *new (alignof(List<Initializer>), r.get_page()) List<Initializer>();
    Array<Member>& members_builder = *new (alignof(Array<Member>), r.get_page()) Array<Member>();
    DeInitializer* de_initializer = nullptr;
    if ((union_syntax.body != nullptr)) {
        const auto _de_initializer_result_result = handle_body(rp, ep, name, path, *union_syntax.body, uses, modules_checker, modules, initializers_builder, functions_builder, members_builder, symbols_builder, file);
        auto de_initializer_result = _de_initializer_result_result._Ok;
        if (_de_initializer_result_result._tag == Success::Error) {
            const auto _de_initializer_result_Error = _de_initializer_result_result._Error;
            switch (_de_initializer_result_Error._tag) {
            default:
                return Result<Union, ModelError>(_de_initializer_result_result._Error);

            }
        };
        de_initializer = de_initializer_result;
    };
    return Result<Union, ModelError>(Union(Span(union_syntax.start, union_syntax.end), private_, Vector<Variant>(rp, variants_builder), Vector<Member>(rp, members_builder), HashMap<String, Nameable>(rp, symbols_builder)));
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
                return Result<Constant, ModelError>(Constant(StringConstant(string.value)));
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

Result<Type*, ModelError> model::handle_binding_annotation(Page* rp, Page* ep, BindingAnnotationSyntax& binding_annotation, String file) {
    {
        auto _result = binding_annotation.spec;
        switch (_result._tag)
        {
            case BindingSpecSyntax::Type:
            {
                auto type = _result._Type;
                {
                    const auto _type_result_result = handle_type(rp, ep, type, file);
                    auto type_result = _type_result_result._Ok;
                    if (_type_result_result._tag == Success::Error) {
                        const auto _type_result_Error = _type_result_result._Error;
                        switch (_type_result_Error._tag) {
                        default:
                            return Result<Type*, ModelError>(_type_result_result._Error);

                        }
                    };
                    return Result<Type*, ModelError>(type_result);
                };
                break;
            }
            case BindingSpecSyntax::Structure:
            {
                auto structure = _result._Structure;
                return Result<Type*, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Structure"), Span(structure.start, structure.end)))));
                break;
            }
            case BindingSpecSyntax::Array:
            {
                auto array = _result._Array;
                return Result<Type*, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Array"), Span(array.start, array.end)))));
                break;
            }
        }
    };
}

Result<Break, ModelError> model::handle_break(Page* rp, Page* ep, BreakSyntax& break_syntax, String file) {
    auto r = Region();
    if (break_syntax.result != nullptr) {
        const auto _result_result = handle_operands(rp, ep, break_syntax.result, file);
        auto result = _result_result._Ok;
        if (_result_result._tag == Success::Error) {
            const auto _result_Error = _result_result._Error;
            switch (_result_Error._tag) {
            default:
                return Result<Break, ModelError>(_result_result._Error);

            }
        };
        return Result<Break, ModelError>(Break(Span(break_syntax.start, break_syntax.end), result));
    };
    return Result<Break, ModelError>(Break(Span(break_syntax.start, break_syntax.end), Vector<Operand>()));
}

Result<Continue, ModelError> model::handle_continue(Page* rp, Page* ep, ContinueSyntax& continue_syntax, String file) {
    return Result<Continue, ModelError>(Continue(Span(continue_syntax.start, continue_syntax.end)));
}

Result<Return, ModelError> model::handle_return(Page* rp, Page* ep, ReturnSyntax& return_syntax, String file) {
    auto r = Region();
    if (return_syntax.result != nullptr) {
        const auto _result_result = handle_operands(rp, ep, return_syntax.result, file);
        auto result = _result_result._Ok;
        if (_result_result._tag == Success::Error) {
            const auto _result_Error = _result_result._Error;
            switch (_result_Error._tag) {
            default:
                return Result<Return, ModelError>(_result_result._Error);

            }
        };
        return Result<Return, ModelError>(Return(Span(return_syntax.start, return_syntax.end), result));
    };
    return Result<Return, ModelError>(Return(Span(return_syntax.start, return_syntax.end), Vector<Operand>()));
}

Result<Throw, ModelError> model::handle_throw(Page* rp, Page* ep, ThrowSyntax& throw_syntax, String file) {
    auto r = Region();
    if (throw_syntax.result != nullptr) {
        const auto _result_result = handle_operands(rp, ep, throw_syntax.result, file);
        auto result = _result_result._Ok;
        if (_result_result._tag == Success::Error) {
            const auto _result_Error = _result_result._Error;
            switch (_result_Error._tag) {
            default:
                return Result<Throw, ModelError>(_result_result._Error);

            }
        };
        return Result<Throw, ModelError>(Throw(Span(throw_syntax.start, throw_syntax.end), result));
    };
    return Result<Throw, ModelError>(Throw(Span(throw_syntax.start, throw_syntax.end), Vector<Operand>()));
}

Result<Statement, ModelError> model::handle_command(Page* rp, Page* ep, CommandSyntax& command_syntax, String file) {
    auto r = Region();
    {
        auto _result = command_syntax;
        switch (_result._tag)
        {
            case CommandSyntax::Operation:
            {
                auto operation_syntax = _result._Operation;
                {
                    const auto _operation_result_result = handle_operation(rp, ep, operation_syntax, file);
                    auto operation_result = _operation_result_result._Ok;
                    if (_operation_result_result._tag == Success::Error) {
                        const auto _operation_result_Error = _operation_result_result._Error;
                        switch (_operation_result_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_operation_result_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(Action(operation_result, Vector<Operand>())));
                };
                break;
            }
            case CommandSyntax::Let:
            {
                auto let_syntax = _result._Let;
                {
                    Type* type = nullptr;
                    if (let_syntax.binding.annotation != nullptr) {
                        const auto _type_result_result = handle_binding_annotation(rp, ep, *let_syntax.binding.annotation, file);
                        auto type_result = _type_result_result._Ok;
                        if (_type_result_result._tag == Success::Error) {
                            const auto _type_result_Error = _type_result_result._Error;
                            switch (_type_result_Error._tag) {
                            default:
                                return Result<Statement, ModelError>(_type_result_result._Error);

                            }
                        };
                        type = type_result;
                    };
                    const auto _operation_result = handle_operands(rp, ep, let_syntax.binding.operation, file);
                    auto operation = _operation_result._Ok;
                    if (_operation_result._tag == Success::Error) {
                        const auto _operation_Error = _operation_result._Error;
                        switch (_operation_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_operation_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(Binding(String(rp, "const"), Item(Span(let_syntax.start, let_syntax.end), false, new (alignof(String), rp) String(rp, let_syntax.binding.name), type, Vector<Attribute>()), operation)));
                };
                break;
            }
            case CommandSyntax::Var:
            {
                auto var_syntax = _result._Var;
                {
                    Type* type = nullptr;
                    if (var_syntax.binding.annotation != nullptr) {
                        const auto _type_result_result = handle_binding_annotation(rp, ep, *var_syntax.binding.annotation, file);
                        auto type_result = _type_result_result._Ok;
                        if (_type_result_result._tag == Success::Error) {
                            const auto _type_result_Error = _type_result_result._Error;
                            switch (_type_result_Error._tag) {
                            default:
                                return Result<Statement, ModelError>(_type_result_result._Error);

                            }
                        };
                        type = type_result;
                    };
                    const auto _operation_result = handle_operands(rp, ep, var_syntax.binding.operation, file);
                    auto operation = _operation_result._Ok;
                    if (_operation_result._tag == Success::Error) {
                        const auto _operation_Error = _operation_result._Error;
                        switch (_operation_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_operation_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(Binding(String(rp, "var"), Item(Span(var_syntax.start, var_syntax.end), false, new (alignof(String), rp) String(rp, var_syntax.binding.name), type, Vector<Attribute>()), operation)));
                };
                break;
            }
            case CommandSyntax::Mutable:
            {
                auto mutable_syntax = _result._Mutable;
                {
                    Type* type = nullptr;
                    if (mutable_syntax.binding.annotation != nullptr) {
                        const auto _type_result_result = handle_binding_annotation(rp, ep, *mutable_syntax.binding.annotation, file);
                        auto type_result = _type_result_result._Ok;
                        if (_type_result_result._tag == Success::Error) {
                            const auto _type_result_Error = _type_result_result._Error;
                            switch (_type_result_Error._tag) {
                            default:
                                return Result<Statement, ModelError>(_type_result_result._Error);

                            }
                        };
                        type = type_result;
                    };
                    const auto _operation_result = handle_operands(rp, ep, mutable_syntax.binding.operation, file);
                    auto operation = _operation_result._Ok;
                    if (_operation_result._tag == Success::Error) {
                        const auto _operation_Error = _operation_result._Error;
                        switch (_operation_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_operation_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(Binding(String(rp, "mutable"), Item(Span(mutable_syntax.start, mutable_syntax.end), false, new (alignof(String), rp) String(rp, mutable_syntax.binding.name), type, Vector<Attribute>()), operation)));
                };
                break;
            }
            case CommandSyntax::Set:
            {
                auto set_syntax = _result._Set;
                {
                    const auto _target_result = handle_operands(rp, ep, set_syntax.target, file);
                    auto target = _target_result._Ok;
                    if (_target_result._tag == Success::Error) {
                        const auto _target_Error = _target_result._Error;
                        switch (_target_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_target_result._Error);

                        }
                    };
                    const auto _source_result = handle_operands(rp, ep, set_syntax.source, file);
                    auto source = _source_result._Ok;
                    if (_source_result._tag == Success::Error) {
                        const auto _source_Error = _source_result._Error;
                        switch (_source_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_source_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(Action(source, target)));
                };
                break;
            }
            case CommandSyntax::Continue:
            {
                auto continue_syntax = _result._Continue;
                {
                    const auto _continue_result_result = handle_continue(rp, ep, continue_syntax, file);
                    auto continue_result = _continue_result_result._Ok;
                    if (_continue_result_result._tag == Success::Error) {
                        const auto _continue_result_Error = _continue_result_result._Error;
                        switch (_continue_result_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_continue_result_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(continue_result));
                };
                break;
            }
            case CommandSyntax::Break:
            {
                auto break_syntax = _result._Break;
                {
                    const auto _break_result_result = handle_break(rp, ep, break_syntax, file);
                    auto break_result = _break_result_result._Ok;
                    if (_break_result_result._tag == Success::Error) {
                        const auto _break_result_Error = _break_result_result._Error;
                        switch (_break_result_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_break_result_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(break_result));
                };
                break;
            }
            case CommandSyntax::Return:
            {
                auto return_syntax = _result._Return;
                {
                    const auto _return_result_result = handle_return(rp, ep, return_syntax, file);
                    auto return_result = _return_result_result._Ok;
                    if (_return_result_result._tag == Success::Error) {
                        const auto _return_result_Error = _return_result_result._Error;
                        switch (_return_result_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_return_result_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(return_result));
                };
                break;
            }
            case CommandSyntax::Throw:
            {
                auto throw_syntax = _result._Throw;
                {
                    const auto _throw_result_result = handle_throw(rp, ep, throw_syntax, file);
                    auto throw_result = _throw_result_result._Ok;
                    if (_throw_result_result._tag == Success::Error) {
                        const auto _throw_result_Error = _throw_result_result._Error;
                        switch (_throw_result_Error._tag) {
                        default:
                            return Result<Statement, ModelError>(_throw_result_result._Error);

                        }
                    };
                    return Result<Statement, ModelError>(Statement(throw_result));
                };
                break;
            }
        }
    };
}

Result<Statement, ModelError> model::handle_statement(Page* rp, Page* ep, StatementSyntax& statement_syntax, String file) {
    const auto _statement_result = handle_command(rp, ep, statement_syntax.command, file);
    auto statement = _statement_result._Ok;
    if (_statement_result._tag == Success::Error) {
        const auto _statement_Error = _statement_result._Error;
        switch (_statement_Error._tag) {
        default:
            return Result<Statement, ModelError>(_statement_result._Error);

        }
    };
    return Result<Statement, ModelError>(statement);
}

Result<Vector<Statement>, ModelError> model::handle_statements(Page* rp, Page* ep, Vector<StatementSyntax>& statements, String file) {
    auto r = Region();
    List<Statement>& statements_builder = *new (alignof(List<Statement>), r.get_page()) List<Statement>();
    
    auto _statement_iterator = statements.get_iterator();
    while (auto _statement = _statement_iterator.next()) {
        auto statement = *_statement;{
            const auto _statement_result_result = handle_statement(rp, ep, statement, file);
            auto statement_result = _statement_result_result._Ok;
            if (_statement_result_result._tag == Success::Error) {
                const auto _statement_result_Error = _statement_result_result._Error;
                switch (_statement_result_Error._tag) {
                default:
                    return Result<Vector<Statement>, ModelError>(_statement_result_result._Error);

                }
            };
            statements_builder.add(statement_result);
        }
    };
    return Result<Vector<Statement>, ModelError>(Vector<Statement>(rp, statements_builder));
}

Result<Component, ModelError> model::handle_component(Page* rp, Page* ep, ComponentSyntax& component, String file) {
    auto r = Region();
    String* name = nullptr;
    List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
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
    List<Component>& components_builder = *new (alignof(List<Component>), r.get_page()) List<Component>();
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
    List<Vector<Operand>>& operations_builder = *new (alignof(List<Vector<Operand>>), r.get_page()) List<Vector<Operand>>();
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
        return Result<Block, ModelError>(Block(Span(block.start, block.end), statements));
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
                        cases_builder.add(Case(Span(case_syntax.start, case_syntax.end), condition));
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
    Statement* alternative = nullptr;
    if (match_syntax.alternative != nullptr) {
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
    Statement* alternative = nullptr;
    if (choose_syntax.alternative != nullptr) {
        const auto _alternative_result_result = handle_command(rp, ep, (*choose_syntax.alternative).alternative, file);
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
            case ConditionSyntax::Let:
            {
                auto let_syntax = _result._Let;
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

Result<While, ModelError> model::handle_while(Page* rp, Page* ep, WhileSyntax& while_syntax, String file) {
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

Result<Try, ModelError> model::handle_try(Page* rp, Page* ep, TrySyntax& try_syntax, String file) {
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
    Statement* dropper = nullptr;
    if (try_syntax.dropper != nullptr) {
        auto drop_syntax = *try_syntax.dropper;
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

Result<SizeOf, ModelError> model::handle_size_of(Page* rp, Page* ep, SizeOfSyntax& size_of_syntax, String file) {
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

Result<Is, ModelError> model::handle_is(Page* rp, Page* ep, IsSyntax& is_syntax, String file) {
    auto r = Region();
    List<String>& name_builder = *new (alignof(List<String>), r.get_page()) List<String>();
    name_builder.add(is_syntax.name.name);
    if (is_syntax.name.extensions != nullptr) {
        
        auto _name_iterator = (*is_syntax.name.extensions).get_iterator();
        while (auto _name = _name_iterator.next()) {
            auto name = *_name;{
                name_builder.add(name.name);
            }
        };
    };
    return Result<Is, ModelError>(Is(Span(is_syntax.start, is_syntax.end), Vector<String>(rp, name_builder)));
}

Result<Expression, ModelError> model::handle_expression(Page* rp, Page* ep, ExpressionSyntax& expression, String file) {
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
            case ExpressionSyntax::Is:
            {
                auto is_syntax = _result._Is;
                {
                    const auto _is_result_result = handle_is(rp, ep, is_syntax, file);
                    auto is_result = _is_result_result._Ok;
                    if (_is_result_result._tag == Success::Error) {
                        const auto _is_result_Error = _is_result_result._Error;
                        switch (_is_result_Error._tag) {
                        default:
                            return Result<Expression, ModelError>(_is_result_result._Error);

                        }
                    };
                    return Result<Expression, ModelError>(is_result);
                };
                break;
            }
        }
    };
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

Result<Vector<Operand>, ModelError> model::handle_operation(Page* rp, Page* ep, OperationSyntax& operation, String file) {
    if (operation.operands != nullptr) {
        const auto _operands_result_result = handle_operands(rp, ep, operation.operands, file);
        auto operands_result = _operands_result_result._Ok;
        if (_operands_result_result._tag == Success::Error) {
            const auto _operands_result_Error = _operands_result_result._Error;
            switch (_operands_result_Error._tag) {
            default:
                return Result<Vector<Operand>, ModelError>(_operands_result_result._Error);

            }
        };
        return Result<Vector<Operand>, ModelError>(operands_result);
    };
    return Result<Vector<Operand>, ModelError>(Vector<Operand>());
}

Result<GenericParameter, ModelError> model::handle_generic_parameter(Page* rp, Page* ep, GenericParameterSyntax& generic_parameter, String file) {
    auto r = Region();
    List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
    if (generic_parameter.attributes != nullptr) {
        
        auto _attribute_syntax_iterator = (*generic_parameter.attributes).get_iterator();
        while (auto _attribute_syntax = _attribute_syntax_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;{
                const auto _attribute_result = handle_attribute(rp, ep, attribute_syntax, file);
                auto attribute = _attribute_result._Ok;
                if (_attribute_result._tag == Success::Error) {
                    const auto _attribute_Error = _attribute_result._Error;
                    switch (_attribute_Error._tag) {
                    default:
                        return Result<GenericParameter, ModelError>(_attribute_result._Error);

                    }
                };
                attributes.add(attribute);
            }
        };
    };
    return Result<GenericParameter, ModelError>(GenericParameter(Span(generic_parameter.start, generic_parameter.end), String(rp, generic_parameter.name), Vector<Attribute>(rp, attributes)));
}

Result<Use, ModelError> model::handle_use(Page* rp, Page* ep, UseSyntax& use_syntax) {
    auto r = Region();
    List<String>& path = *new (alignof(List<String>), r.get_page()) List<String>();
    path.add(String(rp, use_syntax.name.name));
    if (use_syntax.name.extensions != nullptr) {
        
        auto _extension_iterator = (*use_syntax.name.extensions).get_iterator();
        while (auto _extension = _extension_iterator.next()) {
            auto extension = *_extension;{
                path.add(extension.name);
            }
        };
    };
    return Result<Use, ModelError>(Use(Span(use_syntax.start, use_syntax.end), Vector<String>(rp, path)));
}

Result<Concept, ModelError> model::handle_definition(Page* rp, Page* ep, String path, DefinitionSyntax& definition, bool private_, String file) {
    auto r = Region();
    const auto concept = definition.concept_;
    const auto span = Span(definition.start, definition.end);
    List<GenericParameter>& parameters = *new (alignof(List<GenericParameter>), r.get_page()) List<GenericParameter>();
    if (definition.parameters != nullptr) {
        const auto generic_parameters = *definition.parameters;
        if (generic_parameters.parameters != nullptr) {
            
            auto _generic_parameter_iterator = (*generic_parameters.parameters).get_iterator();
            while (auto _generic_parameter = _generic_parameter_iterator.next()) {
                auto generic_parameter = *_generic_parameter;{
                    const auto _parameter_result = handle_generic_parameter(rp, ep, generic_parameter, file);
                    auto parameter = _parameter_result._Ok;
                    if (_parameter_result._tag == Success::Error) {
                        const auto _parameter_Error = _parameter_result._Error;
                        switch (_parameter_Error._tag) {
                        default:
                            return Result<Concept, ModelError>(_parameter_result._Error);

                        }
                    };
                    parameters.add(parameter);
                }
            };
        };
    };
    List<Attribute>& attributes = *new (alignof(List<Attribute>), r.get_page()) List<Attribute>();
    if (definition.attributes != nullptr) {
        
        auto _attribute_syntax_iterator = (*definition.attributes).get_iterator();
        while (auto _attribute_syntax = _attribute_syntax_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;{
                const auto _attribute_result = handle_attribute(rp, ep, attribute_syntax, file);
                auto attribute = _attribute_result._Ok;
                if (_attribute_result._tag == Success::Error) {
                    const auto _attribute_Error = _attribute_result._Error;
                    switch (_attribute_Error._tag) {
                    default:
                        return Result<Concept, ModelError>(_attribute_result._Error);

                    }
                };
                attributes.add(attribute);
            }
        };
    };
    {
        auto _result = concept;
        switch (_result._tag)
        {
            case ConceptSyntax::Class:
            {
                auto class_syntax = _result._Class;
                {
                    const auto _structure_result = handle_class(rp, ep, definition.name, path, class_syntax, private_, file);
                    auto structure = _structure_result._Ok;
                    if (_structure_result._tag == Success::Error) {
                        const auto _structure_Error = _structure_result._Error;
                        switch (_structure_Error._tag) {
                        default:
                            return Result<Concept, ModelError>(_structure_result._Error);

                        }
                    };
                    return Result<Concept, ModelError>(Concept(span, String(rp, definition.name), Vector<GenericParameter>(rp, parameters), Vector<Attribute>(rp, attributes), Definition(structure)));
                };
                break;
            }
            case ConceptSyntax::Namespace:
            {
                auto namespace_syntax = _result._Namespace;
                {
                    const auto _namespace_result_result = handle_namespace(rp, ep, definition.name, Path::join(rp, path, definition.name), namespace_syntax, private_, file);
                    auto namespace_result = _namespace_result_result._Ok;
                    if (_namespace_result_result._tag == Success::Error) {
                        const auto _namespace_result_Error = _namespace_result_result._Error;
                        switch (_namespace_result_Error._tag) {
                        default:
                            return Result<Concept, ModelError>(_namespace_result_result._Error);

                        }
                    };
                    return Result<Concept, ModelError>(Concept(span, String(rp, definition.name), Vector<GenericParameter>(rp, parameters), Vector<Attribute>(rp, attributes), Definition(namespace_result)));
                };
                break;
            }
            case ConceptSyntax::Union:
            {
                auto union_syntax = _result._Union;
                {
                    const auto _union_result_result = handle_union(rp, ep, definition.name, path, union_syntax, private_, file);
                    auto union_result = _union_result_result._Ok;
                    if (_union_result_result._tag == Success::Error) {
                        const auto _union_result_Error = _union_result_result._Error;
                        switch (_union_result_Error._tag) {
                        default:
                            return Result<Concept, ModelError>(_union_result_result._Error);

                        }
                    };
                    return Result<Concept, ModelError>(Concept(span, String(rp, definition.name), Vector<GenericParameter>(rp, parameters), Vector<Attribute>(rp, attributes), Definition(union_result)));
                };
                break;
            }
            case ConceptSyntax::Constant:
            {
                auto constant_syntax = _result._Constant;
                {
                    const auto _operation_result = handle_operands(rp, ep, constant_syntax.operation, file);
                    auto operation = _operation_result._Ok;
                    if (_operation_result._tag == Success::Error) {
                        const auto _operation_Error = _operation_result._Error;
                        switch (_operation_Error._tag) {
                        default:
                            return Result<Concept, ModelError>(_operation_result._Error);

                        }
                    };
                    const auto _type_result = handle_type(rp, ep, constant_syntax.type, file);
                    auto type = _type_result._Ok;
                    if (_type_result._tag == Success::Error) {
                        const auto _type_Error = _type_result._Error;
                        switch (_type_Error._tag) {
                        default:
                            return Result<Concept, ModelError>(_type_result._Error);

                        }
                    };
                    return Result<Concept, ModelError>(Concept(span, String(rp, definition.name), Vector<GenericParameter>(rp, parameters), Vector<Attribute>(rp, attributes), Definition(Global(span, *type, operation))));
                };
                break;
            }
            case ConceptSyntax::Delegate:
            {
                auto delegate_syntax = _result._Delegate;
                return Result<Concept, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Delegate"), Span(delegate_syntax.start, delegate_syntax.end)))));
                break;
            }
            case ConceptSyntax::Intrinsic:
            {
                auto intrinsic_syntax = _result._Intrinsic;
                return Result<Concept, ModelError>(Concept(span, String(rp, definition.name), Vector<GenericParameter>(rp, parameters), Vector<Attribute>(rp, attributes), Definition(Intrinsic())));
                break;
            }
        }
    };
}

Result<Action, ModelError> model::handle_action(Page* rp, Page* ep, ActionSyntax& action, String file) {
    {
        auto _result = action;
        switch (_result._tag)
        {
            case ActionSyntax::Operation:
            {
                auto operation_syntax = _result._Operation;
                {
                    const auto _operation_result = handle_operation(rp, ep, operation_syntax, file);
                    auto operation = _operation_result._Ok;
                    if (_operation_result._tag == Success::Error) {
                        const auto _operation_Error = _operation_result._Error;
                        switch (_operation_Error._tag) {
                        default:
                            return Result<Action, ModelError>(_operation_result._Error);

                        }
                    };
                    return Result<Action, ModelError>(Action(operation, Vector<Operand>()));
                };
                break;
            }
            case ActionSyntax::Set:
            {
                auto set_syntax = _result._Set;
                {
                    const auto _target_result = handle_operands(rp, ep, set_syntax.target, file);
                    auto target = _target_result._Ok;
                    if (_target_result._tag == Success::Error) {
                        const auto _target_Error = _target_result._Error;
                        switch (_target_Error._tag) {
                        default:
                            return Result<Action, ModelError>(_target_result._Error);

                        }
                    };
                    const auto _source_result = handle_operands(rp, ep, set_syntax.source, file);
                    auto source = _source_result._Ok;
                    if (_source_result._tag == Success::Error) {
                        const auto _source_Error = _source_result._Error;
                        switch (_source_Error._tag) {
                        default:
                            return Result<Action, ModelError>(_source_result._Error);

                        }
                    };
                    return Result<Action, ModelError>(Action(source, target));
                };
                break;
            }
        }
    };
}

Result<Function, ModelError> model::build_function(Page* rp, Page* ep, size_t start, size_t end, TargetSyntax& target_syntax, bool private_, bool pure, String file) {
    auto input = Vector<Item>();
    auto output = Vector<Item>();
    {
        auto _result = target_syntax;
        switch (_result._tag)
        {
            case TargetSyntax::Routine:
            {
                auto routine_syntax = _result._Routine;
                return Result<Function, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Non-Symbol Function"), Span(start, end)))));
                break;
            }
            case TargetSyntax::Named:
            {
                auto named_syntax = _result._Named;
                {
                    if (named_syntax.routine.parameters != nullptr) {
                        const auto _input_result_result = handle_parameterset(rp, ep, *named_syntax.routine.parameters, file);
                        auto input_result = _input_result_result._Ok;
                        if (_input_result_result._tag == Success::Error) {
                            const auto _input_result_Error = _input_result_result._Error;
                            switch (_input_result_Error._tag) {
                            default:
                                return Result<Function, ModelError>(_input_result_result._Error);

                            }
                        };
                        input = input_result;
                    };
                    Type* returns_ = nullptr;
                    if (named_syntax.routine.returns_ != nullptr) {
                        const auto _type_result = handle_type(rp, ep, (*named_syntax.routine.returns_).type, file);
                        auto type = _type_result._Ok;
                        if (_type_result._tag == Success::Error) {
                            const auto _type_Error = _type_result._Error;
                            switch (_type_Error._tag) {
                            default:
                                return Result<Function, ModelError>(_type_result._Error);

                            }
                        };
                        returns_ = type;
                    };
                    Type* throws_ = nullptr;
                    if (named_syntax.routine.throws_ != nullptr) {
                        const auto _type_result = handle_type(rp, ep, (*named_syntax.routine.throws_).type, file);
                        auto type = _type_result._Ok;
                        if (_type_result._tag == Success::Error) {
                            const auto _type_Error = _type_result._Error;
                            switch (_type_Error._tag) {
                            default:
                                return Result<Function, ModelError>(_type_result._Error);

                            }
                        };
                        throws_ = type;
                    };
                    auto lifetime = Lifetime(Unspecified());
                    if (named_syntax.routine.lifetime != nullptr) {
                        {
                            auto _result = (*named_syntax.routine.lifetime);
                            switch (_result._tag)
                            {
                                case LifetimeSyntax::Call:
                                {
                                    auto call = _result._Call;
                                    lifetime = Lifetime(Call());
                                    break;
                                }
                                case LifetimeSyntax::Local:
                                {
                                    auto local = _result._Local;
                                    lifetime = Lifetime(Local());
                                    break;
                                }
                                case LifetimeSyntax::Reference:
                                {
                                    auto reference = _result._Reference;
                                    lifetime = Lifetime(Reference(String(rp, reference.location)));
                                    break;
                                }
                                case LifetimeSyntax::Thrown:
                                {
                                    auto thrown = _result._Thrown;
                                    lifetime = Lifetime(Thrown());
                                    break;
                                }
                            }
                        };
                    };
                    {
                        auto _result = named_syntax.routine.implementation;
                        switch (_result._tag)
                        {
                            case ImplementationSyntax::Action:
                            {
                                auto action_syntax = _result._Action;
                                {
                                    const auto _action_result = handle_action(rp, ep, action_syntax, file);
                                    auto action = _action_result._Ok;
                                    if (_action_result._tag == Success::Error) {
                                        const auto _action_Error = _action_result._Error;
                                        switch (_action_Error._tag) {
                                        default:
                                            return Result<Function, ModelError>(_action_result._Error);

                                        }
                                    };
                                    return Result<Function, ModelError>(Function(Span(start, end), private_, pure, String(rp, named_syntax.name), input, returns_, throws_, lifetime, Implementation(action)));
                                };
                                break;
                            }
                            case ImplementationSyntax::Extern:
                            {
                                auto extern_syntax = _result._Extern;
                                return Result<Function, ModelError>(Function(Span(start, end), private_, pure, String(rp, named_syntax.name), input, returns_, nullptr, lifetime, Implementation(Extern())));
                                break;
                            }
                            case ImplementationSyntax::Instruction:
                            {
                                auto instruction_syntax = _result._Instruction;
                                return Result<Function, ModelError>(Function(Span(start, end), private_, pure, String(rp, named_syntax.name), input, returns_, nullptr, lifetime, Implementation(Instruction())));
                                break;
                            }
                            case ImplementationSyntax::Intrinsic:
                            {
                                auto intrinsic_syntax = _result._Intrinsic;
                                return Result<Function, ModelError>(Function(Span(start, end), private_, pure, String(rp, named_syntax.end), input, returns_, nullptr, lifetime, Implementation(Intrinsic())));
                                break;
                            }
                        }
                    };
                };
                break;
            }
        }
    };
}

Result<Initializer, ModelError> model::handle_initializer(Page* rp, Page* ep, InitSyntax& init_syntax, bool private_, String file) {
    auto input = Vector<Item>();
    if (init_syntax.parameters != nullptr) {
        const auto _parameter_set_result = handle_parameterset(rp, ep, *init_syntax.parameters, file);
        auto parameter_set = _parameter_set_result._Ok;
        if (_parameter_set_result._tag == Success::Error) {
            const auto _parameter_set_Error = _parameter_set_result._Error;
            switch (_parameter_set_Error._tag) {
            default:
                return Result<Initializer, ModelError>(_parameter_set_result._Error);

            }
        };
        input = parameter_set;
    };
    const auto _action_result = handle_action(rp, ep, init_syntax.action, file);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
        default:
            return Result<Initializer, ModelError>(_action_result._Error);

        }
    };
    return Result<Initializer, ModelError>(Initializer(Span(init_syntax.start, init_syntax.end), private_, input, Implementation(action)));
}

Result<DeInitializer*, ModelError> model::handle_deinitializer(Page* rp, Page* ep, DeInitSyntax& de_init_syntax, String file) {
    const auto _action_result = handle_action(rp, ep, de_init_syntax.action, file);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
        default:
            return Result<DeInitializer*, ModelError>(_action_result._Error);

        }
    };
    return Result<DeInitializer*, ModelError>(new (alignof(DeInitializer), rp) DeInitializer(Span(de_init_syntax.start, de_init_syntax.end), Implementation(action)));
}

Result<Operator, ModelError> model::handle_operator(Page* rp, Page* ep, OperatorSyntax& operator_syntax, bool private_, String file) {
    auto input = Vector<Item>();
    const auto start = operator_syntax.start;
    const auto end = operator_syntax.end;
    Vector<Operand>* operation = nullptr;
    {
        auto _result = operator_syntax.target;
        switch (_result._tag)
        {
            case TargetSyntax::Routine:
            {
                auto routine = _result._Routine;
                {
                    if (routine.generics != nullptr) {
                        if (routine.parameters != nullptr) {
                            const auto _input_result_result = handle_parameterset(rp, ep, *routine.parameters, file);
                            auto input_result = _input_result_result._Ok;
                            if (_input_result_result._tag == Success::Error) {
                                const auto _input_result_Error = _input_result_result._Error;
                                switch (_input_result_Error._tag) {
                                default:
                                    return Result<Operator, ModelError>(_input_result_result._Error);

                                }
                            };
                            input = input_result;
                        };
                        Type* returns_ = nullptr;
                        if (routine.returns_ != nullptr) {
                            const auto _type_result = handle_type(rp, ep, (*routine.returns_).type, file);
                            auto type = _type_result._Ok;
                            if (_type_result._tag == Success::Error) {
                                const auto _type_Error = _type_result._Error;
                                switch (_type_Error._tag) {
                                default:
                                    return Result<Operator, ModelError>(_type_result._Error);

                                }
                            };
                            returns_ = type;
                        };
                        Type* throws_ = nullptr;
                        if (routine.throws_ != nullptr) {
                            const auto _type_result = handle_type(rp, ep, (*routine.throws_).type, file);
                            auto type = _type_result._Ok;
                            if (_type_result._tag == Success::Error) {
                                const auto _type_Error = _type_result._Error;
                                switch (_type_Error._tag) {
                                default:
                                    return Result<Operator, ModelError>(_type_result._Error);

                                }
                            };
                            throws_ = type;
                        };
                        {
                            auto _result = routine.implementation;
                            switch (_result._tag)
                            {
                                case ImplementationSyntax::Action:
                                {
                                    auto action_syntax = _result._Action;
                                    {
                                        const auto _action_result = handle_action(rp, ep, action_syntax, file);
                                        auto action = _action_result._Ok;
                                        if (_action_result._tag == Success::Error) {
                                            const auto _action_Error = _action_result._Error;
                                            switch (_action_Error._tag) {
                                            default:
                                                return Result<Operator, ModelError>(_action_result._Error);

                                            }
                                        };
                                        return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, "[]"), input, returns_, throws_, Implementation(action)));
                                    };
                                    break;
                                }
                                case ImplementationSyntax::Extern:
                                {
                                    auto extern_syntax = _result._Extern;
                                    return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, "[]"), input, returns_, throws_, Implementation(Extern())));
                                    break;
                                }
                                case ImplementationSyntax::Instruction:
                                {
                                    auto instructio_syntax = _result._Instruction;
                                    return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, "[]"), input, returns_, throws_, Implementation(Instruction())));
                                    break;
                                }
                                case ImplementationSyntax::Intrinsic:
                                {
                                    auto intrinsic_syntax = _result._Intrinsic;
                                    return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, "[]"), input, returns_, throws_, Implementation(Intrinsic())));
                                    break;
                                }
                            }
                        };
                    };
                    return Result<Operator, ModelError>(ModelError(ModelBuilderError(NotImplemented(file, String(ep, "Non-Symbol Operator"), Span(operator_syntax.start, operator_syntax.end)))));
                };
                break;
            }
            case TargetSyntax::Named:
            {
                auto named = _result._Named;
                {
                    if (named.routine.parameters != nullptr) {
                        const auto _input_result_result = handle_parameterset(rp, ep, *named.routine.parameters, file);
                        auto input_result = _input_result_result._Ok;
                        if (_input_result_result._tag == Success::Error) {
                            const auto _input_result_Error = _input_result_result._Error;
                            switch (_input_result_Error._tag) {
                            default:
                                return Result<Operator, ModelError>(_input_result_result._Error);

                            }
                        };
                        input = input_result;
                    };
                    Type* returns_ = nullptr;
                    if (named.routine.returns_ != nullptr) {
                        const auto _type_result = handle_type(rp, ep, (*named.routine.returns_).type, file);
                        auto type = _type_result._Ok;
                        if (_type_result._tag == Success::Error) {
                            const auto _type_Error = _type_result._Error;
                            switch (_type_Error._tag) {
                            default:
                                return Result<Operator, ModelError>(_type_result._Error);

                            }
                        };
                        returns_ = type;
                    };
                    Type* throws_ = nullptr;
                    if (named.routine.throws_ != nullptr) {
                        const auto _type_result = handle_type(rp, ep, (*named.routine.throws_).type, file);
                        auto type = _type_result._Ok;
                        if (_type_result._tag == Success::Error) {
                            const auto _type_Error = _type_result._Error;
                            switch (_type_Error._tag) {
                            default:
                                return Result<Operator, ModelError>(_type_result._Error);

                            }
                        };
                        throws_ = type;
                    };
                    {
                        auto _result = named.routine.implementation;
                        switch (_result._tag)
                        {
                            case ImplementationSyntax::Action:
                            {
                                auto action_syntax = _result._Action;
                                {
                                    const auto _action_result = handle_action(rp, ep, action_syntax, file);
                                    auto action = _action_result._Ok;
                                    if (_action_result._tag == Success::Error) {
                                        const auto _action_Error = _action_result._Error;
                                        switch (_action_Error._tag) {
                                        default:
                                            return Result<Operator, ModelError>(_action_result._Error);

                                        }
                                    };
                                    return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, named.name), input, returns_, throws_, Implementation(action)));
                                };
                                break;
                            }
                            case ImplementationSyntax::Extern:
                            {
                                auto extern_syntax = _result._Extern;
                                return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, named.name), input, returns_, throws_, Implementation(Extern())));
                                break;
                            }
                            case ImplementationSyntax::Instruction:
                            {
                                auto instruction_syntax = _result._Instruction;
                                return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, named.name), input, returns_, throws_, Implementation(Instruction())));
                                break;
                            }
                            case ImplementationSyntax::Intrinsic:
                            {
                                auto intrinsic_syntax = _result._Intrinsic;
                                return Result<Operator, ModelError>(Operator(Span(start, end), private_, String(rp, named.name), input, returns_, throws_, Implementation(Intrinsic())));
                                break;
                            }
                        }
                    };
                };
                break;
            }
        }
    };
}

Result<Module, ModelError> model::build_module(Page* rp, Page* ep, String path, String file_name, String name, FileSyntax& file_syntax, bool private_) {
    auto r = Region();
    List<Use>& uses = *new (alignof(List<Use>), r.get_page()) List<Use>();
    if (file_syntax.uses != nullptr) {
        
        auto _use_syntax_iterator = (*file_syntax.uses).get_iterator();
        while (auto _use_syntax = _use_syntax_iterator.next()) {
            auto use_syntax = *_use_syntax;{
                const auto _use_result_result = handle_use(rp, ep, use_syntax);
                auto use_result = _use_result_result._Ok;
                if (_use_result_result._tag == Success::Error) {
                    const auto _use_result_Error = _use_result_result._Error;
                    switch (_use_result_Error._tag) {
                    default:
                        return Result<Module, ModelError>(_use_result_result._Error);

                    }
                };
                uses.add(use_result);
            }
        };
    };
    Array<Member>& members_builder = *new (alignof(Array<Member>), r.get_page()) Array<Member>();
    HashMapBuilder<String, Nameable>& symbols_builder = *new (alignof(HashMapBuilder<String, Nameable>), r.get_page()) HashMapBuilder<String, Nameable>();
    MultiMapBuilder<String, Function>& functions_builder = *new (alignof(MultiMapBuilder<String, Function>), r.get_page()) MultiMapBuilder<String, Function>();
    HashSetBuilder<String>& modules_checker = *new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new (alignof(List<Module>), r.get_page()) List<Module>();
    if (file_syntax.declarations != nullptr) {
        
        auto _declaration_iterator = (*file_syntax.declarations).get_iterator();
        while (auto _declaration = _declaration_iterator.next()) {
            auto declaration = *_declaration;{
                {
                    auto _result = declaration.symbol;
                    switch (_result._tag)
                    {
                        case SymbolSyntax::Private:
                        {
                            auto private_syntax = _result._Private;
                            {
                                {
                                    auto _result = private_syntax.export_;
                                    switch (_result._tag)
                                    {
                                        case ExportSyntax::Definition:
                                        {
                                            auto definition_syntax = _result._Definition;
                                            {
                                                const auto _concept_result = handle_definition(rp, ep, path, definition_syntax, true, file_name);
                                                auto concept = _concept_result._Ok;
                                                if (_concept_result._tag == Success::Error) {
                                                    const auto _concept_Error = _concept_result._Error;
                                                    switch (_concept_Error._tag) {
                                                    default:
                                                        return Result<Module, ModelError>(_concept_result._Error);

                                                    }
                                                };
                                                if ((functions_builder.contains(concept.name))) 
                                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(FunctionSymbolExists(file_name, Span(definition_syntax.start, definition_syntax.end)))));
                                                if (symbols_builder.add(concept.name, Nameable(concept)) == false) 
                                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(DuplicateName(concept.name, Span(definition_syntax.start, definition_syntax.end)))));
                                                members_builder.add(Member(concept));
                                            };
                                            break;
                                        }
                                        case ExportSyntax::Function:
                                        {
                                            auto function_syntax = _result._Function;
                                            {
                                                const auto _function_result_result = build_function(rp, ep, function_syntax.start, function_syntax.end, function_syntax.target, true, true, file_name);
                                                auto function_result = _function_result_result._Ok;
                                                if (_function_result_result._tag == Success::Error) {
                                                    const auto _function_result_Error = _function_result_result._Error;
                                                    switch (_function_result_Error._tag) {
                                                    default:
                                                        return Result<Module, ModelError>(_function_result_result._Error);

                                                    }
                                                };
                                                const auto symbol_with_function_name = symbols_builder.get(function_result.name);
                                                if (symbol_with_function_name != nullptr) 
                                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(function_syntax.start, function_syntax.end)))));
                                                functions_builder.add(function_result.name, function_result);
                                                members_builder.add(Member(function_result));
                                            };
                                            break;
                                        }
                                        case ExportSyntax::Operator:
                                        {
                                            auto operator_syntax = _result._Operator;
                                            {
                                                const auto _operator_result_result = handle_operator(rp, ep, operator_syntax, true, file_name);
                                                auto operator_result = _operator_result_result._Ok;
                                                if (_operator_result_result._tag == Success::Error) {
                                                    const auto _operator_result_Error = _operator_result_result._Error;
                                                    switch (_operator_result_Error._tag) {
                                                    default:
                                                        return Result<Module, ModelError>(_operator_result_result._Error);

                                                    }
                                                };
                                                if (symbols_builder.add(operator_result.name, Nameable(operator_result)) == false) 
                                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(DuplicateName(file_name, Span(operator_syntax.start, operator_syntax.end)))));
                                                members_builder.add(Member(operator_result));
                                            };
                                            break;
                                        }
                                        case ExportSyntax::Trait:
                                        {
                                            auto trait_syntax = _result._Trait;
                                            return Result<Module, ModelError>(ModelError(ModelBuilderError(NotImplemented(file_name, String(ep, "Private Trait"), Span(trait_syntax.start, trait_syntax.end)))));
                                            break;
                                        }
                                        case ExportSyntax::Module:
                                        {
                                            auto module_syntax = _result._Module;
                                            {
                                                const auto _module_result_result = handle_module(rp, ep, path, module_syntax, true);
                                                auto module_result = _module_result_result._Ok;
                                                if (_module_result_result._tag == Success::Error) {
                                                    const auto _module_result_Error = _module_result_result._Error;
                                                    switch (_module_result_Error._tag) {
                                                    default:
                                                        return Result<Module, ModelError>(_module_result_result._Error);

                                                    }
                                                };
                                                if (modules_checker.add(module_result.name) == false) 
                                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(module_syntax.start, module_syntax.end)))));
                                                modules.add(module_result);
                                            };
                                            break;
                                        }
                                    }
                                };
                            };
                            break;
                        }
                        case SymbolSyntax::Definition:
                        {
                            auto definition_syntax = _result._Definition;
                            {
                                const auto _concept_result = handle_definition(rp, ep, path, definition_syntax, false, file_name);
                                auto concept = _concept_result._Ok;
                                if (_concept_result._tag == Success::Error) {
                                    const auto _concept_Error = _concept_result._Error;
                                    switch (_concept_Error._tag) {
                                    default:
                                        return Result<Module, ModelError>(_concept_result._Error);

                                    }
                                };
                                if ((functions_builder.contains(concept.name))) 
                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(FunctionSymbolExists(file_name, Span(definition_syntax.start, definition_syntax.end)))));
                                if (symbols_builder.add(concept.name, Nameable(concept)) == false) 
                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(DuplicateName(file_name, Span(definition_syntax.start, definition_syntax.end)))));
                                members_builder.add(Member(concept));
                            };
                            break;
                        }
                        case SymbolSyntax::Function:
                        {
                            auto function_syntax = _result._Function;
                            {
                                const auto _function_result_result = build_function(rp, ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file_name);
                                auto function_result = _function_result_result._Ok;
                                if (_function_result_result._tag == Success::Error) {
                                    const auto _function_result_Error = _function_result_result._Error;
                                    switch (_function_result_Error._tag) {
                                    default:
                                        return Result<Module, ModelError>(_function_result_result._Error);

                                    }
                                };
                                const auto symbol_with_function_name = symbols_builder.get(function_result.name);
                                if (symbol_with_function_name != nullptr) 
                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(function_syntax.start, function_syntax.end)))));
                                functions_builder.add(function_result.name, function_result);
                                members_builder.add(Member(function_result));
                            };
                            break;
                        }
                        case SymbolSyntax::Operator:
                        {
                            auto operator_syntax = _result._Operator;
                            {
                                const auto _operator_result_result = handle_operator(rp, ep, operator_syntax, false, file_name);
                                auto operator_result = _operator_result_result._Ok;
                                if (_operator_result_result._tag == Success::Error) {
                                    const auto _operator_result_Error = _operator_result_result._Error;
                                    switch (_operator_result_Error._tag) {
                                    default:
                                        return Result<Module, ModelError>(_operator_result_result._Error);

                                    }
                                };
                                if (symbols_builder.add(operator_result.name, Nameable(operator_result)) == false) 
                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(DuplicateName(file_name, Span(operator_syntax.start, operator_syntax.end)))));
                                members_builder.add(Member(operator_result));
                            };
                            break;
                        }
                        case SymbolSyntax::Trait:
                        {
                            auto trait_syntax = _result._Trait;
                            return Result<Module, ModelError>(ModelError(ModelBuilderError(NotImplemented(file_name, String(ep, "Trait"), Span(trait_syntax.start, trait_syntax.end)))));
                            break;
                        }
                        case SymbolSyntax::Macro:
                        {
                            auto macro_syntax = _result._Macro;
                            return Result<Module, ModelError>(ModelError(ModelBuilderError(NotImplemented(file_name, String(ep, "Macro"), Span(macro_syntax.start, macro_syntax.end)))));
                            break;
                        }
                        case SymbolSyntax::Module:
                        {
                            auto module_syntax = _result._Module;
                            {
                                const auto _module_result_result = handle_module(rp, ep, path, module_syntax, false);
                                auto module_result = _module_result_result._Ok;
                                if (_module_result_result._tag == Success::Error) {
                                    const auto _module_result_Error = _module_result_result._Error;
                                    switch (_module_result_Error._tag) {
                                    default:
                                        return Result<Module, ModelError>(_module_result_result._Error);

                                    }
                                };
                                if (modules_checker.add(module_result.name) == false) 
                                    return Result<Module, ModelError>(ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(module_syntax.start, module_syntax.end)))));
                                modules.add(module_result);
                            };
                            break;
                        }
                    }
                };
            }
        };
    };
    auto multi_map = MultiMap<String, Function>(r.get_page(), functions_builder);
    
    auto _functions_iterator = multi_map.get_iterator();
    while (auto _functions = _functions_iterator.next()) {
        auto functions = *_functions;{
            const auto name = (*functions.get(0)).name;
            symbols_builder.add(String(name), Nameable(Vector<Function>(rp, functions)));
        }
    };
    return Result<Module, ModelError>(Module(private_, String(rp, file_name), name, Vector<Module>(rp, modules), Vector<Use>(rp, uses), Vector<Member>(rp, members_builder), HashMap<String, Nameable>(rp, symbols_builder)));
}

Result<Module, ModelError> model::build_referenced_module(Page* rp, Page* ep, String path, String name, bool private_) {
    auto r = Region();
    StringBuilder& file_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(Path::join(r.get_page(), path, name));
    file_name_builder.append(".scaly");
    const auto file_name = file_name_builder.to_string(rp);
    const auto module_text_result = File::read_to_string(r.get_page(), ep, file_name);
    {
        auto _result = module_text_result;
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                return Result<Module, ModelError>(ModelError(error));
                break;
            }
            case Success::Ok:
            {
                auto module_text = _result._Ok;
                {
                    Parser& parser = *new (alignof(Parser), r.get_page()) Parser(module_text);
                    const auto _file_syntax_result = parser.parse_file(rp, ep);
                    auto file_syntax = _file_syntax_result._Ok;
                    if (_file_syntax_result._tag == Success::Error) {
                        const auto _file_syntax_Error = _file_syntax_result._Error;
                        switch (_file_syntax_Error._tag) {
                            case ParserError::Invalid: {
                                const auto invalid_syntax = _file_syntax_Error._Invalid;
                                return Result<Module, ModelError>(ModelError(ParserModelError(file_name, invalid_syntax)));
                                break;
                            }
                            case ParserError::Different: {
                                const auto different_syntax = _file_syntax_Error._Different;
                                return Result<Module, ModelError>(ModelError(ParserModelError(file_name, different_syntax)));
                                break;
                            }
                        }
                    }if (parser.is_at_end() == false) 
                        return Result<Module, ModelError>(ModelError(ParserModelError(file_name, ParserError(InvalidSyntax(parser.lexer.previous_position, parser.lexer.position, String(ep, "a valid declaration"))))));
                    const auto _module_result_result = build_module(rp, ep, path, file_name, name, file_syntax, private_);
                    auto module_result = _module_result_result._Ok;
                    if (_module_result_result._tag == Success::Error) {
                        const auto _module_result_Error = _module_result_result._Error;
                        switch (_module_result_Error._tag) {
                        default:
                            return Result<Module, ModelError>(_module_result_result._Error);

                        }
                    };
                    return Result<Module, ModelError>(module_result);
                };
                break;
            }
        }
    };
}

Result<Module, ModelError> model::handle_module(Page* rp, Page* ep, String path, ModuleSyntax& module_syntax, bool private_) {
    const auto _module_result_result = build_referenced_module(rp, ep, path, module_syntax.name, private_);
    auto module_result = _module_result_result._Ok;
    if (_module_result_result._tag == Success::Error) {
        const auto _module_result_Error = _module_result_result._Error;
        switch (_module_result_Error._tag) {
        default:
            return Result<Module, ModelError>(_module_result_result._Error);

        }
    };
    return Result<Module, ModelError>(module_result);
}

Result<Module, ModelError> model::handle_package(Page* rp, Page* ep, String path, PackageSyntax& package_syntax) {
    const auto _module_result_result = build_referenced_module(rp, ep, path, package_syntax.name.name, false);
    auto module_result = _module_result_result._Ok;
    if (_module_result_result._tag == Success::Error) {
        const auto _module_result_Error = _module_result_result._Error;
        switch (_module_result_Error._tag) {
        default:
            return Result<Module, ModelError>(_module_result_result._Error);

        }
    };
    return Result<Module, ModelError>(module_result);
}

Result<Program, ModelError> model::build_program(Page* rp, Page* ep, String file_name, String program_name) {
    auto r = Region();
    const auto file_text_result = File::read_to_string(r.get_page(), r.get_page(), file_name);
    {
        auto _result = file_text_result;
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                return Result<Program, ModelError>(ModelError(error));
                break;
            }
            case Success::Ok:
            {
                auto file_text = _result._Ok;
                {
                    const auto _program_syntax_result = parse_program(rp, ep, file_text);
                    auto program_syntax = _program_syntax_result._Ok;
                    if (_program_syntax_result._tag == Success::Error) {
                        const auto _program_syntax_Error = _program_syntax_result._Error;
                        switch (_program_syntax_Error._tag) {
                            case ParserError::Invalid: {
                                const auto invalid_syntax = _program_syntax_Error._Invalid;
                                return Result<Program, ModelError>(ModelError(ParserModelError(file_name, invalid_syntax)));
                                break;
                            }
                            case ParserError::Different: {
                                const auto different_syntax = _program_syntax_Error._Different;
                                return Result<Program, ModelError>(ModelError(ParserModelError(file_name, different_syntax)));
                                break;
                            }
                        }
                    };
                    const auto path = Path::get_directory_name(rp, file_name);
                    List<Module>& packages = *new (alignof(List<Module>), r.get_page()) List<Module>();
                    if (program_syntax.file.packages != nullptr) {
                        
                        auto _package_syntax_iterator = (*program_syntax.file.packages).get_iterator();
                        while (auto _package_syntax = _package_syntax_iterator.next()) {
                            auto package_syntax = *_package_syntax;{
                                const auto _package_result_result = build_referenced_module(rp, ep, path, package_syntax.name.name, false);
                                auto package_result = _package_result_result._Ok;
                                if (_package_result_result._tag == Success::Error) {
                                    const auto _package_result_Error = _package_result_result._Error;
                                    switch (_package_result_Error._tag) {
                                    default:
                                        return Result<Program, ModelError>(_package_result_result._Error);

                                    }
                                };
                                packages.add(package_result);
                            }
                        };
                    };
                    const auto _module_result_result = build_module(rp, ep, Path::get_directory_name(rp, file_name), file_name, program_name, program_syntax.file, false);
                    auto module_result = _module_result_result._Ok;
                    if (_module_result_result._tag == Success::Error) {
                        const auto _module_result_Error = _module_result_result._Error;
                        switch (_module_result_Error._tag) {
                        default:
                            return Result<Program, ModelError>(_module_result_result._Error);

                        }
                    };
                    auto statements = Vector<Statement>();
                    if (program_syntax.statements != nullptr) {
                        const auto _statements_result_result = handle_statements(rp, ep, *program_syntax.statements, file_name);
                        auto statements_result = _statements_result_result._Ok;
                        if (_statements_result_result._tag == Success::Error) {
                            const auto _statements_result_Error = _statements_result_result._Error;
                            switch (_statements_result_Error._tag) {
                            default:
                                return Result<Program, ModelError>(_statements_result_result._Error);

                            }
                        };
                        statements = statements_result;
                    };
                    return Result<Program, ModelError>(Program(Vector<Module>(rp, packages), module_result, statements));
                };
                break;
            }
        }
    };
}

}
}