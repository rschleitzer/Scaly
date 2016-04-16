#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Parser::Parser(String* theFileName, String* text) {
    lexer = new(getPage()->allocateExclusivePage()) Lexer(text);
    fileName = theFileName;
    useKeyword = &String::create(getPage(), "use");
    classKeyword = &String::create(getPage(), "class");
    functionKeyword = &String::create(getPage(), "function");
    ifKeyword = &String::create(getPage(), "if");
    elseKeyword = &String::create(getPage(), "else");
    switchKeyword = &String::create(getPage(), "switch");
    caseKeyword = &String::create(getPage(), "case");
    defaultKeyword = &String::create(getPage(), "default");
    catchKeyword = &String::create(getPage(), "catch");
    forKeyword = &String::create(getPage(), "for");
    inKeyword = &String::create(getPage(), "in");
    whileKeyword = &String::create(getPage(), "while");
    repeatKeyword = &String::create(getPage(), "repeat");
    returnKeyword = &String::create(getPage(), "return");
    throwKeyword = &String::create(getPage(), "throw");
    breakKeyword = &String::create(getPage(), "break");
    throwsKeyword = &String::create(getPage(), "throws");
    existingKeyword = &String::create(getPage(), "existing");
    overrideKeyword = &String::create(getPage(), "override");
    staticKeyword = &String::create(getPage(), "static");
    letKeyword = &String::create(getPage(), "let");
    varKeyword = &String::create(getPage(), "var");
    mutableKeyword = &String::create(getPage(), "mutable");
    isKeyword = &String::create(getPage(), "is");
    asKeyword = &String::create(getPage(), "as");
    initKeyword = &String::create(getPage(), "init");
    enumKeyword = &String::create(getPage(), "enum");
    superKeyword = &String::create(getPage(), "super");
    thisKeyword = &String::create(getPage(), "this");
    nullKeyword = &String::create(getPage(), "null");
    semicolon = &String::create(getPage(), ";");
    equal = &String::create(getPage(), "=");
    leftAngular = &String::create(getPage(), "<");
    rightAngular = &String::create(getPage(), ">");
    comma = &String::create(getPage(), ",");
    leftParen = &String::create(getPage(), "(");
    rightParen = &String::create(getPage(), ")");
    leftCurly = &String::create(getPage(), "{");
    rightCurly = &String::create(getPage(), "}");
    leftBracket = &String::create(getPage(), "[");
    rightBracket = &String::create(getPage(), "]");
    colon = &String::create(getPage(), ":");
    dot = &String::create(getPage(), ".");
    questionMark = &String::create(getPage(), "?");
    underscore = &String::create(getPage(), "_");
    evaluatesTo = &String::create(getPage(), "->");
}

_Result<CompilationUnit, ParserError> Parser::parseCompilationUnit(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _statements_result = parseTerminatedStatementList(_rp, _ep);
    _Vector<TerminatedStatement>* statements;
    if (_statements_result.succeeded())
        statements = _statements_result.getResult();
    else
        statements = nullptr;
    if (statements != nullptr) {
        if (!isAtEnd()) {
            Position* current = lexer->getPosition(_ep);
            return _Result<CompilationUnit, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_notAtEnd(current)));
        }
    }
    CompilationUnit* compilationUnit = new(_rp) CompilationUnit(statements, new(_rp) Position(start), lexer->getPosition(_rp));
    compilationUnit->fileName = fileName;
    if (statements) {
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++)
            (*(*statements)[_i])->parent = compilationUnit;
    }
    return _Result<CompilationUnit, ParserError>(compilationUnit);
}

_Result<_Vector<TerminatedStatement>, ParserError> Parser::parseTerminatedStatementList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<TerminatedStatement>* terminatedStatement = 0;
    while (true) {
        _Result<TerminatedStatement, ParserError> nodeResult = parseTerminatedStatement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!terminatedStatement)
                terminatedStatement = new(_p) _Array<TerminatedStatement>();
            terminatedStatement->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<TerminatedStatement>, ParserError>(terminatedStatement ? &_Vector<TerminatedStatement>::create(_rp, *terminatedStatement) : 0);
}

_Result<TerminatedStatement, ParserError> Parser::parseTerminatedStatement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _statement_result = parseStatement(_rp, _ep);
    Statement* statement;
    if (_statement_result.succeeded())
        statement = _statement_result.getResult();
    else
        return _Result<TerminatedStatement, ParserError>(_statement_result.getError());
    bool successSemicolon2 = lexer->parsePunctuation(semicolon);
    if (successSemicolon2) {
        lexer->advance();
    }
    TerminatedStatement* terminatedStatement = new(_rp) TerminatedStatement(statement, new(_rp) Position(start), lexer->getPosition(_rp));
    statement->parent = terminatedStatement;
    return _Result<TerminatedStatement, ParserError>(terminatedStatement);
}

_Result<Statement, ParserError> Parser::parseStatement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<Declaration, ParserError> result = parseDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<Expression, ParserError> result = parseExpression(_rp, _p);
        if (result.succeeded())
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Statement, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<Declaration, ParserError> Parser::parseDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<UseDeclaration, ParserError> result = parseUseDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ConstantDeclaration, ParserError> result = parseConstantDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<VariableDeclaration, ParserError> result = parseVariableDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<MutableDeclaration, ParserError> result = parseMutableDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<FunctionDeclaration, ParserError> result = parseFunctionDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<EnumDeclaration, ParserError> result = parseEnumDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ClassDeclaration, ParserError> result = parseClassDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<InitializerDeclaration, ParserError> result = parseInitializerDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Declaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<Expression, ParserError> Parser::parseExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<CodeBlock, ParserError> result = parseCodeBlock(_rp, _p);
        if (result.succeeded())
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SimpleExpression, ParserError> result = parseSimpleExpression(_rp, _p);
        if (result.succeeded())
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Expression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<UseDeclaration, ParserError> Parser::parseUseDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startUse1 = lexer->getPreviousPosition(_p);
    bool successUse1 = lexer->parseKeyword(useKeyword);
    if (successUse1) {
        lexer->advance();
    }
    else {
        return _Result<UseDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startUse1), &String::create(_ep, useKeyword))));
    }
    auto _importModule_result = parsePathItem(_rp, _ep);
    PathItem* importModule;
    if (_importModule_result.succeeded())
        importModule = _importModule_result.getResult();
    else
        return _Result<UseDeclaration, ParserError>(_importModule_result.getError());
    auto _importExtensions_result = parsePathIdentifierList(_rp, _ep);
    _Vector<PathIdentifier>* importExtensions;
    if (_importExtensions_result.succeeded())
        importExtensions = _importExtensions_result.getResult();
    else
        importExtensions = nullptr;
    UseDeclaration* useDeclaration = new(_rp) UseDeclaration(importModule, importExtensions, new(_rp) Position(start), lexer->getPosition(_rp));
    importModule->parent = useDeclaration;
    if (importExtensions) {
        size_t _importExtensions_length = importExtensions->length();
        for (size_t _i = 0; _i < _importExtensions_length; _i++)
            (*(*importExtensions)[_i])->parent = useDeclaration;
    }
    return _Result<UseDeclaration, ParserError>(useDeclaration);
}

_Result<ConstantDeclaration, ParserError> Parser::parseConstantDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLet1 = lexer->getPreviousPosition(_p);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1) {
        lexer->advance();
    }
    else {
        return _Result<ConstantDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startLet1), &String::create(_ep, letKeyword))));
    }
    auto _initializer_result = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer;
    if (_initializer_result.succeeded())
        initializer = _initializer_result.getResult();
    else
        return _Result<ConstantDeclaration, ParserError>(_initializer_result.getError());
    ConstantDeclaration* constantDeclaration = new(_rp) ConstantDeclaration(initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = constantDeclaration;
    return _Result<ConstantDeclaration, ParserError>(constantDeclaration);
}

_Result<VariableDeclaration, ParserError> Parser::parseVariableDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startVar1 = lexer->getPreviousPosition(_p);
    bool successVar1 = lexer->parseKeyword(varKeyword);
    if (successVar1) {
        lexer->advance();
    }
    else {
        return _Result<VariableDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startVar1), &String::create(_ep, varKeyword))));
    }
    auto _initializer_result = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer;
    if (_initializer_result.succeeded())
        initializer = _initializer_result.getResult();
    else
        return _Result<VariableDeclaration, ParserError>(_initializer_result.getError());
    VariableDeclaration* variableDeclaration = new(_rp) VariableDeclaration(initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = variableDeclaration;
    return _Result<VariableDeclaration, ParserError>(variableDeclaration);
}

_Result<MutableDeclaration, ParserError> Parser::parseMutableDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startMutable1 = lexer->getPreviousPosition(_p);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1) {
        lexer->advance();
    }
    else {
        return _Result<MutableDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startMutable1), &String::create(_ep, mutableKeyword))));
    }
    auto _initializer_result = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer;
    if (_initializer_result.succeeded())
        initializer = _initializer_result.getResult();
    else
        return _Result<MutableDeclaration, ParserError>(_initializer_result.getError());
    MutableDeclaration* mutableDeclaration = new(_rp) MutableDeclaration(initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = mutableDeclaration;
    return _Result<MutableDeclaration, ParserError>(mutableDeclaration);
}

_Result<FunctionDeclaration, ParserError> Parser::parseFunctionDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _modifiers_result = parseModifierList(_rp, _ep);
    _Vector<Modifier>* modifiers;
    if (_modifiers_result.succeeded())
        modifiers = _modifiers_result.getResult();
    else
        modifiers = nullptr;
    Position* startFunction2 = lexer->getPreviousPosition(_p);
    bool successFunction2 = lexer->parseKeyword(functionKeyword);
    if (successFunction2) {
        lexer->advance();
    }
    else {
        return _Result<FunctionDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startFunction2), &String::create(_ep, functionKeyword))));
    }
    auto _name_result = parseFunctionName(_rp, _ep);
    FunctionName* name;
    if (_name_result.succeeded())
        name = _name_result.getResult();
    else
        return _Result<FunctionDeclaration, ParserError>(_name_result.getError());
    auto _signature_result = parseFunctionSignature(_rp, _ep);
    FunctionSignature* signature;
    if (_signature_result.succeeded())
        signature = _signature_result.getResult();
    else
        return _Result<FunctionDeclaration, ParserError>(_signature_result.getError());
    auto _body_result = parseExpression(_rp, _ep);
    Expression* body;
    if (_body_result.succeeded())
        body = _body_result.getResult();
    else
        body = nullptr;
    FunctionDeclaration* functionDeclaration = new(_rp) FunctionDeclaration(modifiers, name, signature, body, new(_rp) Position(start), lexer->getPosition(_rp));
    if (modifiers) {
        size_t _modifiers_length = modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++)
            (*(*modifiers)[_i])->parent = functionDeclaration;
    }
    name->parent = functionDeclaration;
    signature->parent = functionDeclaration;
    if (body)
        body->parent = functionDeclaration;
    return _Result<FunctionDeclaration, ParserError>(functionDeclaration);
}

_Result<EnumDeclaration, ParserError> Parser::parseEnumDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEnum1 = lexer->getPreviousPosition(_p);
    bool successEnum1 = lexer->parseKeyword(enumKeyword);
    if (successEnum1) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startEnum1), &String::create(_ep, enumKeyword))));
    }
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    Position* startLeftCurly3 = lexer->getPreviousPosition(_p);
    bool successLeftCurly3 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly3) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly3), &String::create(_ep, leftCurly))));
    }
    auto _members_result = parseEnumMemberList(_rp, _ep);
    _Vector<EnumMember>* members;
    if (_members_result.succeeded())
        members = _members_result.getResult();
    else
        return _Result<EnumDeclaration, ParserError>(_members_result.getError());
    Position* startRightCurly5 = lexer->getPreviousPosition(_p);
    bool successRightCurly5 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly5) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly5), &String::create(_ep, rightCurly))));
    }
    EnumDeclaration* enumDeclaration = new(_rp) EnumDeclaration(name, members, new(_rp) Position(start), lexer->getPosition(_rp));
    if (members) {
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++)
            (*(*members)[_i])->parent = enumDeclaration;
    }
    return _Result<EnumDeclaration, ParserError>(enumDeclaration);
}

