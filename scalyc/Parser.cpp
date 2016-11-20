#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Parser::Parser(string* theFileName, string* text) {
    lexer = new(getPage()->allocateExclusivePage()) Lexer(text);
    fileName = theFileName;
    useKeyword = new(getPage()) string("use");
    classKeyword = new(getPage()) string("class");
    functionKeyword = new(getPage()) string("function");
    ifKeyword = new(getPage()) string("if");
    elseKeyword = new(getPage()) string("else");
    switchKeyword = new(getPage()) string("switch");
    caseKeyword = new(getPage()) string("case");
    defaultKeyword = new(getPage()) string("default");
    catchKeyword = new(getPage()) string("catch");
    forKeyword = new(getPage()) string("for");
    inKeyword = new(getPage()) string("in");
    whileKeyword = new(getPage()) string("while");
    repeatKeyword = new(getPage()) string("repeat");
    returnKeyword = new(getPage()) string("return");
    throwKeyword = new(getPage()) string("throw");
    breakKeyword = new(getPage()) string("break");
    throwsKeyword = new(getPage()) string("throws");
    existingKeyword = new(getPage()) string("existing");
    overrideKeyword = new(getPage()) string("override");
    staticKeyword = new(getPage()) string("static");
    letKeyword = new(getPage()) string("let");
    varKeyword = new(getPage()) string("var");
    mutableKeyword = new(getPage()) string("mutable");
    isKeyword = new(getPage()) string("is");
    asKeyword = new(getPage()) string("as");
    constructorKeyword = new(getPage()) string("constructor");
    enumKeyword = new(getPage()) string("enum");
    superKeyword = new(getPage()) string("super");
    thisKeyword = new(getPage()) string("this");
    nullKeyword = new(getPage()) string("null");
    newKeyword = new(getPage()) string("new");
    extendsKeyword = new(getPage()) string("extends");
    equal = new(getPage()) string("=");
    leftAngular = new(getPage()) string("<");
    rightAngular = new(getPage()) string(">");
    comma = new(getPage()) string(",");
    leftParen = new(getPage()) string("(");
    rightParen = new(getPage()) string(")");
    leftCurly = new(getPage()) string("{");
    rightCurly = new(getPage()) string("}");
    leftBracket = new(getPage()) string("[");
    rightBracket = new(getPage()) string("]");
    colon = new(getPage()) string(":");
    dot = new(getPage()) string(".");
    questionMark = new(getPage()) string("?");
    underscore = new(getPage()) string("_");
}

_Result<CompilationUnit, ParserError> Parser::parseCompilationUnit(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _statements_result = parseStatementList(_rp, _ep);
    _Vector<Statement>* statements = nullptr;
    if (_statements_result.succeeded()) {
        statements = _statements_result.getResult();
    }
    else {
        statements = nullptr;
    }
    if (statements != nullptr) {
        if (!isAtEnd()) {
            _Region _region; _Page* _p = _region.get();
            Position* current = lexer->getPosition(_p);
            return _Result<CompilationUnit, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_notAtEnd(new(_ep) Position(current))));
        }
    }
    Position* end = lexer->getPosition(_p);
    CompilationUnit* ret = new(_rp) CompilationUnit(statements, new(_rp) Position(start), new(_rp) Position(end));
    ret->fileName = fileName;
    if (statements != nullptr) {
        Statement* item = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            item = *(*statements)[_i];
            item->parent = ret;
        }
    }
    return _Result<CompilationUnit, ParserError>(ret);
}

_Result<_Vector<Statement>, ParserError> Parser::parseStatementList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Statement>* ret = nullptr;
    while (true) {
        auto _node_result = parseStatement(_rp, _ep);
        Statement* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<Statement>();
        ret->push(node);
    }
    return _Result<_Vector<Statement>, ParserError>(ret ? &_Vector<Statement>::create(_rp, *ret) : 0);
}

_Result<Statement, ParserError> Parser::parseStatement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseDeclaration(_rp, _ep);
        Declaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Statement, ParserError>(node);
    }
    {
        auto _node_result = parseExpression(_rp, _ep);
        Expression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Statement, ParserError>(node);
    }
    return _Result<Statement, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<Declaration, ParserError> Parser::parseDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseConstantDeclaration(_rp, _ep);
        ConstantDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    {
        auto _node_result = parseVariableDeclaration(_rp, _ep);
        VariableDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    {
        auto _node_result = parseMutableDeclaration(_rp, _ep);
        MutableDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    {
        auto _node_result = parseFunctionDeclaration(_rp, _ep);
        FunctionDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    {
        auto _node_result = parseEnumDeclaration(_rp, _ep);
        EnumDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    {
        auto _node_result = parseClassDeclaration(_rp, _ep);
        ClassDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    {
        auto _node_result = parseConstructorDeclaration(_rp, _ep);
        ConstructorDeclaration* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Declaration, ParserError>(node);
    }
    return _Result<Declaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<Expression, ParserError> Parser::parseExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseCodeBlock(_rp, _ep);
        CodeBlock* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Expression, ParserError>(node);
    }
    {
        auto _node_result = parseSimpleExpression(_rp, _ep);
        SimpleExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Expression, ParserError>(node);
    }
    return _Result<Expression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<ConstantDeclaration, ParserError> Parser::parseConstantDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLet1 = lexer->getPreviousPosition(_p);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1)
        lexer->advance();
    else
        return _Result<ConstantDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startLet1), new(_ep) string(letKeyword))));
    auto _initializer_result = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer = nullptr;
    if (_initializer_result.succeeded()) {
        initializer = _initializer_result.getResult();
    }
    else {
        auto error = _initializer_result.getError();
        return _Result<ConstantDeclaration, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ConstantDeclaration* ret = new(_rp) ConstantDeclaration(initializer, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    return _Result<ConstantDeclaration, ParserError>(ret);
}

_Result<VariableDeclaration, ParserError> Parser::parseVariableDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startVar1 = lexer->getPreviousPosition(_p);
    bool successVar1 = lexer->parseKeyword(varKeyword);
    if (successVar1)
        lexer->advance();
    else
        return _Result<VariableDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startVar1), new(_ep) string(varKeyword))));
    auto _initializer_result = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer = nullptr;
    if (_initializer_result.succeeded()) {
        initializer = _initializer_result.getResult();
    }
    else {
        auto error = _initializer_result.getError();
        return _Result<VariableDeclaration, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    VariableDeclaration* ret = new(_rp) VariableDeclaration(initializer, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    return _Result<VariableDeclaration, ParserError>(ret);
}

_Result<MutableDeclaration, ParserError> Parser::parseMutableDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startMutable1 = lexer->getPreviousPosition(_p);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1)
        lexer->advance();
    else
        return _Result<MutableDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startMutable1), new(_ep) string(mutableKeyword))));
    auto _initializer_result = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer = nullptr;
    if (_initializer_result.succeeded()) {
        initializer = _initializer_result.getResult();
    }
    else {
        auto error = _initializer_result.getError();
        return _Result<MutableDeclaration, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    MutableDeclaration* ret = new(_rp) MutableDeclaration(initializer, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    return _Result<MutableDeclaration, ParserError>(ret);
}

_Result<FunctionDeclaration, ParserError> Parser::parseFunctionDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _modifiers_result = parseModifierList(_rp, _ep);
    _Vector<Modifier>* modifiers = nullptr;
    if (_modifiers_result.succeeded()) {
        modifiers = _modifiers_result.getResult();
    }
    else {
        modifiers = nullptr;
    }
    Position* startFunction2 = lexer->getPreviousPosition(_p);
    bool successFunction2 = lexer->parseKeyword(functionKeyword);
    if (successFunction2)
        lexer->advance();
    else
        return _Result<FunctionDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startFunction2), new(_ep) string(functionKeyword))));
    auto _name_result = parseFunctionName(_rp, _ep);
    FunctionName* name = nullptr;
    if (_name_result.succeeded()) {
        name = _name_result.getResult();
    }
    else {
        auto error = _name_result.getError();
        return _Result<FunctionDeclaration, ParserError>(error);
    }
    auto _signature_result = parseFunctionSignature(_rp, _ep);
    FunctionSignature* signature = nullptr;
    if (_signature_result.succeeded()) {
        signature = _signature_result.getResult();
    }
    else {
        auto error = _signature_result.getError();
        return _Result<FunctionDeclaration, ParserError>(error);
    }
    auto _body_result = parseExpression(_rp, _ep);
    Expression* body = nullptr;
    if (_body_result.succeeded()) {
        body = _body_result.getResult();
    }
    else {
        body = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    FunctionDeclaration* ret = new(_rp) FunctionDeclaration(modifiers, name, signature, body, new(_rp) Position(start), new(_rp) Position(end));
    if (modifiers != nullptr) {
        Modifier* item = nullptr;
        size_t _modifiers_length = modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++) {
            item = *(*modifiers)[_i];
            item->parent = ret;
        }
    }
    name->parent = ret;
    signature->parent = ret;
    if (body != nullptr)
        body->parent = ret;
    return _Result<FunctionDeclaration, ParserError>(ret);
}

_Result<EnumDeclaration, ParserError> Parser::parseEnumDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEnum1 = lexer->getPreviousPosition(_p);
    bool successEnum1 = lexer->parseKeyword(enumKeyword);
    if (successEnum1)
        lexer->advance();
    else
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startEnum1), new(_ep) string(enumKeyword))));
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* startLeftCurly3 = lexer->getPreviousPosition(_p);
    bool successLeftCurly3 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly3)
        lexer->advance();
    else
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly3), new(_ep) string(leftCurly))));
    auto _members_result = parseEnumMemberList(_rp, _ep);
    _Vector<EnumMember>* members = nullptr;
    if (_members_result.succeeded()) {
        members = _members_result.getResult();
    }
    else {
        auto error = _members_result.getError();
        return _Result<EnumDeclaration, ParserError>(error);
    }
    Position* startRightCurly5 = lexer->getPreviousPosition(_p);
    bool successRightCurly5 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly5)
        lexer->advance();
    else
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly5), new(_ep) string(rightCurly))));
    Position* end = lexer->getPosition(_p);
    EnumDeclaration* ret = new(_rp) EnumDeclaration(name, members, new(_rp) Position(start), new(_rp) Position(end));
    if (members != nullptr) {
        EnumMember* item = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            item = *(*members)[_i];
            item->parent = ret;
        }
    }
    return _Result<EnumDeclaration, ParserError>(ret);
}

_Result<ClassDeclaration, ParserError> Parser::parseClassDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startClass1 = lexer->getPreviousPosition(_p);
    bool successClass1 = lexer->parseKeyword(classKeyword);
    if (successClass1)
        lexer->advance();
    else
        return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startClass1), new(_ep) string(classKeyword))));
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    auto _genericArgumentClause_result = parseGenericArgumentClause(_rp, _ep);
    GenericArgumentClause* genericArgumentClause = nullptr;
    if (_genericArgumentClause_result.succeeded()) {
        genericArgumentClause = _genericArgumentClause_result.getResult();
    }
    else {
        genericArgumentClause = nullptr;
    }
    auto _typeInheritanceClause_result = parseTypeInheritanceClause(_rp, _ep);
    TypeInheritanceClause* typeInheritanceClause = nullptr;
    if (_typeInheritanceClause_result.succeeded()) {
        typeInheritanceClause = _typeInheritanceClause_result.getResult();
    }
    else {
        typeInheritanceClause = nullptr;
    }
    auto _body_result = parseClassBody(_rp, _ep);
    ClassBody* body = nullptr;
    if (_body_result.succeeded()) {
        body = _body_result.getResult();
    }
    else {
        body = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    ClassDeclaration* ret = new(_rp) ClassDeclaration(name, genericArgumentClause, typeInheritanceClause, body, new(_rp) Position(start), new(_rp) Position(end));
    if (genericArgumentClause != nullptr)
        genericArgumentClause->parent = ret;
    if (typeInheritanceClause != nullptr)
        typeInheritanceClause->parent = ret;
    if (body != nullptr)
        body->parent = ret;
    return _Result<ClassDeclaration, ParserError>(ret);
}

