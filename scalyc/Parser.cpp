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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<_Vector<TerminatedStatement>, ParserError> _result_statements = parseTerminatedStatementList(_rp, _ep);
    _Vector<TerminatedStatement>* statements = 0;
    if (_result_statements.succeeded()) {
        if (!isAtEnd()) {
            Position* current = lexer->getPosition(_ep);
            return _Result<CompilationUnit, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_notAtEnd(current)));
        }
        statements = _result_statements.getResult();
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Statement, ParserError> _result_statement = parseStatement(_rp, _ep);
    Statement* statement = 0;
    if (_result_statement.succeeded()) {
        statement = _result_statement.getResult();
    }
    else {
        return _Result<TerminatedStatement, ParserError>(_result_statement.getError());
    }
    bool successSemicolon2 = lexer->parsePunctuation(semicolon);
    if (successSemicolon2) {
        lexer->advance();
    }
    TerminatedStatement* terminatedStatement = new(_rp) TerminatedStatement(statement, new(_rp) Position(start), lexer->getPosition(_rp));
    statement->parent = terminatedStatement;
    return _Result<TerminatedStatement, ParserError>(terminatedStatement);
}

_Result<Statement, ParserError> Parser::parseStatement(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Declaration, ParserError> result = parseDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Expression, ParserError> result = parseExpression(_rp, _p);
        if (result.succeeded())
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Statement, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<Declaration, ParserError> Parser::parseDeclaration(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<UseDeclaration, ParserError> result = parseUseDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ConstantDeclaration, ParserError> result = parseConstantDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<VariableDeclaration, ParserError> result = parseVariableDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<MutableDeclaration, ParserError> result = parseMutableDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<FunctionDeclaration, ParserError> result = parseFunctionDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<EnumDeclaration, ParserError> result = parseEnumDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ClassDeclaration, ParserError> result = parseClassDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<InitializerDeclaration, ParserError> result = parseInitializerDeclaration(_rp, _p);
        if (result.succeeded())
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Declaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<Expression, ParserError> Parser::parseExpression(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<CodeBlock, ParserError> result = parseCodeBlock(_rp, _p);
        if (result.succeeded())
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SimpleExpression, ParserError> result = parseSimpleExpression(_rp, _p);
        if (result.succeeded())
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Expression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<UseDeclaration, ParserError> Parser::parseUseDeclaration(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startUse1 = lexer->getPreviousPosition(_rp);
    bool successUse1 = lexer->parseKeyword(useKeyword);
    if (successUse1) {
        lexer->advance();
    }
    else {
        return _Result<UseDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startUse1, &String::create(_ep, useKeyword))));
    }
    _Result<PathItem, ParserError> _result_importModule = parsePathItem(_rp, _ep);
    PathItem* importModule = 0;
    if (_result_importModule.succeeded()) {
        importModule = _result_importModule.getResult();
    }
    else {
        return _Result<UseDeclaration, ParserError>(_result_importModule.getError());
    }
    _Result<_Vector<PathIdentifier>, ParserError> _result_importExtensions = parsePathIdentifierList(_rp, _ep);
    _Vector<PathIdentifier>* importExtensions = 0;
    if (_result_importExtensions.succeeded()) {
        importExtensions = _result_importExtensions.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLet1 = lexer->getPreviousPosition(_rp);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1) {
        lexer->advance();
    }
    else {
        return _Result<ConstantDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startLet1, &String::create(_ep, letKeyword))));
    }
    _Result<BindingInitializer, ParserError> _result_initializer = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer = 0;
    if (_result_initializer.succeeded()) {
        initializer = _result_initializer.getResult();
    }
    else {
        return _Result<ConstantDeclaration, ParserError>(_result_initializer.getError());
    }
    ConstantDeclaration* constantDeclaration = new(_rp) ConstantDeclaration(initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = constantDeclaration;
    return _Result<ConstantDeclaration, ParserError>(constantDeclaration);
}

_Result<VariableDeclaration, ParserError> Parser::parseVariableDeclaration(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startVar1 = lexer->getPreviousPosition(_rp);
    bool successVar1 = lexer->parseKeyword(varKeyword);
    if (successVar1) {
        lexer->advance();
    }
    else {
        return _Result<VariableDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startVar1, &String::create(_ep, varKeyword))));
    }
    _Result<BindingInitializer, ParserError> _result_initializer = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer = 0;
    if (_result_initializer.succeeded()) {
        initializer = _result_initializer.getResult();
    }
    else {
        return _Result<VariableDeclaration, ParserError>(_result_initializer.getError());
    }
    VariableDeclaration* variableDeclaration = new(_rp) VariableDeclaration(initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = variableDeclaration;
    return _Result<VariableDeclaration, ParserError>(variableDeclaration);
}

_Result<MutableDeclaration, ParserError> Parser::parseMutableDeclaration(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startMutable1 = lexer->getPreviousPosition(_rp);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1) {
        lexer->advance();
    }
    else {
        return _Result<MutableDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startMutable1, &String::create(_ep, mutableKeyword))));
    }
    _Result<BindingInitializer, ParserError> _result_initializer = parseBindingInitializer(_rp, _ep);
    BindingInitializer* initializer = 0;
    if (_result_initializer.succeeded()) {
        initializer = _result_initializer.getResult();
    }
    else {
        return _Result<MutableDeclaration, ParserError>(_result_initializer.getError());
    }
    MutableDeclaration* mutableDeclaration = new(_rp) MutableDeclaration(initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    initializer->parent = mutableDeclaration;
    return _Result<MutableDeclaration, ParserError>(mutableDeclaration);
}

_Result<FunctionDeclaration, ParserError> Parser::parseFunctionDeclaration(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<_Vector<Modifier>, ParserError> _result_modifiers = parseModifierList(_rp, _ep);
    _Vector<Modifier>* modifiers = 0;
    if (_result_modifiers.succeeded()) {
        modifiers = _result_modifiers.getResult();
    }
    Position* startFunction2 = lexer->getPreviousPosition(_rp);
    bool successFunction2 = lexer->parseKeyword(functionKeyword);
    if (successFunction2) {
        lexer->advance();
    }
    else {
        return _Result<FunctionDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startFunction2, &String::create(_ep, functionKeyword))));
    }
    _Result<FunctionName, ParserError> _result_name = parseFunctionName(_rp, _ep);
    FunctionName* name = 0;
    if (_result_name.succeeded()) {
        name = _result_name.getResult();
    }
    else {
        return _Result<FunctionDeclaration, ParserError>(_result_name.getError());
    }
    _Result<FunctionSignature, ParserError> _result_signature = parseFunctionSignature(_rp, _ep);
    FunctionSignature* signature = 0;
    if (_result_signature.succeeded()) {
        signature = _result_signature.getResult();
    }
    else {
        return _Result<FunctionDeclaration, ParserError>(_result_signature.getError());
    }
    _Result<Expression, ParserError> _result_body = parseExpression(_rp, _ep);
    Expression* body = 0;
    if (_result_body.succeeded()) {
        body = _result_body.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startEnum1 = lexer->getPreviousPosition(_rp);
    bool successEnum1 = lexer->parseKeyword(enumKeyword);
    if (successEnum1) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startEnum1, &String::create(_ep, enumKeyword))));
    }
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    Position* startLeftCurly3 = lexer->getPreviousPosition(_rp);
    bool successLeftCurly3 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly3) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftCurly3, &String::create(_ep, leftCurly))));
    }
    _Result<_Vector<EnumMember>, ParserError> _result_members = parseEnumMemberList(_rp, _ep);
    _Vector<EnumMember>* members = 0;
    if (_result_members.succeeded()) {
        members = _result_members.getResult();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(_result_members.getError());
    }
    Position* startRightCurly5 = lexer->getPreviousPosition(_rp);
    bool successRightCurly5 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly5) {
        lexer->advance();
    }
    else {
        return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightCurly5, &String::create(_ep, rightCurly))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startClass1 = lexer->getPreviousPosition(_rp);
    bool successClass1 = lexer->parseKeyword(classKeyword);
    if (successClass1) {
        lexer->advance();
    }
    else {
        return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startClass1, &String::create(_ep, classKeyword))));
    }
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    _Result<GenericArgumentClause, ParserError> _result_genericArgumentClause = parseGenericArgumentClause(_rp, _ep);
    GenericArgumentClause* genericArgumentClause = 0;
    if (_result_genericArgumentClause.succeeded()) {
        genericArgumentClause = _result_genericArgumentClause.getResult();
    }
    _Result<TypeInheritanceClause, ParserError> _result_typeInheritanceClause = parseTypeInheritanceClause(_rp, _ep);
    TypeInheritanceClause* typeInheritanceClause = 0;
    if (_result_typeInheritanceClause.succeeded()) {
        typeInheritanceClause = _result_typeInheritanceClause.getResult();
    }
    _Result<ClassBody, ParserError> _result_body = parseClassBody(_rp, _ep);
    ClassBody* body = 0;
    if (_result_body.succeeded()) {
        body = _result_body.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<_Vector<Modifier>, ParserError> _result_modifiers = parseModifierList(_rp, _ep);
    _Vector<Modifier>* modifiers = 0;
    if (_result_modifiers.succeeded()) {
        modifiers = _result_modifiers.getResult();
    }
    Position* startInit2 = lexer->getPreviousPosition(_rp);
    bool successInit2 = lexer->parseKeyword(initKeyword);
    if (successInit2) {
        lexer->advance();
    }
    else {
        return _Result<InitializerDeclaration, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startInit2, &String::create(_ep, initKeyword))));
    }
    _Result<ParameterClause, ParserError> _result_parameterClause = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause = 0;
    if (_result_parameterClause.succeeded()) {
        parameterClause = _result_parameterClause.getResult();
    }
    else {
        return _Result<InitializerDeclaration, ParserError>(_result_parameterClause.getError());
    }
    _Result<ThrowsClause, ParserError> _result_throwsClause = parseThrowsClause(_rp, _ep);
    ThrowsClause* throwsClause = 0;
    if (_result_throwsClause.succeeded()) {
        throwsClause = _result_throwsClause.getResult();
    }
    _Result<Expression, ParserError> _result_body = parseExpression(_rp, _ep);
    Expression* body = 0;
    if (_result_body.succeeded()) {
        body = _result_body.getResult();
    }
    else {
        return _Result<InitializerDeclaration, ParserError>(_result_body.getError());
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_rp);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1) {
        lexer->advance();
    }
    else {
        return _Result<CodeBlock, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftCurly1, &String::create(_ep, leftCurly))));
    }
    _Result<_Vector<TerminatedStatement>, ParserError> _result_statements = parseTerminatedStatementList(_rp, _ep);
    _Vector<TerminatedStatement>* statements = 0;
    if (_result_statements.succeeded()) {
        statements = _result_statements.getResult();
    }
    else {
        return _Result<CodeBlock, ParserError>(_result_statements.getError());
    }
    Position* startRightCurly3 = lexer->getPreviousPosition(_rp);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3) {
        lexer->advance();
    }
    else {
        return _Result<CodeBlock, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightCurly3, &String::create(_ep, rightCurly))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<PrefixExpression, ParserError> _result_prefixExpression = parsePrefixExpression(_rp, _ep);
    PrefixExpression* prefixExpression = 0;
    if (_result_prefixExpression.succeeded()) {
        prefixExpression = _result_prefixExpression.getResult();
    }
    else {
        return _Result<SimpleExpression, ParserError>(_result_prefixExpression.getError());
    }
    _Result<_Vector<BinaryOp>, ParserError> _result_binaryOps = parseBinaryOpList(_rp, _ep);
    _Vector<BinaryOp>* binaryOps = 0;
    if (_result_binaryOps.succeeded()) {
        binaryOps = _result_binaryOps.getResult();
    }
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startDot1 = lexer->getPreviousPosition(_rp);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1) {
        lexer->advance();
    }
    else {
        return _Result<PathIdentifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startDot1, &String::create(_ep, dot))));
    }
    _Result<PathItem, ParserError> _result_extension = parsePathItem(_rp, _ep);
    PathItem* extension = 0;
    if (_result_extension.succeeded()) {
        extension = _result_extension.getResult();
    }
    else {
        return _Result<PathIdentifier, ParserError>(_result_extension.getError());
    }
    PathIdentifier* pathIdentifier = new(_rp) PathIdentifier(extension, new(_rp) Position(start), lexer->getPosition(_rp));
    extension->parent = pathIdentifier;
    return _Result<PathIdentifier, ParserError>(pathIdentifier);
}