_Result<ClassDeclaration, ParserError> Parser::parseClassDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startClass1 = lexer->getPreviousPosition(_p);
    bool successClass1 = lexer->parseKeyword(classKeyword);
    if (successClass1) {
        lexer->advance();
    }
    else {
        return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startClass1), &String::create(_ep, classKeyword))));
    }
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    auto _genericArgumentClause_result = parseGenericArgumentClause(_rp, _ep);
    GenericArgumentClause* genericArgumentClause;
    if (_genericArgumentClause_result.succeeded())
        genericArgumentClause = _genericArgumentClause_result.getResult();
    else
        genericArgumentClause = nullptr;
    auto _typeInheritanceClause_result = parseTypeInheritanceClause(_rp, _ep);
    TypeInheritanceClause* typeInheritanceClause;
    if (_typeInheritanceClause_result.succeeded())
        typeInheritanceClause = _typeInheritanceClause_result.getResult();
    else
        typeInheritanceClause = nullptr;
    auto _body_result = parseClassBody(_rp, _ep);
    ClassBody* body;
    if (_body_result.succeeded())
        body = _body_result.getResult();
    else
        body = nullptr;
    ClassDeclaration* classDeclaration = new(_rp) ClassDeclaration(name, genericArgumentClause, typeInheritanceClause, body, new(_rp) Position(start), lexer->getPosition(_rp));
    if (genericArgumentClause)
        genericArgumentClause->parent = classDeclaration;
    if (typeInheritanceClause)
        typeInheritanceClause->parent = classDeclaration;
    if (body)
        body->parent = classDeclaration;
    return _Result<ClassDeclaration, ParserError>(classDeclaration);
}

_Result<InitializerDeclaration, ParserError> Parser::parseInitializerDeclaration(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _modifiers_result = parseModifierList(_rp, _ep);
    _Vector<Modifier>* modifiers;
    if (_modifiers_result.succeeded())
        modifiers = _modifiers_result.getResult();
    else
        modifiers = nullptr;
    Position* startInit2 = lexer->getPreviousPosition(_p);
    bool successInit2 = lexer->parseKeyword(initKeyword);
    if (successInit2) {
        lexer->advance();
    }
    else {
        return _Result<InitializerDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startInit2), &String::create(_ep, initKeyword))));
    }
    auto _parameterClause_result = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause;
    if (_parameterClause_result.succeeded())
        parameterClause = _parameterClause_result.getResult();
    else
        return _Result<InitializerDeclaration, ParserError>(_parameterClause_result.getError());
    auto _throwsClause_result = parseThrowsClause(_rp, _ep);
    ThrowsClause* throwsClause;
    if (_throwsClause_result.succeeded())
        throwsClause = _throwsClause_result.getResult();
    else
        throwsClause = nullptr;
    auto _body_result = parseExpression(_rp, _ep);
    Expression* body;
    if (_body_result.succeeded())
        body = _body_result.getResult();
    else
        return _Result<InitializerDeclaration, ParserError>(_body_result.getError());
    InitializerDeclaration* initializerDeclaration = new(_rp) InitializerDeclaration(modifiers, parameterClause, throwsClause, body, new(_rp) Position(start), lexer->getPosition(_rp));
    if (modifiers) {
        size_t _modifiers_length = modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++)
            (*(*modifiers)[_i])->parent = initializerDeclaration;
    }
    parameterClause->parent = initializerDeclaration;
    if (throwsClause)
        throwsClause->parent = initializerDeclaration;
    body->parent = initializerDeclaration;
    return _Result<InitializerDeclaration, ParserError>(initializerDeclaration);
}

_Result<CodeBlock, ParserError> Parser::parseCodeBlock(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1) {
        lexer->advance();
    }
    else {
        return _Result<CodeBlock, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly1), &String::create(_ep, leftCurly))));
    }
    auto _statements_result = parseTerminatedStatementList(_rp, _ep);
    _Vector<TerminatedStatement>* statements;
    if (_statements_result.succeeded())
        statements = _statements_result.getResult();
    else
        return _Result<CodeBlock, ParserError>(_statements_result.getError());
    Position* startRightCurly3 = lexer->getPreviousPosition(_p);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3) {
        lexer->advance();
    }
    else {
        return _Result<CodeBlock, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly3), &String::create(_ep, rightCurly))));
    }
    CodeBlock* codeBlock = new(_rp) CodeBlock(statements, new(_rp) Position(start), lexer->getPosition(_rp));
    if (statements) {
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++)
            (*(*statements)[_i])->parent = codeBlock;
    }
    return _Result<CodeBlock, ParserError>(codeBlock);
}

_Result<SimpleExpression, ParserError> Parser::parseSimpleExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _prefixExpression_result = parsePrefixExpression(_rp, _ep);
    PrefixExpression* prefixExpression;
    if (_prefixExpression_result.succeeded())
        prefixExpression = _prefixExpression_result.getResult();
    else
        return _Result<SimpleExpression, ParserError>(_prefixExpression_result.getError());
    auto _binaryOps_result = parseBinaryOpList(_rp, _ep);
    _Vector<BinaryOp>* binaryOps;
    if (_binaryOps_result.succeeded())
        binaryOps = _binaryOps_result.getResult();
    else
        binaryOps = nullptr;
    SimpleExpression* simpleExpression = new(_rp) SimpleExpression(prefixExpression, binaryOps, new(_rp) Position(start), lexer->getPosition(_rp));
    prefixExpression->parent = simpleExpression;
    if (binaryOps) {
        size_t _binaryOps_length = binaryOps->length();
        for (size_t _i = 0; _i < _binaryOps_length; _i++)
            (*(*binaryOps)[_i])->parent = simpleExpression;
    }
    return _Result<SimpleExpression, ParserError>(simpleExpression);
}

_Result<_Vector<PathIdentifier>, ParserError> Parser::parsePathIdentifierList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<PathIdentifier>* pathIdentifier = 0;
    while (true) {
        _Result<PathIdentifier, ParserError> nodeResult = parsePathIdentifier(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!pathIdentifier)
                pathIdentifier = new(_p) _Array<PathIdentifier>();
            pathIdentifier->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<PathIdentifier>, ParserError>(pathIdentifier ? &_Vector<PathIdentifier>::create(_rp, *pathIdentifier) : 0);
}

_Result<PathIdentifier, ParserError> Parser::parsePathIdentifier(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDot1 = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1) {
        lexer->advance();
    }
    else {
        return _Result<PathIdentifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot1), &String::create(_ep, dot))));
    }
    auto _extension_result = parsePathItem(_rp, _ep);
    PathItem* extension;
    if (_extension_result.succeeded())
        extension = _extension_result.getResult();
    else
        return _Result<PathIdentifier, ParserError>(_extension_result.getError());
    PathIdentifier* pathIdentifier = new(_rp) PathIdentifier(extension, new(_rp) Position(start), lexer->getPosition(_rp));
    extension->parent = pathIdentifier;
    return _Result<PathIdentifier, ParserError>(pathIdentifier);
}

_Result<PathItem, ParserError> Parser::parsePathItem(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<PathItem, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    PathItem* pathItem = new(_rp) PathItem(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<PathItem, ParserError>(pathItem);
}

_Result<Initializer, ParserError> Parser::parseInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEqual1 = lexer->getPreviousPosition(_p);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1) {
        lexer->advance();
    }
    else {
        return _Result<Initializer, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startEqual1), &String::create(_ep, equal))));
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<Initializer, ParserError>(_expression_result.getError());
    Initializer* initializer = new(_rp) Initializer(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = initializer;
    return _Result<Initializer, ParserError>(initializer);
}

_Result<BindingInitializer, ParserError> Parser::parseBindingInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _initializer_result = parsePatternInitializer(_rp, _ep);
    PatternInitializer* initializer;
    if (_initializer_result.succeeded())
        initializer = _initializer_result.getResult();
    else
        return _Result<BindingInitializer, ParserError>(_initializer_result.getError());
    auto _additionalInitializers_result = parseAdditionalInitializerList(_rp, _ep);
    _Vector<AdditionalInitializer>* additionalInitializers;
    if (_additionalInitializers_result.succeeded())
        additionalInitializers = _additionalInitializers_result.getResult();
    else
        additionalInitializers = nullptr;
    BindingInitializer* bindingInitializer = new(_rp) BindingInitializer(initializer, additionalInitializers, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = bindingInitializer;
    if (additionalInitializers) {
        size_t _additionalInitializers_length = additionalInitializers->length();
        for (size_t _i = 0; _i < _additionalInitializers_length; _i++)
            (*(*additionalInitializers)[_i])->parent = bindingInitializer;
    }
    return _Result<BindingInitializer, ParserError>(bindingInitializer);
}

_Result<_Vector<PatternInitializer>, ParserError> Parser::parsePatternInitializerList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<PatternInitializer>* patternInitializer = 0;
    while (true) {
        _Result<PatternInitializer, ParserError> nodeResult = parsePatternInitializer(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!patternInitializer)
                patternInitializer = new(_p) _Array<PatternInitializer>();
            patternInitializer->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<PatternInitializer>, ParserError>(patternInitializer ? &_Vector<PatternInitializer>::create(_rp, *patternInitializer) : 0);
}

_Result<PatternInitializer, ParserError> Parser::parsePatternInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<PatternInitializer, ParserError>(_pattern_result.getError());
    auto _initializer_result = parseInitializer(_rp, _ep);
    Initializer* initializer;
    if (_initializer_result.succeeded())
        initializer = _initializer_result.getResult();
    else
        initializer = nullptr;
    PatternInitializer* patternInitializer = new(_rp) PatternInitializer(pattern, initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = patternInitializer;
    if (initializer)
        initializer->parent = patternInitializer;
    return _Result<PatternInitializer, ParserError>(patternInitializer);
}

_Result<_Vector<AdditionalInitializer>, ParserError> Parser::parseAdditionalInitializerList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<AdditionalInitializer>* additionalInitializer = 0;
    while (true) {
        _Result<AdditionalInitializer, ParserError> nodeResult = parseAdditionalInitializer(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!additionalInitializer)
                additionalInitializer = new(_p) _Array<AdditionalInitializer>();
            additionalInitializer->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<AdditionalInitializer>, ParserError>(additionalInitializer ? &_Vector<AdditionalInitializer>::create(_rp, *additionalInitializer) : 0);
}

_Result<AdditionalInitializer, ParserError> Parser::parseAdditionalInitializer(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startComma1 = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1) {
        lexer->advance();
    }
    else {
        return _Result<AdditionalInitializer, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startComma1), &String::create(_ep, comma))));
    }
    auto _pattern_result = parsePatternInitializer(_rp, _ep);
    PatternInitializer* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<AdditionalInitializer, ParserError>(_pattern_result.getError());
    AdditionalInitializer* additionalInitializer = new(_rp) AdditionalInitializer(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = additionalInitializer;
    return _Result<AdditionalInitializer, ParserError>(additionalInitializer);
}

_Result<_Vector<Modifier>, ParserError> Parser::parseModifierList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Modifier>* modifier = 0;
    while (true) {
        _Result<Modifier, ParserError> nodeResult = parseModifier(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!modifier)
                modifier = new(_p) _Array<Modifier>();
            modifier->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<Modifier>, ParserError>(modifier ? &_Vector<Modifier>::create(_rp, *modifier) : 0);
}

