namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

Result<ProgramSyntax, ParserError> parse_program(Page* _rp, Page* _ep, const String& program) {
    Region _r;
    {
        Region _r_1;
        Parser& parser = *new(alignof(Parser), _r_1.get_page()) Parser(program);
        auto program_syntax_result = parser.parse_program(_rp, _ep);
        if (!parser.is_at_end())
            return Result<ProgramSyntax, ParserError> { ._tag = Result<ProgramSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(parser.lexer.previous_position, parser.lexer.position, String(_ep, "a valid declaraion"))) };

        return program_syntax_result;
    }
}

Result<Type*, ModelError> handle_type(Page* _rp, Page* _ep, TypeSyntax& type, String file);
Result<Attribute, ModelError> handle_attribute(Page* _rp, Page* _ep, AttributeSyntax& attribute, String file);
Result<Vector<Operand>, ModelError> handle_operands(Page* _rp, Page* _ep, Vector<OperandSyntax>* operands, String file);

Result<Property, ModelError> handle_property(Page* _rp, Page* _ep, bool private_, PropertySyntax& property, String file) {
    Region _r;
    Type* type = nullptr;
    if (property.annotation != nullptr) {
        auto _type_result = handle_type(_rp, _ep, property.annotation->type, file);
        if (_type_result._tag == Result<Type, ModelError>::Error)
            return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Error, ._Error = _type_result._Error };
        type = _type_result._Ok;
    }

    Vector<Operand>* initializer = nullptr;
    if (property.initializer != nullptr) {
        auto _initializer_result = handle_operands(_rp, _ep, property.initializer->operands, file);
        if (_initializer_result._tag == Result<Type, ModelError>::Error)
            return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Error, ._Error = _initializer_result._Error };
        initializer = new (alignof(Vector<Operand>), _rp) Vector<Operand> (_rp, _initializer_result._Ok);
    }

    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (property.attributes != nullptr) {
        auto property_attributes = property.attributes;
        auto _attribute_iterator = property_attributes->get_iterator();
        while (auto _attribute_syntax = _attribute_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;
            auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
            if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Error, ._Error = _attribute_result._Error };
            auto attribute = _attribute_result._Ok;
            attributes.add(attribute);
        }
    }
    return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Ok, ._Ok = Property(Span(property.start, property.end), false, String(_rp, property.name), type, initializer, Vector<Attribute>(_rp, attributes)) };
}

Result<Item, ModelError> handle_item(Page* _rp, Page* _ep, bool private_, ItemSyntax& item, String file) {
    Region _r;
    Type* type = nullptr;
    if (item.annotation != nullptr) {
        auto _type_result = handle_type(_rp, _ep, item.annotation->type, file);
        if (_type_result._tag == Result<Type, ModelError>::Error)
            return Result<Item, ModelError> { ._tag = Result<Item, ModelError>::Error, ._Error = _type_result._Error };
        type = _type_result._Ok;
    }

    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (item.attributes != nullptr) {
        auto item_attributes = item.attributes;
        auto _attribute_iterator = item_attributes->get_iterator();
        while (auto _attribute_syntax = _attribute_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;
            auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
            if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                return Result<Item, ModelError> { ._tag = Result<Item, ModelError>::Error, ._Error = _attribute_result._Error };
            auto attribute = _attribute_result._Ok;
            attributes.add(attribute);
        }
    }
    return Result<Item, ModelError> { ._tag = Result<Item, ModelError>::Ok, ._Ok = Item(Span(item.start, item.end), false, new(alignof(String), _rp) String(_rp, item.name), type, Vector<Attribute>(_rp, attributes)) };
}

Result<Vector<Item>, ModelError> handle_parameterset(Page* _rp, Page* _ep, ParameterSetSyntax& parameterSetSyntax, String file) {
    Region _r;
    List<Item>& items = *new(alignof(List<Item>), _r.get_page()) List<Item>();
    switch (parameterSetSyntax._tag) {
        case ParameterSetSyntax::Parameters: {
            auto parameters_syntax = parameterSetSyntax._Parameters;
            if (parameters_syntax.items != nullptr) {
                auto _item_syntax_iterator = parameters_syntax.items->get_iterator();
                while (auto _item_syntax = _item_syntax_iterator.next()) {
                    auto item_syntax = *_item_syntax;
                    auto _item_result = handle_item(_rp, _ep, false, item_syntax, file);
                    if (_item_result._tag == Result<Item, ModelError>::Error)
                        return Result<Vector<Item>, ModelError> { ._tag = Result<Vector<Item>, ModelError>::Error, ._Error = _item_result._Error };
                    auto item = _item_result._Ok;
                    items.add(item);
                }
            }
        }
        break;
        case ParameterSetSyntax::Type: {
            auto type_syntax = parameterSetSyntax._Type;
            auto _type_result = handle_type(_rp, _ep, type_syntax, file);
            if (_type_result._tag == Result<Type, ModelError>::Error)
                return Result<Vector<Item>, ModelError> { ._tag = Result<Vector<Item>, ModelError>::Error, ._Error = _type_result._Error };
            auto type = _type_result._Ok;
            items.add(Item(Span(type_syntax.start, type_syntax.end), false, nullptr, type, Vector<Attribute>()));
        }
        break;
    }
    return Result<Vector<Item>, ModelError> {
        ._tag = Result<Vector<Item>, ModelError>::Ok, 
        ._Ok = Vector<Item>(_rp, items) };
}

Result<Concept, ModelError> handle_definition(Page* _rp, Page* _ep, String path, DefinitionSyntax& definition, bool private_, String file);
Result<Function, ModelError> build_function(Page* _rp, Page* _ep, size_t start, size_t end, TargetSyntax targetSyntax, bool private_, bool pure, String file);
Result<Initializer, ModelError> handle_initializer(Page* _rp, Page* _ep, InitSyntax& initializer, bool private_, String file);
Result<DeInitializer*, ModelError> handle_deinitializer(Page* _rp, Page* _ep, DeInitSyntax& deinitializer, String file);
Result<Operator, ModelError> handle_operator(Page* _rp, Page* _ep,  OperatorSyntax& operator_syntax, bool private_, String file);
Result<Module, ModelError> handle_module(Page* _rp, Page* _ep, String path, ModuleSyntax& module_syntax, bool private_);
Result<Use, ModelError> handle_use(Page* _rp, Page* _ep, UseSyntax& use_);

Result<Structure, ModelError> handle_class(Page* _rp, Page* _ep, String name, String path, ClassSyntax& class_syntax, bool private_, String file) {    
    Region _r;
    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.get_page()) HashMapBuilder<String, Nameable>();
    Array<Property>& properties_builder = *new(alignof(Array<Property>), _r.get_page()) Array<Property>();
    if (class_syntax.structure.parts != nullptr) {
        auto _parts_iterator = class_syntax.structure.parts->get_iterator();
        while (auto _part = _parts_iterator.next()) {
            auto part = *_part;
            switch (part._tag) {
                case PartSyntax::Field: {
                    auto _property_result = handle_property(_rp, _ep, true, part._Property, file);
                    if (_property_result._tag == Result<Property, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _property_result._Error };
                    auto property = _property_result._Ok;
                    if (!symbols_builder.add(property.name, Nameable { ._tag = Nameable::Property, ._Property = property }))
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(DuplicateName(property.name, Span(part._Property.start, part._Property.end)))) };
                    properties_builder.add(property);
                }
                break;
                case PartSyntax::Property: {
                    auto _property_result = handle_property(_rp, _ep, false, part._Property, file);
                    if (_property_result._tag == Result<Property, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _property_result._Error };
                    auto property = _property_result._Ok;
                    if (!symbols_builder.add(property.name, Nameable { ._tag = Nameable::Property, ._Property = property }))
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(DuplicateName(property.name, Span(part._Property.start, part._Property.end)))) };
                    properties_builder.add(property);
                }
                break;
            }
        }
    }

    HashSetBuilder<String>& modules_checker = *new(alignof(HashSetBuilder<String>), _r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new(alignof(List<Module>), _r.get_page()) List<Module>();
    List<Use>& uses = *new(alignof(List<Use>), _r.get_page()) List<Use>();

    if (class_syntax.body == nullptr) {
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_syntax.start, class_syntax.end), private_, Vector<Property>(_rp, properties_builder), Vector<Module>(), Vector<Use>(), Vector<Initializer>(), nullptr, Vector<Member>(), HashMap<String, Nameable>())
        };
    }
    else {
        if (class_syntax.body->uses != nullptr) {
            auto use_syntaxes = class_syntax.body->uses;
            auto _uses_iterator = use_syntaxes->get_iterator();
            while (auto use_syntax = _uses_iterator.next()) {
                auto use = *use_syntax;
                auto _use_result = handle_use(_rp, _ep, *use_syntax);
                if (_use_result._tag == Result<Use, ModelError>::Error)
                    return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _use_result._Error };
                auto use_ = _use_result._Ok;
                uses.add(use_);

            }
        }

        List<Initializer>& initializers_builder = *new(alignof(List<Initializer>), _r.get_page()) List<Initializer>();
        DeInitializer* deInitializer = nullptr;
        if (class_syntax.body->inits != nullptr)
        {
            auto initializers_iterator = class_syntax.body->inits->get_iterator();
            while (auto initializer = initializers_iterator.next()) {
                auto _initializer_result = handle_initializer(_rp, _ep, *initializer, false, file);
                if (_initializer_result._tag == Result<Initializer, ModelError>::Error)
                    return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _initializer_result._Error };
                initializers_builder.add(_initializer_result._Ok);
            }
        }
        if (class_syntax.body->deInit != nullptr) {
            auto _deinitializer_result = handle_deinitializer(_rp, _ep, *class_syntax.body->deInit, file);
            if (_deinitializer_result._tag == Result<DeInitializer, ModelError>::Error)
                return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _deinitializer_result._Error };
            deInitializer = _deinitializer_result._Ok;
        }
        Array<Member>& members_builder = *new(alignof(Array<Member>), _r.get_page()) Array<Member>();
        MultiMapBuilder<String, Function>& functions_builder = *new(alignof(MultiMapBuilder<String, Function>), _r.get_page()) MultiMapBuilder<String, Function>();
        if (class_syntax.body->members != nullptr) {
            auto members_iterator = class_syntax.body->members->get_iterator();
            while (auto member = members_iterator.next()) {
                switch (member->constituent._tag) {
                    case ConstituentSyntax::Definition: {
                        auto concept_result = handle_definition(_rp, _ep, path, member->constituent._Definition, false, file);
                        if (concept_result._tag == Result<Function, ModelError>::Error)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = concept_result._Error };
                        auto concept = concept_result._Ok;
                        if (functions_builder.contains(concept.name))
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(member->constituent._Definition.start, member->constituent._Definition.end)))) };

                        if (!symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept }))
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(DuplicateName(concept.name, Span(member->constituent._Definition.start, member->constituent._Definition.end)))) };
                        members_builder.add(Member { ._tag = Member::Concept, ._Concept = concept });
                    }
                    break;
                    case ConstituentSyntax::Function: {
                        auto function_syntax = member->constituent._Function;
                        auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file);
                        if (function_result._tag == Result<Function, ModelError>::Error)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = function_result._Error };
                        auto function = function_result._Ok;
                        auto symbol_with_function_name = symbols_builder[function.name];
                        if (symbol_with_function_name != nullptr)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(member->constituent._Function.start, member->constituent._Function.end)))) };
                        functions_builder.add(function.name, function);
                        members_builder.add(Member { ._tag = Member::Function, ._Function = function });
                    }
                    break;
                    case ConstituentSyntax::Procedure: {
                        auto procedure_syntax = member->constituent._Procedure;
                        auto procedure_result = build_function(_rp, _ep, procedure_syntax.start, procedure_syntax.end, procedure_syntax.target, false, false, file);
                        if (procedure_result._tag == Result<Function, ModelError>::Error)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = procedure_result._Error };
                        auto function = procedure_result._Ok;
                        auto symbol_with_function_name = symbols_builder[function.name];
                        if (symbol_with_function_name != nullptr)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(member->constituent._Function.start, member->constituent._Function.end)))) };
                        functions_builder.add(function.name, function);
                        members_builder.add(Member { ._tag = Member::Function, ._Function = function });
                    }
                    break;
                    case ConstituentSyntax::Operator: {
                        auto operator_syntax = member->constituent._Operator;
                        auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, file);
                        if (operator_result._tag == Result<Structure, ModelError>::Error)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                                ._Error = operator_result._Error };
                        auto operator_ = operator_result._Ok;
                        if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
                        members_builder.add(Member { ._tag = Member::Operator, ._Operator = operator_ });
                    }
                    break;
                    case ConstituentSyntax::Implement:
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Implement"), Span(member->constituent._Implement.start, member->constituent._Implement.end)))) };
                    case ConstituentSyntax::Trait:
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Trait"), Span(member->constituent._Trait.start, member->constituent._Trait.end)))) };
                    case ConstituentSyntax::Macro:
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Macro"), Span(member->constituent._Macro.start, member->constituent._Macro.end)))) };
                    case ConstituentSyntax::Module: {
                        auto module_syntax = member->constituent._Module;
                        auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                        if (module_result._tag == Result<Structure, ModelError>::Error)
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                                ._Error = module_result._Error };
                        auto module = module_result._Ok;
                        if (!modules_checker.add(module.name))
                            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                        modules.add(module);
                    }
                    break;
                }
            }
        }

        auto multi_map = MultiMap<String, Function>(_r.get_page(), functions_builder);
        auto multi_map_iterator = multi_map.get_iterator();
        while (auto functions = multi_map_iterator.next()) {
            auto name = (*functions)[0]->name;
            symbols_builder.add(String(name), Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, *functions) });
        }

        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_syntax.start, class_syntax.end), private_, Vector<Property>(_rp, properties_builder), Vector<Module>(_rp, modules), Vector<Use>(_rp, uses), Vector<Initializer>(_rp, initializers_builder), deInitializer, Vector<Member>(_rp, members_builder), HashMap<String, Nameable>(_rp, symbols_builder))
        };
    }
}

