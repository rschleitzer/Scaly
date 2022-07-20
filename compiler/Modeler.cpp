namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

Result<FileSyntax, ParserError> parse_program(Page* _rp, Page* _ep, const String& program) {
    Region _r;
    Array<DeclarationSyntax>& declarations = *new(alignof(Array<DeclarationSyntax>), _r.page) Array<DeclarationSyntax>();
    
    // Parse the scaly module inclusion
    {
        Region _r_1;
        Parser& parser_module = *new(alignof(Parser), _r_1.page) Parser(_r_1.page, String(_r_1.page, "module scaly"));
        auto module_syntax_result = parser_module.parse_module(_rp, _ep);
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
        Region _r_1;
        Parser& parser = *new(alignof(Parser), _r_1.page) Parser(_r_1.page, program);
        while(true) {
            auto node_result = parser.parse_declaration(_rp, _ep);
            if ((node_result._tag == Result<DeclarationSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<DeclarationSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                declarations.add(node);
            } else {
                break;
            }
        }

        Result<Vector<UseSyntax>*, ParserError> uses_result = parser.parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Ok)
            uses = uses_result._Ok;

        Result<Vector<StatementSyntax>*, ParserError> statements_result = parser.parse_statement_list(_rp, _ep);
        if (statements_result._tag == Result<Vector<StatementSyntax>, ParserError>::Ok)
            statements = statements_result._Ok;
    }

    // Parse the main function stub
    Parser& parser_main = *new(alignof(Parser), _r.page) Parser(_r.page, String(_r.page, "function main('argument count': int, 'argument values': pointer[pointer[byte]]) returns int"));
    auto start = parser_main.lexer.previous_position;

    auto success_function = parser_main.lexer.parse_keyword(_r.page, String(_r.page, "function"));
    if (!success_function)
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start, parser_main.lexer.position, String(_ep, "function"))) };

    auto name = parser_main.lexer.parse_identifier(_r.page, parser_main.keywords);
    if (name != nullptr) {
        if (!parser_main.is_identifier(*name)) {
            return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start, parser_main.lexer.position, String(_ep, "main"))) };
        }
    }
    else
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start, parser_main.lexer.position, String(_ep, "main"))) };


    auto parameters_result = parser_main.parse_parameterset(_rp, _ep);
    if (parameters_result._tag == Result<ParameterSetSyntax*, ParserError>::Error)
        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = parameters_result._Error };
    auto parameters = parameters_result._Ok;

    auto returns_result = parser_main.parse_returns(_rp, _ep);
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

Result<Vector<Property>, ModelError> handle_parameterset(Page* _rp, Page* _ep, ParameterSetSyntax& parameterSetSyntax) {
    Region _r;
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

Result<HashMap<String, Property>, ModelError> handle_structure(Page* _rp, Page* _ep, StructureSyntax& structure) {    
    Region _r;
    HashMapBuilder<String, Property>& properties_builder = *new(alignof(HashMapBuilder<String, Property>), _r.page) HashMapBuilder<String, Property>();
    /// Fill it
    return Result<HashMap<String, Property>, ModelError> { ._tag = Result<HashMap<String, Property>, ModelError>::Ok, ._Ok = HashMap<String, Property>(_rp, properties_builder) };
}

Result<Code, ModelError> build_code(Page* _rp, Page* _ep, String name, String path, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations, const Text& text);

Result<Code, ModelError> handle_body(Page* _rp, Page* _ep, String name, String path, BodySyntax& body, const Text& text) {    
    auto code_result = build_code(_rp, _ep, name, path, body.uses, body.declarations, text);
    if (code_result._tag == Result<Code, ModelError>::Error)
        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = code_result._Error };

    auto code = code_result._Ok;
    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Ok, ._Ok = code };
}

Result<Structure, ModelError> handle_class(Page* _rp, Page* _ep, String name, String path, ClassSyntax& class_, bool private_, const Text& text) {    
    auto properties_result = handle_structure(_rp, _ep, class_.structure);
    if (properties_result._tag == Result<HashMap<String, Property>, ModelError>::Error)
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = properties_result._Error };
    auto properties = properties_result._Ok;

    if (class_.body == nullptr) {
        Region _r_1;
        HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r_1.page) HashMapBuilder<String, Nameable>();
        auto code = Code(HashMap<String, Nameable>(_rp, symbols_builder), nullptr, nullptr);
        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_.start, class_.end), private_, properties, code)
        };
    }
    else {
        auto code_result = handle_body(_rp, _ep, name, path, *class_.body, text);
        if (code_result._tag == Result<Code, ModelError>::Error)
            return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Error, ._Error = code_result._Error };
        auto code = code_result._Ok;

        return Result<Structure, ModelError> { ._tag = Result<Structure, ModelError>::Ok,
            ._Ok = Structure(Span(class_.start, class_.end), private_, properties, code)
        };
    }
}

Result<Postfix, ModelError> handle_postfix(Page* _rp, Page* _ep, PostfixSyntax& operand) {
    return Result<Postfix, ModelError> { ._tag = Result<Postfix, ModelError>::Ok, ._Ok = Postfix() };
}

Result<Constant, ModelError> handle_literal(Page* _rp, Page* _ep, LiteralSyntax& literal, const Text& text) {
    Region _r;
    switch (literal.literal._tag) {
        case Literal::Boolean:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError> ::Ok, ._Ok = Constant { ._tag = Constant::Boolean, ._Boolean = literal.literal._Boolean.value } };
        case Literal::Integer: {
            auto integer = (size_t)strtol(literal.literal._Integer.value.to_c_string(_r.page), nullptr, 10);
            const bool range_error = errno == ERANGE;
            if (range_error)
                return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidConstant(text, Span(literal.start, literal.end)))) };

            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError> ::Ok, ._Ok = Constant { ._tag = Constant::Integer, ._Integer = integer } };
        }
        case Literal::Hex: {
            auto hex = (size_t)strtol(literal.literal._Hex.value.to_c_string(_r.page), nullptr, 16);
            const bool range_error = errno == ERANGE;
            if (range_error)
                return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidConstant(text, Span(literal.start, literal.end)))) };

            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError> ::Ok, ._Ok = Constant { ._tag = Constant::Hex, ._Hex = hex } };
        }
        case Literal::FloatingPoint: {
            auto floating_point = strtod(literal.literal._FloatingPoint.value.to_c_string(_r.page), nullptr);
            const bool range_error = errno == ERANGE;
            if (range_error)
                return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidConstant(text, Span(literal.start, literal.end)))) };

            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::FloatingPoint, ._FloatingPoint = floating_point } };
        }
        case Literal::String:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::String, ._String = literal.literal._String.value } };
        case Literal::Fragment:
            return Result<Constant, ModelError> { ._tag = Result<Constant, ModelError>::Ok, ._Ok = Constant { ._tag = Constant::Fragment, ._Fragment = literal.literal._Fragment.value } };
        break;
    }
}