_Result<Modifier, ParserError> Parser::parseModifier(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<OverrideWord, ParserError> result = parseOverrideWord(_rp, _p);
        if (result.succeeded())
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<StaticWord, ParserError> result = parseStaticWord(_rp, _p);
        if (result.succeeded())
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Modifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<OverrideWord, ParserError> Parser::parseOverrideWord(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startOverride1 = lexer->getPreviousPosition(_p);
    bool successOverride1 = lexer->parseKeyword(overrideKeyword);
    if (successOverride1) {
        lexer->advance();
    }
    else {
        return _Result<OverrideWord, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startOverride1), &String::create(_ep, overrideKeyword))));
    }
    OverrideWord* overrideWord = new(_rp) OverrideWord(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<OverrideWord, ParserError>(overrideWord);
}

_Result<StaticWord, ParserError> Parser::parseStaticWord(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startStatic1 = lexer->getPreviousPosition(_p);
    bool successStatic1 = lexer->parseKeyword(staticKeyword);
    if (successStatic1) {
        lexer->advance();
    }
    else {
        return _Result<StaticWord, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startStatic1), &String::create(_ep, staticKeyword))));
    }
    StaticWord* staticWord = new(_rp) StaticWord(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<StaticWord, ParserError>(staticWord);
}

_Result<FunctionName, ParserError> Parser::parseFunctionName(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<IdentifierFunction, ParserError> result = parseIdentifierFunction(_rp, _p);
        if (result.succeeded())
            return _Result<FunctionName, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<FunctionName, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<IdentifierFunction, ParserError> Parser::parseIdentifierFunction(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<IdentifierFunction, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    IdentifierFunction* identifierFunction = new(_rp) IdentifierFunction(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<IdentifierFunction, ParserError>(identifierFunction);
}

_Result<FunctionSignature, ParserError> Parser::parseFunctionSignature(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _parameterClause_result = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause;
    if (_parameterClause_result.succeeded())
        parameterClause = _parameterClause_result.getResult();
    else
        return _Result<FunctionSignature, ParserError>(_parameterClause_result.getError());
    auto _result_result = parseFunctionResult(_rp, _ep);
    FunctionResult* result;
    if (_result_result.succeeded())
        result = _result_result.getResult();
    else
        result = nullptr;
    auto _throwsClause_result = parseThrowsClause(_rp, _ep);
    ThrowsClause* throwsClause;
    if (_throwsClause_result.succeeded())
        throwsClause = _throwsClause_result.getResult();
    else
        throwsClause = nullptr;
    FunctionSignature* functionSignature = new(_rp) FunctionSignature(parameterClause, result, throwsClause, new(_rp) Position(start), lexer->getPosition(_rp));
    parameterClause->parent = functionSignature;
    if (result)
        result->parent = functionSignature;
    if (throwsClause)
        throwsClause->parent = functionSignature;
    return _Result<FunctionSignature, ParserError>(functionSignature);
}

_Result<FunctionResult, ParserError> Parser::parseFunctionResult(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEvaluatesTo1 = lexer->getPreviousPosition(_p);
    bool successEvaluatesTo1 = lexer->parsePunctuation(evaluatesTo);
    if (successEvaluatesTo1) {
        lexer->advance();
    }
    else {
        return _Result<FunctionResult, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startEvaluatesTo1), &String::create(_ep, evaluatesTo))));
    }
    auto _existingObject_result = parseExistingClause(_rp, _ep);
    ExistingClause* existingObject;
    if (_existingObject_result.succeeded())
        existingObject = _existingObject_result.getResult();
    else
        existingObject = nullptr;
    auto _resultType_result = parseType(_rp, _ep);
    Type* resultType;
    if (_resultType_result.succeeded())
        resultType = _resultType_result.getResult();
    else
        return _Result<FunctionResult, ParserError>(_resultType_result.getError());
    FunctionResult* functionResult = new(_rp) FunctionResult(existingObject, resultType, new(_rp) Position(start), lexer->getPosition(_rp));
    if (existingObject)
        existingObject->parent = functionResult;
    resultType->parent = functionResult;
    return _Result<FunctionResult, ParserError>(functionResult);
}

_Result<ExistingClause, ParserError> Parser::parseExistingClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startExisting1 = lexer->getPreviousPosition(_p);
    bool successExisting1 = lexer->parseKeyword(existingKeyword);
    if (successExisting1) {
        lexer->advance();
    }
    else {
        return _Result<ExistingClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startExisting1), &String::create(_ep, existingKeyword))));
    }
    ExistingClause* existingClause = new(_rp) ExistingClause(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<ExistingClause, ParserError>(existingClause);
}

_Result<_Vector<ParameterClause>, ParserError> Parser::parseParameterClauseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParameterClause>* parameterClause = 0;
    while (true) {
        _Result<ParameterClause, ParserError> nodeResult = parseParameterClause(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!parameterClause)
                parameterClause = new(_p) _Array<ParameterClause>();
            parameterClause->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<ParameterClause>, ParserError>(parameterClause ? &_Vector<ParameterClause>::create(_rp, *parameterClause) : 0);
}

_Result<ParameterClause, ParserError> Parser::parseParameterClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftParen1 = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1) {
        lexer->advance();
    }
    else {
        return _Result<ParameterClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftParen1), &String::create(_ep, leftParen))));
    }
    auto _parameters_result = parseParameterList(_rp, _ep);
    _Vector<Parameter>* parameters;
    if (_parameters_result.succeeded())
        parameters = _parameters_result.getResult();
    else
        parameters = nullptr;
    Position* startRightParen3 = lexer->getPreviousPosition(_p);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3) {
        lexer->advance();
    }
    else {
        return _Result<ParameterClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightParen3), &String::create(_ep, rightParen))));
    }
    ParameterClause* parameterClause = new(_rp) ParameterClause(parameters, new(_rp) Position(start), lexer->getPosition(_rp));
    if (parameters) {
        size_t _parameters_length = parameters->length();
        for (size_t _i = 0; _i < _parameters_length; _i++)
            (*(*parameters)[_i])->parent = parameterClause;
    }
    return _Result<ParameterClause, ParserError>(parameterClause);
}

_Result<_Vector<Parameter>, ParserError> Parser::parseParameterList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Parameter>* parameter = 0;
    while (true) {
        _Result<Parameter, ParserError> nodeResult = parseParameter(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!parameter)
                parameter = new(_p) _Array<Parameter>();
            parameter->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<Parameter>, ParserError>(parameter ? &_Vector<Parameter>::create(_rp, *parameter) : 0);
}

_Result<Parameter, ParserError> Parser::parseParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ConstParameter, ParserError> result = parseConstParameter(_rp, _p);
        if (result.succeeded())
            return _Result<Parameter, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<VarParameter, ParserError> result = parseVarParameter(_rp, _p);
        if (result.succeeded())
            return _Result<Parameter, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Parameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<ConstParameter, ParserError> Parser::parseConstParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1) {
        lexer->advance();
    }
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<ConstParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    Position* startColon3 = lexer->getPreviousPosition(_p);
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3) {
        lexer->advance();
    }
    else {
        return _Result<ConstParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon3), &String::create(_ep, colon))));
    }
    auto _parameterType_result = parseType(_rp, _ep);
    Type* parameterType;
    if (_parameterType_result.succeeded())
        parameterType = _parameterType_result.getResult();
    else
        return _Result<ConstParameter, ParserError>(_parameterType_result.getError());
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5) {
        lexer->advance();
    }
    ConstParameter* constParameter = new(_rp) ConstParameter(name, parameterType, new(_rp) Position(start), lexer->getPosition(_rp));
    parameterType->parent = constParameter;
    return _Result<ConstParameter, ParserError>(constParameter);
}

_Result<VarParameter, ParserError> Parser::parseVarParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startMutable1 = lexer->getPreviousPosition(_p);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1) {
        lexer->advance();
    }
    else {
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startMutable1), &String::create(_ep, mutableKeyword))));
    }
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    Position* startColon3 = lexer->getPreviousPosition(_p);
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3) {
        lexer->advance();
    }
    else {
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon3), &String::create(_ep, colon))));
    }
    auto _parameterType_result = parseType(_rp, _ep);
    Type* parameterType;
    if (_parameterType_result.succeeded())
        parameterType = _parameterType_result.getResult();
    else
        return _Result<VarParameter, ParserError>(_parameterType_result.getError());
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5) {
        lexer->advance();
    }
    VarParameter* varParameter = new(_rp) VarParameter(name, parameterType, new(_rp) Position(start), lexer->getPosition(_rp));
    parameterType->parent = varParameter;
    return _Result<VarParameter, ParserError>(varParameter);
}

_Result<ThrowsClause, ParserError> Parser::parseThrowsClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startThrows1 = lexer->getPreviousPosition(_p);
    bool successThrows1 = lexer->parseKeyword(throwsKeyword);
    if (successThrows1) {
        lexer->advance();
    }
    else {
        return _Result<ThrowsClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startThrows1), &String::create(_ep, throwsKeyword))));
    }
    auto _throwsType_result = parseType(_rp, _ep);
    Type* throwsType;
    if (_throwsType_result.succeeded())
        throwsType = _throwsType_result.getResult();
    else
        return _Result<ThrowsClause, ParserError>(_throwsType_result.getError());
    ThrowsClause* throwsClause = new(_rp) ThrowsClause(throwsType, new(_rp) Position(start), lexer->getPosition(_rp));
    throwsType->parent = throwsClause;
    return _Result<ThrowsClause, ParserError>(throwsClause);
}

_Result<_Vector<EnumMember>, ParserError> Parser::parseEnumMemberList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<EnumMember>* enumMember = 0;
    while (true) {
        _Result<EnumMember, ParserError> nodeResult = parseEnumMember(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!enumMember)
                enumMember = new(_p) _Array<EnumMember>();
            enumMember->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<EnumMember>, ParserError>(enumMember ? &_Vector<EnumMember>::create(_rp, *enumMember) : 0);
}

_Result<EnumMember, ParserError> Parser::parseEnumMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startCase1 = lexer->getPreviousPosition(_p);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1) {
        lexer->advance();
    }
    else {
        return _Result<EnumMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startCase1), &String::create(_ep, caseKeyword))));
    }
    auto _enumCase_result = parseEnumCase(_rp, _ep);
    EnumCase* enumCase;
    if (_enumCase_result.succeeded())
        enumCase = _enumCase_result.getResult();
    else
        return _Result<EnumMember, ParserError>(_enumCase_result.getError());
    auto _additionalCases_result = parseAdditionalCaseList(_rp, _ep);
    _Vector<AdditionalCase>* additionalCases;
    if (_additionalCases_result.succeeded())
        additionalCases = _additionalCases_result.getResult();
    else
        additionalCases = nullptr;
    auto _parameterClause_result = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause;
    if (_parameterClause_result.succeeded())
        parameterClause = _parameterClause_result.getResult();
    else
        parameterClause = nullptr;
    EnumMember* enumMember = new(_rp) EnumMember(enumCase, additionalCases, parameterClause, new(_rp) Position(start), lexer->getPosition(_rp));
    enumCase->parent = enumMember;
    if (additionalCases) {
        size_t _additionalCases_length = additionalCases->length();
        for (size_t _i = 0; _i < _additionalCases_length; _i++)
            (*(*additionalCases)[_i])->parent = enumMember;
    }
    if (parameterClause)
        parameterClause->parent = enumMember;
    return _Result<EnumMember, ParserError>(enumMember);
}

