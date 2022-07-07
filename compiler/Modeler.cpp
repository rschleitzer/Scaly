namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct IoModelError {
    FileError file_error;
    IoModelError(FileError file_error) : file_error(file_error) {}
};

struct NotImplemented
{
    NotImplemented(Span _Span) : span(_Span) {}
    Span span;
};

struct DuplicateName {
    DuplicateName(Span span) : span(span) {}
    Span span;
};

struct NonFunctionSymbolExists {
    NonFunctionSymbolExists(Span span) : span(span) {}
    Span span;
};

struct FunctionSymbolExists {
    FunctionSymbolExists(Span span) : span(span) {}
    Span span;
};
struct ModelBuilderError {
    ModelBuilderError(NotImplemented _NotImplementedModelError) : _tag(NotImplemented), _NotImplemented(_NotImplementedModelError) {}
    ModelBuilderError(DuplicateName _DuplicateName) : _tag(DuplicateName), _DuplicateName(_DuplicateName) {}
    ModelBuilderError(NonFunctionSymbolExists _NonFunctionSymbolExists) : _tag(NonFunctionSymbolExists), _NonFunctionSymbolExists(_NonFunctionSymbolExists) {}
    ModelBuilderError(FunctionSymbolExists _FunctionSymbolExists) : _tag(FunctionSymbolExists), _FunctionSymbolExists(_FunctionSymbolExists) {}
    enum {
        NotImplemented,
        DuplicateName,
        NonFunctionSymbolExists,
        FunctionSymbolExists,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct DuplicateName _DuplicateName;
        struct NonFunctionSymbolExists _NonFunctionSymbolExists;
        struct FunctionSymbolExists _FunctionSymbolExists;
    };
};

struct ModelError : Object {
    ModelError(IoModelError ioModelError) : _tag(Io), _Io(ioModelError) {}
    ModelError(ParserError parserError) : _tag(Parser), _Parser(parserError) {}
    ModelError(ModelBuilderError modelBuilderError) : _tag(Builder), _Builder(modelBuilderError) {}
    enum {
        Io,
        Parser,
        Builder,
    } _tag;
    union {
        IoModelError _Io;
        ParserError _Parser;
        ModelBuilderError _Builder;
    };
};

