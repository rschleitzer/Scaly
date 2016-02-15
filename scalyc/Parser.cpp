#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Parser::Parser(_LetString* fileName, _LetString& text)
: lexer(text), fileName(fileName) {
}

_Result<CompilationUnit, ParserError> Parser::parseCompilationUnit(_Page* _rp, _Page* _ep) {
    CompilationUnit* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<StatementWithSemicolon>, ParserError> result = parseStatementWithSemicolonList(_rp, _ep);
        if (result.succeeded()) {
            if (!isAtEnd()) {
                Position current = lexer.getPosition();
                return _Result<CompilationUnit, ParserError>(new(_ep) ParserError(*new(_ep) NotAtEnd(current)));
            }
            if (!ret)
                ret = new(_rp) CompilationUnit(start, lexer.getPosition());

            ret->statements = result.getResult();
        }
    }
    ret->fileName = fileName;

    return _Result<CompilationUnit, ParserError>(ret);
}

_Result<_Array<StatementWithSemicolon>, ParserError> Parser::parseStatementWithSemicolonList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<StatementWithSemicolon>* ret = 0;

    while (true) {
        _Result<StatementWithSemicolon, ParserError> nodeResult = parseStatementWithSemicolon(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<StatementWithSemicolon>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<StatementWithSemicolon>, ParserError>(ret);
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
        bool success = lexer.parsePunctuation(semicolon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) StatementWithSemicolon(start, lexer.getPosition());
        }
    }

    return _Result<StatementWithSemicolon, ParserError>(ret);
}

_Result<Statement, ParserError> Parser::parseStatement(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Declaration, ParserError> result = parseDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Expression, ParserError> result = parseExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<Statement, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Statement, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<Declaration, ParserError> Parser::parseDeclaration(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<UseDeclaration, ParserError> result = parseUseDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ConstantDeclaration, ParserError> result = parseConstantDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<VariableDeclaration, ParserError> result = parseVariableDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<FunctionDeclaration, ParserError> result = parseFunctionDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<EnumDeclaration, ParserError> result = parseEnumDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ClassDeclaration, ParserError> result = parseClassDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<InitializerDeclaration, ParserError> result = parseInitializerDeclaration(_rp, _p);
        if (result.succeeded()) 
            return _Result<Declaration, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Declaration, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<UseDeclaration, ParserError> Parser::parseUseDeclaration(_Page* _rp, _Page* _ep) {
    UseDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(useKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) UseDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<UseDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, useKeyword))));
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
        _Result<_Array<PathIdentifier>, ParserError> result = parsePathIdentifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) UseDeclaration(start, lexer.getPosition());

            ret->importExtensions = result.getResult();
        }
    }

    return _Result<UseDeclaration, ParserError>(ret);
}

_Result<_Array<PathIdentifier>, ParserError> Parser::parsePathIdentifierList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<PathIdentifier>* ret = 0;

    while (true) {
        _Result<PathIdentifier, ParserError> nodeResult = parsePathIdentifier(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<PathIdentifier>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<PathIdentifier>, ParserError>(ret);
}

_Result<PathIdentifier, ParserError> Parser::parsePathIdentifier(_Page* _rp, _Page* _ep) {
    PathIdentifier* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(dot);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PathIdentifier(start, lexer.getPosition());
        }
        else {
            return _Result<PathIdentifier, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, dot))));
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
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PathItem(start, lexer.getPosition());

            ret->name = name;
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
        bool success = lexer.parsePunctuation(equal);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Initializer(start, lexer.getPosition());
        }
        else {
            return _Result<Initializer, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, equal))));
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
        bool success = lexer.parseKeyword(letKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ConstantDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<ConstantDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, letKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<BindingInitializer, ParserError> result = parseBindingInitializer(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ConstantDeclaration(start, lexer.getPosition());

            ret->initializer = result.getResult();
        }
        else {
            return _Result<ConstantDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<ConstantDeclaration, ParserError>(ret);
}

_Result<VariableDeclaration, ParserError> Parser::parseVariableDeclaration(_Page* _rp, _Page* _ep) {
    VariableDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(mutableKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) VariableDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<VariableDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, mutableKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<BindingInitializer, ParserError> result = parseBindingInitializer(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) VariableDeclaration(start, lexer.getPosition());

            ret->initializer = result.getResult();
        }
        else {
            return _Result<VariableDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<VariableDeclaration, ParserError>(ret);
}

_Result<BindingInitializer, ParserError> Parser::parseBindingInitializer(_Page* _rp, _Page* _ep) {
    BindingInitializer* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<PatternInitializer, ParserError> result = parsePatternInitializer(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) BindingInitializer(start, lexer.getPosition());

            ret->initializer = result.getResult();
        }
        else {
            return _Result<BindingInitializer, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<AdditionalInitializer>, ParserError> result = parseAdditionalInitializerList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) BindingInitializer(start, lexer.getPosition());

            ret->additionalInitializers = result.getResult();
        }
    }

    return _Result<BindingInitializer, ParserError>(ret);
}

_Result<_Array<PatternInitializer>, ParserError> Parser::parsePatternInitializerList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<PatternInitializer>* ret = 0;

    while (true) {
        _Result<PatternInitializer, ParserError> nodeResult = parsePatternInitializer(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<PatternInitializer>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<PatternInitializer>, ParserError>(ret);
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

    return _Result<PatternInitializer, ParserError>(ret);
}

_Result<_Array<AdditionalInitializer>, ParserError> Parser::parseAdditionalInitializerList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<AdditionalInitializer>* ret = 0;

    while (true) {
        _Result<AdditionalInitializer, ParserError> nodeResult = parseAdditionalInitializer(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<AdditionalInitializer>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<AdditionalInitializer>, ParserError>(ret);
}

_Result<AdditionalInitializer, ParserError> Parser::parseAdditionalInitializer(_Page* _rp, _Page* _ep) {
    AdditionalInitializer* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) AdditionalInitializer(start, lexer.getPosition());
        }
        else {
            return _Result<AdditionalInitializer, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, comma))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<PatternInitializer, ParserError> result = parsePatternInitializer(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) AdditionalInitializer(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<AdditionalInitializer, ParserError>(result.getError());
        }
    }

    return _Result<AdditionalInitializer, ParserError>(ret);
}