_Result<EnumCase, ParserError> Parser::parseEnumCase(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<EnumCase, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    EnumCase* enumCase = new(_rp) EnumCase(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<EnumCase, ParserError>(enumCase);
}

_Result<_Vector<AdditionalCase>, ParserError> Parser::parseAdditionalCaseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<AdditionalCase>* additionalCase = 0;
    while (true) {
        _Result<AdditionalCase, ParserError> nodeResult = parseAdditionalCase(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!additionalCase)
                additionalCase = new(_p) _Array<AdditionalCase>();
            additionalCase->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<AdditionalCase>, ParserError>(additionalCase ? &_Vector<AdditionalCase>::create(_rp, *additionalCase) : 0);
}

_Result<AdditionalCase, ParserError> Parser::parseAdditionalCase(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startComma1 = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1) {
        lexer->advance();
    }
    else {
        return _Result<AdditionalCase, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startComma1), &String::create(_ep, comma))));
    }
    auto _enumCase_result = parseEnumCase(_rp, _ep);
    EnumCase* enumCase;
    if (_enumCase_result.succeeded())
        enumCase = _enumCase_result.getResult();
    else
        return _Result<AdditionalCase, ParserError>(_enumCase_result.getError());
    AdditionalCase* additionalCase = new(_rp) AdditionalCase(enumCase, new(_rp) Position(start), lexer->getPosition(_rp));
    enumCase->parent = additionalCase;
    return _Result<AdditionalCase, ParserError>(additionalCase);
}

_Result<ClassBody, ParserError> Parser::parseClassBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1) {
        lexer->advance();
    }
    else {
        return _Result<ClassBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly1), &String::create(_ep, leftCurly))));
    }
    auto _members_result = parseClassMemberList(_rp, _ep);
    _Vector<ClassMember>* members;
    if (_members_result.succeeded())
        members = _members_result.getResult();
    else
        members = nullptr;
    Position* startRightCurly3 = lexer->getPreviousPosition(_p);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3) {
        lexer->advance();
    }
    else {
        return _Result<ClassBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly3), &String::create(_ep, rightCurly))));
    }
    ClassBody* classBody = new(_rp) ClassBody(members, new(_rp) Position(start), lexer->getPosition(_rp));
    if (members) {
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++)
            (*(*members)[_i])->parent = classBody;
    }
    return _Result<ClassBody, ParserError>(classBody);
}

_Result<GenericArgumentClause, ParserError> Parser::parseGenericArgumentClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftAngular1 = lexer->getPreviousPosition(_p);
    bool successLeftAngular1 = lexer->parsePunctuation(leftAngular);
    if (successLeftAngular1) {
        lexer->advance();
    }
    else {
        return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftAngular1), &String::create(_ep, leftAngular))));
    }
    auto _genericParameters_result = parseGenericParameterList(_rp, _ep);
    _Vector<GenericParameter>* genericParameters;
    if (_genericParameters_result.succeeded())
        genericParameters = _genericParameters_result.getResult();
    else
        return _Result<GenericArgumentClause, ParserError>(_genericParameters_result.getError());
    Position* startRightAngular3 = lexer->getPreviousPosition(_p);
    bool successRightAngular3 = lexer->parsePunctuation(rightAngular);
    if (successRightAngular3) {
        lexer->advance();
    }
    else {
        return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightAngular3), &String::create(_ep, rightAngular))));
    }
    GenericArgumentClause* genericArgumentClause = new(_rp) GenericArgumentClause(genericParameters, new(_rp) Position(start), lexer->getPosition(_rp));
    if (genericParameters) {
        size_t _genericParameters_length = genericParameters->length();
        for (size_t _i = 0; _i < _genericParameters_length; _i++)
            (*(*genericParameters)[_i])->parent = genericArgumentClause;
    }
    return _Result<GenericArgumentClause, ParserError>(genericArgumentClause);
}

_Result<_Vector<GenericParameter>, ParserError> Parser::parseGenericParameterList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<GenericParameter>* genericParameter = 0;
    while (true) {
        _Result<GenericParameter, ParserError> nodeResult = parseGenericParameter(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!genericParameter)
                genericParameter = new(_p) _Array<GenericParameter>();
            genericParameter->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<GenericParameter>, ParserError>(genericParameter ? &_Vector<GenericParameter>::create(_rp, *genericParameter) : 0);
}

_Result<GenericParameter, ParserError> Parser::parseGenericParameter(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startTypeName = lexer->getPreviousPosition(_p);
    String* typeName = lexer->parseIdentifier(_rp);
    if ((typeName) && (isIdentifier(typeName))) {
        lexer->advance();
    }
    else {
        return _Result<GenericParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startTypeName))));
    }
    GenericParameter* genericParameter = new(_rp) GenericParameter(typeName, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<GenericParameter, ParserError>(genericParameter);
}

_Result<_Vector<ClassMember>, ParserError> Parser::parseClassMemberList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ClassMember>* classMember = 0;
    while (true) {
        _Result<ClassMember, ParserError> nodeResult = parseClassMember(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!classMember)
                classMember = new(_p) _Array<ClassMember>();
            classMember->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<ClassMember>, ParserError>(classMember ? &_Vector<ClassMember>::create(_rp, *classMember) : 0);
}

_Result<ClassMember, ParserError> Parser::parseClassMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _declaration_result = parseDeclaration(_rp, _ep);
    Declaration* declaration;
    if (_declaration_result.succeeded())
        declaration = _declaration_result.getResult();
    else
        return _Result<ClassMember, ParserError>(_declaration_result.getError());
    bool successSemicolon2 = lexer->parsePunctuation(semicolon);
    if (successSemicolon2) {
        lexer->advance();
    }
    ClassMember* classMember = new(_rp) ClassMember(declaration, new(_rp) Position(start), lexer->getPosition(_rp));
    declaration->parent = classMember;
    return _Result<ClassMember, ParserError>(classMember);
}

_Result<PrefixExpression, ParserError> Parser::parsePrefixExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    String* prefixOperator = lexer->parsePrefixOperator(_rp);
    if (prefixOperator) {
        lexer->advance();
    }
    auto _expression_result = parsePostfixExpression(_rp, _ep);
    PostfixExpression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<PrefixExpression, ParserError>(_expression_result.getError());
    PrefixExpression* prefixExpression = new(_rp) PrefixExpression(prefixOperator, expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = prefixExpression;
    return _Result<PrefixExpression, ParserError>(prefixExpression);
}

_Result<PostfixExpression, ParserError> Parser::parsePostfixExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _primaryExpression_result = parsePrimaryExpression(_rp, _ep);
    PrimaryExpression* primaryExpression;
    if (_primaryExpression_result.succeeded())
        primaryExpression = _primaryExpression_result.getResult();
    else
        return _Result<PostfixExpression, ParserError>(_primaryExpression_result.getError());
    auto _postfixes_result = parsePostfixList(_rp, _ep);
    _Vector<Postfix>* postfixes;
    if (_postfixes_result.succeeded())
        postfixes = _postfixes_result.getResult();
    else
        postfixes = nullptr;
    PostfixExpression* postfixExpression = new(_rp) PostfixExpression(primaryExpression, postfixes, new(_rp) Position(start), lexer->getPosition(_rp));
    primaryExpression->parent = postfixExpression;
    if (postfixes) {
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++)
            (*(*postfixes)[_i])->parent = postfixExpression;
    }
    return _Result<PostfixExpression, ParserError>(postfixExpression);
}

_Result<_Vector<BinaryOp>, ParserError> Parser::parseBinaryOpList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<BinaryOp>* binaryOp = 0;
    while (true) {
        _Result<BinaryOp, ParserError> nodeResult = parseBinaryOp(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!binaryOp)
                binaryOp = new(_p) _Array<BinaryOp>();
            binaryOp->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<BinaryOp>, ParserError>(binaryOp ? &_Vector<BinaryOp>::create(_rp, *binaryOp) : 0);
}

_Result<BinaryOp, ParserError> Parser::parseBinaryOp(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<BinaryOperation, ParserError> result = parseBinaryOperation(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<Assignment, ParserError> result = parseAssignment(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<TypeQuery, ParserError> result = parseTypeQuery(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<TypeCast, ParserError> result = parseTypeCast(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<BinaryOp, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<BinaryOperation, ParserError> Parser::parseBinaryOperation(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startBinaryOperator = lexer->getPreviousPosition(_p);
    String* binaryOperator = lexer->parseBinaryOperator(_rp);
    if (binaryOperator) {
        lexer->advance();
    }
    else {
        return _Result<BinaryOperation, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_operatorExpected(new(_ep) Position(startBinaryOperator))));
    }
    auto _expression_result = parsePrefixExpression(_rp, _ep);
    PrefixExpression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<BinaryOperation, ParserError>(_expression_result.getError());
    BinaryOperation* binaryOperation = new(_rp) BinaryOperation(binaryOperator, expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = binaryOperation;
    return _Result<BinaryOperation, ParserError>(binaryOperation);
}

_Result<Assignment, ParserError> Parser::parseAssignment(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startEqual1 = lexer->getPreviousPosition(_p);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1) {
        lexer->advance();
    }
    else {
        return _Result<Assignment, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startEqual1), &String::create(_ep, equal))));
    }
    auto _expression_result = parsePrefixExpression(_rp, _ep);
    PrefixExpression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<Assignment, ParserError>(_expression_result.getError());
    Assignment* assignment = new(_rp) Assignment(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = assignment;
    return _Result<Assignment, ParserError>(assignment);
}

_Result<TypeQuery, ParserError> Parser::parseTypeQuery(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startIs1 = lexer->getPreviousPosition(_p);
    bool successIs1 = lexer->parseKeyword(isKeyword);
    if (successIs1) {
        lexer->advance();
    }
    else {
        return _Result<TypeQuery, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startIs1), &String::create(_ep, isKeyword))));
    }
    auto _objectType_result = parseType(_rp, _ep);
    Type* objectType;
    if (_objectType_result.succeeded())
        objectType = _objectType_result.getResult();
    else
        return _Result<TypeQuery, ParserError>(_objectType_result.getError());
    TypeQuery* typeQuery = new(_rp) TypeQuery(objectType, new(_rp) Position(start), lexer->getPosition(_rp));
    objectType->parent = typeQuery;
    return _Result<TypeQuery, ParserError>(typeQuery);
}

_Result<TypeCast, ParserError> Parser::parseTypeCast(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startAs1 = lexer->getPreviousPosition(_p);
    bool successAs1 = lexer->parseKeyword(asKeyword);
    if (successAs1) {
        lexer->advance();
    }
    else {
        return _Result<TypeCast, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startAs1), &String::create(_ep, asKeyword))));
    }
    auto _objectType_result = parseType(_rp, _ep);
    Type* objectType;
    if (_objectType_result.succeeded())
        objectType = _objectType_result.getResult();
    else
        return _Result<TypeCast, ParserError>(_objectType_result.getError());
    TypeCast* typeCast = new(_rp) TypeCast(objectType, new(_rp) Position(start), lexer->getPosition(_rp));
    objectType->parent = typeCast;
    return _Result<TypeCast, ParserError>(typeCast);
}

_Result<_Vector<CatchClause>, ParserError> Parser::parseCatchClauseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<CatchClause>* catchClause = 0;
    while (true) {
        _Result<CatchClause, ParserError> nodeResult = parseCatchClause(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!catchClause)
                catchClause = new(_p) _Array<CatchClause>();
            catchClause->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<CatchClause>, ParserError>(catchClause ? &_Vector<CatchClause>::create(_rp, *catchClause) : 0);
}