_Result<PathItem, ParserError> Parser::parsePathItem(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<PathItem, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    PathItem* pathItem = new(_rp) PathItem(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<PathItem, ParserError>(pathItem);
}

_Result<Initializer, ParserError> Parser::parseInitializer(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startEqual1 = lexer->getPreviousPosition(_rp);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1) {
        lexer->advance();
    }
    else {
        return _Result<Initializer, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startEqual1, &String::create(_ep, equal))));
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<Initializer, ParserError>(_result_expression.getError());
    }
    Initializer* initializer = new(_rp) Initializer(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = initializer;
    return _Result<Initializer, ParserError>(initializer);
}

_Result<BindingInitializer, ParserError> Parser::parseBindingInitializer(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<PatternInitializer, ParserError> _result_initializer = parsePatternInitializer(_rp, _ep);
    PatternInitializer* initializer = 0;
    if (_result_initializer.succeeded()) {
        initializer = _result_initializer.getResult();
    }
    else {
        return _Result<BindingInitializer, ParserError>(_result_initializer.getError());
    }
    _Result<_Vector<AdditionalInitializer>, ParserError> _result_additionalInitializers = parseAdditionalInitializerList(_rp, _ep);
    _Vector<AdditionalInitializer>* additionalInitializers = 0;
    if (_result_additionalInitializers.succeeded()) {
        additionalInitializers = _result_additionalInitializers.getResult();
    }
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Pattern, ParserError> _result_pattern = parsePattern(_rp, _ep);
    Pattern* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<PatternInitializer, ParserError>(_result_pattern.getError());
    }
    _Result<Initializer, ParserError> _result_initializer = parseInitializer(_rp, _ep);
    Initializer* initializer = 0;
    if (_result_initializer.succeeded()) {
        initializer = _result_initializer.getResult();
    }
    PatternInitializer* patternInitializer = new(_rp) PatternInitializer(pattern, initializer, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = patternInitializer;
    if (initializer)
        initializer->parent = patternInitializer;
    return _Result<PatternInitializer, ParserError>(patternInitializer);
}

_Result<_Vector<AdditionalInitializer>, ParserError> Parser::parseAdditionalInitializerList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startComma1 = lexer->getPreviousPosition(_rp);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1) {
        lexer->advance();
    }
    else {
        return _Result<AdditionalInitializer, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startComma1, &String::create(_ep, comma))));
    }
    _Result<PatternInitializer, ParserError> _result_pattern = parsePatternInitializer(_rp, _ep);
    PatternInitializer* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<AdditionalInitializer, ParserError>(_result_pattern.getError());
    }
    AdditionalInitializer* additionalInitializer = new(_rp) AdditionalInitializer(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = additionalInitializer;
    return _Result<AdditionalInitializer, ParserError>(additionalInitializer);
}

_Result<_Vector<Modifier>, ParserError> Parser::parseModifierList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OverrideWord, ParserError> result = parseOverrideWord(_rp, _p);
        if (result.succeeded())
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<StaticWord, ParserError> result = parseStaticWord(_rp, _p);
        if (result.succeeded())
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Modifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<OverrideWord, ParserError> Parser::parseOverrideWord(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startOverride1 = lexer->getPreviousPosition(_rp);
    bool successOverride1 = lexer->parseKeyword(overrideKeyword);
    if (successOverride1) {
        lexer->advance();
    }
    else {
        return _Result<OverrideWord, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startOverride1, &String::create(_ep, overrideKeyword))));
    }
    OverrideWord* overrideWord = new(_rp) OverrideWord(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<OverrideWord, ParserError>(overrideWord);
}

_Result<StaticWord, ParserError> Parser::parseStaticWord(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startStatic1 = lexer->getPreviousPosition(_rp);
    bool successStatic1 = lexer->parseKeyword(staticKeyword);
    if (successStatic1) {
        lexer->advance();
    }
    else {
        return _Result<StaticWord, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startStatic1, &String::create(_ep, staticKeyword))));
    }
    StaticWord* staticWord = new(_rp) StaticWord(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<StaticWord, ParserError>(staticWord);
}

_Result<FunctionName, ParserError> Parser::parseFunctionName(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierFunction, ParserError> result = parseIdentifierFunction(_rp, _p);
        if (result.succeeded())
            return _Result<FunctionName, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<FunctionName, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<IdentifierFunction, ParserError> Parser::parseIdentifierFunction(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<IdentifierFunction, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    IdentifierFunction* identifierFunction = new(_rp) IdentifierFunction(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<IdentifierFunction, ParserError>(identifierFunction);
}

_Result<FunctionSignature, ParserError> Parser::parseFunctionSignature(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<ParameterClause, ParserError> _result_parameterClause = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause = 0;
    if (_result_parameterClause.succeeded()) {
        parameterClause = _result_parameterClause.getResult();
    }
    else {
        return _Result<FunctionSignature, ParserError>(_result_parameterClause.getError());
    }
    _Result<FunctionResult, ParserError> _result_result = parseFunctionResult(_rp, _ep);
    FunctionResult* result = 0;
    if (_result_result.succeeded()) {
        result = _result_result.getResult();
    }
    _Result<ThrowsClause, ParserError> _result_throwsClause = parseThrowsClause(_rp, _ep);
    ThrowsClause* throwsClause = 0;
    if (_result_throwsClause.succeeded()) {
        throwsClause = _result_throwsClause.getResult();
    }
    FunctionSignature* functionSignature = new(_rp) FunctionSignature(parameterClause, result, throwsClause, new(_rp) Position(start), lexer->getPosition(_rp));
    parameterClause->parent = functionSignature;
    if (result)
        result->parent = functionSignature;
    if (throwsClause)
        throwsClause->parent = functionSignature;
    return _Result<FunctionSignature, ParserError>(functionSignature);
}

_Result<FunctionResult, ParserError> Parser::parseFunctionResult(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startEvaluatesTo1 = lexer->getPreviousPosition(_rp);
    bool successEvaluatesTo1 = lexer->parsePunctuation(evaluatesTo);
    if (successEvaluatesTo1) {
        lexer->advance();
    }
    else {
        return _Result<FunctionResult, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startEvaluatesTo1, &String::create(_ep, evaluatesTo))));
    }
    _Result<ExistingClause, ParserError> _result_existingObject = parseExistingClause(_rp, _ep);
    ExistingClause* existingObject = 0;
    if (_result_existingObject.succeeded()) {
        existingObject = _result_existingObject.getResult();
    }
    _Result<Type, ParserError> _result_resultType = parseType(_rp, _ep);
    Type* resultType = 0;
    if (_result_resultType.succeeded()) {
        resultType = _result_resultType.getResult();
    }
    else {
        return _Result<FunctionResult, ParserError>(_result_resultType.getError());
    }
    FunctionResult* functionResult = new(_rp) FunctionResult(existingObject, resultType, new(_rp) Position(start), lexer->getPosition(_rp));
    if (existingObject)
        existingObject->parent = functionResult;
    resultType->parent = functionResult;
    return _Result<FunctionResult, ParserError>(functionResult);
}

