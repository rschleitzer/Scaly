#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Parser::Parser(String& text)
: lexer(text) {
}

_Result<TopLevelDeclaration, ParserError> Parser::parseTopLevelDeclaration(_Page* _rp, _Page* _ep) {
    TopLevelDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<StatementWithSemicolon>, ParserError> result = parseStatementWithSemicolonList(_rp, _ep);
        if (result.succeeded()) {
            if (!isAtEnd()) {
                Position current = lexer.getPreviousPosition();
                return _Result<TopLevelDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) NotAtEnd(current)));
            }
            if (!ret)
                ret = new(_rp) TopLevelDeclaration(start, lexer.getPosition());

            ret->statements = result.getResult();
        }
        else {
            return _Result<TopLevelDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<TopLevelDeclaration, ParserError>(ret);
}

_Result<Array<StatementWithSemicolon>, ParserError> Parser::parseStatementWithSemicolonList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<StatementWithSemicolon>* ret = 0;

    while (true) {
        _Result<StatementWithSemicolon, ParserError> nodeResult = parseStatementWithSemicolon(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<StatementWithSemicolon>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<StatementWithSemicolon>, ParserError>(ret);
}

_Result<StatementWithSemicolon, ParserError> Parser::parseStatementWithSemicolon(_Page* _rp, _Page* _ep) {
    StatementWithSemicolon* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Statement, ParserError> result = parseStatement(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) StatementWithSemicolon(start, lexer.getPosition());

            ret->statement = result.getResult();
        }
        else {
            return _Result<StatementWithSemicolon, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(semicolon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) StatementWithSemicolon(start, lexer.getPosition());
        }
    }

    return _Result<StatementWithSemicolon, ParserError>(ret);
}

_Result<Statement, ParserError> Parser::parseStatement(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Declaration, ParserError> result = parseDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Expression, ParserError> result = parseExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Statement, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<Declaration, ParserError> Parser::parseDeclaration(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<UseDeclaration, ParserError> result = parseUseDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<VariableDeclaration, ParserError> result = parseVariableDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ConstantDeclaration, ParserError> result = parseConstantDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<FunctionDeclaration, ParserError> result = parseFunctionDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ClassDeclaration, ParserError> result = parseClassDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Declaration, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<UseDeclaration, ParserError> Parser::parseUseDeclaration(_Page* _rp, _Page* _ep) {
    UseDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(useKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) UseDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<UseDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(useKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<PathItem, ParserError> result = parsePathItem(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) UseDeclaration(start, lexer.getPosition());

            ret->importModule = result.getResult();
        }
        else {
            return _Result<UseDeclaration, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<PathIdentifier>, ParserError> result = parsePathIdentifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) UseDeclaration(start, lexer.getPosition());

            ret->importExtensions = result.getResult();
        }
    }

    return _Result<UseDeclaration, ParserError>(ret);
}

_Result<Array<PathIdentifier>, ParserError> Parser::parsePathIdentifierList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<PathIdentifier>* ret = 0;

    while (true) {
        _Result<PathIdentifier, ParserError> nodeResult = parsePathIdentifier(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<PathIdentifier>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<PathIdentifier>, ParserError>(ret);
}

_Result<PathIdentifier, ParserError> Parser::parsePathIdentifier(_Page* _rp, _Page* _ep) {
    PathIdentifier* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(dot)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PathIdentifier(start, lexer.getPosition());
        }
        else {
            return _Result<PathIdentifier, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(dot))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<PathItem, ParserError> result = parsePathItem(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PathIdentifier(start, lexer.getPosition());

            ret->extension = result.getResult();
        }
        else {
            return _Result<PathIdentifier, ParserError>(result.getError());
        }
    }

    return _Result<PathIdentifier, ParserError>(ret);
}

_Result<PathItem, ParserError> Parser::parsePathItem(_Page* _rp, _Page* _ep) {
    PathItem* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* idname = lexer.parseIdentifier(_rp);
        if ((idname) && (isIdentifier(*idname))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PathItem(start, lexer.getPosition());

            ret->name = idname;
        }
        else {
            return _Result<PathItem, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<PathItem, ParserError>(ret);
}

_Result<Initializer, ParserError> Parser::parseInitializer(_Page* _rp, _Page* _ep) {
    Initializer* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(equal)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Initializer(start, lexer.getPosition());
        }
        else {
            return _Result<Initializer, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(equal))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) Initializer(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<Initializer, ParserError>(result.getError());
        }
    }

    return _Result<Initializer, ParserError>(ret);
}