_Result<FunctionDeclaration, ParserError> Parser::parseFunctionDeclaration(_Page* _rp, _Page* _ep) {
    FunctionDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<Modifier>, ParserError> result = parseModifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());

            ret->modifiers = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(functionKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) FunctionDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<FunctionDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, functionKeyword))));
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

_Result<InitializerDeclaration, ParserError> Parser::parseInitializerDeclaration(_Page* _rp, _Page* _ep) {
    InitializerDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<Modifier>, ParserError> result = parseModifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerDeclaration(start, lexer.getPosition());

            ret->modifiers = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(initKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) InitializerDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<InitializerDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, initKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<ParameterClause, ParserError> result = parseParameterClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerDeclaration(start, lexer.getPosition());

            ret->parameterClause = result.getResult();
        }
        else {
            return _Result<InitializerDeclaration, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<ThrowsClause, ParserError> result = parseThrowsClause(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerDeclaration(start, lexer.getPosition());

            ret->throwsClause = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerDeclaration(start, lexer.getPosition());

            ret->body = result.getResult();
        }
        else {
            return _Result<InitializerDeclaration, ParserError>(result.getError());
        }
    }

    return _Result<InitializerDeclaration, ParserError>(ret);
}

_Result<_Array<Modifier>, ParserError> Parser::parseModifierList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<Modifier>* ret = 0;

    while (true) {
        _Result<Modifier, ParserError> nodeResult = parseModifier(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<Modifier>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<Modifier>, ParserError>(ret);
}

_Result<Modifier, ParserError> Parser::parseModifier(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Override, ParserError> result = parseOverride(_rp, _p);
        if (result.succeeded()) 
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<StaticWord, ParserError> result = parseStaticWord(_rp, _p);
        if (result.succeeded()) 
            return _Result<Modifier, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Modifier, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<Override, ParserError> Parser::parseOverride(_Page* _rp, _Page* _ep) {
    Override* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(overrideKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Override(start, lexer.getPosition());
        }
        else {
            return _Result<Override, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, overrideKeyword))));
        }
    }

    return _Result<Override, ParserError>(ret);
}

_Result<StaticWord, ParserError> Parser::parseStaticWord(_Page* _rp, _Page* _ep) {
    StaticWord* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(staticKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) StaticWord(start, lexer.getPosition());
        }
        else {
            return _Result<StaticWord, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, staticKeyword))));
        }
    }

    return _Result<StaticWord, ParserError>(ret);
}

_Result<FunctionName, ParserError> Parser::parseFunctionName(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierFunction, ParserError> result = parseIdentifierFunction(_rp, _p);
        if (result.succeeded()) 
            return _Result<FunctionName, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<FunctionName, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<IdentifierFunction, ParserError> Parser::parseIdentifierFunction(_Page* _rp, _Page* _ep) {
    IdentifierFunction* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IdentifierFunction(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<IdentifierFunction, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<IdentifierFunction, ParserError>(ret);
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
        bool success = lexer.parsePunctuation(evaluatesTo);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) FunctionResult(start, lexer.getPosition());
        }
        else {
            return _Result<FunctionResult, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, evaluatesTo))));
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

_Result<_Array<ParameterClause>, ParserError> Parser::parseParameterClauseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<ParameterClause>* ret = 0;

    while (true) {
        _Result<ParameterClause, ParserError> nodeResult = parseParameterClause(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<ParameterClause>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<ParameterClause>, ParserError>(ret);
}

_Result<ParameterClause, ParserError> Parser::parseParameterClause(_Page* _rp, _Page* _ep) {
    ParameterClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParameterClause(start, lexer.getPosition());
        }
        else {
            return _Result<ParameterClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<Parameter>, ParserError> result = parseParameterList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ParameterClause(start, lexer.getPosition());

            ret->parameters = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(rightParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParameterClause(start, lexer.getPosition());
        }
        else {
            return _Result<ParameterClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightParen))));
        }
    }

    return _Result<ParameterClause, ParserError>(ret);
}

_Result<_Array<Parameter>, ParserError> Parser::parseParameterList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<Parameter>* ret = 0;

    while (true) {
        _Result<Parameter, ParserError> nodeResult = parseParameter(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<Parameter>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<Parameter>, ParserError>(ret);
}

_Result<Parameter, ParserError> Parser::parseParameter(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ConstParameter, ParserError> result = parseConstParameter(_rp, _p);
        if (result.succeeded()) 
            return _Result<Parameter, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<VarParameter, ParserError> result = parseVarParameter(_rp, _p);
        if (result.succeeded()) 
            return _Result<Parameter, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Parameter, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ConstParameter, ParserError> Parser::parseConstParameter(_Page* _rp, _Page* _ep) {
    ConstParameter* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(letKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ConstParameter(start, lexer.getPosition());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ConstParameter(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<ConstParameter, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(colon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ConstParameter(start, lexer.getPosition());
        }
        else {
            return _Result<ConstParameter, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, colon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ConstParameter(start, lexer.getPosition());

            ret->type = result.getResult();
        }
        else {
            return _Result<ConstParameter, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ConstParameter(start, lexer.getPosition());
        }
    }

    return _Result<ConstParameter, ParserError>(ret);
}

_Result<VarParameter, ParserError> Parser::parseVarParameter(_Page* _rp, _Page* _ep) {
    VarParameter* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(mutableKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) VarParameter(start, lexer.getPosition());
        }
        else {
            return _Result<VarParameter, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, mutableKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) VarParameter(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<VarParameter, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(colon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) VarParameter(start, lexer.getPosition());
        }
        else {
            return _Result<VarParameter, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, colon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) VarParameter(start, lexer.getPosition());

            ret->parameterType = result.getResult();
        }
        else {
            return _Result<VarParameter, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) VarParameter(start, lexer.getPosition());
        }
    }

    return _Result<VarParameter, ParserError>(ret);
}