Result<FileSyntax, ParserError> parse_program(Region& _pr, Page* _rp, Page* _ep, const String& program) {
    Region _r(_pr);
    Array<DeclarationSyntax>& declarations = *new(alignof(Array<DeclarationSyntax>), _r.page) Array<DeclarationSyntax>();
    
    // Parse the scaly module inclusion
    {
        Region _r_1(_r);
        Parser& parser_module = *new(alignof(Parser), _r_1.page) Parser(_r_1, _r_1.page, String(_r_1.page, "module scaly"));
        auto module_syntax_result = parser_module.parse_module(_r_1, _rp, _ep);
        if (module_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
            return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = module_syntax_result._Error };
        auto module_syntax = module_syntax_result._Ok;
        auto module_declaration = DeclarationSyntax(ModuleSyntax(module_syntax));
        declarations.add(module_declaration);
    }

    // Parse the uses and declarations of the program
    Vector<UseSyntax>* uses = nullptr;
    Vector<StatementSyntax>* statements = nullptr;
    {
        Region _r_1(_r);
        Parser& parser = *new(alignof(Parser), _r_1.page) Parser(_r_1, _r_1.page, program);
        while(true) {
            auto node_result = parser.parse_declaration(_r_1, _rp, _ep);
            if ((node_result._tag == Result<DeclarationSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<DeclarationSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                declarations.add(node);
            } else {
                break;
            }
        }

        Result<Vector<UseSyntax>*, ParserError> uses_result = parser.parse_use_list(_r_1, _rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Ok)
            uses = uses_result._Ok;

        Result<Vector<StatementSyntax>*, ParserError> statements_result = parser.parse_statement_list(_r_1, _rp, _ep);
        if (statements_result._tag == Result<Vector<StatementSyntax>, ParserError>::Ok)
            statements = statements_result._Ok;
    }

    // Parse the main function stub
    Parser& parser_main = *new(alignof(Parser), _r.page) Parser(_r, _r.page, String(_r.page, "function main('argument count': int, 'argument values': pointer[pointer[byte]]) returns int"));
    auto start = parser_main.lexer.previous_position;

    auto success_function = parser_main.lexer.parse_keyword(_r, _r.page, String(_r.page, "function"));
    if (!success_function)
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };

    auto name = parser_main.lexer.parse_identifier(_r, _r.page, parser_main.keywords);
    if (name != nullptr) {
        if (!parser_main.is_identifier(*name)) {
            return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };
        }
    }
    else
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };


    auto parameters_result = parser_main.parse_parameterset(_r, _rp, _ep);
    if (parameters_result._tag == Result<ParameterSetSyntax*, ParserError>::Error)
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = parameters_result._Error };
    auto parameters = parameters_result._Ok;

    auto returns_result = parser_main.parse_returns(_r, _rp, _ep);
    if (returns_result._tag == Result<ReturnsSyntax*, ParserError>::Error)
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = returns_result._Error };
    auto returns = returns_result._Ok;

    auto end = parser_main.lexer.position;

    auto block = new(alignof(BlockSyntax), _rp) BlockSyntax(start, end, uses, statements);
    auto block_expression = new (alignof(ExpressionSyntax), _rp) ExpressionSyntax(BlockSyntax(*block));
    auto operand = new(alignof(OperandSyntax), _rp) OperandSyntax(start, end, *block_expression, nullptr);
    Array<OperandSyntax>& operands_array = *new(alignof(Array<OperandSyntax>), _r.page) Array<OperandSyntax>();
    operands_array.add(*operand);
    auto operation = OperationSyntax(start, end, new(alignof(Vector<OperandSyntax>), _rp) Vector<OperandSyntax>(_rp, operands_array));
    auto action = ActionSyntax(OperationSyntax(operation));
    auto implementation = ImplementationSyntax(ActionSyntax(action));

    auto routine = RoutineSyntax(start, end, new (alignof(ParameterSetSyntax), _rp) ParameterSetSyntax(parameters), nullptr, new (alignof(ReturnsSyntax), _rp) ReturnsSyntax(returns), nullptr, implementation);
    FunctionSyntax& main_function_syntax = *new(alignof(FunctionSyntax), _rp) FunctionSyntax(start, end, *name, nullptr, routine);
    main_function_syntax.routine.implementation = implementation;
    DeclarationSyntax& main_function_declaration = *new (alignof(DeclarationSyntax), _rp) DeclarationSyntax(FunctionSyntax(main_function_syntax));;
    declarations.add(main_function_declaration);

    FileSyntax file_syntax(0, 0, nullptr, new(alignof(Vector<DeclarationSyntax>), _rp) Vector<DeclarationSyntax>(_rp, declarations), nullptr);
    return Result<FileSyntax, ParserError> { 
        ._tag = Result<FileSyntax, ParserError>::Ok, 
        ._Ok = file_syntax
    };
}

Result<Vector<Property>, ModelError> handle_parameterset(Region& _pr, Page* _rp, Page* _ep, ParameterSetSyntax& parameterSetSyntax) {
    Region _r(_pr);
    Array<Property>& parameters = *new(alignof(Array<Property>), _r.page) Array<Property>();
    switch (parameterSetSyntax._tag) {
        case ParameterSetSyntax::Parameters: {
            auto parameters = parameterSetSyntax._Parameters;
        }
        break;
        case ParameterSetSyntax::Type: {
            auto type_syntax = parameterSetSyntax._Type;
            parameters.add(Property(nullptr, nullptr));
        }
        break;
    }
    return Result<Vector<Property>, ModelError> {
        ._tag = Result<Vector<Property>, ModelError>::Ok, 
        ._Ok = Vector<Property>(_rp, parameters) };
}

Result<HashMap<String, Property>, ModelError> handle_structure(Region& _pr, Page* _rp, Page* _ep, StructureSyntax& structure) {    
    Region _r(_pr);
    HashMapBuilder<String, Property>& properties_builder = *new(alignof(HashMapBuilder<String, Property>), _r.page) HashMapBuilder<String, Property>();
    /// Fill it
    return Result<HashMap<String, Property>, ModelError> { ._tag = Result<HashMap<String, Property>, ModelError>::Ok, ._Ok = HashMap<String, Property>(_r, _rp, properties_builder) };
}