_Result<ConstantDeclaration, ParserError> Parser::parseConstantDeclaration(_Page* _rp, _Page* _ep) {
    ConstantDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(letKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ConstantDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<ConstantDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(letKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<PatternInitializer>, ParserError> result = parsePatternInitializerList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ConstantDeclaration(start, lexer.getPosition());

            ret->patternInitializers = result.getResult();
        }
        else {
            return _Result<ConstantDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<ConstantDeclaration, ParserError>(ret);
}

_Result<Array<PatternInitializer>, ParserError> Parser::parsePatternInitializerList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<PatternInitializer>* ret = 0;

    while (true) {
        _Result<PatternInitializer, ParserError> nodeResult = parsePatternInitializer(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<PatternInitializer>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<PatternInitializer>, ParserError>(ret);
}

_Result<PatternInitializer, ParserError> Parser::parsePatternInitializer(_Page* _rp, _Page* _ep) {
    PatternInitializer* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Pattern, ParserError> result = parsePattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PatternInitializer(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<PatternInitializer, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Initializer, ParserError> result = parseInitializer(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PatternInitializer(start, lexer.getPosition());

            ret->initializer = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(comma)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PatternInitializer(start, lexer.getPosition());
        }
    }

    return _Result<PatternInitializer, ParserError>(ret);
}

_Result<VariableDeclaration, ParserError> Parser::parseVariableDeclaration(_Page* _rp, _Page* _ep) {
    VariableDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(mutableKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) VariableDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<VariableDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(mutableKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<PatternInitializer>, ParserError> result = parsePatternInitializerList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) VariableDeclaration(start, lexer.getPosition());

            ret->patternInitializers = result.getResult();
        }
        else {
            return _Result<VariableDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<VariableDeclaration, ParserError>(ret);
}

_Result<FunctionDeclaration, ParserError> Parser::parseFunctionDeclaration(_Page* _rp, _Page* _ep) {
    FunctionDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<Modifier>, ParserError> result = parseModifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());

            ret->modifiers = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(functionKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<FunctionDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(functionKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<FunctionName, ParserError> result = parseFunctionName(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());

            ret->name = result.getResult();
        }
        else {
            return _Result<FunctionDeclaration, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<FunctionSignature, ParserError> result = parseFunctionSignature(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());

            ret->signature = result.getResult();
        }
        else {
            return _Result<FunctionDeclaration, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());

            ret->body = result.getResult();
        }
        else {
            return _Result<FunctionDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<FunctionDeclaration, ParserError>(ret);
}

_Result<Array<Modifier>, ParserError> Parser::parseModifierList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<Modifier>* ret = 0;

    while (true) {
        _Result<Modifier, ParserError> nodeResult = parseModifier(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<Modifier>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<Modifier>, ParserError>(ret);
}

_Result<Modifier, ParserError> Parser::parseModifier(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Static, ParserError> result = parseStatic(_rp, _p);
        if (result.succeeded()) 
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Modifier, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<Static, ParserError> Parser::parseStatic(_Page* _rp, _Page* _ep) {
    Static* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(staticKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Static(start, lexer.getPosition());
        }
        else {
            return _Result<Static, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(staticKeyword))));
        }
    }

    return _Result<Static, ParserError>(ret);
}

_Result<FunctionName, ParserError> Parser::parseFunctionName(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierFunctionName, ParserError> result = parseIdentifierFunctionName(_rp, _p);
        if (result.succeeded()) 
            return _Result<FunctionName, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<FunctionName, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<IdentifierFunctionName, ParserError> Parser::parseIdentifierFunctionName(_Page* _rp, _Page* _ep) {
    IdentifierFunctionName* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* idname = lexer.parseIdentifier(_rp);
        if ((idname) && (isIdentifier(*idname))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IdentifierFunctionName(start, lexer.getPosition());

            ret->name = idname;
        }
        else {
            return _Result<IdentifierFunctionName, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<IdentifierFunctionName, ParserError>(ret);
}

_Result<FunctionSignature, ParserError> Parser::parseFunctionSignature(_Page* _rp, _Page* _ep) {
    FunctionSignature* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<ParameterClause, ParserError> result = parseParameterClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionSignature(start, lexer.getPosition());

            ret->parameterClause = result.getResult();
        }
        else {
            return _Result<FunctionSignature, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<FunctionResult, ParserError> result = parseFunctionResult(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionSignature(start, lexer.getPosition());

            ret->result = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<ThrowsClause, ParserError> result = parseThrowsClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionSignature(start, lexer.getPosition());

            ret->throwsClause = result.getResult();
        }
    }

    return _Result<FunctionSignature, ParserError>(ret);
}

_Result<FunctionResult, ParserError> Parser::parseFunctionResult(_Page* _rp, _Page* _ep) {
    FunctionResult* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(evaluatesTo)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) FunctionResult(start, lexer.getPosition());
        }
        else {
            return _Result<FunctionResult, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(evaluatesTo))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionResult(start, lexer.getPosition());

            ret->resultType = result.getResult();
        }
        else {
            return _Result<FunctionResult, ParserError>(result.getError());
        }
    }

    return _Result<FunctionResult, ParserError>(ret);
}

_Result<Array<ParameterClause>, ParserError> Parser::parseParameterClauseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<ParameterClause>* ret = 0;

    while (true) {
        _Result<ParameterClause, ParserError> nodeResult = parseParameterClause(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<ParameterClause>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<ParameterClause>, ParserError>(ret);
}

_Result<ParameterClause, ParserError> Parser::parseParameterClause(_Page* _rp, _Page* _ep) {
    ParameterClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftParen)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParameterClause(start, lexer.getPosition());
        }
        else {
            return _Result<ParameterClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<Parameter>, ParserError> result = parseParameterList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ParameterClause(start, lexer.getPosition());

            ret->parameters = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightParen)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParameterClause(start, lexer.getPosition());
        }
        else {
            return _Result<ParameterClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightParen))));
        }
    }

    return _Result<ParameterClause, ParserError>(ret);
}