_Result<ConstructorDeclaration, ParserError> Parser::parseConstructorDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _modifiers_result = parseModifierList(_rp, _ep);
    _Vector<Modifier>* modifiers = nullptr;
    if (_modifiers_result.succeeded()) {
        modifiers = _modifiers_result.getResult();
    }
    else {
        modifiers = nullptr;
    }
    Position* startConstructor2 = lexer->getPreviousPosition(_p);
    bool successConstructor2 = lexer->parseKeyword(constructorKeyword);
    if (successConstructor2)
        lexer->advance();
    else
        return _Result<ConstructorDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startConstructor2), new(_ep) string(constructorKeyword))));
    auto _parameterClause_result = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause = nullptr;
    if (_parameterClause_result.succeeded()) {
        parameterClause = _parameterClause_result.getResult();
    }
    else {
        auto error = _parameterClause_result.getError();
        return _Result<ConstructorDeclaration, ParserError>(error);
    }
    auto _throwsClause_result = parseThrowsClause(_rp, _ep);
    ThrowsClause* throwsClause = nullptr;
    if (_throwsClause_result.succeeded()) {
        throwsClause = _throwsClause_result.getResult();
    }
    else {
        throwsClause = nullptr;
    }
    auto _body_result = parseExpression(_rp, _ep);
    Expression* body = nullptr;
    if (_body_result.succeeded()) {
        body = _body_result.getResult();
    }
    else {
        auto error = _body_result.getError();
        return _Result<ConstructorDeclaration, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ConstructorDeclaration* ret = new(_rp) ConstructorDeclaration(modifiers, parameterClause, throwsClause, body, new(_rp) Position(start), new(_rp) Position(end));
    if (modifiers != nullptr) {
        Modifier* item = nullptr;
        size_t _modifiers_length = modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++) {
            item = *(*modifiers)[_i];
            item->parent = ret;
        }
    }
    parameterClause->parent = ret;
    if (throwsClause != nullptr)
        throwsClause->parent = ret;
    body->parent = ret;
    return _Result<ConstructorDeclaration, ParserError>(ret);
}

_Result<CodeBlock, ParserError> Parser::parseCodeBlock(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1)
        lexer->advance();
    else
        return _Result<CodeBlock, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly1), new(_ep) string(leftCurly))));
    auto _statements_result = parseStatementList(_rp, _ep);
    _Vector<Statement>* statements = nullptr;
    if (_statements_result.succeeded()) {
        statements = _statements_result.getResult();
    }
    else {
        auto error = _statements_result.getError();
        return _Result<CodeBlock, ParserError>(error);
    }
    Position* startRightCurly3 = lexer->getPreviousPosition(_p);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3)
        lexer->advance();
    else
        return _Result<CodeBlock, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly3), new(_ep) string(rightCurly))));
    Position* end = lexer->getPosition(_p);
    CodeBlock* ret = new(_rp) CodeBlock(statements, new(_rp) Position(start), new(_rp) Position(end));
    if (statements != nullptr) {
        Statement* item = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            item = *(*statements)[_i];
            item->parent = ret;
        }
    }
    return _Result<CodeBlock, ParserError>(ret);
}

_Result<SimpleExpression, ParserError> Parser::parseSimpleExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _prefixExpression_result = parsePrefixExpression(_rp, _ep);
    PrefixExpression* prefixExpression = nullptr;
    if (_prefixExpression_result.succeeded()) {
        prefixExpression = _prefixExpression_result.getResult();
    }
    else {
        auto error = _prefixExpression_result.getError();
        return _Result<SimpleExpression, ParserError>(error);
    }
    auto _binaryOps_result = parseBinaryOpList(_rp, _ep);
    _Vector<BinaryOp>* binaryOps = nullptr;
    if (_binaryOps_result.succeeded()) {
        binaryOps = _binaryOps_result.getResult();
    }
    else {
        binaryOps = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    SimpleExpression* ret = new(_rp) SimpleExpression(prefixExpression, binaryOps, new(_rp) Position(start), new(_rp) Position(end));
    prefixExpression->parent = ret;
    if (binaryOps != nullptr) {
        BinaryOp* item = nullptr;
        size_t _binaryOps_length = binaryOps->length();
        for (size_t _i = 0; _i < _binaryOps_length; _i++) {
            item = *(*binaryOps)[_i];
            item->parent = ret;
        }
    }
    return _Result<SimpleExpression, ParserError>(ret);
}

_Result<Initializer, ParserError> Parser::parseInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEqual1 = lexer->getPreviousPosition(_p);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1)
        lexer->advance();
    else
        return _Result<Initializer, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startEqual1), new(_ep) string(equal))));
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<Initializer, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    Initializer* ret = new(_rp) Initializer(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return _Result<Initializer, ParserError>(ret);
}

_Result<BindingInitializer, ParserError> Parser::parseBindingInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _initializer_result = parsePatternInitializer(_rp, _ep);
    PatternInitializer* initializer = nullptr;
    if (_initializer_result.succeeded()) {
        initializer = _initializer_result.getResult();
    }
    else {
        auto error = _initializer_result.getError();
        return _Result<BindingInitializer, ParserError>(error);
    }
    auto _additionalInitializers_result = parseAdditionalInitializerList(_rp, _ep);
    _Vector<AdditionalInitializer>* additionalInitializers = nullptr;
    if (_additionalInitializers_result.succeeded()) {
        additionalInitializers = _additionalInitializers_result.getResult();
    }
    else {
        additionalInitializers = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    BindingInitializer* ret = new(_rp) BindingInitializer(initializer, additionalInitializers, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    if (additionalInitializers != nullptr) {
        AdditionalInitializer* item = nullptr;
        size_t _additionalInitializers_length = additionalInitializers->length();
        for (size_t _i = 0; _i < _additionalInitializers_length; _i++) {
            item = *(*additionalInitializers)[_i];
            item->parent = ret;
        }
    }
    return _Result<BindingInitializer, ParserError>(ret);
}

_Result<_Vector<PatternInitializer>, ParserError> Parser::parsePatternInitializerList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<PatternInitializer>* ret = nullptr;
    while (true) {
        auto _node_result = parsePatternInitializer(_rp, _ep);
        PatternInitializer* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<PatternInitializer>();
        ret->push(node);
    }
    return _Result<_Vector<PatternInitializer>, ParserError>(ret ? &_Vector<PatternInitializer>::create(_rp, *ret) : 0);
}

_Result<PatternInitializer, ParserError> Parser::parsePatternInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<PatternInitializer, ParserError>(error);
    }
    auto _initializer_result = parseInitializer(_rp, _ep);
    Initializer* initializer = nullptr;
    if (_initializer_result.succeeded()) {
        initializer = _initializer_result.getResult();
    }
    else {
        initializer = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    PatternInitializer* ret = new(_rp) PatternInitializer(pattern, initializer, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    if (initializer != nullptr)
        initializer->parent = ret;
    return _Result<PatternInitializer, ParserError>(ret);
}

_Result<_Vector<AdditionalInitializer>, ParserError> Parser::parseAdditionalInitializerList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<AdditionalInitializer>* ret = nullptr;
    while (true) {
        auto _node_result = parseAdditionalInitializer(_rp, _ep);
        AdditionalInitializer* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<AdditionalInitializer>();
        ret->push(node);
    }
    return _Result<_Vector<AdditionalInitializer>, ParserError>(ret ? &_Vector<AdditionalInitializer>::create(_rp, *ret) : 0);
}

_Result<AdditionalInitializer, ParserError> Parser::parseAdditionalInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startComma1 = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1)
        lexer->advance();
    else
        return _Result<AdditionalInitializer, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startComma1), new(_ep) string(comma))));
    auto _pattern_result = parsePatternInitializer(_rp, _ep);
    PatternInitializer* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<AdditionalInitializer, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    AdditionalInitializer* ret = new(_rp) AdditionalInitializer(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return _Result<AdditionalInitializer, ParserError>(ret);
}

_Result<_Vector<Modifier>, ParserError> Parser::parseModifierList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Modifier>* ret = nullptr;
    while (true) {
        auto _node_result = parseModifier(_rp, _ep);
        Modifier* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<Modifier>();
        ret->push(node);
    }
    return _Result<_Vector<Modifier>, ParserError>(ret ? &_Vector<Modifier>::create(_rp, *ret) : 0);
}

_Result<Modifier, ParserError> Parser::parseModifier(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseOverrideWord(_rp, _ep);
        OverrideWord* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Modifier, ParserError>(node);
    }
    {
        auto _node_result = parseStaticWord(_rp, _ep);
        StaticWord* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Modifier, ParserError>(node);
    }
    return _Result<Modifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<OverrideWord, ParserError> Parser::parseOverrideWord(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startOverride1 = lexer->getPreviousPosition(_p);
    bool successOverride1 = lexer->parseKeyword(overrideKeyword);
    if (successOverride1)
        lexer->advance();
    else
        return _Result<OverrideWord, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startOverride1), new(_ep) string(overrideKeyword))));
    Position* end = lexer->getPosition(_p);
    OverrideWord* ret = new(_rp) OverrideWord(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<OverrideWord, ParserError>(ret);
}

_Result<StaticWord, ParserError> Parser::parseStaticWord(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startStatic1 = lexer->getPreviousPosition(_p);
    bool successStatic1 = lexer->parseKeyword(staticKeyword);
    if (successStatic1)
        lexer->advance();
    else
        return _Result<StaticWord, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startStatic1), new(_ep) string(staticKeyword))));
    Position* end = lexer->getPosition(_p);
    StaticWord* ret = new(_rp) StaticWord(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<StaticWord, ParserError>(ret);
}