_Result<ThrowsClause, ParserError> Parser::parseThrowsClause(_Page* _rp, _Page* _ep) {
    ThrowsClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(throwsKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThrowsClause(start, lexer.getPosition());
        }
        else {
            return _Result<ThrowsClause, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, throwsKeyword))));
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

_Result<EnumDeclaration, ParserError> Parser::parseEnumDeclaration(_Page* _rp, _Page* _ep) {
    EnumDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(enumKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EnumDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, enumKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EnumDeclaration(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EnumDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<EnumMember>, ParserError> result = parseEnumMemberList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) EnumDeclaration(start, lexer.getPosition());

            ret->members = result.getResult();
        }
        else {
            return _Result<EnumDeclaration, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(rightCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EnumDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<EnumDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightCurly))));
        }
    }

    return _Result<EnumDeclaration, ParserError>(ret);
}

_Result<_Array<EnumMember>, ParserError> Parser::parseEnumMemberList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<EnumMember>* ret = 0;

    while (true) {
        _Result<EnumMember, ParserError> nodeResult = parseEnumMember(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<EnumMember>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<EnumMember>, ParserError>(ret);
}

_Result<EnumMember, ParserError> Parser::parseEnumMember(_Page* _rp, _Page* _ep) {
    EnumMember* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(caseKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EnumMember(start, lexer.getPosition());
        }
        else {
            return _Result<EnumMember, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, caseKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<EnumCase, ParserError> result = parseEnumCase(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) EnumMember(start, lexer.getPosition());

            ret->enumCase = result.getResult();
        }
        else {
            return _Result<EnumMember, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<AdditionalCase>, ParserError> result = parseAdditionalCaseList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) EnumMember(start, lexer.getPosition());

            ret->additionalCases = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<TupleType, ParserError> result = parseTupleType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) EnumMember(start, lexer.getPosition());

            ret->tupleType = result.getResult();
        }
    }

    return _Result<EnumMember, ParserError>(ret);
}

_Result<TupleType, ParserError> Parser::parseTupleType(_Page* _rp, _Page* _ep) {
    TupleType* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TupleType(start, lexer.getPosition());
        }
        else {
            return _Result<TupleType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TupleType(start, lexer.getPosition());

            ret->tupleType = result.getResult();
        }
        else {
            return _Result<TupleType, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<AdditionalType>, ParserError> result = parseAdditionalTypeList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TupleType(start, lexer.getPosition());

            ret->additionalTypes = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(rightParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TupleType(start, lexer.getPosition());
        }
        else {
            return _Result<TupleType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightParen))));
        }
    }

    return _Result<TupleType, ParserError>(ret);
}

_Result<_Array<AdditionalType>, ParserError> Parser::parseAdditionalTypeList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<AdditionalType>* ret = 0;

    while (true) {
        _Result<AdditionalType, ParserError> nodeResult = parseAdditionalType(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<AdditionalType>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<AdditionalType>, ParserError>(ret);
}

_Result<AdditionalType, ParserError> Parser::parseAdditionalType(_Page* _rp, _Page* _ep) {
    AdditionalType* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) AdditionalType(start, lexer.getPosition());
        }
        else {
            return _Result<AdditionalType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, comma))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) AdditionalType(start, lexer.getPosition());

            ret->enumCase = result.getResult();
        }
        else {
            return _Result<AdditionalType, ParserError>(result.getError());
        }
    }

    return _Result<AdditionalType, ParserError>(ret);
}

_Result<EnumCase, ParserError> Parser::parseEnumCase(_Page* _rp, _Page* _ep) {
    EnumCase* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EnumCase(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<EnumCase, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<EnumCase, ParserError>(ret);
}

_Result<_Array<AdditionalCase>, ParserError> Parser::parseAdditionalCaseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<AdditionalCase>* ret = 0;

    while (true) {
        _Result<AdditionalCase, ParserError> nodeResult = parseAdditionalCase(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<AdditionalCase>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<AdditionalCase>, ParserError>(ret);
}

_Result<AdditionalCase, ParserError> Parser::parseAdditionalCase(_Page* _rp, _Page* _ep) {
    AdditionalCase* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) AdditionalCase(start, lexer.getPosition());
        }
        else {
            return _Result<AdditionalCase, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, comma))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<EnumCase, ParserError> result = parseEnumCase(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) AdditionalCase(start, lexer.getPosition());

            ret->enumCase = result.getResult();
        }
        else {
            return _Result<AdditionalCase, ParserError>(result.getError());
        }
    }

    return _Result<AdditionalCase, ParserError>(ret);
}

_Result<ClassDeclaration, ParserError> Parser::parseClassDeclaration(_Page* _rp, _Page* _ep) {
    ClassDeclaration* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(classKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, classKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());

            ret->name = name;
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
        bool success = lexer.parsePunctuation(leftCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<ClassMember>, ParserError> result = parseClassMemberList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());

            ret->members = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(rightCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassDeclaration(start, lexer.getPosition());
        }
        else {
            return _Result<ClassDeclaration, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightCurly))));
        }
    }

    return _Result<ClassDeclaration, ParserError>(ret);
}

_Result<GenericArgumentClause, ParserError> Parser::parseGenericArgumentClause(_Page* _rp, _Page* _ep) {
    GenericArgumentClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftAngular);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) GenericArgumentClause(start, lexer.getPosition());
        }
        else {
            return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftAngular))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<GenericParameter>, ParserError> result = parseGenericParameterList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(rightAngular);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) GenericArgumentClause(start, lexer.getPosition());
        }
        else {
            return _Result<GenericArgumentClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightAngular))));
        }
    }

    return _Result<GenericArgumentClause, ParserError>(ret);
}