_Result<CatchClause, ParserError> Parser::parseCatchClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startCatch1 = lexer->getPreviousPosition(_p);
    bool successCatch1 = lexer->parseKeyword(catchKeyword);
    if (successCatch1) {
        lexer->advance();
    }
    else {
        return _Result<CatchClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startCatch1), &String::create(_ep, catchKeyword))));
    }
    auto _catchPattern_result = parseCatchPattern(_rp, _ep);
    CatchPattern* catchPattern;
    if (_catchPattern_result.succeeded())
        catchPattern = _catchPattern_result.getResult();
    else
        return _Result<CatchClause, ParserError>(_catchPattern_result.getError());
    auto _bindingPattern_result = parseTuplePattern(_rp, _ep);
    TuplePattern* bindingPattern;
    if (_bindingPattern_result.succeeded())
        bindingPattern = _bindingPattern_result.getResult();
    else
        bindingPattern = nullptr;
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<CatchClause, ParserError>(_expression_result.getError());
    CatchClause* catchClause = new(_rp) CatchClause(catchPattern, bindingPattern, expression, new(_rp) Position(start), lexer->getPosition(_rp));
    catchPattern->parent = catchClause;
    if (bindingPattern)
        bindingPattern->parent = catchClause;
    expression->parent = catchClause;
    return _Result<CatchClause, ParserError>(catchClause);
}

_Result<CatchPattern, ParserError> Parser::parseCatchPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<WildCardCatchPattern, ParserError> result = parseWildCardCatchPattern(_rp, _p);
        if (result.succeeded())
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<PathItemCatchPattern, ParserError> result = parsePathItemCatchPattern(_rp, _p);
        if (result.succeeded())
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CatchPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<WildCardCatchPattern, ParserError> Parser::parseWildCardCatchPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _pattern_result = parseWildcardPattern(_rp, _ep);
    WildcardPattern* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<WildCardCatchPattern, ParserError>(_pattern_result.getError());
    WildCardCatchPattern* wildCardCatchPattern = new(_rp) WildCardCatchPattern(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = wildCardCatchPattern;
    return _Result<WildCardCatchPattern, ParserError>(wildCardCatchPattern);
}

_Result<PathItemCatchPattern, ParserError> Parser::parsePathItemCatchPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _catchCase_result = parsePathItem(_rp, _ep);
    PathItem* catchCase;
    if (_catchCase_result.succeeded())
        catchCase = _catchCase_result.getResult();
    else
        return _Result<PathItemCatchPattern, ParserError>(_catchCase_result.getError());
    auto _catchCaseExtensions_result = parsePathIdentifierList(_rp, _ep);
    _Vector<PathIdentifier>* catchCaseExtensions;
    if (_catchCaseExtensions_result.succeeded())
        catchCaseExtensions = _catchCaseExtensions_result.getResult();
    else
        catchCaseExtensions = nullptr;
    PathItemCatchPattern* pathItemCatchPattern = new(_rp) PathItemCatchPattern(catchCase, catchCaseExtensions, new(_rp) Position(start), lexer->getPosition(_rp));
    catchCase->parent = pathItemCatchPattern;
    if (catchCaseExtensions) {
        size_t _catchCaseExtensions_length = catchCaseExtensions->length();
        for (size_t _i = 0; _i < _catchCaseExtensions_length; _i++)
            (*(*catchCaseExtensions)[_i])->parent = pathItemCatchPattern;
    }
    return _Result<PathItemCatchPattern, ParserError>(pathItemCatchPattern);
}

_Result<_Vector<Postfix>, ParserError> Parser::parsePostfixList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Postfix>* postfix = 0;
    while (true) {
        _Result<Postfix, ParserError> nodeResult = parsePostfix(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!postfix)
                postfix = new(_p) _Array<Postfix>();
            postfix->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<Postfix>, ParserError>(postfix ? &_Vector<Postfix>::create(_rp, *postfix) : 0);
}

_Result<Postfix, ParserError> Parser::parsePostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<OperatorPostfix, ParserError> result = parseOperatorPostfix(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<FunctionCall, ParserError> result = parseFunctionCall(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ExplicitMemberExpression, ParserError> result = parseExplicitMemberExpression(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<Subscript, ParserError> result = parseSubscript(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Postfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<OperatorPostfix, ParserError> Parser::parseOperatorPostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startPostfixOperator = lexer->getPreviousPosition(_p);
    String* postfixOperator = lexer->parsePostfixOperator(_rp);
    if (postfixOperator) {
        lexer->advance();
    }
    else {
        return _Result<OperatorPostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_operatorExpected(new(_ep) Position(startPostfixOperator))));
    }
    OperatorPostfix* operatorPostfix = new(_rp) OperatorPostfix(postfixOperator, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<OperatorPostfix, ParserError>(operatorPostfix);
}

_Result<FunctionCall, ParserError> Parser::parseFunctionCall(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _arguments_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments;
    if (_arguments_result.succeeded())
        arguments = _arguments_result.getResult();
    else
        return _Result<FunctionCall, ParserError>(_arguments_result.getError());
    auto _catchClauses_result = parseCatchClauseList(_rp, _ep);
    _Vector<CatchClause>* catchClauses;
    if (_catchClauses_result.succeeded())
        catchClauses = _catchClauses_result.getResult();
    else
        catchClauses = nullptr;
    FunctionCall* functionCall = new(_rp) FunctionCall(arguments, catchClauses, new(_rp) Position(start), lexer->getPosition(_rp));
    arguments->parent = functionCall;
    if (catchClauses) {
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++)
            (*(*catchClauses)[_i])->parent = functionCall;
    }
    return _Result<FunctionCall, ParserError>(functionCall);
}

_Result<ExplicitMemberExpression, ParserError> Parser::parseExplicitMemberExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDot1 = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1) {
        lexer->advance();
    }
    else {
        return _Result<ExplicitMemberExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot1), &String::create(_ep, dot))));
    }
    auto _memberPostfix_result = parseMemberPostfix(_rp, _ep);
    MemberPostfix* memberPostfix;
    if (_memberPostfix_result.succeeded())
        memberPostfix = _memberPostfix_result.getResult();
    else
        return _Result<ExplicitMemberExpression, ParserError>(_memberPostfix_result.getError());
    ExplicitMemberExpression* explicitMemberExpression = new(_rp) ExplicitMemberExpression(memberPostfix, new(_rp) Position(start), lexer->getPosition(_rp));
    memberPostfix->parent = explicitMemberExpression;
    return _Result<ExplicitMemberExpression, ParserError>(explicitMemberExpression);
}

_Result<Subscript, ParserError> Parser::parseSubscript(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftBracket1 = lexer->getPreviousPosition(_p);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1) {
        lexer->advance();
    }
    else {
        return _Result<Subscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftBracket1), &String::create(_ep, leftBracket))));
    }
    auto _expressions_result = parseExpressionElementList(_rp, _ep);
    _Vector<ExpressionElement>* expressions;
    if (_expressions_result.succeeded())
        expressions = _expressions_result.getResult();
    else
        return _Result<Subscript, ParserError>(_expressions_result.getError());
    Position* startRightBracket3 = lexer->getPreviousPosition(_p);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3) {
        lexer->advance();
    }
    else {
        return _Result<Subscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightBracket3), &String::create(_ep, rightBracket))));
    }
    Subscript* subscript = new(_rp) Subscript(expressions, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expressions) {
        size_t _expressions_length = expressions->length();
        for (size_t _i = 0; _i < _expressions_length; _i++)
            (*(*expressions)[_i])->parent = subscript;
    }
    return _Result<Subscript, ParserError>(subscript);
}

_Result<_Vector<ExpressionElement>, ParserError> Parser::parseExpressionElementList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ExpressionElement>* expressionElement = 0;
    while (true) {
        _Result<ExpressionElement, ParserError> nodeResult = parseExpressionElement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!expressionElement)
                expressionElement = new(_p) _Array<ExpressionElement>();
            expressionElement->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<ExpressionElement>, ParserError>(expressionElement ? &_Vector<ExpressionElement>::create(_rp, *expressionElement) : 0);
}

_Result<ExpressionElement, ParserError> Parser::parseExpressionElement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<ExpressionElement, ParserError>(_expression_result.getError());
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2) {
        lexer->advance();
    }
    ExpressionElement* expressionElement = new(_rp) ExpressionElement(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = expressionElement;
    return _Result<ExpressionElement, ParserError>(expressionElement);
}

_Result<MemberPostfix, ParserError> Parser::parseMemberPostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<NamedMemberPostfix, ParserError> result = parseNamedMemberPostfix(_rp, _p);
        if (result.succeeded())
            return _Result<MemberPostfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<MemberPostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<NamedMemberPostfix, ParserError> Parser::parseNamedMemberPostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _identifier_result = parseIdentifierExpression(_rp, _ep);
    IdentifierExpression* identifier;
    if (_identifier_result.succeeded())
        identifier = _identifier_result.getResult();
    else
        return _Result<NamedMemberPostfix, ParserError>(_identifier_result.getError());
    NamedMemberPostfix* namedMemberPostfix = new(_rp) NamedMemberPostfix(identifier, new(_rp) Position(start), lexer->getPosition(_rp));
    identifier->parent = namedMemberPostfix;
    return _Result<NamedMemberPostfix, ParserError>(namedMemberPostfix);
}

_Result<PrimaryExpression, ParserError> Parser::parsePrimaryExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<IdentifierExpression, ParserError> result = parseIdentifierExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<LiteralExpression, ParserError> result = parseLiteralExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<IfExpression, ParserError> result = parseIfExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SwitchExpression, ParserError> result = parseSwitchExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ForExpression, ParserError> result = parseForExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<WhileExpression, ParserError> result = parseWhileExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<RepeatExpression, ParserError> result = parseRepeatExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ParenthesizedExpression, ParserError> result = parseParenthesizedExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ReturnExpression, ParserError> result = parseReturnExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ThrowExpression, ParserError> result = parseThrowExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<BreakExpression, ParserError> result = parseBreakExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<InitializerCall, ParserError> result = parseInitializerCall(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ThisExpression, ParserError> result = parseThisExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SuperExpression, ParserError> result = parseSuperExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<NullExpression, ParserError> result = parseNullExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<PrimaryExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<IdentifierExpression, ParserError> Parser::parseIdentifierExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<IdentifierExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    IdentifierExpression* identifierExpression = new(_rp) IdentifierExpression(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<IdentifierExpression, ParserError>(identifierExpression);
}

_Result<LiteralExpression, ParserError> Parser::parseLiteralExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLiteral = lexer->getPreviousPosition(_p);
    Literal* literal = lexer->parseLiteral(_rp);
    if (literal) {
        lexer->advance();
    }
    else {
        return _Result<LiteralExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_literalExpected(new(_ep) Position(startLiteral))));
    }
    LiteralExpression* literalExpression = new(_rp) LiteralExpression(literal, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<LiteralExpression, ParserError>(literalExpression);
}

_Result<IfExpression, ParserError> Parser::parseIfExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startIf1 = lexer->getPreviousPosition(_p);
    bool successIf1 = lexer->parseKeyword(ifKeyword);
    if (successIf1) {
        lexer->advance();
    }
    else {
        return _Result<IfExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startIf1), &String::create(_ep, ifKeyword))));
    }
    auto _condition_result = parseExpression(_rp, _ep);
    Expression* condition;
    if (_condition_result.succeeded())
        condition = _condition_result.getResult();
    else
        return _Result<IfExpression, ParserError>(_condition_result.getError());
    auto _consequent_result = parseExpression(_rp, _ep);
    Expression* consequent;
    if (_consequent_result.succeeded())
        consequent = _consequent_result.getResult();
    else
        return _Result<IfExpression, ParserError>(_consequent_result.getError());
    auto _elseClause_result = parseElseClause(_rp, _ep);
    ElseClause* elseClause;
    if (_elseClause_result.succeeded())
        elseClause = _elseClause_result.getResult();
    else
        elseClause = nullptr;
    IfExpression* ifExpression = new(_rp) IfExpression(condition, consequent, elseClause, new(_rp) Position(start), lexer->getPosition(_rp));
    condition->parent = ifExpression;
    consequent->parent = ifExpression;
    if (elseClause)
        elseClause->parent = ifExpression;
    return _Result<IfExpression, ParserError>(ifExpression);
}