Result<Type, ModelError> handle_type(Page* _rp, Page* _ep, TypeSyntax& type, const Text& text) {
    return Result<Type, ModelError> { ._tag = Result<Type, ModelError>::Ok, ._Ok = Type(String(_rp, type.name.name)) };
}

Result<Type, ModelError> handle_binding_annotation(Page* _rp, Page* _ep, BindingAnnotationSyntax& binding_annotation, const Text& text) {
    switch (binding_annotation.spec._tag) {
        case BindingSpecSyntax::Type: {
            auto type_result = handle_type(_rp, _ep, binding_annotation.spec._Type, text);
            if (type_result._tag == Result<Type, ModelError>::Error)
                return Result<Type, ModelError> { ._tag = Result<Type, ModelError>::Error, ._Error = type_result._Error };
            return Result<Type, ModelError> { ._tag = Result<Type, ModelError>::Ok, ._Ok = type_result._Ok };
        }
        case BindingSpecSyntax::Structure:
            return Result<Type, ModelError> { ._tag = Result<Type, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Structure"), Span(binding_annotation.spec._Structure.start, binding_annotation.spec._Structure.end)))) };
        case BindingSpecSyntax::Array:
            return Result<Type, ModelError> { ._tag = Result<Type, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Array"), Span(binding_annotation.spec._Array.start, binding_annotation.spec._Array.end)))) };

    }
}

Result<Operation, ModelError> handle_operation(Page* _rp, Page* _ep, OperationSyntax& operation, const Text& text);

Result<Vector<Statement>, ModelError> handle_statements(Page* _rp, Page* _ep, Vector<StatementSyntax>& statements, const Text& text) {
    Region _r;
    Array<Statement>& statements_builder = *new(alignof(Array<Statement>), _r.page) Array<Statement>();
    auto _statements_iterator = VectorIterator<StatementSyntax>(statements);
    while (auto statement = _statements_iterator.next()) {
        switch (statement->_tag)
        {
            case StatementSyntax::Operation: {
                auto operation_result = handle_operation(_rp, _ep, statement->_Operation, text);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                statements_builder.add(Statement { ._tag = Statement::Action, ._Action = { ._tag = Action::Operation, ._Operation = operation_result._Ok }});
                break;
            }
            case StatementSyntax::Let: {
                auto binding = statement->_Let.binding.operation;
                Type* type = nullptr;
                if (statement->_Let.binding.annotation != nullptr) {
                    auto _type_result = handle_binding_annotation(_rp, _ep, *statement->_Let.binding.annotation, text);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _type_result._Error };
                    type = new(alignof(Type), _rp) Type(_type_result._Ok);
                }
                auto operation_result = handle_operation(_rp, _ep, binding, text);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                statements_builder.add(Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Constant, Property(new(alignof(String), _rp) String(_rp, statement->_Let.binding.name), type), operation_result._Ok) });
                return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
            }
            case StatementSyntax::Var: {
                auto binding = statement->_Var.binding.operation;
                Type* type = nullptr;
                if (statement->_Var.binding.annotation != nullptr) {
                    auto _type_result = handle_binding_annotation(_rp, _ep, *statement->_Var.binding.annotation, text);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _type_result._Error };
                    type = new(alignof(Type), _rp) Type(_type_result._Ok);
                }
                auto operation_result = handle_operation(_rp, _ep, binding, text);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                Array<Statement>& body_statements_builder = *new(alignof(Array<Statement>), _r.page) Array<Statement>();
                return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
            }
            case StatementSyntax::Mutable: {
                auto binding = statement->_Mutable.binding.operation;
                Type* type = nullptr;
                if (statement->_Mutable.binding.annotation != nullptr) {
                    auto _type_result = handle_binding_annotation(_rp, _ep, *statement->_Mutable.binding.annotation, text);
                    if (_type_result._tag == Result<Type, ModelError>::Error)
                        return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _type_result._Error };
                    type = new(alignof(Type), _rp) Type(_type_result._Ok);
                }
                auto operation_result = handle_operation(_rp, _ep, binding, text);
                if (operation_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = operation_result._Error };
                Array<Statement>& body_statements_builder = *new(alignof(Array<Statement>), _r.page) Array<Statement>();
                statements_builder.add(Statement { ._tag = Statement::Binding, ._Binding = Binding(Binding::Mutability::Mutable, Property(new(alignof(String), _rp) String(_rp, statement->_Mutable.binding.name), type), operation_result._Ok) });
                return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
            }
            case StatementSyntax::Set:
                auto _target_result = handle_operation(_rp, _ep, statement->_Set.target, text);
                if (_target_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _target_result._Error };
                auto target = _target_result._Ok;
                auto _source_result = handle_operation(_rp, _ep, statement->_Set.source, text);
                if (_source_result._tag == Result<Operation, ModelError>::Error)
                    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError>::Error, ._Error = _source_result._Error };
                auto source = _source_result._Ok;
                statements_builder.add(Statement { ._tag = Statement::Action, ._Action = { ._tag = Action::Mutation, ._Mutation = Mutation(source, target) }});
                break;
        }
    }
    
    return Result<Vector<Statement>, ModelError> { ._tag = Result<Vector<Statement>, ModelError> ::Ok, ._Ok = Vector<Statement>(_rp, statements_builder) };
}