_Result<_Array<GenericParameter>, ParserError> Parser::parseGenericParameterList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<GenericParameter>* ret = 0;

    while (true) {
        _Result<GenericParameter, ParserError> nodeResult = parseGenericParameter(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<GenericParameter>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<GenericParameter>, ParserError>(ret);
}

_Result<GenericParameter, ParserError> Parser::parseGenericParameter(_Page* _rp, _Page* _ep) {
    GenericParameter* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* typeName = lexer.parseIdentifier(_rp);
        if ((typeName) && (isIdentifier(*typeName))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) GenericParameter(start, lexer.getPosition());

            ret->typeName = typeName;
        }
        else {
            return _Result<GenericParameter, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<GenericParameter, ParserError>(ret);
}

_Result<_Array<ClassMember>, ParserError> Parser::parseClassMemberList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<ClassMember>* ret = 0;

    while (true) {
        _Result<ClassMember, ParserError> nodeResult = parseClassMember(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<ClassMember>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<ClassMember>, ParserError>(ret);
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
        bool success = lexer.parsePunctuation(semicolon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ClassMember(start, lexer.getPosition());
        }
    }

    return _Result<ClassMember, ParserError>(ret);
}

_Result<Expression, ParserError> Parser::parseExpression(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SimpleExpression, ParserError> result = parseSimpleExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<CodeBlock, ParserError> result = parseCodeBlock(_rp, _p);
        if (result.succeeded()) 
            return _Result<Expression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Expression, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<CodeBlock, ParserError> Parser::parseCodeBlock(_Page* _rp, _Page* _ep) {
    CodeBlock* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CodeBlock(start, lexer.getPosition());
        }
        else {
            return _Result<CodeBlock, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<StatementWithSemicolon>, ParserError> result = parseStatementWithSemicolonList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(rightCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CodeBlock(start, lexer.getPosition());
        }
        else {
            return _Result<CodeBlock, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightCurly))));
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
        _Result<_Array<BinaryOp>, ParserError> result = parseBinaryOpList(_rp, _ep);
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
        _LetString* prefixOperator = lexer.parsePrefixOperator(_rp);
        if (prefixOperator) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PrefixExpression(start, lexer.getPosition());

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
        _Result<_Array<Postfix>, ParserError> result = parsePostfixList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PostfixExpression(start, lexer.getPosition());

            ret->postfixes = result.getResult();
        }
    }

    return _Result<PostfixExpression, ParserError>(ret);
}

_Result<_Array<BinaryOp>, ParserError> Parser::parseBinaryOpList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<BinaryOp>* ret = 0;

    while (true) {
        _Result<BinaryOp, ParserError> nodeResult = parseBinaryOp(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<BinaryOp>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<BinaryOp>, ParserError>(ret);
}

_Result<BinaryOp, ParserError> Parser::parseBinaryOp(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BinaryOperation, ParserError> result = parseBinaryOperation(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Assignment, ParserError> result = parseAssignment(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeQuery, ParserError> result = parseTypeQuery(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeCast, ParserError> result = parseTypeCast(_rp, _p);
        if (result.succeeded()) 
            return _Result<BinaryOp, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<BinaryOp, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<BinaryOperation, ParserError> Parser::parseBinaryOperation(_Page* _rp, _Page* _ep) {
    BinaryOperation* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* binaryOperator = lexer.parseBinaryOperator(_rp);
        if (binaryOperator) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) BinaryOperation(start, lexer.getPosition());

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
        bool success = lexer.parsePunctuation(equal);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Assignment(start, lexer.getPosition());
        }
        else {
            return _Result<Assignment, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, equal))));
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
        bool success = lexer.parseKeyword(isKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeQuery(start, lexer.getPosition());
        }
        else {
            return _Result<TypeQuery, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, isKeyword))));
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
        bool success = lexer.parseKeyword(asKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeCast(start, lexer.getPosition());
        }
        else {
            return _Result<TypeCast, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, asKeyword))));
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

_Result<_Array<CatchClause>, ParserError> Parser::parseCatchClauseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<CatchClause>* ret = 0;

    while (true) {
        _Result<CatchClause, ParserError> nodeResult = parseCatchClause(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<CatchClause>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<CatchClause>, ParserError>(ret);
}

_Result<CatchClause, ParserError> Parser::parseCatchClause(_Page* _rp, _Page* _ep) {
    CatchClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(catchKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CatchClause(start, lexer.getPosition());
        }
        else {
            return _Result<CatchClause, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, catchKeyword))));
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
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WildCardCatchPattern, ParserError> result = parseWildCardCatchPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<PathItemCatchPattern, ParserError> result = parsePathItemCatchPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<CatchPattern, ParserError>(result.getResult());
        else
            errors.push(result.getError());
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
        _Result<_Array<PathIdentifier>, ParserError> result = parsePathIdentifierList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PathItemCatchPattern(start, lexer.getPosition());

            ret->catchCaseExtensions = result.getResult();
        }
    }

    return _Result<PathItemCatchPattern, ParserError>(ret);
}

_Result<_Array<Postfix>, ParserError> Parser::parsePostfixList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<Postfix>* ret = 0;

    while (true) {
        _Result<Postfix, ParserError> nodeResult = parsePostfix(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<Postfix>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<Postfix>, ParserError>(ret);
}

_Result<Postfix, ParserError> Parser::parsePostfix(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OperatorPostfix, ParserError> result = parseOperatorPostfix(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<FunctionCall, ParserError> result = parseFunctionCall(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ExplicitMemberExpression, ParserError> result = parseExplicitMemberExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<Subscript, ParserError> result = parseSubscript(_rp, _p);
        if (result.succeeded()) 
            return _Result<Postfix, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Postfix, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<OperatorPostfix, ParserError> Parser::parseOperatorPostfix(_Page* _rp, _Page* _ep) {
    OperatorPostfix* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* postfixOperator = lexer.parsePostfixOperator(_rp);
        if (postfixOperator) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) OperatorPostfix(start, lexer.getPosition());

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

            ret->arguments = result.getResult();
        }
        else {
            return _Result<FunctionCall, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<CatchClause>, ParserError> result = parseCatchClauseList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(dot);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ExplicitMemberExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ExplicitMemberExpression, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, dot))));
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
        bool success = lexer.parsePunctuation(leftBracket);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Subscript(start, lexer.getPosition());
        }
        else {
            return _Result<Subscript, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftBracket))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<ExpressionElement>, ParserError> result = parseExpressionElementList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(rightBracket);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) Subscript(start, lexer.getPosition());
        }
        else {
            return _Result<Subscript, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightBracket))));
        }
    }

    return _Result<Subscript, ParserError>(ret);
}