_Result<ExistingClause, ParserError> Parser::parseExistingClause(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startExisting1 = lexer->getPreviousPosition(_rp);
    bool successExisting1 = lexer->parseKeyword(existingKeyword);
    if (successExisting1) {
        lexer->advance();
    }
    else {
        return _Result<ExistingClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startExisting1, &String::create(_ep, existingKeyword))));
    }
    ExistingClause* existingClause = new(_rp) ExistingClause(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<ExistingClause, ParserError>(existingClause);
}

_Result<_Vector<ParameterClause>, ParserError> Parser::parseParameterClauseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftParen1 = lexer->getPreviousPosition(_rp);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1) {
        lexer->advance();
    }
    else {
        return _Result<ParameterClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftParen1, &String::create(_ep, leftParen))));
    }
    _Result<_Vector<Parameter>, ParserError> _result_parameters = parseParameterList(_rp, _ep);
    _Vector<Parameter>* parameters = 0;
    if (_result_parameters.succeeded()) {
        parameters = _result_parameters.getResult();
    }
    Position* startRightParen3 = lexer->getPreviousPosition(_rp);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3) {
        lexer->advance();
    }
    else {
        return _Result<ParameterClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightParen3, &String::create(_ep, rightParen))));
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ConstParameter, ParserError> result = parseConstParameter(_rp, _p);
        if (result.succeeded())
            return _Result<Parameter, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<VarParameter, ParserError> result = parseVarParameter(_rp, _p);
        if (result.succeeded())
            return _Result<Parameter, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Parameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<ConstParameter, ParserError> Parser::parseConstParameter(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1) {
        lexer->advance();
    }
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<ConstParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    Position* startColon3 = lexer->getPreviousPosition(_rp);
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3) {
        lexer->advance();
    }
    else {
        return _Result<ConstParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startColon3, &String::create(_ep, colon))));
    }
    _Result<Type, ParserError> _result_parameterType = parseType(_rp, _ep);
    Type* parameterType = 0;
    if (_result_parameterType.succeeded()) {
        parameterType = _result_parameterType.getResult();
    }
    else {
        return _Result<ConstParameter, ParserError>(_result_parameterType.getError());
    }
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5) {
        lexer->advance();
    }
    ConstParameter* constParameter = new(_rp) ConstParameter(name, parameterType, new(_rp) Position(start), lexer->getPosition(_rp));
    parameterType->parent = constParameter;
    return _Result<ConstParameter, ParserError>(constParameter);
}

_Result<VarParameter, ParserError> Parser::parseVarParameter(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startMutable1 = lexer->getPreviousPosition(_rp);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1) {
        lexer->advance();
    }
    else {
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startMutable1, &String::create(_ep, mutableKeyword))));
    }
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    Position* startColon3 = lexer->getPreviousPosition(_rp);
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3) {
        lexer->advance();
    }
    else {
        return _Result<VarParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startColon3, &String::create(_ep, colon))));
    }
    _Result<Type, ParserError> _result_parameterType = parseType(_rp, _ep);
    Type* parameterType = 0;
    if (_result_parameterType.succeeded()) {
        parameterType = _result_parameterType.getResult();
    }
    else {
        return _Result<VarParameter, ParserError>(_result_parameterType.getError());
    }
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5) {
        lexer->advance();
    }
    VarParameter* varParameter = new(_rp) VarParameter(name, parameterType, new(_rp) Position(start), lexer->getPosition(_rp));
    parameterType->parent = varParameter;
    return _Result<VarParameter, ParserError>(varParameter);
}

_Result<ThrowsClause, ParserError> Parser::parseThrowsClause(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startThrows1 = lexer->getPreviousPosition(_rp);
    bool successThrows1 = lexer->parseKeyword(throwsKeyword);
    if (successThrows1) {
        lexer->advance();
    }
    else {
        return _Result<ThrowsClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startThrows1, &String::create(_ep, throwsKeyword))));
    }
    _Result<Type, ParserError> _result_throwsType = parseType(_rp, _ep);
    Type* throwsType = 0;
    if (_result_throwsType.succeeded()) {
        throwsType = _result_throwsType.getResult();
    }
    else {
        return _Result<ThrowsClause, ParserError>(_result_throwsType.getError());
    }
    ThrowsClause* throwsClause = new(_rp) ThrowsClause(throwsType, new(_rp) Position(start), lexer->getPosition(_rp));
    throwsType->parent = throwsClause;
    return _Result<ThrowsClause, ParserError>(throwsClause);
}

_Result<_Vector<EnumMember>, ParserError> Parser::parseEnumMemberList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startCase1 = lexer->getPreviousPosition(_rp);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1) {
        lexer->advance();
    }
    else {
        return _Result<EnumMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startCase1, &String::create(_ep, caseKeyword))));
    }
    _Result<EnumCase, ParserError> _result_enumCase = parseEnumCase(_rp, _ep);
    EnumCase* enumCase = 0;
    if (_result_enumCase.succeeded()) {
        enumCase = _result_enumCase.getResult();
    }
    else {
        return _Result<EnumMember, ParserError>(_result_enumCase.getError());
    }
    _Result<_Vector<AdditionalCase>, ParserError> _result_additionalCases = parseAdditionalCaseList(_rp, _ep);
    _Vector<AdditionalCase>* additionalCases = 0;
    if (_result_additionalCases.succeeded()) {
        additionalCases = _result_additionalCases.getResult();
    }
    _Result<ParameterClause, ParserError> _result_parameterClause = parseParameterClause(_rp, _ep);
    ParameterClause* parameterClause = 0;
    if (_result_parameterClause.succeeded()) {
        parameterClause = _result_parameterClause.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<EnumCase, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    EnumCase* enumCase = new(_rp) EnumCase(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<EnumCase, ParserError>(enumCase);
}

_Result<_Vector<AdditionalCase>, ParserError> Parser::parseAdditionalCaseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startComma1 = lexer->getPreviousPosition(_rp);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1) {
        lexer->advance();
    }
    else {
        return _Result<AdditionalCase, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startComma1, &String::create(_ep, comma))));
    }
    _Result<EnumCase, ParserError> _result_enumCase = parseEnumCase(_rp, _ep);
    EnumCase* enumCase = 0;
    if (_result_enumCase.succeeded()) {
        enumCase = _result_enumCase.getResult();
    }
    else {
        return _Result<AdditionalCase, ParserError>(_result_enumCase.getError());
    }
    AdditionalCase* additionalCase = new(_rp) AdditionalCase(enumCase, new(_rp) Position(start), lexer->getPosition(_rp));
    enumCase->parent = additionalCase;
    return _Result<AdditionalCase, ParserError>(additionalCase);
}

_Result<ClassBody, ParserError> Parser::parseClassBody(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_rp);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1) {
        lexer->advance();
    }
    else {
        return _Result<ClassBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftCurly1, &String::create(_ep, leftCurly))));
    }
    _Result<_Vector<ClassMember>, ParserError> _result_members = parseClassMemberList(_rp, _ep);
    _Vector<ClassMember>* members = 0;
    if (_result_members.succeeded()) {
        members = _result_members.getResult();
    }
    Position* startRightCurly3 = lexer->getPreviousPosition(_rp);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3) {
        lexer->advance();
    }
    else {
        return _Result<ClassBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightCurly3, &String::create(_ep, rightCurly))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftAngular1 = lexer->getPreviousPosition(_rp);
    bool successLeftAngular1 = lexer->parsePunctuation(leftAngular);
    if (successLeftAngular1) {
        lexer->advance();
    }
    else {
        return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftAngular1, &String::create(_ep, leftAngular))));
    }
    _Result<_Vector<GenericParameter>, ParserError> _result_genericParameters = parseGenericParameterList(_rp, _ep);
    _Vector<GenericParameter>* genericParameters = 0;
    if (_result_genericParameters.succeeded()) {
        genericParameters = _result_genericParameters.getResult();
    }
    else {
        return _Result<GenericArgumentClause, ParserError>(_result_genericParameters.getError());
    }
    Position* startRightAngular3 = lexer->getPreviousPosition(_rp);
    bool successRightAngular3 = lexer->parsePunctuation(rightAngular);
    if (successRightAngular3) {
        lexer->advance();
    }
    else {
        return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightAngular3, &String::create(_ep, rightAngular))));
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startTypeName = lexer->getPreviousPosition(_rp);
    String* typeName = lexer->parseIdentifier(_rp);
    if ((typeName) && (isIdentifier(typeName))) {
        lexer->advance();
    }
    else {
        return _Result<GenericParameter, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startTypeName)));
    }
    GenericParameter* genericParameter = new(_rp) GenericParameter(typeName, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<GenericParameter, ParserError>(genericParameter);
}