Result<Namespace, ModelError> handle_namespace(Page* _rp, Page* _ep, String name, String path, NamespaceSyntax& namespace_syntax, bool private_, String file) {    
    Region _r;
    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.get_page()) HashMapBuilder<String, Nameable>();
    Array<Member>& members_builder = *new(alignof(Array<Member>), _r.get_page()) Array<Member>();
    MultiMapBuilder<String, Function>& functions_builder = *new(alignof(MultiMapBuilder<String, Function>), _r.get_page()) MultiMapBuilder<String, Function>();
    HashSetBuilder<String>& modules_checker = *new(alignof(HashSetBuilder<String>), _r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new(alignof(List<Module>), _r.get_page()) List<Module>();

    auto declarations_iterator = namespace_syntax.declarations->get_iterator();
    while (auto declaration = declarations_iterator.next()) {
        switch (declaration->symbol._tag) {
            case SymbolSyntax::Private:{
                auto export_ = declaration->symbol._Private.export_;
                switch (export_._tag) {
                    case ExportSyntax::Definition: {
                        auto concept_result = handle_definition(_rp, _ep, path, export_._Definition, true, file);
                        if (concept_result._tag == Result<Function, ModelError>::Error)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = concept_result._Error };
                        auto concept = concept_result._Ok;
                        if (functions_builder.contains(concept.name))
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(declaration->symbol._Definition.start, declaration->symbol._Definition.end)))) };

                        if (!symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept }))
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(DuplicateName(concept.name, Span(export_._Definition.start, export_._Definition.end)))) };
                        members_builder.add(Member { ._tag = Member::Concept, ._Concept = concept });
                    }
                    break;
                    case ExportSyntax::Function: {
                        auto function_syntax = export_._Function;
                        auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, true, true, file);
                        if (function_result._tag == Result<Function, ModelError>::Error)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = function_result._Error };
                        auto function = function_result._Ok;
                        auto symbol_with_function_name = symbols_builder[function.name];
                        if (symbol_with_function_name != nullptr)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(declaration->symbol._Function.start, declaration->symbol._Function.end)))) };
                        functions_builder.add(function.name, function);
                        members_builder.add(Member { ._tag = Member::Function, ._Function = function });
                    }
                    break;
                    case ExportSyntax::Operator: {
                        auto operator_syntax = export_._Operator;
                        auto operator_result = handle_operator(_rp, _ep, operator_syntax, true, file);
                        if (operator_result._tag == Result<Namespace, ModelError>::Error)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = operator_result._Error };
                        auto operator_ = operator_result._Ok;
                        if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
                        members_builder.add(Member { ._tag = Member::Operator, ._Operator = operator_ });
                    }
                    break;
                    case ExportSyntax::Trait:
                        return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Private Trait"), Span(declaration->symbol._Trait.start, declaration->symbol._Trait.end)))) };
                    case ExportSyntax::Module: {
                        auto module_syntax = export_._Module;
                        auto module_result = handle_module(_rp, _ep, path, module_syntax, true);
                        if (module_result._tag == Result<Namespace, ModelError>::Error)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = module_result._Error };
                        auto module = module_result._Ok;
                        if (!modules_checker.add(module.name))
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                        modules.add(module);
                    }
                    break;
                }
            }
            break;
            case SymbolSyntax::Definition: {
                auto concept_result = handle_definition(_rp, _ep, path, declaration->symbol._Definition, false, file);
                if (concept_result._tag == Result<Function, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = concept_result._Error };
                auto concept = concept_result._Ok;
                if (functions_builder.contains(concept.name))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(declaration->symbol._Definition.start, declaration->symbol._Definition.end)))) };

                if (!symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept }))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(declaration->symbol._Definition.start, declaration->symbol._Definition.end)))) };
                members_builder.add(Member { ._tag = Member::Concept, ._Concept = concept });
            }
            break;
            case SymbolSyntax::Function: {
                auto function_syntax = declaration->symbol._Function;
                auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file);
                if (function_result._tag == Result<Function, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = function_result._Error };
                auto function = function_result._Ok;
                auto symbol_with_function_name = symbols_builder[function.name];
                if (symbol_with_function_name != nullptr)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(declaration->symbol._Function.start, declaration->symbol._Function.end)))) };
                functions_builder.add(function.name, function);
                members_builder.add(Member { ._tag = Member::Function, ._Function = function });
            }
            break;
            case SymbolSyntax::Operator: {
                auto operator_syntax = declaration->symbol._Operator;
                auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, file);
                if (operator_result._tag == Result<Namespace, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = operator_result._Error };
                auto operator_ = operator_result._Ok;
                if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
                members_builder.add(Member { ._tag = Member::Operator, ._Operator = operator_ });
            }
            break;
            case SymbolSyntax::Trait:
                return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Trait"), Span(declaration->symbol._Trait.start, declaration->symbol._Trait.end)))) };
            case SymbolSyntax::Macro:
                return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Macro"), Span(declaration->symbol._Macro.start, declaration->symbol._Macro.end)))) };
            case SymbolSyntax::Module: {
                auto module_syntax = declaration->symbol._Module;
                auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                if (module_result._tag == Result<Namespace, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = module_result._Error };
                auto module = module_result._Ok;
                if (!modules_checker.add(module.name))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                modules.add(module);
            }
            break;
        }
    }

    auto multi_map = MultiMap<String, Function>(_r.get_page(), functions_builder);
    auto multi_map_iterator = multi_map.get_iterator();
    while (auto functions = multi_map_iterator.next()) {
        auto name = (*functions)[0]->name;
        symbols_builder.add(String(name), Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, *functions) });
    }

    Span span(namespace_syntax.start, namespace_syntax.end);
    auto ret = Namespace(span, Vector<Module>(_rp, modules), Vector<Member>(_rp, members_builder), HashMap<String, Nameable>(_rp, symbols_builder));
    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Ok, ._Ok = ret };
}