_Result<FunctionName, ParserError> Parser::parseFunctionName(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseIdentifierFunction(_rp, _ep);
        IdentifierFunction* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<FunctionName, ParserError>(node);
    }
    return _Result<FunctionName, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<IdentifierFunction, ParserError> Parser::parseIdentifierFunction(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<IdentifierFunction, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* end = lexer->getPosition(_p);
    IdentifierFunction* ret = new(_rp) IdentifierFunction(name, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<IdentifierFunction, ParserError>(ret);
}

_Result<FunctionSignature, ParserError> Parser::parseFunctionSignature(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _parameterClause_result = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause = nullptr;
    if (_parameterClause_result.succeeded()) {
        parameterClause = _parameterClause_result.getResult();
    }
    else {
        auto error = _parameterClause_result.getError();
        return _Result<FunctionSignature, ParserError>(error);
    }
    auto _result_result = parseFunctionResult(_rp, _ep);
    FunctionResult* result = nullptr;
    if (_result_result.succeeded()) {
        result = _result_result.getResult();
    }
    else {
        result = nullptr;
    }
    auto _throwsClause_result = parseThrowsClause(_rp, _ep);
    ThrowsClause* throwsClause = nullptr;
    if (_throwsClause_result.succeeded()) {
        throwsClause = _throwsClause_result.getResult();
    }
    else {
        throwsClause = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    FunctionSignature* ret = new(_rp) FunctionSignature(parameterClause, result, throwsClause, new(_rp) Position(start), new(_rp) Position(end));
    parameterClause->parent = ret;
    if (result != nullptr)
        result->parent = ret;
    if (throwsClause != nullptr)
        throwsClause->parent = ret;
    return _Result<FunctionSignature, ParserError>(ret);
}

_Result<FunctionResult, ParserError> Parser::parseFunctionResult(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startColon1 = lexer->getPreviousPosition(_p);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1)
        lexer->advance();
    else
        return _Result<FunctionResult, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon1), new(_ep) string(colon))));
    auto _existingObject_result = parseExistingClause(_rp, _ep);
    ExistingClause* existingObject = nullptr;
    if (_existingObject_result.succeeded()) {
        existingObject = _existingObject_result.getResult();
    }
    else {
        existingObject = nullptr;
    }
    auto _resultType_result = parseType(_rp, _ep);
    Type* resultType = nullptr;
    if (_resultType_result.succeeded()) {
        resultType = _resultType_result.getResult();
    }
    else {
        auto error = _resultType_result.getError();
        return _Result<FunctionResult, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    FunctionResult* ret = new(_rp) FunctionResult(existingObject, resultType, new(_rp) Position(start), new(_rp) Position(end));
    if (existingObject != nullptr)
        existingObject->parent = ret;
    resultType->parent = ret;
    return _Result<FunctionResult, ParserError>(ret);
}

_Result<ExistingClause, ParserError> Parser::parseExistingClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startExisting1 = lexer->getPreviousPosition(_p);
    bool successExisting1 = lexer->parseKeyword(existingKeyword);
    if (successExisting1)
        lexer->advance();
    else
        return _Result<ExistingClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startExisting1), new(_ep) string(existingKeyword))));
    Position* end = lexer->getPosition(_p);
    ExistingClause* ret = new(_rp) ExistingClause(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<ExistingClause, ParserError>(ret);
}

_Result<_Vector<ParameterClause>, ParserError> Parser::parseParameterClauseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParameterClause>* ret = nullptr;
    while (true) {
        auto _node_result = parseParameterClause(_rp, _ep);
        ParameterClause* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<ParameterClause>();
        ret->push(node);
    }
    return _Result<_Vector<ParameterClause>, ParserError>(ret ? &_Vector<ParameterClause>::create(_rp, *ret) : 0);
}

_Result<ParameterClause, ParserError> Parser::parseParameterClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftParen1 = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1)
        lexer->advance();
    else
        return _Result<ParameterClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftParen1), new(_ep) string(leftParen))));
    auto _parameters_result = parseParameterList(_rp, _ep);
    _Vector<Parameter>* parameters = nullptr;
    if (_parameters_result.succeeded()) {
        parameters = _parameters_result.getResult();
    }
    else {
        parameters = nullptr;
    }
    Position* startRightParen3 = lexer->getPreviousPosition(_p);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3)
        lexer->advance();
    else
        return _Result<ParameterClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightParen3), new(_ep) string(rightParen))));
    Position* end = lexer->getPosition(_p);
    ParameterClause* ret = new(_rp) ParameterClause(parameters, new(_rp) Position(start), new(_rp) Position(end));
    if (parameters != nullptr) {
        Parameter* item = nullptr;
        size_t _parameters_length = parameters->length();
        for (size_t _i = 0; _i < _parameters_length; _i++) {
            item = *(*parameters)[_i];
            item->parent = ret;
        }
    }
    return _Result<ParameterClause, ParserError>(ret);
}

_Result<_Vector<Parameter>, ParserError> Parser::parseParameterList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Parameter>* ret = nullptr;
    while (true) {
        auto _node_result = parseParameter(_rp, _ep);
        Parameter* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<Parameter>();
        ret->push(node);
    }
    return _Result<_Vector<Parameter>, ParserError>(ret ? &_Vector<Parameter>::create(_rp, *ret) : 0);
}

_Result<Parameter, ParserError> Parser::parseParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseConstParameter(_rp, _ep);
        ConstParameter* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Parameter, ParserError>(node);
    }
    {
        auto _node_result = parseVarParameter(_rp, _ep);
        VarParameter* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Parameter, ParserError>(node);
    }
    return _Result<Parameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<ConstParameter, ParserError> Parser::parseConstParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1)
        lexer->advance();
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<ConstParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* startColon3 = lexer->getPreviousPosition(_p);
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3)
        lexer->advance();
    else
        return _Result<ConstParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon3), new(_ep) string(colon))));
    auto _parameterType_result = parseType(_rp, _ep);
    Type* parameterType = nullptr;
    if (_parameterType_result.succeeded()) {
        parameterType = _parameterType_result.getResult();
    }
    else {
        auto error = _parameterType_result.getError();
        return _Result<ConstParameter, ParserError>(error);
    }
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    ConstParameter* ret = new(_rp) ConstParameter(name, parameterType, new(_rp) Position(start), new(_rp) Position(end));
    parameterType->parent = ret;
    return _Result<ConstParameter, ParserError>(ret);
}

_Result<VarParameter, ParserError> Parser::parseVarParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startMutable1 = lexer->getPreviousPosition(_p);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1)
        lexer->advance();
    else
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startMutable1), new(_ep) string(mutableKeyword))));
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* startColon3 = lexer->getPreviousPosition(_p);
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3)
        lexer->advance();
    else
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon3), new(_ep) string(colon))));
    auto _parameterType_result = parseType(_rp, _ep);
    Type* parameterType = nullptr;
    if (_parameterType_result.succeeded()) {
        parameterType = _parameterType_result.getResult();
    }
    else {
        auto error = _parameterType_result.getError();
        return _Result<VarParameter, ParserError>(error);
    }
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    VarParameter* ret = new(_rp) VarParameter(name, parameterType, new(_rp) Position(start), new(_rp) Position(end));
    parameterType->parent = ret;
    return _Result<VarParameter, ParserError>(ret);
}

_Result<ThrowsClause, ParserError> Parser::parseThrowsClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startThrows1 = lexer->getPreviousPosition(_p);
    bool successThrows1 = lexer->parseKeyword(throwsKeyword);
    if (successThrows1)
        lexer->advance();
    else
        return _Result<ThrowsClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startThrows1), new(_ep) string(throwsKeyword))));
    auto _throwsType_result = parseType(_rp, _ep);
    Type* throwsType = nullptr;
    if (_throwsType_result.succeeded()) {
        throwsType = _throwsType_result.getResult();
    }
    else {
        auto error = _throwsType_result.getError();
        return _Result<ThrowsClause, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ThrowsClause* ret = new(_rp) ThrowsClause(throwsType, new(_rp) Position(start), new(_rp) Position(end));
    throwsType->parent = ret;
    return _Result<ThrowsClause, ParserError>(ret);
}

_Result<_Vector<EnumMember>, ParserError> Parser::parseEnumMemberList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<EnumMember>* ret = nullptr;
    while (true) {
        auto _node_result = parseEnumMember(_rp, _ep);
        EnumMember* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<EnumMember>();
        ret->push(node);
    }
    return _Result<_Vector<EnumMember>, ParserError>(ret ? &_Vector<EnumMember>::create(_rp, *ret) : 0);
}

_Result<EnumMember, ParserError> Parser::parseEnumMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startCase1 = lexer->getPreviousPosition(_p);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1)
        lexer->advance();
    else
        return _Result<EnumMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startCase1), new(_ep) string(caseKeyword))));
    auto _enumCase_result = parseEnumCase(_rp, _ep);
    EnumCase* enumCase = nullptr;
    if (_enumCase_result.succeeded()) {
        enumCase = _enumCase_result.getResult();
    }
    else {
        auto error = _enumCase_result.getError();
        return _Result<EnumMember, ParserError>(error);
    }
    auto _additionalCases_result = parseAdditionalCaseList(_rp, _ep);
    _Vector<AdditionalCase>* additionalCases = nullptr;
    if (_additionalCases_result.succeeded()) {
        additionalCases = _additionalCases_result.getResult();
    }
    else {
        additionalCases = nullptr;
    }
    auto _parameterClause_result = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause = nullptr;
    if (_parameterClause_result.succeeded()) {
        parameterClause = _parameterClause_result.getResult();
    }
    else {
        parameterClause = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    EnumMember* ret = new(_rp) EnumMember(enumCase, additionalCases, parameterClause, new(_rp) Position(start), new(_rp) Position(end));
    enumCase->parent = ret;
    if (additionalCases != nullptr) {
        AdditionalCase* item = nullptr;
        size_t _additionalCases_length = additionalCases->length();
        for (size_t _i = 0; _i < _additionalCases_length; _i++) {
            item = *(*additionalCases)[_i];
            item->parent = ret;
        }
    }
    if (parameterClause != nullptr)
        parameterClause->parent = ret;
    return _Result<EnumMember, ParserError>(ret);
}

_Result<EnumCase, ParserError> Parser::parseEnumCase(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<EnumCase, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* end = lexer->getPosition(_p);
    EnumCase* ret = new(_rp) EnumCase(name, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<EnumCase, ParserError>(ret);
}

_Result<_Vector<AdditionalCase>, ParserError> Parser::parseAdditionalCaseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<AdditionalCase>* ret = nullptr;
    while (true) {
        auto _node_result = parseAdditionalCase(_rp, _ep);
        AdditionalCase* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<AdditionalCase>();
        ret->push(node);
    }
    return _Result<_Vector<AdditionalCase>, ParserError>(ret ? &_Vector<AdditionalCase>::create(_rp, *ret) : 0);
}

_Result<AdditionalCase, ParserError> Parser::parseAdditionalCase(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startComma1 = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1)
        lexer->advance();
    else
        return _Result<AdditionalCase, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startComma1), new(_ep) string(comma))));
    auto _enumCase_result = parseEnumCase(_rp, _ep);
    EnumCase* enumCase = nullptr;
    if (_enumCase_result.succeeded()) {
        enumCase = _enumCase_result.getResult();
    }
    else {
        auto error = _enumCase_result.getError();
        return _Result<AdditionalCase, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    AdditionalCase* ret = new(_rp) AdditionalCase(enumCase, new(_rp) Position(start), new(_rp) Position(end));
    enumCase->parent = ret;
    return _Result<AdditionalCase, ParserError>(ret);
}

_Result<ClassBody, ParserError> Parser::parseClassBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1)
        lexer->advance();
    else
        return _Result<ClassBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly1), new(_ep) string(leftCurly))));
    auto _members_result = parseClassMemberList(_rp, _ep);
    _Vector<ClassMember>* members = nullptr;
    if (_members_result.succeeded()) {
        members = _members_result.getResult();
    }
    else {
        members = nullptr;
    }
    Position* startRightCurly3 = lexer->getPreviousPosition(_p);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3)
        lexer->advance();
    else
        return _Result<ClassBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly3), new(_ep) string(rightCurly))));
    Position* end = lexer->getPosition(_p);
    ClassBody* ret = new(_rp) ClassBody(members, new(_rp) Position(start), new(_rp) Position(end));
    if (members != nullptr) {
        ClassMember* item = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            item = *(*members)[_i];
            item->parent = ret;
        }
    }
    return _Result<ClassBody, ParserError>(ret);
}