_Result<_Vector<ClassMember>, ParserError> Parser::parseClassMemberList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Declaration, ParserError> _result_declaration = parseDeclaration(_rp, _ep);
    Declaration* declaration = 0;
    if (_result_declaration.succeeded()) {
        declaration = _result_declaration.getResult();
    }
    else {
        return _Result<ClassMember, ParserError>(_result_declaration.getError());
    }
    bool successSemicolon2 = lexer->parsePunctuation(semicolon);
    if (successSemicolon2) {
        lexer->advance();
    }
    ClassMember* classMember = new(_rp) ClassMember(declaration, new(_rp) Position(start), lexer->getPosition(_rp));
    declaration->parent = classMember;
    return _Result<ClassMember, ParserError>(classMember);
}

_Result<PrefixExpression, ParserError> Parser::parsePrefixExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    String* prefixOperator = lexer->parsePrefixOperator(_rp);
    if (prefixOperator) {
        lexer->advance();
    }
    _Result<PostfixExpression, ParserError> _result_expression = parsePostfixExpression(_rp, _ep);
    PostfixExpression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<PrefixExpression, ParserError>(_result_expression.getError());
    }
    PrefixExpression* prefixExpression = new(_rp) PrefixExpression(prefixOperator, expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = prefixExpression;
    return _Result<PrefixExpression, ParserError>(prefixExpression);
}

_Result<PostfixExpression, ParserError> Parser::parsePostfixExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<PrimaryExpression, ParserError> _result_primaryExpression = parsePrimaryExpression(_rp, _ep);
    PrimaryExpression* primaryExpression = 0;
    if (_result_primaryExpression.succeeded()) {
        primaryExpression = _result_primaryExpression.getResult();
    }
    else {
        return _Result<PostfixExpression, ParserError>(_result_primaryExpression.getError());
    }
    _Result<_Vector<Postfix>, ParserError> _result_postfixes = parsePostfixList(_rp, _ep);
    _Vector<Postfix>* postfixes = 0;
    if (_result_postfixes.succeeded()) {
        postfixes = _result_postfixes.getResult();
    }
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BinaryOperation, ParserError> result = parseBinaryOperation(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Assignment, ParserError> result = parseAssignment(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeQuery, ParserError> result = parseTypeQuery(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeCast, ParserError> result = parseTypeCast(_rp, _p);
        if (result.succeeded())
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<BinaryOp, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<BinaryOperation, ParserError> Parser::parseBinaryOperation(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startBinaryOperator = lexer->getPreviousPosition(_rp);
    String* binaryOperator = lexer->parseBinaryOperator(_rp);
    if (binaryOperator) {
        lexer->advance();
    }
    else {
        return _Result<BinaryOperation, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_operatorExpected(startBinaryOperator)));
    }
    _Result<PrefixExpression, ParserError> _result_expression = parsePrefixExpression(_rp, _ep);
    PrefixExpression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<BinaryOperation, ParserError>(_result_expression.getError());
    }
    BinaryOperation* binaryOperation = new(_rp) BinaryOperation(binaryOperator, expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = binaryOperation;
    return _Result<BinaryOperation, ParserError>(binaryOperation);
}

_Result<Assignment, ParserError> Parser::parseAssignment(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startEqual1 = lexer->getPreviousPosition(_rp);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1) {
        lexer->advance();
    }
    else {
        return _Result<Assignment, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startEqual1, &String::create(_ep, equal))));
    }
    _Result<PrefixExpression, ParserError> _result_expression = parsePrefixExpression(_rp, _ep);
    PrefixExpression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<Assignment, ParserError>(_result_expression.getError());
    }
    Assignment* assignment = new(_rp) Assignment(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = assignment;
    return _Result<Assignment, ParserError>(assignment);
}

_Result<TypeQuery, ParserError> Parser::parseTypeQuery(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startIs1 = lexer->getPreviousPosition(_rp);
    bool successIs1 = lexer->parseKeyword(isKeyword);
    if (successIs1) {
        lexer->advance();
    }
    else {
        return _Result<TypeQuery, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startIs1, &String::create(_ep, isKeyword))));
    }
    _Result<Type, ParserError> _result_objectType = parseType(_rp, _ep);
    Type* objectType = 0;
    if (_result_objectType.succeeded()) {
        objectType = _result_objectType.getResult();
    }
    else {
        return _Result<TypeQuery, ParserError>(_result_objectType.getError());
    }
    TypeQuery* typeQuery = new(_rp) TypeQuery(objectType, new(_rp) Position(start), lexer->getPosition(_rp));
    objectType->parent = typeQuery;
    return _Result<TypeQuery, ParserError>(typeQuery);
}

_Result<TypeCast, ParserError> Parser::parseTypeCast(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startAs1 = lexer->getPreviousPosition(_rp);
    bool successAs1 = lexer->parseKeyword(asKeyword);
    if (successAs1) {
        lexer->advance();
    }
    else {
        return _Result<TypeCast, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startAs1, &String::create(_ep, asKeyword))));
    }
    _Result<Type, ParserError> _result_objectType = parseType(_rp, _ep);
    Type* objectType = 0;
    if (_result_objectType.succeeded()) {
        objectType = _result_objectType.getResult();
    }
    else {
        return _Result<TypeCast, ParserError>(_result_objectType.getError());
    }
    TypeCast* typeCast = new(_rp) TypeCast(objectType, new(_rp) Position(start), lexer->getPosition(_rp));
    objectType->parent = typeCast;
    return _Result<TypeCast, ParserError>(typeCast);
}

_Result<_Vector<CatchClause>, ParserError> Parser::parseCatchClauseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startCatch1 = lexer->getPreviousPosition(_rp);
    bool successCatch1 = lexer->parseKeyword(catchKeyword);
    if (successCatch1) {
        lexer->advance();
    }
    else {
        return _Result<CatchClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startCatch1, &String::create(_ep, catchKeyword))));
    }
    _Result<CatchPattern, ParserError> _result_catchPattern = parseCatchPattern(_rp, _ep);
    CatchPattern* catchPattern = 0;
    if (_result_catchPattern.succeeded()) {
        catchPattern = _result_catchPattern.getResult();
    }
    else {
        return _Result<CatchClause, ParserError>(_result_catchPattern.getError());
    }
    _Result<TuplePattern, ParserError> _result_bindingPattern = parseTuplePattern(_rp, _ep);
    TuplePattern* bindingPattern = 0;
    if (_result_bindingPattern.succeeded()) {
        bindingPattern = _result_bindingPattern.getResult();
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<CatchClause, ParserError>(_result_expression.getError());
    }
    CatchClause* catchClause = new(_rp) CatchClause(catchPattern, bindingPattern, expression, new(_rp) Position(start), lexer->getPosition(_rp));
    catchPattern->parent = catchClause;
    if (bindingPattern)
        bindingPattern->parent = catchClause;
    expression->parent = catchClause;
    return _Result<CatchClause, ParserError>(catchClause);
}

_Result<CatchPattern, ParserError> Parser::parseCatchPattern(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WildCardCatchPattern, ParserError> result = parseWildCardCatchPattern(_rp, _p);
        if (result.succeeded())
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<PathItemCatchPattern, ParserError> result = parsePathItemCatchPattern(_rp, _p);
        if (result.succeeded())
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CatchPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<WildCardCatchPattern, ParserError> Parser::parseWildCardCatchPattern(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<WildcardPattern, ParserError> _result_pattern = parseWildcardPattern(_rp, _ep);
    WildcardPattern* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<WildCardCatchPattern, ParserError>(_result_pattern.getError());
    }
    WildCardCatchPattern* wildCardCatchPattern = new(_rp) WildCardCatchPattern(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = wildCardCatchPattern;
    return _Result<WildCardCatchPattern, ParserError>(wildCardCatchPattern);
}

_Result<PathItemCatchPattern, ParserError> Parser::parsePathItemCatchPattern(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<PathItem, ParserError> _result_catchCase = parsePathItem(_rp, _ep);
    PathItem* catchCase = 0;
    if (_result_catchCase.succeeded()) {
        catchCase = _result_catchCase.getResult();
    }
    else {
        return _Result<PathItemCatchPattern, ParserError>(_result_catchCase.getError());
    }
    _Result<_Vector<PathIdentifier>, ParserError> _result_catchCaseExtensions = parsePathIdentifierList(_rp, _ep);
    _Vector<PathIdentifier>* catchCaseExtensions = 0;
    if (_result_catchCaseExtensions.succeeded()) {
        catchCaseExtensions = _result_catchCaseExtensions.getResult();
    }
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OperatorPostfix, ParserError> result = parseOperatorPostfix(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<FunctionCall, ParserError> result = parseFunctionCall(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ExplicitMemberExpression, ParserError> result = parseExplicitMemberExpression(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Subscript, ParserError> result = parseSubscript(_rp, _p);
        if (result.succeeded())
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Postfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<OperatorPostfix, ParserError> Parser::parseOperatorPostfix(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startPostfixOperator = lexer->getPreviousPosition(_rp);
    String* postfixOperator = lexer->parsePostfixOperator(_rp);
    if (postfixOperator) {
        lexer->advance();
    }
    else {
        return _Result<OperatorPostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_operatorExpected(startPostfixOperator)));
    }
    OperatorPostfix* operatorPostfix = new(_rp) OperatorPostfix(postfixOperator, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<OperatorPostfix, ParserError>(operatorPostfix);
}

_Result<FunctionCall, ParserError> Parser::parseFunctionCall(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<ParenthesizedExpression, ParserError> _result_arguments = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments = 0;
    if (_result_arguments.succeeded()) {
        arguments = _result_arguments.getResult();
    }
    else {
        return _Result<FunctionCall, ParserError>(_result_arguments.getError());
    }
    _Result<_Vector<CatchClause>, ParserError> _result_catchClauses = parseCatchClauseList(_rp, _ep);
    _Vector<CatchClause>* catchClauses = 0;
    if (_result_catchClauses.succeeded()) {
        catchClauses = _result_catchClauses.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startDot1 = lexer->getPreviousPosition(_rp);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1) {
        lexer->advance();
    }
    else {
        return _Result<ExplicitMemberExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startDot1, &String::create(_ep, dot))));
    }
    _Result<MemberPostfix, ParserError> _result_memberPostfix = parseMemberPostfix(_rp, _ep);
    MemberPostfix* memberPostfix = 0;
    if (_result_memberPostfix.succeeded()) {
        memberPostfix = _result_memberPostfix.getResult();
    }
    else {
        return _Result<ExplicitMemberExpression, ParserError>(_result_memberPostfix.getError());
    }
    ExplicitMemberExpression* explicitMemberExpression = new(_rp) ExplicitMemberExpression(memberPostfix, new(_rp) Position(start), lexer->getPosition(_rp));
    memberPostfix->parent = explicitMemberExpression;
    return _Result<ExplicitMemberExpression, ParserError>(explicitMemberExpression);
}

_Result<Subscript, ParserError> Parser::parseSubscript(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftBracket1 = lexer->getPreviousPosition(_rp);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1) {
        lexer->advance();
    }
    else {
        return _Result<Subscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftBracket1, &String::create(_ep, leftBracket))));
    }
    _Result<_Vector<ExpressionElement>, ParserError> _result_expressions = parseExpressionElementList(_rp, _ep);
    _Vector<ExpressionElement>* expressions = 0;
    if (_result_expressions.succeeded()) {
        expressions = _result_expressions.getResult();
    }
    else {
        return _Result<Subscript, ParserError>(_result_expressions.getError());
    }
    Position* startRightBracket3 = lexer->getPreviousPosition(_rp);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3) {
        lexer->advance();
    }
    else {
        return _Result<Subscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightBracket3, &String::create(_ep, rightBracket))));
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
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<ExpressionElement, ParserError>(_result_expression.getError());
    }
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2) {
        lexer->advance();
    }
    ExpressionElement* expressionElement = new(_rp) ExpressionElement(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = expressionElement;
    return _Result<ExpressionElement, ParserError>(expressionElement);
}

