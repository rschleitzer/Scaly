namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

Result<FileSyntax, ParserError> parse_file(Page* _rp, Page* _ep, const String& module) {
    Region _r;
    {
        Region _r_1;
        Parser& parser = *new(alignof(Parser), _r_1.get_page()) Parser(_r_1.get_page(), module);
        return parser.parse_file(_rp, _ep);
    }
}

Result<ProgramSyntax, ParserError> parse_program(Page* _rp, Page* _ep, const String& program) {
    Region _r;
    {
        Region _r_1;
        Parser& parser = *new(alignof(Parser), _r_1.get_page()) Parser(_r_1.get_page(), program);
        return parser.parse_program(_rp, _ep);
    }
}

Result<Type*, ModelError> handle_type(Page* _rp, Page* _ep, TypeSyntax& type, String file);
Result<Attribute, ModelError> handle_attribute(Page* _rp, Page* _ep, AttributeSyntax& attribute, String file);

Result<Property, ModelError> handle_property(Page* _rp, Page* _ep, bool private_, PropertySyntax& property, String file) {
    Region _r;
    Type* type = nullptr;
    if (property.annotation != nullptr) {
        auto _type_result = handle_type(_rp, _ep, property.annotation->type, file);
        if (_type_result._tag == Result<Type, ModelError>::Error)
            return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Error, ._Error = _type_result._Error };
        type = _type_result._Ok;
    }

    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (property.attributes != nullptr) {
        auto property_attributes = property.attributes;
        auto _attribute_iterator = VectorIterator<AttributeSyntax>(property_attributes);
        while (auto _attribute_syntax = _attribute_iterator.next()) {
            auto attribute_syntax = *_attribute_syntax;
            auto _attribute_result = handle_attribute(_rp, _ep, attribute_syntax, file);
            if (_attribute_result._tag == Result<Attribute, ModelError>::Error)
                return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Error, ._Error = _attribute_result._Error };
            auto attribute = _attribute_result._Ok;
            attributes.add(attribute);
        }
    }
    return Result<Property, ModelError> { ._tag = Result<Property, ModelError>::Ok, ._Ok = Property(Span(property.start, property.end), false, new(alignof(String), _rp) String(_rp, property.name), type, Vector<Attribute>(_rp, attributes)) };
}

Result<Vector<Property>, ModelError> handle_parameterset(Page* _rp, Page* _ep, ParameterSetSyntax& parameterSetSyntax, String file) {
    Region _r;
    List<Property>& parameters = *new(alignof(List<Property>), _r.get_page()) List<Property>();
    switch (parameterSetSyntax._tag) {
        case ParameterSetSyntax::Parameters: {
            auto parameters_syntax = parameterSetSyntax._Parameters;
            if (parameters_syntax.properties != nullptr) {
                auto _property_syntax_iterator = VectorIterator<PropertySyntax>(parameters_syntax.properties);
                while (auto _property_syntax = _property_syntax_iterator.next()) {
                    auto property_syntax = *_property_syntax;
                    auto _property_result = handle_property(_rp, _ep, false, property_syntax, file);
                    if (_property_result._tag == Result<Property, ModelError>::Error)
                        return Result<Vector<Property>, ModelError> { ._tag = Result<Vector<Property>, ModelError>::Error, ._Error = _property_result._Error };
                    auto property = _property_result._Ok;
                    parameters.add(property);
                }
            }
        }
        break;
        case ParameterSetSyntax::Type: {
            auto type_syntax = parameterSetSyntax._Type;
            auto _type_result = handle_type(_rp, _ep, type_syntax, file);
            if (_type_result._tag == Result<Type, ModelError>::Error)
                return Result<Vector<Property>, ModelError> { ._tag = Result<Vector<Property>, ModelError>::Error, ._Error = _type_result._Error };
            auto type = _type_result._Ok;
            parameters.add(Property(Span(type_syntax.start, type_syntax.end), false, nullptr, type, Vector<Attribute>()));
        }
        break;
    }
    return Result<Vector<Property>, ModelError> {
        ._tag = Result<Vector<Property>, ModelError>::Ok, 
        ._Ok = Vector<Property>(_rp, parameters) };
}

Result<HashMap<String, Property>, ModelError> handle_structure(Page* _rp, Page* _ep, StructureSyntax& structure, String file) {    
    Region _r;
    HashMapBuilder<String, Property>& properties_builder = *new(alignof(HashMapBuilder<String, Property>), _r.get_page()) HashMapBuilder<String, Property>();
    if (structure.parts != nullptr) {
        auto _members_iterator = VectorIterator<PartSyntax>(structure.parts);
        while (auto _member = _members_iterator.next()) {
            auto member = *_member;
            switch (member._tag) {
                case PartSyntax::Field: {
                    auto _property_result = handle_property(_rp, _ep, true, member._Property, file);
                    if (_property_result._tag == Result<Property, ModelError>::Error)
                        return Result<HashMap<String, Property>, ModelError> { ._tag = Result<HashMap<String, Property>, ModelError>::Error, ._Error = _property_result._Error };
                    auto property = _property_result._Ok;
                    properties_builder.add(String(_rp, *property.name), property);
                }
                break;
                case PartSyntax::Property: {
                    auto _property_result = handle_property(_rp, _ep, false, member._Property, file);
                    if (_property_result._tag == Result<Property, ModelError>::Error)
                        return Result<HashMap<String, Property>, ModelError> { ._tag = Result<HashMap<String, Property>, ModelError>::Error, ._Error = _property_result._Error };
                    auto property = _property_result._Ok;
                    properties_builder.add(String(_rp, *property.name), property);
                }
                break;
            }
        }
    }
    return Result<HashMap<String, Property>, ModelError> { ._tag = Result<HashMap<String, Property>, ModelError>::Ok, ._Ok = HashMap<String, Property>(_rp, properties_builder) };
}