_Result<GenericArgumentClause, ParserError> Parser::parseGenericArgumentClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftAngular1 = lexer->getPreviousPosition(_p);
    bool successLeftAngular1 = lexer->parsePunctuation(leftAngular);
    if (successLeftAngular1)
        lexer->advance();
    else
        return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftAngular1), new(_ep) string(leftAngular))));
    auto _genericParameters_result = parseGenericParameterList(_rp, _ep);
    _Vector<GenericParameter>* genericParameters = nullptr;
    if (_genericParameters_result.succeeded()) {
        genericParameters = _genericParameters_result.getResult();
    }
    else {
        auto error = _genericParameters_result.getError();
        return _Result<GenericArgumentClause, ParserError>(error);
    }
    Position* startRightAngular3 = lexer->getPreviousPosition(_p);
    bool successRightAngular3 = lexer->parsePunctuation(rightAngular);
    if (successRightAngular3)
        lexer->advance();
    else
        return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightAngular3), new(_ep) string(rightAngular))));
    Position* end = lexer->getPosition(_p);
    GenericArgumentClause* ret = new(_rp) GenericArgumentClause(genericParameters, new(_rp) Position(start), new(_rp) Position(end));
    if (genericParameters != nullptr) {
        GenericParameter* item = nullptr;
        size_t _genericParameters_length = genericParameters->length();
        for (size_t _i = 0; _i < _genericParameters_length; _i++) {
            item = *(*genericParameters)[_i];
            item->parent = ret;
        }
    }
    return _Result<GenericArgumentClause, ParserError>(ret);
}

_Result<_Vector<GenericParameter>, ParserError> Parser::parseGenericParameterList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<GenericParameter>* ret = nullptr;
    while (true) {
        auto _node_result = parseGenericParameter(_rp, _ep);
        GenericParameter* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<GenericParameter>();
        ret->push(node);
    }
    return _Result<_Vector<GenericParameter>, ParserError>(ret ? &_Vector<GenericParameter>::create(_rp, *ret) : 0);
}

_Result<GenericParameter, ParserError> Parser::parseGenericParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startTypeName = lexer->getPreviousPosition(_p);
    string* typeName = lexer->parseIdentifier(_rp);
    if ((typeName != nullptr) && isIdentifier(typeName))
        lexer->advance();
    else
        return _Result<GenericParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startTypeName))));
    Position* end = lexer->getPosition(_p);
    GenericParameter* ret = new(_rp) GenericParameter(typeName, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<GenericParameter, ParserError>(ret);
}

_Result<_Vector<ClassMember>, ParserError> Parser::parseClassMemberList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ClassMember>* ret = nullptr;
    while (true) {
        auto _node_result = parseClassMember(_rp, _ep);
        ClassMember* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<ClassMember>();
        ret->push(node);
    }
    return _Result<_Vector<ClassMember>, ParserError>(ret ? &_Vector<ClassMember>::create(_rp, *ret) : 0);
}

_Result<ClassMember, ParserError> Parser::parseClassMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _declaration_result = parseDeclaration(_rp, _ep);
    Declaration* declaration = nullptr;
    if (_declaration_result.succeeded()) {
        declaration = _declaration_result.getResult();
    }
    else {
        auto error = _declaration_result.getError();
        return _Result<ClassMember, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ClassMember* ret = new(_rp) ClassMember(declaration, new(_rp) Position(start), new(_rp) Position(end));
    declaration->parent = ret;
    return _Result<ClassMember, ParserError>(ret);
}

_Result<PrefixExpression, ParserError> Parser::parsePrefixExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* prefixOperator = lexer->parsePrefixOperator(_rp);
    if (prefixOperator != nullptr)
        lexer->advance();
    auto _expression_result = parsePostfixExpression(_rp, _ep);
    PostfixExpression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<PrefixExpression, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    PrefixExpression* ret = new(_rp) PrefixExpression(prefixOperator, expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return _Result<PrefixExpression, ParserError>(ret);
}

_Result<PostfixExpression, ParserError> Parser::parsePostfixExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _primaryExpression_result = parsePrimaryExpression(_rp, _ep);
    PrimaryExpression* primaryExpression = nullptr;
    if (_primaryExpression_result.succeeded()) {
        primaryExpression = _primaryExpression_result.getResult();
    }
    else {
        auto error = _primaryExpression_result.getError();
        return _Result<PostfixExpression, ParserError>(error);
    }
    auto _postfixes_result = parsePostfixList(_rp, _ep);
    _Vector<Postfix>* postfixes = nullptr;
    if (_postfixes_result.succeeded()) {
        postfixes = _postfixes_result.getResult();
    }
    else {
        postfixes = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    PostfixExpression* ret = new(_rp) PostfixExpression(primaryExpression, postfixes, new(_rp) Position(start), new(_rp) Position(end));
    primaryExpression->parent = ret;
    if (postfixes != nullptr) {
        Postfix* item = nullptr;
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++) {
            item = *(*postfixes)[_i];
            item->parent = ret;
        }
    }
    return _Result<PostfixExpression, ParserError>(ret);
}

_Result<_Vector<BinaryOp>, ParserError> Parser::parseBinaryOpList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<BinaryOp>* ret = nullptr;
    while (true) {
        auto _node_result = parseBinaryOp(_rp, _ep);
        BinaryOp* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<BinaryOp>();
        ret->push(node);
    }
    return _Result<_Vector<BinaryOp>, ParserError>(ret ? &_Vector<BinaryOp>::create(_rp, *ret) : 0);
}

_Result<BinaryOp, ParserError> Parser::parseBinaryOp(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseBinaryOperation(_rp, _ep);
        BinaryOperation* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<BinaryOp, ParserError>(node);
    }
    {
        auto _node_result = parseAssignment(_rp, _ep);
        Assignment* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<BinaryOp, ParserError>(node);
    }
    {
        auto _node_result = parseTypeQuery(_rp, _ep);
        TypeQuery* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<BinaryOp, ParserError>(node);
    }
    {
        auto _node_result = parseTypeCast(_rp, _ep);
        TypeCast* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<BinaryOp, ParserError>(node);
    }
    return _Result<BinaryOp, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<BinaryOperation, ParserError> Parser::parseBinaryOperation(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startBinaryOperator = lexer->getPreviousPosition(_p);
    string* binaryOperator = lexer->parseBinaryOperator(_rp);
    if (binaryOperator != nullptr)
        lexer->advance();
    else
        return _Result<BinaryOperation, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_operatorExpected(new(_ep) Position(startBinaryOperator))));
    auto _expression_result = parsePrefixExpression(_rp, _ep);
    PrefixExpression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<BinaryOperation, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    BinaryOperation* ret = new(_rp) BinaryOperation(binaryOperator, expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return _Result<BinaryOperation, ParserError>(ret);
}

_Result<Assignment, ParserError> Parser::parseAssignment(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEqual1 = lexer->getPreviousPosition(_p);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1)
        lexer->advance();
    else
        return _Result<Assignment, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startEqual1), new(_ep) string(equal))));
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<Assignment, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    Assignment* ret = new(_rp) Assignment(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return _Result<Assignment, ParserError>(ret);
}

_Result<TypeQuery, ParserError> Parser::parseTypeQuery(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startIs1 = lexer->getPreviousPosition(_p);
    bool successIs1 = lexer->parseKeyword(isKeyword);
    if (successIs1)
        lexer->advance();
    else
        return _Result<TypeQuery, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startIs1), new(_ep) string(isKeyword))));
    auto _objectType_result = parseType(_rp, _ep);
    Type* objectType = nullptr;
    if (_objectType_result.succeeded()) {
        objectType = _objectType_result.getResult();
    }
    else {
        auto error = _objectType_result.getError();
        return _Result<TypeQuery, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    TypeQuery* ret = new(_rp) TypeQuery(objectType, new(_rp) Position(start), new(_rp) Position(end));
    objectType->parent = ret;
    return _Result<TypeQuery, ParserError>(ret);
}

_Result<TypeCast, ParserError> Parser::parseTypeCast(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startAs1 = lexer->getPreviousPosition(_p);
    bool successAs1 = lexer->parseKeyword(asKeyword);
    if (successAs1)
        lexer->advance();
    else
        return _Result<TypeCast, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startAs1), new(_ep) string(asKeyword))));
    auto _objectType_result = parseType(_rp, _ep);
    Type* objectType = nullptr;
    if (_objectType_result.succeeded()) {
        objectType = _objectType_result.getResult();
    }
    else {
        auto error = _objectType_result.getError();
        return _Result<TypeCast, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    TypeCast* ret = new(_rp) TypeCast(objectType, new(_rp) Position(start), new(_rp) Position(end));
    objectType->parent = ret;
    return _Result<TypeCast, ParserError>(ret);
}

_Result<_Vector<CatchClause>, ParserError> Parser::parseCatchClauseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<CatchClause>* ret = nullptr;
    while (true) {
        auto _node_result = parseCatchClause(_rp, _ep);
        CatchClause* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<CatchClause>();
        ret->push(node);
    }
    return _Result<_Vector<CatchClause>, ParserError>(ret ? &_Vector<CatchClause>::create(_rp, *ret) : 0);
}

_Result<CatchClause, ParserError> Parser::parseCatchClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startCatch1 = lexer->getPreviousPosition(_p);
    bool successCatch1 = lexer->parseKeyword(catchKeyword);
    if (successCatch1)
        lexer->advance();
    else
        return _Result<CatchClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startCatch1), new(_ep) string(catchKeyword))));
    auto _catchPattern_result = parseCatchPattern(_rp, _ep);
    CatchPattern* catchPattern = nullptr;
    if (_catchPattern_result.succeeded()) {
        catchPattern = _catchPattern_result.getResult();
    }
    else {
        auto error = _catchPattern_result.getError();
        return _Result<CatchClause, ParserError>(error);
    }
    auto _bindingPattern_result = parseTuplePattern(_rp, _ep);
    TuplePattern* bindingPattern = nullptr;
    if (_bindingPattern_result.succeeded()) {
        bindingPattern = _bindingPattern_result.getResult();
    }
    else {
        bindingPattern = nullptr;
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<CatchClause, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    CatchClause* ret = new(_rp) CatchClause(catchPattern, bindingPattern, expression, new(_rp) Position(start), new(_rp) Position(end));
    catchPattern->parent = ret;
    if (bindingPattern != nullptr)
        bindingPattern->parent = ret;
    expression->parent = ret;
    return _Result<CatchClause, ParserError>(ret);
}

_Result<CatchPattern, ParserError> Parser::parseCatchPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseWildCardCatchPattern(_rp, _ep);
        WildCardCatchPattern* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<CatchPattern, ParserError>(node);
    }
    {
        auto _node_result = parseIdentifierCatchPattern(_rp, _ep);
        IdentifierCatchPattern* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<CatchPattern, ParserError>(node);
    }
    return _Result<CatchPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<WildCardCatchPattern, ParserError> Parser::parseWildCardCatchPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _pattern_result = parseWildcardPattern(_rp, _ep);
    WildcardPattern* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<WildCardCatchPattern, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    WildCardCatchPattern* ret = new(_rp) WildCardCatchPattern(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return _Result<WildCardCatchPattern, ParserError>(ret);
}