_Result<_Array<ExpressionElement>, ParserError> Parser::parseExpressionElementList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<ExpressionElement>* ret = 0;

    while (true) {
        _Result<ExpressionElement, ParserError> nodeResult = parseExpressionElement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<ExpressionElement>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<ExpressionElement>, ParserError>(ret);
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
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ExpressionElement(start, lexer.getPosition());
        }
    }

    return _Result<ExpressionElement, ParserError>(ret);
}

_Result<MemberPostfix, ParserError> Parser::parseMemberPostfix(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NamedMemberPostfix, ParserError> result = parseNamedMemberPostfix(_rp, _p);
        if (result.succeeded()) 
            return _Result<MemberPostfix, ParserError>(result.getResult());
        else
            errors.push(result.getError());
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
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierExpression, ParserError> result = parseIdentifierExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<LiteralExpression, ParserError> result = parseLiteralExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IfExpression, ParserError> result = parseIfExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SwitchExpression, ParserError> result = parseSwitchExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ForExpression, ParserError> result = parseForExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ParenthesizedExpression, ParserError> result = parseParenthesizedExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ReturnExpression, ParserError> result = parseReturnExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThrowExpression, ParserError> result = parseThrowExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BreakExpression, ParserError> result = parseBreakExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<InitializerCall, ParserError> result = parseInitializerCall(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisExpression, ParserError> result = parseThisExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperExpression, ParserError> result = parseSuperExpression(_rp, _p);
        if (result.succeeded()) 
            return _Result<PrimaryExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<PrimaryExpression, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ParenthesizedExpression, ParserError> Parser::parseParenthesizedExpression(_Page* _rp, _Page* _ep) {
    ParenthesizedExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParenthesizedExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<ExpressionElement>, ParserError> result = parseExpressionElementList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ParenthesizedExpression(start, lexer.getPosition());

            ret->expressionElements = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(rightParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ParenthesizedExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ParenthesizedExpression, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightParen))));
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
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IdentifierExpression(start, lexer.getPosition());

            ret->name = name;
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
        bool success = lexer.parseKeyword(ifKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IfExpression(start, lexer.getPosition());
        }
        else {
            return _Result<IfExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, ifKeyword))));
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
        bool success = lexer.parseKeyword(elseKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ElseClause(start, lexer.getPosition());
        }
        else {
            return _Result<ElseClause, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, elseKeyword))));
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
        bool success = lexer.parseKeyword(switchKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SwitchExpression(start, lexer.getPosition());
        }
        else {
            return _Result<SwitchExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, switchKeyword))));
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
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<CurliedSwitchBody, ParserError> result = parseCurliedSwitchBody(_rp, _p);
        if (result.succeeded()) 
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<NakedSwitchBody, ParserError> result = parseNakedSwitchBody(_rp, _p);
        if (result.succeeded()) 
            return _Result<SwitchBody, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<SwitchBody, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<CurliedSwitchBody, ParserError> Parser::parseCurliedSwitchBody(_Page* _rp, _Page* _ep) {
    CurliedSwitchBody* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CurliedSwitchBody(start, lexer.getPosition());
        }
        else {
            return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftCurly))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<SwitchCase>, ParserError> result = parseSwitchCaseList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(rightCurly);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CurliedSwitchBody(start, lexer.getPosition());
        }
        else {
            return _Result<CurliedSwitchBody, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightCurly))));
        }
    }

    return _Result<CurliedSwitchBody, ParserError>(ret);
}

_Result<NakedSwitchBody, ParserError> Parser::parseNakedSwitchBody(_Page* _rp, _Page* _ep) {
    NakedSwitchBody* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<SwitchCase>, ParserError> result = parseSwitchCaseList(_rp, _ep);
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

_Result<_Array<SwitchCase>, ParserError> Parser::parseSwitchCaseList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<SwitchCase>* ret = 0;

    while (true) {
        _Result<SwitchCase, ParserError> nodeResult = parseSwitchCase(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<SwitchCase>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<SwitchCase>, ParserError>(ret);
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
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ItemCaseLabel, ParserError> result = parseItemCaseLabel(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<DefaultCaseLabel, ParserError> result = parseDefaultCaseLabel(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseLabel, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<CaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ItemCaseLabel, ParserError> Parser::parseItemCaseLabel(_Page* _rp, _Page* _ep) {
    ItemCaseLabel* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(caseKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, caseKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Pattern, ParserError> result = parsePattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<ItemCaseLabel, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<CaseItem>, ParserError> result = parseCaseItemList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());

            ret->additionalPatterns = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(colon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ItemCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<ItemCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, colon))));
        }
    }

    return _Result<ItemCaseLabel, ParserError>(ret);
}

_Result<_Array<CaseItem>, ParserError> Parser::parseCaseItemList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<CaseItem>* ret = 0;

    while (true) {
        _Result<CaseItem, ParserError> nodeResult = parseCaseItem(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<CaseItem>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<CaseItem>, ParserError>(ret);
}

_Result<CaseItem, ParserError> Parser::parseCaseItem(_Page* _rp, _Page* _ep) {
    CaseItem* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) CaseItem(start, lexer.getPosition());
        }
        else {
            return _Result<CaseItem, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, comma))));
        }
    }
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

    return _Result<CaseItem, ParserError>(ret);
}