Result<HashMap<String, Variant>, ModelError> handle_variants(Page* _rp, Page* _ep, Vector<VariantSyntax>& variants) {    
    Region _r;
    HashMapBuilder<String, Variant>& variants_builder = *new(alignof(HashMapBuilder<String, Variant>), _r.get_page()) HashMapBuilder<String, Variant>();
    /// Fill it
    return Result<HashMap<String, Variant>, ModelError> { ._tag = Result<HashMap<String, Variant>, ModelError>::Ok, ._Ok = HashMap<String, Variant>(_rp, variants_builder) };
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
    auto properties_result = handle_structure(_rp, _ep, class_syntax.structure, file);
    if (properties_result._tag == Result<HashMap<String, Property>, ModelError>::Error)
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = properties_result._Error };
    auto properties = properties_result._Ok;

    List<Use>& uses = *new(alignof(List<Use>), _r.get_page()) List<Use>();

    if (class_syntax.body == nullptr) {
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_syntax.start, class_syntax.end), private_, properties, Vector<Use>(_rp, uses), Vector<Initializer>(), nullptr, HashMap<String, Nameable>())
        };
    }
    else {
        if (class_syntax.body->uses != nullptr) {
            auto use_syntaxes = class_syntax.body->uses;
            auto _uses_iterator = VectorIterator<UseSyntax>(use_syntaxes);
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
        auto initializers_iterator = VectorIterator<InitSyntax>(class_syntax.body->inits);
        while (auto initializer = initializers_iterator.next()) {
            auto _initializer_result = handle_initializer(_rp, _ep, *initializer, false, file);
            if (_initializer_result._tag == Result<Initializer, ModelError>::Error)
                return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _initializer_result._Error };
            initializers_builder.add(_initializer_result._Ok);
        }
        if (class_syntax.body->deInit != nullptr) {
            auto _deinitializer_result = handle_deinitializer(_rp, _ep, *class_syntax.body->deInit, file);
            if (_deinitializer_result._tag == Result<DeInitializer, ModelError>::Error)
                return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = _deinitializer_result._Error };
            deInitializer = _deinitializer_result._Ok;
        }
        HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.get_page()) HashMapBuilder<String, Nameable>();
        HashMapBuilder<String, List<Function>>& functions_builder = *new(alignof(HashMapBuilder<String, List<Function>>), _r.get_page()) HashMapBuilder<String, List<Function>>();
        auto members_iterator = VectorIterator<MemberSyntax>(class_syntax.body->members);
        while (auto member = members_iterator.next()) {
            switch (member->_tag) {
                case MemberSyntax::Definition: {
                    auto concept_result = handle_definition(_rp, _ep, path, member->_Definition, false, file);
                    if (concept_result._tag == Result<Function, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = concept_result._Error };
                    auto concept = concept_result._Ok;
                    if (functions_builder.contains(concept.name))
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(member->_Definition.start, member->_Definition.end)))) };

                    symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
                }
                break;
                case MemberSyntax::Function: {
                    auto function_syntax = member->_Function;
                    auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file);
                    if (function_result._tag == Result<Function, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = function_result._Error };
                    auto function = function_result._Ok;
                    auto symbol_with_function_name = symbols_builder[function.name];
                    if (symbol_with_function_name != nullptr)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(member->_Function.start, member->_Function.end)))) };
                    List<Function>* functions_list = functions_builder[function.name];
                    if (functions_list == nullptr)
                    {
                        functions_builder.add(function.name, List<Function>());
                        functions_list = functions_builder[function.name];
                    }
                    (*functions_list).add(function);
                }
                break;
                case MemberSyntax::Procedure: {
                    auto procedure_syntax = member->_Procedure;
                    auto procedure_result = build_function(_rp, _ep, procedure_syntax.start, procedure_syntax.end, procedure_syntax.target, false, false, file);
                    if (procedure_result._tag == Result<Function, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = procedure_result._Error };
                    auto function = procedure_result._Ok;
                    auto symbol_with_function_name = symbols_builder[function.name];
                    if (symbol_with_function_name != nullptr)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(member->_Function.start, member->_Function.end)))) };
                    List<Function>* functions_list = functions_builder[function.name];
                    if (functions_list == nullptr)
                    {
                        functions_builder.add(function.name, List<Function>());
                        functions_list = functions_builder[function.name];
                    }
                    (*functions_list).add(function);
                }
                break;
                case MemberSyntax::Operator: {
                    auto operator_syntax = member->_Operator;
                    auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, file);
                    if (operator_result._tag == Result<Structure, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                            ._Error = operator_result._Error };
                    auto operator_ = operator_result._Ok;
                    if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
                }
                break;
                case MemberSyntax::Implement:
                    return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Implement"), Span(member->_Implement.start, member->_Implement.end)))) };
                case MemberSyntax::Trait:
                    return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Trait"), Span(member->_Trait.start, member->_Trait.end)))) };
                case MemberSyntax::Macro:
                    return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Macro"), Span(member->_Macro.start, member->_Macro.end)))) };
                case MemberSyntax::Module: {
                    auto module_syntax = member->_Module;
                    auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                    if (module_result._tag == Result<Structure, ModelError>::Error)
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                            ._Error = module_result._Error };
                    auto module = module_result._Ok;
                    if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                }
                break;
            }
        }

        HashMap<String, List<Function>>& functions = *new(alignof(HashMap<String, List<Function>>), _r.get_page()) HashMap<String, List<Function>>(_r.get_page(), functions_builder);
        if (functions.slots != nullptr) {
            auto functions_slots_iterator = VectorIterator<Vector<KeyValuePair<String, List<Function>>>>(functions.slots);
            while (auto function_slot = functions_slots_iterator.next()) {
                auto functions_iterator = VectorIterator<KeyValuePair<String, List<Function>>>(function_slot);
                while (auto function_kvp = functions_iterator.next()) {
                    symbols_builder.add(function_kvp->key, Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, function_kvp->value) });
                }
            }
        }

        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_syntax.start, class_syntax.end), private_, properties, Vector<Use>(_rp, uses), Vector<Initializer>(_rp, initializers_builder), deInitializer, HashMap<String, Nameable>(_rp, symbols_builder))
        };
    }
}