Result<Union, ModelError> handle_union(Page* _rp, Page* _ep, String name, String path, UnionSyntax& union_, bool private_, String file) {    
    Region _r;
    Array<Variant>& variants_builder = *new(alignof(Array<Variant>), _r.get_page()) Array<Variant>();
    HashMapBuilder<String, Variant>& symbols_builder = *new(alignof(HashMapBuilder<String, Variant>), _r.get_page()) HashMapBuilder<String, Variant>();
    auto variant_syntaxes = union_.variants;
    auto _variants_iterator = variant_syntaxes->get_iterator();
    while (auto _variant_syntax = _variants_iterator.next()) {
        auto variant_syntax = *_variant_syntax;
        Type* type = nullptr;
        if (variant_syntax.annotation != nullptr) {
            auto _type_result = handle_type(_rp, _ep, variant_syntax.annotation->type, file);
            if (_type_result._tag == Result<Type, ModelError>::Error)
                return Result<Union, ModelError> { ._tag = Result<Union, ModelError>::Error, ._Error = _type_result._Error };
            type = _type_result._Ok;
        }

        List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
        if (variant_syntax.attributes != nullptr) {
            auto _attribute_iterator = variant_syntax.attributes->get_iterator();
            while (auto _attribute_syntax = _attribute_iterator.next()) {
                auto attribute_syntax = *_attribute_syntax;
                auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
                if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                    return Result<Union, ModelError> { ._tag = Result<Union, ModelError>::Error, ._Error = _attribute_result._Error };
                auto attribute = _attribute_result._Ok;
                attributes.add(attribute);
            }
        }
        auto variant = Variant(Span(variant_syntax.start, variant_syntax.end), variant_syntax.name, type, Vector<Attribute>(_rp, attributes));
        if (!symbols_builder.add(variant_syntax.name, variant))
            return Result<Union, ModelError> { ._tag = Result<Union, ModelError>::Error,
                ._Error = ModelError(ModelBuilderError(DuplicateName(variant_syntax.name, Span(variant_syntax.start, variant_syntax.end)))) };
        variants_builder.add(variant);
    }
    return Result<Union, ModelError> { ._tag = Result<Union, ModelError>::Ok,
        ._Ok = Union(Span(union_.start, union_.end), private_, Vector<Variant>(_rp, variants_builder), HashMap<String, Variant>(_rp, symbols_builder))
    };
}

Result<Vector<Operand>, ModelError> handle_operation(Page* _rp, Page* _ep, OperationSyntax& operation, String file);

Result<Action, ModelError> handle_action(Page* _rp, Page* _ep, ActionSyntax& action, String file);

Result<Catch, ModelError> handle_catch(Page* _rp, Page* _ep, CatchSyntax& catch_, String file) {
    Region _r;
    List<String>& name_builder = *new(alignof(List<String>), _r.get_page()) List<String>();
    name_builder.add(catch_.error.name);
    if (catch_.error.extensions != nullptr) {
        auto _name_iterator = catch_.error.extensions->get_iterator();
        while(auto _name = _name_iterator.next()) {
            name_builder.add(_name->name);
        }
    }
    auto _action_result =  handle_action(_rp, _ep, catch_.action, file);
    if (_action_result._tag == Result<Operand, ModelError>::Error)
        return Result<Catch, ModelError> { ._tag = Result<Catch, ModelError>::Error, ._Error = _action_result._Error };
    auto action = _action_result._Ok;
    return Result<Catch, ModelError> { ._tag = Result<Catch, ModelError>::Ok, ._Ok = Catch(Span(catch_.start, catch_.end), catch_.name, Vector<String>(_rp, name_builder), action) };
}

Result<Drop*, ModelError> handle_drop(Page* _rp, Page* _ep, DropSyntax& drop_, String file) {
    Region _r;
    auto _action_result =  handle_action(_rp, _ep, drop_.action, file);
    if (_action_result._tag == Result<Operand, ModelError>::Error)
        return Result<Drop*, ModelError> { ._tag = Result<Drop*, ModelError>::Error, ._Error = _action_result._Error };
    auto handler = _action_result._Ok;
    return Result<Drop*, ModelError> { ._tag = Result<Drop*, ModelError>::Ok, ._Ok = new(alignof(Drop), _rp) Drop(Span(drop_.start, drop_.end), handler) };
}

Result<Catcher, ModelError> handle_catcher(Page* _rp, Page* _ep, CatcherSyntax& catcher, String file) {
    Region _r;
    List<Catch>& catches_builder = *new(alignof(List<Catch>), _r.get_page()) List<Catch>();
    if (catcher.catches != nullptr) {
        auto catch_syntaxes = catcher.catches;
        auto _catches_iterator = catch_syntaxes->get_iterator();
        while (auto _catch_syntax = _catches_iterator.next()) {
            auto catch_syntax = *_catch_syntax;
            auto _catch_result = handle_catch(_rp, _ep, catch_syntax, file);
            if (_catch_result._tag == Result<Catch, ModelError>::Error)
                return Result<Catcher, ModelError> { ._tag = Result<Catcher, ModelError>::Error, ._Error = _catch_result._Error };
            auto catch_ = _catch_result._Ok;
            catches_builder.add(catch_);
        }
    }
    Drop* drop_ = nullptr;
    if (catcher.dropper != nullptr) {
        auto drop_syntax = *catcher.dropper;
        auto _drop_result = handle_drop(_rp, _ep, drop_syntax, file);
        if (_drop_result._tag == Result<Drop, ModelError>::Error)
            return Result<Catcher, ModelError> { ._tag = Result<Catcher, ModelError>::Error, ._Error = _drop_result._Error };
        drop_ = _drop_result._Ok;
    }
    return Result<Catcher, ModelError> { ._tag = Result<Catcher, ModelError>::Ok, ._Ok = Catcher(Span(catcher.start, catcher.end), Vector<Catch>(_rp, catches_builder), drop_) };
}

Result<Constant, ModelError> handle_literal(Page* _rp, Page* _ep, LiteralSyntax& literal, String file) {
    Region _r;
    switch (literal.literal._tag) {
        case Literal::Boolean:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError> ::Ok, ._Ok = Constant { ._tag = Constant::Boolean, ._Boolean = literal.literal._Boolean.value } };
        case Literal::Integer: {
            auto integer = (size_t)strtol(literal.literal._Integer.value.to_c_string(_r.get_page()), nullptr, 10);
            const bool range_error = errno == ERANGE;
            if (range_error)
                return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))) };

            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError> ::Ok, ._Ok = Constant { ._tag = Constant::Integer, ._Integer = integer } };
        }
        case Literal::Hex: {
            auto hex = (size_t)strtoul(literal.literal._Hex.value.to_c_string(_r.get_page()), nullptr, 16);
            const bool range_error = errno == ERANGE;
            if (range_error)
                return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))) };

            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError> ::Ok, ._Ok = Constant { ._tag = Constant::Hex, ._Hex = hex } };
        }
        case Literal::FloatingPoint: {
            auto floating_point = strtod(literal.literal._FloatingPoint.value.to_c_string(_r.get_page()), nullptr);
            const bool range_error = errno == ERANGE;
            if (range_error)
                return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidConstant(file, Span(literal.start, literal.end)))) };

            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::FloatingPoint, ._FloatingPoint = floating_point } };
        }
        case Literal::String:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::String, ._String = literal.literal._String.value } };
        case Literal::Character:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::Character, ._Character = literal.literal._Character.value } };
        case Literal::Fragment:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::Fragment, ._Fragment = literal.literal._Fragment.value } };
        break;
    }
}

Result<Type*, ModelError> handle_type(Page* _rp, Page* _ep, TypeSyntax& type, String file) {
    Region _r;

    List<String>& path = *new(alignof(List<String>), _r.get_page()) List<String>();
    path.add(String(_rp, type.name.name));
    if (type.name.extensions != nullptr) {
        auto extensions = type.name.extensions;
        auto _extensions_iterator = extensions->get_iterator();
        while (auto _extension = _extensions_iterator.next()) {
            auto extension = *_extension;
            path.add(extension.name);
        }
    }

    Vector<Type>* generics = nullptr;
    Lifetime lifetime = Lifetime(Unspecified());
    List<Type>& generics_builder = *new(alignof(List<Type>), _r.get_page()) List<Type>();
    if (type.generics != nullptr) {
        auto generic_arguments = type.generics->generics;
        if (generic_arguments != nullptr) {
            auto _generics_iterator = generic_arguments->get_iterator();
            while (auto _generic = _generics_iterator.next()) {
                auto generic = *_generic;
                auto _type_result = handle_type(_rp, _ep, generic.type, file);
                if (_type_result._tag == Result<Type*, ModelError>::Error)
                    return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Error, ._Error = _type_result._Error };
                auto type = _type_result._Ok;
                generics_builder.add(*type);
            }

            generics = new(alignof(Vector<Type>), _rp) Vector<Type>(_rp, generics_builder);
        } else {
            generics = new(alignof(Vector<Type>), _rp) Vector<Type>();
        }
    }

    if (type.lifetime != nullptr) {
        auto lifetime_syntax = *type.lifetime;
        switch (lifetime_syntax._tag) {
            case LifetimeSyntax::Call:
                lifetime = Lifetime(Call());
            break;
            case LifetimeSyntax::Local:
                lifetime = Lifetime(Local());
            break;
            case LifetimeSyntax::Reference:
                lifetime = Lifetime(Reference(String(_rp, lifetime_syntax._Reference.location)));
            break;
            case LifetimeSyntax::Thrown:
                lifetime = Lifetime(Thrown());
            break;
        }
    }

    return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Ok, ._Ok = new(alignof(Type), _rp) Type(Span(type.start, type.end), Vector<String>(_rp, path), generics, lifetime) };
}

Result<Type*, ModelError> handle_binding_annotation(Page* _rp, Page* _ep, BindingAnnotationSyntax& binding_annotation, String file) {
    switch (binding_annotation.spec._tag) {
        case BindingSpecSyntax::Type: {
            auto type_result = handle_type(_rp, _ep, binding_annotation.spec._Type, file);
            if (type_result._tag == Result<Type, ModelError>::Error)
                return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Error, ._Error = type_result._Error };
            return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Ok, ._Ok = type_result._Ok };
        }
        case BindingSpecSyntax::Structure:
            return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Structure"), Span(binding_annotation.spec._Structure.start, binding_annotation.spec._Structure.end)))) };
        case BindingSpecSyntax::Array:
            return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Array"), Span(binding_annotation.spec._Array.start, binding_annotation.spec._Array.end)))) };

    }
}

