#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Parser::Parser(string* theFileName, string* text) {
    lexer = new(_getPage()->allocateExclusivePage()) Lexer(text);
    fileName = theFileName;
    classKeyword = new(_getPage()) string("class");
    functionKeyword = new(_getPage()) string("function");
    ifKeyword = new(_getPage()) string("if");
    elseKeyword = new(_getPage()) string("else");
    switchKeyword = new(_getPage()) string("switch");
    caseKeyword = new(_getPage()) string("case");
    defaultKeyword = new(_getPage()) string("default");
    catchKeyword = new(_getPage()) string("catch");
    forKeyword = new(_getPage()) string("for");
    inKeyword = new(_getPage()) string("in");
    whileKeyword = new(_getPage()) string("while");
    doKeyword = new(_getPage()) string("do");
    returnKeyword = new(_getPage()) string("return");
    throwKeyword = new(_getPage()) string("throw");
    breakKeyword = new(_getPage()) string("break");
    throwsKeyword = new(_getPage()) string("throws");
    overrideKeyword = new(_getPage()) string("override");
    staticKeyword = new(_getPage()) string("static");
    letKeyword = new(_getPage()) string("let");
    varKeyword = new(_getPage()) string("var");
    mutableKeyword = new(_getPage()) string("mutable");
    isKeyword = new(_getPage()) string("is");
    asKeyword = new(_getPage()) string("as");
    constructorKeyword = new(_getPage()) string("constructor");
    enumKeyword = new(_getPage()) string("enum");
    thisKeyword = new(_getPage()) string("this");
    nullKeyword = new(_getPage()) string("null");
    newKeyword = new(_getPage()) string("new");
    extendsKeyword = new(_getPage()) string("extends");
    equal = new(_getPage()) string("=");
    comma = new(_getPage()) string(",");
    leftParen = new(_getPage()) string("(");
    rightParen = new(_getPage()) string(")");
    leftCurly = new(_getPage()) string("{");
    rightCurly = new(_getPage()) string("}");
    leftBracket = new(_getPage()) string("[");
    rightBracket = new(_getPage()) string("]");
    colon = new(_getPage()) string(":");
    dot = new(_getPage()) string(".");
    underscore = new(_getPage()) string("_");
    circumflex = new(_getPage()) string("^");
    dollar = new(_getPage()) string("$");
    at = new(_getPage()) string("@");
    hash = new(_getPage()) string("#");
    ampersand = new(_getPage()) string("&");
}

CompilationUnit* Parser::parseCompilationUnit(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    _Array<Statement>* statements = parseStatementList(_rp);
    if (statements != nullptr) {
        if (!isAtEnd()) {
            return nullptr;
        }
    }
    Position* end = lexer->getPosition(_p);
    CompilationUnit* ret = new(_rp) CompilationUnit(statements, new(_rp) Position(start), new(_rp) Position(end));
    if (statements != nullptr) {
        Statement* item = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            item = *(*statements)[_i];
            item->parent = ret;
        }
    }
    return ret;
}