Result<Namespace, ModelError> handle_namespace(Page* _rp, Page* _ep, String name, String path, NamespaceSyntax& namespace_syntax, bool private_, String file) {    
    Region _r;
    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.get_page()) HashMapBuilder<String, Nameable>();
    HashMapBuilder<String, List<Function>>& functions_builder = *new(alignof(HashMapBuilder<String, List<Function>>), _r.get_page()) HashMapBuilder<String, List<Function>>();

    auto declarations_iterator = VectorIterator<DeclarationSyntax>(namespace_syntax.declarations);
    while (auto declaration = declarations_iterator.next()) {
        switch (declaration->_tag) {
            case DeclarationSyntax::Private:{
                auto export_ = declaration->_Private.export_;
                switch (export_._tag) {
                    case ExportSyntax::Definition: {
                        auto concept_result = handle_definition(_rp, _ep, path, export_._Definition, true, file);
                        if (concept_result._tag == Result<Function, ModelError>::Error)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = concept_result._Error };
                        auto concept = concept_result._Ok;
                        if (functions_builder.contains(concept.name))
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                        symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
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
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(declaration->_Function.start, declaration->_Function.end)))) };
                        List<Function>* functions_list = functions_builder[function.name];
                        if (functions_list == nullptr)
                        {
                            functions_builder.add(function.name, List<Function>());
                            functions_list = functions_builder[function.name];
                        }
                        (*functions_list).add(function);
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
                    }
                    break;
                    case ExportSyntax::Trait:
                        return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Private Trait"), Span(declaration->_Trait.start, declaration->_Trait.end)))) };
                    case ExportSyntax::Module: {
                        auto module_syntax = export_._Module;
                        auto module_result = handle_module(_rp, _ep, path, module_syntax, true);
                        if (module_result._tag == Result<Namespace, ModelError>::Error)
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = module_result._Error };
                        auto module = module_result._Ok;
                        if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                            return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                                ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                    }
                    break;
                }
            }
            break;
            case DeclarationSyntax::Definition: {
                auto concept_result = handle_definition(_rp, _ep, path, declaration->_Definition, false, file);
                if (concept_result._tag == Result<Function, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = concept_result._Error };
                auto concept = concept_result._Ok;
                if (functions_builder.contains(concept.name))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
            }
            break;
            case DeclarationSyntax::Function: {
                auto function_syntax = declaration->_Function;
                auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file);
                if (function_result._tag == Result<Function, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = function_result._Error };
                auto function = function_result._Ok;
                auto symbol_with_function_name = symbols_builder[function.name];
                if (symbol_with_function_name != nullptr)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(declaration->_Function.start, declaration->_Function.end)))) };
                List<Function>* functions_list = functions_builder[function.name];
                if (functions_list == nullptr)
                {
                    functions_builder.add(function.name, List<Function>());
                    functions_list = functions_builder[function.name];
                }
                (*functions_list).add(function);
            }
            break;
            case DeclarationSyntax::Operator: {
                auto operator_syntax = declaration->_Operator;
                auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, file);
                if (operator_result._tag == Result<Namespace, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = operator_result._Error };
                auto operator_ = operator_result._Ok;
                if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
            }
            break;
            case DeclarationSyntax::Trait:
                return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Trait"), Span(declaration->_Trait.start, declaration->_Trait.end)))) };
            case DeclarationSyntax::Macro:
                return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Macro"), Span(declaration->_Macro.start, declaration->_Macro.end)))) };
            case DeclarationSyntax::Module: {
                auto module_syntax = declaration->_Module;
                auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                if (module_result._tag == Result<Namespace, ModelError>::Error)
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = module_result._Error };
                auto module = module_result._Ok;
                if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Error,
                        ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
            }
            break;
        }
    }

    HashMap<String, List<Function>>& functions = *new(alignof(HashMap<String, List<Function>>), _r.get_page()) HashMap<String, List<Function>>(_r.get_page(), functions_builder);
    if (functions.slots != nullptr) {
        auto functions_slots_iterator = VectorIterator<Vector<KeyValuePair<String, List<Function>>>>(functions.slots);
        while (auto function_slot = functions_slots_iterator.next()) {
            auto functions_iterator = VectorIterator<KeyValuePair<String, List<Function>>>(function_slot);
            while (auto function_kvp = functions_iterator.next()) {
                symbols_builder.add(function_kvp->key, Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, function_kvp->value) });
            }
        }
    }
    Span span(namespace_syntax.start, namespace_syntax.end);
    auto ret = Namespace(span,  HashMap<String, Nameable>(_rp, symbols_builder));
    return Result<Namespace, ModelError> { ._tag = Result<Namespace, ModelError>::Ok, ._Ok = ret };
}

Result<Union, ModelError> handle_union(Page* _rp, Page* _ep, String name, String path, UnionSyntax& union_, bool private_, String file) {    
    Region _r_1;
    auto variants_result = handle_variants(_rp, _ep, *union_.variants);
    if (variants_result._tag == Result<HashMap<String, Property>, ModelError>::Error)
        return Result<Union, ModelError> { ._tag = Result<Union, ModelError>::Error, ._Error = variants_result._Error };
    auto variants = variants_result._Ok;
    return Result<Union, ModelError> { ._tag = Result<Union, ModelError>::Ok,
        ._Ok = Union(Span(union_.start, union_.end), private_, variants)
    };
}

Result<Operation, ModelError> handle_operation(Page* _rp, Page* _ep, OperationSyntax& operation, String file);

Result<Catch, ModelError> handle_catch(Page* _rp, Page* _ep, CatchSyntax& catch_, String file) {
    Region _r;
    auto _condition_result =  handle_operation(_rp, _ep, catch_.condition, file);
    if (_condition_result._tag == Result<Operand, ModelError>::Error)
        return Result<Catch, ModelError> { ._tag = Result<Catch, ModelError>::Error, ._Error = _condition_result._Error };
    auto condition = _condition_result._Ok;
    auto _handler_result =  handle_operation(_rp, _ep, catch_.handler, file);
    if (_handler_result._tag == Result<Operand, ModelError>::Error)
        return Result<Catch, ModelError> { ._tag = Result<Catch, ModelError>::Error, ._Error = _handler_result._Error };
    auto handler = _handler_result._Ok;
    return Result<Catch, ModelError> { ._tag = Result<Catch, ModelError>::Ok, ._Ok = Catch(Span(catch_.start, catch_.end), condition, handler) };
}

Result<Drop, ModelError> handle_drop(Page* _rp, Page* _ep, DropSyntax& drop_, String file) {
    Region _r;
    auto _handler_result =  handle_operation(_rp, _ep, drop_.handler, file);
    if (_handler_result._tag == Result<Operand, ModelError>::Error)
        return Result<Drop, ModelError> { ._tag = Result<Drop, ModelError>::Error, ._Error = _handler_result._Error };
    auto handler = _handler_result._Ok;
    return Result<Drop, ModelError> { ._tag = Result<Drop, ModelError>::Ok, ._Ok = Drop(Span(drop_.start, drop_.end), handler) };
}

Result<Catcher, ModelError> handle_catcher(Page* _rp, Page* _ep, CatcherSyntax& catcher, String file) {
    Region _r;
    List<Catch>& catches_builder = *new(alignof(List<Catch>), _r.get_page()) List<Catch>();
    if (catcher.catches != nullptr) {
        auto catch_syntaxes = catcher.catches;
        auto _catches_iterator = VectorIterator<CatchSyntax>(catch_syntaxes);
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
        auto drop_ = _drop_result._Ok;

    }
    return Result<Catcher, ModelError> { ._tag = Result<Catcher, ModelError>::Ok, ._Ok = Catcher(Span(catcher.start, catcher.end), Vector<Catch>(_rp, catches_builder), drop_) };
}