_Result<Array<Parameter>, ParserError> Parser::parseParameterList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<Parameter>* ret = 0;

    while (true) {
        _Result<Parameter, ParserError> nodeResult = parseParameter(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<Parameter>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<Parameter>, ParserError>(ret);
}

_Result<Parameter, ParserError> Parser::parseParameter(_Page* _rp, _Page* _ep) {
    Parameter* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* idparameterName = lexer.parseIdentifier(_rp);
        if ((idparameterName) && (isIdentifier(*idparameterName))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Parameter(start, lexer.getPosition());

            ret->parameterName = idparameterName;
        }
        else {
            return _Result<Parameter, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(colon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Parameter(start, lexer.getPosition());
        }
        else {
            return _Result<Parameter, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(colon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) Parameter(start, lexer.getPosition());

            ret->parameterType = result.getResult();
        }
        else {
            return _Result<Parameter, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(comma)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Parameter(start, lexer.getPosition());
        }
    }

    return _Result<Parameter, ParserError>(ret);
}

_Result<ThrowsClause, ParserError> Parser::parseThrowsClause(_Page* _rp, _Page* _ep) {
    ThrowsClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(throwsKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThrowsClause(start, lexer.getPosition());
        }
        else {
            return _Result<ThrowsClause, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(throwsKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ThrowsClause(start, lexer.getPosition());

            ret->throwsType = result.getResult();
        }
        else {
            return _Result<ThrowsClause, ParserError>(result.getError());
        }
    }

    return _Result<ThrowsClause, ParserError>(ret);
}

_Result<ClassDeclaration, ParserError> Parser::parseClassDeclaration(_Page* _rp, _Page* _ep) {
    ClassDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(classKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(classKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        String* idname = lexer.parseIdentifier(_rp);
        if ((idname) && (isIdentifier(*idname))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());

            ret->name = idname;
        }
        else {
            return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<GenericArgumentClause, ParserError> result = parseGenericArgumentClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());

            ret->genericArgumentClause = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<TypeInheritanceClause, ParserError> result = parseTypeInheritanceClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());

            ret->typeInheritanceClause = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<ClassBody, ParserError> result = parseClassBody(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());

            ret->body = result.getResult();
        }
        else {
            return _Result<ClassDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<ClassDeclaration, ParserError>(ret);
}

_Result<GenericArgumentClause, ParserError> Parser::parseGenericArgumentClause(_Page* _rp, _Page* _ep) {
    GenericArgumentClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftAngular)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) GenericArgumentClause(start, lexer.getPosition());
        }
        else {
            return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftAngular))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<GenericParameter>, ParserError> result = parseGenericParameterList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) GenericArgumentClause(start, lexer.getPosition());

            ret->genericParameters = result.getResult();
        }
        else {
            return _Result<GenericArgumentClause, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightAngular)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) GenericArgumentClause(start, lexer.getPosition());
        }
        else {
            return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightAngular))));
        }
    }

    return _Result<GenericArgumentClause, ParserError>(ret);
}

_Result<Array<GenericParameter>, ParserError> Parser::parseGenericParameterList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<GenericParameter>* ret = 0;

    while (true) {
        _Result<GenericParameter, ParserError> nodeResult = parseGenericParameter(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<GenericParameter>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<GenericParameter>, ParserError>(ret);
}

_Result<GenericParameter, ParserError> Parser::parseGenericParameter(_Page* _rp, _Page* _ep) {
    GenericParameter* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* idtypeName = lexer.parseIdentifier(_rp);
        if ((idtypeName) && (isIdentifier(*idtypeName))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) GenericParameter(start, lexer.getPosition());

            ret->typeName = idtypeName;
        }
        else {
            return _Result<GenericParameter, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<GenericParameter, ParserError>(ret);
}

_Result<ClassBody, ParserError> Parser::parseClassBody(_Page* _rp, _Page* _ep) {
    ClassBody* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftCurly)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassBody(start, lexer.getPosition());
        }
        else {
            return _Result<ClassBody, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<ClassMember>, ParserError> result = parseClassMemberList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ClassBody(start, lexer.getPosition());

            ret->declarations = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightCurly)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassBody(start, lexer.getPosition());
        }
        else {
            return _Result<ClassBody, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightCurly))));
        }
    }

    return _Result<ClassBody, ParserError>(ret);
}

_Result<Array<ClassMember>, ParserError> Parser::parseClassMemberList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<ClassMember>* ret = 0;

    while (true) {
        _Result<ClassMember, ParserError> nodeResult = parseClassMember(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<ClassMember>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<ClassMember>, ParserError>(ret);
}

_Result<ClassMember, ParserError> Parser::parseClassMember(_Page* _rp, _Page* _ep) {
    ClassMember* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Declaration, ParserError> result = parseDeclaration(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ClassMember(start, lexer.getPosition());

            ret->declaration = result.getResult();
        }
        else {
            return _Result<ClassMember, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(semicolon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassMember(start, lexer.getPosition());
        }
    }

    return _Result<ClassMember, ParserError>(ret);
}

_Result<Expression, ParserError> Parser::parseExpression(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SimpleExpression, ParserError> result = parseSimpleExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<CodeBlock, ParserError> result = parseCodeBlock(_rp, _p);
        if (result.succeeded()) 
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Expression, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<CodeBlock, ParserError> Parser::parseCodeBlock(_Page* _rp, _Page* _ep) {
    CodeBlock* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftCurly)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CodeBlock(start, lexer.getPosition());
        }
        else {
            return _Result<CodeBlock, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<StatementWithSemicolon>, ParserError> result = parseStatementWithSemicolonList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) CodeBlock(start, lexer.getPosition());

            ret->statements = result.getResult();
        }
        else {
            return _Result<CodeBlock, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightCurly)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CodeBlock(start, lexer.getPosition());
        }
        else {
            return _Result<CodeBlock, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightCurly))));
        }
    }

    return _Result<CodeBlock, ParserError>(ret);
}

_Result<SimpleExpression, ParserError> Parser::parseSimpleExpression(_Page* _rp, _Page* _ep) {
    SimpleExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<PrefixExpression, ParserError> result = parsePrefixExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SimpleExpression(start, lexer.getPosition());

            ret->prefixExpression = result.getResult();
        }
        else {
            return _Result<SimpleExpression, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<BinaryOp>, ParserError> result = parseBinaryOpList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SimpleExpression(start, lexer.getPosition());

            ret->binaryOps = result.getResult();
        }
    }

    return _Result<SimpleExpression, ParserError>(ret);
}

_Result<PrefixExpression, ParserError> Parser::parsePrefixExpression(_Page* _rp, _Page* _ep) {
    PrefixExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* prefixOperator = lexer.parsePrefixOperator(_rp); 
        if (prefixOperator) {
            lexer.advance();
            if (!ret)
                ret = new (_rp) PrefixExpression(start, lexer.getPosition()); 

            ret->prefixOperator = prefixOperator;
	    }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<PostfixExpression, ParserError> result = parsePostfixExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PrefixExpression(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<PrefixExpression, ParserError>(result.getError());
        }
    }

    return _Result<PrefixExpression, ParserError>(ret);
}