_Result<MemberPostfix, ParserError> Parser::parseMemberPostfix(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NamedMemberPostfix, ParserError> result = parseNamedMemberPostfix(_rp, _p);
        if (result.succeeded())
            return _Result<MemberPostfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<MemberPostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<NamedMemberPostfix, ParserError> Parser::parseNamedMemberPostfix(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<IdentifierExpression, ParserError> _result_identifier = parseIdentifierExpression(_rp, _ep);
    IdentifierExpression* identifier = 0;
    if (_result_identifier.succeeded()) {
        identifier = _result_identifier.getResult();
    }
    else {
        return _Result<NamedMemberPostfix, ParserError>(_result_identifier.getError());
    }
    NamedMemberPostfix* namedMemberPostfix = new(_rp) NamedMemberPostfix(identifier, new(_rp) Position(start), lexer->getPosition(_rp));
    identifier->parent = namedMemberPostfix;
    return _Result<NamedMemberPostfix, ParserError>(namedMemberPostfix);
}

_Result<PrimaryExpression, ParserError> Parser::parsePrimaryExpression(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierExpression, ParserError> result = parseIdentifierExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<LiteralExpression, ParserError> result = parseLiteralExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IfExpression, ParserError> result = parseIfExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SwitchExpression, ParserError> result = parseSwitchExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ForExpression, ParserError> result = parseForExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WhileExpression, ParserError> result = parseWhileExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<RepeatExpression, ParserError> result = parseRepeatExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ParenthesizedExpression, ParserError> result = parseParenthesizedExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ReturnExpression, ParserError> result = parseReturnExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThrowExpression, ParserError> result = parseThrowExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BreakExpression, ParserError> result = parseBreakExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<InitializerCall, ParserError> result = parseInitializerCall(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisExpression, ParserError> result = parseThisExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperExpression, ParserError> result = parseSuperExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NullExpression, ParserError> result = parseNullExpression(_rp, _p);
        if (result.succeeded())
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<PrimaryExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<IdentifierExpression, ParserError> Parser::parseIdentifierExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<IdentifierExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    IdentifierExpression* identifierExpression = new(_rp) IdentifierExpression(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<IdentifierExpression, ParserError>(identifierExpression);
}

_Result<LiteralExpression, ParserError> Parser::parseLiteralExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLiteral = lexer->getPreviousPosition(_rp);
    Literal* literal = lexer->parseLiteral(_rp);
    if (literal) {
        lexer->advance();
    }
    else {
        return _Result<LiteralExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_literalExpected(startLiteral)));
    }
    LiteralExpression* literalExpression = new(_rp) LiteralExpression(literal, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<LiteralExpression, ParserError>(literalExpression);
}

_Result<IfExpression, ParserError> Parser::parseIfExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startIf1 = lexer->getPreviousPosition(_rp);
    bool successIf1 = lexer->parseKeyword(ifKeyword);
    if (successIf1) {
        lexer->advance();
    }
    else {
        return _Result<IfExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startIf1, &String::create(_ep, ifKeyword))));
    }
    _Result<Expression, ParserError> _result_condition = parseExpression(_rp, _ep);
    Expression* condition = 0;
    if (_result_condition.succeeded()) {
        condition = _result_condition.getResult();
    }
    else {
        return _Result<IfExpression, ParserError>(_result_condition.getError());
    }
    _Result<Expression, ParserError> _result_consequent = parseExpression(_rp, _ep);
    Expression* consequent = 0;
    if (_result_consequent.succeeded()) {
        consequent = _result_consequent.getResult();
    }
    else {
        return _Result<IfExpression, ParserError>(_result_consequent.getError());
    }
    _Result<ElseClause, ParserError> _result_elseClause = parseElseClause(_rp, _ep);
    ElseClause* elseClause = 0;
    if (_result_elseClause.succeeded()) {
        elseClause = _result_elseClause.getResult();
    }
    IfExpression* ifExpression = new(_rp) IfExpression(condition, consequent, elseClause, new(_rp) Position(start), lexer->getPosition(_rp));
    condition->parent = ifExpression;
    consequent->parent = ifExpression;
    if (elseClause)
        elseClause->parent = ifExpression;
    return _Result<IfExpression, ParserError>(ifExpression);
}

_Result<SwitchExpression, ParserError> Parser::parseSwitchExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startSwitch1 = lexer->getPreviousPosition(_rp);
    bool successSwitch1 = lexer->parseKeyword(switchKeyword);
    if (successSwitch1) {
        lexer->advance();
    }
    else {
        return _Result<SwitchExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startSwitch1, &String::create(_ep, switchKeyword))));
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<SwitchExpression, ParserError>(_result_expression.getError());
    }
    _Result<SwitchBody, ParserError> _result_body = parseSwitchBody(_rp, _ep);
    SwitchBody* body = 0;
    if (_result_body.succeeded()) {
        body = _result_body.getResult();
    }
    else {
        return _Result<SwitchExpression, ParserError>(_result_body.getError());
    }
    SwitchExpression* switchExpression = new(_rp) SwitchExpression(expression, body, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = switchExpression;
    body->parent = switchExpression;
    return _Result<SwitchExpression, ParserError>(switchExpression);
}

_Result<ForExpression, ParserError> Parser::parseForExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startFor1 = lexer->getPreviousPosition(_rp);
    bool successFor1 = lexer->parseKeyword(forKeyword);
    if (successFor1) {
        lexer->advance();
    }
    else {
        return _Result<ForExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startFor1, &String::create(_ep, forKeyword))));
    }
    _Result<Pattern, ParserError> _result_pattern = parsePattern(_rp, _ep);
    Pattern* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<ForExpression, ParserError>(_result_pattern.getError());
    }
    Position* startIn3 = lexer->getPreviousPosition(_rp);
    bool successIn3 = lexer->parseKeyword(inKeyword);
    if (successIn3) {
        lexer->advance();
    }
    else {
        return _Result<ForExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startIn3, &String::create(_ep, inKeyword))));
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<ForExpression, ParserError>(_result_expression.getError());
    }
    _Result<Expression, ParserError> _result_code = parseExpression(_rp, _ep);
    Expression* code = 0;
    if (_result_code.succeeded()) {
        code = _result_code.getResult();
    }
    else {
        return _Result<ForExpression, ParserError>(_result_code.getError());
    }
    ForExpression* forExpression = new(_rp) ForExpression(pattern, expression, code, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = forExpression;
    expression->parent = forExpression;
    code->parent = forExpression;
    return _Result<ForExpression, ParserError>(forExpression);
}

_Result<WhileExpression, ParserError> Parser::parseWhileExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startWhile1 = lexer->getPreviousPosition(_rp);
    bool successWhile1 = lexer->parseKeyword(whileKeyword);
    if (successWhile1) {
        lexer->advance();
    }
    else {
        return _Result<WhileExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startWhile1, &String::create(_ep, whileKeyword))));
    }
    _Result<Expression, ParserError> _result_condition = parseExpression(_rp, _ep);
    Expression* condition = 0;
    if (_result_condition.succeeded()) {
        condition = _result_condition.getResult();
    }
    else {
        return _Result<WhileExpression, ParserError>(_result_condition.getError());
    }
    _Result<Expression, ParserError> _result_code = parseExpression(_rp, _ep);
    Expression* code = 0;
    if (_result_code.succeeded()) {
        code = _result_code.getResult();
    }
    else {
        return _Result<WhileExpression, ParserError>(_result_code.getError());
    }
    WhileExpression* whileExpression = new(_rp) WhileExpression(condition, code, new(_rp) Position(start), lexer->getPosition(_rp));
    condition->parent = whileExpression;
    code->parent = whileExpression;
    return _Result<WhileExpression, ParserError>(whileExpression);
}

