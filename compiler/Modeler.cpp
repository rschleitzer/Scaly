namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct IoModelError {
    String file_name;
};

struct NotImplementedModelError
{
    NotImplementedModelError(Span _Span) : span(_Span) {}
    Span span;
};

struct DuplicateNameError {
    DuplicateNameError(Span span) : span(span) {}
    Span span;
};

struct ModelBuilderError {
    ModelBuilderError(NotImplementedModelError _NotImplementedModelError) : _tag(NotImplemented), _NotImplemented(_NotImplementedModelError) {}
    ModelBuilderError(DuplicateNameError _DuplicateNameError) : _tag(DuplicateName), _DuplicateName(_DuplicateNameError) {}
    enum {
        NotImplemented,
        DuplicateName,
    } _tag;
    union {
        NotImplementedModelError _NotImplemented;
        DuplicateNameError _DuplicateName;
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

Result<Vector<DeclarationSyntax>*, ParserError> parse_program(Region& _pr, Page* _rp, Page* _ep, const String& program) {
    auto _r = Region::create(_pr);
    Array<DeclarationSyntax>& declarations = *new(alignof(Array<DeclarationSyntax>), _r.page) Array<DeclarationSyntax>();
    
    // Parse the scaly module inclusion
    {
        auto _r_1 = Region::create(_r);
        Parser& parser_module = *new(alignof(Parser), _r_1.page) Parser(_r_1, _r_1.page, String(_r_1.page, "module scaly"));
        auto module_syntax_result = parser_module.parse_module(_r_1, _rp, _ep);
        if (module_syntax_result._tag == Result<ModuleSyntax*, ParserError>::Error)
            return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = module_syntax_result._Error };
        auto module_syntax = module_syntax_result._Ok;
        auto module_declaration = DeclarationSyntax(ModuleSyntax(module_syntax));
        declarations.add(module_declaration);
    }

    // Parse the uses and declarations of the program
    Vector<UseSyntax>* uses = nullptr;
    Vector<StatementSyntax>* statements = nullptr;
    {
        auto _r_1 = Region::create(_r);
        Parser& parser = *new(alignof(Parser), _r_1.page) Parser(_r_1, _r_1.page, program);
        while(true) {
            auto node_result = parser.parse_declaration(_r_1, _rp, _ep);
            if ((node_result._tag == Result<DeclarationSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = node_result._Error };
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
    return Result<Vector<DeclarationSyntax>*, ParserError> { 
        ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Ok, 
        ._Ok = new(alignof(Vector<DeclarationSyntax>), _rp) Vector<DeclarationSyntax>(_rp, declarations)
    };
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
    return Result<Vector<Property>, ModelError> {
        ._tag = Result<Vector<Property>, ModelError>::Ok, 
        ._Ok = Vector<Property>(_rp, parameters) };
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

    return Result<Function, ModelError> { ._tag = Result<Function, ModelError>::Ok, ._Ok = Function(Span(function_syntax.start, function_syntax.end), String(_rp, function_syntax.name), input, output, operation) };
}

Result<Concept, ModelError> handle_module(Region& _pr, Page* _rp, Page* _ep, ModuleSyntax& module_syntax) {
    auto _r = Region::create(_pr);
    auto file_name_builder = new(alignof(StringBuilder), _r.page) StringBuilder(); 
    return Result<Concept, ModelError> { ._tag = Result<Concept, ModelError>::Ok, ._Ok = Concept(Span(module_syntax.start, module_syntax.end), Type(String(_rp, module_syntax.name.name)))  };
}

Result<Model, ModelError> build_model(Region& _pr, Page* _rp, Page* _ep, Vector<DeclarationSyntax>& declarations) {
    auto _r = Region::create(_pr);

    HashMapBuilder<String, Concept>& concepts_builder = *new(alignof(HashMapBuilder<String, Array<Concept>>), _r.page) HashMapBuilder<String, Concept>();
    MultiMapBuilder<String, Function>& functions_builder = *new(alignof(MultiMapBuilder<String, Array<Function>>), _r.page) MultiMapBuilder<String, Function>();

    auto declarations_iterator = VectorIterator<DeclarationSyntax>(declarations);
    while (auto declaration = declarations_iterator.next()) {
        auto _r_1 = Region::create(_r);
        switch (declaration->_tag)
        {
            case DeclarationSyntax::Private:
            break;
            case DeclarationSyntax::Definition:
            break;
            case DeclarationSyntax::Function: {
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
            case DeclarationSyntax::Implement:
            break;
            case DeclarationSyntax::Trait:
            break;
            case DeclarationSyntax::Macro:
            break;
            case DeclarationSyntax::Module:
                auto module_syntax = declaration->_Module;
                auto concept_result = handle_module(_r, _rp, _ep, module_syntax);
                if (concept_result._tag == Result<Concept, ModelError>::Error)
                    return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = concept_result._Error };
                auto concept = concept_result._Ok;
                if (!concepts_builder.add(concept.type.name, concept))
                    return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Error, ._Error = ModelError(ModelBuilderError(DuplicateNameError(Span(module_syntax.start, module_syntax.end)))) };
            break;
        }
    }
    return Result<Model, ModelError> { ._tag = Result<Model, ModelError>::Ok, ._Ok = Model(HashMap<String, Concept>(_r, _rp, concepts_builder), MultiMap<String, Function>(_r, _rp, functions_builder)) };
}

Result<Model, ModelError> build_program_model(Region& _pr, Page* _rp, Page* _ep, const String& program) {
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