_Result<PostfixExpression, ParserError> Parser::parsePostfixExpression(_Page* _rp, _Page* _ep) {
    PostfixExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<PrimaryExpression, ParserError> result = parsePrimaryExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PostfixExpression(start, lexer.getPosition());

            ret->primaryExpression = result.getResult();
        }
        else {
            return _Result<PostfixExpression, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<Postfix>, ParserError> result = parsePostfixList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PostfixExpression(start, lexer.getPosition());

            ret->postfixes = result.getResult();
        }
    }

    return _Result<PostfixExpression, ParserError>(ret);
}

_Result<Array<BinaryOp>, ParserError> Parser::parseBinaryOpList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<BinaryOp>* ret = 0;

    while (true) {
        _Result<BinaryOp, ParserError> nodeResult = parseBinaryOp(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<BinaryOp>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<BinaryOp>, ParserError>(ret);
}

_Result<BinaryOp, ParserError> Parser::parseBinaryOp(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BinaryOperation, ParserError> result = parseBinaryOperation(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Assignment, ParserError> result = parseAssignment(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeQuery, ParserError> result = parseTypeQuery(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeCast, ParserError> result = parseTypeCast(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<BinaryOp, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<BinaryOperation, ParserError> Parser::parseBinaryOperation(_Page* _rp, _Page* _ep) {
    BinaryOperation* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* binaryOperator = lexer.parseBinaryOperator(_rp); 
        if (binaryOperator) {
            lexer.advance();
            if (!ret)
                ret = new (_rp) BinaryOperation(start, lexer.getPosition()); 

            ret->binaryOperator = binaryOperator;
	    }
        else {
            return _Result<BinaryOperation, ParserError>(new(_ep) ParserError(*new(_ep) OperatorExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<PrefixExpression, ParserError> result = parsePrefixExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) BinaryOperation(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<BinaryOperation, ParserError>(result.getError());
        }
    }

    return _Result<BinaryOperation, ParserError>(ret);
}

_Result<Assignment, ParserError> Parser::parseAssignment(_Page* _rp, _Page* _ep) {
    Assignment* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(equal)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Assignment(start, lexer.getPosition());
        }
        else {
            return _Result<Assignment, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(equal))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<PrefixExpression, ParserError> result = parsePrefixExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) Assignment(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<Assignment, ParserError>(result.getError());
        }
    }

    return _Result<Assignment, ParserError>(ret);
}

_Result<TypeQuery, ParserError> Parser::parseTypeQuery(_Page* _rp, _Page* _ep) {
    TypeQuery* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(isKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeQuery(start, lexer.getPosition());
        }
        else {
            return _Result<TypeQuery, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(isKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeQuery(start, lexer.getPosition());

            ret->objectType = result.getResult();
        }
        else {
            return _Result<TypeQuery, ParserError>(result.getError());
        }
    }

    return _Result<TypeQuery, ParserError>(ret);
}

_Result<TypeCast, ParserError> Parser::parseTypeCast(_Page* _rp, _Page* _ep) {
    TypeCast* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(asKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeCast(start, lexer.getPosition());
        }
        else {
            return _Result<TypeCast, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(asKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeCast(start, lexer.getPosition());

            ret->objectType = result.getResult();
        }
        else {
            return _Result<TypeCast, ParserError>(result.getError());
        }
    }

    return _Result<TypeCast, ParserError>(ret);
}

_Result<Array<CatchClause>, ParserError> Parser::parseCatchClauseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<CatchClause>* ret = 0;

    while (true) {
        _Result<CatchClause, ParserError> nodeResult = parseCatchClause(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<CatchClause>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<CatchClause>, ParserError>(ret);
}

_Result<CatchClause, ParserError> Parser::parseCatchClause(_Page* _rp, _Page* _ep) {
    CatchClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(catchKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CatchClause(start, lexer.getPosition());
        }
        else {
            return _Result<CatchClause, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(catchKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<CatchPattern, ParserError> result = parseCatchPattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) CatchClause(start, lexer.getPosition());

            ret->catchPattern = result.getResult();
        }
        else {
            return _Result<CatchClause, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<TuplePattern, ParserError> result = parseTuplePattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) CatchClause(start, lexer.getPosition());

            ret->bindingPattern = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) CatchClause(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<CatchClause, ParserError>(result.getError());
        }
    }

    return _Result<CatchClause, ParserError>(ret);
}

_Result<CatchPattern, ParserError> Parser::parseCatchPattern(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WildCardCatchPattern, ParserError> result = parseWildCardCatchPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<PathItemCatchPattern, ParserError> result = parsePathItemCatchPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<CatchPattern, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<WildCardCatchPattern, ParserError> Parser::parseWildCardCatchPattern(_Page* _rp, _Page* _ep) {
    WildCardCatchPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<WildcardPattern, ParserError> result = parseWildcardPattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) WildCardCatchPattern(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<WildCardCatchPattern, ParserError>(result.getError());
        }
    }

    return _Result<WildCardCatchPattern, ParserError>(ret);
}

_Result<PathItemCatchPattern, ParserError> Parser::parsePathItemCatchPattern(_Page* _rp, _Page* _ep) {
    PathItemCatchPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<PathItem, ParserError> result = parsePathItem(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PathItemCatchPattern(start, lexer.getPosition());

            ret->catchCase = result.getResult();
        }
        else {
            return _Result<PathItemCatchPattern, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<PathIdentifier>, ParserError> result = parsePathIdentifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PathItemCatchPattern(start, lexer.getPosition());

            ret->catchCaseExtensions = result.getResult();
        }
    }

    return _Result<PathItemCatchPattern, ParserError>(ret);
}

_Result<Array<Postfix>, ParserError> Parser::parsePostfixList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<Postfix>* ret = 0;

    while (true) {
        _Result<Postfix, ParserError> nodeResult = parsePostfix(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<Postfix>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<Postfix>, ParserError>(ret);
}

_Result<Postfix, ParserError> Parser::parsePostfix(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OperatorPostfix, ParserError> result = parseOperatorPostfix(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<FunctionCall, ParserError> result = parseFunctionCall(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ExplicitMemberExpression, ParserError> result = parseExplicitMemberExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Subscript, ParserError> result = parseSubscript(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Postfix, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<OperatorPostfix, ParserError> Parser::parseOperatorPostfix(_Page* _rp, _Page* _ep) {
    OperatorPostfix* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* postfixOperator = lexer.parsePostfixOperator(_rp); 
        if (postfixOperator) {
            lexer.advance();
            if (!ret)
                ret = new (_rp) OperatorPostfix(start, lexer.getPosition()); 

            ret->postfixOperator = postfixOperator;
	    }
        else {
            return _Result<OperatorPostfix, ParserError>(new(_ep) ParserError(*new(_ep) OperatorExpected(start)));
        }
    }

    return _Result<OperatorPostfix, ParserError>(ret);
}

_Result<FunctionCall, ParserError> Parser::parseFunctionCall(_Page* _rp, _Page* _ep) {
    FunctionCall* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<ParenthesizedExpression, ParserError> result = parseParenthesizedExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionCall(start, lexer.getPosition());

            ret->parenthesizedExpression = result.getResult();
        }
        else {
            return _Result<FunctionCall, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<CatchClause>, ParserError> result = parseCatchClauseList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionCall(start, lexer.getPosition());

            ret->catchClauses = result.getResult();
        }
    }

    return _Result<FunctionCall, ParserError>(ret);
}

_Result<ExplicitMemberExpression, ParserError> Parser::parseExplicitMemberExpression(_Page* _rp, _Page* _ep) {
    ExplicitMemberExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(dot)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ExplicitMemberExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ExplicitMemberExpression, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(dot))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<MemberPostfix, ParserError> result = parseMemberPostfix(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ExplicitMemberExpression(start, lexer.getPosition());

            ret->memberPostfix = result.getResult();
        }
        else {
            return _Result<ExplicitMemberExpression, ParserError>(result.getError());
        }
    }

    return _Result<ExplicitMemberExpression, ParserError>(ret);
}

_Result<Subscript, ParserError> Parser::parseSubscript(_Page* _rp, _Page* _ep) {
    Subscript* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftBracket)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Subscript(start, lexer.getPosition());
        }
        else {
            return _Result<Subscript, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftBracket))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<ExpressionElement>, ParserError> result = parseExpressionElementList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) Subscript(start, lexer.getPosition());

            ret->expressions = result.getResult();
        }
        else {
            return _Result<Subscript, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightBracket)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Subscript(start, lexer.getPosition());
        }
        else {
            return _Result<Subscript, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightBracket))));
        }
    }

    return _Result<Subscript, ParserError>(ret);
}