Result<Vector<Operand>, ModelError> handle_operands(Page* _rp, Page* _ep, Vector<OperandSyntax>& operands, const Text& text);

Result<Component, ModelError> handle_component(Page* _rp, Page* _ep, ComponentSyntax& component, const Text& text) {
    Region _r;
    String* name = nullptr;
    if (component.value != nullptr) {
        if (component.operands != nullptr) {
            auto name_operands = *component.operands;
            if (name_operands.length > 0) {
                auto name_operand = *name_operands[0];
                if (name_operands.length > 1)
                    return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidComponentName(text, Span(name_operand.start, name_operand.end)))) };
                if (name_operand.postfixes != nullptr)
                    return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = ModelError(ModelBuilderError(InvalidComponentName(text, Span(name_operand.start, name_operand.end)))) };
                if (name_operand.expression._tag == ExpressionSyntax::Variable)
                    name = new(alignof(String), _rp) String(_rp, name_operand.expression._Variable.identifier);
            }
        }
        auto value = component.value->value;
        if (value.operands != nullptr) {
            auto operands = *value.operands;
            auto value_result = handle_operands(_rp, _ep, operands, text);
            if (value_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = value_result._Error };
            else
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, value_result._Ok)};
        } else {
            return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, Vector<Operand>(_rp, 0))};
        }
    } else {
        if (component.operands != nullptr) {
            auto operands = *component.operands;
            auto value_result = handle_operands(_rp, _ep, operands, text);
            if (value_result._tag == Result<Vector<Operand>, ModelError>::Error)
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Error, ._Error = value_result._Error };
            else
                return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, value_result._Ok)};
        } else {
            return Result<Component, ModelError> { ._tag = Result<Component, ModelError>::Ok, ._Ok = Component(Span(component.start, component.end), name, Vector<Operand>(_rp, 0))};
        }
    }
}

Result<Tuple, ModelError> handle_object(Page* _rp, Page* _ep, ObjectSyntax& object, const Text& text) {
    Region _r;
    Array<Component>& components_builder = *new(alignof(Array<Component>), _r.page) Array<Component>();
    if (object.components != nullptr) {
        auto components_iterator = VectorIterator<ComponentSyntax>(*(object.components));
        while (auto component = components_iterator.next()) {
            auto component_result =  handle_component(_rp, _ep, *component, text);
            if (component_result._tag == Result<Operand, ModelError>::Error)
                return Result<Tuple, ModelError> { ._tag = Result<Tuple, ModelError>::Error, ._Error = component_result._Error };
            components_builder.add(component_result._Ok);
        }
    }
    return Result<Tuple, ModelError> { ._tag = Result<Tuple, ModelError>::Ok, ._Ok = Tuple(Span(object.start, object.end), Vector<Component>(_rp, components_builder)) };
}

Result<Matrix, ModelError> handle_vector(Page* _rp, Page* _ep, VectorSyntax& vector, const Text& text) {
    Region _r;
    Array<Operation>& operations_builder = *new(alignof(Array<Operation>), _r.page) Array<Operation>();
    if (vector.elements != nullptr) {
        auto elements_iterator = VectorIterator<ElementSyntax>(*(vector.elements));
        while (auto element = elements_iterator.next()) {
            auto operation_result =  handle_operation(_rp, _ep, element->operation, text);
            if (operation_result._tag == Result<Operand, ModelError>::Error)
                return Result<Matrix, ModelError> { ._tag = Result<Matrix, ModelError>::Error, ._Error = operation_result._Error };
            operations_builder.add(operation_result._Ok);
        }
    }
    return Result<Matrix, ModelError> { ._tag = Result<Matrix, ModelError>::Ok, ._Ok = Matrix(Span(vector.start, vector.end), Vector<Operation>(_rp, operations_builder)) };
}

Result<Block, ModelError> handle_block(Page* _rp, Page* _ep, BlockSyntax& block, const Text& text) {
    Region _r;
    if (block.statements != nullptr) {
        auto _statements_result = handle_statements(_rp, _ep, *block.statements, text);
        if (_statements_result._tag == Result<Vector<Statement>, ModelError>::Error)
            return Result<Block, ModelError> { ._tag = Result<Block, ModelError>::Error, ._Error = _statements_result._Error };
        auto statements = _statements_result._Ok;
        return Result<Block, ModelError> { ._tag = Result<Block, ModelError>::Ok, ._Ok = Block(Span(block.start, block.end), statements) };
    }

    return Result<Block, ModelError> { ._tag = Result<Block, ModelError>::Ok, ._Ok = Block(Span(block.start, block.end), Vector<Statement>(_rp, 0)) };
}

Result<Action, ModelError> handle_action(Page* _rp, Page* _ep, ActionSyntax& action, const Text& text);

Result<If, ModelError> handle_if(Page* _rp, Page* _ep, IfSyntax& if_, const Text& text) {
    Region _r;
    Property* property = nullptr;
    auto condition_result = handle_operands(_rp, _ep, *if_.condition.operands, text);
    if (condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = condition_result._Error };
    auto condition = condition_result._Ok;
    Action consequent = Action { ._tag = Action::Operation, ._Operation = Operation(Vector<Operand>(_rp, 0)) };
    if (if_.consequent != nullptr) {
        auto _consequent_result = handle_action(_rp, _ep, *if_.consequent, text);
        if (_consequent_result._tag == Result<Action, ModelError>::Error)
            return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = _consequent_result._Error };
        consequent = Action(_consequent_result._Ok);
    }
    Action alternative = Action { ._tag = Action::Operation, ._Operation = Operation(Vector<Operand>(_rp, 0)) };
    if (if_.alternative != nullptr) {
        auto _alternative_result = handle_action(_rp, _ep, if_.alternative->alternative, text);
        if (_alternative_result._tag == Result<Action, ModelError>::Error)
            return Result<If, ModelError> { ._tag = Result<If, ModelError>::Error, ._Error = _alternative_result._Error };
        alternative = Action(_alternative_result._Ok);
    }
    return Result<If, ModelError> { ._tag = Result<If, ModelError>::Ok, ._Ok = If(Span(if_.start, if_.end), condition, property, consequent, alternative) };
}

