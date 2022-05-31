namespace scaly {
namespace compiler {
namespace model {

struct Span : Object {
    size_t file_id;
    size_t start;
    size_t end;
    Span(size_t file_id, size_t start, size_t end) : file_id(file_id), start(start), end(end) {}
};

struct Expression;

struct Parameter {
    String name;
};


struct NameExpression : Object {
    String path;
    NameExpression(String path) : path(path) {}
};

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

struct Routine : Object {
    Span span;
    Vector<Parameter>* input;
    Vector<Parameter>* result;
    Operation* operation;
    Routine(Span span, Vector<Parameter>* input, Vector<Parameter>* result, Operation* operation) : span(span), input(input), result(result), operation(operation) {}
};

struct Function : Object {
    Span span;
    String name;
    Routine routine;
    Function(Span span, String name, Routine routine) : span(span), name(name), routine(routine) {}
};

struct Model : Object {
    HashMap<String, Vector<Function>> functions;
    Model(HashMap<String, Vector<Function>> functions) : functions(functions) {};
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

Result<Vector<DeclarationSyntax>*, ParserError*> parse_program(Region& _pr, Page* _rp, Page* _ep, String& program) {
    auto _r = Region::create(_pr);
    Array<DeclarationSyntax>* declarations = Array<DeclarationSyntax>::create(_r.page);
    
    // Parse the scaly module inclusion
    Parser& parser_module = *new(alignof(Parser), _r.page) Parser(*String::from_c_string(_r.page, "module scaly"));
    auto module_syntax_result = parser_module.parse_module(_r, _rp, _ep);
    if (module_syntax_result.tag == Result<ModuleSyntax*, ParserError*>::Error)
        return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = module_syntax_result.error };
    auto module_syntax = module_syntax_result.ok;
    DeclarationSyntax* module_declaration = nullptr;
    if (module_syntax != nullptr)
        module_declaration =  new (alignof(DeclarationSyntax), _rp) DeclarationSyntax(ModuleSyntax(*module_syntax));
    declarations->add(*module_declaration);

    // Parse the declarations of the program
    Parser& parser = *new(alignof(Parser), _r.page) Parser(program);
    while(true) {
        auto node_result = parser.parse_declaration(_r, _rp, _ep);
        if (node_result.tag == Result<DeclarationSyntax*, ParserError*>::Error)
            return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = node_result.error };
        auto node = node_result.ok;
        if (node != nullptr) {
            declarations->add(*node);
        } else {
            break;
        }
    }

    // Parse the main function stub
    Parser& parser_main = *new(alignof(Parser), _r.page) Parser(*String::from_c_string(_r.page, "function main('argument count': int, 'argument values': pointer[pointer[byte]]) returns int"));
    auto start = parser_main.lexer.previous_position;

    auto success_function = parser_main.lexer.parse_keyword(_r, _r.page, *String::from_c_string(_r.page, "function"));
    if (!success_function)
        return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Ok, .ok = nullptr };

    auto name = parser_main.lexer.parse_identifier(_r, _r.page, parser_main.keywords);
    if (name != nullptr) {
        if (!parser_main.is_identifier(*name)) {
            return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };
        }
    }
    else
        return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };


    auto parameters_result = parser_main.parse_parameterset(_r, _rp, _ep);
    if (parameters_result.tag == Result<ParameterSetSyntax*, ParserError*>::Error)
        return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = parameters_result.error };
    auto parameters = parameters_result.ok;

    auto returns_result = parser_main.parse_returns(_r, _rp, _ep);
    if (returns_result.tag == Result<ReturnsSyntax*, ParserError*>::Error)
        return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = returns_result.error };
    auto returns = returns_result.ok;

    auto end = parser_main.lexer.position;

    // Parse the statements of the program and put them into the function implementation
    Array<StatementSyntax>* statements = Array<StatementSyntax>::create(_r.page);
    while(true) {
        auto node_result = parser.parse_statement(_r, _rp, _ep);
        if (node_result.tag == Result<StatementSyntax*, ParserError*>::Error)
            return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = node_result.error };
        auto node = node_result.ok;
        if (node != nullptr) {
            statements->add(*node);
        } else {
            break;
        }
    }
    auto block = new(alignof(BlockSyntax), _rp) BlockSyntax(start, end, Vector<StatementSyntax>::from_array(_rp, *statements));
    auto block_expression = new (alignof(ExpressionSyntax), _rp) ExpressionSyntax(BlockSyntax(*block));
    auto operand = new(alignof(OperandSyntax), _rp) OperandSyntax(start, end, *block_expression, nullptr);
    Array<OperandSyntax>* operands_array = Array<OperandSyntax>::create(_r.page);;
    operands_array->add(*operand);
    auto operation = new(alignof(OperationSyntax), _rp) OperationSyntax(start, end, *Vector<OperandSyntax>::from_array(_rp, *operands_array));
    auto action = new (alignof(ActionSyntax), _rp) ActionSyntax(OperationSyntax(*operation));
    auto implementation = new (alignof(ImplementationSyntax), _rp) ImplementationSyntax(ActionSyntax(*action));

    auto routine = new(alignof(RoutineSyntax), _rp) RoutineSyntax(start, end, parameters, nullptr, returns, nullptr, *implementation);
    if (routine == nullptr)
        return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError(InvalidSyntaxParserError(start, parser_main.lexer.position)) };

    auto main_function_syntax = new(alignof(FunctionSyntax), _rp) FunctionSyntax(start, end, *name, nullptr, *routine);
    DeclarationSyntax* main_function_declaration = nullptr;
    if (main_function_syntax != nullptr)
    {
        main_function_syntax->routine.implementation = *implementation;
        main_function_declaration =  new (alignof(DeclarationSyntax), _rp) DeclarationSyntax(FunctionSyntax(*main_function_syntax));
        declarations->add(*main_function_declaration);
    }
    return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Ok, .ok = Vector<DeclarationSyntax>::from_array(_rp, *declarations) };
}