_Result<ForExpression, ParserError> Parser::parseForExpression(_Page* _rp, _Page* _ep) {
    ForExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(forKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ForExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ForExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, forKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<ForLoop, ParserError> result = parseForLoop(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ForExpression(start, lexer.getPosition());

            ret->loop = result.getResult();
        }
        else {
            return _Result<ForExpression, ParserError>(result.getError());
        }
    }

    return _Result<ForExpression, ParserError>(ret);
}

_Result<ForLoop, ParserError> Parser::parseForLoop(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ForEach, ParserError> result = parseForEach(_rp, _p);
        if (result.succeeded()) 
            return _Result<ForLoop, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<PlainFor, ParserError> result = parsePlainFor(_rp, _p);
        if (result.succeeded()) 
            return _Result<ForLoop, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<ForLoop, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ForEach, ParserError> Parser::parseForEach(_Page* _rp, _Page* _ep) {
    ForEach* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<Pattern, ParserError> result = parsePattern(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ForEach(start, lexer.getPosition());

            ret->pattern = result.getResult();
        }
        else {
            return _Result<ForEach, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(inKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ForEach(start, lexer.getPosition());
        }
        else {
            return _Result<ForEach, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, inKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ForEach(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
        else {
            return _Result<ForEach, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ForEach(start, lexer.getPosition());

            ret->code = result.getResult();
        }
        else {
            return _Result<ForEach, ParserError>(result.getError());
        }
    }

    return _Result<ForEach, ParserError>(ret);
}

_Result<PlainFor, ParserError> Parser::parsePlainFor(_Page* _rp, _Page* _ep) {
    PlainFor* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<VariableDeclaration, ParserError> result = parseVariableDeclaration(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PlainFor(start, lexer.getPosition());

            ret->forInit = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(semicolon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PlainFor(start, lexer.getPosition());
        }
        else {
            return _Result<PlainFor, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, semicolon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PlainFor(start, lexer.getPosition());

            ret->forCheck = result.getResult();
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(semicolon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) PlainFor(start, lexer.getPosition());
        }
        else {
            return _Result<PlainFor, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, semicolon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PlainFor(start, lexer.getPosition());

            ret->forNext = result.getResult();
        }
        else {
            return _Result<PlainFor, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) PlainFor(start, lexer.getPosition());

            ret->code = result.getResult();
        }
        else {
            return _Result<PlainFor, ParserError>(result.getError());
        }
    }

    return _Result<PlainFor, ParserError>(ret);
}

_Result<ReturnExpression, ParserError> Parser::parseReturnExpression(_Page* _rp, _Page* _ep) {
    ReturnExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(returnKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ReturnExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ReturnExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, returnKeyword))));
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
        bool success = lexer.parseKeyword(throwKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThrowExpression(start, lexer.getPosition());
        }
        else {
            return _Result<ThrowExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, throwKeyword))));
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

_Result<BreakExpression, ParserError> Parser::parseBreakExpression(_Page* _rp, _Page* _ep) {
    BreakExpression* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(breakKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) BreakExpression(start, lexer.getPosition());
        }
        else {
            return _Result<BreakExpression, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, breakKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Expression, ParserError> result = parseExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) BreakExpression(start, lexer.getPosition());

            ret->expression = result.getResult();
        }
    }

    return _Result<BreakExpression, ParserError>(ret);
}

_Result<Pattern, ParserError> Parser::parsePattern(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<WildcardPattern, ParserError> result = parseWildcardPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<IdentifierPattern, ParserError> result = parseIdentifierPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TuplePattern, ParserError> result = parseTuplePattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ExpressionPattern, ParserError> result = parseExpressionPattern(_rp, _p);
        if (result.succeeded()) 
            return _Result<Pattern, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Pattern, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<WildcardPattern, ParserError> Parser::parseWildcardPattern(_Page* _rp, _Page* _ep) {
    WildcardPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(underscore);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) WildcardPattern(start, lexer.getPosition());
        }
        else {
            return _Result<WildcardPattern, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, underscore))));
        }
    }

    return _Result<WildcardPattern, ParserError>(ret);
}

_Result<IdentifierPattern, ParserError> Parser::parseIdentifierPattern(_Page* _rp, _Page* _ep) {
    IdentifierPattern* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* identifier = lexer.parseIdentifier(_rp);
        if ((identifier) && (isIdentifier(*identifier))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) IdentifierPattern(start, lexer.getPosition());

            ret->identifier = identifier;
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
        bool success = lexer.parsePunctuation(leftParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TuplePattern(start, lexer.getPosition());
        }
        else {
            return _Result<TuplePattern, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftParen))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<TuplePatternElement>, ParserError> result = parseTuplePatternElementList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(rightParen);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TuplePattern(start, lexer.getPosition());
        }
        else {
            return _Result<TuplePattern, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightParen))));
        }
    }

    return _Result<TuplePattern, ParserError>(ret);
}

_Result<_Array<TuplePatternElement>, ParserError> Parser::parseTuplePatternElementList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<TuplePatternElement>* ret = 0;

    while (true) {
        _Result<TuplePatternElement, ParserError> nodeResult = parseTuplePatternElement(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<TuplePatternElement>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<TuplePatternElement>, ParserError>(ret);
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
        bool success = lexer.parsePunctuation(comma);
        if (success) {
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
        bool success = lexer.parseKeyword(defaultKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) DefaultCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, defaultKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(colon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) DefaultCaseLabel(start, lexer.getPosition());
        }
        else {
            return _Result<DefaultCaseLabel, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, colon))));
        }
    }

    return _Result<DefaultCaseLabel, ParserError>(ret);
}