Result<For, ModelError> handle_for(Page* _rp, Page* _ep, ForSyntax& for_, const Text& text) {
    Region _r;
    auto expression_result = handle_operands(_rp, _ep, *for_.expression.operands, text);
    if (expression_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<For, ModelError> { ._tag = Result<For, ModelError>::Error, ._Error = expression_result._Error };
    auto expression = expression_result._Ok;
    if (for_.name != nullptr)
        return Result<For, ModelError> { ._tag = Result<For, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Label in For"), Span(for_.name->start, for_.name->end)))) };

    auto _action_result = handle_action(_rp, _ep, for_.action, text);
    if (_action_result._tag == Result<Action, ModelError>::Error)
        return Result<For, ModelError> { ._tag = Result<For, ModelError>::Error, ._Error = _action_result._Error };
    auto action = Action(_action_result._Ok);
    return Result<For, ModelError> { ._tag = Result<For, ModelError>::Ok, ._Ok = For(Span(for_.start, for_.end), String(_rp, for_.variable), expression, action) };
}

Result<While, ModelError> handle_while(Page* _rp, Page* _ep, WhileSyntax& while_, const Text& text) {
    Region _r;
    auto _condition_result = handle_operands(_rp, _ep, *while_.condition.operands, text);
    if (_condition_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<While, ModelError> { ._tag = Result<While, ModelError>::Error, ._Error = _condition_result._Error };
    auto condition = _condition_result._Ok;
    if (while_.name != nullptr)
        return Result<While, ModelError> { ._tag = Result<While, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Label in While"), Span(while_.name->start, while_.name->end)))) };

    auto _action_result = handle_action(_rp, _ep, while_.action, text);
    if (_action_result._tag == Result<Action, ModelError>::Error)
        return Result<While, ModelError> { ._tag = Result<While, ModelError>::Error, ._Error = _action_result._Error };
    auto action = Action(_action_result._Ok);
    return Result<While, ModelError> { ._tag = Result<While, ModelError>::Ok, ._Ok = While(Span(while_.start, while_.end), condition, action) };
}

Result<SizeOf, ModelError> handle_size_of(Page* _rp, Page* _ep, SizeOfSyntax& size_of, const Text& text) {
    Region _r;
    auto type_result = handle_type(_rp, _ep, size_of.type, text);
    if (type_result._tag == Result<Vector<Operand>, ModelError>::Error)
        return Result<SizeOf, ModelError> { ._tag = Result<SizeOf, ModelError>::Error, ._Error = type_result._Error };
    auto type = type_result._Ok;
    return Result<SizeOf, ModelError> { ._tag = Result<SizeOf, ModelError>::Ok, ._Ok = SizeOf(Span(size_of.start, size_of.end), type) };
}

Result<Return, ModelError> handle_return(Page* _rp, Page* _ep, ReturnSyntax& return_, const Text& text) {
    Region _r;
    if (return_.result != nullptr) {
        auto result = return_.result->operands;
        auto result_result = handle_operands(_rp, _ep, *result, text);
        if (result_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Error, ._Error = result_result._Error };
        return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(return_.start, return_.end), result_result._Ok) };
    }
    return Result<Return, ModelError> { ._tag = Result<Return, ModelError>::Ok, ._Ok = Return(Span(return_.start, return_.end), Vector<Operand>(_rp, 0)) };
}

Result<Expression, ModelError> handle_expression(Page* _rp, Page* _ep, ExpressionSyntax& expression, const Text& text) {
    Region _r;
    switch (expression._tag) {
        case ExpressionSyntax::Literal: {
            auto literal = expression._Literal;
            auto constant_result = handle_literal(_rp, _ep, literal, text);
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
            auto object_result = handle_object(_rp, _ep, object, text);
            if (object_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = object_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Tuple, ._Tuple = object_result._Ok} };
        }
        case ExpressionSyntax::Vector: {
            auto vector = expression._Vector;
            auto vector_result = handle_vector(_rp, _ep, vector, text);
            if (vector_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = vector_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Matrix, ._Matrix = vector_result._Ok} };
        }
        case ExpressionSyntax::Block: {
            auto block = expression._Block;
            auto block_result = handle_block(_rp, _ep, block, text);
            if (block_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = block_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Block, ._Block = block_result._Ok} };
        }
        case ExpressionSyntax::If: {
            auto if_ = expression._If;
            auto if_result = handle_if(_rp, _ep, if_, text);
            if (if_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = if_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::If, ._If = if_result._Ok} };
        }
        case ExpressionSyntax::Match:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Match"), Span(expression._Match.start, expression._Match.end)))) };
        case ExpressionSyntax::Lambda:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Lambda"), Span(expression._Lambda.start, expression._Lambda.end)))) };
        case ExpressionSyntax::For: {
            auto for_ = expression._For;
            auto for_result = handle_for(_rp, _ep, for_, text);
            if (for_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = for_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::For, ._For = for_result._Ok} };
        }
        case ExpressionSyntax::While: {
            auto while_syntax = expression._While;
            auto _while_result = handle_while(_rp, _ep, while_syntax, text);
            if (_while_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = _while_result._Error };
            auto while_ = _while_result._Ok;
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::While, ._While = while_} };
        }
        case ExpressionSyntax::Repeat:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Repeat"), Span(expression._Repeat.start, expression._Repeat.end)))) };
        case ExpressionSyntax::SizeOf: {
            auto size_of = expression._SizeOf;
            auto size_of_result = handle_size_of(_rp, _ep, size_of, text);
            if (size_of_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = size_of_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::SizeOf, ._SizeOf = size_of_result._Ok} };
        }
        case ExpressionSyntax::Continue:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Continue"), Span(expression._Continue.start, expression._Continue.end)))) };
        case ExpressionSyntax::Break:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Break"), Span(expression._Break.start, expression._Break.end)))) };
        case ExpressionSyntax::Return: {
            auto return_ = expression._Return;
            auto return_result = handle_return(_rp, _ep, return_, text);
            if (return_result._tag == Result<Expression, ModelError>::Error)
                return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = return_result._Error };
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Ok, ._Ok = Expression { ._tag = Expression::Return, ._Return = return_result._Ok} };
        }
        case ExpressionSyntax::Throw:
            return Result<Expression, ModelError> { ._tag = Result<Expression, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Throw"), Span(expression._Throw.start, expression._Throw.end)))) };
    }
}