Result<Break, ModelError> handle_break(Page* _rp, Page* _ep, BreakSyntax& return_, String file) {
    Region _r;
    if (return_.result != nullptr) {
        auto result = return_.result;
        auto result_result = handle_operands(_rp, _ep, result, file);
        if (result_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Break, ModelError> { ._tag = Result<Break, ModelError>::Error, ._Error = result_result._Error };
        return Result<Break, ModelError> { ._tag = Result<Break, ModelError>::Ok, ._Ok = Break(Span(return_.start, return_.end), result_result._Ok) };
    }
    return Result<Break, ModelError> { ._tag = Result<Break, ModelError>::Ok, ._Ok = Break(Span(return_.start, return_.end), Vector<Operand>(_rp, 0)) };
}

Result<Return, ModelError> handle_return(Page* _rp, Page* _ep, ReturnSyntax& return_, String file) {
    Region _r;
    if (return_.result != nullptr) {
        auto result = return_.result;
        auto result_result = handle_operands(_rp, _ep, result, file);
        if (result_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Error, ._Error = result_result._Error };
        return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(return_.start, return_.end), result_result._Ok) };
    }
    return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(return_.start, return_.end), Vector<Operand>(_rp, 0)) };
}

Result<Return, ModelError> handle_throw(Page* _rp, Page* _ep, ThrowSyntax& throw_, String file) {
    Region _r;
    if (throw_.result != nullptr) {
        auto result = throw_.result;
        auto result_result = handle_operands(_rp, _ep, result, file);
        if (result_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Error, ._Error = result_result._Error };
        return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(throw_.start, throw_.end), result_result._Ok) };
    }
    return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(throw_.start, throw_.end), Vector<Operand>(_rp, 0)) };
}

Result<Statement, ModelError> handle_command(Page* _rp, Page* _ep, CommandSyntax& command, String file) {
    Region _r;
    switch (command._tag)
    {
        case CommandSyntax::Operation: {
            auto operation_result = handle_operation(_rp, _ep, command._Operation, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Action, ._Action = Action(operation_result._Ok, Vector<Operand>())}};
        }
        case CommandSyntax::Let: {
            auto binding = command._Let.binding.operation;
            Type* type = nullptr;
            if (command._Let.binding.annotation != nullptr) {
                auto _type_result = handle_binding_annotation(_rp, _ep, *command._Let.binding.annotation, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = _type_result._Error };
                type = _type_result._Ok;
            }
            auto operation_result = handle_operands(_rp, _ep, binding, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Constant, Item(Span(command._Let.start, command._Let.end), false, new(alignof(String), _rp) String(_rp, command._Let.binding.name), type, Vector<Attribute>(_rp, 0)), operation_result._Ok) }};
        }
        case CommandSyntax::Var: {
            auto binding = command._Var.binding.operation;
            Type* type = nullptr;
            if (command._Var.binding.annotation != nullptr) {
                auto _type_result = handle_binding_annotation(_rp, _ep, *command._Var.binding.annotation, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = _type_result._Error };
                type = _type_result._Ok;
            }
            auto operation_result = handle_operands(_rp, _ep, binding, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Extendable, Item(Span(command._Var.start, command._Var.end), false, new(alignof(String), _rp) String(_rp, command._Mutable.binding.name), type, Vector<Attribute>(_rp, 0)), operation_result._Ok) }};
        }
        case CommandSyntax::Mutable: {
            auto binding = command._Mutable.binding.operation;
            Type* type = nullptr;
            if (command._Mutable.binding.annotation != nullptr) {
                auto _type_result = handle_binding_annotation(_rp, _ep, *command._Mutable.binding.annotation, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = _type_result._Error };
                type = _type_result._Ok;
            }
            auto operation_result = handle_operands(_rp, _ep, binding, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Mutable, Item(Span(command._Mutable.start, command._Mutable.end), false, new(alignof(String), _rp) String(_rp, command._Mutable.binding.name), type, Vector<Attribute>(_rp, 0)), operation_result._Ok) }};
        }
        case CommandSyntax::Set: {
            auto _target_result = handle_operands(_rp, _ep, command._Set.target, file);
            if (_target_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = _target_result._Error };
            auto target = _target_result._Ok;
            auto _source_result = handle_operands(_rp, _ep, command._Set.source, file);
            if (_source_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = _source_result._Error };
            auto source = _source_result._Ok;
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Action, ._Action = Action(source, target)}};
        }
        case CommandSyntax::Continue:
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Continue"), Span(command._Continue.start, command._Continue.end)))) };
        case CommandSyntax::Break: {
            auto break_ = command._Break;
            auto break_result = handle_break(_rp, _ep, break_, file);
            if (break_result._tag == Result<Expression, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = break_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Break, ._Break = break_result._Ok}};
        }
        case CommandSyntax::Return: {
            auto return_ = command._Return;
            auto return_result = handle_return(_rp, _ep, return_, file);
            if (return_result._tag == Result<Expression, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = return_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Return, ._Return = return_result._Ok}};
        }
        case CommandSyntax::Throw: {
            auto throw_ = command._Throw;
            auto throw_result = handle_throw(_rp, _ep, throw_, file);
            if (throw_result._tag == Result<Expression, ModelError>::Error)
                return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = throw_result._Error };
            return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Ok, ._Ok = Statement { ._tag = Statement::Throw, ._Return = throw_result._Ok}};
        }
    }
}

Result<Statement, ModelError> handle_statement(Page* _rp, Page* _ep, StatementSyntax& statement, String file) {
    auto result = handle_command(_rp, _ep, statement.command, file);
    if (result._tag == Result<Statement, ModelError>::Error)
        return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError>::Error, ._Error = result._Error };
    return Result<Statement, ModelError> { ._tag = Result<Statement, ModelError> ::Ok, ._Ok = result._Ok };
}

Result<Vector<Statement>, ModelError> handle_statements(Page* _rp, Page* _ep, Vector<StatementSyntax>& statements, String file) {
    Region _r;
    List<Statement>& statements_builder = *new(alignof(List<Statement>), _r.get_page()) List<Statement>();
    auto _statements_iterator = statements.get_iterator();
    while (auto statement = _statements_iterator.next()) {
        auto _statement_result = handle_statement(_rp, _ep, *statement, file);
        if (_statement_result._tag == Result<Statement, ModelError>::Error)
            return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _statement_result._Error };
        statements_builder.add(_statement_result._Ok);
    }
    
    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError> ::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
}

Result<Component, ModelError> handle_component(Page* _rp, Page* _ep, ComponentSyntax& component, String file) {
    Region _r;
    String* name = nullptr;

    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (component.attributes != nullptr) {
        auto definition_attributes = component.attributes;
        auto _attribute_iterator = definition_attributes->get_iterator();
        while (auto _attribute_syntax = _attribute_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;
            auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
            if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = _attribute_result._Error };
            auto attribute = _attribute_result._Ok;
            attributes.add(attribute);
        }
    }

    if (component.value != nullptr) {
        if (component.operands != nullptr) {
            auto name_operands = *component.operands;
            if (name_operands.length > 0) {
                auto name_operand = *name_operands[0];
                if (name_operands.length > 1)
                    return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidComponentName(file, Span(name_operand.start, name_operand.end)))) };
                if (name_operand.expression._tag == ExpressionSyntax::Type)
                    name = new(alignof(String), _rp) String(_rp, name_operand.expression._Type.name.name);
            }
        }
        auto value = component.value->value;
        if (value != nullptr) {
            auto value_result = handle_operands(_rp, _ep, value, file);
            if (value_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = value_result._Error };
            else
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, value_result._Ok, Vector<Attribute>(_rp,attributes))};
        } else {
            return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, Vector<Operand>(_rp, 0), Vector<Attribute>(_rp,attributes))};
        }
    } else {
        if (component.operands != nullptr) {
            auto value_result = handle_operands(_rp, _ep, component.operands, file);
            if (value_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = value_result._Error };
            else
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, value_result._Ok, Vector<Attribute>(_rp,attributes))};
        } else {
            return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, Vector<Operand>(_rp, 0), Vector<Attribute>(_rp,attributes))};
        }
    }
}

Result<Tuple, ModelError> handle_object(Page* _rp, Page* _ep, ObjectSyntax& object, String file) {
    Region _r;
    List<Component>& components_builder = *new(alignof(List<Component>), _r.get_page()) List<Component>();
    if (object.components != nullptr) {
        auto components_iterator = object.components->get_iterator();
        while (auto component = components_iterator.next()) {
            auto component_result =  handle_component(_rp, _ep, *component, file);
            if (component_result._tag == Result<Operand, ModelError>::Error)
                return Result<Tuple, ModelError> { ._tag = Result<Tuple, ModelError>::Error, ._Error = component_result._Error };
            components_builder.add(component_result._Ok);
        }
    }
    return Result<Tuple, ModelError> { ._tag = Result<Tuple, ModelError>::Ok, ._Ok = Tuple(Span(object.start, object.end), Vector<Component>(_rp, components_builder)) };
}

Result<Matrix, ModelError> handle_vector(Page* _rp, Page* _ep, VectorSyntax& vector, String file) {
    Region _r;
    List<Vector<Operand>>& operations_builder = *new(alignof(List<Vector<Operand>>), _r.get_page()) List<Vector<Operand>>();
    if (vector.elements != nullptr) {
        auto elements_iterator = vector.elements->get_iterator();
        while (auto element = elements_iterator.next()) {
            auto operation_result =  handle_operands(_rp, _ep, element->operation, file);
            if (operation_result._tag == Result<Operand, ModelError>::Error)
                return Result<Matrix, ModelError> { ._tag = Result<Matrix, ModelError>::Error, ._Error = operation_result._Error };
            operations_builder.add(operation_result._Ok);
        }
    }
    return Result<Matrix, ModelError> { ._tag = Result<Matrix, ModelError>::Ok, ._Ok = Matrix(Span(vector.start, vector.end), Vector<Vector<Operand>>(_rp, operations_builder)) };
}