_Result<IdentifierCatchPattern, ParserError> Parser::parseIdentifierCatchPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<IdentifierCatchPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    auto _member_result = parseExplicitMemberExpression(_rp, _ep);
    ExplicitMemberExpression* member = nullptr;
    if (_member_result.succeeded()) {
        member = _member_result.getResult();
    }
    else {
        member = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    IdentifierCatchPattern* ret = new(_rp) IdentifierCatchPattern(name, member, new(_rp) Position(start), new(_rp) Position(end));
    if (member != nullptr)
        member->parent = ret;
    return _Result<IdentifierCatchPattern, ParserError>(ret);
}

_Result<_Vector<Postfix>, ParserError> Parser::parsePostfixList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Postfix>* ret = nullptr;
    while (true) {
        auto _node_result = parsePostfix(_rp, _ep);
        Postfix* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<Postfix>();
        ret->push(node);
    }
    return _Result<_Vector<Postfix>, ParserError>(ret ? &_Vector<Postfix>::create(_rp, *ret) : 0);
}

_Result<Postfix, ParserError> Parser::parsePostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseOperatorPostfix(_rp, _ep);
        OperatorPostfix* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Postfix, ParserError>(node);
    }
    {
        auto _node_result = parseFunctionCall(_rp, _ep);
        FunctionCall* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Postfix, ParserError>(node);
    }
    {
        auto _node_result = parseExplicitMemberExpression(_rp, _ep);
        ExplicitMemberExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Postfix, ParserError>(node);
    }
    {
        auto _node_result = parseSubscript(_rp, _ep);
        Subscript* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Postfix, ParserError>(node);
    }
    return _Result<Postfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<OperatorPostfix, ParserError> Parser::parseOperatorPostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startPostfixOperator = lexer->getPreviousPosition(_p);
    string* postfixOperator = lexer->parsePostfixOperator(_rp);
    if (postfixOperator != nullptr)
        lexer->advance();
    else
        return _Result<OperatorPostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_operatorExpected(new(_ep) Position(startPostfixOperator))));
    Position* end = lexer->getPosition(_p);
    OperatorPostfix* ret = new(_rp) OperatorPostfix(postfixOperator, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<OperatorPostfix, ParserError>(ret);
}

_Result<FunctionCall, ParserError> Parser::parseFunctionCall(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _arguments_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments = nullptr;
    if (_arguments_result.succeeded()) {
        arguments = _arguments_result.getResult();
    }
    else {
        auto error = _arguments_result.getError();
        return _Result<FunctionCall, ParserError>(error);
    }
    auto _catchClauses_result = parseCatchClauseList(_rp, _ep);
    _Vector<CatchClause>* catchClauses = nullptr;
    if (_catchClauses_result.succeeded()) {
        catchClauses = _catchClauses_result.getResult();
    }
    else {
        catchClauses = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    FunctionCall* ret = new(_rp) FunctionCall(arguments, catchClauses, new(_rp) Position(start), new(_rp) Position(end));
    arguments->parent = ret;
    if (catchClauses != nullptr) {
        CatchClause* item = nullptr;
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++) {
            item = *(*catchClauses)[_i];
            item->parent = ret;
        }
    }
    return _Result<FunctionCall, ParserError>(ret);
}

_Result<ExplicitMemberExpression, ParserError> Parser::parseExplicitMemberExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDot1 = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1)
        lexer->advance();
    else
        return _Result<ExplicitMemberExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot1), new(_ep) string(dot))));
    auto _memberPostfix_result = parseMemberPostfix(_rp, _ep);
    MemberPostfix* memberPostfix = nullptr;
    if (_memberPostfix_result.succeeded()) {
        memberPostfix = _memberPostfix_result.getResult();
    }
    else {
        auto error = _memberPostfix_result.getError();
        return _Result<ExplicitMemberExpression, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ExplicitMemberExpression* ret = new(_rp) ExplicitMemberExpression(memberPostfix, new(_rp) Position(start), new(_rp) Position(end));
    memberPostfix->parent = ret;
    return _Result<ExplicitMemberExpression, ParserError>(ret);
}

_Result<Subscript, ParserError> Parser::parseSubscript(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftBracket1 = lexer->getPreviousPosition(_p);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1)
        lexer->advance();
    else
        return _Result<Subscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftBracket1), new(_ep) string(leftBracket))));
    auto _expressions_result = parseExpressionElementList(_rp, _ep);
    _Vector<ExpressionElement>* expressions = nullptr;
    if (_expressions_result.succeeded()) {
        expressions = _expressions_result.getResult();
    }
    else {
        auto error = _expressions_result.getError();
        return _Result<Subscript, ParserError>(error);
    }
    Position* startRightBracket3 = lexer->getPreviousPosition(_p);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3)
        lexer->advance();
    else
        return _Result<Subscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightBracket3), new(_ep) string(rightBracket))));
    Position* end = lexer->getPosition(_p);
    Subscript* ret = new(_rp) Subscript(expressions, new(_rp) Position(start), new(_rp) Position(end));
    if (expressions != nullptr) {
        ExpressionElement* item = nullptr;
        size_t _expressions_length = expressions->length();
        for (size_t _i = 0; _i < _expressions_length; _i++) {
            item = *(*expressions)[_i];
            item->parent = ret;
        }
    }
    return _Result<Subscript, ParserError>(ret);
}

_Result<_Vector<ExpressionElement>, ParserError> Parser::parseExpressionElementList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ExpressionElement>* ret = nullptr;
    while (true) {
        auto _node_result = parseExpressionElement(_rp, _ep);
        ExpressionElement* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<ExpressionElement>();
        ret->push(node);
    }
    return _Result<_Vector<ExpressionElement>, ParserError>(ret ? &_Vector<ExpressionElement>::create(_rp, *ret) : 0);
}

_Result<ExpressionElement, ParserError> Parser::parseExpressionElement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<ExpressionElement, ParserError>(error);
    }
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    ExpressionElement* ret = new(_rp) ExpressionElement(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return _Result<ExpressionElement, ParserError>(ret);
}

_Result<MemberPostfix, ParserError> Parser::parseMemberPostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseNamedMemberPostfix(_rp, _ep);
        NamedMemberPostfix* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<MemberPostfix, ParserError>(node);
    }
    return _Result<MemberPostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<NamedMemberPostfix, ParserError> Parser::parseNamedMemberPostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _identifier_result = parseIdentifierExpression(_rp, _ep);
    IdentifierExpression* identifier = nullptr;
    if (_identifier_result.succeeded()) {
        identifier = _identifier_result.getResult();
    }
    else {
        auto error = _identifier_result.getError();
        return _Result<NamedMemberPostfix, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    NamedMemberPostfix* ret = new(_rp) NamedMemberPostfix(identifier, new(_rp) Position(start), new(_rp) Position(end));
    identifier->parent = ret;
    return _Result<NamedMemberPostfix, ParserError>(ret);
}

_Result<PrimaryExpression, ParserError> Parser::parsePrimaryExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseIdentifierExpression(_rp, _ep);
        IdentifierExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseLiteralExpression(_rp, _ep);
        LiteralExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseIfExpression(_rp, _ep);
        IfExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseSwitchExpression(_rp, _ep);
        SwitchExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseForExpression(_rp, _ep);
        ForExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseWhileExpression(_rp, _ep);
        WhileExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseRepeatExpression(_rp, _ep);
        RepeatExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseParenthesizedExpression(_rp, _ep);
        ParenthesizedExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseReturnExpression(_rp, _ep);
        ReturnExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseThrowExpression(_rp, _ep);
        ThrowExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseBreakExpression(_rp, _ep);
        BreakExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseConstructorCall(_rp, _ep);
        ConstructorCall* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseThisExpression(_rp, _ep);
        ThisExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseSuperExpression(_rp, _ep);
        SuperExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    {
        auto _node_result = parseNullExpression(_rp, _ep);
        NullExpression* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<PrimaryExpression, ParserError>(node);
    }
    return _Result<PrimaryExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<IdentifierExpression, ParserError> Parser::parseIdentifierExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<IdentifierExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* end = lexer->getPosition(_p);
    IdentifierExpression* ret = new(_rp) IdentifierExpression(name, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<IdentifierExpression, ParserError>(ret);
}

_Result<LiteralExpression, ParserError> Parser::parseLiteralExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLiteral = lexer->getPreviousPosition(_p);
    Literal* literal = lexer->parseLiteral(_rp);
    if (literal != nullptr)
        lexer->advance();
    else
        return _Result<LiteralExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_literalExpected(new(_ep) Position(startLiteral))));
    Position* end = lexer->getPosition(_p);
    LiteralExpression* ret = new(_rp) LiteralExpression(literal, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<LiteralExpression, ParserError>(ret);
}

_Result<IfExpression, ParserError> Parser::parseIfExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startIf1 = lexer->getPreviousPosition(_p);
    bool successIf1 = lexer->parseKeyword(ifKeyword);
    if (successIf1)
        lexer->advance();
    else
        return _Result<IfExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startIf1), new(_ep) string(ifKeyword))));
    auto _condition_result = parseExpression(_rp, _ep);
    Expression* condition = nullptr;
    if (_condition_result.succeeded()) {
        condition = _condition_result.getResult();
    }
    else {
        auto error = _condition_result.getError();
        return _Result<IfExpression, ParserError>(error);
    }
    auto _consequent_result = parseExpression(_rp, _ep);
    Expression* consequent = nullptr;
    if (_consequent_result.succeeded()) {
        consequent = _consequent_result.getResult();
    }
    else {
        auto error = _consequent_result.getError();
        return _Result<IfExpression, ParserError>(error);
    }
    auto _elseClause_result = parseElseClause(_rp, _ep);
    ElseClause* elseClause = nullptr;
    if (_elseClause_result.succeeded()) {
        elseClause = _elseClause_result.getResult();
    }
    else {
        elseClause = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    IfExpression* ret = new(_rp) IfExpression(condition, consequent, elseClause, new(_rp) Position(start), new(_rp) Position(end));
    condition->parent = ret;
    consequent->parent = ret;
    if (elseClause != nullptr)
        elseClause->parent = ret;
    return _Result<IfExpression, ParserError>(ret);
}