Result<Postfix, ModelError> handle_postfix(Page* _rp, Page* _ep, PostfixSyntax& postfix, String file) {
    Region _r;
    switch (postfix._tag) {
        case Postfix::MemberAccess: {
            auto member_access = postfix._MemberAccess;
            List<String>& path = *new(alignof(List<String>), _r.get_page()) List<String>();
            path.add(String(_rp, member_access.member.name));
            if (member_access.member.extensions != nullptr) {
                auto extensions = member_access.member.extensions;
                auto _extensions_iterator = VectorIterator<ExtensionSyntax>(extensions);
                while (auto _extension = _extensions_iterator.next()) {
                    auto extension = *_extension;
                    path.add(extension.name);
                }
            }
            return Result<Postfix, ModelError> { ._tag = Result<Postfix, ModelError>::Ok, ._Ok = Postfix { ._tag = Postfix::MemberAccess, ._MemberAccess = Vector<String>(_rp, path) } };
        }
        case Postfix::Catcher: {
            auto catcher_syntax = postfix._Catcher;
            auto _catcher_result = handle_catcher(_rp, _ep, catcher_syntax, file);
            if (_catcher_result._tag == Result<Catcher, ModelError>::Error)
                return Result<Postfix, ModelError> { ._tag = Result<Postfix, ModelError>::Error, ._Error = _catcher_result._Error };
            auto catcher = _catcher_result._Ok;
            return Result<Postfix, ModelError> { ._tag = Result<Postfix, ModelError>::Ok, ._Ok = Postfix { ._tag = Postfix::Catcher, ._Catcher = catcher } };
        }
    }
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
            auto hex = (size_t)strtol(literal.literal._Hex.value.to_c_string(_r.get_page()), nullptr, 16);
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
        auto _extensions_iterator = VectorIterator<ExtensionSyntax>(extensions);
        while (auto _extension = _extensions_iterator.next()) {
            auto extension = *_extension;
            path.add(extension.name);
        }
    }

    Vector<Type> generics = Vector<Type>(_rp, 0);
    Lifetime lifetime = Lifetime(Unspecified());
    List<Type>& generics_builder = *new(alignof(List<Type>), _r.get_page()) List<Type>();
    if (type.generics != nullptr) {
        auto generic_arguments = type.generics->generics;
        if (generic_arguments != nullptr) {
            auto generics = generic_arguments;
            auto _generics_iterator = VectorIterator<GenericArgumentSyntax>(generics);
            while (auto _generic = _generics_iterator.next()) {
                auto generic = *_generic;
                auto _type_result = handle_type(_rp, _ep, generic.type, file);
                if (_type_result._tag == Result<Type*, ModelError>::Error)
                    return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Error, ._Error = _type_result._Error };
                auto type = _type_result._Ok;
                generics_builder.add(*type);
            }
        }
    }

    if (type.lifetime != nullptr) {
        auto lifetime_syntax = *type.lifetime;
        switch (lifetime_syntax._tag) {
            case LifetimeSyntax::Root:
                lifetime = Lifetime(Root());
            break;
            case LifetimeSyntax::Local:
                lifetime = Lifetime(Local(String(_rp, lifetime_syntax._Local.location)));
            break;
            case LifetimeSyntax::Reference:
                lifetime = Lifetime(Reference(String(_rp, lifetime_syntax._Reference.age)));
            break;
            case LifetimeSyntax::Thrown:
                lifetime = Lifetime(Thrown());
            break;
        }
    }

    return Result<Type*, ModelError> { ._tag = Result<Type*, ModelError>::Ok, ._Ok = new(alignof(Type), _rp) Type(Span(type.start, type.end), Vector<String>(_rp, path), Vector<Type>(_rp, generics_builder), lifetime) };
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

Result<Vector<Statement>, ModelError> handle_statements(Page* _rp, Page* _ep, Vector<StatementSyntax>& statements, String file) {
    Region _r;
    List<Statement>& statements_builder = *new(alignof(List<Statement>), _r.get_page()) List<Statement>();
    auto _statements_iterator = VectorIterator<StatementSyntax>(&statements);
    while (auto statement = _statements_iterator.next()) {
        switch (statement->_tag)
        {
            case StatementSyntax::Operation: {
                auto operation_result = handle_operation(_rp, _ep, statement->_Operation, file);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                statements_builder.add(Statement { ._tag = Statement::Action, ._Action = { ._tag = Action::Operation, ._Operation = operation_result._Ok }});
                break;
            }
            case StatementSyntax::Let: {
                auto binding = statement->_Let.binding.operation;
                Type* type = nullptr;
                if (statement->_Let.binding.annotation != nullptr) {
                    auto _type_result = handle_binding_annotation(_rp, _ep, *statement->_Let.binding.annotation, file);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _type_result._Error };
                    type = _type_result._Ok;
                }
                auto operation_result = handle_operation(_rp, _ep, binding, file);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                statements_builder.add(Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Constant, Property(Span(binding.start, binding.end), false, new(alignof(String), _rp) String(_rp, statement->_Let.binding.name), type, Vector<Attribute>(_rp, 0)), operation_result._Ok) });
                return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
            }
            case StatementSyntax::Var: {
                auto binding = statement->_Var.binding.operation;
                Type* type = nullptr;
                if (statement->_Var.binding.annotation != nullptr) {
                    auto _type_result = handle_binding_annotation(_rp, _ep, *statement->_Var.binding.annotation, file);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _type_result._Error };
                    type = _type_result._Ok;
                }
                auto operation_result = handle_operation(_rp, _ep, binding, file);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                statements_builder.add(Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Extendable, Property(Span(binding.start, binding.end), false, new(alignof(String), _rp) String(_rp, statement->_Mutable.binding.name), type, Vector<Attribute>(_rp, 0)), operation_result._Ok) });
                return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
            }
            case StatementSyntax::Mutable: {
                auto binding = statement->_Mutable.binding.operation;
                Type* type = nullptr;
                if (statement->_Mutable.binding.annotation != nullptr) {
                    auto _type_result = handle_binding_annotation(_rp, _ep, *statement->_Mutable.binding.annotation, file);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _type_result._Error };
                    type = _type_result._Ok;
                }
                auto operation_result = handle_operation(_rp, _ep, binding, file);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                statements_builder.add(Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Mutable, Property(Span(binding.start, binding.end), false, new(alignof(String), _rp) String(_rp, statement->_Mutable.binding.name), type, Vector<Attribute>(_rp, 0)), operation_result._Ok) });
                return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
            }
            case StatementSyntax::Set:
                auto _target_result = handle_operation(_rp, _ep, statement->_Set.target, file);
                if (_target_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _target_result._Error };
                auto target = _target_result._Ok;
                auto _source_result = handle_operation(_rp, _ep, statement->_Set.source, file);
                if (_source_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _source_result._Error };
                auto source = _source_result._Ok;
                statements_builder.add(Statement { ._tag = Statement::Action, ._Action = { ._tag = Action::Mutation, ._Mutation = Mutation(source, target) }});
                break;
        }
    }
    
    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError> ::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
}