_Result<RepeatExpression, ParserError> Parser::parseRepeatExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startRepeat1 = lexer->getPreviousPosition(_rp);
    bool successRepeat1 = lexer->parseKeyword(repeatKeyword);
    if (successRepeat1) {
        lexer->advance();
    }
    else {
        return _Result<RepeatExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startRepeat1, &String::create(_ep, repeatKeyword))));
    }
    _Result<Expression, ParserError> _result_code = parseExpression(_rp, _ep);
    Expression* code = 0;
    if (_result_code.succeeded()) {
        code = _result_code.getResult();
    }
    else {
        return _Result<RepeatExpression, ParserError>(_result_code.getError());
    }
    Position* startWhile3 = lexer->getPreviousPosition(_rp);
    bool successWhile3 = lexer->parseKeyword(whileKeyword);
    if (successWhile3) {
        lexer->advance();
    }
    else {
        return _Result<RepeatExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startWhile3, &String::create(_ep, whileKeyword))));
    }
    _Result<Expression, ParserError> _result_condition = parseExpression(_rp, _ep);
    Expression* condition = 0;
    if (_result_condition.succeeded()) {
        condition = _result_condition.getResult();
    }
    else {
        return _Result<RepeatExpression, ParserError>(_result_condition.getError());
    }
    RepeatExpression* repeatExpression = new(_rp) RepeatExpression(code, condition, new(_rp) Position(start), lexer->getPosition(_rp));
    code->parent = repeatExpression;
    condition->parent = repeatExpression;
    return _Result<RepeatExpression, ParserError>(repeatExpression);
}

_Result<ParenthesizedExpression, ParserError> Parser::parseParenthesizedExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftParen1 = lexer->getPreviousPosition(_rp);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1) {
        lexer->advance();
    }
    else {
        return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftParen1, &String::create(_ep, leftParen))));
    }
    _Result<_Vector<ExpressionElement>, ParserError> _result_expressionElements = parseExpressionElementList(_rp, _ep);
    _Vector<ExpressionElement>* expressionElements = 0;
    if (_result_expressionElements.succeeded()) {
        expressionElements = _result_expressionElements.getResult();
    }
    Position* startRightParen3 = lexer->getPreviousPosition(_rp);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3) {
        lexer->advance();
    }
    else {
        return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightParen3, &String::create(_ep, rightParen))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startReturn1 = lexer->getPreviousPosition(_rp);
    bool successReturn1 = lexer->parseKeyword(returnKeyword);
    if (successReturn1) {
        lexer->advance();
    }
    else {
        return _Result<ReturnExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startReturn1, &String::create(_ep, returnKeyword))));
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    ReturnExpression* returnExpression = new(_rp) ReturnExpression(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expression)
        expression->parent = returnExpression;
    return _Result<ReturnExpression, ParserError>(returnExpression);
}

_Result<ThrowExpression, ParserError> Parser::parseThrowExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startThrow1 = lexer->getPreviousPosition(_rp);
    bool successThrow1 = lexer->parseKeyword(throwKeyword);
    if (successThrow1) {
        lexer->advance();
    }
    else {
        return _Result<ThrowExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startThrow1, &String::create(_ep, throwKeyword))));
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    ThrowExpression* throwExpression = new(_rp) ThrowExpression(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expression)
        expression->parent = throwExpression;
    return _Result<ThrowExpression, ParserError>(throwExpression);
}

_Result<BreakExpression, ParserError> Parser::parseBreakExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startBreak1 = lexer->getPreviousPosition(_rp);
    bool successBreak1 = lexer->parseKeyword(breakKeyword);
    if (successBreak1) {
        lexer->advance();
    }
    else {
        return _Result<BreakExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startBreak1, &String::create(_ep, breakKeyword))));
    }
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    BreakExpression* breakExpression = new(_rp) BreakExpression(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    if (expression)
        expression->parent = breakExpression;
    return _Result<BreakExpression, ParserError>(breakExpression);
}

_Result<InitializerCall, ParserError> Parser::parseInitializerCall(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Type, ParserError> _result_typeToInitialize = parseType(_rp, _ep);
    Type* typeToInitialize = 0;
    if (_result_typeToInitialize.succeeded()) {
        typeToInitialize = _result_typeToInitialize.getResult();
    }
    else {
        return _Result<InitializerCall, ParserError>(_result_typeToInitialize.getError());
    }
    _Result<ParenthesizedExpression, ParserError> _result_arguments = parseParenthesizedExpression(_rp, _ep);
    ParenthesizedExpression* arguments = 0;
    if (_result_arguments.succeeded()) {
        arguments = _result_arguments.getResult();
    }
    else {
        return _Result<InitializerCall, ParserError>(_result_arguments.getError());
    }
    _Result<_Vector<CatchClause>, ParserError> _result_catchClauses = parseCatchClauseList(_rp, _ep);
    _Vector<CatchClause>* catchClauses = 0;
    if (_result_catchClauses.succeeded()) {
        catchClauses = _result_catchClauses.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startThis1 = lexer->getPreviousPosition(_rp);
    bool successThis1 = lexer->parseKeyword(thisKeyword);
    if (successThis1) {
        lexer->advance();
    }
    else {
        return _Result<ThisExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startThis1, &String::create(_ep, thisKeyword))));
    }
    ThisExpression* thisExpression = new(_rp) ThisExpression(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<ThisExpression, ParserError>(thisExpression);
}

_Result<SuperExpression, ParserError> Parser::parseSuperExpression(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperDot, ParserError> result = parseSuperDot(_rp, _p);
        if (result.succeeded())
            return _Result<SuperExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperSubscript, ParserError> result = parseSuperSubscript(_rp, _p);
        if (result.succeeded())
            return _Result<SuperExpression, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<SuperExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<SuperDot, ParserError> Parser::parseSuperDot(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startSuper1 = lexer->getPreviousPosition(_rp);
    bool successSuper1 = lexer->parseKeyword(superKeyword);
    if (successSuper1) {
        lexer->advance();
    }
    else {
        return _Result<SuperDot, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startSuper1, &String::create(_ep, superKeyword))));
    }
    Position* startDot2 = lexer->getPreviousPosition(_rp);
    bool successDot2 = lexer->parsePunctuation(dot);
    if (successDot2) {
        lexer->advance();
    }
    else {
        return _Result<SuperDot, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startDot2, &String::create(_ep, dot))));
    }
    _Result<CommonSuperMember, ParserError> _result_member = parseCommonSuperMember(_rp, _ep);
    CommonSuperMember* member = 0;
    if (_result_member.succeeded()) {
        member = _result_member.getResult();
    }
    else {
        return _Result<SuperDot, ParserError>(_result_member.getError());
    }
    SuperDot* superDot = new(_rp) SuperDot(member, new(_rp) Position(start), lexer->getPosition(_rp));
    member->parent = superDot;
    return _Result<SuperDot, ParserError>(superDot);
}

_Result<SuperSubscript, ParserError> Parser::parseSuperSubscript(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startSuper1 = lexer->getPreviousPosition(_rp);
    bool successSuper1 = lexer->parseKeyword(superKeyword);
    if (successSuper1) {
        lexer->advance();
    }
    else {
        return _Result<SuperSubscript, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startSuper1, &String::create(_ep, superKeyword))));
    }
    _Result<Subscript, ParserError> _result_subscript = parseSubscript(_rp, _ep);
    Subscript* subscript = 0;
    if (_result_subscript.succeeded()) {
        subscript = _result_subscript.getResult();
    }
    else {
        return _Result<SuperSubscript, ParserError>(_result_subscript.getError());
    }
    SuperSubscript* superSubscript = new(_rp) SuperSubscript(subscript, new(_rp) Position(start), lexer->getPosition(_rp));
    subscript->parent = superSubscript;
    return _Result<SuperSubscript, ParserError>(superSubscript);
}

_Result<NullExpression, ParserError> Parser::parseNullExpression(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startNull1 = lexer->getPreviousPosition(_rp);
    bool successNull1 = lexer->parseKeyword(nullKeyword);
    if (successNull1) {
        lexer->advance();
    }
    else {
        return _Result<NullExpression, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startNull1, &String::create(_ep, nullKeyword))));
    }
    NullExpression* nullExpression = new(_rp) NullExpression(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<NullExpression, ParserError>(nullExpression);
}

_Result<ElseClause, ParserError> Parser::parseElseClause(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startElse1 = lexer->getPreviousPosition(_rp);
    bool successElse1 = lexer->parseKeyword(elseKeyword);
    if (successElse1) {
        lexer->advance();
    }
    else {
        return _Result<ElseClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startElse1, &String::create(_ep, elseKeyword))));
    }
    _Result<Expression, ParserError> _result_alternative = parseExpression(_rp, _ep);
    Expression* alternative = 0;
    if (_result_alternative.succeeded()) {
        alternative = _result_alternative.getResult();
    }
    else {
        return _Result<ElseClause, ParserError>(_result_alternative.getError());
    }
    ElseClause* elseClause = new(_rp) ElseClause(alternative, new(_rp) Position(start), lexer->getPosition(_rp));
    alternative->parent = elseClause;
    return _Result<ElseClause, ParserError>(elseClause);
}