Result<Model, ModelError> handle_model(Page* _rp, Page* _ep, ModelSyntax& model, String file) {
    switch (model._tag) {
        case ModelSyntax::Literal: {
            auto literal = model._Literal;
            auto constant_result = handle_literal(_rp, _ep, literal, file);
            if (constant_result._tag == Result<Constant, ModelError>::Error)
                return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = constant_result._Error };
            return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = Model { ._tag = Model::Constant, ._Constant = constant_result._Ok} };
        }
        case ModelSyntax::Name: {
            auto variable = model._Name;
            return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = Model { ._tag = Model::Variable, ._Variable = String(_rp, variable.name)} };
        }
        case ModelSyntax::Object: {
            auto object = model._Object;
            auto object_result = handle_object(_rp, _ep, object, file);
            if (object_result._tag == Result<Expression, ModelError>::Error)
                return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = object_result._Error };
            return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = Model { ._tag = Model::Tuple, ._Tuple = object_result._Ok} };
        }
        case ExpressionSyntax::Vector: {
            auto vector = model._Vector;
            auto vector_result = handle_vector(_rp, _ep, vector, file);
            if (vector_result._tag == Result<Expression, ModelError>::Error)
                return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = vector_result._Error };
            return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = Model { ._tag = Model::Matrix, ._Matrix = vector_result._Ok} };
        }
    }
}

Result<Attribute, ModelError> handle_attribute(Page* _rp, Page* _ep, AttributeSyntax& attribute, String file) {
    auto _model_result = handle_model(_rp, _ep, attribute.model, file);
    if (_model_result._tag == Result<Model, ModelError>::Error)
        return Result<Attribute, ModelError> { ._tag = Result<Attribute, ModelError>::Error, ._Error = _model_result._Error };
    auto model = _model_result._Ok;
    return Result<Attribute, ModelError> { ._tag = Result<Attribute, ModelError>::Ok, ._Ok = Attribute(Span(attribute.start, attribute.end), attribute.name, model) };
}

Result<Block, ModelError> handle_block(Page* _rp, Page* _ep, BlockSyntax& block, String file) {
    Region _r;
    if (block.statements != nullptr) {
        auto _statements_result = handle_statements(_rp, _ep, *block.statements, file);
        if (_statements_result._tag == Result<Vector<Statement>, ModelError>::Error)
            return Result<Block, ModelError> { ._tag = Result<Block, ModelError>::Error, ._Error = _statements_result._Error };
        auto statements = _statements_result._Ok;
        return Result<Block, ModelError> { ._tag = Result<Block, ModelError>::Ok, ._Ok = Block(Span(block.start, block.end), statements) };
    }

    return Result<Block, ModelError> { ._tag = Result<Block, ModelError>::Ok, ._Ok = Block(Span(block.start, block.end), Vector<Statement>(_rp, 0)) };
}

Result<If, ModelError> handle_if(Page* _rp, Page* _ep, IfSyntax& if_, String file) {
    Region _r;
    Property* property = nullptr;
    auto condition_result = handle_operands(_rp, _ep, if_.condition, file);
    if (condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = condition_result._Error };
    auto condition = condition_result._Ok;
    auto _consequent_result = handle_command(_rp, _ep, if_.consequent, file);
    if (_consequent_result._tag == Result<Action, ModelError>::Error)
        return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = _consequent_result._Error };
    auto consequent = _consequent_result._Ok;

    Statement* alternative = nullptr;
    if (if_.alternative != nullptr) {
        auto _alternative_result = handle_command(_rp, _ep, if_.alternative->alternative, file);
        if (_alternative_result._tag == Result<Statement, ModelError>::Error)
            return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = _alternative_result._Error };
        alternative = new(alignof(Statement), _rp) Statement(_alternative_result._Ok);
    }
    return Result<If, ModelError> { ._tag = Result<If, ModelError>::Ok, ._Ok = If(Span(if_.start, if_.end), condition, property, consequent, alternative) };
}

Result<Match, ModelError> handle_match(Page* _rp, Page* _ep, MatchSyntax& match_, String file) {
    Region _r;
    Property* property = nullptr;

    auto condition_result = handle_operands(_rp, _ep, match_.scrutinee, file);
    if (condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = condition_result._Error };
    auto condition = condition_result._Ok;

    List<Case>& cases_builder = *new(alignof(List<Case>), _r.get_page()) List<Case>();
    if (match_.cases != nullptr) {
        auto case_iterator = match_.cases->get_iterator();
        while (auto case_ = case_iterator.next()) {
            auto condition_result =  handle_operands(_rp, _ep, case_->condition, file);
            if (condition_result._tag == Result<Operand, ModelError>::Error)
                return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = condition_result._Error };
            auto condition = condition_result._Ok;
            auto _consequent_result = handle_command(_rp, _ep, case_->consequent, file);
            if (_consequent_result._tag == Result<Action, ModelError>::Error)
                return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = _consequent_result._Error };
            auto consequent = Case(Span(case_->start, case_->end), condition, _consequent_result._Ok);
            cases_builder.add(consequent);
        }
    }

    Statement* alternative = nullptr;
    if (match_.alternative != nullptr) {
        auto _alternative_result = handle_command(_rp, _ep, match_.alternative->alternative, file);
        if (_alternative_result._tag == Result<Statement, ModelError>::Error)
            return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = _alternative_result._Error };
        alternative = new(alignof(Statement), _rp) Statement(_alternative_result._Ok);
    }
    return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Ok, ._Ok = Match(Span(match_.start, match_.end), condition, Vector<Case>(_rp, cases_builder), alternative) };
}

Result<For, ModelError> handle_for(Page* _rp, Page* _ep, ForSyntax& for_, String file) {
    Region _r;
    auto expression_result = handle_operands(_rp, _ep, for_.operation, file);
    if (expression_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<For, ModelError> { ._tag = Result<For, ModelError>::Error, ._Error = expression_result._Error };
    auto expression = expression_result._Ok;
    if (for_.name != nullptr)
        return Result<For, ModelError> { ._tag = Result<For, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Label in For"), Span(for_.name->start, for_.name->end)))) };

    auto _action_result = handle_action(_rp, _ep, for_.action, file);
    if (_action_result._tag == Result<Action, ModelError>::Error)
        return Result<For, ModelError> { ._tag = Result<For, ModelError>::Error, ._Error = _action_result._Error };
    auto action = Action(_action_result._Ok);
    return Result<For, ModelError> { ._tag = Result<For, ModelError>::Ok, ._Ok = For(Span(for_.start, for_.end), String(_rp, for_.variable), expression, action) };
}

Result<Binding, ModelError> handle_condition(Page* _rp, Page* _ep, ConditionSyntax& condition, String file) {
    Region _r;
    switch (condition._tag)
    {
        case ConditionSyntax::Operation: {
            auto operation_result = handle_operation(_rp, _ep, condition._Operation, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Binding, ModelError> { ._tag = Result<Binding, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Binding, ModelError> { ._tag = Result<Binding, ModelError>::Ok, ._Ok = Binding(Binding::Mutability::Constant, Item(Span(condition._Operation.start, condition._Operation.end), false, nullptr, nullptr, Vector<Attribute>(_rp, 0)), operation_result._Ok)};
        }
        case ConditionSyntax::Let: {
            auto binding = condition._Let.binding.operation;
            Type* type = nullptr;
            if (condition._Let.binding.annotation != nullptr) {
                auto _type_result = handle_binding_annotation(_rp, _ep, *condition._Let.binding.annotation, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Binding, ModelError> { ._tag = Result<Binding, ModelError>::Error, ._Error = _type_result._Error };
                type = _type_result._Ok;
            }
            auto operation_result = handle_operands(_rp, _ep, binding, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Binding, ModelError> { ._tag = Result<Binding, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Binding, ModelError> { ._tag = Result<Binding, ModelError>::Ok, ._Ok = Binding(Binding::Mutability::Constant, Item(Span(condition._Let.start, condition._Let.end), false, new(alignof(String), _rp) String(_rp, condition._Let.binding.name), nullptr, Vector<Attribute>(_rp, 0)), operation_result._Ok)};
        }
    }
}

Result<While, ModelError> handle_while(Page* _rp, Page* _ep, WhileSyntax& while_, String file) {
    Region _r;
    auto _condition_result = handle_condition(_rp, _ep, while_.condition, file);
    if (_condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<While, ModelError> { ._tag = Result<While, ModelError>::Error, ._Error = _condition_result._Error };
    auto condition = _condition_result._Ok;
    if (while_.name != nullptr)
        return Result<While, ModelError> { ._tag = Result<While, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Label in While"), Span(while_.name->start, while_.name->end)))) };

    auto _action_result = handle_action(_rp, _ep, while_.action, file);
    if (_action_result._tag == Result<Action, ModelError>::Error)
        return Result<While, ModelError> { ._tag = Result<While, ModelError>::Error, ._Error = _action_result._Error };
    auto action = Action(_action_result._Ok);
    return Result<While, ModelError> { ._tag = Result<While, ModelError>::Ok, ._Ok = While(Span(while_.start, while_.end), condition, action) };
}

Result<Try, ModelError> handle_try(Page* _rp, Page* _ep, TrySyntax& try_, String file) {
    Region _r;
    auto _condition_result = handle_condition(_rp, _ep, try_.condition, file);
    if (_condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<Try, ModelError> { ._tag = Result<Try, ModelError>::Error, ._Error = _condition_result._Error };
    auto condition = _condition_result._Ok;

    auto _catcher_result = handle_catcher(_rp, _ep, try_.catcher, file);
    if (_catcher_result._tag == Result<Action, ModelError>::Error)
        return Result<Try, ModelError> { ._tag = Result<Try, ModelError>::Error, ._Error = _catcher_result._Error };
    auto catcher = _catcher_result._Ok;
    return Result<Try, ModelError> { ._tag = Result<Try, ModelError>::Ok, ._Ok = Try(Span(try_.start, try_.end), condition, catcher) };
}

Result<SizeOf, ModelError> handle_size_of(Page* _rp, Page* _ep, SizeOfSyntax& size_of, String file) {
    Region _r;
    auto type_result = handle_type(_rp, _ep, size_of.type, file);
    if (type_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<SizeOf, ModelError> { ._tag = Result<SizeOf, ModelError>::Error, ._Error = type_result._Error };
    auto type = type_result._Ok;
    return Result<SizeOf, ModelError> { ._tag = Result<SizeOf, ModelError>::Ok, ._Ok = SizeOf(Span(size_of.start, size_of.end), *type) };
}

Result<Expression, ModelError> handle_expression(Page* _rp, Page* _ep, ExpressionSyntax& expression, String file) {
    Region _r;
    switch (expression._tag) {
        case ExpressionSyntax::Literal: {
            auto literal = expression._Literal;
            auto constant_result = handle_literal(_rp, _ep, literal, file);
            if (constant_result._tag == Result<Constant, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = constant_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Constant, ._Constant = constant_result._Ok} };
        }
        case ExpressionSyntax::Type: {
            auto type = expression._Type;
            auto _type_result = handle_type(_rp, _ep, type, file);
            if (_type_result._tag == Result<Type, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = _type_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Type, ._Type = *_type_result._Ok} };
        }
        case ExpressionSyntax::Object: {
            auto object = expression._Object;
            auto object_result = handle_object(_rp, _ep, object, file);
            if (object_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = object_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Tuple, ._Tuple = object_result._Ok} };
        }
        case ExpressionSyntax::Vector: {
            auto vector = expression._Vector;
            auto vector_result = handle_vector(_rp, _ep, vector, file);
            if (vector_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = vector_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Matrix, ._Matrix = vector_result._Ok} };
        }
        case ExpressionSyntax::Block: {
            auto block = expression._Block;
            auto block_result = handle_block(_rp, _ep, block, file);
            if (block_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = block_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Block, ._Block = block_result._Ok} };
        }
        case ExpressionSyntax::If: {
            auto if_ = expression._If;
            auto if_result = handle_if(_rp, _ep, if_, file);
            if (if_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = if_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::If, ._If = if_result._Ok} };
        }
        case ExpressionSyntax::Match: {
            auto match_ = expression._Match;
            auto match_result = handle_match(_rp, _ep, match_, file);
            if (match_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = match_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Match, ._Match = match_result._Ok} };
        }
        case ExpressionSyntax::Lambda:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Lambda"), Span(expression._Lambda.start, expression._Lambda.end)))) };
        case ExpressionSyntax::For: {
            auto for_ = expression._For;
            auto for_result = handle_for(_rp, _ep, for_, file);
            if (for_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = for_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::For, ._For = for_result._Ok} };
        }
        case ExpressionSyntax::While: {
            auto while_syntax = expression._While;
            auto _while_result = handle_while(_rp, _ep, while_syntax, file);
            if (_while_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = _while_result._Error };
            auto while_ = _while_result._Ok;
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::While, ._While = while_} };
        }
        case ExpressionSyntax::Try: {
            auto try_syntax = expression._Try;
            auto _try_result = handle_try(_rp, _ep, try_syntax, file);
            if (_try_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = _try_result._Error };
            auto try_ = _try_result._Ok;
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Try, ._Try = try_} };
        }
        case ExpressionSyntax::Repeat:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Repeat"), Span(expression._Repeat.start, expression._Repeat.end)))) };
        case ExpressionSyntax::SizeOf: {
            auto size_of = expression._SizeOf;
            auto size_of_result = handle_size_of(_rp, _ep, size_of, file);
            if (size_of_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = size_of_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::SizeOf, ._SizeOf = size_of_result._Ok} };
        }
    }
}