Result<Vector<Operand>, ModelError> handle_operands(Page* _rp, Page* _ep, Vector<OperandSyntax>& operands, String file);

Result<Component, ModelError> handle_component(Page* _rp, Page* _ep, ComponentSyntax& component, String file) {
    Region _r;
    String* name = nullptr;

    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (component.attributes != nullptr) {
        auto definition_attributes = component.attributes;
        auto _attribute_iterator = VectorIterator<AttributeSyntax>(definition_attributes);
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
                if (name_operand.postfixes != nullptr)
                    return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidComponentName(file, Span(name_operand.start, name_operand.end)))) };
                if (name_operand.expression._tag == ExpressionSyntax::Variable)
                    name = new(alignof(String), _rp) String(_rp, name_operand.expression._Variable.identifier);
            }
        }
        auto value = component.value->value;
        if (value.operands != nullptr) {
            auto operands = *value.operands;
            auto value_result = handle_operands(_rp, _ep, operands, file);
            if (value_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = value_result._Error };
            else
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, value_result._Ok, Vector<Attribute>(_rp,attributes))};
        } else {
            return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, Vector<Operand>(_rp, 0), Vector<Attribute>(_rp,attributes))};
        }
    } else {
        if (component.operands != nullptr) {
            auto operands = *component.operands;
            auto value_result = handle_operands(_rp, _ep, operands, file);
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
        auto components_iterator = VectorIterator<ComponentSyntax>(object.components);
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
    List<Operation>& operations_builder = *new(alignof(List<Operation>), _r.get_page()) List<Operation>();
    if (vector.elements != nullptr) {
        auto elements_iterator = VectorIterator<ElementSyntax>(vector.elements);
        while (auto element = elements_iterator.next()) {
            auto operation_result =  handle_operation(_rp, _ep, element->operation, file);
            if (operation_result._tag == Result<Operand, ModelError>::Error)
                return Result<Matrix, ModelError> { ._tag = Result<Matrix, ModelError>::Error, ._Error = operation_result._Error };
            operations_builder.add(operation_result._Ok);
        }
    }
    return Result<Matrix, ModelError> { ._tag = Result<Matrix, ModelError>::Ok, ._Ok = Matrix(Span(vector.start, vector.end), Vector<Operation>(_rp, operations_builder)) };
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

Result<Action, ModelError> handle_action(Page* _rp, Page* _ep, ActionSyntax& action, String file);

Result<If, ModelError> handle_if(Page* _rp, Page* _ep, IfSyntax& if_, String file) {
    Region _r;
    Property* property = nullptr;
    auto condition_result = handle_operands(_rp, _ep, *if_.condition.operands, file);
    if (condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = condition_result._Error };
    auto condition = condition_result._Ok;
    Action consequent = Action { ._tag = Action::Operation, ._Operation = Operation(Vector<Operand>(_rp, 0)) };
    if (if_.consequent != nullptr) {
        auto _consequent_result = handle_action(_rp, _ep, *if_.consequent, file);
        if (_consequent_result._tag == Result<Action, ModelError>::Error)
            return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = _consequent_result._Error };
        consequent = Action(_consequent_result._Ok);
    }

    Action* alternative = nullptr;
    if (if_.alternative != nullptr) {
        auto _alternative_result = handle_action(_rp, _ep, if_.alternative->alternative, file);
        if (_alternative_result._tag == Result<Action, ModelError>::Error)
            return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = _alternative_result._Error };
        alternative = new(alignof(Action), _rp) Action(_alternative_result._Ok);
    }
    return Result<If, ModelError> { ._tag = Result<If, ModelError>::Ok, ._Ok = If(Span(if_.start, if_.end), condition, property, consequent, alternative) };
}

Result<Match, ModelError> handle_match(Page* _rp, Page* _ep, MatchSyntax& match_, String file) {
    Region _r;
    Property* property = nullptr;

    auto condition_result = handle_operands(_rp, _ep, *match_.scrutinee.operands, file);
    if (condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = condition_result._Error };
    auto condition = condition_result._Ok;

    List<Case> cases_builder;
    if (match_.cases != nullptr) {
        auto case_iterator = VectorIterator<CaseSyntax>(match_.cases);
        while (auto case_ = case_iterator.next()) {
            auto condition_result =  handle_operation(_rp, _ep, case_->condition, file);
            if (condition_result._tag == Result<Operand, ModelError>::Error)
                return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = condition_result._Error };
            auto _consequent_result = handle_action(_rp, _ep, case_->consequent, file);
            if (_consequent_result._tag == Result<Action, ModelError>::Error)
                return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = _consequent_result._Error };
            auto consequent = Action(_consequent_result._Ok);
        }
    }

    Action alternative = Action { ._tag = Action::Operation, ._Operation = Operation(Vector<Operand>(_rp, 0)) };
    if (match_.alternative != nullptr) {
        auto _alternative_result = handle_action(_rp, _ep, *(*(match_.alternative)).alternative, file);
        if (_alternative_result._tag == Result<Action, ModelError>::Error)
            return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Error, ._Error = _alternative_result._Error };
        alternative = Action(_alternative_result._Ok);
    }
    return Result<Match, ModelError> { ._tag = Result<Match, ModelError>::Ok, ._Ok = Match(Span(match_.start, match_.end), condition, Vector<Case>(_rp, cases_builder), alternative) };
}