_Result<SwitchBody, ParserError> Parser::parseSwitchBody(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<CurliedSwitchBody, ParserError> result = parseCurliedSwitchBody(_rp, _p);
        if (result.succeeded())
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NakedSwitchBody, ParserError> result = parseNakedSwitchBody(_rp, _p);
        if (result.succeeded())
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<SwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<CurliedSwitchBody, ParserError> Parser::parseCurliedSwitchBody(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftCurly1 = lexer->getPreviousPosition(_rp);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1) {
        lexer->advance();
    }
    else {
        return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftCurly1, &String::create(_ep, leftCurly))));
    }
    _Result<_Vector<SwitchCase>, ParserError> _result_cases = parseSwitchCaseList(_rp, _ep);
    _Vector<SwitchCase>* cases = 0;
    if (_result_cases.succeeded()) {
        cases = _result_cases.getResult();
    }
    else {
        return _Result<CurliedSwitchBody, ParserError>(_result_cases.getError());
    }
    Position* startRightCurly3 = lexer->getPreviousPosition(_rp);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3) {
        lexer->advance();
    }
    else {
        return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightCurly3, &String::create(_ep, rightCurly))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<_Vector<SwitchCase>, ParserError> _result_cases = parseSwitchCaseList(_rp, _ep);
    _Vector<SwitchCase>* cases = 0;
    if (_result_cases.succeeded()) {
        cases = _result_cases.getResult();
    }
    else {
        return _Result<NakedSwitchBody, ParserError>(_result_cases.getError());
    }
    NakedSwitchBody* nakedSwitchBody = new(_rp) NakedSwitchBody(cases, new(_rp) Position(start), lexer->getPosition(_rp));
    if (cases) {
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++)
            (*(*cases)[_i])->parent = nakedSwitchBody;
    }
    return _Result<NakedSwitchBody, ParserError>(nakedSwitchBody);
}

_Result<_Vector<SwitchCase>, ParserError> Parser::parseSwitchCaseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<CaseLabel, ParserError> _result_label = parseCaseLabel(_rp, _ep);
    CaseLabel* label = 0;
    if (_result_label.succeeded()) {
        label = _result_label.getResult();
    }
    else {
        return _Result<SwitchCase, ParserError>(_result_label.getError());
    }
    _Result<CaseContent, ParserError> _result_content = parseCaseContent(_rp, _ep);
    CaseContent* content = 0;
    if (_result_content.succeeded()) {
        content = _result_content.getResult();
    }
    else {
        return _Result<SwitchCase, ParserError>(_result_content.getError());
    }
    SwitchCase* switchCase = new(_rp) SwitchCase(label, content, new(_rp) Position(start), lexer->getPosition(_rp));
    label->parent = switchCase;
    content->parent = switchCase;
    return _Result<SwitchCase, ParserError>(switchCase);
}

_Result<CaseLabel, ParserError> Parser::parseCaseLabel(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ItemCaseLabel, ParserError> result = parseItemCaseLabel(_rp, _p);
        if (result.succeeded())
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<DefaultCaseLabel, ParserError> result = parseDefaultCaseLabel(_rp, _p);
        if (result.succeeded())
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<ItemCaseLabel, ParserError> Parser::parseItemCaseLabel(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startCase1 = lexer->getPreviousPosition(_rp);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1) {
        lexer->advance();
    }
    else {
        return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startCase1, &String::create(_ep, caseKeyword))));
    }
    _Result<Pattern, ParserError> _result_pattern = parsePattern(_rp, _ep);
    Pattern* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<ItemCaseLabel, ParserError>(_result_pattern.getError());
    }
    _Result<_Vector<CaseItem>, ParserError> _result_additionalPatterns = parseCaseItemList(_rp, _ep);
    _Vector<CaseItem>* additionalPatterns = 0;
    if (_result_additionalPatterns.succeeded()) {
        additionalPatterns = _result_additionalPatterns.getResult();
    }
    Position* startColon4 = lexer->getPreviousPosition(_rp);
    bool successColon4 = lexer->parsePunctuation(colon);
    if (successColon4) {
        lexer->advance();
    }
    else {
        return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startColon4, &String::create(_ep, colon))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startDefault1 = lexer->getPreviousPosition(_rp);
    bool successDefault1 = lexer->parseKeyword(defaultKeyword);
    if (successDefault1) {
        lexer->advance();
    }
    else {
        return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startDefault1, &String::create(_ep, defaultKeyword))));
    }
    Position* startColon2 = lexer->getPreviousPosition(_rp);
    bool successColon2 = lexer->parsePunctuation(colon);
    if (successColon2) {
        lexer->advance();
    }
    else {
        return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startColon2, &String::create(_ep, colon))));
    }
    DefaultCaseLabel* defaultCaseLabel = new(_rp) DefaultCaseLabel(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<DefaultCaseLabel, ParserError>(defaultCaseLabel);
}

_Result<_Vector<CaseItem>, ParserError> Parser::parseCaseItemList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startComma1 = lexer->getPreviousPosition(_rp);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1) {
        lexer->advance();
    }
    else {
        return _Result<CaseItem, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startComma1, &String::create(_ep, comma))));
    }
    _Result<Pattern, ParserError> _result_pattern = parsePattern(_rp, _ep);
    Pattern* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<CaseItem, ParserError>(_result_pattern.getError());
    }
    CaseItem* caseItem = new(_rp) CaseItem(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = caseItem;
    return _Result<CaseItem, ParserError>(caseItem);
}

_Result<Pattern, ParserError> Parser::parsePattern(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WildcardPattern, ParserError> result = parseWildcardPattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierPattern, ParserError> result = parseIdentifierPattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TuplePattern, ParserError> result = parseTuplePattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ExpressionPattern, ParserError> result = parseExpressionPattern(_rp, _p);
        if (result.succeeded())
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Pattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<WildcardPattern, ParserError> Parser::parseWildcardPattern(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startUnderscore1 = lexer->getPreviousPosition(_rp);
    bool successUnderscore1 = lexer->parsePunctuation(underscore);
    if (successUnderscore1) {
        lexer->advance();
    }
    else {
        return _Result<WildcardPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startUnderscore1, &String::create(_ep, underscore))));
    }
    WildcardPattern* wildcardPattern = new(_rp) WildcardPattern(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<WildcardPattern, ParserError>(wildcardPattern);
}

_Result<IdentifierPattern, ParserError> Parser::parseIdentifierPattern(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startIdentifier = lexer->getPreviousPosition(_rp);
    String* identifier = lexer->parseIdentifier(_rp);
    if ((identifier) && (isIdentifier(identifier))) {
        lexer->advance();
    }
    else {
        return _Result<IdentifierPattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startIdentifier)));
    }
    _Result<TypeAnnotation, ParserError> _result_annotationForType = parseTypeAnnotation(_rp, _ep);
    TypeAnnotation* annotationForType = 0;
    if (_result_annotationForType.succeeded()) {
        annotationForType = _result_annotationForType.getResult();
    }
    IdentifierPattern* identifierPattern = new(_rp) IdentifierPattern(identifier, annotationForType, new(_rp) Position(start), lexer->getPosition(_rp));
    if (annotationForType)
        annotationForType->parent = identifierPattern;
    return _Result<IdentifierPattern, ParserError>(identifierPattern);
}

_Result<TuplePattern, ParserError> Parser::parseTuplePattern(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftParen1 = lexer->getPreviousPosition(_rp);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1) {
        lexer->advance();
    }
    else {
        return _Result<TuplePattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftParen1, &String::create(_ep, leftParen))));
    }
    _Result<_Vector<TuplePatternElement>, ParserError> _result_elements = parseTuplePatternElementList(_rp, _ep);
    _Vector<TuplePatternElement>* elements = 0;
    if (_result_elements.succeeded()) {
        elements = _result_elements.getResult();
    }
    else {
        return _Result<TuplePattern, ParserError>(_result_elements.getError());
    }
    Position* startRightParen3 = lexer->getPreviousPosition(_rp);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3) {
        lexer->advance();
    }
    else {
        return _Result<TuplePattern, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightParen3, &String::create(_ep, rightParen))));
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Expression, ParserError> _result_expression = parseExpression(_rp, _ep);
    Expression* expression = 0;
    if (_result_expression.succeeded()) {
        expression = _result_expression.getResult();
    }
    else {
        return _Result<ExpressionPattern, ParserError>(_result_expression.getError());
    }
    ExpressionPattern* expressionPattern = new(_rp) ExpressionPattern(expression, new(_rp) Position(start), lexer->getPosition(_rp));
    expression->parent = expressionPattern;
    return _Result<ExpressionPattern, ParserError>(expressionPattern);
}

_Result<_Vector<TuplePatternElement>, ParserError> Parser::parseTuplePatternElementList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<Pattern, ParserError> _result_pattern = parsePattern(_rp, _ep);
    Pattern* pattern = 0;
    if (_result_pattern.succeeded()) {
        pattern = _result_pattern.getResult();
    }
    else {
        return _Result<TuplePatternElement, ParserError>(_result_pattern.getError());
    }
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2) {
        lexer->advance();
    }
    TuplePatternElement* tuplePatternElement = new(_rp) TuplePatternElement(pattern, new(_rp) Position(start), lexer->getPosition(_rp));
    pattern->parent = tuplePatternElement;
    return _Result<TuplePatternElement, ParserError>(tuplePatternElement);
}