_Result<SwitchExpression, ParserError> Parser::parseSwitchExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSwitch1 = lexer->getPreviousPosition(_p);
    bool successSwitch1 = lexer->parseKeyword(switchKeyword);
    if (successSwitch1)
        lexer->advance();
    else
        return _Result<SwitchExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startSwitch1), new(_ep) string(switchKeyword))));
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<SwitchExpression, ParserError>(error);
    }
    auto _body_result = parseSwitchBody(_rp, _ep);
    SwitchBody* body = nullptr;
    if (_body_result.succeeded()) {
        body = _body_result.getResult();
    }
    else {
        auto error = _body_result.getError();
        return _Result<SwitchExpression, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    SwitchExpression* ret = new(_rp) SwitchExpression(expression, body, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    body->parent = ret;
    return _Result<SwitchExpression, ParserError>(ret);
}

_Result<ForExpression, ParserError> Parser::parseForExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startFor1 = lexer->getPreviousPosition(_p);
    bool successFor1 = lexer->parseKeyword(forKeyword);
    if (successFor1)
        lexer->advance();
    else
        return _Result<ForExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startFor1), new(_ep) string(forKeyword))));
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<ForExpression, ParserError>(error);
    }
    Position* startIn3 = lexer->getPreviousPosition(_p);
    bool successIn3 = lexer->parseKeyword(inKeyword);
    if (successIn3)
        lexer->advance();
    else
        return _Result<ForExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startIn3), new(_ep) string(inKeyword))));
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<ForExpression, ParserError>(error);
    }
    auto _code_result = parseExpression(_rp, _ep);
    Expression* code = nullptr;
    if (_code_result.succeeded()) {
        code = _code_result.getResult();
    }
    else {
        auto error = _code_result.getError();
        return _Result<ForExpression, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ForExpression* ret = new(_rp) ForExpression(pattern, expression, code, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    expression->parent = ret;
    code->parent = ret;
    return _Result<ForExpression, ParserError>(ret);
}

_Result<WhileExpression, ParserError> Parser::parseWhileExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startWhile1 = lexer->getPreviousPosition(_p);
    bool successWhile1 = lexer->parseKeyword(whileKeyword);
    if (successWhile1)
        lexer->advance();
    else
        return _Result<WhileExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startWhile1), new(_ep) string(whileKeyword))));
    auto _condition_result = parseExpression(_rp, _ep);
    Expression* condition = nullptr;
    if (_condition_result.succeeded()) {
        condition = _condition_result.getResult();
    }
    else {
        auto error = _condition_result.getError();
        return _Result<WhileExpression, ParserError>(error);
    }
    auto _code_result = parseExpression(_rp, _ep);
    Expression* code = nullptr;
    if (_code_result.succeeded()) {
        code = _code_result.getResult();
    }
    else {
        auto error = _code_result.getError();
        return _Result<WhileExpression, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    WhileExpression* ret = new(_rp) WhileExpression(condition, code, new(_rp) Position(start), new(_rp) Position(end));
    condition->parent = ret;
    code->parent = ret;
    return _Result<WhileExpression, ParserError>(ret);
}

_Result<RepeatExpression, ParserError> Parser::parseRepeatExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startRepeat1 = lexer->getPreviousPosition(_p);
    bool successRepeat1 = lexer->parseKeyword(repeatKeyword);
    if (successRepeat1)
        lexer->advance();
    else
        return _Result<RepeatExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startRepeat1), new(_ep) string(repeatKeyword))));
    auto _code_result = parseExpression(_rp, _ep);
    Expression* code = nullptr;
    if (_code_result.succeeded()) {
        code = _code_result.getResult();
    }
    else {
        auto error = _code_result.getError();
        return _Result<RepeatExpression, ParserError>(error);
    }
    Position* startWhile3 = lexer->getPreviousPosition(_p);
    bool successWhile3 = lexer->parseKeyword(whileKeyword);
    if (successWhile3)
        lexer->advance();
    else
        return _Result<RepeatExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startWhile3), new(_ep) string(whileKeyword))));
    auto _condition_result = parseExpression(_rp, _ep);
    Expression* condition = nullptr;
    if (_condition_result.succeeded()) {
        condition = _condition_result.getResult();
    }
    else {
        auto error = _condition_result.getError();
        return _Result<RepeatExpression, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    RepeatExpression* ret = new(_rp) RepeatExpression(code, condition, new(_rp) Position(start), new(_rp) Position(end));
    code->parent = ret;
    condition->parent = ret;
    return _Result<RepeatExpression, ParserError>(ret);
}

_Result<ParenthesizedExpression, ParserError> Parser::parseParenthesizedExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftParen1 = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1)
        lexer->advance();
    else
        return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftParen1), new(_ep) string(leftParen))));
    auto _expressionElements_result = parseExpressionElementList(_rp, _ep);
    _Vector<ExpressionElement>* expressionElements = nullptr;
    if (_expressionElements_result.succeeded()) {
        expressionElements = _expressionElements_result.getResult();
    }
    else {
        expressionElements = nullptr;
    }
    Position* startRightParen3 = lexer->getPreviousPosition(_p);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3)
        lexer->advance();
    else
        return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightParen3), new(_ep) string(rightParen))));
    Position* end = lexer->getPosition(_p);
    ParenthesizedExpression* ret = new(_rp) ParenthesizedExpression(expressionElements, new(_rp) Position(start), new(_rp) Position(end));
    if (expressionElements != nullptr) {
        ExpressionElement* item = nullptr;
        size_t _expressionElements_length = expressionElements->length();
        for (size_t _i = 0; _i < _expressionElements_length; _i++) {
            item = *(*expressionElements)[_i];
            item->parent = ret;
        }
    }
    return _Result<ParenthesizedExpression, ParserError>(ret);
}

_Result<ReturnExpression, ParserError> Parser::parseReturnExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startReturn1 = lexer->getPreviousPosition(_p);
    bool successReturn1 = lexer->parseKeyword(returnKeyword);
    if (successReturn1)
        lexer->advance();
    else
        return _Result<ReturnExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startReturn1), new(_ep) string(returnKeyword))));
    auto _expression_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        expression = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    ReturnExpression* ret = new(_rp) ReturnExpression(expression, new(_rp) Position(start), new(_rp) Position(end));
    if (expression != nullptr)
        expression->parent = ret;
    return _Result<ReturnExpression, ParserError>(ret);
}

_Result<ThrowExpression, ParserError> Parser::parseThrowExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startThrow1 = lexer->getPreviousPosition(_p);
    bool successThrow1 = lexer->parseKeyword(throwKeyword);
    if (successThrow1)
        lexer->advance();
    else
        return _Result<ThrowExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startThrow1), new(_ep) string(throwKeyword))));
    auto _error_result = parseIdentifierExpression(_rp, _ep);
    IdentifierExpression* error = nullptr;
    if (_error_result.succeeded()) {
        error = _error_result.getResult();
    }
    else {
        auto error = _error_result.getError();
        return _Result<ThrowExpression, ParserError>(error);
    }
    auto _arguments_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments = nullptr;
    if (_arguments_result.succeeded()) {
        arguments = _arguments_result.getResult();
    }
    else {
        arguments = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    ThrowExpression* ret = new(_rp) ThrowExpression(error, arguments, new(_rp) Position(start), new(_rp) Position(end));
    error->parent = ret;
    if (arguments != nullptr)
        arguments->parent = ret;
    return _Result<ThrowExpression, ParserError>(ret);
}

_Result<BreakExpression, ParserError> Parser::parseBreakExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startBreak1 = lexer->getPreviousPosition(_p);
    bool successBreak1 = lexer->parseKeyword(breakKeyword);
    if (successBreak1)
        lexer->advance();
    else
        return _Result<BreakExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startBreak1), new(_ep) string(breakKeyword))));
    auto _expression_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        expression = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    BreakExpression* ret = new(_rp) BreakExpression(expression, new(_rp) Position(start), new(_rp) Position(end));
    if (expression != nullptr)
        expression->parent = ret;
    return _Result<BreakExpression, ParserError>(ret);
}

_Result<ConstructorCall, ParserError> Parser::parseConstructorCall(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startNew1 = lexer->getPreviousPosition(_p);
    bool successNew1 = lexer->parseKeyword(newKeyword);
    if (successNew1)
        lexer->advance();
    else
        return _Result<ConstructorCall, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startNew1), new(_ep) string(newKeyword))));
    auto _typeToInitialize_result = parseType(_rp, _ep);
    Type* typeToInitialize = nullptr;
    if (_typeToInitialize_result.succeeded()) {
        typeToInitialize = _typeToInitialize_result.getResult();
    }
    else {
        auto error = _typeToInitialize_result.getError();
        return _Result<ConstructorCall, ParserError>(error);
    }
    auto _arguments_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments = nullptr;
    if (_arguments_result.succeeded()) {
        arguments = _arguments_result.getResult();
    }
    else {
        auto error = _arguments_result.getError();
        return _Result<ConstructorCall, ParserError>(error);
    }
    auto _catchClauses_result = parseCatchClauseList(_rp, _ep);
    _Vector<CatchClause>* catchClauses = nullptr;
    if (_catchClauses_result.succeeded()) {
        catchClauses = _catchClauses_result.getResult();
    }
    else {
        catchClauses = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    ConstructorCall* ret = new(_rp) ConstructorCall(typeToInitialize, arguments, catchClauses, new(_rp) Position(start), new(_rp) Position(end));
    typeToInitialize->parent = ret;
    arguments->parent = ret;
    if (catchClauses != nullptr) {
        CatchClause* item = nullptr;
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++) {
            item = *(*catchClauses)[_i];
            item->parent = ret;
        }
    }
    return _Result<ConstructorCall, ParserError>(ret);
}

_Result<ThisExpression, ParserError> Parser::parseThisExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startThis1 = lexer->getPreviousPosition(_p);
    bool successThis1 = lexer->parseKeyword(thisKeyword);
    if (successThis1)
        lexer->advance();
    else
        return _Result<ThisExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startThis1), new(_ep) string(thisKeyword))));
    Position* end = lexer->getPosition(_p);
    ThisExpression* ret = new(_rp) ThisExpression(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<ThisExpression, ParserError>(ret);
}

_Result<SuperExpression, ParserError> Parser::parseSuperExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseSuperDot(_rp, _ep);
        SuperDot* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<SuperExpression, ParserError>(node);
    }
    {
        auto _node_result = parseSuperSubscript(_rp, _ep);
        SuperSubscript* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<SuperExpression, ParserError>(node);
    }
    return _Result<SuperExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<SuperDot, ParserError> Parser::parseSuperDot(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSuper1 = lexer->getPreviousPosition(_p);
    bool successSuper1 = lexer->parseKeyword(superKeyword);
    if (successSuper1)
        lexer->advance();
    else
        return _Result<SuperDot, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startSuper1), new(_ep) string(superKeyword))));
    Position* startDot2 = lexer->getPreviousPosition(_p);
    bool successDot2 = lexer->parsePunctuation(dot);
    if (successDot2)
        lexer->advance();
    else
        return _Result<SuperDot, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot2), new(_ep) string(dot))));
    auto _member_result = parseCommonSuperMember(_rp, _ep);
    CommonSuperMember* member = nullptr;
    if (_member_result.succeeded()) {
        member = _member_result.getResult();
    }
    else {
        auto error = _member_result.getError();
        return _Result<SuperDot, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    SuperDot* ret = new(_rp) SuperDot(member, new(_rp) Position(start), new(_rp) Position(end));
    member->parent = ret;
    return _Result<SuperDot, ParserError>(ret);
}

_Result<SuperSubscript, ParserError> Parser::parseSuperSubscript(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSuper1 = lexer->getPreviousPosition(_p);
    bool successSuper1 = lexer->parseKeyword(superKeyword);
    if (successSuper1)
        lexer->advance();
    else
        return _Result<SuperSubscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startSuper1), new(_ep) string(superKeyword))));
    auto _subscript_result = parseSubscript(_rp, _ep);
    Subscript* subscript = nullptr;
    if (_subscript_result.succeeded()) {
        subscript = _subscript_result.getResult();
    }
    else {
        auto error = _subscript_result.getError();
        return _Result<SuperSubscript, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    SuperSubscript* ret = new(_rp) SuperSubscript(subscript, new(_rp) Position(start), new(_rp) Position(end));
    subscript->parent = ret;
    return _Result<SuperSubscript, ParserError>(ret);
}

_Result<NullExpression, ParserError> Parser::parseNullExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startNull1 = lexer->getPreviousPosition(_p);
    bool successNull1 = lexer->parseKeyword(nullKeyword);
    if (successNull1)
        lexer->advance();
    else
        return _Result<NullExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startNull1), new(_ep) string(nullKeyword))));
    Position* end = lexer->getPosition(_p);
    NullExpression* ret = new(_rp) NullExpression(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<NullExpression, ParserError>(ret);
}