_Result<SwitchExpression, ParserError> Parser::parseSwitchExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSwitch1 = lexer->getPreviousPosition(_p);
    bool successSwitch1 = lexer->parseKeyword(switchKeyword);
    if (successSwitch1) {
        lexer->advance();
    }
    else {
        return _Result<SwitchExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startSwitch1), &String::create(_ep, switchKeyword))));
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<SwitchExpression, ParserError>(_expression_result.getError());
    auto _body_result = parseSwitchBody(_rp, _ep);
    SwitchBody* body;
    if (_body_result.succeeded())
        body = _body_result.getResult();
    else
        return _Result<SwitchExpression, ParserError>(_body_result.getError());
    SwitchExpression* switchExpression = new(_rp) SwitchExpression(expression, body, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = switchExpression;
    body->parent = switchExpression;
    return _Result<SwitchExpression, ParserError>(switchExpression);
}

_Result<ForExpression, ParserError> Parser::parseForExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startFor1 = lexer->getPreviousPosition(_p);
    bool successFor1 = lexer->parseKeyword(forKeyword);
    if (successFor1) {
        lexer->advance();
    }
    else {
        return _Result<ForExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startFor1), &String::create(_ep, forKeyword))));
    }
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<ForExpression, ParserError>(_pattern_result.getError());
    Position* startIn3 = lexer->getPreviousPosition(_p);
    bool successIn3 = lexer->parseKeyword(inKeyword);
    if (successIn3) {
        lexer->advance();
    }
    else {
        return _Result<ForExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startIn3), &String::create(_ep, inKeyword))));
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<ForExpression, ParserError>(_expression_result.getError());
    auto _code_result = parseExpression(_rp, _ep);
    Expression* code;
    if (_code_result.succeeded())
        code = _code_result.getResult();
    else
        return _Result<ForExpression, ParserError>(_code_result.getError());
    ForExpression* forExpression = new(_rp) ForExpression(pattern, expression, code, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = forExpression;
    expression->parent = forExpression;
    code->parent = forExpression;
    return _Result<ForExpression, ParserError>(forExpression);
}

_Result<WhileExpression, ParserError> Parser::parseWhileExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startWhile1 = lexer->getPreviousPosition(_p);
    bool successWhile1 = lexer->parseKeyword(whileKeyword);
    if (successWhile1) {
        lexer->advance();
    }
    else {
        return _Result<WhileExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startWhile1), &String::create(_ep, whileKeyword))));
    }
    auto _condition_result = parseExpression(_rp, _ep);
    Expression* condition;
    if (_condition_result.succeeded())
        condition = _condition_result.getResult();
    else
        return _Result<WhileExpression, ParserError>(_condition_result.getError());
    auto _code_result = parseExpression(_rp, _ep);
    Expression* code;
    if (_code_result.succeeded())
        code = _code_result.getResult();
    else
        return _Result<WhileExpression, ParserError>(_code_result.getError());
    WhileExpression* whileExpression = new(_rp) WhileExpression(condition, code, new(_rp) Position(start), lexer->getPosition(_rp));
    condition->parent = whileExpression;
    code->parent = whileExpression;
    return _Result<WhileExpression, ParserError>(whileExpression);
}

_Result<RepeatExpression, ParserError> Parser::parseRepeatExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startRepeat1 = lexer->getPreviousPosition(_p);
    bool successRepeat1 = lexer->parseKeyword(repeatKeyword);
    if (successRepeat1) {
        lexer->advance();
    }
    else {
        return _Result<RepeatExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startRepeat1), &String::create(_ep, repeatKeyword))));
    }
    auto _code_result = parseExpression(_rp, _ep);
    Expression* code;
    if (_code_result.succeeded())
        code = _code_result.getResult();
    else
        return _Result<RepeatExpression, ParserError>(_code_result.getError());
    Position* startWhile3 = lexer->getPreviousPosition(_p);
    bool successWhile3 = lexer->parseKeyword(whileKeyword);
    if (successWhile3) {
        lexer->advance();
    }
    else {
        return _Result<RepeatExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startWhile3), &String::create(_ep, whileKeyword))));
    }
    auto _condition_result = parseExpression(_rp, _ep);
    Expression* condition;
    if (_condition_result.succeeded())
        condition = _condition_result.getResult();
    else
        return _Result<RepeatExpression, ParserError>(_condition_result.getError());
    RepeatExpression* repeatExpression = new(_rp) RepeatExpression(code, condition, new(_rp) Position(start), lexer->getPosition(_rp));
    code->parent = repeatExpression;
    condition->parent = repeatExpression;
    return _Result<RepeatExpression, ParserError>(repeatExpression);
}

_Result<ParenthesizedExpression, ParserError> Parser::parseParenthesizedExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftParen1 = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1) {
        lexer->advance();
    }
    else {
        return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftParen1), &String::create(_ep, leftParen))));
    }
    auto _expressionElements_result = parseExpressionElementList(_rp, _ep);
    _Vector<ExpressionElement>* expressionElements;
    if (_expressionElements_result.succeeded())
        expressionElements = _expressionElements_result.getResult();
    else
        expressionElements = nullptr;
    Position* startRightParen3 = lexer->getPreviousPosition(_p);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3) {
        lexer->advance();
    }
    else {
        return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightParen3), &String::create(_ep, rightParen))));
    }
    ParenthesizedExpression* parenthesizedExpression = new(_rp) ParenthesizedExpression(expressionElements, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expressionElements) {
        size_t _expressionElements_length = expressionElements->length();
        for (size_t _i = 0; _i < _expressionElements_length; _i++)
            (*(*expressionElements)[_i])->parent = parenthesizedExpression;
    }
    return _Result<ParenthesizedExpression, ParserError>(parenthesizedExpression);
}

_Result<ReturnExpression, ParserError> Parser::parseReturnExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startReturn1 = lexer->getPreviousPosition(_p);
    bool successReturn1 = lexer->parseKeyword(returnKeyword);
    if (successReturn1) {
        lexer->advance();
    }
    else {
        return _Result<ReturnExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startReturn1), &String::create(_ep, returnKeyword))));
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        expression = nullptr;
    ReturnExpression* returnExpression = new(_rp) ReturnExpression(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expression)
        expression->parent = returnExpression;
    return _Result<ReturnExpression, ParserError>(returnExpression);
}

_Result<ThrowExpression, ParserError> Parser::parseThrowExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startThrow1 = lexer->getPreviousPosition(_p);
    bool successThrow1 = lexer->parseKeyword(throwKeyword);
    if (successThrow1) {
        lexer->advance();
    }
    else {
        return _Result<ThrowExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startThrow1), &String::create(_ep, throwKeyword))));
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        expression = nullptr;
    ThrowExpression* throwExpression = new(_rp) ThrowExpression(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expression)
        expression->parent = throwExpression;
    return _Result<ThrowExpression, ParserError>(throwExpression);
}

_Result<BreakExpression, ParserError> Parser::parseBreakExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startBreak1 = lexer->getPreviousPosition(_p);
    bool successBreak1 = lexer->parseKeyword(breakKeyword);
    if (successBreak1) {
        lexer->advance();
    }
    else {
        return _Result<BreakExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startBreak1), &String::create(_ep, breakKeyword))));
    }
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        expression = nullptr;
    BreakExpression* breakExpression = new(_rp) BreakExpression(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expression)
        expression->parent = breakExpression;
    return _Result<BreakExpression, ParserError>(breakExpression);
}

_Result<InitializerCall, ParserError> Parser::parseInitializerCall(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _typeToInitialize_result = parseType(_rp, _ep);
    Type* typeToInitialize;
    if (_typeToInitialize_result.succeeded())
        typeToInitialize = _typeToInitialize_result.getResult();
    else
        return _Result<InitializerCall, ParserError>(_typeToInitialize_result.getError());
    auto _arguments_result = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments;
    if (_arguments_result.succeeded())
        arguments = _arguments_result.getResult();
    else
        return _Result<InitializerCall, ParserError>(_arguments_result.getError());
    auto _catchClauses_result = parseCatchClauseList(_rp, _ep);
    _Vector<CatchClause>* catchClauses;
    if (_catchClauses_result.succeeded())
        catchClauses = _catchClauses_result.getResult();
    else
        catchClauses = nullptr;
    InitializerCall* initializerCall = new(_rp) InitializerCall(typeToInitialize, arguments, catchClauses, new(_rp) Position(start), lexer->getPosition(_rp));
    typeToInitialize->parent = initializerCall;
    arguments->parent = initializerCall;
    if (catchClauses) {
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++)
            (*(*catchClauses)[_i])->parent = initializerCall;
    }
    return _Result<InitializerCall, ParserError>(initializerCall);
}

_Result<ThisExpression, ParserError> Parser::parseThisExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startThis1 = lexer->getPreviousPosition(_p);
    bool successThis1 = lexer->parseKeyword(thisKeyword);
    if (successThis1) {
        lexer->advance();
    }
    else {
        return _Result<ThisExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startThis1), &String::create(_ep, thisKeyword))));
    }
    ThisExpression* thisExpression = new(_rp) ThisExpression(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<ThisExpression, ParserError>(thisExpression);
}

_Result<SuperExpression, ParserError> Parser::parseSuperExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SuperDot, ParserError> result = parseSuperDot(_rp, _p);
        if (result.succeeded())
            return _Result<SuperExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SuperSubscript, ParserError> result = parseSuperSubscript(_rp, _p);
        if (result.succeeded())
            return _Result<SuperExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<SuperExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<SuperDot, ParserError> Parser::parseSuperDot(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSuper1 = lexer->getPreviousPosition(_p);
    bool successSuper1 = lexer->parseKeyword(superKeyword);
    if (successSuper1) {
        lexer->advance();
    }
    else {
        return _Result<SuperDot, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startSuper1), &String::create(_ep, superKeyword))));
    }
    Position* startDot2 = lexer->getPreviousPosition(_p);
    bool successDot2 = lexer->parsePunctuation(dot);
    if (successDot2) {
        lexer->advance();
    }
    else {
        return _Result<SuperDot, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot2), &String::create(_ep, dot))));
    }
    auto _member_result = parseCommonSuperMember(_rp, _ep);
    CommonSuperMember* member;
    if (_member_result.succeeded())
        member = _member_result.getResult();
    else
        return _Result<SuperDot, ParserError>(_member_result.getError());
    SuperDot* superDot = new(_rp) SuperDot(member, new(_rp) Position(start), lexer->getPosition(_rp));
    member->parent = superDot;
    return _Result<SuperDot, ParserError>(superDot);
}

_Result<SuperSubscript, ParserError> Parser::parseSuperSubscript(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSuper1 = lexer->getPreviousPosition(_p);
    bool successSuper1 = lexer->parseKeyword(superKeyword);
    if (successSuper1) {
        lexer->advance();
    }
    else {
        return _Result<SuperSubscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startSuper1), &String::create(_ep, superKeyword))));
    }
    auto _subscript_result = parseSubscript(_rp, _ep);
    Subscript* subscript;
    if (_subscript_result.succeeded())
        subscript = _subscript_result.getResult();
    else
        return _Result<SuperSubscript, ParserError>(_subscript_result.getError());
    SuperSubscript* superSubscript = new(_rp) SuperSubscript(subscript, new(_rp) Position(start), lexer->getPosition(_rp));
    subscript->parent = superSubscript;
    return _Result<SuperSubscript, ParserError>(superSubscript);
}