_Result<Array<ExpressionElement>, ParserError> Parser::parseExpressionElementList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<ExpressionElement>* ret = 0;

    while (true) {
        _Result<ExpressionElement, ParserError> nodeResult = parseExpressionElement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<ExpressionElement>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<ExpressionElement>, ParserError>(ret);
}

_Result<ExpressionElement, ParserError> Parser::parseExpressionElement(_Page* _rp, _Page* _ep) {
    ExpressionElement* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ExpressionElement(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<ExpressionElement, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(comma)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ExpressionElement(start, lexer.getPosition());
        }
    }

    return _Result<ExpressionElement, ParserError>(ret);
}

_Result<MemberPostfix, ParserError> Parser::parseMemberPostfix(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NamedMemberPostfix, ParserError> result = parseNamedMemberPostfix(_rp, _p);
        if (result.succeeded()) 
            return _Result<MemberPostfix, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<MemberPostfix, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<NamedMemberPostfix, ParserError> Parser::parseNamedMemberPostfix(_Page* _rp, _Page* _ep) {
    NamedMemberPostfix* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<IdentifierExpression, ParserError> result = parseIdentifierExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) NamedMemberPostfix(start, lexer.getPosition());

            ret->identifier = result.getResult();
        }
        else {
            return _Result<NamedMemberPostfix, ParserError>(result.getError());
        }
    }

    return _Result<NamedMemberPostfix, ParserError>(ret);
}

_Result<PrimaryExpression, ParserError> Parser::parsePrimaryExpression(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<LiteralExpression, ParserError> result = parseLiteralExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierExpression, ParserError> result = parseIdentifierExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IfExpression, ParserError> result = parseIfExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SwitchExpression, ParserError> result = parseSwitchExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ParenthesizedExpression, ParserError> result = parseParenthesizedExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ReturnExpression, ParserError> result = parseReturnExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThrowExpression, ParserError> result = parseThrowExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<PrimaryExpression, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ParenthesizedExpression, ParserError> Parser::parseParenthesizedExpression(_Page* _rp, _Page* _ep) {
    ParenthesizedExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftParen)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParenthesizedExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<ExpressionElement>, ParserError> result = parseExpressionElementList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ParenthesizedExpression(start, lexer.getPosition());

            ret->expressionElements = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightParen)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParenthesizedExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightParen))));
        }
    }

    return _Result<ParenthesizedExpression, ParserError>(ret);
}

_Result<LiteralExpression, ParserError> Parser::parseLiteralExpression(_Page* _rp, _Page* _ep) {
    LiteralExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        Literal* literal = lexer.parseLiteral(_rp);
        if (literal) {
        	lexer.advance();
            if (!ret)
                ret = new(_rp) LiteralExpression(start, lexer.getPosition());

            ret->literal = literal;
        }
        else {
            return _Result<LiteralExpression, ParserError>(new(_ep) ParserError(*new(_ep) LiteralExpected(start)));
        }
    }

    return _Result<LiteralExpression, ParserError>(ret);
}