_Result<ElseClause, ParserError> Parser::parseElseClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startElse1 = lexer->getPreviousPosition(_p);
    bool successElse1 = lexer->parseKeyword(elseKeyword);
    if (successElse1)
        lexer->advance();
    else
        return _Result<ElseClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startElse1), new(_ep) string(elseKeyword))));
    auto _alternative_result = parseExpression(_rp, _ep);
    Expression* alternative = nullptr;
    if (_alternative_result.succeeded()) {
        alternative = _alternative_result.getResult();
    }
    else {
        auto error = _alternative_result.getError();
        return _Result<ElseClause, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ElseClause* ret = new(_rp) ElseClause(alternative, new(_rp) Position(start), new(_rp) Position(end));
    alternative->parent = ret;
    return _Result<ElseClause, ParserError>(ret);
}

_Result<SwitchBody, ParserError> Parser::parseSwitchBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseCurliedSwitchBody(_rp, _ep);
        CurliedSwitchBody* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<SwitchBody, ParserError>(node);
    }
    {
        auto _node_result = parseNakedSwitchBody(_rp, _ep);
        NakedSwitchBody* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<SwitchBody, ParserError>(node);
    }
    return _Result<SwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<CurliedSwitchBody, ParserError> Parser::parseCurliedSwitchBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1)
        lexer->advance();
    else
        return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly1), new(_ep) string(leftCurly))));
    auto _cases_result = parseSwitchCaseList(_rp, _ep);
    _Vector<SwitchCase>* cases = nullptr;
    if (_cases_result.succeeded()) {
        cases = _cases_result.getResult();
    }
    else {
        auto error = _cases_result.getError();
        return _Result<CurliedSwitchBody, ParserError>(error);
    }
    Position* startRightCurly3 = lexer->getPreviousPosition(_p);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3)
        lexer->advance();
    else
        return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly3), new(_ep) string(rightCurly))));
    Position* end = lexer->getPosition(_p);
    CurliedSwitchBody* ret = new(_rp) CurliedSwitchBody(cases, new(_rp) Position(start), new(_rp) Position(end));
    if (cases != nullptr) {
        SwitchCase* item = nullptr;
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++) {
            item = *(*cases)[_i];
            item->parent = ret;
        }
    }
    return _Result<CurliedSwitchBody, ParserError>(ret);
}

_Result<NakedSwitchBody, ParserError> Parser::parseNakedSwitchBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _cases_result = parseSwitchCaseList(_rp, _ep);
    _Vector<SwitchCase>* cases = nullptr;
    if (_cases_result.succeeded()) {
        cases = _cases_result.getResult();
    }
    else {
        auto error = _cases_result.getError();
        return _Result<NakedSwitchBody, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    NakedSwitchBody* ret = new(_rp) NakedSwitchBody(cases, new(_rp) Position(start), new(_rp) Position(end));
    if (cases != nullptr) {
        SwitchCase* item = nullptr;
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++) {
            item = *(*cases)[_i];
            item->parent = ret;
        }
    }
    return _Result<NakedSwitchBody, ParserError>(ret);
}

_Result<_Vector<SwitchCase>, ParserError> Parser::parseSwitchCaseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<SwitchCase>* ret = nullptr;
    while (true) {
        auto _node_result = parseSwitchCase(_rp, _ep);
        SwitchCase* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<SwitchCase>();
        ret->push(node);
    }
    return _Result<_Vector<SwitchCase>, ParserError>(ret ? &_Vector<SwitchCase>::create(_rp, *ret) : 0);
}

_Result<SwitchCase, ParserError> Parser::parseSwitchCase(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _label_result = parseCaseLabel(_rp, _ep);
    CaseLabel* label = nullptr;
    if (_label_result.succeeded()) {
        label = _label_result.getResult();
    }
    else {
        auto error = _label_result.getError();
        return _Result<SwitchCase, ParserError>(error);
    }
    auto _content_result = parseCaseContent(_rp, _ep);
    CaseContent* content = nullptr;
    if (_content_result.succeeded()) {
        content = _content_result.getResult();
    }
    else {
        auto error = _content_result.getError();
        return _Result<SwitchCase, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    SwitchCase* ret = new(_rp) SwitchCase(label, content, new(_rp) Position(start), new(_rp) Position(end));
    label->parent = ret;
    content->parent = ret;
    return _Result<SwitchCase, ParserError>(ret);
}

_Result<CaseLabel, ParserError> Parser::parseCaseLabel(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseItemCaseLabel(_rp, _ep);
        ItemCaseLabel* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<CaseLabel, ParserError>(node);
    }
    {
        auto _node_result = parseDefaultCaseLabel(_rp, _ep);
        DefaultCaseLabel* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<CaseLabel, ParserError>(node);
    }
    return _Result<CaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<ItemCaseLabel, ParserError> Parser::parseItemCaseLabel(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startCase1 = lexer->getPreviousPosition(_p);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1)
        lexer->advance();
    else
        return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startCase1), new(_ep) string(caseKeyword))));
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<ItemCaseLabel, ParserError>(error);
    }
    auto _additionalPatterns_result = parseCaseItemList(_rp, _ep);
    _Vector<CaseItem>* additionalPatterns = nullptr;
    if (_additionalPatterns_result.succeeded()) {
        additionalPatterns = _additionalPatterns_result.getResult();
    }
    else {
        additionalPatterns = nullptr;
    }
    Position* startColon4 = lexer->getPreviousPosition(_p);
    bool successColon4 = lexer->parsePunctuation(colon);
    if (successColon4)
        lexer->advance();
    else
        return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon4), new(_ep) string(colon))));
    Position* end = lexer->getPosition(_p);
    ItemCaseLabel* ret = new(_rp) ItemCaseLabel(pattern, additionalPatterns, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    if (additionalPatterns != nullptr) {
        CaseItem* item = nullptr;
        size_t _additionalPatterns_length = additionalPatterns->length();
        for (size_t _i = 0; _i < _additionalPatterns_length; _i++) {
            item = *(*additionalPatterns)[_i];
            item->parent = ret;
        }
    }
    return _Result<ItemCaseLabel, ParserError>(ret);
}

_Result<DefaultCaseLabel, ParserError> Parser::parseDefaultCaseLabel(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDefault1 = lexer->getPreviousPosition(_p);
    bool successDefault1 = lexer->parseKeyword(defaultKeyword);
    if (successDefault1)
        lexer->advance();
    else
        return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startDefault1), new(_ep) string(defaultKeyword))));
    Position* startColon2 = lexer->getPreviousPosition(_p);
    bool successColon2 = lexer->parsePunctuation(colon);
    if (successColon2)
        lexer->advance();
    else
        return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon2), new(_ep) string(colon))));
    Position* end = lexer->getPosition(_p);
    DefaultCaseLabel* ret = new(_rp) DefaultCaseLabel(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<DefaultCaseLabel, ParserError>(ret);
}

_Result<_Vector<CaseItem>, ParserError> Parser::parseCaseItemList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<CaseItem>* ret = nullptr;
    while (true) {
        auto _node_result = parseCaseItem(_rp, _ep);
        CaseItem* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<CaseItem>();
        ret->push(node);
    }
    return _Result<_Vector<CaseItem>, ParserError>(ret ? &_Vector<CaseItem>::create(_rp, *ret) : 0);
}

_Result<CaseItem, ParserError> Parser::parseCaseItem(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startComma1 = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1)
        lexer->advance();
    else
        return _Result<CaseItem, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startComma1), new(_ep) string(comma))));
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<CaseItem, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    CaseItem* ret = new(_rp) CaseItem(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return _Result<CaseItem, ParserError>(ret);
}

_Result<Pattern, ParserError> Parser::parsePattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseWildcardPattern(_rp, _ep);
        WildcardPattern* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Pattern, ParserError>(node);
    }
    {
        auto _node_result = parseIdentifierPattern(_rp, _ep);
        IdentifierPattern* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Pattern, ParserError>(node);
    }
    {
        auto _node_result = parseTuplePattern(_rp, _ep);
        TuplePattern* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Pattern, ParserError>(node);
    }
    {
        auto _node_result = parseExpressionPattern(_rp, _ep);
        ExpressionPattern* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<Pattern, ParserError>(node);
    }
    return _Result<Pattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<WildcardPattern, ParserError> Parser::parseWildcardPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startUnderscore1 = lexer->getPreviousPosition(_p);
    bool successUnderscore1 = lexer->parsePunctuation(underscore);
    if (successUnderscore1)
        lexer->advance();
    else
        return _Result<WildcardPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startUnderscore1), new(_ep) string(underscore))));
    Position* end = lexer->getPosition(_p);
    WildcardPattern* ret = new(_rp) WildcardPattern(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<WildcardPattern, ParserError>(ret);
}

_Result<IdentifierPattern, ParserError> Parser::parseIdentifierPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startIdentifier = lexer->getPreviousPosition(_p);
    string* identifier = lexer->parseIdentifier(_rp);
    if ((identifier != nullptr) && isIdentifier(identifier))
        lexer->advance();
    else
        return _Result<IdentifierPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startIdentifier))));
    auto _annotationForType_result = parseTypeAnnotation(_rp, _ep);
    TypeAnnotation* annotationForType = nullptr;
    if (_annotationForType_result.succeeded()) {
        annotationForType = _annotationForType_result.getResult();
    }
    else {
        annotationForType = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    IdentifierPattern* ret = new(_rp) IdentifierPattern(identifier, annotationForType, new(_rp) Position(start), new(_rp) Position(end));
    if (annotationForType != nullptr)
        annotationForType->parent = ret;
    return _Result<IdentifierPattern, ParserError>(ret);
}

_Result<TuplePattern, ParserError> Parser::parseTuplePattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftParen1 = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1)
        lexer->advance();
    else
        return _Result<TuplePattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftParen1), new(_ep) string(leftParen))));
    auto _elements_result = parseTuplePatternElementList(_rp, _ep);
    _Vector<TuplePatternElement>* elements = nullptr;
    if (_elements_result.succeeded()) {
        elements = _elements_result.getResult();
    }
    else {
        auto error = _elements_result.getError();
        return _Result<TuplePattern, ParserError>(error);
    }
    Position* startRightParen3 = lexer->getPreviousPosition(_p);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3)
        lexer->advance();
    else
        return _Result<TuplePattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightParen3), new(_ep) string(rightParen))));
    Position* end = lexer->getPosition(_p);
    TuplePattern* ret = new(_rp) TuplePattern(elements, new(_rp) Position(start), new(_rp) Position(end));
    if (elements != nullptr) {
        TuplePatternElement* item = nullptr;
        size_t _elements_length = elements->length();
        for (size_t _i = 0; _i < _elements_length; _i++) {
            item = *(*elements)[_i];
            item->parent = ret;
        }
    }
    return _Result<TuplePattern, ParserError>(ret);
}

_Result<ExpressionPattern, ParserError> Parser::parseExpressionPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression = nullptr;
    if (_expression_result.succeeded()) {
        expression = _expression_result.getResult();
    }
    else {
        auto error = _expression_result.getError();
        return _Result<ExpressionPattern, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    ExpressionPattern* ret = new(_rp) ExpressionPattern(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return _Result<ExpressionPattern, ParserError>(ret);
}

_Result<_Vector<TuplePatternElement>, ParserError> Parser::parseTuplePatternElementList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<TuplePatternElement>* ret = nullptr;
    while (true) {
        auto _node_result = parseTuplePatternElement(_rp, _ep);
        TuplePatternElement* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<TuplePatternElement>();
        ret->push(node);
    }
    return _Result<_Vector<TuplePatternElement>, ParserError>(ret ? &_Vector<TuplePatternElement>::create(_rp, *ret) : 0);
}