Result<For, ModelError> handle_for(Page* _rp, Page* _ep, ForSyntax& for_, String file) {
    Region _r;
    auto expression_result = handle_operands(_rp, _ep, *for_.operation.operands, file);
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

Result<While, ModelError> handle_while(Page* _rp, Page* _ep, WhileSyntax& while_, String file) {
    Region _r;
    auto _condition_result = handle_operands(_rp, _ep, *while_.condition.operands, file);
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

Result<SizeOf, ModelError> handle_size_of(Page* _rp, Page* _ep, SizeOfSyntax& size_of, String file) {
    Region _r;
    auto type_result = handle_type(_rp, _ep, size_of.type, file);
    if (type_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<SizeOf, ModelError> { ._tag = Result<SizeOf, ModelError>::Error, ._Error = type_result._Error };
    auto type = type_result._Ok;
    return Result<SizeOf, ModelError> { ._tag = Result<SizeOf, ModelError>::Ok, ._Ok = SizeOf(Span(size_of.start, size_of.end), *type) };
}

Result<Return, ModelError> handle_return(Page* _rp, Page* _ep, ReturnSyntax& return_, String file) {
    Region _r;
    if (return_.result != nullptr) {
        auto result = return_.result->operands;
        auto result_result = handle_operands(_rp, _ep, *result, file);
        if (result_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Error, ._Error = result_result._Error };
        return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(return_.start, return_.end), result_result._Ok) };
    }
    return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(return_.start, return_.end), Vector<Operand>(_rp, 0)) };
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
        case ExpressionSyntax::Variable: {
            auto variable = expression._Variable;
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Variable, ._Variable = String(_rp, variable.identifier)} };
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
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::If, ._Match = match_result._Ok} };
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
        case ExpressionSyntax::Repeat:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Repeat"), Span(expression._Repeat.start, expression._Repeat.end)))) };
        case ExpressionSyntax::SizeOf: {
            auto size_of = expression._SizeOf;
            auto size_of_result = handle_size_of(_rp, _ep, size_of, file);
            if (size_of_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = size_of_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::SizeOf, ._SizeOf = size_of_result._Ok} };
        }
        case ExpressionSyntax::Continue:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Continue"), Span(expression._Continue.start, expression._Continue.end)))) };
        case ExpressionSyntax::Break:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Break"), Span(expression._Break.start, expression._Break.end)))) };
        case ExpressionSyntax::Return: {
            auto return_ = expression._Return;
            auto return_result = handle_return(_rp, _ep, return_, file);
            if (return_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = return_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Return, ._Return = return_result._Ok} };
        }
        case ExpressionSyntax::Throw:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Throw"), Span(expression._Throw.start, expression._Throw.end)))) };
    }
}

Result<Operand, ModelError> handle_operand(Page* _rp, Page* _ep, OperandSyntax& operand, String file) {
    Region _r;
    Vector<Postfix>* postfixes = nullptr;
    if (operand.postfixes != nullptr) {
        Region _r_1;
        List<Postfix>& postfixes_builder = *new(alignof(List<Postfix>), _r.get_page()) List<Postfix>();
        auto postfixes_iterator = VectorIterator<PostfixSyntax>(operand.postfixes);
        while (auto postfix = postfixes_iterator.next()) {
            auto postfix_result = handle_postfix(_rp, _ep, *postfix, file);
            if (postfix_result._tag == Result<Operand, ModelError>::Error)
                return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Error, ._Error = postfix_result._Error };
            postfixes_builder.add(postfix_result._Ok);
        }
        postfixes = new(alignof(Vector<Postfix>), _rp) Vector<Postfix>(_rp, postfixes_builder);
    }

    auto expression_result = handle_expression(_rp, _ep, operand.expression, file);
    if (expression_result._tag == Result<Expression, ModelError>::Error)
        return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Error, ._Error = expression_result._Error };

    return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Ok, ._Ok = Operand(Span(operand.expression._Literal.start, operand.expression._Literal.end), expression_result._Ok, postfixes) };
}

Result<Vector<Operand>, ModelError> handle_operands(Page* _rp, Page* _ep, Vector<OperandSyntax>& operands, String file) {
    Region _r;
    List<Operand>& operands_builder = *new(alignof(List<Operand>), _r.get_page()) List<Operand>();
    auto operands_iterator = VectorIterator<OperandSyntax>(&operands);
    while (auto operand = operands_iterator.next()) {
        auto operand_result = handle_operand(_rp, _ep, *operand, file);
        if (operand_result._tag == Result<Operand, ModelError>::Error)
            return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Error, ._Error = operand_result._Error };
        operands_builder.add(operand_result._Ok);
    }
    return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Ok, ._Ok = (Vector<Operand>(_rp, operands_builder)) };
}

Result<Operation, ModelError> handle_operation(Page* _rp, Page* _ep, OperationSyntax& operation, String file) {
    if (operation.operands != nullptr) {
        auto operands = *operation.operands;
        auto operands_result = handle_operands(_rp, _ep, operands, file);
        if (operands_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Error, ._Error = operands_result._Error };
        return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Ok, ._Ok = operands_result._Ok };
    }
    return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Ok, ._Ok = Operation(Vector<Operand>(_rp, 0)) };
}

Result<GenericParameter, ModelError> handle_generic_parameter(Page* _rp, Page* _ep, GenericParameterSyntax& generic_parameter, String file) {
    Region _r;
    List<Attribute>& attributes = *new(alignof(List<Attribute>), _r.get_page()) List<Attribute>();
    if (generic_parameter.attributes != nullptr) {
        auto definition_attributes = generic_parameter.attributes;
        auto _attribute_iterator = VectorIterator<AttributeSyntax>(definition_attributes);
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
        auto _extensions_iterator = VectorIterator<ExtensionSyntax>(extensions);
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
            auto _parameters_iterator = VectorIterator<GenericParameterSyntax>(generic_parameters.parameters);
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
        auto _attribute_iterator = VectorIterator<AttributeSyntax>(definition_attributes);
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
            auto operation_result = handle_operation(_rp, _ep, constant.operation, file);
            if (operation_result._tag == Result<Operation, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = operation_result._Error };
            auto operation = operation_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name), Vector<GenericParameter>(_rp, parameters), Vector<Attribute>(_rp, attributes),
                    Definition { ._tag = Definition::Constant, ._Constant = operation }
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
            if (operation_result._tag == Result<Operation, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Ok, ._Ok = Action { ._tag = Action::Operation, ._Operation = operation_result._Ok } };
        }
        case ActionSyntax::Set: {
            auto _target_result = handle_operation(_rp, _ep, action._Set.target, file);
            if (_target_result._tag == Result<Operation, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = _target_result._Error };
            auto target = _target_result._Ok;
            auto _source_result = handle_operation(_rp, _ep, action._Set.source, file);
            if (_source_result._tag == Result<Operation, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = _source_result._Error };
            auto source = _source_result._Ok;
            return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Ok, ._Ok = Action { ._tag = Action::Mutation, ._Mutation = Mutation(source, target) } };
        }
    }
}