_Result<IdentifierExpression, ParserError> Parser::parseIdentifierExpression(_Page* _rp, _Page* _ep) {
    IdentifierExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* idname = lexer.parseIdentifier(_rp);
        if ((idname) && (isIdentifier(*idname))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IdentifierExpression(start, lexer.getPosition());

            ret->name = idname;
        }
        else {
            return _Result<IdentifierExpression, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<IdentifierExpression, ParserError>(ret);
}

_Result<IfExpression, ParserError> Parser::parseIfExpression(_Page* _rp, _Page* _ep) {
    IfExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(ifKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IfExpression(start, lexer.getPosition());
        }
        else {
            return _Result<IfExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(ifKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) IfExpression(start, lexer.getPosition());

            ret->condition = result.getResult();
        }
        else {
            return _Result<IfExpression, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) IfExpression(start, lexer.getPosition());

            ret->consequent = result.getResult();
        }
        else {
            return _Result<IfExpression, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<ElseClause, ParserError> result = parseElseClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) IfExpression(start, lexer.getPosition());

            ret->elseClause = result.getResult();
        }
    }

    return _Result<IfExpression, ParserError>(ret);
}

_Result<ElseClause, ParserError> Parser::parseElseClause(_Page* _rp, _Page* _ep) {
    ElseClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(elseKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ElseClause(start, lexer.getPosition());
        }
        else {
            return _Result<ElseClause, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(elseKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ElseClause(start, lexer.getPosition());

            ret->alternative = result.getResult();
        }
        else {
            return _Result<ElseClause, ParserError>(result.getError());
        }
    }

    return _Result<ElseClause, ParserError>(ret);
}

_Result<SwitchExpression, ParserError> Parser::parseSwitchExpression(_Page* _rp, _Page* _ep) {
    SwitchExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(switchKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SwitchExpression(start, lexer.getPosition());
        }
        else {
            return _Result<SwitchExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(switchKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SwitchExpression(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<SwitchExpression, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<SwitchBody, ParserError> result = parseSwitchBody(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SwitchExpression(start, lexer.getPosition());

            ret->body = result.getResult();
        }
        else {
            return _Result<SwitchExpression, ParserError>(result.getError());
        }
    }

    return _Result<SwitchExpression, ParserError>(ret);
}

_Result<SwitchBody, ParserError> Parser::parseSwitchBody(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<CurliedSwitchBody, ParserError> result = parseCurliedSwitchBody(_rp, _p);
        if (result.succeeded()) 
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NakedSwitchBody, ParserError> result = parseNakedSwitchBody(_rp, _p);
        if (result.succeeded()) 
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<SwitchBody, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<CurliedSwitchBody, ParserError> Parser::parseCurliedSwitchBody(_Page* _rp, _Page* _ep) {
    CurliedSwitchBody* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftCurly)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CurliedSwitchBody(start, lexer.getPosition());
        }
        else {
            return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<SwitchCase>, ParserError> result = parseSwitchCaseList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) CurliedSwitchBody(start, lexer.getPosition());

            ret->cases = result.getResult();
        }
        else {
            return _Result<CurliedSwitchBody, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightCurly)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CurliedSwitchBody(start, lexer.getPosition());
        }
        else {
            return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightCurly))));
        }
    }

    return _Result<CurliedSwitchBody, ParserError>(ret);
}

_Result<NakedSwitchBody, ParserError> Parser::parseNakedSwitchBody(_Page* _rp, _Page* _ep) {
    NakedSwitchBody* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<SwitchCase>, ParserError> result = parseSwitchCaseList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) NakedSwitchBody(start, lexer.getPosition());

            ret->cases = result.getResult();
        }
        else {
            return _Result<NakedSwitchBody, ParserError>(result.getError());
        }
    }

    return _Result<NakedSwitchBody, ParserError>(ret);
}

_Result<Array<SwitchCase>, ParserError> Parser::parseSwitchCaseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<SwitchCase>* ret = 0;

    while (true) {
        _Result<SwitchCase, ParserError> nodeResult = parseSwitchCase(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<SwitchCase>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<SwitchCase>, ParserError>(ret);
}

_Result<SwitchCase, ParserError> Parser::parseSwitchCase(_Page* _rp, _Page* _ep) {
    SwitchCase* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<CaseLabel, ParserError> result = parseCaseLabel(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SwitchCase(start, lexer.getPosition());

            ret->label = result.getResult();
        }
        else {
            return _Result<SwitchCase, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<CaseContent, ParserError> result = parseCaseContent(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SwitchCase(start, lexer.getPosition());

            ret->content = result.getResult();
        }
        else {
            return _Result<SwitchCase, ParserError>(result.getError());
        }
    }

    return _Result<SwitchCase, ParserError>(ret);
}

_Result<CaseLabel, ParserError> Parser::parseCaseLabel(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ItemCaseLabel, ParserError> result = parseItemCaseLabel(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<DefaultCaseLabel, ParserError> result = parseDefaultCaseLabel(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<CaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ItemCaseLabel, ParserError> Parser::parseItemCaseLabel(_Page* _rp, _Page* _ep) {
    ItemCaseLabel* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(caseKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(caseKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<CaseItem>, ParserError> result = parseCaseItemList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());

            ret->items = result.getResult();
        }
        else {
            return _Result<ItemCaseLabel, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(colon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(colon))));
        }
    }

    return _Result<ItemCaseLabel, ParserError>(ret);
}

_Result<Array<CaseItem>, ParserError> Parser::parseCaseItemList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<CaseItem>* ret = 0;

    while (true) {
        _Result<CaseItem, ParserError> nodeResult = parseCaseItem(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<CaseItem>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<CaseItem>, ParserError>(ret);
}

_Result<CaseItem, ParserError> Parser::parseCaseItem(_Page* _rp, _Page* _ep) {
    CaseItem* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Pattern, ParserError> result = parsePattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) CaseItem(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<CaseItem, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(comma)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CaseItem(start, lexer.getPosition());
        }
        else {
            return _Result<CaseItem, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(comma))));
        }
    }

    return _Result<CaseItem, ParserError>(ret);
}