_Result<TuplePatternElement, ParserError> Parser::parseTuplePatternElement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern = nullptr;
    if (_pattern_result.succeeded()) {
        pattern = _pattern_result.getResult();
    }
    else {
        auto error = _pattern_result.getError();
        return _Result<TuplePatternElement, ParserError>(error);
    }
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    TuplePatternElement* ret = new(_rp) TuplePatternElement(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return _Result<TuplePatternElement, ParserError>(ret);
}

_Result<CaseContent, ParserError> Parser::parseCaseContent(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _statements_result = parseStatementList(_rp, _ep);
    _Vector<Statement>* statements = nullptr;
    if (_statements_result.succeeded()) {
        statements = _statements_result.getResult();
    }
    else {
        auto error = _statements_result.getError();
        return _Result<CaseContent, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    CaseContent* ret = new(_rp) CaseContent(statements, new(_rp) Position(start), new(_rp) Position(end));
    if (statements != nullptr) {
        Statement* item = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            item = *(*statements)[_i];
            item->parent = ret;
        }
    }
    return _Result<CaseContent, ParserError>(ret);
}

_Result<CommonSuperMember, ParserError> Parser::parseCommonSuperMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseSuperConstructor(_rp, _ep);
        SuperConstructor* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<CommonSuperMember, ParserError>(node);
    }
    {
        auto _node_result = parseSuperMember(_rp, _ep);
        SuperMember* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<CommonSuperMember, ParserError>(node);
    }
    return _Result<CommonSuperMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<SuperConstructor, ParserError> Parser::parseSuperConstructor(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startConstructor1 = lexer->getPreviousPosition(_p);
    bool successConstructor1 = lexer->parseKeyword(constructorKeyword);
    if (successConstructor1)
        lexer->advance();
    else
        return _Result<SuperConstructor, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startConstructor1), new(_ep) string(constructorKeyword))));
    Position* end = lexer->getPosition(_p);
    SuperConstructor* ret = new(_rp) SuperConstructor(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<SuperConstructor, ParserError>(ret);
}

_Result<SuperMember, ParserError> Parser::parseSuperMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<SuperMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    Position* end = lexer->getPosition(_p);
    SuperMember* ret = new(_rp) SuperMember(name, new(_rp) Position(start), new(_rp) Position(end));
    return _Result<SuperMember, ParserError>(ret);
}

_Result<Type, ParserError> Parser::parseType(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return _Result<Type, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    auto _subType_result = parseSubtype(_rp, _ep);
    Subtype* subType = nullptr;
    if (_subType_result.succeeded()) {
        subType = _subType_result.getResult();
    }
    else {
        subType = nullptr;
    }
    auto _postfixes_result = parseTypePostfixList(_rp, _ep);
    _Vector<TypePostfix>* postfixes = nullptr;
    if (_postfixes_result.succeeded()) {
        postfixes = _postfixes_result.getResult();
    }
    else {
        postfixes = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    Type* ret = new(_rp) Type(name, subType, postfixes, new(_rp) Position(start), new(_rp) Position(end));
    if (subType != nullptr)
        subType->parent = ret;
    if (postfixes != nullptr) {
        TypePostfix* item = nullptr;
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++) {
            item = *(*postfixes)[_i];
            item->parent = ret;
        }
    }
    return _Result<Type, ParserError>(ret);
}

_Result<TypeAnnotation, ParserError> Parser::parseTypeAnnotation(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startColon1 = lexer->getPreviousPosition(_p);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1)
        lexer->advance();
    else
        return _Result<TypeAnnotation, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon1), new(_ep) string(colon))));
    auto _annotationForType_result = parseType(_rp, _ep);
    Type* annotationForType = nullptr;
    if (_annotationForType_result.succeeded()) {
        annotationForType = _annotationForType_result.getResult();
    }
    else {
        auto error = _annotationForType_result.getError();
        return _Result<TypeAnnotation, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    TypeAnnotation* ret = new(_rp) TypeAnnotation(annotationForType, new(_rp) Position(start), new(_rp) Position(end));
    annotationForType->parent = ret;
    return _Result<TypeAnnotation, ParserError>(ret);
}

_Result<Subtype, ParserError> Parser::parseSubtype(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDot1 = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1)
        lexer->advance();
    else
        return _Result<Subtype, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot1), new(_ep) string(dot))));
    auto _type_result = parseType(_rp, _ep);
    Type* type = nullptr;
    if (_type_result.succeeded()) {
        type = _type_result.getResult();
    }
    else {
        auto error = _type_result.getError();
        return _Result<Subtype, ParserError>(error);
    }
    Position* end = lexer->getPosition(_p);
    Subtype* ret = new(_rp) Subtype(type, new(_rp) Position(start), new(_rp) Position(end));
    type->parent = ret;
    return _Result<Subtype, ParserError>(ret);
}

_Result<_Vector<TypePostfix>, ParserError> Parser::parseTypePostfixList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<TypePostfix>* ret = nullptr;
    while (true) {
        auto _node_result = parseTypePostfix(_rp, _ep);
        TypePostfix* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<TypePostfix>();
        ret->push(node);
    }
    return _Result<_Vector<TypePostfix>, ParserError>(ret ? &_Vector<TypePostfix>::create(_rp, *ret) : 0);
}

_Result<TypePostfix, ParserError> Parser::parseTypePostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_p) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        auto _node_result = parseOptionalType(_rp, _ep);
        OptionalType* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<TypePostfix, ParserError>(node);
    }
    {
        auto _node_result = parseIndexedType(_rp, _ep);
        IndexedType* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<TypePostfix, ParserError>(node);
    }
    return _Result<TypePostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *(errors)))));
}

_Result<OptionalType, ParserError> Parser::parseOptionalType(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startQuestionMark1 = lexer->getPreviousPosition(_p);
    bool successQuestionMark1 = lexer->parsePunctuation(questionMark);
    if (successQuestionMark1)
        lexer->advance();
    else
        return _Result<OptionalType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startQuestionMark1), new(_ep) string(questionMark))));
    Position* end = lexer->getPosition(_p);
    OptionalType* ret = new(_rp) OptionalType(new(_rp) Position(start), new(_rp) Position(end));
    return _Result<OptionalType, ParserError>(ret);
}

_Result<IndexedType, ParserError> Parser::parseIndexedType(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftBracket1 = lexer->getPreviousPosition(_p);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1)
        lexer->advance();
    else
        return _Result<IndexedType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftBracket1), new(_ep) string(leftBracket))));
    auto _key_result = parseType(_rp, _ep);
    Type* key = nullptr;
    if (_key_result.succeeded()) {
        key = _key_result.getResult();
    }
    else {
        key = nullptr;
    }
    Position* startRightBracket3 = lexer->getPreviousPosition(_p);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3)
        lexer->advance();
    else
        return _Result<IndexedType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightBracket3), new(_ep) string(rightBracket))));
    Position* end = lexer->getPosition(_p);
    IndexedType* ret = new(_rp) IndexedType(key, new(_rp) Position(start), new(_rp) Position(end));
    if (key != nullptr)
        key->parent = ret;
    return _Result<IndexedType, ParserError>(ret);
}

_Result<TypeInheritanceClause, ParserError> Parser::parseTypeInheritanceClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startExtends1 = lexer->getPreviousPosition(_p);
    bool successExtends1 = lexer->parseKeyword(extendsKeyword);
    if (successExtends1)
        lexer->advance();
    else
        return _Result<TypeInheritanceClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startExtends1), new(_ep) string(extendsKeyword))));
    auto _inheritances_result = parseInheritanceList(_rp, _ep);
    _Vector<Inheritance>* inheritances = nullptr;
    if (_inheritances_result.succeeded()) {
        inheritances = _inheritances_result.getResult();
    }
    else {
        inheritances = nullptr;
    }
    Position* end = lexer->getPosition(_p);
    TypeInheritanceClause* ret = new(_rp) TypeInheritanceClause(inheritances, new(_rp) Position(start), new(_rp) Position(end));
    if (inheritances != nullptr) {
        Inheritance* item = nullptr;
        size_t _inheritances_length = inheritances->length();
        for (size_t _i = 0; _i < _inheritances_length; _i++) {
            item = *(*inheritances)[_i];
            item->parent = ret;
        }
    }
    return _Result<TypeInheritanceClause, ParserError>(ret);
}

_Result<_Vector<Inheritance>, ParserError> Parser::parseInheritanceList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Inheritance>* ret = nullptr;
    while (true) {
        auto _node_result = parseInheritance(_rp, _ep);
        Inheritance* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<Inheritance>();
        ret->push(node);
    }
    return _Result<_Vector<Inheritance>, ParserError>(ret ? &_Vector<Inheritance>::create(_rp, *ret) : 0);
}

_Result<Inheritance, ParserError> Parser::parseInheritance(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _type_result = parseType(_rp, _ep);
    Type* type = nullptr;
    if (_type_result.succeeded()) {
        type = _type_result.getResult();
    }
    else {
        auto error = _type_result.getError();
        return _Result<Inheritance, ParserError>(error);
    }
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    Inheritance* ret = new(_rp) Inheritance(type, new(_rp) Position(start), new(_rp) Position(end));
    type->parent = ret;
    return _Result<Inheritance, ParserError>(ret);
}

bool Parser::isAtEnd() {
    return lexer->isAtEnd();
}

bool Parser::isIdentifier(string* id) {
    if (id->equals(useKeyword))
        return false;
    if (id->equals(classKeyword))
        return false;
    if (id->equals(functionKeyword))
        return false;
    if (id->equals(ifKeyword))
        return false;
    if (id->equals(elseKeyword))
        return false;
    if (id->equals(switchKeyword))
        return false;
    if (id->equals(caseKeyword))
        return false;
    if (id->equals(defaultKeyword))
        return false;
    if (id->equals(catchKeyword))
        return false;
    if (id->equals(forKeyword))
        return false;
    if (id->equals(inKeyword))
        return false;
    if (id->equals(whileKeyword))
        return false;
    if (id->equals(repeatKeyword))
        return false;
    if (id->equals(returnKeyword))
        return false;
    if (id->equals(throwKeyword))
        return false;
    if (id->equals(breakKeyword))
        return false;
    if (id->equals(throwsKeyword))
        return false;
    if (id->equals(existingKeyword))
        return false;
    if (id->equals(overrideKeyword))
        return false;
    if (id->equals(staticKeyword))
        return false;
    if (id->equals(letKeyword))
        return false;
    if (id->equals(varKeyword))
        return false;
    if (id->equals(mutableKeyword))
        return false;
    if (id->equals(isKeyword))
        return false;
    if (id->equals(asKeyword))
        return false;
    if (id->equals(constructorKeyword))
        return false;
    if (id->equals(enumKeyword))
        return false;
    if (id->equals(superKeyword))
        return false;
    if (id->equals(thisKeyword))
        return false;
    if (id->equals(nullKeyword))
        return false;
    if (id->equals(newKeyword))
        return false;
    if (id->equals(extendsKeyword))
        return false;
    return true;
}


}