Result<Operand, ModelError> handle_operand(Page* _rp, Page* _ep, OperandSyntax& operand, const Text& text) {
    Vector<Postfix>* postfixes = nullptr;
    if (operand.postfixes != nullptr) {
        Region _r_1;
        Array<Postfix>& postfixes_builder = *new(alignof(Array<Postfix>), _r_1.page) Array<Postfix>();
        auto postfixes_iterator = VectorIterator<PostfixSyntax>(*(operand.postfixes));
        while (auto postfix = postfixes_iterator.next()) {
            auto postfix_result = handle_postfix(_rp, _ep, *postfix);
            if (postfix_result._tag == Result<Operand, ModelError>::Error)
                return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Error, ._Error = postfix_result._Error };
            postfixes_builder.add(postfix_result._Ok);
        }
        postfixes = new(alignof(Vector<Postfix>), _rp) Vector<Postfix>(_rp, postfixes_builder);
    }

    auto expression_result = handle_expression(_rp, _ep, operand.expression, text);
    if (expression_result._tag == Result<Expression, ModelError>::Error)
        return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Error, ._Error = expression_result._Error };

    return Result<Operand, ModelError> { ._tag = Result<Operand, ModelError>::Ok, ._Ok = Operand(Span(operand.expression._Literal.start, operand.expression._Literal.end), expression_result._Ok, postfixes) };
}

Result<Vector<Operand>, ModelError> handle_operands(Page* _rp, Page* _ep, Vector<OperandSyntax>& operands, const Text& text) {
    Region _r;
    Array<Operand>& operands_builder = *new(alignof(Array<Operand>), _r.page) Array<Operand>();
    auto operands_iterator = VectorIterator<OperandSyntax>(operands);
    while (auto operand = operands_iterator.next()) {
        auto operand_result = handle_operand(_rp, _ep, *operand, text);
        if (operand_result._tag == Result<Operand, ModelError>::Error)
            return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Error, ._Error = operand_result._Error };
        operands_builder.add(operand_result._Ok);
    }
    return Result<Vector<Operand>, ModelError> { ._tag = Result<Vector<Operand>, ModelError>::Ok, ._Ok = (Vector<Operand>(_rp, operands_builder)) };
}

Result<Operation, ModelError> handle_operation(Page* _rp, Page* _ep, OperationSyntax& operation, const Text& text) {
    if (operation.operands != nullptr) {
        auto operands = *operation.operands;
        auto operands_result = handle_operands(_rp, _ep, operands, text);
        if (operands_result._tag == Result<Vector<Operand>, ModelError>::Error)
            return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Error, ._Error = operands_result._Error };
        return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Ok, ._Ok = operands_result._Ok };
    }
     return Result<Operation, ModelError> { ._tag = Result<Operation, ModelError>::Ok, ._Ok = Operation(Vector<Operand>(_rp, 0)) };
}

Result<Concept, ModelError> handle_definition(Page* _rp, Page* _ep, String name, String path, DefinitionSyntax& definition, bool private_, const Text& text) {
    auto concept = definition.concept_;
    Span span(definition.start, definition.end);
    switch (concept._tag)
    {
        case ConceptSyntax::Class: {
            auto class_ = concept._Class;
            auto structure_result = handle_class(_rp, _ep, name, path, class_, private_, text);
            if (structure_result._tag == Result<Structure, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = structure_result._Error };
            auto structure = structure_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name),
                    Body { ._tag = Body::Structure, ._Structure = structure }
                )};
        }
        case ConceptSyntax::Namespace:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Namespace"), Span(concept._Namespace.start, concept._Namespace.end)))) };
        case ConceptSyntax::Union:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Union"), Span(concept._Union.start, concept._Union.end)))) };
        case ConceptSyntax::Constant: {
            auto constant = concept._Constant;
            auto operation_result = handle_operation(_rp, _ep, constant.operation, text);
            if (operation_result._tag == Result<Operation, ModelError>::Error)
                return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = operation_result._Error };
            auto operation = operation_result._Ok;
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name),
                    Body { ._tag = Body::Constant, ._Constant = operation }
                )};
        }
        case ConceptSyntax::Delegate:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Delegate"), Span(concept._Delegate.start, concept._Delegate.end)))) };
        case ConceptSyntax::Intrinsic:
            return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = 
                Concept(span, String(_rp, definition.name),
                    Body { ._tag = Body::Intrinsic }
                )};
    }
}

Result<Action, ModelError> handle_action(Page* _rp, Page* _ep, ActionSyntax& action, const Text& text) {
    switch (action._tag) {
        case ActionSyntax::Operation: {
            auto operation_result = handle_operation(_rp, _ep, action._Operation, text);
            if (operation_result._tag == Result<Operation, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = operation_result._Error };
            return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Ok, ._Ok = Action { ._tag = Action::Operation, ._Operation = operation_result._Ok } };
        }
        case ActionSyntax::Set: {
            auto _target_result = handle_operation(_rp, _ep, action._Set.target, text);
            if (_target_result._tag == Result<Operation, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = _target_result._Error };
            auto target = _target_result._Ok;
            auto _source_result = handle_operation(_rp, _ep, action._Set.source, text);
            if (_source_result._tag == Result<Operation, ModelError>::Error)
                return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Error, ._Error = _source_result._Error };
            auto source = _source_result._Ok;
            return Result<Action, ModelError> { ._tag = Result<Action, ModelError>::Ok, ._Ok = Action { ._tag = Action::Mutation, ._Mutation = Mutation(source, target) } };
        }
    }
}