Result<Operand, ModelError> handle_operand(Page* _rp, Page* _ep, OperandSyntax& operand, String file) {
    Region _r;
    Vector<String>* member_access = nullptr;
    if (operand.members != nullptr) {
        List<String>& path = *new(alignof(List<String>), _r.get_page()) List<String>();
        auto _member_iterator = operand.members->get_iterator();
        while (auto _member = _member_iterator.next()) {
            auto member = *_member;
            path.add(String(_rp, member.name.name));
            if (member.name.extensions != nullptr) {
                auto extensions = member.name.extensions;
                auto _extensions_iterator = extensions->get_iterator();
                while (auto _extension = _extensions_iterator.next()) {
                    auto extension = *_extension;
                    path.add(extension.name);
                }
            }

        }
        member_access = new(alignof(Vector<String>), _rp) Vector<String>(_rp, path);
    }

    auto expression_result = handle_expression(_rp, _ep, operand.expression, file);
    if (expression_result._tag == Result<Expression, ModelError>::Error)
        return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Error, ._Error = expression_result._Error };

    return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Ok, ._Ok = Operand(Span(operand.expression._Literal.start, operand.expression._Literal.end), expression_result._Ok, member_access) };
}

Result<Vector<Operand>, ModelError> handle_operands(Page* _rp, Page* _ep, Vector<OperandSyntax>* operands, String file) {
    Region _r;
    List<Operand>& operands_builder = *new(alignof(List<Operand>), _r.get_page()) List<Operand>();
    if (operands != nullptr)
    {
        auto operands_iterator = operands->get_iterator();
        while (auto operand = operands_iterator.next()) {
            auto operand_result = handle_operand(_rp, _ep, *operand, file);
            if (operand_result._tag == Result<Operand, ModelError>::Error)
                return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Error, ._Error = operand_result._Error };
            operands_builder.add(operand_result._Ok);
        }
    }
    return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Ok, ._Ok = (Vector<Operand>(_rp, operands_builder)) };
}

Result<Vector<Operand>, ModelError> handle_operation(Page* _rp, Page* _ep, OperationSyntax& operation, String file) {
    if (operation.operands != nullptr) {
        auto operands_result = handle_operands(_rp, _ep, operation.operands, file);
        if (operands_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Error, ._Error = operands_result._Error };
        return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Ok, ._Ok = operands_result._Ok };
    }
    return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Ok, ._Ok = Vector<Operand>(_rp, 0) };
}

Result<GenericParameter, ModelError> handle_generic_parameter(Page* _rp, Page* _ep, GenericParameterSyntax& generic_parameter, String file) {
    Region _r;
    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (generic_parameter.attributes != nullptr) {
        auto definition_attributes = generic_parameter.attributes;
        auto _attribute_iterator = definition_attributes->get_iterator();
        while (auto _attribute_syntax = _attribute_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;
            auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
            if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                return Result<GenericParameter, ModelError> { ._tag = Result<GenericParameter, ModelError>::Error, ._Error = _attribute_result._Error };
            auto attribute = _attribute_result._Ok;
            attributes.add(attribute);
        }
    }
    return Result<GenericParameter, ModelError> { ._tag = Result<GenericParameter, ModelError>::Ok, ._Ok = GenericParameter(Span(generic_parameter.start, generic_parameter.end), String(_rp, generic_parameter.name), Vector<Attribute>(_rp, attributes)) };
}

Result<Use, ModelError> handle_use(Page* _rp, Page* _ep, UseSyntax& use_) {
    Region _r;
    List<String>& path = *new(alignof(List<String>), _r.get_page()) List<String>();
    path.add(String(_rp, use_.name.name));
    if (use_.name.extensions != nullptr) {
        auto extensions = use_.name.extensions;
        auto _extensions_iterator = extensions->get_iterator();
        while (auto _extension = _extensions_iterator.next()) {
            auto extension = *_extension;
            path.add(extension.name);
        }
    }

    return Result<Use, ModelError> { ._tag = Result<Use, ModelError>::Ok, ._Ok = Use(Span(use_.start, use_.end), Vector<String>(_rp, path)) };
}

Result<Concept, ModelError> handle_definition(Page* _rp, Page* _ep, String path, DefinitionSyntax& definition, bool private_, String file) {
    Region _r;
    auto concept = definition.concept_;
    Span span(definition.start, definition.end);

    List<GenericParameter>& parameters = *new(alignof(List<GenericParameter>), _r.get_page()) List<GenericParameter>();
    if (definition.parameters != nullptr) {
        auto generic_parameters = *definition.parameters;
        if (generic_parameters.parameters != nullptr) {    
            auto _parameters_iterator = generic_parameters.parameters->get_iterator();
            while (auto _generic_parameter = _parameters_iterator.next()) {
                auto generic_parameter = *_generic_parameter;
                auto _parameter_result = handle_generic_parameter(_rp, _ep, generic_parameter, file);
                if (_parameter_result._tag == Result<GenericParameter, ModelError>::Error)
                    return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = _parameter_result._Error };
                auto parameter = _parameter_result._Ok;
                parameters.add(parameter);

            }
        }
    }

    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (definition.attributes != nullptr) {
        auto definition_attributes = definition.attributes;
        auto _attribute_iterator = definition_attributes->get_iterator();
        while (auto _attribute_syntax = _attribute_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;
            auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
            if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = _attribute_result._Error };
            auto attribute = _attribute_result._Ok;
            attributes.add(attribute);
        }
    }

    switch (concept._tag)
    {
        case ConceptSyntax::Class: {
            auto class_ = concept._Class;
            auto structure_result = handle_class(_rp, _ep, definition.name, path, class_, private_, file);
            if (structure_result._tag == Result<Structure, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = structure_result._Error };
            auto structure = structure_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name), Vector<GenericParameter>(_rp, parameters), Vector<Attribute>(_rp, attributes),
                    Definition { ._tag = Definition::Structure, ._Structure = structure }
                )};
        }
        case ConceptSyntax::Namespace: {
            auto namespace_syntax = concept._Namespace;
            auto _namespace_result = handle_namespace(_rp, _ep, definition.name, Path::join(_rp, path, definition.name), namespace_syntax, private_, file);
            if (_namespace_result._tag == Result<Namespace, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = _namespace_result._Error };
            auto namespace_ = _namespace_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name), Vector<GenericParameter>(_rp, parameters), Vector<Attribute>(_rp, attributes),
                    Definition { ._tag = Definition::Namespace, ._Namespace = namespace_ }
                )};
        }
        case ConceptSyntax::Union: {
            auto union_syntax = concept._Union;
            auto _union__result = handle_union(_rp, _ep, definition.name, path, union_syntax, private_, file);
            if (_union__result._tag == Result<Structure, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = _union__result._Error };
            auto union_ = _union__result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name), Vector<GenericParameter>(_rp, parameters), Vector<Attribute>(_rp, attributes),
                    Definition { ._tag = Definition::Union, ._Union = union_ }
                )};
        }
        case ConceptSyntax::Constant: {
            auto constant = concept._Constant;
            auto operation_result = handle_operands(_rp, _ep, constant.operation, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = operation_result._Error };
            auto _type_result = handle_type(_rp, _ep, constant.type, file);
            if (_type_result._tag == Result<Type, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = _type_result._Error };
            auto type = _type_result._Ok;
            auto operation = operation_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name), Vector<GenericParameter>(_rp, parameters), Vector<Attribute>(_rp, attributes),
                    Definition { ._tag = Definition::Global, ._Global = Global(span, *type, operation) }
                )};
        }
        case ConceptSyntax::Delegate:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Delegate"), Span(concept._Delegate.start, concept._Delegate.end)))) };
        case ConceptSyntax::Intrinsic:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name), Vector<GenericParameter>(_rp, parameters), Vector<Attribute>(_rp, attributes),
                    Definition { ._tag = Definition::Intrinsic }
                )};
    }
}