Result<Code, ModelError> build_code(Region& _pr, Page* _rp, Page* _ep, String name, String path, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations);

Result<Code, ModelError> handle_body(Region& _pr, Page* _rp, Page* _ep, String name, String path, BodySyntax& body) {    
    Region _r(_pr);
    auto code_result = build_code(_r, _rp, _ep, name, path, body.uses, body.declarations);
    if (code_result._tag == Result<Code, ModelError>::Error)
        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = code_result._Error };

    auto code = code_result._Ok;
    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Ok, ._Ok = code };
}

Result<Structure, ModelError> handle_class(Region& _pr, Page* _rp, Page* _ep, String name, String path, ClassSyntax& class_) {    
    Region _r(_pr);
    // return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplementedModelError(Span(class_.start, class_.end)))) };

    auto properties_result = handle_structure(_pr, _rp, _ep, class_.structure);
    if (properties_result._tag == Result<HashMap<String, Property>, ModelError>::Error)
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = properties_result._Error };
    auto properties = properties_result._Ok;

    if (class_.body == nullptr) {
        HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.page) HashMapBuilder<String, Nameable>();
        auto code = Code(HashMap<String, Nameable>(_r, _rp, symbols_builder));
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_.start, class_.end), properties, code)
        };
    }
    else {
        auto code_result = handle_body(_pr, _rp, _ep, name, path, *class_.body);
        if (code_result._tag == Result<Code, ModelError>::Error)
            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = code_result._Error };
        auto code = code_result._Ok;

        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_.start, class_.end), properties, code)
        };
    }
}

Result<Operation, ModelError> handle_operation(Region& _pr, Page* _rp, Page* _ep, OperationSyntax& operation) {
    return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(operation.start, operation.end)))) };
}

Result<Concept, ModelError> handle_definition(Region& _pr, Page* _rp, Page* _ep, String name, String path, DefinitionSyntax& definition) {
    Region _r(_pr);

    auto concept = definition.concept_;
    Span span(definition.start, definition.end);
    Type type(String(_rp, definition.type.name.name));
    switch (concept._tag)
    {
        case ConceptSyntax::Class: {
            auto class_ = concept._Class;
            auto structure_result = handle_class(_r, _rp, _ep, name, path, class_);
            if (structure_result._tag == Result<Structure, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = structure_result._Error };
            auto structure = structure_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, type,
                    Implementation { ._tag = Implementation::Structure, ._Structure = structure }
                )};
        }
        case ConceptSyntax::Namespace:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(concept._Namespace.start, concept._Namespace.end)))) };
        case ConceptSyntax::Union:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(concept._Union.start, concept._Union.end)))) };
        case ConceptSyntax::Constant: {
            auto constant = concept._Constant;
            auto operation_result = handle_operation(_r, _rp, _ep, constant.operation);
            if (operation_result._tag == Result<Operation, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = operation_result._Error };
            auto operation = operation_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, type,
                    Implementation { ._tag = Implementation::Constant, ._Constant = operation }
                )};
        }
        case ConceptSyntax::Delegate:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(concept._Delegate.start, concept._Delegate.end)))) };
        case ConceptSyntax::Intrinsic:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, type,
                    Implementation { ._tag = Implementation::Intrinsic }
                )};
    }
}

Result<Function, ModelError> handle_function(Region& _pr, Page* _rp, Page* _ep, FunctionSyntax& function_syntax) {
    Region _r(_pr);
    Vector<Property>* input = nullptr;
    Vector<Property>* output = nullptr;
    Operation* operation = nullptr;
    if (function_syntax.routine.parameters != nullptr) {
        ParameterSetSyntax& parameterSetSyntax = *function_syntax.routine.parameters;
        auto parameterset_result = handle_parameterset(_r, _rp, _ep, parameterSetSyntax);
    }
    if (function_syntax.routine.returns != nullptr)
    {
        ParameterSetSyntax& parameterSetSyntax = function_syntax.routine.returns->parameters; 
        auto parameterset_result = handle_parameterset(_r, _rp, _ep, parameterSetSyntax);
    }

    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(function_syntax.start, function_syntax.end), String(_rp, function_syntax.name), input, output, operation) };
}