Result<Function, ModelError> handle_function(Page* _rp, Page* _ep, FunctionSyntax& function_syntax, bool private_, const Text& text) {
    Vector<Property> input = Vector<Property>(_rp, 0);
    Vector<Property> output = Vector<Property>(_rp, 0);

    if (function_syntax.routine.parameters != nullptr) {
        ParameterSetSyntax& parameterSetSyntax = *function_syntax.routine.parameters;
        auto _input_result = handle_parameterset(_rp, _ep, parameterSetSyntax);
        if (_input_result._tag == Result<Vector<Property>, ModelError>::Error)
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _input_result._Error };
        input = _input_result._Ok;
    }

    if (function_syntax.routine.returns != nullptr)
    {
        ParameterSetSyntax& parameterSetSyntax = function_syntax.routine.returns->parameters; 
        auto _output_result = handle_parameterset(_rp, _ep, parameterSetSyntax);
        if (_output_result._tag == Result<Vector<Property>, ModelError>::Error)
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = _output_result._Error };
        output = _output_result._Ok;
    }

    switch (function_syntax.routine.implementation._tag) {
        case ImplementationSyntax::Action: {
            auto action_result = handle_action(_rp, _ep, function_syntax.routine.implementation._Action, text);
            if (action_result._tag == Result<Statement, ModelError>::Error)
                return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Error, ._Error = action_result._Error };
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(function_syntax.start, function_syntax.end), private_, String(_rp, function_syntax.name), input, output, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
        }
        case ImplementationSyntax::Extern:
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(function_syntax.start, function_syntax.end), private_, String(_rp, function_syntax.name), input, output, Implementation { ._tag = Implementation::Extern, ._Extern = Extern() }) };
        case ImplementationSyntax::Instruction:
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(function_syntax.start, function_syntax.end), private_, String(_rp, function_syntax.name), input, output, Implementation { ._tag = Implementation::Instruction, ._Instruction = Instruction() }) };
        case ImplementationSyntax::Intrinsic:
            return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(function_syntax.start, function_syntax.end), private_, String(_rp, function_syntax.name), input, output, Implementation { ._tag = Implementation::Intrinsic, ._Intrinsic = Intrinsic() }) };
        break;

    }

}

Result<Initializer, ModelError> handle_initializer(Page* _rp, Page* _ep, InitSyntax& initializer, bool private_, const Text& text) {
    Vector<Property>* input = nullptr;

    if (initializer.parameters != nullptr) {
        ParameterSetSyntax& parameterSetSyntax = *initializer.parameters;
        auto parameterset_result = handle_parameterset(_rp, _ep, parameterSetSyntax);
    }

    auto action_result = handle_action(_rp, _ep, initializer.action, text);
    if (action_result._tag == Result<Statement, ModelError>::Error)
        return Result<Initializer, ModelError> { ._tag = Result<Initializer, ModelError>::Error, ._Error = action_result._Error };

    return Result<Initializer, ModelError> { ._tag = Result<Initializer, ModelError>::Ok, ._Ok = Initializer(Span(initializer.start, initializer.end), private_, input, Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
}

Result<DeInitializer*, ModelError> handle_deinitializer(Page* _rp, Page* _ep, DeInitSyntax& deinitializer, const Text& text) {
    Vector<Property>* input = nullptr;

    auto action_result = handle_action(_rp, _ep, deinitializer.action, text);
    if (action_result._tag == Result<Statement, ModelError>::Error)
        return Result<DeInitializer*, ModelError> { ._tag = Result<DeInitializer*, ModelError>::Error, ._Error = action_result._Error };

    return Result<DeInitializer*, ModelError> { ._tag = Result<DeInitializer*, ModelError>::Ok, ._Ok = new(alignof(DeInitializer), _rp) DeInitializer(Span(deinitializer.start, deinitializer.end), Implementation { ._tag = Implementation::Action, ._Action = action_result._Ok }) };
}

Result<Operator, ModelError> handle_operator(Page* _rp, Page* _ep, OperatorSyntax& operator_syntax, bool private_, const Text& text) {
    Vector<Property>* output = nullptr;
    Operation* operation = nullptr;
    switch (operator_syntax.target._tag) {
        case TargetSyntax::Routine:
            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Routine"), Span(operator_syntax.start, operator_syntax.end)))) };
        case TargetSyntax::Symbol:
            auto operator_ = operator_syntax.target._Symbol;
            if (operator_.returns != nullptr) {
                ParameterSetSyntax& parameterSetSyntax = operator_.returns->parameters; 
                auto parameterset_result = handle_parameterset(_rp, _ep, parameterSetSyntax);
            }
            return Result<Operator, ModelError> { ._tag = Result<Operator, ModelError>::Ok, ._Ok = Operator(Span(operator_syntax.start, operator_syntax.end), private_, String(_rp, operator_.name), output, operation) };
    }
}

Result<Concept, ModelError> build_module_concept(Page* _rp, Page* _ep, String name, String path, FileSyntax file_syntax, const Text& text);

Result<Module, ModelError> handle_module(Page* _rp, Page* _ep, String path, ModuleSyntax& module_syntax, bool private_) {
    Region _r;
    StringBuilder& file_name_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
    if (path.get_length() > 0) {
        file_name_builder.append_string(path);
        file_name_builder.append_character('/');
    }
    file_name_builder.append_string(module_syntax.name.name);
    file_name_builder.append_string(String(_r.page, ".scaly"));
    auto file_name = file_name_builder.to_string(_rp);
    auto module_text_result = File::read_to_string(_r.page, _r.page, file_name);
    if (module_text_result._tag == Result<String, FileError>::Error) {
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(module_text_result._Error) };
    }
    auto module_text = module_text_result._Ok;

    Parser& parser = *new(alignof(Parser), _r.page) Parser(_r.page, module_text);
    auto file_syntax_result = parser.parse_file(_rp, _ep);
    auto text = Text {._tag = Text::File, ._Program = String(_ep, file_name) };
    if (file_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ParserModelError(text, file_syntax_result._Error)) };
    auto file_syntax = file_syntax_result._Ok;

    StringBuilder& path_builder = *new(alignof(StringBuilder), _r.page) StringBuilder(path);
    if (path.get_length() > 0)
        path_builder.append_character('/');
    path_builder.append_string(module_syntax.name.name);


    auto concept_result = build_module_concept(_rp, _ep, String(_rp, module_syntax.name.name), path_builder.to_string(_r.page), file_syntax, text);
    if (concept_result._tag == Result<Concept, ModelError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = concept_result._Error };
    auto concept = concept_result._Ok;
    
    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Ok,
        ._Ok = Module(private_, String(_rp, module_syntax.name.name), Text { ._tag = Text::File, ._File = file_name }, concept)
    };
}