Result<Action, ModelError> handle_action(Page* _rp, Page* _ep, ActionSyntax& action, String file) {
    switch (action._tag) {
        case ActionSyntax::Operation: {
            auto operation_result = handle_operation(_rp, _ep, action._Operation, file);
            if (operation_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Ok, ._Ok = Action(operation_result._Ok, Vector<Operand>()) };
        }
        case ActionSyntax::Set: {
            auto _target_result = handle_operands(_rp, _ep, action._Set.target, file);
            if (_target_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = _target_result._Error };
            auto target = _target_result._Ok;
            auto _source_result = handle_operands(_rp, _ep, action._Set.source, file);
            if (_source_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = _source_result._Error };
            auto source = _source_result._Ok;
            return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Ok, ._Ok = Action(source, target) };
        }
    }
}

Result<Function, ModelError> build_function(Page* _rp, Page* _ep, size_t start, size_t end, TargetSyntax targetSyntax, bool private_, bool pure, String file) {
    Vector<Item> input = Vector<Item>(_rp, 0);
    Vector<Item> output = Vector<Item>(_rp, 0);
    switch (targetSyntax._tag) {
        case TargetSyntax::Routine:
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Non-Symbol Function"), Span(start, end)))) };
        case TargetSyntax::Named:
            auto named = targetSyntax._Named;

            if (named.routine.parameters != nullptr) {
                ParameterSetSyntax& parameterSetSyntax = *named.routine.parameters;
                auto _input_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                if (_input_result._tag == Result<Vector<Item>, ModelError>::Error)
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _input_result._Error };
                input = _input_result._Ok;
            }

            Type* returns_ = nullptr;
            if (named.routine.returns_ != nullptr) {
                auto _type_result = handle_type(_rp, _ep, named.routine.returns_->type, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _type_result._Error };
                returns_ = _type_result._Ok;
            }

            Type* throws_ = nullptr;
            if (named.routine.throws_ != nullptr) {
                auto _type_result = handle_type(_rp, _ep, named.routine.throws_->type, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _type_result._Error };
                throws_ = _type_result._Ok;
            }

            auto lifetime = Lifetime(Unspecified());
            if (named.routine.lifetime != nullptr) {
                switch (named.routine.lifetime->_tag) {
                    case LifetimeSyntax::Call:
                        lifetime = Lifetime(Call());
                    break;
                    case LifetimeSyntax::Local:
                        lifetime = Lifetime(Local());
                    break;
                    case LifetimeSyntax::Reference:
                        lifetime = Lifetime(Reference(String(_rp, named.routine.lifetime->_Reference.location)));
                    break;
                    case LifetimeSyntax::Thrown:
                        lifetime = Lifetime(Thrown());
                    break;    
                }
            }

            switch (named.routine.implementation._tag) {
                case ImplementationSyntax::Action: {
                    auto action_result = handle_action(_rp, _ep, named.routine.implementation._Action, file);
                    if (action_result._tag == Result<Statement, ModelError>::Error)
                        return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = action_result._Error };
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, named.name), input, returns_, throws_, lifetime, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
                }
                case ImplementationSyntax::Extern:
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, named.name), input, returns_, nullptr, lifetime, Implementation { ._tag = Implementation::Extern, ._Extern = Extern() }) };
                case ImplementationSyntax::Instruction:
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, named.name), input, returns_, nullptr, lifetime, Implementation { ._tag = Implementation::Instruction, ._Instruction = Instruction() }) };
                case ImplementationSyntax::Intrinsic:
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, named.end), input, returns_, nullptr, lifetime, Implementation { ._tag = Implementation::Intrinsic, ._Intrinsic = Intrinsic() }) };
                break;

            }
    }

}

Result<Initializer, ModelError> handle_initializer(Page* _rp, Page* _ep, InitSyntax& initializer, bool private_, String file) {
    Vector<Item> input = Vector<Item>(_rp, 0);

    if (initializer.parameters != nullptr) {
        ParameterSetSyntax& parameterSetSyntax = *initializer.parameters;
        auto parameterset_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
        input = parameterset_result._Ok;
    }

    auto action_result = handle_action(_rp, _ep, initializer.action, file);
    if (action_result._tag == Result<Statement, ModelError>::Error)
        return Result<Initializer, ModelError> { ._tag = Result<Initializer, ModelError>::Error, ._Error = action_result._Error };

    return Result<Initializer, ModelError> { ._tag = Result<Initializer, ModelError>::Ok, ._Ok = Initializer(Span(initializer.start, initializer.end), private_, input, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
}

Result<DeInitializer*, ModelError> handle_deinitializer(Page* _rp, Page* _ep, DeInitSyntax& deinitializer, String file) {
    auto action_result = handle_action(_rp, _ep, deinitializer.action, file);
    if (action_result._tag == Result<Statement, ModelError>::Error)
        return Result<DeInitializer*, ModelError> { ._tag = Result<DeInitializer*, ModelError>::Error, ._Error = action_result._Error };

    return Result<DeInitializer*, ModelError> { ._tag = Result<DeInitializer*, ModelError>::Ok, ._Ok = new(alignof(DeInitializer), _rp) DeInitializer(Span(deinitializer.start, deinitializer.end), Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
}

Result<Operator, ModelError> handle_operator(Page* _rp, Page* _ep,  OperatorSyntax& operator_syntax, bool private_, String file) {
    Vector<Item> input = Vector<Item>(_rp, 0);
    auto start = operator_syntax.start;
    auto end = operator_syntax.end;
    Vector<Operand>* operation = nullptr;
    switch (operator_syntax.target._tag) {
        case TargetSyntax::Routine: {
            auto routine = operator_syntax.target._Routine;
            if (routine.generics != nullptr) {
                if (routine.parameters != nullptr) {
                    ParameterSetSyntax& parameterSetSyntax = *routine.parameters;
                    auto _input_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                    if (_input_result._tag == Result<Vector<Item>, ModelError>::Error)
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _input_result._Error };
                    input = _input_result._Ok;
                }

                Type* returns_ = nullptr;
                if (routine.returns_ != nullptr) {
                    auto _type_result = handle_type(_rp, _ep, routine.returns_->type, file);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _type_result._Error };
                    returns_ = _type_result._Ok;
                }

                Type* throws_ = nullptr;
                if (routine.throws_ != nullptr) {
                    auto _type_result = handle_type(_rp, _ep, routine.throws_->type, file);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _type_result._Error };
                    throws_ = _type_result._Ok;
                }

                switch (routine.implementation._tag) {
                    case ImplementationSyntax::Action: {
                        auto action_result = handle_action(_rp, _ep, routine.implementation._Action, file);
                        if (action_result._tag == Result<Statement, ModelError>::Error)
                            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = action_result._Error };
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, "[]"), input, returns_, throws_, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
                    }
                    case ImplementationSyntax::Extern:
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, "[]"), input, returns_, throws_, Implementation { ._tag = Implementation::Extern, ._Extern = Extern() }) };
                    case ImplementationSyntax::Instruction:
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, "[]"), input, returns_, throws_, Implementation { ._tag = Implementation::Instruction, ._Instruction = Instruction() }) };
                    case ImplementationSyntax::Intrinsic:
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, "[]"), input, returns_, throws_, Implementation { ._tag = Implementation::Intrinsic, ._Intrinsic = Intrinsic() }) };
                }
            }
            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Non-Symbol Operator"), Span(operator_syntax.start, operator_syntax.end)))) };
        }
        case TargetSyntax::Named: {
            auto named = operator_syntax.target._Named;
            if (named.routine.parameters != nullptr) {
                ParameterSetSyntax& parameterSetSyntax = *named.routine.parameters;
                auto _input_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                if (_input_result._tag == Result<Vector<Item>, ModelError>::Error)
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _input_result._Error };
                input = _input_result._Ok;
            }

            Type* returns_ = nullptr;
            if (named.routine.returns_ != nullptr) {
                auto _type_result = handle_type(_rp, _ep, named.routine.returns_->type, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _type_result._Error };
                returns_ = _type_result._Ok;
            }

            Type* throws_ = nullptr;
            if (named.routine.throws_ != nullptr) {
                auto _type_result = handle_type(_rp, _ep, named.routine.throws_->type, file);
                if (_type_result._tag == Result<Type, ModelError>::Error)
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _type_result._Error };
                throws_ = _type_result._Ok;
            }

            switch (named.routine.implementation._tag) {
                case ImplementationSyntax::Action: {
                    auto action_result = handle_action(_rp, _ep, named.routine.implementation._Action, file);
                    if (action_result._tag == Result<Statement, ModelError>::Error)
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = action_result._Error };
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, named.name), input, returns_, throws_, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
                }
                case ImplementationSyntax::Extern:
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, named.name), input, returns_, throws_, Implementation { ._tag = Implementation::Extern, ._Extern = Extern() }) };
                case ImplementationSyntax::Instruction:
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, named.name), input, returns_, throws_, Implementation { ._tag = Implementation::Instruction, ._Instruction = Instruction() }) };
                case ImplementationSyntax::Intrinsic:
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, named.name), input, returns_, throws_, Implementation { ._tag = Implementation::Intrinsic, ._Intrinsic = Intrinsic() }) };
                break;
            }
        }
    }
}