Result<Function, ModelError> build_function(Page* _rp, Page* _ep, size_t start, size_t end, TargetSyntax targetSyntax, bool private_, bool pure, String file) {
    Vector<Property> input = Vector<Property>(_rp, 0);
    Vector<Property> output = Vector<Property>(_rp, 0);
    switch (targetSyntax._tag) {
        case TargetSyntax::Routine:
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Non-Symbol Function"), Span(start, end)))) };
        case TargetSyntax::Symbol:
            auto symbol = targetSyntax._Symbol;

            if (symbol.routine.parameters != nullptr) {
                ParameterSetSyntax& parameterSetSyntax = *symbol.routine.parameters;
                auto _input_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                if (_input_result._tag == Result<Vector<Property>, ModelError>::Error)
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _input_result._Error };
                input = _input_result._Ok;
            }

            if (symbol.routine.returns != nullptr)
            {
                ParameterSetSyntax& parameterSetSyntax = symbol.routine.returns->parameters; 
                auto _output_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                if (_output_result._tag == Result<Vector<Property>, ModelError>::Error)
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _output_result._Error };
                output = _output_result._Ok;
            }

            switch (symbol.routine.implementation._tag) {
                case ImplementationSyntax::Action: {
                    auto action_result = handle_action(_rp, _ep, symbol.routine.implementation._Action, file);
                    if (action_result._tag == Result<Statement, ModelError>::Error)
                        return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = action_result._Error };
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
                }
                case ImplementationSyntax::Extern:
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Extern, ._Extern = Extern() }) };
                case ImplementationSyntax::Instruction:
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Instruction, ._Instruction = Instruction() }) };
                case ImplementationSyntax::Intrinsic:
                    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(start, end), private_, pure, String(_rp, symbol.end), input, output, Implementation { ._tag = Implementation::Intrinsic, ._Intrinsic = Intrinsic() }) };
                break;

            }
    }

}

Result<Initializer, ModelError> handle_initializer(Page* _rp, Page* _ep, InitSyntax& initializer, bool private_, String file) {
    Vector<Property> input = Vector<Property>(_rp, 0);

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
    Vector<Property> input = Vector<Property>(_rp, 0);
    Vector<Property> output = Vector<Property>(_rp, 0);
    auto start = operator_syntax.start;
    auto end = operator_syntax.end;
    Operation* operation = nullptr;
    switch (operator_syntax.target._tag) {
        case TargetSyntax::Routine:
            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Non-Symbol Operator"), Span(operator_syntax.start, operator_syntax.end)))) };
        case TargetSyntax::Symbol:
            auto symbol = operator_syntax.target._Symbol;
            if (symbol.routine.parameters != nullptr) {
                ParameterSetSyntax& parameterSetSyntax = *symbol.routine.parameters;
                auto _input_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                if (_input_result._tag == Result<Vector<Property>, ModelError>::Error)
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _input_result._Error };
                input = _input_result._Ok;
            }
            if (symbol.routine.returns != nullptr)
            {
                ParameterSetSyntax& parameterSetSyntax = symbol.routine.returns->parameters; 
                auto _output_result = handle_parameterset(_rp, _ep, parameterSetSyntax, file);
                if (_output_result._tag == Result<Vector<Property>, ModelError>::Error)
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = _output_result._Error };
                output = _output_result._Ok;
            }

            switch (symbol.routine.implementation._tag) {
                case ImplementationSyntax::Action: {
                    auto action_result = handle_action(_rp, _ep, symbol.routine.implementation._Action, file);
                    if (action_result._tag == Result<Statement, ModelError>::Error)
                        return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = action_result._Error };
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
                }
                case ImplementationSyntax::Extern:
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Extern, ._Extern = Extern() }) };
                case ImplementationSyntax::Instruction:
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Instruction, ._Instruction = Instruction() }) };
                case ImplementationSyntax::Intrinsic:
                    return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(start, end), private_, String(_rp, symbol.name), input, output, Implementation { ._tag = Implementation::Intrinsic, ._Intrinsic = Intrinsic() }) };
                break;

            }
    }
}

