namespace scaly {
namespace compiler {
namespace model {

struct Span : Object {
    size_t file_id;
    size_t start;
    size_t end;
    Span(size_t file_id, size_t start, size_t end) : file_id(file_id), start(start), end(end) {}
};

struct Type {
    String name;
    Type(String name) : name(name) {}
};

struct Property {
    String* name;
    Type* type;
    Property(String* name, Type* type) : name(name), type(type) {}
};


struct NameExpression : Object {
    String path;
    NameExpression(String path) : path(path) {}
};

struct Expression;

struct Operand : Object {
    Expression* expression;
};


struct Component : Object {
    Span span;
    String name;
    Vector<Operand> value;
    Component(Span span, String name, Vector<Operand> value) : span(span), name(name), value(value) {}
};


struct TupleExpression : Object {
    Span span;
    Vector<Component> components;
    TupleExpression(Span span, Vector<Component> components) : span(span), components(components) {}
};

struct NameExpression;
struct TupleExpression;
struct Expression : Object {
    Expression(NameExpression nameExpression) : tag(Name), nameExpression(nameExpression) {}
    Expression(TupleExpression tupleExpression) : tag(Tuple), tupleExpression(tupleExpression) {}
    enum {
        Name,
        Tuple,
    } tag;
    union {
        NameExpression nameExpression;
        TupleExpression tupleExpression;
    };

};

struct Operation : Object {
    Vector<Operand> operands;
    Operation(Vector<Operand> operands) : operands(operands) {}
};

struct Function : Object {
    Span span;
    String name;
    Vector<Property>* input;
    Vector<Property>* output;
    Operation* operation;
    Function(Span span, String name, Vector<Property>* input, Vector<Property>* output, Operation* operation) : span(span), name(name), input(input), output(output), operation(operation) {}
};

struct Model : Object {
    MultiMap<String, Function> functions;
    Model(MultiMap<String, Function> functions) : functions(functions) {};
};

struct IoModelError {
    String file_name;
};

struct DuplicateNameError
{
    size_t file_id;
    size_t start;
    size_t end;
};

struct ModelBuilderError {
    ModelBuilderError(DuplicateNameError& duplicateNameError) : duplicateNameError(duplicateNameError) {}
    enum {
        DuplicateName,
    } tag;
    union {
        DuplicateNameError duplicateNameError;
    };
};

struct ModelError : Object {
    ModelError(ParserError& parserError) : parserError(parserError) {}
    ModelError(ModelBuilderError& modelBuilderError) : modelBuilderError(modelBuilderError) {}
    enum {
        Io,
        Parser,
        Builder,
    } tag;
    union {
        IoModelError ioModelError;
        ParserError parserError;
        ModelBuilderError modelBuilderError;
    };
};

Result<Vector<DeclarationSyntax>*, ParserError> parse_program(Region& _pr, Page* _rp, Page* _ep, String& program) {
    auto _r = Region::create(_pr);
    Array<DeclarationSyntax>& declarations = *Array<DeclarationSyntax>::create(_r.page);
    
    // Parse the scaly module inclusion
    Parser& parser_module = *new(alignof(Parser), _r.page) Parser(*String::from_c_string(_r.page, "module scaly"));
    auto module_syntax_result = parser_module.parse_module(_r, _rp, _ep);
    if (module_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
        return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = module_syntax_result._Error };
    auto module_syntax = module_syntax_result._Ok;
    auto module_declaration = DeclarationSyntax(ModuleSyntax(module_syntax));
    declarations.add(module_declaration);

    // Parse the declarations of the program
    Parser& parser = *new(alignof(Parser), _r.page) Parser(program);
    while(true) {
        auto node_result = parser.parse_declaration(_r, _rp, _ep);
        if ((node_result._tag == Result<DeclarationSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
            return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = node_result._Error };
        if (node_result._tag == Result<DeclarationSyntax, ParserError>::Ok) {
            auto node = node_result._Ok;
            declarations.add(node);
        } else {
            break;
        }
    }

    // Parse the main function stub
    Parser& parser_main = *new(alignof(Parser), _r.page) Parser(*String::from_c_string(_r.page, "function main('argument count': int, 'argument values': pointer[pointer[byte]]) returns int"));
    auto start = parser_main.lexer.previous_position;

    auto success_function = parser_main.lexer.parse_keyword(_r, _r.page, *String::from_c_string(_r.page, "function"));
    if (!success_function)
        return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Ok, ._Ok = nullptr };

    auto name = parser_main.lexer.parse_identifier(_r, _r.page, parser_main.keywords);
    if (name != nullptr) {
        if (!parser_main.is_identifier(*name)) {
            return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };
        }
    }
    else
        return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };


    auto parameters_result = parser_main.parse_parameterset(_r, _rp, _ep);
    if (parameters_result._tag == Result<ParameterSetSyntax*, ParserError>::Error)
        return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = parameters_result._Error };
    auto parameters = parameters_result._Ok;