_Result<ReturnExpression, ParserError> Parser::parseReturnExpression(_Page* _rp, _Page* _ep) {
    ReturnExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(returnKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ReturnExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ReturnExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(returnKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ReturnExpression(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
    }

    return _Result<ReturnExpression, ParserError>(ret);
}

_Result<ThrowExpression, ParserError> Parser::parseThrowExpression(_Page* _rp, _Page* _ep) {
    ThrowExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(throwKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThrowExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ThrowExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(throwKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ThrowExpression(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
    }

    return _Result<ThrowExpression, ParserError>(ret);
}

_Result<Pattern, ParserError> Parser::parsePattern(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WildcardPattern, ParserError> result = parseWildcardPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierPattern, ParserError> result = parseIdentifierPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TuplePattern, ParserError> result = parseTuplePattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ExpressionPattern, ParserError> result = parseExpressionPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Pattern, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<WildcardPattern, ParserError> Parser::parseWildcardPattern(_Page* _rp, _Page* _ep) {
    WildcardPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(underscore)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) WildcardPattern(start, lexer.getPosition());
        }
        else {
            return _Result<WildcardPattern, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(underscore))));
        }
    }

    return _Result<WildcardPattern, ParserError>(ret);
}

_Result<IdentifierPattern, ParserError> Parser::parseIdentifierPattern(_Page* _rp, _Page* _ep) {
    IdentifierPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* ididentifier = lexer.parseIdentifier(_rp);
        if ((ididentifier) && (isIdentifier(*ididentifier))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IdentifierPattern(start, lexer.getPosition());

            ret->identifier = ididentifier;
        }
        else {
            return _Result<IdentifierPattern, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<TypeAnnotation, ParserError> result = parseTypeAnnotation(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) IdentifierPattern(start, lexer.getPosition());

            ret->typeAnnotation = result.getResult();
        }
    }

    return _Result<IdentifierPattern, ParserError>(ret);
}

_Result<TuplePattern, ParserError> Parser::parseTuplePattern(_Page* _rp, _Page* _ep) {
    TuplePattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftParen)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TuplePattern(start, lexer.getPosition());
        }
        else {
            return _Result<TuplePattern, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<TuplePatternElement>, ParserError> result = parseTuplePatternElementList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TuplePattern(start, lexer.getPosition());

            ret->elements = result.getResult();
        }
        else {
            return _Result<TuplePattern, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightParen)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TuplePattern(start, lexer.getPosition());
        }
        else {
            return _Result<TuplePattern, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightParen))));
        }
    }

    return _Result<TuplePattern, ParserError>(ret);
}

_Result<Array<TuplePatternElement>, ParserError> Parser::parseTuplePatternElementList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<TuplePatternElement>* ret = 0;

    while (true) {
        _Result<TuplePatternElement, ParserError> nodeResult = parseTuplePatternElement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<TuplePatternElement>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<TuplePatternElement>, ParserError>(ret);
}

_Result<TuplePatternElement, ParserError> Parser::parseTuplePatternElement(_Page* _rp, _Page* _ep) {
    TuplePatternElement* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Pattern, ParserError> result = parsePattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TuplePatternElement(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<TuplePatternElement, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(comma)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TuplePatternElement(start, lexer.getPosition());
        }
    }

    return _Result<TuplePatternElement, ParserError>(ret);
}

_Result<ExpressionPattern, ParserError> Parser::parseExpressionPattern(_Page* _rp, _Page* _ep) {
    ExpressionPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ExpressionPattern(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<ExpressionPattern, ParserError>(result.getError());
        }
    }

    return _Result<ExpressionPattern, ParserError>(ret);
}

_Result<DefaultCaseLabel, ParserError> Parser::parseDefaultCaseLabel(_Page* _rp, _Page* _ep) {
    DefaultCaseLabel* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parseKeyword(defaultKeyword)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) DefaultCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, *new(_ep) String(defaultKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(colon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) DefaultCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(colon))));
        }
    }

    return _Result<DefaultCaseLabel, ParserError>(ret);
}

_Result<CaseContent, ParserError> Parser::parseCaseContent(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BlockCaseContent, ParserError> result = parseBlockCaseContent(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<EmptyCaseContent, ParserError> result = parseEmptyCaseContent(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<CaseContent, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<BlockCaseContent, ParserError> Parser::parseBlockCaseContent(_Page* _rp, _Page* _ep) {
    BlockCaseContent* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<StatementWithSemicolon>, ParserError> result = parseStatementWithSemicolonList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) BlockCaseContent(start, lexer.getPosition());

            ret->statements = result.getResult();
        }
        else {
            return _Result<BlockCaseContent, ParserError>(result.getError());
        }
    }

    return _Result<BlockCaseContent, ParserError>(ret);
}

_Result<EmptyCaseContent, ParserError> Parser::parseEmptyCaseContent(_Page* _rp, _Page* _ep) {
    EmptyCaseContent* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(semicolon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EmptyCaseContent(start, lexer.getPosition());
        }
        else {
            return _Result<EmptyCaseContent, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(semicolon))));
        }
    }

    return _Result<EmptyCaseContent, ParserError>(ret);
}