Result<Operator, ModelError> handle_operator(Region& _pr, Page* _rp, Page* _ep, OperatorSyntax& operator_syntax) {
    Region _r(_pr);
    Vector<Property>* output = nullptr;
    Operation* operation = nullptr;
    switch (operator_syntax.target._tag) {
        case TargetSyntax::Routine:
            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(operator_syntax.start, operator_syntax.end)))) };
        case TargetSyntax::Symbol:
            auto operator_ = operator_syntax.target._Symbol;
            if (operator_.returns != nullptr) {
                ParameterSetSyntax& parameterSetSyntax = operator_.returns->parameters; 
                auto parameterset_result = handle_parameterset(_r, _rp, _ep, parameterSetSyntax);
            }
            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(operator_syntax.start, operator_syntax.end), String(_rp, operator_.name), output, operation) };
    }
}

Result<Concept, ModelError> build_module_concept(Region& _pr, Page* _rp, Page* _ep, String name, String path, FileSyntax file_syntax);

Result<Module, ModelError> handle_module(Region& _pr, Page* _rp, Page* _ep, String path, ModuleSyntax& module_syntax) {
    Region _r(_pr);
    StringBuilder& file_name_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
    if (path.get_length() > 0) {
        file_name_builder.append_string(path);
        file_name_builder.append_character('/');
    }
    file_name_builder.append_string(module_syntax.name.name);
    file_name_builder.append_string(String(_r.page, ".scaly"));
    auto file_name = file_name_builder.to_string(_rp);
    auto module_text_result = File::read_to_string(_r, _r.page, _r.page, file_name);
    if (module_text_result._tag == Result<String, FileError>::Error) {
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(module_text_result._Error) };
    }
    auto module_text = module_text_result._Ok;

    Parser& parser = *new(alignof(Parser), _r.page) Parser(_r, _r.page, module_text);
    auto file_syntax_result = parser.parse_file(_r, _rp, _ep);
    if (file_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = file_syntax_result._Error };
    auto file_syntax = file_syntax_result._Ok;

    StringBuilder& path_builder = *new(alignof(StringBuilder), _r.page) StringBuilder(path);
    if (path.get_length() > 0)
        file_name_builder.append_character('/');
    path_builder.append_string(module_syntax.name.name);


    auto concept_result = build_module_concept(_r, _rp, _ep, String(_rp, module_syntax.name.name), path_builder.to_string(_r.page), file_syntax);
    if (concept_result._tag == Result<Concept, ModelError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = concept_result._Error };
    auto concept = concept_result._Ok;
    
    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Ok,
        ._Ok = Module(String(_rp, module_syntax.name.name), Text { ._tag = Text::File, ._File = file_name }, concept)
    };
}