    auto returns_result = parser_main.parse_returns(_r, _rp, _ep);
    if (returns_result._tag == Result<ReturnsSyntax*, ParserError>::Error)
        return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = returns_result._Error };
    auto returns = returns_result._Ok;

    auto end = parser_main.lexer.position;

    // Parse the statements of the program and put them into the function implementation
    Array<StatementSyntax>& statements = *Array<StatementSyntax>::create(_r.page);
    while(true) {
        auto node_result = parser.parse_statement(_r, _rp, _ep);
        if ((node_result._tag == Result<StatementSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
            return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = node_result._Error };
        if (node_result._tag == Result<StatementSyntax, ParserError>::Ok) {
            auto node = node_result._Ok;
            statements.add(node);
        } else {
            if ((statements.length == 0) && (node_result._tag == Result<StatementSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
            return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            break;
        }
    }
    auto block = new(alignof(BlockSyntax), _rp) BlockSyntax(start, end, Vector<StatementSyntax>::from_array(_rp, statements));
    auto block_expression = new (alignof(ExpressionSyntax), _rp) ExpressionSyntax(BlockSyntax(*block));
    auto operand = new(alignof(OperandSyntax), _rp) OperandSyntax(start, end, *block_expression, nullptr);
    Array<OperandSyntax>& operands_array = *Array<OperandSyntax>::create(_r.page);;
    operands_array.add(*operand);
    auto operation = OperationSyntax(start, end, Vector<OperandSyntax>::from_array(_rp, operands_array));
    auto action = ActionSyntax(OperationSyntax(operation));
    auto implementation = ImplementationSyntax(ActionSyntax(action));

    auto routine = RoutineSyntax(start, end, new (alignof(ParameterSetSyntax), _rp) ParameterSetSyntax(parameters), nullptr, new (alignof(ReturnsSyntax), _rp) ReturnsSyntax(returns), nullptr, implementation);
    FunctionSyntax& main_function_syntax = *new(alignof(FunctionSyntax), _rp) FunctionSyntax(start, end, *name, nullptr, routine);
    main_function_syntax.routine.implementation = implementation;
    DeclarationSyntax& main_function_declaration = *new (alignof(DeclarationSyntax), _rp) DeclarationSyntax(FunctionSyntax(main_function_syntax));;
    declarations.add(main_function_declaration);
    return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Ok, ._Ok = Vector<DeclarationSyntax>::from_array(_rp, declarations) };
}

Result<Vector<Property>, ModelError> handle_parameterset(Region& _pr, Page* _rp, Page* _ep, ParameterSetSyntax& parameterSetSyntax) {
    auto _r = Region::create(_pr);
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
    return Result<Vector<Property>, ModelError> { ._tag = Result<Vector<Property>, ModelError>::Ok, ._Ok = *Vector<Property>::from_array(_rp, parameters) };
}

Result<Function, ModelError> handle_function(Region& _pr, Page* _rp, Page* _ep, FunctionSyntax& function_syntax, MultiMapBuilder<String, Function>& functions_builder) {
    auto _r = Region::create(_pr);
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

    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(0, function_syntax.start, function_syntax.end), *function_syntax.name.copy(_rp), input, output, operation) };
}

Result<Model, ModelError> build_model(Region& _pr, Page* _rp, Page* _ep, Vector<DeclarationSyntax>& declarations) {
    auto _r = Region::create(_pr);

    MultiMapBuilder<String, Function>& functions_builder = *new(alignof(MultiMapBuilder<String, Array<Function>>), _r.page) MultiMapBuilder<String, Function>();

    auto declarations_iterator = VectorIterator<DeclarationSyntax>::create(declarations);
    while (auto declaration = declarations_iterator.next()) {
        switch (declaration->_tag)
        {
            case DeclarationSyntax::Private:
            break;
            case DeclarationSyntax::Definition:
            break;
            case DeclarationSyntax::Function: {
                auto _r_1 = Region::create(_r);
                auto function_result = handle_function(_r_1, _rp, _ep, declaration->_Function, functions_builder);
                if (function_result._tag == Result<Function, ModelError>::Error)
                    return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = function_result._Error };
                auto function = function_result._Ok;
                functions_builder.add(function.name, function);
            }
            break;
            case DeclarationSyntax::Procedure:
            break;
            case DeclarationSyntax::Operator:
            break;
            case DeclarationSyntax::Use:
            break;
            case DeclarationSyntax::Implement:
            break;
            case DeclarationSyntax::Trait:
            break;
            case DeclarationSyntax::Macro:
            break;
            case DeclarationSyntax::Module:
            break;
        }
    }

    //auto function_builder_iterator = functions_builder.
    //auto ret = new(alignof(Model), _rp) Model(functions);
    return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = Model(*MultiMap<String, Function>::from_multi_map_builder(_r, _rp, functions_builder)) };
}

Result<Model, ModelError> build_program_model(Region& _pr, Page* _rp, Page* _ep, String& program) {
    auto _r = Region::create(_pr);

    auto declarations_result = parse_program(_r, _r.page, _ep, program);
    if (declarations_result._tag == Result<Vector<DeclarationSyntax>*, ParserError>::Error)
        return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = ModelError(declarations_result._Error) };
    auto declarations = declarations_result._Ok;

    auto model_result = build_model(_r, _r.page, _ep, *declarations);
    if (model_result._tag == Result<Model*, ModelError*>::Error)
        return model_result;
    auto model = model_result._Ok;

    //auto functions = model->functions;

    return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = model };
}

}
}
}