_Array<Statement>* Parser::parseStatementList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<Statement>* ret = nullptr;
    while (true) {
        Statement* node = parseStatement(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<Statement>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<Statement>(ret) : nullptr;
}

Statement* Parser::parseStatement(_Page* _rp) {
    {
        Declaration* node = parseDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Expression* node = parseExpression(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

Declaration* Parser::parseDeclaration(_Page* _rp) {
    {
        ConstantDeclaration* node = parseConstantDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        VariableDeclaration* node = parseVariableDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        MutableDeclaration* node = parseMutableDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        FunctionDeclaration* node = parseFunctionDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        EnumDeclaration* node = parseEnumDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ClassDeclaration* node = parseClassDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ConstructorDeclaration* node = parseConstructorDeclaration(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

Expression* Parser::parseExpression(_Page* _rp) {
    {
        CodeBlock* node = parseCodeBlock(_rp);
        if (node != nullptr)
            return node;
    }
    {
        SimpleExpression* node = parseSimpleExpression(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

ConstantDeclaration* Parser::parseConstantDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1)
        lexer->advance();
    else
        return nullptr;
    BindingInitializer* initializer = parseBindingInitializer(_rp);
    if (initializer == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ConstantDeclaration* ret = new(_rp) ConstantDeclaration(initializer, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    return ret;
}

VariableDeclaration* Parser::parseVariableDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successVar1 = lexer->parseKeyword(varKeyword);
    if (successVar1)
        lexer->advance();
    else
        return nullptr;
    BindingInitializer* initializer = parseBindingInitializer(_rp);
    if (initializer == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    VariableDeclaration* ret = new(_rp) VariableDeclaration(initializer, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    return ret;
}

MutableDeclaration* Parser::parseMutableDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1)
        lexer->advance();
    else
        return nullptr;
    BindingInitializer* initializer = parseBindingInitializer(_rp);
    if (initializer == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    MutableDeclaration* ret = new(_rp) MutableDeclaration(initializer, new(_rp) Position(start), new(_rp) Position(end));
    initializer->parent = ret;
    return ret;
}

FunctionDeclaration* Parser::parseFunctionDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    _Array<Modifier>* modifiers = parseModifierList(_rp);
    bool successFunction2 = lexer->parseKeyword(functionKeyword);
    if (successFunction2)
        lexer->advance();
    else
        return nullptr;
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    FunctionSignature* signature = parseFunctionSignature(_rp);
    if (signature == nullptr)
        return nullptr;
    Expression* body = parseExpression(_rp);
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
    signature->parent = ret;
    if (body != nullptr)
        body->parent = ret;
    return ret;
}

EnumDeclaration* Parser::parseEnumDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successEnum1 = lexer->parseKeyword(enumKeyword);
    if (successEnum1)
        lexer->advance();
    else
        return nullptr;
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    bool successLeftCurly3 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly3)
        lexer->advance();
    else
        return nullptr;
    _Array<EnumMember>* members = parseEnumMemberList(_rp);
    bool successRightCurly5 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly5)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

ClassDeclaration* Parser::parseClassDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successClass1 = lexer->parseKeyword(classKeyword);
    if (successClass1)
        lexer->advance();
    else
        return nullptr;
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    TypeInheritanceClause* typeInheritanceClause = parseTypeInheritanceClause(_rp);
    ClassBody* body = parseClassBody(_rp);
    Position* end = lexer->getPosition(_p);
    ClassDeclaration* ret = new(_rp) ClassDeclaration(name, typeInheritanceClause, body, new(_rp) Position(start), new(_rp) Position(end));
    if (typeInheritanceClause != nullptr)
        typeInheritanceClause->parent = ret;
    if (body != nullptr)
        body->parent = ret;
    return ret;
}

ConstructorDeclaration* Parser::parseConstructorDeclaration(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successConstructor1 = lexer->parseKeyword(constructorKeyword);
    if (successConstructor1)
        lexer->advance();
    else
        return nullptr;
    ParameterClause* parameterClause = parseParameterClause(_rp);
    if (parameterClause == nullptr)
        return nullptr;
    Expression* body = parseExpression(_rp);
    if (body == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ConstructorDeclaration* ret = new(_rp) ConstructorDeclaration(parameterClause, body, new(_rp) Position(start), new(_rp) Position(end));
    parameterClause->parent = ret;
    body->parent = ret;
    return ret;
}

CodeBlock* Parser::parseCodeBlock(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1)
        lexer->advance();
    else
        return nullptr;
    _Array<Statement>* statements = parseStatementList(_rp);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

SimpleExpression* Parser::parseSimpleExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    PrefixExpression* prefixExpression = parsePrefixExpression(_rp);
    if (prefixExpression == nullptr)
        return nullptr;
    _Array<BinaryOp>* binaryOps = parseBinaryOpList(_rp);
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
    return ret;
}

Initializer* Parser::parseInitializer(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1)
        lexer->advance();
    else
        return nullptr;
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Initializer* ret = new(_rp) Initializer(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return ret;
}

BindingInitializer* Parser::parseBindingInitializer(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    PatternInitializer* initializer = parsePatternInitializer(_rp);
    if (initializer == nullptr)
        return nullptr;
    _Array<AdditionalInitializer>* additionalInitializers = parseAdditionalInitializerList(_rp);
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
    return ret;
}

_Array<PatternInitializer>* Parser::parsePatternInitializerList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<PatternInitializer>* ret = nullptr;
    while (true) {
        PatternInitializer* node = parsePatternInitializer(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<PatternInitializer>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<PatternInitializer>(ret) : nullptr;
}

PatternInitializer* Parser::parsePatternInitializer(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Pattern* pattern = parsePattern(_rp);
    if (pattern == nullptr)
        return nullptr;
    Initializer* initializer = parseInitializer(_rp);
    Position* end = lexer->getPosition(_p);
    PatternInitializer* ret = new(_rp) PatternInitializer(pattern, initializer, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    if (initializer != nullptr)
        initializer->parent = ret;
    return ret;
}

_Array<AdditionalInitializer>* Parser::parseAdditionalInitializerList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<AdditionalInitializer>* ret = nullptr;
    while (true) {
        AdditionalInitializer* node = parseAdditionalInitializer(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<AdditionalInitializer>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<AdditionalInitializer>(ret) : nullptr;
}

AdditionalInitializer* Parser::parseAdditionalInitializer(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1)
        lexer->advance();
    else
        return nullptr;
    PatternInitializer* pattern = parsePatternInitializer(_rp);
    if (pattern == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    AdditionalInitializer* ret = new(_rp) AdditionalInitializer(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return ret;
}

_Array<Modifier>* Parser::parseModifierList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<Modifier>* ret = nullptr;
    while (true) {
        Modifier* node = parseModifier(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<Modifier>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<Modifier>(ret) : nullptr;
}

Modifier* Parser::parseModifier(_Page* _rp) {
    {
        OverrideWord* node = parseOverrideWord(_rp);
        if (node != nullptr)
            return node;
    }
    {
        StaticWord* node = parseStaticWord(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

OverrideWord* Parser::parseOverrideWord(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successOverride1 = lexer->parseKeyword(overrideKeyword);
    if (successOverride1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    OverrideWord* ret = new(_rp) OverrideWord(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

StaticWord* Parser::parseStaticWord(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successStatic1 = lexer->parseKeyword(staticKeyword);
    if (successStatic1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    StaticWord* ret = new(_rp) StaticWord(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

FunctionSignature* Parser::parseFunctionSignature(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    ParameterClause* parameterClause = parseParameterClause(_rp);
    if (parameterClause == nullptr)
        return nullptr;
    FunctionResult* result = parseFunctionResult(_rp);
    ThrowsClause* throwsClause = parseThrowsClause(_rp);
    Position* end = lexer->getPosition(_p);
    FunctionSignature* ret = new(_rp) FunctionSignature(parameterClause, result, throwsClause, new(_rp) Position(start), new(_rp) Position(end));
    parameterClause->parent = ret;
    if (result != nullptr)
        result->parent = ret;
    if (throwsClause != nullptr)
        throwsClause->parent = ret;
    return ret;
}

FunctionResult* Parser::parseFunctionResult(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1)
        lexer->advance();
    else
        return nullptr;
    Type* resultType = parseType(_rp);
    if (resultType == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    FunctionResult* ret = new(_rp) FunctionResult(resultType, new(_rp) Position(start), new(_rp) Position(end));
    resultType->parent = ret;
    return ret;
}

_Array<ParameterClause>* Parser::parseParameterClauseList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<ParameterClause>* ret = nullptr;
    while (true) {
        ParameterClause* node = parseParameterClause(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<ParameterClause>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<ParameterClause>(ret) : nullptr;
}

ParameterClause* Parser::parseParameterClause(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1)
        lexer->advance();
    else
        return nullptr;
    _Array<Parameter>* parameters = parseParameterList(_rp);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

_Array<Parameter>* Parser::parseParameterList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<Parameter>* ret = nullptr;
    while (true) {
        Parameter* node = parseParameter(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<Parameter>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<Parameter>(ret) : nullptr;
}

Parameter* Parser::parseParameter(_Page* _rp) {
    {
        ConstParameter* node = parseConstParameter(_rp);
        if (node != nullptr)
            return node;
    }
    {
        VarParameter* node = parseVarParameter(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

ConstParameter* Parser::parseConstParameter(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLet1 = lexer->parseKeyword(letKeyword);
    if (successLet1)
        lexer->advance();
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3)
        lexer->advance();
    else
        return nullptr;
    Type* parameterType = parseType(_rp);
    if (parameterType == nullptr)
        return nullptr;
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    ConstParameter* ret = new(_rp) ConstParameter(name, parameterType, new(_rp) Position(start), new(_rp) Position(end));
    parameterType->parent = ret;
    return ret;
}

VarParameter* Parser::parseVarParameter(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successMutable1 = lexer->parseKeyword(mutableKeyword);
    if (successMutable1)
        lexer->advance();
    else
        return nullptr;
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    bool successColon3 = lexer->parsePunctuation(colon);
    if (successColon3)
        lexer->advance();
    else
        return nullptr;
    Type* parameterType = parseType(_rp);
    if (parameterType == nullptr)
        return nullptr;
    bool successComma5 = lexer->parsePunctuation(comma);
    if (successComma5)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    VarParameter* ret = new(_rp) VarParameter(name, parameterType, new(_rp) Position(start), new(_rp) Position(end));
    parameterType->parent = ret;
    return ret;
}

ThrowsClause* Parser::parseThrowsClause(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successThrows1 = lexer->parseKeyword(throwsKeyword);
    if (successThrows1)
        lexer->advance();
    else
        return nullptr;
    Type* throwsType = parseType(_rp);
    if (throwsType == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ThrowsClause* ret = new(_rp) ThrowsClause(throwsType, new(_rp) Position(start), new(_rp) Position(end));
    throwsType->parent = ret;
    return ret;
}

_Array<EnumMember>* Parser::parseEnumMemberList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<EnumMember>* ret = nullptr;
    while (true) {
        EnumMember* node = parseEnumMember(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<EnumMember>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<EnumMember>(ret) : nullptr;
}

EnumMember* Parser::parseEnumMember(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1)
        lexer->advance();
    else
        return nullptr;
    EnumCase* enumCase = parseEnumCase(_rp);
    if (enumCase == nullptr)
        return nullptr;
    _Array<AdditionalCase>* additionalCases = parseAdditionalCaseList(_rp);
    ParameterClause* parameterClause = parseParameterClause(_rp);
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
    return ret;
}

EnumCase* Parser::parseEnumCase(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    EnumCase* ret = new(_rp) EnumCase(name, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

_Array<AdditionalCase>* Parser::parseAdditionalCaseList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<AdditionalCase>* ret = nullptr;
    while (true) {
        AdditionalCase* node = parseAdditionalCase(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<AdditionalCase>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<AdditionalCase>(ret) : nullptr;
}

AdditionalCase* Parser::parseAdditionalCase(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1)
        lexer->advance();
    else
        return nullptr;
    EnumCase* enumCase = parseEnumCase(_rp);
    if (enumCase == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    AdditionalCase* ret = new(_rp) AdditionalCase(enumCase, new(_rp) Position(start), new(_rp) Position(end));
    enumCase->parent = ret;
    return ret;
}

ClassBody* Parser::parseClassBody(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1)
        lexer->advance();
    else
        return nullptr;
    _Array<ClassMember>* members = parseClassMemberList(_rp);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

_Array<ClassMember>* Parser::parseClassMemberList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<ClassMember>* ret = nullptr;
    while (true) {
        ClassMember* node = parseClassMember(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<ClassMember>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<ClassMember>(ret) : nullptr;
}

ClassMember* Parser::parseClassMember(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Declaration* declaration = parseDeclaration(_rp);
    if (declaration == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ClassMember* ret = new(_rp) ClassMember(declaration, new(_rp) Position(start), new(_rp) Position(end));
    declaration->parent = ret;
    return ret;
}

PrefixExpression* Parser::parsePrefixExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* prefixOperator = lexer->parsePrefixOperator(_rp);
    if (prefixOperator != nullptr)
        lexer->advance();
    PostfixExpression* expression = parsePostfixExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    PrefixExpression* ret = new(_rp) PrefixExpression(prefixOperator, expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return ret;
}

PostfixExpression* Parser::parsePostfixExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    PrimaryExpression* primaryExpression = parsePrimaryExpression(_rp);
    if (primaryExpression == nullptr)
        return nullptr;
    _Array<Postfix>* postfixes = parsePostfixList(_rp);
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
    return ret;
}

_Array<BinaryOp>* Parser::parseBinaryOpList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<BinaryOp>* ret = nullptr;
    while (true) {
        BinaryOp* node = parseBinaryOp(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<BinaryOp>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<BinaryOp>(ret) : nullptr;
}

BinaryOp* Parser::parseBinaryOp(_Page* _rp) {
    {
        BinaryOperation* node = parseBinaryOperation(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Assignment* node = parseAssignment(_rp);
        if (node != nullptr)
            return node;
    }
    {
        TypeQuery* node = parseTypeQuery(_rp);
        if (node != nullptr)
            return node;
    }
    {
        TypeCast* node = parseTypeCast(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

BinaryOperation* Parser::parseBinaryOperation(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* binaryOperator = lexer->parseBinaryOperator(_rp);
    if (binaryOperator != nullptr)
        lexer->advance();
    else
        return nullptr;
    PrefixExpression* expression = parsePrefixExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    BinaryOperation* ret = new(_rp) BinaryOperation(binaryOperator, expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return ret;
}

Assignment* Parser::parseAssignment(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successEqual1 = lexer->parsePunctuation(equal);
    if (successEqual1)
        lexer->advance();
    else
        return nullptr;
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Assignment* ret = new(_rp) Assignment(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return ret;
}

TypeQuery* Parser::parseTypeQuery(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successIs1 = lexer->parseKeyword(isKeyword);
    if (successIs1)
        lexer->advance();
    else
        return nullptr;
    Type* objectType = parseType(_rp);
    if (objectType == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    TypeQuery* ret = new(_rp) TypeQuery(objectType, new(_rp) Position(start), new(_rp) Position(end));
    objectType->parent = ret;
    return ret;
}

TypeCast* Parser::parseTypeCast(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successAs1 = lexer->parseKeyword(asKeyword);
    if (successAs1)
        lexer->advance();
    else
        return nullptr;
    Type* objectType = parseType(_rp);
    if (objectType == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    TypeCast* ret = new(_rp) TypeCast(objectType, new(_rp) Position(start), new(_rp) Position(end));
    objectType->parent = ret;
    return ret;
}

_Array<CatchClause>* Parser::parseCatchClauseList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<CatchClause>* ret = nullptr;
    while (true) {
        CatchClause* node = parseCatchClause(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<CatchClause>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<CatchClause>(ret) : nullptr;
}

CatchClause* Parser::parseCatchClause(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successCatch1 = lexer->parseKeyword(catchKeyword);
    if (successCatch1)
        lexer->advance();
    else
        return nullptr;
    CatchPattern* catchPattern = parseCatchPattern(_rp);
    if (catchPattern == nullptr)
        return nullptr;
    TuplePattern* bindingPattern = parseTuplePattern(_rp);
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    CatchClause* ret = new(_rp) CatchClause(catchPattern, bindingPattern, expression, new(_rp) Position(start), new(_rp) Position(end));
    catchPattern->parent = ret;
    if (bindingPattern != nullptr)
        bindingPattern->parent = ret;
    expression->parent = ret;
    return ret;
}

CatchPattern* Parser::parseCatchPattern(_Page* _rp) {
    {
        WildCardCatchPattern* node = parseWildCardCatchPattern(_rp);
        if (node != nullptr)
            return node;
    }
    {
        IdentifierCatchPattern* node = parseIdentifierCatchPattern(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

WildCardCatchPattern* Parser::parseWildCardCatchPattern(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    WildcardPattern* pattern = parseWildcardPattern(_rp);
    if (pattern == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    WildCardCatchPattern* ret = new(_rp) WildCardCatchPattern(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return ret;
}

IdentifierCatchPattern* Parser::parseIdentifierCatchPattern(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    MemberExpression* member = parseMemberExpression(_rp);
    Position* end = lexer->getPosition(_p);
    IdentifierCatchPattern* ret = new(_rp) IdentifierCatchPattern(name, member, new(_rp) Position(start), new(_rp) Position(end));
    if (member != nullptr)
        member->parent = ret;
    return ret;
}

_Array<Postfix>* Parser::parsePostfixList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<Postfix>* ret = nullptr;
    while (true) {
        Postfix* node = parsePostfix(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<Postfix>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<Postfix>(ret) : nullptr;
}

Postfix* Parser::parsePostfix(_Page* _rp) {
    {
        OperatorPostfix* node = parseOperatorPostfix(_rp);
        if (node != nullptr)
            return node;
    }
    {
        FunctionCall* node = parseFunctionCall(_rp);
        if (node != nullptr)
            return node;
    }
    {
        MemberExpression* node = parseMemberExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Subscript* node = parseSubscript(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

OperatorPostfix* Parser::parseOperatorPostfix(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* postfixOperator = lexer->parsePostfixOperator(_rp);
    if (postfixOperator != nullptr)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    OperatorPostfix* ret = new(_rp) OperatorPostfix(postfixOperator, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

FunctionCall* Parser::parseFunctionCall(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    ParenthesizedExpression* arguments = parseParenthesizedExpression(_rp);
    if (arguments == nullptr)
        return nullptr;
    _Array<CatchClause>* catchClauses = parseCatchClauseList(_rp);
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
    return ret;
}

MemberExpression* Parser::parseMemberExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1)
        lexer->advance();
    else
        return nullptr;
    string* member = lexer->parseIdentifier(_rp);
    if ((member != nullptr) && isIdentifier(member))
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    MemberExpression* ret = new(_rp) MemberExpression(member, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

Subscript* Parser::parseSubscript(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1)
        lexer->advance();
    else
        return nullptr;
    _Array<ExpressionElement>* expressions = parseExpressionElementList(_rp);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

_Array<ExpressionElement>* Parser::parseExpressionElementList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<ExpressionElement>* ret = nullptr;
    while (true) {
        ExpressionElement* node = parseExpressionElement(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<ExpressionElement>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<ExpressionElement>(ret) : nullptr;
}

ExpressionElement* Parser::parseExpressionElement(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    ExpressionElement* ret = new(_rp) ExpressionElement(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return ret;
}

PrimaryExpression* Parser::parsePrimaryExpression(_Page* _rp) {
    {
        IdentifierExpression* node = parseIdentifierExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        LiteralExpression* node = parseLiteralExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        IfExpression* node = parseIfExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        SwitchExpression* node = parseSwitchExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ForExpression* node = parseForExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        WhileExpression* node = parseWhileExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        DoExpression* node = parseDoExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ParenthesizedExpression* node = parseParenthesizedExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ReturnExpression* node = parseReturnExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ThrowExpression* node = parseThrowExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        BreakExpression* node = parseBreakExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ConstructorCall* node = parseConstructorCall(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ThisExpression* node = parseThisExpression(_rp);
        if (node != nullptr)
            return node;
    }
    {
        NullExpression* node = parseNullExpression(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

IdentifierExpression* Parser::parseIdentifierExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    IdentifierExpression* ret = new(_rp) IdentifierExpression(name, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

LiteralExpression* Parser::parseLiteralExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Literal* literal = lexer->parseLiteral(_rp);
    if (literal != nullptr)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    LiteralExpression* ret = new(_rp) LiteralExpression(literal, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

IfExpression* Parser::parseIfExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successIf1 = lexer->parseKeyword(ifKeyword);
    if (successIf1)
        lexer->advance();
    else
        return nullptr;
    Expression* condition = parseExpression(_rp);
    if (condition == nullptr)
        return nullptr;
    Expression* consequent = parseExpression(_rp);
    if (consequent == nullptr)
        return nullptr;
    ElseClause* elseClause = parseElseClause(_rp);
    Position* end = lexer->getPosition(_p);
    IfExpression* ret = new(_rp) IfExpression(condition, consequent, elseClause, new(_rp) Position(start), new(_rp) Position(end));
    condition->parent = ret;
    consequent->parent = ret;
    if (elseClause != nullptr)
        elseClause->parent = ret;
    return ret;
}

SwitchExpression* Parser::parseSwitchExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successSwitch1 = lexer->parseKeyword(switchKeyword);
    if (successSwitch1)
        lexer->advance();
    else
        return nullptr;
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    SwitchBody* body = parseSwitchBody(_rp);
    if (body == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    SwitchExpression* ret = new(_rp) SwitchExpression(expression, body, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    body->parent = ret;
    return ret;
}

ForExpression* Parser::parseForExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successFor1 = lexer->parseKeyword(forKeyword);
    if (successFor1)
        lexer->advance();
    else
        return nullptr;
    Pattern* pattern = parsePattern(_rp);
    if (pattern == nullptr)
        return nullptr;
    bool successIn3 = lexer->parseKeyword(inKeyword);
    if (successIn3)
        lexer->advance();
    else
        return nullptr;
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Expression* code = parseExpression(_rp);
    if (code == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ForExpression* ret = new(_rp) ForExpression(pattern, expression, code, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    expression->parent = ret;
    code->parent = ret;
    return ret;
}

WhileExpression* Parser::parseWhileExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successWhile1 = lexer->parseKeyword(whileKeyword);
    if (successWhile1)
        lexer->advance();
    else
        return nullptr;
    Expression* condition = parseExpression(_rp);
    if (condition == nullptr)
        return nullptr;
    Expression* code = parseExpression(_rp);
    if (code == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    WhileExpression* ret = new(_rp) WhileExpression(condition, code, new(_rp) Position(start), new(_rp) Position(end));
    condition->parent = ret;
    code->parent = ret;
    return ret;
}

DoExpression* Parser::parseDoExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successDo1 = lexer->parseKeyword(doKeyword);
    if (successDo1)
        lexer->advance();
    else
        return nullptr;
    Expression* code = parseExpression(_rp);
    if (code == nullptr)
        return nullptr;
    bool successWhile3 = lexer->parseKeyword(whileKeyword);
    if (successWhile3)
        lexer->advance();
    else
        return nullptr;
    Expression* condition = parseExpression(_rp);
    if (condition == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    DoExpression* ret = new(_rp) DoExpression(code, condition, new(_rp) Position(start), new(_rp) Position(end));
    code->parent = ret;
    condition->parent = ret;
    return ret;
}

ParenthesizedExpression* Parser::parseParenthesizedExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1)
        lexer->advance();
    else
        return nullptr;
    _Array<ExpressionElement>* expressionElements = parseExpressionElementList(_rp);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

ReturnExpression* Parser::parseReturnExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successReturn1 = lexer->parseKeyword(returnKeyword);
    if (successReturn1)
        lexer->advance();
    else
        return nullptr;
    ParenthesizedExpression* expression = parseParenthesizedExpression(_rp);
    Position* end = lexer->getPosition(_p);
    ReturnExpression* ret = new(_rp) ReturnExpression(expression, new(_rp) Position(start), new(_rp) Position(end));
    if (expression != nullptr)
        expression->parent = ret;
    return ret;
}

ThrowExpression* Parser::parseThrowExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successThrow1 = lexer->parseKeyword(throwKeyword);
    if (successThrow1)
        lexer->advance();
    else
        return nullptr;
    IdentifierExpression* error = parseIdentifierExpression(_rp);
    if (error == nullptr)
        return nullptr;
    ParenthesizedExpression* arguments = parseParenthesizedExpression(_rp);
    Position* end = lexer->getPosition(_p);
    ThrowExpression* ret = new(_rp) ThrowExpression(error, arguments, new(_rp) Position(start), new(_rp) Position(end));
    error->parent = ret;
    if (arguments != nullptr)
        arguments->parent = ret;
    return ret;
}

BreakExpression* Parser::parseBreakExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successBreak1 = lexer->parseKeyword(breakKeyword);
    if (successBreak1)
        lexer->advance();
    else
        return nullptr;
    ParenthesizedExpression* expression = parseParenthesizedExpression(_rp);
    Position* end = lexer->getPosition(_p);
    BreakExpression* ret = new(_rp) BreakExpression(expression, new(_rp) Position(start), new(_rp) Position(end));
    if (expression != nullptr)
        expression->parent = ret;
    return ret;
}

ConstructorCall* Parser::parseConstructorCall(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successNew1 = lexer->parseKeyword(newKeyword);
    if (successNew1)
        lexer->advance();
    else
        return nullptr;
    Type* typeToConstruct = parseType(_rp);
    if (typeToConstruct == nullptr)
        return nullptr;
    ParenthesizedExpression* arguments = parseParenthesizedExpression(_rp);
    if (arguments == nullptr)
        return nullptr;
    _Array<CatchClause>* catchClauses = parseCatchClauseList(_rp);
    Position* end = lexer->getPosition(_p);
    ConstructorCall* ret = new(_rp) ConstructorCall(typeToConstruct, arguments, catchClauses, new(_rp) Position(start), new(_rp) Position(end));
    typeToConstruct->parent = ret;
    arguments->parent = ret;
    if (catchClauses != nullptr) {
        CatchClause* item = nullptr;
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++) {
            item = *(*catchClauses)[_i];
            item->parent = ret;
        }
    }
    return ret;
}

ThisExpression* Parser::parseThisExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successThis1 = lexer->parseKeyword(thisKeyword);
    if (successThis1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ThisExpression* ret = new(_rp) ThisExpression(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

NullExpression* Parser::parseNullExpression(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successNull1 = lexer->parseKeyword(nullKeyword);
    if (successNull1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    NullExpression* ret = new(_rp) NullExpression(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

ElseClause* Parser::parseElseClause(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successElse1 = lexer->parseKeyword(elseKeyword);
    if (successElse1)
        lexer->advance();
    else
        return nullptr;
    Expression* alternative = parseExpression(_rp);
    if (alternative == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ElseClause* ret = new(_rp) ElseClause(alternative, new(_rp) Position(start), new(_rp) Position(end));
    alternative->parent = ret;
    return ret;
}

SwitchBody* Parser::parseSwitchBody(_Page* _rp) {
    {
        CurliedSwitchBody* node = parseCurliedSwitchBody(_rp);
        if (node != nullptr)
            return node;
    }
    {
        NakedSwitchBody* node = parseNakedSwitchBody(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

CurliedSwitchBody* Parser::parseCurliedSwitchBody(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftCurly1 = lexer->parsePunctuation(leftCurly);
    if (successLeftCurly1)
        lexer->advance();
    else
        return nullptr;
    _Array<SwitchCase>* cases = parseSwitchCaseList(_rp);
    bool successRightCurly3 = lexer->parsePunctuation(rightCurly);
    if (successRightCurly3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

NakedSwitchBody* Parser::parseNakedSwitchBody(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    _Array<SwitchCase>* cases = parseSwitchCaseList(_rp);
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
    return ret;
}

_Array<SwitchCase>* Parser::parseSwitchCaseList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<SwitchCase>* ret = nullptr;
    while (true) {
        SwitchCase* node = parseSwitchCase(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<SwitchCase>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<SwitchCase>(ret) : nullptr;
}

SwitchCase* Parser::parseSwitchCase(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    CaseLabel* label = parseCaseLabel(_rp);
    if (label == nullptr)
        return nullptr;
    CaseContent* content = parseCaseContent(_rp);
    if (content == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    SwitchCase* ret = new(_rp) SwitchCase(label, content, new(_rp) Position(start), new(_rp) Position(end));
    label->parent = ret;
    content->parent = ret;
    return ret;
}

CaseLabel* Parser::parseCaseLabel(_Page* _rp) {
    {
        ItemCaseLabel* node = parseItemCaseLabel(_rp);
        if (node != nullptr)
            return node;
    }
    {
        DefaultCaseLabel* node = parseDefaultCaseLabel(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

ItemCaseLabel* Parser::parseItemCaseLabel(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successCase1 = lexer->parseKeyword(caseKeyword);
    if (successCase1)
        lexer->advance();
    else
        return nullptr;
    Pattern* pattern = parsePattern(_rp);
    if (pattern == nullptr)
        return nullptr;
    _Array<CaseItem>* additionalPatterns = parseCaseItemList(_rp);
    bool successColon4 = lexer->parsePunctuation(colon);
    if (successColon4)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

DefaultCaseLabel* Parser::parseDefaultCaseLabel(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successDefault1 = lexer->parseKeyword(defaultKeyword);
    if (successDefault1)
        lexer->advance();
    else
        return nullptr;
    bool successColon2 = lexer->parsePunctuation(colon);
    if (successColon2)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    DefaultCaseLabel* ret = new(_rp) DefaultCaseLabel(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

_Array<CaseItem>* Parser::parseCaseItemList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<CaseItem>* ret = nullptr;
    while (true) {
        CaseItem* node = parseCaseItem(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<CaseItem>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<CaseItem>(ret) : nullptr;
}

CaseItem* Parser::parseCaseItem(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successComma1 = lexer->parsePunctuation(comma);
    if (successComma1)
        lexer->advance();
    else
        return nullptr;
    Pattern* pattern = parsePattern(_rp);
    if (pattern == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    CaseItem* ret = new(_rp) CaseItem(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return ret;
}

Pattern* Parser::parsePattern(_Page* _rp) {
    {
        WildcardPattern* node = parseWildcardPattern(_rp);
        if (node != nullptr)
            return node;
    }
    {
        IdentifierPattern* node = parseIdentifierPattern(_rp);
        if (node != nullptr)
            return node;
    }
    {
        TuplePattern* node = parseTuplePattern(_rp);
        if (node != nullptr)
            return node;
    }
    {
        ExpressionPattern* node = parseExpressionPattern(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

WildcardPattern* Parser::parseWildcardPattern(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successUnderscore1 = lexer->parsePunctuation(underscore);
    if (successUnderscore1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    WildcardPattern* ret = new(_rp) WildcardPattern(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

IdentifierPattern* Parser::parseIdentifierPattern(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* identifier = lexer->parseIdentifier(_rp);
    if ((identifier != nullptr) && isIdentifier(identifier))
        lexer->advance();
    else
        return nullptr;
    TypeAnnotation* annotationForType = parseTypeAnnotation(_rp);
    Position* end = lexer->getPosition(_p);
    IdentifierPattern* ret = new(_rp) IdentifierPattern(identifier, annotationForType, new(_rp) Position(start), new(_rp) Position(end));
    if (annotationForType != nullptr)
        annotationForType->parent = ret;
    return ret;
}

TuplePattern* Parser::parseTuplePattern(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftParen1 = lexer->parsePunctuation(leftParen);
    if (successLeftParen1)
        lexer->advance();
    else
        return nullptr;
    _Array<TuplePatternElement>* elements = parseTuplePatternElementList(_rp);
    bool successRightParen3 = lexer->parsePunctuation(rightParen);
    if (successRightParen3)
        lexer->advance();
    else
        return nullptr;
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
    return ret;
}

ExpressionPattern* Parser::parseExpressionPattern(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Expression* expression = parseExpression(_rp);
    if (expression == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    ExpressionPattern* ret = new(_rp) ExpressionPattern(expression, new(_rp) Position(start), new(_rp) Position(end));
    expression->parent = ret;
    return ret;
}

_Array<TuplePatternElement>* Parser::parseTuplePatternElementList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<TuplePatternElement>* ret = nullptr;
    while (true) {
        TuplePatternElement* node = parseTuplePatternElement(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<TuplePatternElement>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<TuplePatternElement>(ret) : nullptr;
}

TuplePatternElement* Parser::parseTuplePatternElement(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Pattern* pattern = parsePattern(_rp);
    if (pattern == nullptr)
        return nullptr;
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    TuplePatternElement* ret = new(_rp) TuplePatternElement(pattern, new(_rp) Position(start), new(_rp) Position(end));
    pattern->parent = ret;
    return ret;
}

CaseContent* Parser::parseCaseContent(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    _Array<Statement>* statements = parseStatementList(_rp);
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
    return ret;
}

Type* Parser::parseType(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    string* name = lexer->parseIdentifier(_rp);
    if ((name != nullptr) && isIdentifier(name))
        lexer->advance();
    else
        return nullptr;
    Subtype* subType = parseSubtype(_rp);
    _Array<TypePostfix>* postfixes = parseTypePostfixList(_rp);
    LifeTime* lifeTime = parseLifeTime(_rp);
    Position* end = lexer->getPosition(_p);
    Type* ret = new(_rp) Type(name, subType, postfixes, lifeTime, new(_rp) Position(start), new(_rp) Position(end));
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
    if (lifeTime != nullptr)
        lifeTime->parent = ret;
    return ret;
}

TypeAnnotation* Parser::parseTypeAnnotation(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successColon1 = lexer->parsePunctuation(colon);
    if (successColon1)
        lexer->advance();
    else
        return nullptr;
    Type* annotationForType = parseType(_rp);
    if (annotationForType == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    TypeAnnotation* ret = new(_rp) TypeAnnotation(annotationForType, new(_rp) Position(start), new(_rp) Position(end));
    annotationForType->parent = ret;
    return ret;
}

Subtype* Parser::parseSubtype(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successDot1 = lexer->parsePunctuation(dot);
    if (successDot1)
        lexer->advance();
    else
        return nullptr;
    Type* type = parseType(_rp);
    if (type == nullptr)
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Subtype* ret = new(_rp) Subtype(type, new(_rp) Position(start), new(_rp) Position(end));
    type->parent = ret;
    return ret;
}

_Array<TypePostfix>* Parser::parseTypePostfixList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<TypePostfix>* ret = nullptr;
    while (true) {
        TypePostfix* node = parseTypePostfix(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<TypePostfix>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<TypePostfix>(ret) : nullptr;
}

TypePostfix* Parser::parseTypePostfix(_Page* _rp) {
    {
        IndexedType* node = parseIndexedType(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Pointer* node = parsePointer(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

IndexedType* Parser::parseIndexedType(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successLeftBracket1 = lexer->parsePunctuation(leftBracket);
    if (successLeftBracket1)
        lexer->advance();
    else
        return nullptr;
    Type* key = parseType(_rp);
    bool successRightBracket3 = lexer->parsePunctuation(rightBracket);
    if (successRightBracket3)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    IndexedType* ret = new(_rp) IndexedType(key, new(_rp) Position(start), new(_rp) Position(end));
    if (key != nullptr)
        key->parent = ret;
    return ret;
}

Pointer* Parser::parsePointer(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successCircumflex1 = lexer->parsePunctuation(circumflex);
    if (successCircumflex1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Pointer* ret = new(_rp) Pointer(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

LifeTime* Parser::parseLifeTime(_Page* _rp) {
    {
        Root* node = parseRoot(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Local* node = parseLocal(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Reference* node = parseReference(_rp);
        if (node != nullptr)
            return node;
    }
    {
        Thrown* node = parseThrown(_rp);
        if (node != nullptr)
            return node;
    }
    return nullptr;
}

Root* Parser::parseRoot(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successDollar1 = lexer->parsePunctuation(dollar);
    if (successDollar1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Root* ret = new(_rp) Root(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

Local* Parser::parseLocal(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successAt1 = lexer->parsePunctuation(at);
    if (successAt1)
        lexer->advance();
    else
        return nullptr;
    string* location = lexer->parseIdentifier(_rp);
    if ((location != nullptr) && isIdentifier(location))
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Local* ret = new(_rp) Local(location, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

Reference* Parser::parseReference(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successAmpersand1 = lexer->parsePunctuation(ampersand);
    if (successAmpersand1)
        lexer->advance();
    else
        return nullptr;
    Literal* age = lexer->parseLiteral(_rp);
    if (age != nullptr)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    Reference* ret = new(_rp) Reference(age, new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

Thrown* Parser::parseThrown(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successHash1 = lexer->parsePunctuation(hash);
    if (successHash1)
        lexer->advance();
    else
        return nullptr;
    Position* end = lexer->getPosition(_p);
    Thrown* ret = new(_rp) Thrown(new(_rp) Position(start), new(_rp) Position(end));
    return ret;
}

TypeInheritanceClause* Parser::parseTypeInheritanceClause(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    bool successExtends1 = lexer->parseKeyword(extendsKeyword);
    if (successExtends1)
        lexer->advance();
    else
        return nullptr;
    _Array<Inheritance>* inheritances = parseInheritanceList(_rp);
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
    return ret;
}

_Array<Inheritance>* Parser::parseInheritanceList(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    _Array<Inheritance>* ret = nullptr;
    while (true) {
        Inheritance* node = parseInheritance(_rp);
        if (node == nullptr)
            break;
        if (ret == nullptr)
            ret = new(_p) _Array<Inheritance>();
        ret->push(node);
    }
    return ret ? new(_rp) _Array<Inheritance>(ret) : nullptr;
}

Inheritance* Parser::parseInheritance(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    Position* start = lexer->getPreviousPosition(_p);
    Type* type = parseType(_rp);
    if (type == nullptr)
        return nullptr;
    bool successComma2 = lexer->parsePunctuation(comma);
    if (successComma2)
        lexer->advance();
    Position* end = lexer->getPosition(_p);
    Inheritance* ret = new(_rp) Inheritance(type, new(_rp) Position(start), new(_rp) Position(end));
    type->parent = ret;
    return ret;
}

bool Parser::isAtEnd() {
    return lexer->isAtEnd();
}

bool Parser::isIdentifier(string* id) {
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
    if (id->equals(doKeyword))
        return false;
    if (id->equals(returnKeyword))
        return false;
    if (id->equals(throwKeyword))
        return false;
    if (id->equals(breakKeyword))
        return false;
    if (id->equals(throwsKeyword))
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

bool Visitor::openProgram(Program* program) {
    return true;
}

void Visitor::closeProgram(Program* program) {
}

bool Visitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    return true;
}

void Visitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
}

bool Visitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    return true;
}

void Visitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool Visitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    return true;
}

void Visitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
}

bool Visitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    return true;
}

void Visitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
}

bool Visitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    return true;
}

void Visitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
}

bool Visitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    return true;
}

void Visitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
}

bool Visitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    return true;
}

void Visitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
}

bool Visitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    return true;
}

void Visitor::closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
}

bool Visitor::openCodeBlock(CodeBlock* codeBlock) {
    return true;
}

void Visitor::closeCodeBlock(CodeBlock* codeBlock) {
}

bool Visitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    return true;
}

void Visitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
}

bool Visitor::openInitializer(Initializer* initializer) {
    return true;
}

void Visitor::closeInitializer(Initializer* initializer) {
}

bool Visitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    return true;
}

void Visitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool Visitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    return true;
}

void Visitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool Visitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return true;
}

void Visitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

void Visitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void Visitor::visitStaticWord(StaticWord* staticWord) {
}

bool Visitor::openFunctionSignature(FunctionSignature* functionSignature) {
    return true;
}

void Visitor::closeFunctionSignature(FunctionSignature* functionSignature) {
}

bool Visitor::openFunctionResult(FunctionResult* functionResult) {
    return true;
}

void Visitor::closeFunctionResult(FunctionResult* functionResult) {
}

bool Visitor::openParameterClause(ParameterClause* parameterClause) {
    return true;
}

void Visitor::closeParameterClause(ParameterClause* parameterClause) {
}

bool Visitor::openConstParameter(ConstParameter* constParameter) {
    return true;
}

void Visitor::closeConstParameter(ConstParameter* constParameter) {
}

bool Visitor::openVarParameter(VarParameter* varParameter) {
    return true;
}

void Visitor::closeVarParameter(VarParameter* varParameter) {
}

bool Visitor::openThrowsClause(ThrowsClause* throwsClause) {
    return true;
}

void Visitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool Visitor::openEnumMember(EnumMember* enumMember) {
    return true;
}

void Visitor::closeEnumMember(EnumMember* enumMember) {
}

void Visitor::visitEnumCase(EnumCase* enumCase) {
}

bool Visitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void Visitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool Visitor::openClassBody(ClassBody* classBody) {
    return true;
}

void Visitor::closeClassBody(ClassBody* classBody) {
}

bool Visitor::openClassMember(ClassMember* classMember) {
    return true;
}

void Visitor::closeClassMember(ClassMember* classMember) {
}

bool Visitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    return true;
}

void Visitor::closePrefixExpression(PrefixExpression* prefixExpression) {
}

bool Visitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    return true;
}

void Visitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool Visitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    return true;
}

void Visitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool Visitor::openAssignment(Assignment* assignment) {
    return true;
}

void Visitor::closeAssignment(Assignment* assignment) {
}

bool Visitor::openTypeQuery(TypeQuery* typeQuery) {
    return true;
}

void Visitor::closeTypeQuery(TypeQuery* typeQuery) {
}

bool Visitor::openTypeCast(TypeCast* typeCast) {
    return true;
}

void Visitor::closeTypeCast(TypeCast* typeCast) {
}

bool Visitor::openCatchClause(CatchClause* catchClause) {
    return true;
}

void Visitor::closeCatchClause(CatchClause* catchClause) {
}

bool Visitor::openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
    return true;
}

void Visitor::closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
}

bool Visitor::openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
    return true;
}

void Visitor::closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
}

void Visitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
}

bool Visitor::openFunctionCall(FunctionCall* functionCall) {
    return true;
}

void Visitor::closeFunctionCall(FunctionCall* functionCall) {
}

void Visitor::visitMemberExpression(MemberExpression* memberExpression) {
}

bool Visitor::openSubscript(Subscript* subscript) {
    return true;
}

void Visitor::closeSubscript(Subscript* subscript) {
}

bool Visitor::openExpressionElement(ExpressionElement* expressionElement) {
    return true;
}

void Visitor::closeExpressionElement(ExpressionElement* expressionElement) {
}

void Visitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
}

void Visitor::visitLiteralExpression(LiteralExpression* literalExpression) {
}

bool Visitor::openIfExpression(IfExpression* ifExpression) {
    return true;
}

void Visitor::closeIfExpression(IfExpression* ifExpression) {
}

bool Visitor::openSwitchExpression(SwitchExpression* switchExpression) {
    return true;
}

void Visitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool Visitor::openForExpression(ForExpression* forExpression) {
    return true;
}

void Visitor::closeForExpression(ForExpression* forExpression) {
}

bool Visitor::openWhileExpression(WhileExpression* whileExpression) {
    return true;
}

void Visitor::closeWhileExpression(WhileExpression* whileExpression) {
}

bool Visitor::openDoExpression(DoExpression* doExpression) {
    return true;
}

void Visitor::closeDoExpression(DoExpression* doExpression) {
}

bool Visitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    return true;
}

void Visitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
}

bool Visitor::openReturnExpression(ReturnExpression* returnExpression) {
    return true;
}

void Visitor::closeReturnExpression(ReturnExpression* returnExpression) {
}

bool Visitor::openThrowExpression(ThrowExpression* throwExpression) {
    return true;
}

void Visitor::closeThrowExpression(ThrowExpression* throwExpression) {
}

bool Visitor::openBreakExpression(BreakExpression* breakExpression) {
    return true;
}

void Visitor::closeBreakExpression(BreakExpression* breakExpression) {
}

bool Visitor::openConstructorCall(ConstructorCall* constructorCall) {
    return true;
}

void Visitor::closeConstructorCall(ConstructorCall* constructorCall) {
}

void Visitor::visitThisExpression(ThisExpression* thisExpression) {
}

void Visitor::visitNullExpression(NullExpression* nullExpression) {
}

bool Visitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void Visitor::closeElseClause(ElseClause* elseClause) {
}

bool Visitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    return true;
}

void Visitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
}

bool Visitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return true;
}

void Visitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool Visitor::openSwitchCase(SwitchCase* switchCase) {
    return true;
}

void Visitor::closeSwitchCase(SwitchCase* switchCase) {
}

bool Visitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    return true;
}

void Visitor::closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
}

void Visitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool Visitor::openCaseItem(CaseItem* caseItem) {
    return true;
}

void Visitor::closeCaseItem(CaseItem* caseItem) {
}

void Visitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool Visitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    return true;
}

void Visitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
}

bool Visitor::openTuplePattern(TuplePattern* tuplePattern) {
    return true;
}

void Visitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool Visitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return true;
}

void Visitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
}

bool Visitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return true;
}

void Visitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool Visitor::openCaseContent(CaseContent* caseContent) {
    return true;
}

void Visitor::closeCaseContent(CaseContent* caseContent) {
}

bool Visitor::openType(Type* type) {
    return true;
}

void Visitor::closeType(Type* type) {
}

bool Visitor::openTypeAnnotation(TypeAnnotation* typeAnnotation) {
    return true;
}

void Visitor::closeTypeAnnotation(TypeAnnotation* typeAnnotation) {
}

bool Visitor::openSubtype(Subtype* subtype) {
    return true;
}

void Visitor::closeSubtype(Subtype* subtype) {
}

bool Visitor::openIndexedType(IndexedType* indexedType) {
    return true;
}

void Visitor::closeIndexedType(IndexedType* indexedType) {
}

void Visitor::visitPointer(Pointer* pointer) {
}

void Visitor::visitRoot(Root* root) {
}

void Visitor::visitLocal(Local* local) {
}

void Visitor::visitReference(Reference* reference) {
}

void Visitor::visitThrown(Thrown* thrown) {
}

bool Visitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return true;
}

void Visitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool Visitor::openInheritance(Inheritance* inheritance) {
    return true;
}

void Visitor::closeInheritance(Inheritance* inheritance) {
}

bool Visitor::_isCppVisitor() { return (false); }
bool Visitor::_isHeaderVisitor() { return (false); }
bool Visitor::_isSourceVisitor() { return (false); }

bool SyntaxNode::_isProgram() { return (false); }
bool SyntaxNode::_isCompilationUnit() { return (false); }
bool SyntaxNode::_isStatement() { return (false); }
bool SyntaxNode::_isDeclaration() { return (false); }
bool SyntaxNode::_isConstantDeclaration() { return (false); }
bool SyntaxNode::_isVariableDeclaration() { return (false); }
bool SyntaxNode::_isMutableDeclaration() { return (false); }
bool SyntaxNode::_isFunctionDeclaration() { return (false); }
bool SyntaxNode::_isEnumDeclaration() { return (false); }
bool SyntaxNode::_isClassDeclaration() { return (false); }
bool SyntaxNode::_isConstructorDeclaration() { return (false); }
bool SyntaxNode::_isExpression() { return (false); }
bool SyntaxNode::_isCodeBlock() { return (false); }
bool SyntaxNode::_isSimpleExpression() { return (false); }
bool SyntaxNode::_isInitializer() { return (false); }
bool SyntaxNode::_isBindingInitializer() { return (false); }
bool SyntaxNode::_isPatternInitializer() { return (false); }
bool SyntaxNode::_isAdditionalInitializer() { return (false); }
bool SyntaxNode::_isModifier() { return (false); }
bool SyntaxNode::_isOverrideWord() { return (false); }
bool SyntaxNode::_isStaticWord() { return (false); }
bool SyntaxNode::_isFunctionSignature() { return (false); }
bool SyntaxNode::_isFunctionResult() { return (false); }
bool SyntaxNode::_isParameterClause() { return (false); }
bool SyntaxNode::_isParameter() { return (false); }
bool SyntaxNode::_isConstParameter() { return (false); }
bool SyntaxNode::_isVarParameter() { return (false); }
bool SyntaxNode::_isThrowsClause() { return (false); }
bool SyntaxNode::_isEnumMember() { return (false); }
bool SyntaxNode::_isEnumCase() { return (false); }
bool SyntaxNode::_isAdditionalCase() { return (false); }
bool SyntaxNode::_isClassBody() { return (false); }
bool SyntaxNode::_isClassMember() { return (false); }
bool SyntaxNode::_isPrefixExpression() { return (false); }
bool SyntaxNode::_isPostfixExpression() { return (false); }
bool SyntaxNode::_isBinaryOp() { return (false); }
bool SyntaxNode::_isBinaryOperation() { return (false); }
bool SyntaxNode::_isAssignment() { return (false); }
bool SyntaxNode::_isTypeQuery() { return (false); }
bool SyntaxNode::_isTypeCast() { return (false); }
bool SyntaxNode::_isCatchClause() { return (false); }
bool SyntaxNode::_isCatchPattern() { return (false); }
bool SyntaxNode::_isWildCardCatchPattern() { return (false); }
bool SyntaxNode::_isIdentifierCatchPattern() { return (false); }
bool SyntaxNode::_isPostfix() { return (false); }
bool SyntaxNode::_isOperatorPostfix() { return (false); }
bool SyntaxNode::_isFunctionCall() { return (false); }
bool SyntaxNode::_isMemberExpression() { return (false); }
bool SyntaxNode::_isSubscript() { return (false); }
bool SyntaxNode::_isExpressionElement() { return (false); }
bool SyntaxNode::_isPrimaryExpression() { return (false); }
bool SyntaxNode::_isIdentifierExpression() { return (false); }
bool SyntaxNode::_isLiteralExpression() { return (false); }
bool SyntaxNode::_isIfExpression() { return (false); }
bool SyntaxNode::_isSwitchExpression() { return (false); }
bool SyntaxNode::_isForExpression() { return (false); }
bool SyntaxNode::_isWhileExpression() { return (false); }
bool SyntaxNode::_isDoExpression() { return (false); }
bool SyntaxNode::_isParenthesizedExpression() { return (false); }
bool SyntaxNode::_isReturnExpression() { return (false); }
bool SyntaxNode::_isThrowExpression() { return (false); }
bool SyntaxNode::_isBreakExpression() { return (false); }
bool SyntaxNode::_isConstructorCall() { return (false); }
bool SyntaxNode::_isThisExpression() { return (false); }
bool SyntaxNode::_isNullExpression() { return (false); }
bool SyntaxNode::_isElseClause() { return (false); }
bool SyntaxNode::_isSwitchBody() { return (false); }
bool SyntaxNode::_isCurliedSwitchBody() { return (false); }
bool SyntaxNode::_isNakedSwitchBody() { return (false); }
bool SyntaxNode::_isSwitchCase() { return (false); }
bool SyntaxNode::_isCaseLabel() { return (false); }
bool SyntaxNode::_isItemCaseLabel() { return (false); }
bool SyntaxNode::_isDefaultCaseLabel() { return (false); }
bool SyntaxNode::_isCaseItem() { return (false); }
bool SyntaxNode::_isPattern() { return (false); }
bool SyntaxNode::_isWildcardPattern() { return (false); }
bool SyntaxNode::_isIdentifierPattern() { return (false); }
bool SyntaxNode::_isTuplePattern() { return (false); }
bool SyntaxNode::_isExpressionPattern() { return (false); }
bool SyntaxNode::_isTuplePatternElement() { return (false); }
bool SyntaxNode::_isCaseContent() { return (false); }
bool SyntaxNode::_isType() { return (false); }
bool SyntaxNode::_isTypeAnnotation() { return (false); }
bool SyntaxNode::_isSubtype() { return (false); }
bool SyntaxNode::_isTypePostfix() { return (false); }
bool SyntaxNode::_isIndexedType() { return (false); }
bool SyntaxNode::_isPointer() { return (false); }
bool SyntaxNode::_isLifeTime() { return (false); }
bool SyntaxNode::_isRoot() { return (false); }
bool SyntaxNode::_isLocal() { return (false); }
bool SyntaxNode::_isReference() { return (false); }
bool SyntaxNode::_isThrown() { return (false); }
bool SyntaxNode::_isTypeInheritanceClause() { return (false); }
bool SyntaxNode::_isInheritance() { return (false); }

Program::Program(string* name, _Array<CompilationUnit>* compilationUnits) {
    start = new(_getPage()) Position(0, 0);
    end = new(_getPage()) Position(0, 0);
    this->name = name;
    this->compilationUnits = compilationUnits;
    this->parent = nullptr;
}

void Program::accept(Visitor* visitor) {
    if (!visitor->openProgram(this))
        return;
    if (compilationUnits != nullptr) {
        CompilationUnit* node = nullptr;
        size_t _compilationUnits_length = compilationUnits->length();
        for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
            node = *(*compilationUnits)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeProgram(this);
}

bool Program::_isProgram() { return (true); }

CompilationUnit::CompilationUnit(_Array<Statement>* statements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->statements = statements;
}

void CompilationUnit::accept(Visitor* visitor) {
    if (!visitor->openCompilationUnit(this))
        return;
    if (statements != nullptr) {
        Statement* node = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            node = *(*statements)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeCompilationUnit(this);
}

bool CompilationUnit::_isCompilationUnit() { return (true); }

void Statement::accept(Visitor* visitor) {
}

bool Statement::_isStatement() { return (true); }

bool Statement::_isDeclaration() { return (false); }
bool Statement::_isConstantDeclaration() { return (false); }
bool Statement::_isVariableDeclaration() { return (false); }
bool Statement::_isMutableDeclaration() { return (false); }
bool Statement::_isFunctionDeclaration() { return (false); }
bool Statement::_isEnumDeclaration() { return (false); }
bool Statement::_isClassDeclaration() { return (false); }
bool Statement::_isConstructorDeclaration() { return (false); }
bool Statement::_isExpression() { return (false); }
bool Statement::_isCodeBlock() { return (false); }
bool Statement::_isSimpleExpression() { return (false); }

void Declaration::accept(Visitor* visitor) {
}

bool Declaration::_isDeclaration() { return (true); }

bool Declaration::_isConstantDeclaration() { return (false); }
bool Declaration::_isVariableDeclaration() { return (false); }
bool Declaration::_isMutableDeclaration() { return (false); }
bool Declaration::_isFunctionDeclaration() { return (false); }
bool Declaration::_isEnumDeclaration() { return (false); }
bool Declaration::_isClassDeclaration() { return (false); }
bool Declaration::_isConstructorDeclaration() { return (false); }

void Expression::accept(Visitor* visitor) {
}

bool Expression::_isExpression() { return (true); }

bool Expression::_isCodeBlock() { return (false); }
bool Expression::_isSimpleExpression() { return (false); }

ConstantDeclaration::ConstantDeclaration(BindingInitializer* initializer, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->initializer = initializer;
}

void ConstantDeclaration::accept(Visitor* visitor) {
    if (!visitor->openConstantDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeConstantDeclaration(this);
}

bool ConstantDeclaration::_isConstantDeclaration() { return (true); }

VariableDeclaration::VariableDeclaration(BindingInitializer* initializer, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->initializer = initializer;
}

void VariableDeclaration::accept(Visitor* visitor) {
    if (!visitor->openVariableDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeVariableDeclaration(this);
}

bool VariableDeclaration::_isVariableDeclaration() { return (true); }

MutableDeclaration::MutableDeclaration(BindingInitializer* initializer, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->initializer = initializer;
}

void MutableDeclaration::accept(Visitor* visitor) {
    if (!visitor->openMutableDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeMutableDeclaration(this);
}

bool MutableDeclaration::_isMutableDeclaration() { return (true); }

FunctionDeclaration::FunctionDeclaration(_Array<Modifier>* modifiers, string* name, FunctionSignature* signature, Expression* body, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->modifiers = modifiers;
    this->name = name;
    this->signature = signature;
    this->body = body;
}

void FunctionDeclaration::accept(Visitor* visitor) {
    if (!visitor->openFunctionDeclaration(this))
        return;
    if (modifiers != nullptr) {
        Modifier* node = nullptr;
        size_t _modifiers_length = modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++) {
            node = *(*modifiers)[_i];
            node->accept(visitor);
        }
    }
    signature->accept(visitor);
    if (body != nullptr)
        body->accept(visitor);
    visitor->closeFunctionDeclaration(this);
}

bool FunctionDeclaration::_isFunctionDeclaration() { return (true); }

EnumDeclaration::EnumDeclaration(string* name, _Array<EnumMember>* members, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->members = members;
}

void EnumDeclaration::accept(Visitor* visitor) {
    if (!visitor->openEnumDeclaration(this))
        return;
    if (members != nullptr) {
        EnumMember* node = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            node = *(*members)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeEnumDeclaration(this);
}

bool EnumDeclaration::_isEnumDeclaration() { return (true); }

ClassDeclaration::ClassDeclaration(string* name, TypeInheritanceClause* typeInheritanceClause, ClassBody* body, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->typeInheritanceClause = typeInheritanceClause;
    this->body = body;
}

void ClassDeclaration::accept(Visitor* visitor) {
    if (!visitor->openClassDeclaration(this))
        return;
    if (typeInheritanceClause != nullptr)
        typeInheritanceClause->accept(visitor);
    if (body != nullptr)
        body->accept(visitor);
    visitor->closeClassDeclaration(this);
}

bool ClassDeclaration::_isClassDeclaration() { return (true); }

ConstructorDeclaration::ConstructorDeclaration(ParameterClause* parameterClause, Expression* body, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->parameterClause = parameterClause;
    this->body = body;
}

void ConstructorDeclaration::accept(Visitor* visitor) {
    if (!visitor->openConstructorDeclaration(this))
        return;
    parameterClause->accept(visitor);
    body->accept(visitor);
    visitor->closeConstructorDeclaration(this);
}

bool ConstructorDeclaration::_isConstructorDeclaration() { return (true); }

CodeBlock::CodeBlock(_Array<Statement>* statements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->statements = statements;
}

void CodeBlock::accept(Visitor* visitor) {
    if (!visitor->openCodeBlock(this))
        return;
    if (statements != nullptr) {
        Statement* node = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            node = *(*statements)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeCodeBlock(this);
}

bool CodeBlock::_isCodeBlock() { return (true); }

SimpleExpression::SimpleExpression(PrefixExpression* prefixExpression, _Array<BinaryOp>* binaryOps, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->prefixExpression = prefixExpression;
    this->binaryOps = binaryOps;
}

void SimpleExpression::accept(Visitor* visitor) {
    if (!visitor->openSimpleExpression(this))
        return;
    prefixExpression->accept(visitor);
    if (binaryOps != nullptr) {
        BinaryOp* node = nullptr;
        size_t _binaryOps_length = binaryOps->length();
        for (size_t _i = 0; _i < _binaryOps_length; _i++) {
            node = *(*binaryOps)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeSimpleExpression(this);
}

bool SimpleExpression::_isSimpleExpression() { return (true); }

Initializer::Initializer(Expression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
}

void Initializer::accept(Visitor* visitor) {
    if (!visitor->openInitializer(this))
        return;
    expression->accept(visitor);
    visitor->closeInitializer(this);
}

bool Initializer::_isInitializer() { return (true); }

BindingInitializer::BindingInitializer(PatternInitializer* initializer, _Array<AdditionalInitializer>* additionalInitializers, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->initializer = initializer;
    this->additionalInitializers = additionalInitializers;
}

void BindingInitializer::accept(Visitor* visitor) {
    if (!visitor->openBindingInitializer(this))
        return;
    initializer->accept(visitor);
    if (additionalInitializers != nullptr) {
        AdditionalInitializer* node = nullptr;
        size_t _additionalInitializers_length = additionalInitializers->length();
        for (size_t _i = 0; _i < _additionalInitializers_length; _i++) {
            node = *(*additionalInitializers)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeBindingInitializer(this);
}

bool BindingInitializer::_isBindingInitializer() { return (true); }

PatternInitializer::PatternInitializer(Pattern* pattern, Initializer* initializer, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
    this->initializer = initializer;
}

void PatternInitializer::accept(Visitor* visitor) {
    if (!visitor->openPatternInitializer(this))
        return;
    pattern->accept(visitor);
    if (initializer != nullptr)
        initializer->accept(visitor);
    visitor->closePatternInitializer(this);
}

bool PatternInitializer::_isPatternInitializer() { return (true); }

AdditionalInitializer::AdditionalInitializer(PatternInitializer* pattern, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
}

void AdditionalInitializer::accept(Visitor* visitor) {
    if (!visitor->openAdditionalInitializer(this))
        return;
    pattern->accept(visitor);
    visitor->closeAdditionalInitializer(this);
}

bool AdditionalInitializer::_isAdditionalInitializer() { return (true); }

void Modifier::accept(Visitor* visitor) {
}

bool Modifier::_isModifier() { return (true); }

bool Modifier::_isOverrideWord() { return (false); }
bool Modifier::_isStaticWord() { return (false); }

OverrideWord::OverrideWord(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void OverrideWord::accept(Visitor* visitor) {
    visitor->visitOverrideWord(this);
}

bool OverrideWord::_isOverrideWord() { return (true); }

StaticWord::StaticWord(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void StaticWord::accept(Visitor* visitor) {
    visitor->visitStaticWord(this);
}

bool StaticWord::_isStaticWord() { return (true); }

FunctionSignature::FunctionSignature(ParameterClause* parameterClause, FunctionResult* result, ThrowsClause* throwsClause, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->parameterClause = parameterClause;
    this->result = result;
    this->throwsClause = throwsClause;
}

void FunctionSignature::accept(Visitor* visitor) {
    if (!visitor->openFunctionSignature(this))
        return;
    parameterClause->accept(visitor);
    if (result != nullptr)
        result->accept(visitor);
    if (throwsClause != nullptr)
        throwsClause->accept(visitor);
    visitor->closeFunctionSignature(this);
}

bool FunctionSignature::_isFunctionSignature() { return (true); }

FunctionResult::FunctionResult(Type* resultType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->resultType = resultType;
}

void FunctionResult::accept(Visitor* visitor) {
    if (!visitor->openFunctionResult(this))
        return;
    resultType->accept(visitor);
    visitor->closeFunctionResult(this);
}

bool FunctionResult::_isFunctionResult() { return (true); }

ParameterClause::ParameterClause(_Array<Parameter>* parameters, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->parameters = parameters;
}

void ParameterClause::accept(Visitor* visitor) {
    if (!visitor->openParameterClause(this))
        return;
    if (parameters != nullptr) {
        Parameter* node = nullptr;
        size_t _parameters_length = parameters->length();
        for (size_t _i = 0; _i < _parameters_length; _i++) {
            node = *(*parameters)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeParameterClause(this);
}

bool ParameterClause::_isParameterClause() { return (true); }

void Parameter::accept(Visitor* visitor) {
}

bool Parameter::_isParameter() { return (true); }

bool Parameter::_isConstParameter() { return (false); }
bool Parameter::_isVarParameter() { return (false); }

ConstParameter::ConstParameter(string* name, Type* parameterType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->parameterType = parameterType;
}

void ConstParameter::accept(Visitor* visitor) {
    if (!visitor->openConstParameter(this))
        return;
    parameterType->accept(visitor);
    visitor->closeConstParameter(this);
}

bool ConstParameter::_isConstParameter() { return (true); }

VarParameter::VarParameter(string* name, Type* parameterType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->parameterType = parameterType;
}

void VarParameter::accept(Visitor* visitor) {
    if (!visitor->openVarParameter(this))
        return;
    parameterType->accept(visitor);
    visitor->closeVarParameter(this);
}

bool VarParameter::_isVarParameter() { return (true); }

ThrowsClause::ThrowsClause(Type* throwsType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->throwsType = throwsType;
}

void ThrowsClause::accept(Visitor* visitor) {
    if (!visitor->openThrowsClause(this))
        return;
    throwsType->accept(visitor);
    visitor->closeThrowsClause(this);
}

bool ThrowsClause::_isThrowsClause() { return (true); }

EnumMember::EnumMember(EnumCase* enumCase, _Array<AdditionalCase>* additionalCases, ParameterClause* parameterClause, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->enumCase = enumCase;
    this->additionalCases = additionalCases;
    this->parameterClause = parameterClause;
}

void EnumMember::accept(Visitor* visitor) {
    if (!visitor->openEnumMember(this))
        return;
    enumCase->accept(visitor);
    if (additionalCases != nullptr) {
        AdditionalCase* node = nullptr;
        size_t _additionalCases_length = additionalCases->length();
        for (size_t _i = 0; _i < _additionalCases_length; _i++) {
            node = *(*additionalCases)[_i];
            node->accept(visitor);
        }
    }
    if (parameterClause != nullptr)
        parameterClause->accept(visitor);
    visitor->closeEnumMember(this);
}

bool EnumMember::_isEnumMember() { return (true); }

EnumCase::EnumCase(string* name, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
}

void EnumCase::accept(Visitor* visitor) {
    visitor->visitEnumCase(this);
}

bool EnumCase::_isEnumCase() { return (true); }

AdditionalCase::AdditionalCase(EnumCase* enumCase, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->enumCase = enumCase;
}

void AdditionalCase::accept(Visitor* visitor) {
    if (!visitor->openAdditionalCase(this))
        return;
    enumCase->accept(visitor);
    visitor->closeAdditionalCase(this);
}

bool AdditionalCase::_isAdditionalCase() { return (true); }

ClassBody::ClassBody(_Array<ClassMember>* members, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->members = members;
}

void ClassBody::accept(Visitor* visitor) {
    if (!visitor->openClassBody(this))
        return;
    if (members != nullptr) {
        ClassMember* node = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            node = *(*members)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeClassBody(this);
}

bool ClassBody::_isClassBody() { return (true); }

ClassMember::ClassMember(Declaration* declaration, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->declaration = declaration;
}

void ClassMember::accept(Visitor* visitor) {
    if (!visitor->openClassMember(this))
        return;
    declaration->accept(visitor);
    visitor->closeClassMember(this);
}

bool ClassMember::_isClassMember() { return (true); }

PrefixExpression::PrefixExpression(string* prefixOperator, PostfixExpression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->prefixOperator = prefixOperator;
    this->expression = expression;
}

void PrefixExpression::accept(Visitor* visitor) {
    if (!visitor->openPrefixExpression(this))
        return;
    expression->accept(visitor);
    visitor->closePrefixExpression(this);
}

bool PrefixExpression::_isPrefixExpression() { return (true); }

PostfixExpression::PostfixExpression(PrimaryExpression* primaryExpression, _Array<Postfix>* postfixes, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->primaryExpression = primaryExpression;
    this->postfixes = postfixes;
}

void PostfixExpression::accept(Visitor* visitor) {
    if (!visitor->openPostfixExpression(this))
        return;
    primaryExpression->accept(visitor);
    if (postfixes != nullptr) {
        Postfix* node = nullptr;
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++) {
            node = *(*postfixes)[_i];
            node->accept(visitor);
        }
    }
    visitor->closePostfixExpression(this);
}

bool PostfixExpression::_isPostfixExpression() { return (true); }

void BinaryOp::accept(Visitor* visitor) {
}

bool BinaryOp::_isBinaryOp() { return (true); }

bool BinaryOp::_isBinaryOperation() { return (false); }
bool BinaryOp::_isAssignment() { return (false); }
bool BinaryOp::_isTypeQuery() { return (false); }
bool BinaryOp::_isTypeCast() { return (false); }

BinaryOperation::BinaryOperation(string* binaryOperator, PrefixExpression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->binaryOperator = binaryOperator;
    this->expression = expression;
}

void BinaryOperation::accept(Visitor* visitor) {
    if (!visitor->openBinaryOperation(this))
        return;
    expression->accept(visitor);
    visitor->closeBinaryOperation(this);
}

bool BinaryOperation::_isBinaryOperation() { return (true); }

Assignment::Assignment(Expression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
}

void Assignment::accept(Visitor* visitor) {
    if (!visitor->openAssignment(this))
        return;
    expression->accept(visitor);
    visitor->closeAssignment(this);
}

bool Assignment::_isAssignment() { return (true); }

TypeQuery::TypeQuery(Type* objectType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->objectType = objectType;
}

void TypeQuery::accept(Visitor* visitor) {
    if (!visitor->openTypeQuery(this))
        return;
    objectType->accept(visitor);
    visitor->closeTypeQuery(this);
}

bool TypeQuery::_isTypeQuery() { return (true); }

TypeCast::TypeCast(Type* objectType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->objectType = objectType;
}

void TypeCast::accept(Visitor* visitor) {
    if (!visitor->openTypeCast(this))
        return;
    objectType->accept(visitor);
    visitor->closeTypeCast(this);
}

bool TypeCast::_isTypeCast() { return (true); }

CatchClause::CatchClause(CatchPattern* catchPattern, TuplePattern* bindingPattern, Expression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->catchPattern = catchPattern;
    this->bindingPattern = bindingPattern;
    this->expression = expression;
}

void CatchClause::accept(Visitor* visitor) {
    if (!visitor->openCatchClause(this))
        return;
    catchPattern->accept(visitor);
    if (bindingPattern != nullptr)
        bindingPattern->accept(visitor);
    expression->accept(visitor);
    visitor->closeCatchClause(this);
}

bool CatchClause::_isCatchClause() { return (true); }

void CatchPattern::accept(Visitor* visitor) {
}

bool CatchPattern::_isCatchPattern() { return (true); }

bool CatchPattern::_isWildCardCatchPattern() { return (false); }
bool CatchPattern::_isIdentifierCatchPattern() { return (false); }

WildCardCatchPattern::WildCardCatchPattern(WildcardPattern* pattern, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
}

void WildCardCatchPattern::accept(Visitor* visitor) {
    if (!visitor->openWildCardCatchPattern(this))
        return;
    pattern->accept(visitor);
    visitor->closeWildCardCatchPattern(this);
}

bool WildCardCatchPattern::_isWildCardCatchPattern() { return (true); }

IdentifierCatchPattern::IdentifierCatchPattern(string* name, MemberExpression* member, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->member = member;
}

void IdentifierCatchPattern::accept(Visitor* visitor) {
    if (!visitor->openIdentifierCatchPattern(this))
        return;
    if (member != nullptr)
        member->accept(visitor);
    visitor->closeIdentifierCatchPattern(this);
}

bool IdentifierCatchPattern::_isIdentifierCatchPattern() { return (true); }

void Postfix::accept(Visitor* visitor) {
}

bool Postfix::_isPostfix() { return (true); }

bool Postfix::_isOperatorPostfix() { return (false); }
bool Postfix::_isFunctionCall() { return (false); }
bool Postfix::_isMemberExpression() { return (false); }
bool Postfix::_isSubscript() { return (false); }

OperatorPostfix::OperatorPostfix(string* postfixOperator, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->postfixOperator = postfixOperator;
}

void OperatorPostfix::accept(Visitor* visitor) {
    visitor->visitOperatorPostfix(this);
}

bool OperatorPostfix::_isOperatorPostfix() { return (true); }

FunctionCall::FunctionCall(ParenthesizedExpression* arguments, _Array<CatchClause>* catchClauses, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->arguments = arguments;
    this->catchClauses = catchClauses;
}

void FunctionCall::accept(Visitor* visitor) {
    if (!visitor->openFunctionCall(this))
        return;
    arguments->accept(visitor);
    if (catchClauses != nullptr) {
        CatchClause* node = nullptr;
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++) {
            node = *(*catchClauses)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeFunctionCall(this);
}

bool FunctionCall::_isFunctionCall() { return (true); }

MemberExpression::MemberExpression(string* member, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->member = member;
}

void MemberExpression::accept(Visitor* visitor) {
    visitor->visitMemberExpression(this);
}

bool MemberExpression::_isMemberExpression() { return (true); }

Subscript::Subscript(_Array<ExpressionElement>* expressions, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expressions = expressions;
}

void Subscript::accept(Visitor* visitor) {
    if (!visitor->openSubscript(this))
        return;
    if (expressions != nullptr) {
        ExpressionElement* node = nullptr;
        size_t _expressions_length = expressions->length();
        for (size_t _i = 0; _i < _expressions_length; _i++) {
            node = *(*expressions)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeSubscript(this);
}

bool Subscript::_isSubscript() { return (true); }

ExpressionElement::ExpressionElement(Expression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
}

void ExpressionElement::accept(Visitor* visitor) {
    if (!visitor->openExpressionElement(this))
        return;
    expression->accept(visitor);
    visitor->closeExpressionElement(this);
}

bool ExpressionElement::_isExpressionElement() { return (true); }

void PrimaryExpression::accept(Visitor* visitor) {
}

bool PrimaryExpression::_isPrimaryExpression() { return (true); }

bool PrimaryExpression::_isIdentifierExpression() { return (false); }
bool PrimaryExpression::_isLiteralExpression() { return (false); }
bool PrimaryExpression::_isIfExpression() { return (false); }
bool PrimaryExpression::_isSwitchExpression() { return (false); }
bool PrimaryExpression::_isForExpression() { return (false); }
bool PrimaryExpression::_isWhileExpression() { return (false); }
bool PrimaryExpression::_isDoExpression() { return (false); }
bool PrimaryExpression::_isParenthesizedExpression() { return (false); }
bool PrimaryExpression::_isReturnExpression() { return (false); }
bool PrimaryExpression::_isThrowExpression() { return (false); }
bool PrimaryExpression::_isBreakExpression() { return (false); }
bool PrimaryExpression::_isConstructorCall() { return (false); }
bool PrimaryExpression::_isThisExpression() { return (false); }
bool PrimaryExpression::_isNullExpression() { return (false); }

IdentifierExpression::IdentifierExpression(string* name, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
}

void IdentifierExpression::accept(Visitor* visitor) {
    visitor->visitIdentifierExpression(this);
}

bool IdentifierExpression::_isIdentifierExpression() { return (true); }

LiteralExpression::LiteralExpression(Literal* literal, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->literal = literal;
}

void LiteralExpression::accept(Visitor* visitor) {
    visitor->visitLiteralExpression(this);
}

bool LiteralExpression::_isLiteralExpression() { return (true); }

IfExpression::IfExpression(Expression* condition, Expression* consequent, ElseClause* elseClause, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->condition = condition;
    this->consequent = consequent;
    this->elseClause = elseClause;
}

void IfExpression::accept(Visitor* visitor) {
    if (!visitor->openIfExpression(this))
        return;
    condition->accept(visitor);
    consequent->accept(visitor);
    if (elseClause != nullptr)
        elseClause->accept(visitor);
    visitor->closeIfExpression(this);
}

bool IfExpression::_isIfExpression() { return (true); }

SwitchExpression::SwitchExpression(Expression* expression, SwitchBody* body, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
    this->body = body;
}

void SwitchExpression::accept(Visitor* visitor) {
    if (!visitor->openSwitchExpression(this))
        return;
    expression->accept(visitor);
    body->accept(visitor);
    visitor->closeSwitchExpression(this);
}

bool SwitchExpression::_isSwitchExpression() { return (true); }

ForExpression::ForExpression(Pattern* pattern, Expression* expression, Expression* code, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
    this->expression = expression;
    this->code = code;
}

void ForExpression::accept(Visitor* visitor) {
    if (!visitor->openForExpression(this))
        return;
    pattern->accept(visitor);
    expression->accept(visitor);
    code->accept(visitor);
    visitor->closeForExpression(this);
}

bool ForExpression::_isForExpression() { return (true); }

WhileExpression::WhileExpression(Expression* condition, Expression* code, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->condition = condition;
    this->code = code;
}

void WhileExpression::accept(Visitor* visitor) {
    if (!visitor->openWhileExpression(this))
        return;
    condition->accept(visitor);
    code->accept(visitor);
    visitor->closeWhileExpression(this);
}

bool WhileExpression::_isWhileExpression() { return (true); }

DoExpression::DoExpression(Expression* code, Expression* condition, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->code = code;
    this->condition = condition;
}

void DoExpression::accept(Visitor* visitor) {
    if (!visitor->openDoExpression(this))
        return;
    code->accept(visitor);
    condition->accept(visitor);
    visitor->closeDoExpression(this);
}

bool DoExpression::_isDoExpression() { return (true); }

ParenthesizedExpression::ParenthesizedExpression(_Array<ExpressionElement>* expressionElements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expressionElements = expressionElements;
}

void ParenthesizedExpression::accept(Visitor* visitor) {
    if (!visitor->openParenthesizedExpression(this))
        return;
    if (expressionElements != nullptr) {
        ExpressionElement* node = nullptr;
        size_t _expressionElements_length = expressionElements->length();
        for (size_t _i = 0; _i < _expressionElements_length; _i++) {
            node = *(*expressionElements)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeParenthesizedExpression(this);
}

bool ParenthesizedExpression::_isParenthesizedExpression() { return (true); }

ReturnExpression::ReturnExpression(ParenthesizedExpression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
}

void ReturnExpression::accept(Visitor* visitor) {
    if (!visitor->openReturnExpression(this))
        return;
    if (expression != nullptr)
        expression->accept(visitor);
    visitor->closeReturnExpression(this);
}

bool ReturnExpression::_isReturnExpression() { return (true); }

ThrowExpression::ThrowExpression(IdentifierExpression* error, ParenthesizedExpression* arguments, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->error = error;
    this->arguments = arguments;
}

void ThrowExpression::accept(Visitor* visitor) {
    if (!visitor->openThrowExpression(this))
        return;
    error->accept(visitor);
    if (arguments != nullptr)
        arguments->accept(visitor);
    visitor->closeThrowExpression(this);
}

bool ThrowExpression::_isThrowExpression() { return (true); }

BreakExpression::BreakExpression(ParenthesizedExpression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
}

void BreakExpression::accept(Visitor* visitor) {
    if (!visitor->openBreakExpression(this))
        return;
    if (expression != nullptr)
        expression->accept(visitor);
    visitor->closeBreakExpression(this);
}

bool BreakExpression::_isBreakExpression() { return (true); }

ConstructorCall::ConstructorCall(Type* typeToConstruct, ParenthesizedExpression* arguments, _Array<CatchClause>* catchClauses, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->typeToConstruct = typeToConstruct;
    this->arguments = arguments;
    this->catchClauses = catchClauses;
}

void ConstructorCall::accept(Visitor* visitor) {
    if (!visitor->openConstructorCall(this))
        return;
    typeToConstruct->accept(visitor);
    arguments->accept(visitor);
    if (catchClauses != nullptr) {
        CatchClause* node = nullptr;
        size_t _catchClauses_length = catchClauses->length();
        for (size_t _i = 0; _i < _catchClauses_length; _i++) {
            node = *(*catchClauses)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeConstructorCall(this);
}

bool ConstructorCall::_isConstructorCall() { return (true); }

ThisExpression::ThisExpression(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void ThisExpression::accept(Visitor* visitor) {
    visitor->visitThisExpression(this);
}

bool ThisExpression::_isThisExpression() { return (true); }

NullExpression::NullExpression(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void NullExpression::accept(Visitor* visitor) {
    visitor->visitNullExpression(this);
}

bool NullExpression::_isNullExpression() { return (true); }

ElseClause::ElseClause(Expression* alternative, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->alternative = alternative;
}

void ElseClause::accept(Visitor* visitor) {
    if (!visitor->openElseClause(this))
        return;
    alternative->accept(visitor);
    visitor->closeElseClause(this);
}

bool ElseClause::_isElseClause() { return (true); }

void SwitchBody::accept(Visitor* visitor) {
}

bool SwitchBody::_isSwitchBody() { return (true); }

bool SwitchBody::_isCurliedSwitchBody() { return (false); }
bool SwitchBody::_isNakedSwitchBody() { return (false); }

CurliedSwitchBody::CurliedSwitchBody(_Array<SwitchCase>* cases, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->cases = cases;
}

void CurliedSwitchBody::accept(Visitor* visitor) {
    if (!visitor->openCurliedSwitchBody(this))
        return;
    if (cases != nullptr) {
        SwitchCase* node = nullptr;
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++) {
            node = *(*cases)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeCurliedSwitchBody(this);
}

bool CurliedSwitchBody::_isCurliedSwitchBody() { return (true); }

NakedSwitchBody::NakedSwitchBody(_Array<SwitchCase>* cases, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->cases = cases;
}

void NakedSwitchBody::accept(Visitor* visitor) {
    if (!visitor->openNakedSwitchBody(this))
        return;
    if (cases != nullptr) {
        SwitchCase* node = nullptr;
        size_t _cases_length = cases->length();
        for (size_t _i = 0; _i < _cases_length; _i++) {
            node = *(*cases)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeNakedSwitchBody(this);
}

bool NakedSwitchBody::_isNakedSwitchBody() { return (true); }

SwitchCase::SwitchCase(CaseLabel* label, CaseContent* content, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->label = label;
    this->content = content;
}

void SwitchCase::accept(Visitor* visitor) {
    if (!visitor->openSwitchCase(this))
        return;
    label->accept(visitor);
    content->accept(visitor);
    visitor->closeSwitchCase(this);
}

bool SwitchCase::_isSwitchCase() { return (true); }

void CaseLabel::accept(Visitor* visitor) {
}

bool CaseLabel::_isCaseLabel() { return (true); }

bool CaseLabel::_isItemCaseLabel() { return (false); }
bool CaseLabel::_isDefaultCaseLabel() { return (false); }

ItemCaseLabel::ItemCaseLabel(Pattern* pattern, _Array<CaseItem>* additionalPatterns, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
    this->additionalPatterns = additionalPatterns;
}

void ItemCaseLabel::accept(Visitor* visitor) {
    if (!visitor->openItemCaseLabel(this))
        return;
    pattern->accept(visitor);
    if (additionalPatterns != nullptr) {
        CaseItem* node = nullptr;
        size_t _additionalPatterns_length = additionalPatterns->length();
        for (size_t _i = 0; _i < _additionalPatterns_length; _i++) {
            node = *(*additionalPatterns)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeItemCaseLabel(this);
}

bool ItemCaseLabel::_isItemCaseLabel() { return (true); }

DefaultCaseLabel::DefaultCaseLabel(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void DefaultCaseLabel::accept(Visitor* visitor) {
    visitor->visitDefaultCaseLabel(this);
}

bool DefaultCaseLabel::_isDefaultCaseLabel() { return (true); }

CaseItem::CaseItem(Pattern* pattern, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
}

void CaseItem::accept(Visitor* visitor) {
    if (!visitor->openCaseItem(this))
        return;
    pattern->accept(visitor);
    visitor->closeCaseItem(this);
}

bool CaseItem::_isCaseItem() { return (true); }

void Pattern::accept(Visitor* visitor) {
}

bool Pattern::_isPattern() { return (true); }

bool Pattern::_isWildcardPattern() { return (false); }
bool Pattern::_isIdentifierPattern() { return (false); }
bool Pattern::_isTuplePattern() { return (false); }
bool Pattern::_isExpressionPattern() { return (false); }

WildcardPattern::WildcardPattern(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void WildcardPattern::accept(Visitor* visitor) {
    visitor->visitWildcardPattern(this);
}

bool WildcardPattern::_isWildcardPattern() { return (true); }

IdentifierPattern::IdentifierPattern(string* identifier, TypeAnnotation* annotationForType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->identifier = identifier;
    this->annotationForType = annotationForType;
}

void IdentifierPattern::accept(Visitor* visitor) {
    if (!visitor->openIdentifierPattern(this))
        return;
    if (annotationForType != nullptr)
        annotationForType->accept(visitor);
    visitor->closeIdentifierPattern(this);
}

bool IdentifierPattern::_isIdentifierPattern() { return (true); }

TuplePattern::TuplePattern(_Array<TuplePatternElement>* elements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->elements = elements;
}

void TuplePattern::accept(Visitor* visitor) {
    if (!visitor->openTuplePattern(this))
        return;
    if (elements != nullptr) {
        TuplePatternElement* node = nullptr;
        size_t _elements_length = elements->length();
        for (size_t _i = 0; _i < _elements_length; _i++) {
            node = *(*elements)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeTuplePattern(this);
}

bool TuplePattern::_isTuplePattern() { return (true); }

ExpressionPattern::ExpressionPattern(Expression* expression, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expression = expression;
}

void ExpressionPattern::accept(Visitor* visitor) {
    if (!visitor->openExpressionPattern(this))
        return;
    expression->accept(visitor);
    visitor->closeExpressionPattern(this);
}

bool ExpressionPattern::_isExpressionPattern() { return (true); }

TuplePatternElement::TuplePatternElement(Pattern* pattern, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
}

void TuplePatternElement::accept(Visitor* visitor) {
    if (!visitor->openTuplePatternElement(this))
        return;
    pattern->accept(visitor);
    visitor->closeTuplePatternElement(this);
}

bool TuplePatternElement::_isTuplePatternElement() { return (true); }

CaseContent::CaseContent(_Array<Statement>* statements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->statements = statements;
}

void CaseContent::accept(Visitor* visitor) {
    if (!visitor->openCaseContent(this))
        return;
    if (statements != nullptr) {
        Statement* node = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            node = *(*statements)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeCaseContent(this);
}

bool CaseContent::_isCaseContent() { return (true); }

Type::Type(string* name, Subtype* subType, _Array<TypePostfix>* postfixes, LifeTime* lifeTime, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->subType = subType;
    this->postfixes = postfixes;
    this->lifeTime = lifeTime;
}

void Type::accept(Visitor* visitor) {
    if (!visitor->openType(this))
        return;
    if (subType != nullptr)
        subType->accept(visitor);
    if (postfixes != nullptr) {
        TypePostfix* node = nullptr;
        size_t _postfixes_length = postfixes->length();
        for (size_t _i = 0; _i < _postfixes_length; _i++) {
            node = *(*postfixes)[_i];
            node->accept(visitor);
        }
    }
    if (lifeTime != nullptr)
        lifeTime->accept(visitor);
    visitor->closeType(this);
}

bool Type::_isType() { return (true); }

TypeAnnotation::TypeAnnotation(Type* annotationForType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->annotationForType = annotationForType;
}

void TypeAnnotation::accept(Visitor* visitor) {
    if (!visitor->openTypeAnnotation(this))
        return;
    annotationForType->accept(visitor);
    visitor->closeTypeAnnotation(this);
}

bool TypeAnnotation::_isTypeAnnotation() { return (true); }

Subtype::Subtype(Type* type, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->type = type;
}

void Subtype::accept(Visitor* visitor) {
    if (!visitor->openSubtype(this))
        return;
    type->accept(visitor);
    visitor->closeSubtype(this);
}

bool Subtype::_isSubtype() { return (true); }

void TypePostfix::accept(Visitor* visitor) {
}

bool TypePostfix::_isTypePostfix() { return (true); }

bool TypePostfix::_isIndexedType() { return (false); }
bool TypePostfix::_isPointer() { return (false); }

IndexedType::IndexedType(Type* key, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->key = key;
}

void IndexedType::accept(Visitor* visitor) {
    if (!visitor->openIndexedType(this))
        return;
    if (key != nullptr)
        key->accept(visitor);
    visitor->closeIndexedType(this);
}

bool IndexedType::_isIndexedType() { return (true); }

Pointer::Pointer(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void Pointer::accept(Visitor* visitor) {
    visitor->visitPointer(this);
}

bool Pointer::_isPointer() { return (true); }

void LifeTime::accept(Visitor* visitor) {
}

bool LifeTime::_isLifeTime() { return (true); }

bool LifeTime::_isRoot() { return (false); }
bool LifeTime::_isLocal() { return (false); }
bool LifeTime::_isReference() { return (false); }
bool LifeTime::_isThrown() { return (false); }

Root::Root(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void Root::accept(Visitor* visitor) {
    visitor->visitRoot(this);
}

bool Root::_isRoot() { return (true); }

Local::Local(string* location, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->location = location;
}

void Local::accept(Visitor* visitor) {
    visitor->visitLocal(this);
}

bool Local::_isLocal() { return (true); }

Reference::Reference(Literal* age, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->age = age;
}

void Reference::accept(Visitor* visitor) {
    visitor->visitReference(this);
}

bool Reference::_isReference() { return (true); }

Thrown::Thrown(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void Thrown::accept(Visitor* visitor) {
    visitor->visitThrown(this);
}

bool Thrown::_isThrown() { return (true); }

TypeInheritanceClause::TypeInheritanceClause(_Array<Inheritance>* inheritances, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->inheritances = inheritances;
}

void TypeInheritanceClause::accept(Visitor* visitor) {
    if (!visitor->openTypeInheritanceClause(this))
        return;
    if (inheritances != nullptr) {
        Inheritance* node = nullptr;
        size_t _inheritances_length = inheritances->length();
        for (size_t _i = 0; _i < _inheritances_length; _i++) {
            node = *(*inheritances)[_i];
            node->accept(visitor);
        }
    }
    visitor->closeTypeInheritanceClause(this);
}

bool TypeInheritanceClause::_isTypeInheritanceClause() { return (true); }

Inheritance::Inheritance(Type* type, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->type = type;
}

void Inheritance::accept(Visitor* visitor) {
    if (!visitor->openInheritance(this))
        return;
    type->accept(visitor);
    visitor->closeInheritance(this);
}

bool Inheritance::_isInheritance() { return (true); }


}