Result<HashMap<String, Nameable>, ModelError> build_symbols(Page* _rp, Page* _ep, String path, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations, String file) {
    Region _r;
    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.get_page()) HashMapBuilder<String, Nameable>();
    HashMapBuilder<String, List<Function>>& functions_builder = *new(alignof(HashMapBuilder<String, List<Function>>), _r.get_page()) HashMapBuilder<String, List<Function>>();

    if (declarations != nullptr) {
        auto declarations_iterator = VectorIterator<DeclarationSyntax>(declarations);
        while (auto declaration = declarations_iterator.next()) {
            switch (declaration->_tag) {
                case DeclarationSyntax::Private:{
                    auto export_ = declaration->_Private.export_;
                    switch (export_._tag) {
                        case ExportSyntax::Definition: {
                            auto concept_result = handle_definition(_rp, _ep, path, export_._Definition, true, file);
                            if (concept_result._tag == Result<Function, ModelError>::Error)
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = concept_result._Error };
                            auto concept = concept_result._Ok;
                            if (functions_builder.contains(concept.name))
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                            symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
                        }
                        break;
                        case ExportSyntax::Function: {
                            auto function_syntax = export_._Function;
                            auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, true, true, file);
                            if (function_result._tag == Result<Function, ModelError>::Error)
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = function_result._Error };
                            auto function = function_result._Ok;
                            auto symbol_with_function_name = symbols_builder[function.name];
                            if (symbol_with_function_name != nullptr)
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(declaration->_Function.start, declaration->_Function.end)))) };
                            List<Function>* functions_list = functions_builder[function.name];
                            if (functions_list == nullptr)
                            {
                                functions_builder.add(function.name, List<Function>());
                                functions_list = functions_builder[function.name];
                            }
                            (*functions_list).add(function);
                        }
                        break;
                        case ExportSyntax::Operator: {
                            auto operator_syntax = export_._Operator;
                            auto operator_result = handle_operator(_rp, _ep, operator_syntax, true, file);
                            if (operator_result._tag == Result<HashMap<String, Nameable>, ModelError>::Error)
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                                    ._Error = operator_result._Error };
                            auto operator_ = operator_result._Ok;
                            if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                                    ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
                        }
                        break;
                        case ExportSyntax::Trait:
                            return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Private Trait"), Span(declaration->_Trait.start, declaration->_Trait.end)))) };
                        case ExportSyntax::Module: {
                            auto module_syntax = export_._Module;
                            auto module_result = handle_module(_rp, _ep, path, module_syntax, true);
                            if (module_result._tag == Result<HashMap<String, Nameable>, ModelError>::Error)
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                                    ._Error = module_result._Error };
                            auto module = module_result._Ok;
                            if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                                return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                                    ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                        }
                        break;
                    }
                }
                break;
                case DeclarationSyntax::Definition: {
                    auto concept_result = handle_definition(_rp, _ep, path, declaration->_Definition, false, file);
                    if (concept_result._tag == Result<Function, ModelError>::Error)
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = concept_result._Error };
                    auto concept = concept_result._Ok;
                    if (functions_builder.contains(concept.name))
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(file, Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                    symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
                }
                break;
                case DeclarationSyntax::Function: {
                    auto function_syntax = declaration->_Function;
                    auto function_result = build_function(_rp, _ep, function_syntax.start, function_syntax.end, function_syntax.target, false, true, file);
                    if (function_result._tag == Result<Function, ModelError>::Error)
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = function_result._Error };
                    auto function = function_result._Ok;
                    auto symbol_with_function_name = symbols_builder[function.name];
                    if (symbol_with_function_name != nullptr)
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(declaration->_Function.start, declaration->_Function.end)))) };
                    List<Function>* functions_list = functions_builder[function.name];
                    if (functions_list == nullptr)
                    {
                        functions_builder.add(function.name, List<Function>());
                        functions_list = functions_builder[function.name];
                    }
                    (*functions_list).add(function);
                }
                break;
                case DeclarationSyntax::Operator: {
                    auto operator_syntax = declaration->_Operator;
                    auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, file);
                    if (operator_result._tag == Result<HashMap<String, Nameable>, ModelError>::Error)
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                            ._Error = operator_result._Error };
                    auto operator_ = operator_result._Ok;
                    if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(DuplicateName(file, Span(operator_syntax.start, operator_syntax.end)))) };
                }
                break;
                case DeclarationSyntax::Trait:
                    return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Trait"), Span(declaration->_Trait.start, declaration->_Trait.end)))) };
                case DeclarationSyntax::Macro:
                    return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(file, String(_ep, "Macro"), Span(declaration->_Macro.start, declaration->_Macro.end)))) };
                case DeclarationSyntax::Module: {
                    Region _r_1;
                    auto module_syntax = declaration->_Module;
                    auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                    if (module_result._tag == Result<HashMap<String, Nameable>, ModelError>::Error)
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                            ._Error = module_result._Error };
                    auto module = module_result._Ok;
                    if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                        return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(file, Span(module_syntax.start, module_syntax.end)))) };
                }
                break;
            }
        }
    }

    HashMap<String, List<Function>>& functions = *new(alignof(HashMap<String, List<Function>>), _r.get_page()) HashMap<String, List<Function>>(_r.get_page(), functions_builder);
    if (functions.slots != nullptr) {
        auto functions_slots_iterator = VectorIterator<Vector<KeyValuePair<String, List<Function>>>>(functions.slots);
        while (auto function_slot = functions_slots_iterator.next()) {
            auto functions_iterator = VectorIterator<KeyValuePair<String, List<Function>>>(function_slot);
            while (auto function_kvp = functions_iterator.next()) {
                symbols_builder.add(function_kvp->key, Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, function_kvp->value) });
            }
        }
    }

    return Result<HashMap<String, Nameable>, ModelError> { ._tag = Result<HashMap<String, Nameable>, ModelError>::Ok, ._Ok = HashMap<String, Nameable>(_rp, symbols_builder) };
}

Result<Module, ModelError> build_module(Page* _rp, Page* _ep, String path, String file_name, String name, FileSyntax& file_syntax, bool private_) {
    Region _r;
    List<Use>& uses = *new(alignof(List<Use>), _r.get_page()) List<Use>();
    if (file_syntax.uses != nullptr) {
        auto use_syntaxes = file_syntax.uses;
        auto _uses_iterator = VectorIterator<UseSyntax>(use_syntaxes);
        while (auto _use_ = _uses_iterator.next()) {
            auto use = *_use_;
            auto _use_result = handle_use(_rp, _ep, use);
            if (_use_result._tag == Result<Use, ModelError>::Error)
                return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = _use_result._Error };
            auto use_ = _use_result._Ok;
            uses.add(use_);
        }
    }

    auto symbols_result = build_symbols(_rp, _ep, path, file_syntax.uses, file_syntax.declarations, file_name);
    if (symbols_result._tag == Result<Concept, ModelError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = symbols_result._Error };
    auto symbols = symbols_result._Ok;
    
    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Ok,
        ._Ok = Module(private_, String(_rp, file_name), name, Vector<Use>(_rp, uses), symbols)
    };
}

Result<Module, ModelError> build_referenced_module(Page* _rp, Page* _ep, String path, String name, bool private_) {
    Region _r;
    StringBuilder& file_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), path, name));
    file_name_builder.append(".scaly");
    auto file_name = file_name_builder.to_string(_r.get_page());
    auto module_text_result = File::read_to_string(_r.get_page(), _r.get_page(), file_name);
    if (module_text_result._tag == Result<String, FileError>::Error) {
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(module_text_result._Error) };
    }
    auto module_text = module_text_result._Ok;

    Parser& parser = *new(alignof(Parser), _r.get_page()) Parser(_r.get_page(), module_text);
    auto file_syntax_result = parser.parse_file(_rp, _ep);
    if (file_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ParserModelError(file_name, file_syntax_result._Error)) };
    auto file_syntax = file_syntax_result._Ok;
    return build_module(_rp, _ep, path, file_name, name, file_syntax, private_);
}

Result<Module, ModelError> handle_module(Page* _rp, Page* _ep, String path, ModuleSyntax& module_syntax, bool private_) {
    return build_referenced_module(_rp, _ep, path, module_syntax.name, private_);
}

Result<Module, ModelError> handle_package(Page* _rp, Page* _ep, String path, PackageSyntax package_syntax) {
    return build_referenced_module(_rp, _ep, path, package_syntax.name.name, false);
}

Result<Program, ModelError> build_program(Page* _rp, Page* _ep, const String& file_name) {
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
        auto _package_iterator = VectorIterator<PackageSyntax>(file_packages);
        while (auto _package_syntax = _package_iterator.next()) {
            auto package_syntax = *_package_syntax;
            auto _package_result = build_referenced_module(_rp, _ep, path, _package_syntax->name.name, false);
            if (_package_result._tag == Result<Module, ModelError>::Error)
                return Result<Program, ModelError> { ._tag = Result<Program, ModelError>::Error, ._Error = _package_result._Error };
            auto package = _package_result._Ok;
            packages.add(package);
        }
    }

    auto module_result = build_module(_rp, _ep, Path::get_directory_name(_rp, file_name), file_name, Path::get_file_name_without_extension(_rp, file_name), program_syntax.file, false);
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