Result<Code, ModelError> build_code(Region& _pr, Page* _rp, Page* _ep, String name, String path, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations) {
    Region _r(_pr);

    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.page) HashMapBuilder<String, Nameable>();
    HashMapBuilder<String, Array<Function>>& functions_builder = *new(alignof(HashMapBuilder<String, Array<Function>>), _r.page) HashMapBuilder<String, Array<Function>>();

    if (declarations != nullptr) {
        auto declarations_iterator = VectorIterator<DeclarationSyntax>(*(declarations));
        while (auto declaration = declarations_iterator.next()) {
            Region _r_1(_r);
            switch (declaration->_tag) {
                case DeclarationSyntax::Private:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(declaration->_Private.start, declaration->_Private.end)))) };
                case DeclarationSyntax::Definition: {
                    auto concept_result = handle_definition(_r_1, _rp, _ep, name, path, declaration->_Definition);
                    if (concept_result._tag == Result<Function, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = concept_result._Error };
                    auto concept = concept_result._Ok;
                    if (functions_builder.contains(concept.type.name))
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                    symbols_builder.add(concept.type.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
                }
                break;
                case DeclarationSyntax::Function: {
                    auto function_result = handle_function(_r_1, _rp, _ep, declaration->_Function);
                    if (function_result._tag == Result<Function, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = function_result._Error };
                    auto function = function_result._Ok;
                    auto symbol_with_function_name = symbols_builder[function.name];
                    if (symbol_with_function_name != nullptr)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(Span(declaration->_Function.start, declaration->_Function.end)))) };
                    Array<Function>* functions_array = functions_builder[function.name];
                    if (functions_array == nullptr)
                    {
                        functions_builder.add(function.name, Array<Function>());
                        functions_array = functions_builder[function.name];
                    }
                    functions_array->add(function);
                }
                break;
                case DeclarationSyntax::Procedure:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(declaration->_Procedure.start, declaration->_Procedure.end)))) };
                case DeclarationSyntax::Operator: {
                    auto operator_syntax = declaration->_Operator;
                    auto operator_result = handle_operator(_r_1, _rp, _ep, operator_syntax);
                    if (operator_result._tag == Result<Code, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = operator_result._Error };
                    auto operator_ = operator_result._Ok;
                    if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(DuplicateName(Span(operator_syntax.start, operator_syntax.end)))) };
                }
                break;
                case DeclarationSyntax::Implement:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(declaration->_Implement.start, declaration->_Implement.end)))) };
                case DeclarationSyntax::Trait:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(declaration->_Trait.start, declaration->_Trait.end)))) };
                case DeclarationSyntax::Macro:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(Span(declaration->_Macro.start, declaration->_Macro.end)))) };
                case DeclarationSyntax::Module:
                    auto module_syntax = declaration->_Module;
                    auto module_result = handle_module(_r_1, _rp, _ep, path, module_syntax);
                    if (module_result._tag == Result<Code, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = module_result._Error };
                    auto module = module_result._Ok;
                    if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(Span(module_syntax.start, module_syntax.end)))) };
                    break;
            }
        }
    }

    HashMap<String, Array<Function>> functions(_r, _r.page, functions_builder);
    if (functions.slots != nullptr) {
        auto functions_slots_iterator = VectorIterator<Vector<KeyValuePair<String, Array<Function>>>>(*functions.slots);
        while (auto function_slot = functions_slots_iterator.next()) {
            auto functions_iterator = VectorIterator<KeyValuePair<String, Array<Function>>>(*function_slot);
            while (auto function_kvp = functions_iterator.next()) {
                symbols_builder.add(function_kvp->key, Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, function_kvp->value) });
            }
        }
    }

    auto code = Code(HashMap<String, Nameable>(_r, _rp, symbols_builder));
    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Ok, ._Ok = code };
}

Result<Concept, ModelError> build_module_concept(Region& _pr, Page* _rp, Page* _ep, String name, String path, FileSyntax file_syntax) {
    Region _r(_pr);

    auto code_result = build_code(_r, _rp, _ep, name, path, file_syntax.uses, file_syntax.declarations);
    if (code_result._tag == Result<Code, ModelError>::Error)
        return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = code_result._Error };

    auto code = code_result._Ok;

    return Result<Concept, ModelError> {
        ._tag = Result<Concept, ModelError>::Ok, 
        ._Ok = Concept(Span(file_syntax.start, file_syntax.end),
                    Type(String(_rp, name)),
                    Implementation { ._tag = Implementation::NameSpace, ._NameSpace = NameSpace(code) }) };
}

Result<Module, ModelError> build_program_module(Region& _pr, Page* _rp, Page* _ep, const String& program) {
    Region _r(_pr);
    
    auto file_result = parse_program(_r, _r.page, _ep, program);
    if (file_result._tag == Result<Vector<DeclarationSyntax>*, ParserError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(file_result._Error) };
    auto file = file_result._Ok;

    auto concept_result = build_module_concept(_r, _rp, _ep, String(_rp, ""), String(_rp, ""), file);
    if (concept_result._tag == Result<Module*, ModelError*>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = concept_result._Error };
    auto concept = concept_result._Ok;

    Module module(String(_rp, ""), Text { ._tag = Text::Program, ._Program = String(_rp, program) }, concept);
    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Ok, ._Ok = module };
}

}
}
}