Result<Code, ModelError> build_code(Page* _rp, Page* _ep, String name, String path, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations, const Text& text) {
    Region _r;
    HashMapBuilder<String, Nameable>& symbols_builder = *new(alignof(HashMapBuilder<String, Nameable>), _r.page) HashMapBuilder<String, Nameable>();
    HashMapBuilder<String, Array<Function>>& functions_builder = *new(alignof(HashMapBuilder<String, Array<Function>>), _r.page) HashMapBuilder<String, Array<Function>>();
    Array<Initializer>& initializers_builder = *new(alignof(Array<Initializer>), _r.page) Array<Initializer>();
    DeInitializer* deinitializer = nullptr;

    if (declarations != nullptr) {
        auto declarations_iterator = VectorIterator<DeclarationSyntax>(*(declarations));
        while (auto declaration = declarations_iterator.next()) {
            switch (declaration->_tag) {
                case DeclarationSyntax::Private:{
                    auto export_ = declaration->_Private.export_;
                    switch (export_._tag) {
                        case ExportSyntax::Definition: {
                            auto concept_result = handle_definition(_rp, _ep, name, path, export_._Definition, true, text);
                            if (concept_result._tag == Result<Function, ModelError>::Error)
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = concept_result._Error };
                            auto concept = concept_result._Ok;
                            if (functions_builder.contains(concept.name))
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(text, Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                            symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
                        }
                        break;
                        case ExportSyntax::Function: {
                            auto function_result = handle_function(_rp, _ep, export_._Function, true, text);
                            if (function_result._tag == Result<Function, ModelError>::Error)
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = function_result._Error };
                            auto function = function_result._Ok;
                            auto symbol_with_function_name = symbols_builder[function.name];
                            if (symbol_with_function_name != nullptr)
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(text, Span(declaration->_Function.start, declaration->_Function.end)))) };
                            Array<Function>* functions_array = functions_builder[function.name];
                            if (functions_array == nullptr)
                            {
                                functions_builder.add(function.name, Array<Function>());
                                functions_array = functions_builder[function.name];
                            }
                            functions_array->add(function);
                        }
                        break;
                        case ExportSyntax::Init: {
                            auto _initializer_result = handle_initializer(_rp, _ep, export_._Init, true, text);
                            if (_initializer_result._tag == Result<Initializer, ModelError>::Error)
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = _initializer_result._Error };
                            auto initializer = _initializer_result._Ok;
                            initializers_builder.add(initializer);
                        }
                        break;
                        case ExportSyntax::Procedure:
                            return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Private Procedure"), Span(declaration->_Procedure.start, declaration->_Procedure.end)))) };
                        case ExportSyntax::Operator: {
                            auto operator_syntax = export_._Operator;
                            auto operator_result = handle_operator(_rp, _ep, operator_syntax, true, text);
                            if (operator_result._tag == Result<Code, ModelError>::Error)
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                                    ._Error = operator_result._Error };
                            auto operator_ = operator_result._Ok;
                            if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                                    ._Error = ModelError(ModelBuilderError(DuplicateName(text, Span(operator_syntax.start, operator_syntax.end)))) };
                        }
                        break;
                        case ExportSyntax::Implement:
                            return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Private Implement"), Span(declaration->_Implement.start, declaration->_Implement.end)))) };
                        case ExportSyntax::Trait:
                            return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Private Trait"), Span(declaration->_Trait.start, declaration->_Trait.end)))) };
                        case ExportSyntax::Module: {
                            auto module_syntax = export_._Module;
                            auto module_result = handle_module(_rp, _ep, path, module_syntax, true);
                            if (module_result._tag == Result<Code, ModelError>::Error)
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                                    ._Error = module_result._Error };
                            auto module = module_result._Ok;
                            if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                                return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                                    ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(text, Span(module_syntax.start, module_syntax.end)))) };
                        }
                        break;
                    }
                }
                break;
                case DeclarationSyntax::Definition: {
                    auto concept_result = handle_definition(_rp, _ep, name, path, declaration->_Definition, false, text);
                    if (concept_result._tag == Result<Function, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = concept_result._Error };
                    auto concept = concept_result._Ok;
                    if (functions_builder.contains(concept.name))
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(FunctionSymbolExists(text, Span(declaration->_Definition.start, declaration->_Definition.end)))) };

                    symbols_builder.add(concept.name, Nameable { ._tag = Nameable::Concept, ._Concept = concept });
                }
                break;
                case DeclarationSyntax::Function: {
                    auto function_result = handle_function(_rp, _ep, declaration->_Function, false, text);
                    if (function_result._tag == Result<Function, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = function_result._Error };
                    auto function = function_result._Ok;
                    auto symbol_with_function_name = symbols_builder[function.name];
                    if (symbol_with_function_name != nullptr)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(text, Span(declaration->_Function.start, declaration->_Function.end)))) };
                    Array<Function>* functions_array = functions_builder[function.name];
                    if (functions_array == nullptr)
                    {
                        functions_builder.add(function.name, Array<Function>());
                        functions_array = functions_builder[function.name];
                    }
                    functions_array->add(function);
                }
                break;
                case DeclarationSyntax::Init: {
                    auto _initializer_result = handle_initializer(_rp, _ep, declaration->_Init, false, text);
                    if (_initializer_result._tag == Result<Initializer, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = _initializer_result._Error };
                    auto initializer = _initializer_result._Ok;
                    initializers_builder.add(initializer);
                }
                break;
                case DeclarationSyntax::DeInit: {
                    if (deinitializer != nullptr)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(DeInitializerExists(text, Span(declaration->_DeInit.start, declaration->_DeInit.end)))) };
                    auto _deinitializer_result = handle_deinitializer(_rp, _ep, declaration->_DeInit, text);
                    if (_deinitializer_result._tag == Result<DeInitializer, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = _deinitializer_result._Error };
                    deinitializer = _deinitializer_result._Ok;
                }
                break;
                case DeclarationSyntax::Procedure:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Procedure"), Span(declaration->_Procedure.start, declaration->_Procedure.end)))) };
                case DeclarationSyntax::Operator: {
                    auto operator_syntax = declaration->_Operator;
                    auto operator_result = handle_operator(_rp, _ep, operator_syntax, false, text);
                    if (operator_result._tag == Result<Code, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = operator_result._Error };
                    auto operator_ = operator_result._Ok;
                    if (!symbols_builder.add(operator_.name, Nameable { ._tag = Nameable::Operator, ._Operator = operator_ }))
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(DuplicateName(text, Span(operator_syntax.start, operator_syntax.end)))) };
                }
                break;
                case DeclarationSyntax::Implement:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Implement"), Span(declaration->_Implement.start, declaration->_Implement.end)))) };
                case DeclarationSyntax::Trait:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Trait"), Span(declaration->_Trait.start, declaration->_Trait.end)))) };
                case DeclarationSyntax::Macro:
                    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error, ._Error = ModelError(ModelBuilderError(NotImplemented(text, String(_ep, "Macro"), Span(declaration->_Macro.start, declaration->_Macro.end)))) };
                case DeclarationSyntax::Module: {
                    auto module_syntax = declaration->_Module;
                    auto module_result = handle_module(_rp, _ep, path, module_syntax, false);
                    if (module_result._tag == Result<Code, ModelError>::Error)
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = module_result._Error };
                    auto module = module_result._Ok;
                    if (!symbols_builder.add(module.name, Nameable { ._tag = Nameable::Module, ._Module = module }))
                        return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Error,
                            ._Error = ModelError(ModelBuilderError(NonFunctionSymbolExists(text, Span(module_syntax.start, module_syntax.end)))) };
                }
                break;
            }
        }
    }

    HashMap<String, Array<Function>> functions(_r.page, functions_builder);
    if (functions.slots != nullptr) {
        auto functions_slots_iterator = VectorIterator<Vector<KeyValuePair<String, Array<Function>>>>(*functions.slots);
        while (auto function_slot = functions_slots_iterator.next()) {
            auto functions_iterator = VectorIterator<KeyValuePair<String, Array<Function>>>(*function_slot);
            while (auto function_kvp = functions_iterator.next()) {
                symbols_builder.add(function_kvp->key, Nameable { ._tag = Nameable::Functions, ._Functions = Vector<Function>(_rp, function_kvp->value) });
            }
        }
    }

    Vector<Initializer>* initializers = nullptr;
    if (initializers_builder.length > 0)
        initializers = new(alignof(Vector<Initializer>), _rp) Vector<Initializer>(_rp, initializers_builder);

    auto code = Code(HashMap<String, Nameable>(_rp, symbols_builder), initializers, nullptr);
    return Result<Code, ModelError> { ._tag = Result<Code, ModelError>::Ok, ._Ok = code };
}