_Result<CaseContent, ParserError> Parser::parseCaseContent(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BlockCaseContent, ParserError> result = parseBlockCaseContent(_rp, _p);
        if (result.succeeded())
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<EmptyCaseContent, ParserError> result = parseEmptyCaseContent(_rp, _p);
        if (result.succeeded())
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CaseContent, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<BlockCaseContent, ParserError> Parser::parseBlockCaseContent(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<_Vector<TerminatedStatement>, ParserError> _result_statements = parseTerminatedStatementList(_rp, _ep);
    _Vector<TerminatedStatement>* statements = 0;
    if (_result_statements.succeeded()) {
        statements = _result_statements.getResult();
    }
    else {
        return _Result<BlockCaseContent, ParserError>(_result_statements.getError());
    }
    BlockCaseContent* blockCaseContent = new(_rp) BlockCaseContent(statements, new(_rp) Position(start), lexer->getPosition(_rp));
    if (statements) {
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++)
            (*(*statements)[_i])->parent = blockCaseContent;
    }
    return _Result<BlockCaseContent, ParserError>(blockCaseContent);
}

_Result<EmptyCaseContent, ParserError> Parser::parseEmptyCaseContent(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startSemicolon1 = lexer->getPreviousPosition(_rp);
    bool successSemicolon1 = lexer->parsePunctuation(semicolon);
    if (successSemicolon1) {
        lexer->advance();
    }
    else {
        return _Result<EmptyCaseContent, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startSemicolon1, &String::create(_ep, semicolon))));
    }
    EmptyCaseContent* emptyCaseContent = new(_rp) EmptyCaseContent(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<EmptyCaseContent, ParserError>(emptyCaseContent);
}

_Result<CommonSuperMember, ParserError> Parser::parseCommonSuperMember(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperInit, ParserError> result = parseSuperInit(_rp, _p);
        if (result.succeeded())
            return _Result<CommonSuperMember, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperMember, ParserError> result = parseSuperMember(_rp, _p);
        if (result.succeeded())
            return _Result<CommonSuperMember, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<CommonSuperMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<SuperInit, ParserError> Parser::parseSuperInit(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startInit1 = lexer->getPreviousPosition(_rp);
    bool successInit1 = lexer->parseKeyword(initKeyword);
    if (successInit1) {
        lexer->advance();
    }
    else {
        return _Result<SuperInit, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_keywordExpected(startInit1, &String::create(_ep, initKeyword))));
    }
    SuperInit* superInit = new(_rp) SuperInit(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<SuperInit, ParserError>(superInit);
}

_Result<SuperMember, ParserError> Parser::parseSuperMember(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<SuperMember, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    SuperMember* superMember = new(_rp) SuperMember(name, new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<SuperMember, ParserError>(superMember);
}

_Result<Type, ParserError> Parser::parseType(_Page* _rp, _Page* _ep) {
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ArrayType, ParserError> result = parseArrayType(_rp, _p);
        if (result.succeeded())
            return _Result<Type, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _p);
        if (result.succeeded())
            return _Result<Type, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<Type, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<TypeIdentifier, ParserError> Parser::parseTypeIdentifier(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startName = lexer->getPreviousPosition(_rp);
    String* name = lexer->parseIdentifier(_rp);
    if ((name) && (isIdentifier(name))) {
        lexer->advance();
    }
    else {
        return _Result<TypeIdentifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_identifierExpected(startName)));
    }
    _Result<SubtypeIdentifier, ParserError> _result_subType = parseSubtypeIdentifier(_rp, _ep);
    SubtypeIdentifier* subType = 0;
    if (_result_subType.succeeded()) {
        subType = _result_subType.getResult();
    }
    _Result<_Vector<TypePostfix>, ParserError> _result_postfixes = parseTypePostfixList(_rp, _ep);
    _Vector<TypePostfix>* postfixes = 0;
    if (_result_postfixes.succeeded()) {
        postfixes = _result_postfixes.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startLeftBracket1 = lexer->getPreviousPosition(_rp);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1) {
        lexer->advance();
    }
    else {
        return _Result<ArrayType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startLeftBracket1, &String::create(_ep, leftBracket))));
    }
    _Result<Type, ParserError> _result_elementType = parseType(_rp, _ep);
    Type* elementType = 0;
    if (_result_elementType.succeeded()) {
        elementType = _result_elementType.getResult();
    }
    else {
        return _Result<ArrayType, ParserError>(_result_elementType.getError());
    }
    Position* startRightBracket3 = lexer->getPreviousPosition(_rp);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3) {
        lexer->advance();
    }
    else {
        return _Result<ArrayType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startRightBracket3, &String::create(_ep, rightBracket))));
    }
    _Result<_Vector<TypePostfix>, ParserError> _result_postfixes = parseTypePostfixList(_rp, _ep);
    _Vector<TypePostfix>* postfixes = 0;
    if (_result_postfixes.succeeded()) {
        postfixes = _result_postfixes.getResult();
    }
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
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startColon1 = lexer->getPreviousPosition(_rp);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1) {
        lexer->advance();
    }
    else {
        return _Result<TypeAnnotation, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startColon1, &String::create(_ep, colon))));
    }
    _Result<Type, ParserError> _result_annotationForType = parseType(_rp, _ep);
    Type* annotationForType = 0;
    if (_result_annotationForType.succeeded()) {
        annotationForType = _result_annotationForType.getResult();
    }
    else {
        return _Result<TypeAnnotation, ParserError>(_result_annotationForType.getError());
    }
    TypeAnnotation* typeAnnotation = new(_rp) TypeAnnotation(annotationForType, new(_rp) Position(start), lexer->getPosition(_rp));
    annotationForType->parent = typeAnnotation;
    return _Result<TypeAnnotation, ParserError>(typeAnnotation);
}

_Result<SubtypeIdentifier, ParserError> Parser::parseSubtypeIdentifier(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startDot1 = lexer->getPreviousPosition(_rp);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1) {
        lexer->advance();
    }
    else {
        return _Result<SubtypeIdentifier, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startDot1, &String::create(_ep, dot))));
    }
    _Result<TypeIdentifier, ParserError> _result_typeIdentifier = parseTypeIdentifier(_rp, _ep);
    TypeIdentifier* typeIdentifier = 0;
    if (_result_typeIdentifier.succeeded()) {
        typeIdentifier = _result_typeIdentifier.getResult();
    }
    else {
        return _Result<SubtypeIdentifier, ParserError>(_result_typeIdentifier.getError());
    }
    SubtypeIdentifier* subtypeIdentifier = new(_rp) SubtypeIdentifier(typeIdentifier, new(_rp) Position(start), lexer->getPosition(_rp));
    typeIdentifier->parent = subtypeIdentifier;
    return _Result<SubtypeIdentifier, ParserError>(subtypeIdentifier);
}

_Result<_Vector<TypePostfix>, ParserError> Parser::parseTypePostfixList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    _Array<ParserError>* errors = new(_ep) _Array<ParserError>();
    Position* start = lexer->getPreviousPosition(_rp);
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OptionalType, ParserError> result = parseOptionalType(_rp, _p);
        if (result.succeeded())
            return _Result<TypePostfix, ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
    return _Result<TypePostfix, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<ParserError>::create(_ep, *errors))));
}

_Result<OptionalType, ParserError> Parser::parseOptionalType(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startQuestionMark1 = lexer->getPreviousPosition(_rp);
    bool successQuestionMark1 = lexer->parsePunctuation(questionMark);
    if (successQuestionMark1) {
        lexer->advance();
    }
    else {
        return _Result<OptionalType, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startQuestionMark1, &String::create(_ep, questionMark))));
    }
    OptionalType* optionalType = new(_rp) OptionalType(new(_rp) Position(start), lexer->getPosition(_rp));
    return _Result<OptionalType, ParserError>(optionalType);
}

_Result<TypeInheritanceClause, ParserError> Parser::parseTypeInheritanceClause(_Page* _rp, _Page* _ep) {
    Position* start = lexer->getPreviousPosition(_rp);
    Position* startColon1 = lexer->getPreviousPosition(_rp);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1) {
        lexer->advance();
    }
    else {
        return _Result<TypeInheritanceClause, ParserError>(new(_ep) ParserError(new(_ep) _ParserError_punctuationExpected(startColon1, &String::create(_ep, colon))));
    }
    _Result<_Vector<Inheritance>, ParserError> _result_inheritances = parseInheritanceList(_rp, _ep);
    _Vector<Inheritance>* inheritances = 0;
    if (_result_inheritances.succeeded()) {
        inheritances = _result_inheritances.getResult();
    }
    TypeInheritanceClause* typeInheritanceClause = new(_rp) TypeInheritanceClause(inheritances, new(_rp) Position(start), lexer->getPosition(_rp));
    if (inheritances) {
        size_t _inheritances_length = inheritances->length();
        for (size_t _i = 0; _i < _inheritances_length; _i++)
            (*(*inheritances)[_i])->parent = typeInheritanceClause;
    }
    return _Result<TypeInheritanceClause, ParserError>(typeInheritanceClause);
}

_Result<_Vector<Inheritance>, ParserError> Parser::parseInheritanceList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
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
    Position* start = lexer->getPreviousPosition(_rp);
    _Result<TypeIdentifier, ParserError> _result_typeIdentifier = parseTypeIdentifier(_rp, _ep);
    TypeIdentifier* typeIdentifier = 0;
    if (_result_typeIdentifier.succeeded()) {
        typeIdentifier = _result_typeIdentifier.getResult();
    }
    else {
        return _Result<Inheritance, ParserError>(_result_typeIdentifier.getError());
    }
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