_Result<CaseContent, ParserError> Parser::parseCaseContent(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<BlockCaseContent, ParserError> result = parseBlockCaseContent(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<EmptyCaseContent, ParserError> result = parseEmptyCaseContent(_rp, _p);
        if (result.succeeded()) 
            return _Result<CaseContent, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<CaseContent, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<BlockCaseContent, ParserError> Parser::parseBlockCaseContent(_Page* _rp, _Page* _ep) {
    BlockCaseContent* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<StatementWithSemicolon>, ParserError> result = parseStatementWithSemicolonList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(semicolon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) EmptyCaseContent(start, lexer.getPosition());
        }
        else {
            return _Result<EmptyCaseContent, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, semicolon))));
        }
    }

    return _Result<EmptyCaseContent, ParserError>(ret);
}

_Result<InitializerCall, ParserError> Parser::parseInitializerCall(_Page* _rp, _Page* _ep) {
    InitializerCall* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<Type, ParserError> result = parseType(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerCall(start, lexer.getPosition());

            ret->typeToInitialize = result.getResult();
        }
        else {
            return _Result<InitializerCall, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<ParenthesizedExpression, ParserError> result = parseParenthesizedExpression(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerCall(start, lexer.getPosition());

            ret->arguments = result.getResult();
        }
        else {
            return _Result<InitializerCall, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<CatchClause>, ParserError> result = parseCatchClauseList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) InitializerCall(start, lexer.getPosition());

            ret->catchClauses = result.getResult();
        }
    }

    return _Result<InitializerCall, ParserError>(ret);
}

_Result<ThisExpression, ParserError> Parser::parseThisExpression(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisDot, ParserError> result = parseThisDot(_rp, _p);
        if (result.succeeded()) 
            return _Result<ThisExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisSubscript, ParserError> result = parseThisSubscript(_rp, _p);
        if (result.succeeded()) 
            return _Result<ThisExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisWord, ParserError> result = parseThisWord(_rp, _p);
        if (result.succeeded()) 
            return _Result<ThisExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<ThisExpression, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ThisDot, ParserError> Parser::parseThisDot(_Page* _rp, _Page* _ep) {
    ThisDot* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(thisKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThisDot(start, lexer.getPosition());
        }
        else {
            return _Result<ThisDot, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, thisKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(dot);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThisDot(start, lexer.getPosition());
        }
        else {
            return _Result<ThisDot, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, dot))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<CommonThisMember, ParserError> result = parseCommonThisMember(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ThisDot(start, lexer.getPosition());

            ret->member = result.getResult();
        }
        else {
            return _Result<ThisDot, ParserError>(result.getError());
        }
    }

    return _Result<ThisDot, ParserError>(ret);
}

_Result<ThisSubscript, ParserError> Parser::parseThisSubscript(_Page* _rp, _Page* _ep) {
    ThisSubscript* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(thisKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThisSubscript(start, lexer.getPosition());
        }
        else {
            return _Result<ThisSubscript, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, thisKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Subscript, ParserError> result = parseSubscript(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) ThisSubscript(start, lexer.getPosition());

            ret->subscript = result.getResult();
        }
        else {
            return _Result<ThisSubscript, ParserError>(result.getError());
        }
    }

    return _Result<ThisSubscript, ParserError>(ret);
}

_Result<ThisWord, ParserError> Parser::parseThisWord(_Page* _rp, _Page* _ep) {
    ThisWord* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(thisKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThisWord(start, lexer.getPosition());
        }
        else {
            return _Result<ThisWord, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, thisKeyword))));
        }
    }

    return _Result<ThisWord, ParserError>(ret);
}

_Result<CommonThisMember, ParserError> Parser::parseCommonThisMember(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisInit, ParserError> result = parseThisInit(_rp, _p);
        if (result.succeeded()) 
            return _Result<CommonThisMember, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ThisMember, ParserError> result = parseThisMember(_rp, _p);
        if (result.succeeded()) 
            return _Result<CommonThisMember, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<CommonThisMember, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ThisInit, ParserError> Parser::parseThisInit(_Page* _rp, _Page* _ep) {
    ThisInit* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(initKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThisInit(start, lexer.getPosition());
        }
        else {
            return _Result<ThisInit, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, initKeyword))));
        }
    }

    return _Result<ThisInit, ParserError>(ret);
}

_Result<ThisMember, ParserError> Parser::parseThisMember(_Page* _rp, _Page* _ep) {
    ThisMember* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ThisMember(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<ThisMember, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<ThisMember, ParserError>(ret);
}

_Result<SuperExpression, ParserError> Parser::parseSuperExpression(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperDot, ParserError> result = parseSuperDot(_rp, _p);
        if (result.succeeded()) 
            return _Result<SuperExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperSubscript, ParserError> result = parseSuperSubscript(_rp, _p);
        if (result.succeeded()) 
            return _Result<SuperExpression, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<SuperExpression, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<SuperDot, ParserError> Parser::parseSuperDot(_Page* _rp, _Page* _ep) {
    SuperDot* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(superKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SuperDot(start, lexer.getPosition());
        }
        else {
            return _Result<SuperDot, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, superKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(dot);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SuperDot(start, lexer.getPosition());
        }
        else {
            return _Result<SuperDot, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, dot))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<CommonSuperMember, ParserError> result = parseCommonSuperMember(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SuperDot(start, lexer.getPosition());

            ret->member = result.getResult();
        }
        else {
            return _Result<SuperDot, ParserError>(result.getError());
        }
    }

    return _Result<SuperDot, ParserError>(ret);
}

_Result<SuperSubscript, ParserError> Parser::parseSuperSubscript(_Page* _rp, _Page* _ep) {
    SuperSubscript* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(superKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SuperSubscript(start, lexer.getPosition());
        }
        else {
            return _Result<SuperSubscript, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, superKeyword))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<Subscript, ParserError> result = parseSubscript(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) SuperSubscript(start, lexer.getPosition());

            ret->subscript = result.getResult();
        }
        else {
            return _Result<SuperSubscript, ParserError>(result.getError());
        }
    }

    return _Result<SuperSubscript, ParserError>(ret);
}