Result<Concept, ModelError> build_module_concept(Page* _rp, Page* _ep, String name, String path, FileSyntax file_syntax, const Text& text) {
    if (file_syntax.declarations != nullptr && file_syntax.declarations->length == 1) {
        auto first_declaration = *(*(file_syntax.declarations))[0];
        switch (first_declaration._tag) {
            case DeclarationSyntax::Definition: {
                auto definition_syntax = first_declaration._Definition;
                if (name.equals(definition_syntax.name))
                    return handle_definition(_rp, _ep, name, path, definition_syntax, false, text);
            }
            break;
            case DeclarationSyntax::Private: {
                auto private_syntax = first_declaration._Private;
                switch (private_syntax.export_._tag) {
                    case ExportSyntax::Definition: {
                        auto definition_syntax = private_syntax.export_._Definition;
                        if (name.equals(definition_syntax.name))
                            return handle_definition(_rp, _ep, name, path, definition_syntax, true, text);

                    }
                    break;
                    default:
                    break;
                }
            }
            break;
            default:
            break;
        }
    }

    auto code_result = build_code(_rp, _ep, name, path, file_syntax.uses, file_syntax.declarations, text);
    if (code_result._tag == Result<Code, ModelError>::Error)
        return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Error, ._Error = code_result._Error };

    auto code = code_result._Ok;

    return Result<Concept, ModelError> {
        ._tag = Result<Concept, ModelError>::Ok, 
        ._Ok = Concept(Span(file_syntax.start, file_syntax.end),
                    String(_rp, name),
                    Body { ._tag = Body::NameSpace, ._NameSpace = NameSpace(code) }) };
}

Result<Module, ModelError> build_program_module(Page* _rp, Page* _ep, const String& program) {
    Region _r;
    
    auto file_result = parse_program(_r.page, _ep, program);
    auto text = Text {._tag = Text::Program, ._Program = String(_ep, program) };
    if (file_result._tag == Result<Vector<DeclarationSyntax>*, ParserError>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = ModelError(ParserModelError(text, file_result._Error)) };
    auto file = file_result._Ok;

    auto concept_result = build_module_concept(_rp, _ep, String(_rp, ""), String(_rp, ""), file, text);
    if (concept_result._tag == Result<Module*, ModelError*>::Error)
        return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Error, ._Error = concept_result._Error };
    auto concept = concept_result._Ok;

    Module module(false, String(_rp, ""), Text { ._tag = Text::Program, ._Program = String(_rp, program) }, concept);
    return Result<Module, ModelError> { ._tag = Result<Module, ModelError>::Ok, ._Ok = module };
}

}
}
}