_Result<NullExpression, ParserError> Parser::parseNullExpression(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startNull1 = lexer->getPreviousPosition(_p);
    bool successNull1 = lexer->parseKeyword(nullKeyword);
    if (successNull1) {
        lexer->advance();
    }
    else {
        return _Result<NullExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startNull1), &String::create(_ep, nullKeyword))));
    }
    NullExpression* nullExpression = new(_rp) NullExpression(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<NullExpression, ParserError>(nullExpression);
}

_Result<ElseClause, ParserError> Parser::parseElseClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startElse1 = lexer->getPreviousPosition(_p);
    bool successElse1 = lexer->parseKeyword(elseKeyword);
    if (successElse1) {
        lexer->advance();
    }
    else {
        return _Result<ElseClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startElse1), &String::create(_ep, elseKeyword))));
    }
    auto _alternative_result = parseExpression(_rp, _ep);
    Expression* alternative;
    if (_alternative_result.succeeded())
        alternative = _alternative_result.getResult();
    else
        return _Result<ElseClause, ParserError>(_alternative_result.getError());
    ElseClause* elseClause = new(_rp) ElseClause(alternative, new(_rp) Position(start), lexer->getPosition(_rp));
    alternative->parent = elseClause;
    return _Result<ElseClause, ParserError>(elseClause);
}

_Result<SwitchBody, ParserError> Parser::parseSwitchBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<CurliedSwitchBody, ParserError> result = parseCurliedSwitchBody(_rp, _p);
        if (result.succeeded())
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<NakedSwitchBody, ParserError> result = parseNakedSwitchBody(_rp, _p);
        if (result.succeeded())
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<SwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<CurliedSwitchBody, ParserError> Parser::parseCurliedSwitchBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1) {
        lexer->advance();
    }
    else {
        return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftCurly1), &String::create(_ep, leftCurly))));
    }
    auto _cases_result = parseSwitchCaseList(_rp, _ep);
    _Vector<SwitchCase>* cases;
    if (_cases_result.succeeded())
        cases = _cases_result.getResult();
    else
        return _Result<CurliedSwitchBody, ParserError>(_cases_result.getError());
    Position* startRightCurly3 = lexer->getPreviousPosition(_p);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3) {
        lexer->advance();
    }
    else {
        return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightCurly3), &String::create(_ep, rightCurly))));
    }
    CurliedSwitchBody* curliedSwitchBody = new(_rp) CurliedSwitchBody(cases, new(_rp) Position(start), lexer->getPosition(_rp));
    if (cases) {
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++)
            (*(*cases)[_i])->parent = curliedSwitchBody;
    }
    return _Result<CurliedSwitchBody, ParserError>(curliedSwitchBody);
}

_Result<NakedSwitchBody, ParserError> Parser::parseNakedSwitchBody(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _cases_result = parseSwitchCaseList(_rp, _ep);
    _Vector<SwitchCase>* cases;
    if (_cases_result.succeeded())
        cases = _cases_result.getResult();
    else
        return _Result<NakedSwitchBody, ParserError>(_cases_result.getError());
    NakedSwitchBody* nakedSwitchBody = new(_rp) NakedSwitchBody(cases, new(_rp) Position(start), lexer->getPosition(_rp));
    if (cases) {
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++)
            (*(*cases)[_i])->parent = nakedSwitchBody;
    }
    return _Result<NakedSwitchBody, ParserError>(nakedSwitchBody);
}

_Result<_Vector<SwitchCase>, ParserError> Parser::parseSwitchCaseList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<SwitchCase>* switchCase = 0;
    while (true) {
        _Result<SwitchCase, ParserError> nodeResult = parseSwitchCase(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!switchCase)
                switchCase = new(_p) _Array<SwitchCase>();
            switchCase->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<SwitchCase>, ParserError>(switchCase ? &_Vector<SwitchCase>::create(_rp, *switchCase) : 0);
}

_Result<SwitchCase, ParserError> Parser::parseSwitchCase(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _label_result = parseCaseLabel(_rp, _ep);
    CaseLabel* label;
    if (_label_result.succeeded())
        label = _label_result.getResult();
    else
        return _Result<SwitchCase, ParserError>(_label_result.getError());
    auto _content_result = parseCaseContent(_rp, _ep);
    CaseContent* content;
    if (_content_result.succeeded())
        content = _content_result.getResult();
    else
        return _Result<SwitchCase, ParserError>(_content_result.getError());
    SwitchCase* switchCase = new(_rp) SwitchCase(label, content, new(_rp) Position(start), lexer->getPosition(_rp));
    label->parent = switchCase;
    content->parent = switchCase;
    return _Result<SwitchCase, ParserError>(switchCase);
}

_Result<CaseLabel, ParserError> Parser::parseCaseLabel(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ItemCaseLabel, ParserError> result = parseItemCaseLabel(_rp, _p);
        if (result.succeeded())
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<DefaultCaseLabel, ParserError> result = parseDefaultCaseLabel(_rp, _p);
        if (result.succeeded())
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<ItemCaseLabel, ParserError> Parser::parseItemCaseLabel(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startCase1 = lexer->getPreviousPosition(_p);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1) {
        lexer->advance();
    }
    else {
        return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startCase1), &String::create(_ep, caseKeyword))));
    }
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<ItemCaseLabel, ParserError>(_pattern_result.getError());
    auto _additionalPatterns_result = parseCaseItemList(_rp, _ep);
    _Vector<CaseItem>* additionalPatterns;
    if (_additionalPatterns_result.succeeded())
        additionalPatterns = _additionalPatterns_result.getResult();
    else
        additionalPatterns = nullptr;
    Position* startColon4 = lexer->getPreviousPosition(_p);
    bool successColon4 = lexer->parsePunctuation(colon);
    if (successColon4) {
        lexer->advance();
    }
    else {
        return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon4), &String::create(_ep, colon))));
    }
    ItemCaseLabel* itemCaseLabel = new(_rp) ItemCaseLabel(pattern, additionalPatterns, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = itemCaseLabel;
    if (additionalPatterns) {
        size_t _additionalPatterns_length = additionalPatterns->length();
        for (size_t _i = 0; _i < _additionalPatterns_length; _i++)
            (*(*additionalPatterns)[_i])->parent = itemCaseLabel;
    }
    return _Result<ItemCaseLabel, ParserError>(itemCaseLabel);
}

_Result<DefaultCaseLabel, ParserError> Parser::parseDefaultCaseLabel(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDefault1 = lexer->getPreviousPosition(_p);
    bool successDefault1 = lexer->parseKeyword(defaultKeyword);
    if (successDefault1) {
        lexer->advance();
    }
    else {
        return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startDefault1), &String::create(_ep, defaultKeyword))));
    }
    Position* startColon2 = lexer->getPreviousPosition(_p);
    bool successColon2 = lexer->parsePunctuation(colon);
    if (successColon2) {
        lexer->advance();
    }
    else {
        return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon2), &String::create(_ep, colon))));
    }
    DefaultCaseLabel* defaultCaseLabel = new(_rp) DefaultCaseLabel(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<DefaultCaseLabel, ParserError>(defaultCaseLabel);
}

_Result<_Vector<CaseItem>, ParserError> Parser::parseCaseItemList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<CaseItem>* caseItem = 0;
    while (true) {
        _Result<CaseItem, ParserError> nodeResult = parseCaseItem(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!caseItem)
                caseItem = new(_p) _Array<CaseItem>();
            caseItem->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<CaseItem>, ParserError>(caseItem ? &_Vector<CaseItem>::create(_rp, *caseItem) : 0);
}

_Result<CaseItem, ParserError> Parser::parseCaseItem(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startComma1 = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1) {
        lexer->advance();
    }
    else {
        return _Result<CaseItem, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startComma1), &String::create(_ep, comma))));
    }
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<CaseItem, ParserError>(_pattern_result.getError());
    CaseItem* caseItem = new(_rp) CaseItem(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = caseItem;
    return _Result<CaseItem, ParserError>(caseItem);
}

_Result<Pattern, ParserError> Parser::parsePattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<WildcardPattern, ParserError> result = parseWildcardPattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<IdentifierPattern, ParserError> result = parseIdentifierPattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<TuplePattern, ParserError> result = parseTuplePattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ExpressionPattern, ParserError> result = parseExpressionPattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Pattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<WildcardPattern, ParserError> Parser::parseWildcardPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startUnderscore1 = lexer->getPreviousPosition(_p);
    bool successUnderscore1 = lexer->parsePunctuation(underscore);
    if (successUnderscore1) {
        lexer->advance();
    }
    else {
        return _Result<WildcardPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startUnderscore1), &String::create(_ep, underscore))));
    }
    WildcardPattern* wildcardPattern = new(_rp) WildcardPattern(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<WildcardPattern, ParserError>(wildcardPattern);
}

_Result<IdentifierPattern, ParserError> Parser::parseIdentifierPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startIdentifier = lexer->getPreviousPosition(_p);
    String* identifier = lexer->parseIdentifier(_rp);
    if ((identifier) && (isIdentifier(identifier))) {
        lexer->advance();
    }
    else {
        return _Result<IdentifierPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startIdentifier))));
    }
    auto _annotationForType_result = parseTypeAnnotation(_rp, _ep);
    TypeAnnotation* annotationForType;
    if (_annotationForType_result.succeeded())
        annotationForType = _annotationForType_result.getResult();
    else
        annotationForType = nullptr;
    IdentifierPattern* identifierPattern = new(_rp) IdentifierPattern(identifier, annotationForType, new(_rp) Position(start), lexer->getPosition(_rp));
    if (annotationForType)
        annotationForType->parent = identifierPattern;
    return _Result<IdentifierPattern, ParserError>(identifierPattern);
}

_Result<TuplePattern, ParserError> Parser::parseTuplePattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftParen1 = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1) {
        lexer->advance();
    }
    else {
        return _Result<TuplePattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftParen1), &String::create(_ep, leftParen))));
    }
    auto _elements_result = parseTuplePatternElementList(_rp, _ep);
    _Vector<TuplePatternElement>* elements;
    if (_elements_result.succeeded())
        elements = _elements_result.getResult();
    else
        return _Result<TuplePattern, ParserError>(_elements_result.getError());
    Position* startRightParen3 = lexer->getPreviousPosition(_p);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3) {
        lexer->advance();
    }
    else {
        return _Result<TuplePattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightParen3), &String::create(_ep, rightParen))));
    }
    TuplePattern* tuplePattern = new(_rp) TuplePattern(elements, new(_rp) Position(start), lexer->getPosition(_rp));
    if (elements) {
        size_t _elements_length = elements->length();
        for (size_t _i = 0; _i < _elements_length; _i++)
            (*(*elements)[_i])->parent = tuplePattern;
    }
    return _Result<TuplePattern, ParserError>(tuplePattern);
}

_Result<ExpressionPattern, ParserError> Parser::parseExpressionPattern(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _expression_result = parseExpression(_rp, _ep);
    Expression* expression;
    if (_expression_result.succeeded())
        expression = _expression_result.getResult();
    else
        return _Result<ExpressionPattern, ParserError>(_expression_result.getError());
    ExpressionPattern* expressionPattern = new(_rp) ExpressionPattern(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = expressionPattern;
    return _Result<ExpressionPattern, ParserError>(expressionPattern);
}

_Result<_Vector<TuplePatternElement>, ParserError> Parser::parseTuplePatternElementList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<TuplePatternElement>* tuplePatternElement = 0;
    while (true) {
        _Result<TuplePatternElement, ParserError> nodeResult = parseTuplePatternElement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!tuplePatternElement)
                tuplePatternElement = new(_p) _Array<TuplePatternElement>();
            tuplePatternElement->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<TuplePatternElement>, ParserError>(tuplePatternElement ? &_Vector<TuplePatternElement>::create(_rp, *tuplePatternElement) : 0);
}