_Result<CommonSuperMember, ParserError> Parser::parseCommonSuperMember(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperInit, ParserError> result = parseSuperInit(_rp, _p);
        if (result.succeeded()) 
            return _Result<CommonSuperMember, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<SuperMember, ParserError> result = parseSuperMember(_rp, _p);
        if (result.succeeded()) 
            return _Result<CommonSuperMember, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<CommonSuperMember, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<SuperInit, ParserError> Parser::parseSuperInit(_Page* _rp, _Page* _ep) {
    SuperInit* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parseKeyword(initKeyword);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SuperInit(start, lexer.getPosition());
        }
        else {
            return _Result<SuperInit, ParserError>(new(_ep) ParserError(*new(_ep) KeywordExpected(start, _LetString::create(_ep, initKeyword))));
        }
    }

    return _Result<SuperInit, ParserError>(ret);
}

_Result<SuperMember, ParserError> Parser::parseSuperMember(_Page* _rp, _Page* _ep) {
    SuperMember* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SuperMember(start, lexer.getPosition());

            ret->name = name;
        }
        else {
            return _Result<SuperMember, ParserError>(new(_ep) ParserError(*new(_ep) IdentifierExpected(start)));
        }
    }

    return _Result<SuperMember, ParserError>(ret);
}

_Result<Type, ParserError> Parser::parseType(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<ArrayType, ParserError> result = parseArrayType(_rp, _p);
        if (result.succeeded()) 
            return _Result<Type, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _p);
        if (result.succeeded()) 
            return _Result<Type, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<Type, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<TypeAnnotation, ParserError> Parser::parseTypeAnnotation(_Page* _rp, _Page* _ep) {
    TypeAnnotation* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(colon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeAnnotation(start, lexer.getPosition());
        }
        else {
            return _Result<TypeAnnotation, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, colon))));
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
        _LetString* name = lexer.parseIdentifier(_rp);
        if ((name) && (isIdentifier(*name))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeIdentifier(start, lexer.getPosition());

            ret->name = name;
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
        _Result<_Array<TypePostfix>, ParserError> result = parseTypePostfixList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(dot);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) SubtypeIdentifier(start, lexer.getPosition());
        }
        else {
            return _Result<SubtypeIdentifier, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, dot))));
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

_Result<_Array<TypePostfix>, ParserError> Parser::parseTypePostfixList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<TypePostfix>* ret = 0;

    while (true) {
        _Result<TypePostfix, ParserError> nodeResult = parseTypePostfix(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<TypePostfix>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<TypePostfix>, ParserError>(ret);
}

_Result<TypePostfix, ParserError> Parser::parseTypePostfix(_Page* _rp, _Page* _ep) {
    _Array<ParserError>& errors = *new(_ep) _Array<ParserError>();
    Position start = lexer.getPreviousPosition();

    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<OptionalType, ParserError> result = parseOptionalType(_rp, _p);
        if (result.succeeded()) 
            return _Result<TypePostfix, ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }

    return _Result<TypePostfix, ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
}

_Result<ArrayType, ParserError> Parser::parseArrayType(_Page* _rp, _Page* _ep) {
    ArrayType* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(leftBracket);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ArrayType(start, lexer.getPosition());
        }
        else {
            return _Result<ArrayType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, leftBracket))));
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
        bool success = lexer.parsePunctuation(rightBracket);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) ArrayType(start, lexer.getPosition());
        }
        else {
            return _Result<ArrayType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, rightBracket))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<TypePostfix>, ParserError> result = parseTypePostfixList(_rp, _ep);
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
        bool success = lexer.parsePunctuation(questionMark);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) OptionalType(start, lexer.getPosition());
        }
        else {
            return _Result<OptionalType, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, questionMark))));
        }
    }

    return _Result<OptionalType, ParserError>(ret);
}

_Result<TypeInheritanceClause, ParserError> Parser::parseTypeInheritanceClause(_Page* _rp, _Page* _ep) {
    TypeInheritanceClause* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(colon);
        if (success) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) TypeInheritanceClause(start, lexer.getPosition());
        }
        else {
            return _Result<TypeInheritanceClause, ParserError>(new(_ep) ParserError(*new(_ep) PunctuationExpected(start, _LetString::create(_ep, colon))));
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        _Result<_Array<Inheritance>, ParserError> result = parseInheritanceList(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) TypeInheritanceClause(start, lexer.getPosition());

            ret->inheritances = result.getResult();
        }
    }

    return _Result<TypeInheritanceClause, ParserError>(ret);
}

_Result<_Array<Inheritance>, ParserError> Parser::parseInheritanceList(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<Inheritance>* ret = 0;

    while (true) {
        _Result<Inheritance, ParserError> nodeResult = parseInheritance(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<Inheritance>();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<_Array<Inheritance>, ParserError>(ret);
}

_Result<Inheritance, ParserError> Parser::parseInheritance(_Page* _rp, _Page* _ep) {
    Inheritance* ret = 0;
    {
        Position start = lexer.getPreviousPosition();
        _Result<TypeIdentifier, ParserError> result = parseTypeIdentifier(_rp, _ep);
        if (result.succeeded()) {
            if (!ret)
                ret = new(_rp) Inheritance(start, lexer.getPosition());

            ret->typeIdentifier = result.getResult();
        }
        else {
            return _Result<Inheritance, ParserError>(result.getError());
        }
    }
    {
        Position start = lexer.getPreviousPosition();
        bool success = lexer.parsePunctuation(comma);
        if (success) {
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
    
bool Parser::isIdentifier(_LetString& id) {
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

    if (id == forKeyword)
        return false;

    if (id == inKeyword)
        return false;

    if (id == returnKeyword)
        return false;

    if (id == throwKeyword)
        return false;

    if (id == breakKeyword)
        return false;

    if (id == throwsKeyword)
        return false;

    if (id == overrideKeyword)
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

    if (id == initKeyword)
        return false;

    if (id == enumKeyword)
        return false;

    if (id == superKeyword)
        return false;

    if (id == thisKeyword)
        return false;

    return true;
}

}