Result<Module, ModelError> build_module(Page* _rp, Page* _ep, String path, String file_name, String name, FileSyntax& file_syntax, bool private_) {
    Region _r;
    List<Use>& uses = *new(alignof(List<Use>), _r.get_page()) List<Use>();
    if (file_syntax.uses != nullptr) {
        auto use_syntaxes = file_syntax.uses;
        auto _uses_iterator = use_syntaxes->get_iterator();
        while (auto _use_ = _uses_iterator.next()) {
            auto use = *_use_;
            auto _use_result = handle_use(_rp, _ep, use);
            if (_use_result._tag == Result<Use, ModelError>::Error)
                return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = _use_result._Error };
            auto use_ = _use_result._Ok;
            uses.add(use_);
        }
    }

    Array<Member>& members_builder = *new(alignof(Array<Member>), _r.get_page()) Array<Member>();
    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.get_page()) HashMapBuilder<String, Nameable>();
    MultiMapBuilder<String, Function>& functions_builder = *new(alignof(MultiMapBuilder<String, Function>), _r.get_page()) MultiMapBuilder<String, Function>();
    HashSetBuilder<String>& modules_checker = *new(alignof(HashSetBuilder<String>), _r.get_page()) HashSetBuilder<String>();
    List<Module>& modules = *new(alignof(List<Module>), _r.get_page()) List<Module>();

    auto declarations_iterator = file_syntax.declarations->get_iterator();
    while (auto declaration = declarations_iterator.next()) {
        switch (declaration->symbol._tag) {
            case SymbolSyntax::Private:{
                auto export_ = declaration->symbol._Private.export_;
                switch (export_._tag) {
                    case ExportSyntax::Definition: {
                        auto concept_result = handle_definition(_rp, _ep, path, export_._Definition, true, file_name);
                        if (concept_result._tag == Result<Function, ModelError>::Error)
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = concept_result._Error };
                        auto concept = concept_result._Ok;
                        if (functions_builder.contains(concept.name))
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file_name, Span(declaration->symbol._Definition.start, declaration->symbol._Definition.end)))) };

                        if (!symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept }))
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(DuplicateName(concept.name, Span(export_._Definition.start, export_._Definition.end)))) };
                        members_builder.add(Member { ._tag = Member::Concept, ._Concept = concept });
                    }
                    break;
                    case ExportSyntax::Function: {
                        auto function_syntax = export_._Function;
                        auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, true, true, file_name);
                        if (function_result._tag == Result<Function, ModelError>::Error)
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = function_result._Error };
                        auto function = function_result._Ok;
                        auto symbol_with_function_name = symbols_builder[function.name];
                        if (symbol_with_function_name != nullptr)
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(declaration->symbol._Function.start, declaration->symbol._Function.end)))) };
                        functions_builder.add(function.name, function);
                        members_builder.add(Member { ._tag = Member::Function, ._Function = function });
                    }
                    break;
                    case ExportSyntax::Operator: {
                        auto operator_syntax = export_._Operator;
                        auto operator_result = handle_operator(_rp, _ep, operator_syntax, true, file_name);
                        if (operator_result._tag == Result<Module, ModelError>::Error)
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                                ._Error = operator_result._Error };
                        auto operator_ = operator_result._Ok;
                        if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(DuplicateName(file_name, Span(operator_syntax.start, operator_syntax.end)))) };
                        members_builder.add(Member { ._tag = Member::Operator, ._Operator = operator_ });
                    }
                    break;
                    case ExportSyntax::Trait:
                        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file_name, String(_ep, "Private Trait"), Span(declaration->symbol._Trait.start, declaration->symbol._Trait.end)))) };
                    case ExportSyntax::Module: {
                        auto module_syntax = export_._Module;
                        auto module_result = handle_module(_rp, _ep, path, module_syntax, true);
                        if (module_result._tag == Result<Module, ModelError>::Error)
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                                ._Error = module_result._Error };
                        auto module = module_result._Ok;
                        if (!modules_checker.add(module.name))
                            return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(module_syntax.start, module_syntax.end)))) };
                        modules.add(module);
                    }
                    break;
                }
            }
            break;
            case SymbolSyntax::Definition: {
                auto concept_result = handle_definition(_rp, _ep, path, declaration->symbol._Definition, false, file_name);
                if (concept_result._tag == Result<Function, ModelError>::Error)
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = concept_result._Error };
                auto concept = concept_result._Ok;
                if (functions_builder.contains(concept.name))
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file_name, Span(declaration->symbol._Definition.start, declaration->symbol._Definition.end)))) };

                if (!symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept }))
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(DuplicateName(file_name, Span(declaration->symbol._Definition.start, declaration->symbol._Definition.end)))) };
                members_builder.add(Member { ._tag = Member::Concept, ._Concept = concept });
            }
            break;
            case SymbolSyntax::Function: {
                auto function_syntax = declaration->symbol._Function;
                auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file_name);
                if (function_result._tag == Result<Function, ModelError>::Error)
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = function_result._Error };
                auto function = function_result._Ok;
                auto symbol_with_function_name = symbols_builder[function.name];
                if (symbol_with_function_name != nullptr)
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(declaration->symbol._Function.start, declaration->symbol._Function.end)))) };
                functions_builder.add(function.name, function);
                members_builder.add(Member { ._tag = Member::Function, ._Function = function });
            }
            break;
            case SymbolSyntax::Operator: {
                auto operator_syntax = declaration->symbol._Operator;
                auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, file_name);
                if (operator_result._tag == Result<Module, ModelError>::Error)
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                        ._Error = operator_result._Error };
                auto operator_ = operator_result._Ok;
                if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(DuplicateName(file_name, Span(operator_syntax.start, operator_syntax.end)))) };
                members_builder.add(Member { ._tag = Member::Operator, ._Operator = operator_ });
            }
            break;
            case SymbolSyntax::Trait:
                return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file_name, String(_ep, "Trait"), Span(declaration->symbol._Trait.start, declaration->symbol._Trait.end)))) };
            case SymbolSyntax::Macro:
                return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file_name, String(_ep, "Macro"), Span(declaration->symbol._Macro.start, declaration->symbol._Macro.end)))) };
            case SymbolSyntax::Module: {
                auto module_syntax = declaration->symbol._Module;
                auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                if (module_result._tag == Result<Module, ModelError>::Error)
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                        ._Error = module_result._Error };
                auto module = module_result._Ok;
                    if (!modules_checker.add(module.name))
                    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file_name, Span(module_syntax.start, module_syntax.end)))) };
                modules.add(module);
           }
            break;
        }
    }

    auto multi_map = MultiMap<String, Function>(_r.get_page(), functions_builder);
    auto multi_map_iterator = multi_map.get_iterator();
    while (auto functions = multi_map_iterator.next()) {
        auto name = (*functions)[0]->name;
        symbols_builder.add(String(name), Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, *functions) });
    }

    Span span(file_syntax.start, file_syntax.end);
    auto ret = Module(private_, String(_rp, file_name), name, Vector<Module>(_rp, modules), Vector<Use>(_rp, uses), Vector<Member>(_rp, members_builder), HashMap<String, Nameable>(_rp, symbols_builder));
    
    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Ok,
        ._Ok = ret
    };
}

Result<Module, ModelError> build_referenced_module(Page* _rp, Page* _ep, String path, String name, bool private_) {
    Region _r;
    StringBuilder& file_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), path, name));
    file_name_builder.append(".scaly");
    auto file_name = file_name_builder.to_string(_r.get_page());
    auto module_text_result = File::read_to_string(_r.get_page(), _ep, file_name);
    if (module_text_result._tag == Result<String, FileError>::Error) {
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(module_text_result._Error) };
    }
    auto module_text = module_text_result._Ok;

    Parser& parser = *new(alignof(Parser), _r.get_page()) Parser(module_text);
    auto file_syntax_result = parser.parse_file(_rp, _ep);
    if (file_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ParserModelError(file_name, file_syntax_result._Error)) };
    auto file_syntax = file_syntax_result._Ok;
    if (!parser.is_at_end())
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ParserModelError(file_name, ParserError(InvalidSyntax(parser.lexer.previous_position, parser.lexer.position, String(_ep, "a valid declaraion")))))};

    return build_module(_rp, _ep, path, file_name, name, file_syntax, private_);
}

Result<Module, ModelError> handle_module(Page* _rp, Page* _ep, String path, ModuleSyntax& module_syntax, bool private_) {
    return build_referenced_module(_rp, _ep, path, module_syntax.name, private_);
}

Result<Module, ModelError> handle_package(Page* _rp, Page* _ep, String path, PackageSyntax package_syntax) {
    return build_referenced_module(_rp, _ep, path, package_syntax.name.name, false);
}

Result<Program, ModelError> build_program(Page* _rp, Page* _ep, String file_name, String program_name) {
    Region _r;
    auto file_text_result = File::read_to_string(_r.get_page(), _r.get_page(), file_name);
    if (file_text_result._tag == Result<String, FileError>::Error) {
        return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Error, ._Error = ModelError(file_text_result._Error) };
    }
    auto file_text = file_text_result._Ok;
    auto program_result = parse_program(_rp, _ep, file_text);
    if (program_result._tag == Result<Vector<DeclarationSyntax>*, ParserError>::Error)
        return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Error, ._Error = ModelError(ParserModelError(file_name, program_result._Error)) };
    auto program_syntax = program_result._Ok;

    auto path = Path::get_directory_name(_rp, file_name);
    List<Module>& packages = *new(alignof(List<Module>), _r.get_page()) List<Module>();
    if (program_syntax.file.packages != nullptr) {
        auto file_packages = program_syntax.file.packages;
        auto _package_iterator = file_packages->get_iterator();
        while (auto _package_syntax = _package_iterator.next()) {
            auto package_syntax = *_package_syntax;
            auto _package_result = build_referenced_module(_rp, _ep, path, _package_syntax->name.name, false);
            if (_package_result._tag == Result<Module, ModelError>::Error)
                return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Error, ._Error = _package_result._Error };
            auto package = _package_result._Ok;
            packages.add(package);
        }
    }

    auto module_result = build_module(_rp, _ep, Path::get_directory_name(_rp, file_name), file_name, program_name, program_syntax.file, false);
    if (module_result._tag == Result<Module, ModelError>::Error)
        return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Error, ._Error = module_result._Error };
    auto module = module_result._Ok;

    Vector<Statement> statements = Vector<Statement>();
    if (program_syntax.statements != nullptr) {
        auto _statements_result = handle_statements(_rp, _ep, *program_syntax.statements, file_name);
        if (_statements_result._tag == Result<Vector<Statement>, ModelError>::Error)
            return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Error, ._Error = _statements_result._Error };
        statements = _statements_result._Ok;
    }
    Program program(Vector<Module>(_rp, packages), module, statements);
    return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Ok, ._Ok = program };
}

}
}
}