_Result<TuplePatternElement, ParserError> Parser::parseTuplePatternElement(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _pattern_result = parsePattern(_rp, _ep);
    Pattern* pattern;
    if (_pattern_result.succeeded())
        pattern = _pattern_result.getResult();
    else
        return _Result<TuplePatternElement, ParserError>(_pattern_result.getError());
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2) {
        lexer->advance();
    }
    TuplePatternElement* tuplePatternElement = new(_rp) TuplePatternElement(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = tuplePatternElement;
    return _Result<TuplePatternElement, ParserError>(tuplePatternElement);
}

_Result<CaseContent, ParserError> Parser::parseCaseContent(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<BlockCaseContent, ParserError> result = parseBlockCaseContent(_rp, _p);
        if (result.succeeded())
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<EmptyCaseContent, ParserError> result = parseEmptyCaseContent(_rp, _p);
        if (result.succeeded())
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CaseContent, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<BlockCaseContent, ParserError> Parser::parseBlockCaseContent(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _statements_result = parseTerminatedStatementList(_rp, _ep);
    _Vector<TerminatedStatement>* statements;
    if (_statements_result.succeeded())
        statements = _statements_result.getResult();
    else
        return _Result<BlockCaseContent, ParserError>(_statements_result.getError());
    BlockCaseContent* blockCaseContent = new(_rp) BlockCaseContent(statements, new(_rp) Position(start), lexer->getPosition(_rp));
    if (statements) {
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++)
            (*(*statements)[_i])->parent = blockCaseContent;
    }
    return _Result<BlockCaseContent, ParserError>(blockCaseContent);
}

_Result<EmptyCaseContent, ParserError> Parser::parseEmptyCaseContent(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startSemicolon1 = lexer->getPreviousPosition(_p);
    bool successSemicolon1 = lexer->parsePunctuation(semicolon);
    if (successSemicolon1) {
        lexer->advance();
    }
    else {
        return _Result<EmptyCaseContent, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startSemicolon1), &String::create(_ep, semicolon))));
    }
    EmptyCaseContent* emptyCaseContent = new(_rp) EmptyCaseContent(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<EmptyCaseContent, ParserError>(emptyCaseContent);
}

_Result<CommonSuperMember, ParserError> Parser::parseCommonSuperMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SuperInit, ParserError> result = parseSuperInit(_rp, _p);
        if (result.succeeded())
            return _Result<CommonSuperMember, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<SuperMember, ParserError> result = parseSuperMember(_rp, _p);
        if (result.succeeded())
            return _Result<CommonSuperMember, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CommonSuperMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<SuperInit, ParserError> Parser::parseSuperInit(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startInit1 = lexer->getPreviousPosition(_p);
    bool successInit1 = lexer->parseKeyword(initKeyword);
    if (successInit1) {
        lexer->advance();
    }
    else {
        return _Result<SuperInit, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(new(_ep) Position(startInit1), &String::create(_ep, initKeyword))));
    }
    SuperInit* superInit = new(_rp) SuperInit(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<SuperInit, ParserError>(superInit);
}

_Result<SuperMember, ParserError> Parser::parseSuperMember(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<SuperMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    SuperMember* superMember = new(_rp) SuperMember(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<SuperMember, ParserError>(superMember);
}

_Result<Type, ParserError> Parser::parseType(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<ArrayType, ParserError> result = parseArrayType(_rp, _p);
        if (result.succeeded())
            return _Result<Type, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _p);
        if (result.succeeded())
            return _Result<Type, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Type, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<TypeIdentifier, ParserError> Parser::parseTypeIdentifier(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startName = lexer->getPreviousPosition(_p);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<TypeIdentifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(new(_ep) Position(startName))));
    }
    auto _subType_result = parseSubtypeIdentifier(_rp, _ep);
    SubtypeIdentifier* subType;
    if (_subType_result.succeeded())
        subType = _subType_result.getResult();
    else
        subType = nullptr;
    auto _postfixes_result = parseTypePostfixList(_rp, _ep);
    _Vector<TypePostfix>* postfixes;
    if (_postfixes_result.succeeded())
        postfixes = _postfixes_result.getResult();
    else
        postfixes = nullptr;
    TypeIdentifier* typeIdentifier = new(_rp) TypeIdentifier(name, subType, postfixes, new(_rp) Position(start), lexer->getPosition(_rp));
    if (subType)
        subType->parent = typeIdentifier;
    if (postfixes) {
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++)
            (*(*postfixes)[_i])->parent = typeIdentifier;
    }
    return _Result<TypeIdentifier, ParserError>(typeIdentifier);
}

_Result<ArrayType, ParserError> Parser::parseArrayType(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startLeftBracket1 = lexer->getPreviousPosition(_p);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1) {
        lexer->advance();
    }
    else {
        return _Result<ArrayType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startLeftBracket1), &String::create(_ep, leftBracket))));
    }
    auto _elementType_result = parseType(_rp, _ep);
    Type* elementType;
    if (_elementType_result.succeeded())
        elementType = _elementType_result.getResult();
    else
        return _Result<ArrayType, ParserError>(_elementType_result.getError());
    Position* startRightBracket3 = lexer->getPreviousPosition(_p);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3) {
        lexer->advance();
    }
    else {
        return _Result<ArrayType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startRightBracket3), &String::create(_ep, rightBracket))));
    }
    auto _postfixes_result = parseTypePostfixList(_rp, _ep);
    _Vector<TypePostfix>* postfixes;
    if (_postfixes_result.succeeded())
        postfixes = _postfixes_result.getResult();
    else
        postfixes = nullptr;
    ArrayType* arrayType = new(_rp) ArrayType(elementType, postfixes, new(_rp) Position(start), lexer->getPosition(_rp));
    elementType->parent = arrayType;
    if (postfixes) {
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++)
            (*(*postfixes)[_i])->parent = arrayType;
    }
    return _Result<ArrayType, ParserError>(arrayType);
}

_Result<TypeAnnotation, ParserError> Parser::parseTypeAnnotation(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startColon1 = lexer->getPreviousPosition(_p);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1) {
        lexer->advance();
    }
    else {
        return _Result<TypeAnnotation, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon1), &String::create(_ep, colon))));
    }
    auto _annotationForType_result = parseType(_rp, _ep);
    Type* annotationForType;
    if (_annotationForType_result.succeeded())
        annotationForType = _annotationForType_result.getResult();
    else
        return _Result<TypeAnnotation, ParserError>(_annotationForType_result.getError());
    TypeAnnotation* typeAnnotation = new(_rp) TypeAnnotation(annotationForType, new(_rp) Position(start), lexer->getPosition(_rp));
    annotationForType->parent = typeAnnotation;
    return _Result<TypeAnnotation, ParserError>(typeAnnotation);
}

_Result<SubtypeIdentifier, ParserError> Parser::parseSubtypeIdentifier(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startDot1 = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1) {
        lexer->advance();
    }
    else {
        return _Result<SubtypeIdentifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startDot1), &String::create(_ep, dot))));
    }
    auto _typeIdentifier_result = parseTypeIdentifier(_rp, _ep);
    TypeIdentifier* typeIdentifier;
    if (_typeIdentifier_result.succeeded())
        typeIdentifier = _typeIdentifier_result.getResult();
    else
        return _Result<SubtypeIdentifier, ParserError>(_typeIdentifier_result.getError());
    SubtypeIdentifier* subtypeIdentifier = new(_rp) SubtypeIdentifier(typeIdentifier, new(_rp) Position(start), lexer->getPosition(_rp));
    typeIdentifier->parent = subtypeIdentifier;
    return _Result<SubtypeIdentifier, ParserError>(subtypeIdentifier);
}

_Result<_Vector<TypePostfix>, ParserError> Parser::parseTypePostfixList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<TypePostfix>* typePostfix = 0;
    while (true) {
        _Result<TypePostfix, ParserError> nodeResult = parseTypePostfix(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!typePostfix)
                typePostfix = new(_p) _Array<TypePostfix>();
            typePostfix->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<TypePostfix>, ParserError>(typePostfix ? &_Vector<TypePostfix>::create(_rp, *typePostfix) : 0);
}

_Result<TypePostfix, ParserError> Parser::parseTypePostfix(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
    {
        _Region _region; _Page* _p = _region.get();
        _Result<OptionalType, ParserError> result = parseOptionalType(_rp, _p);
        if (result.succeeded())
            return _Result<TypePostfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<TypePostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<OptionalType, ParserError> Parser::parseOptionalType(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startQuestionMark1 = lexer->getPreviousPosition(_p);
    bool successQuestionMark1 = lexer->parsePunctuation(questionMark);
    if (successQuestionMark1) {
        lexer->advance();
    }
    else {
        return _Result<OptionalType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startQuestionMark1), &String::create(_ep, questionMark))));
    }
    OptionalType* optionalType = new(_rp) OptionalType(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<OptionalType, ParserError>(optionalType);
}

_Result<TypeInheritanceClause, ParserError> Parser::parseTypeInheritanceClause(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Position* startColon1 = lexer->getPreviousPosition(_p);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1) {
        lexer->advance();
    }
    else {
        return _Result<TypeInheritanceClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(new(_ep) Position(startColon1), &String::create(_ep, colon))));
    }
    auto _inheritances_result = parseInheritanceList(_rp, _ep);
    _Vector<Inheritance>* inheritances;
    if (_inheritances_result.succeeded())
        inheritances = _inheritances_result.getResult();
    else
        inheritances = nullptr;
    TypeInheritanceClause* typeInheritanceClause = new(_rp) TypeInheritanceClause(inheritances, new(_rp) Position(start), lexer->getPosition(_rp));
    if (inheritances) {
        size_t _inheritances_length = inheritances->length();
        for (size_t _i = 0; _i < _inheritances_length; _i++)
            (*(*inheritances)[_i])->parent = typeInheritanceClause;
    }
    return _Result<TypeInheritanceClause, ParserError>(typeInheritanceClause);
}

_Result<_Vector<Inheritance>, ParserError> Parser::parseInheritanceList(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<Inheritance>* inheritance = 0;
    while (true) {
        _Result<Inheritance, ParserError> nodeResult = parseInheritance(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!inheritance)
                inheritance = new(_p) _Array<Inheritance>();
            inheritance->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<_Vector<Inheritance>, ParserError>(inheritance ? &_Vector<Inheritance>::create(_rp, *inheritance) : 0);
}

_Result<Inheritance, ParserError> Parser::parseInheritance(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    auto _typeIdentifier_result = parseTypeIdentifier(_rp, _ep);
    TypeIdentifier* typeIdentifier;
    if (_typeIdentifier_result.succeeded())
        typeIdentifier = _typeIdentifier_result.getResult();
    else
        return _Result<Inheritance, ParserError>(_typeIdentifier_result.getError());
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2) {
        lexer->advance();
    }
    Inheritance* inheritance = new(_rp) Inheritance(typeIdentifier, new(_rp) Position(start), lexer->getPosition(_rp));
    typeIdentifier->parent = inheritance;
    return _Result<Inheritance, ParserError>(inheritance);
}

bool Parser::isAtEnd() {
    return lexer->isAtEnd();
}

bool Parser::isIdentifier(String* id) {
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

    if (id->equals(initKeyword))
        return false;

    if (id->equals(enumKeyword))
        return false;

    if (id->equals(superKeyword))
        return false;

    if (id->equals(thisKeyword))
        return false;

    if (id->equals(nullKeyword))
        return false;

    return true;
}

}