_Result<Type, ParserError> Parser::parseType(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ArrayType, ParserError> result = parseArrayType(_rp, _p);
        if (result.succeeded()) 
            return _Result<Type, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _p);
        if (result.succeeded()) 
            return _Result<Type, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<Type, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<TypeAnnotation, ParserError> Parser::parseTypeAnnotation(_Page* _rp, _Page* _ep) {
    TypeAnnotation* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(colon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeAnnotation(start, lexer.getPosition());
        }
        else {
            return _Result<TypeAnnotation, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(colon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeAnnotation(start, lexer.getPosition());

            ret->typeAnnotation = result.getResult();
        }
        else {
            return _Result<TypeAnnotation, ParserError>(result.getError());
        }
    }

    return _Result<TypeAnnotation, ParserError>(ret);
}

_Result<TypeIdentifier, ParserError> Parser::parseTypeIdentifier(_Page* _rp, _Page* _ep) {
    TypeIdentifier* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        String* idname = lexer.parseIdentifier(_rp);
        if ((idname) && (isIdentifier(*idname))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeIdentifier(start, lexer.getPosition());

            ret->name = idname;
        }
        else {
            return _Result<TypeIdentifier, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<SubtypeIdentifier, ParserError> result = parseSubtypeIdentifier(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeIdentifier(start, lexer.getPosition());

            ret->subType = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<TypePostfix>, ParserError> result = parseTypePostfixList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeIdentifier(start, lexer.getPosition());

            ret->postfixes = result.getResult();
        }
    }

    return _Result<TypeIdentifier, ParserError>(ret);
}

_Result<SubtypeIdentifier, ParserError> Parser::parseSubtypeIdentifier(_Page* _rp, _Page* _ep) {
    SubtypeIdentifier* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(dot)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SubtypeIdentifier(start, lexer.getPosition());
        }
        else {
            return _Result<SubtypeIdentifier, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(dot))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SubtypeIdentifier(start, lexer.getPosition());

            ret->typeIdentifier = result.getResult();
        }
        else {
            return _Result<SubtypeIdentifier, ParserError>(result.getError());
        }
    }

    return _Result<SubtypeIdentifier, ParserError>(ret);
}

_Result<Array<TypePostfix>, ParserError> Parser::parseTypePostfixList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<TypePostfix>* ret = 0;

    while (true) {
        _Result<TypePostfix, ParserError> nodeResult = parseTypePostfix(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<TypePostfix>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<TypePostfix>, ParserError>(ret);
}

_Result<TypePostfix, ParserError> Parser::parseTypePostfix(_Page* _rp, _Page* _ep) {
    Array<ParserError>& errors = *new(_ep) Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OptionalType, ParserError> result = parseOptionalType(_rp, _p);
        if (result.succeeded()) 
            return _Result<TypePostfix, ParserError>(result.getResult());
        else
            errors.append(result.getError());
    }

    return _Result<TypePostfix, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ArrayType, ParserError> Parser::parseArrayType(_Page* _rp, _Page* _ep) {
    ArrayType* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(leftBracket)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ArrayType(start, lexer.getPosition());
        }
        else {
            return _Result<ArrayType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(leftBracket))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ArrayType(start, lexer.getPosition());

            ret->elementType = result.getResult();
        }
        else {
            return _Result<ArrayType, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(rightBracket)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ArrayType(start, lexer.getPosition());
        }
        else {
            return _Result<ArrayType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(rightBracket))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<TypePostfix>, ParserError> result = parseTypePostfixList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ArrayType(start, lexer.getPosition());

            ret->postfixes = result.getResult();
        }
    }

    return _Result<ArrayType, ParserError>(ret);
}

_Result<OptionalType, ParserError> Parser::parseOptionalType(_Page* _rp, _Page* _ep) {
    OptionalType* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(questionMark)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) OptionalType(start, lexer.getPosition());
        }
        else {
            return _Result<OptionalType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(questionMark))));
        }
    }

    return _Result<OptionalType, ParserError>(ret);
}

_Result<TypeInheritanceClause, ParserError> Parser::parseTypeInheritanceClause(_Page* _rp, _Page* _ep) {
    TypeInheritanceClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(colon)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeInheritanceClause(start, lexer.getPosition());
        }
        else {
            return _Result<TypeInheritanceClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, *new(_ep) String(colon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();

        _Result<Array<Inheritance>, ParserError> result = parseInheritanceList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeInheritanceClause(start, lexer.getPosition());

            ret->inheritances = result.getResult();
        }
    }

    return _Result<TypeInheritanceClause, ParserError>(ret);
}

_Result<Array<Inheritance>, ParserError> Parser::parseInheritanceList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<Inheritance>* ret = 0;

    while (true) {
        _Result<Inheritance, ParserError> nodeResult = parseInheritance(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<Inheritance>();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<Array<Inheritance>, ParserError>(ret);
}

_Result<Inheritance, ParserError> Parser::parseInheritance(_Page* _rp, _Page* _ep) {
    Inheritance* ret = 0;
    {
        Position start = lexer.getPreviousPosition();

        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) Inheritance(start, lexer.getPosition());

            ret->name = result.getResult();
        }
        else {
            return _Result<Inheritance, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        if (lexer.parsePunctuation(comma)) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Inheritance(start, lexer.getPosition());
        }
    }

    return _Result<Inheritance, ParserError>(ret);
}

bool Parser::isAtEnd() {
    return lexer.isAtEnd();
}
    
bool Parser::isIdentifier(String& id) {
    if (id == useKeyword)
        return false;

    if (id == classKeyword)
        return false;

    if (id == functionKeyword)
        return false;

    if (id == ifKeyword)
        return false;

    if (id == elseKeyword)
        return false;

    if (id == switchKeyword)
        return false;

    if (id == caseKeyword)
        return false;

    if (id == defaultKeyword)
        return false;

    if (id == catchKeyword)
        return false;

    if (id == returnKeyword)
        return false;

    if (id == throwKeyword)
        return false;

    if (id == throwsKeyword)
        return false;

    if (id == staticKeyword)
        return false;

    if (id == letKeyword)
        return false;

    if (id == mutableKeyword)
        return false;

    if (id == isKeyword)
        return false;

    if (id == asKeyword)
        return false;

    return true;
}

}