Result<Function*, ModelError*> build_function(Region& _pr, Page* _rp, Page* _ep, FunctionSyntax& function_syntax, MultiMapBuilder<String, Array<Function>>& functions_builder) {
    // auto function = new(alignof(Function), _rp) Function(Span(0, 0, 0), function_syntax.name, Routine(Span(0,0,0), ));
    // functions_builder.add(*function_syntax.name, *function);
    return Result<Function*, ModelError*> { .tag = Result<Function*, ModelError*>::Error, .error = nullptr };
}

Result<Model*, ModelError*> build_model(Region& _pr, Page* _rp, Page* _ep, Vector<DeclarationSyntax>& declarations) {
    auto _r = Region::create(_pr);

    MultiMapBuilder<String, Array<Function>>& functions_builder = *new(alignof(MultiMapBuilder<String, Array<Function>>), _r.page) MultiMapBuilder<String, Array<Function>>();

    auto declarations_iterator = VectorIterator<DeclarationSyntax>::create(declarations);
    while (auto declaration = declarations_iterator.next()) {
        switch (declaration->tag)
        {
            case DeclarationSyntax::Private:
            break;
            case DeclarationSyntax::Definition:
            break;
            case DeclarationSyntax::Function:
            {
                auto _r_1 = Region::create(_r);
                auto function_result = build_function(_r_1, _rp, _ep, declaration->functionSyntax, functions_builder);
                // if (function_result.tag == Result<Vector<DeclarationSyntax>*, ParserError*>::Error)
                //     return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Error, .error = new(alignof(ModelError), _ep) ModelError(*function_result.error) };
                auto function = function_result.ok;
                Array<Function>* function_array = nullptr;
                if (!functions_builder.contains(function->name))
                {
                    function_array = Array<Function>::create(_r.page);
                    //functions_builder.add(function->name, *function_array);
                }
                else
                {
                    //function_array = functions_builder[function->name];
                }
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
    return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Ok, .ok = nullptr };
}

Result<Model*, ModelError*> build_program_model(Region& _pr, Page* _rp, Page* _ep, String& program) {
    auto _r = Region::create(_pr);

    auto declarations_result = parse_program(_r, _r.page, _ep, program);
    if (declarations_result.tag == Result<Vector<DeclarationSyntax>*, ParserError*>::Error)
        return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Error, .error = new(alignof(ModelError), _ep) ModelError(*declarations_result.error) };
    auto declarations = declarations_result.ok;

    auto model_result = build_model(_r, _r.page, _ep, *declarations);
    if (model_result.tag == Result<Model*, ModelError*>::Error)
        return model_result;
    auto model = model_result.ok;

    //auto functions = model->functions;

    return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Ok, .ok = model };
}

}
}
}