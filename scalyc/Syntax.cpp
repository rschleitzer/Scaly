#include "scalyc.h"
using namespace scaly;
namespace scalyc {

SyntaxNode::SyntaxNode(Position* start, Position* end)
: start(start), end(end) {
}

bool SyntaxNode::_isProgram() { return false; }
bool SyntaxNode::_isCompilationUnit() { return false; }
bool SyntaxNode::_isTerminatedStatement() { return false; }
bool SyntaxNode::_isStatement() { return false; }
bool SyntaxNode::_isDeclaration() { return false; }
bool SyntaxNode::_isExpression() { return false; }
bool SyntaxNode::_isUseDeclaration() { return false; }
bool SyntaxNode::_isConstantDeclaration() { return false; }
bool SyntaxNode::_isVariableDeclaration() { return false; }
bool SyntaxNode::_isMutableDeclaration() { return false; }
bool SyntaxNode::_isFunctionDeclaration() { return false; }
bool SyntaxNode::_isEnumDeclaration() { return false; }
bool SyntaxNode::_isClassDeclaration() { return false; }
bool SyntaxNode::_isInitializerDeclaration() { return false; }
bool SyntaxNode::_isCodeBlock() { return false; }
bool SyntaxNode::_isSimpleExpression() { return false; }
bool SyntaxNode::_isPathIdentifier() { return false; }
bool SyntaxNode::_isPathItem() { return false; }
bool SyntaxNode::_isInitializer() { return false; }
bool SyntaxNode::_isBindingInitializer() { return false; }
bool SyntaxNode::_isPatternInitializer() { return false; }
bool SyntaxNode::_isAdditionalInitializer() { return false; }
bool SyntaxNode::_isModifier() { return false; }
bool SyntaxNode::_isOverrideWord() { return false; }
bool SyntaxNode::_isStaticWord() { return false; }
bool SyntaxNode::_isFunctionName() { return false; }
bool SyntaxNode::_isIdentifierFunction() { return false; }
bool SyntaxNode::_isFunctionSignature() { return false; }
bool SyntaxNode::_isFunctionResult() { return false; }
bool SyntaxNode::_isExistingClause() { return false; }
bool SyntaxNode::_isParameterClause() { return false; }
bool SyntaxNode::_isParameter() { return false; }
bool SyntaxNode::_isConstParameter() { return false; }
bool SyntaxNode::_isVarParameter() { return false; }
bool SyntaxNode::_isThrowsClause() { return false; }
bool SyntaxNode::_isEnumMember() { return false; }
bool SyntaxNode::_isEnumCase() { return false; }
bool SyntaxNode::_isAdditionalCase() { return false; }
bool SyntaxNode::_isClassBody() { return false; }
bool SyntaxNode::_isGenericArgumentClause() { return false; }
bool SyntaxNode::_isGenericParameter() { return false; }
bool SyntaxNode::_isClassMember() { return false; }
bool SyntaxNode::_isPrefixExpression() { return false; }
bool SyntaxNode::_isPostfixExpression() { return false; }
bool SyntaxNode::_isBinaryOp() { return false; }
bool SyntaxNode::_isBinaryOperation() { return false; }
bool SyntaxNode::_isAssignment() { return false; }
bool SyntaxNode::_isTypeQuery() { return false; }
bool SyntaxNode::_isTypeCast() { return false; }
bool SyntaxNode::_isCatchClause() { return false; }
bool SyntaxNode::_isCatchPattern() { return false; }
bool SyntaxNode::_isWildCardCatchPattern() { return false; }
bool SyntaxNode::_isPathItemCatchPattern() { return false; }
bool SyntaxNode::_isPostfix() { return false; }
bool SyntaxNode::_isOperatorPostfix() { return false; }
bool SyntaxNode::_isFunctionCall() { return false; }
bool SyntaxNode::_isExplicitMemberExpression() { return false; }
bool SyntaxNode::_isSubscript() { return false; }
bool SyntaxNode::_isExpressionElement() { return false; }
bool SyntaxNode::_isMemberPostfix() { return false; }
bool SyntaxNode::_isNamedMemberPostfix() { return false; }
bool SyntaxNode::_isPrimaryExpression() { return false; }
bool SyntaxNode::_isIdentifierExpression() { return false; }
bool SyntaxNode::_isLiteralExpression() { return false; }
bool SyntaxNode::_isIfExpression() { return false; }
bool SyntaxNode::_isSwitchExpression() { return false; }
bool SyntaxNode::_isForExpression() { return false; }
bool SyntaxNode::_isWhileExpression() { return false; }
bool SyntaxNode::_isRepeatExpression() { return false; }
bool SyntaxNode::_isParenthesizedExpression() { return false; }
bool SyntaxNode::_isReturnExpression() { return false; }
bool SyntaxNode::_isThrowExpression() { return false; }
bool SyntaxNode::_isBreakExpression() { return false; }
bool SyntaxNode::_isInitializerCall() { return false; }
bool SyntaxNode::_isThisExpression() { return false; }
bool SyntaxNode::_isThisDot() { return false; }
bool SyntaxNode::_isThisSubscript() { return false; }
bool SyntaxNode::_isThisWord() { return false; }
bool SyntaxNode::_isSuperExpression() { return false; }
bool SyntaxNode::_isSuperDot() { return false; }
bool SyntaxNode::_isSuperSubscript() { return false; }
bool SyntaxNode::_isNullExpression() { return false; }
bool SyntaxNode::_isElseClause() { return false; }
bool SyntaxNode::_isSwitchBody() { return false; }
bool SyntaxNode::_isCurliedSwitchBody() { return false; }
bool SyntaxNode::_isNakedSwitchBody() { return false; }
bool SyntaxNode::_isSwitchCase() { return false; }
bool SyntaxNode::_isCaseLabel() { return false; }
bool SyntaxNode::_isItemCaseLabel() { return false; }
bool SyntaxNode::_isDefaultCaseLabel() { return false; }
bool SyntaxNode::_isCaseItem() { return false; }
bool SyntaxNode::_isPattern() { return false; }
bool SyntaxNode::_isWildcardPattern() { return false; }
bool SyntaxNode::_isIdentifierPattern() { return false; }
bool SyntaxNode::_isTuplePattern() { return false; }
bool SyntaxNode::_isExpressionPattern() { return false; }
bool SyntaxNode::_isTuplePatternElement() { return false; }
bool SyntaxNode::_isCaseContent() { return false; }
bool SyntaxNode::_isBlockCaseContent() { return false; }
bool SyntaxNode::_isEmptyCaseContent() { return false; }
bool SyntaxNode::_isCommonThisMember() { return false; }
bool SyntaxNode::_isThisInit() { return false; }
bool SyntaxNode::_isThisMember() { return false; }
bool SyntaxNode::_isCommonSuperMember() { return false; }
bool SyntaxNode::_isSuperInit() { return false; }
bool SyntaxNode::_isSuperMember() { return false; }
bool SyntaxNode::_isType() { return false; }
bool SyntaxNode::_isTypeIdentifier() { return false; }
bool SyntaxNode::_isArrayType() { return false; }
bool SyntaxNode::_isTypeAnnotation() { return false; }
bool SyntaxNode::_isSubtypeIdentifier() { return false; }
bool SyntaxNode::_isTypePostfix() { return false; }
bool SyntaxNode::_isOptionalType() { return false; }
bool SyntaxNode::_isTypeInheritanceClause() { return false; }
bool SyntaxNode::_isInheritance() { return false; }

Program::Program(String* name, String* directory, _Vector<CompilationUnit>* compilationUnits)
: SyntaxNode(new(getPage()) Position(0, 0), new(getPage()) Position(0, 0)), name(name), directory(directory), compilationUnits(compilationUnits) {}

bool Program::_isProgram() { return true; }

void Program::accept(SyntaxVisitor* visitor) {
    if (!visitor->openProgram(this))
        return;
    if (compilationUnits) {
        CompilationUnit* node = 0;
        size_t _alength = compilationUnits->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*compilationUnits)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeProgram(this);
}

CompilationUnit::CompilationUnit(_Vector<TerminatedStatement>* statements, Position* start, Position* end)
: SyntaxNode(start, end), statements(statements) {}

bool CompilationUnit::_isCompilationUnit() { return true; }

void CompilationUnit::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCompilationUnit(this))
        return;
    if (statements) {
        TerminatedStatement* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeCompilationUnit(this);
}

TerminatedStatement::TerminatedStatement(Statement* statement, Position* start, Position* end)
: SyntaxNode(start, end), statement(statement) {}

bool TerminatedStatement::_isTerminatedStatement() { return true; }

void TerminatedStatement::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTerminatedStatement(this))
        return;
    statement->accept(visitor);
    visitor->closeTerminatedStatement(this);
}

Statement::Statement(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool Statement::_isDeclaration() { return false; }
bool Statement::_isExpression() { return false; }
bool Statement::_isUseDeclaration() { return false; }
bool Statement::_isConstantDeclaration() { return false; }
bool Statement::_isVariableDeclaration() { return false; }
bool Statement::_isMutableDeclaration() { return false; }
bool Statement::_isFunctionDeclaration() { return false; }
bool Statement::_isEnumDeclaration() { return false; }
bool Statement::_isClassDeclaration() { return false; }
bool Statement::_isInitializerDeclaration() { return false; }
bool Statement::_isCodeBlock() { return false; }
bool Statement::_isSimpleExpression() { return false; }
bool Statement::_isStatement() { return true; }

void Statement::accept(SyntaxVisitor* visitor) {
}

Declaration::Declaration(Position* start, Position* end)
: Statement(start, end) {}

bool Declaration::_isUseDeclaration() { return false; }
bool Declaration::_isConstantDeclaration() { return false; }
bool Declaration::_isVariableDeclaration() { return false; }
bool Declaration::_isMutableDeclaration() { return false; }
bool Declaration::_isFunctionDeclaration() { return false; }
bool Declaration::_isEnumDeclaration() { return false; }
bool Declaration::_isClassDeclaration() { return false; }
bool Declaration::_isInitializerDeclaration() { return false; }
bool Declaration::_isDeclaration() { return true; }

void Declaration::accept(SyntaxVisitor* visitor) {
}

Expression::Expression(Position* start, Position* end)
: Statement(start, end) {}

bool Expression::_isCodeBlock() { return false; }
bool Expression::_isSimpleExpression() { return false; }
bool Expression::_isExpression() { return true; }

void Expression::accept(SyntaxVisitor* visitor) {
}

UseDeclaration::UseDeclaration(PathItem* importModule, _Vector<PathIdentifier>* importExtensions, Position* start, Position* end)
: Declaration(start, end), importModule(importModule), importExtensions(importExtensions) {}

bool UseDeclaration::_isUseDeclaration() { return true; }

void UseDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openUseDeclaration(this))
        return;
    importModule->accept(visitor);
    if (importExtensions) {
        PathIdentifier* node = 0;
        size_t _alength = importExtensions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*importExtensions)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeUseDeclaration(this);
}

ConstantDeclaration::ConstantDeclaration(BindingInitializer* initializer, Position* start, Position* end)
: Declaration(start, end), initializer(initializer) {}

bool ConstantDeclaration::_isConstantDeclaration() { return true; }

void ConstantDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openConstantDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeConstantDeclaration(this);
}

VariableDeclaration::VariableDeclaration(BindingInitializer* initializer, Position* start, Position* end)
: Declaration(start, end), initializer(initializer) {}

bool VariableDeclaration::_isVariableDeclaration() { return true; }

void VariableDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openVariableDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeVariableDeclaration(this);
}

MutableDeclaration::MutableDeclaration(BindingInitializer* initializer, Position* start, Position* end)
: Declaration(start, end), initializer(initializer) {}

bool MutableDeclaration::_isMutableDeclaration() { return true; }

void MutableDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openMutableDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeMutableDeclaration(this);
}

FunctionDeclaration::FunctionDeclaration(_Vector<Modifier>* modifiers, FunctionName* name, FunctionSignature* signature, Expression* body, Position* start, Position* end)
: Declaration(start, end), modifiers(modifiers), name(name), signature(signature), body(body) {}

bool FunctionDeclaration::_isFunctionDeclaration() { return true; }

void FunctionDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openFunctionDeclaration(this))
        return;
    if (modifiers) {
        Modifier* node = 0;
        size_t _alength = modifiers->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*modifiers)[_a];
            node->accept(visitor);
        }
    }
    name->accept(visitor);
    signature->accept(visitor);
    if (body)
        body->accept(visitor);
    visitor->closeFunctionDeclaration(this);
}

EnumDeclaration::EnumDeclaration(String* name, _Vector<EnumMember>* members, Position* start, Position* end)
: Declaration(start, end), name(name), members(members) {}

bool EnumDeclaration::_isEnumDeclaration() { return true; }

void EnumDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openEnumDeclaration(this))
        return;
    if (members) {
        EnumMember* node = 0;
        size_t _alength = members->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*members)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeEnumDeclaration(this);
}

ClassDeclaration::ClassDeclaration(String* name, GenericArgumentClause* genericArgumentClause, TypeInheritanceClause* typeInheritanceClause, ClassBody* body, Position* start, Position* end)
: Declaration(start, end), name(name), genericArgumentClause(genericArgumentClause), typeInheritanceClause(typeInheritanceClause), body(body) {}

bool ClassDeclaration::_isClassDeclaration() { return true; }

void ClassDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openClassDeclaration(this))
        return;
    if (genericArgumentClause)
        genericArgumentClause->accept(visitor);
    if (typeInheritanceClause)
        typeInheritanceClause->accept(visitor);
    if (body)
        body->accept(visitor);
    visitor->closeClassDeclaration(this);
}

InitializerDeclaration::InitializerDeclaration(_Vector<Modifier>* modifiers, ParameterClause* parameterClause, ThrowsClause* throwsClause, Expression* body, Position* start, Position* end)
: Declaration(start, end), modifiers(modifiers), parameterClause(parameterClause), throwsClause(throwsClause), body(body) {}

bool InitializerDeclaration::_isInitializerDeclaration() { return true; }

void InitializerDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openInitializerDeclaration(this))
        return;
    if (modifiers) {
        Modifier* node = 0;
        size_t _alength = modifiers->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*modifiers)[_a];
            node->accept(visitor);
        }
    }
    parameterClause->accept(visitor);
    if (throwsClause)
        throwsClause->accept(visitor);
    body->accept(visitor);
    visitor->closeInitializerDeclaration(this);
}

CodeBlock::CodeBlock(_Vector<TerminatedStatement>* statements, Position* start, Position* end)
: Expression(start, end), statements(statements) {}

bool CodeBlock::_isCodeBlock() { return true; }

void CodeBlock::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCodeBlock(this))
        return;
    if (statements) {
        TerminatedStatement* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeCodeBlock(this);
}

SimpleExpression::SimpleExpression(PrefixExpression* prefixExpression, _Vector<BinaryOp>* binaryOps, Position* start, Position* end)
: Expression(start, end), prefixExpression(prefixExpression), binaryOps(binaryOps) {}

bool SimpleExpression::_isSimpleExpression() { return true; }

void SimpleExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSimpleExpression(this))
        return;
    prefixExpression->accept(visitor);
    if (binaryOps) {
        BinaryOp* node = 0;
        size_t _alength = binaryOps->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*binaryOps)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeSimpleExpression(this);
}

PathIdentifier::PathIdentifier(PathItem* extension, Position* start, Position* end)
: SyntaxNode(start, end), extension(extension) {}

bool PathIdentifier::_isPathIdentifier() { return true; }

void PathIdentifier::accept(SyntaxVisitor* visitor) {
    if (!visitor->openPathIdentifier(this))
        return;
    extension->accept(visitor);
    visitor->closePathIdentifier(this);
}

PathItem::PathItem(String* name, Position* start, Position* end)
: SyntaxNode(start, end), name(name) {}

bool PathItem::_isPathItem() { return true; }

void PathItem::accept(SyntaxVisitor* visitor) {
    visitor->visitPathItem(this);
}

Initializer::Initializer(Expression* expression, Position* start, Position* end)
: SyntaxNode(start, end), expression(expression) {}

bool Initializer::_isInitializer() { return true; }

void Initializer::accept(SyntaxVisitor* visitor) {
    if (!visitor->openInitializer(this))
        return;
    expression->accept(visitor);
    visitor->closeInitializer(this);
}

BindingInitializer::BindingInitializer(PatternInitializer* initializer, _Vector<AdditionalInitializer>* additionalInitializers, Position* start, Position* end)
: SyntaxNode(start, end), initializer(initializer), additionalInitializers(additionalInitializers) {}

bool BindingInitializer::_isBindingInitializer() { return true; }

void BindingInitializer::accept(SyntaxVisitor* visitor) {
    if (!visitor->openBindingInitializer(this))
        return;
    initializer->accept(visitor);
    if (additionalInitializers) {
        AdditionalInitializer* node = 0;
        size_t _alength = additionalInitializers->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalInitializers)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeBindingInitializer(this);
}

PatternInitializer::PatternInitializer(Pattern* pattern, Initializer* initializer, Position* start, Position* end)
: SyntaxNode(start, end), pattern(pattern), initializer(initializer) {}

bool PatternInitializer::_isPatternInitializer() { return true; }

void PatternInitializer::accept(SyntaxVisitor* visitor) {
    if (!visitor->openPatternInitializer(this))
        return;
    pattern->accept(visitor);
    if (initializer)
        initializer->accept(visitor);
    visitor->closePatternInitializer(this);
}

AdditionalInitializer::AdditionalInitializer(PatternInitializer* pattern, Position* start, Position* end)
: SyntaxNode(start, end), pattern(pattern) {}

bool AdditionalInitializer::_isAdditionalInitializer() { return true; }

void AdditionalInitializer::accept(SyntaxVisitor* visitor) {
    if (!visitor->openAdditionalInitializer(this))
        return;
    pattern->accept(visitor);
    visitor->closeAdditionalInitializer(this);
}

Modifier::Modifier(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool Modifier::_isOverrideWord() { return false; }
bool Modifier::_isStaticWord() { return false; }
bool Modifier::_isModifier() { return true; }

void Modifier::accept(SyntaxVisitor* visitor) {
}

OverrideWord::OverrideWord(Position* start, Position* end)
: Modifier(start, end) {}

bool OverrideWord::_isOverrideWord() { return true; }

void OverrideWord::accept(SyntaxVisitor* visitor) {
    visitor->visitOverrideWord(this);
}

StaticWord::StaticWord(Position* start, Position* end)
: Modifier(start, end) {}

bool StaticWord::_isStaticWord() { return true; }

void StaticWord::accept(SyntaxVisitor* visitor) {
    visitor->visitStaticWord(this);
}

FunctionName::FunctionName(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool FunctionName::_isIdentifierFunction() { return false; }
bool FunctionName::_isFunctionName() { return true; }

void FunctionName::accept(SyntaxVisitor* visitor) {
}

IdentifierFunction::IdentifierFunction(String* name, Position* start, Position* end)
: FunctionName(start, end), name(name) {}

bool IdentifierFunction::_isIdentifierFunction() { return true; }

void IdentifierFunction::accept(SyntaxVisitor* visitor) {
    visitor->visitIdentifierFunction(this);
}

FunctionSignature::FunctionSignature(ParameterClause* parameterClause, FunctionResult* result, ThrowsClause* throwsClause, Position* start, Position* end)
: SyntaxNode(start, end), parameterClause(parameterClause), result(result), throwsClause(throwsClause) {}

bool FunctionSignature::_isFunctionSignature() { return true; }

void FunctionSignature::accept(SyntaxVisitor* visitor) {
    if (!visitor->openFunctionSignature(this))
        return;
    parameterClause->accept(visitor);
    if (result)
        result->accept(visitor);
    if (throwsClause)
        throwsClause->accept(visitor);
    visitor->closeFunctionSignature(this);
}

FunctionResult::FunctionResult(ExistingClause* existingObject, Type* resultType, Position* start, Position* end)
: SyntaxNode(start, end), existingObject(existingObject), resultType(resultType) {}

bool FunctionResult::_isFunctionResult() { return true; }

void FunctionResult::accept(SyntaxVisitor* visitor) {
    if (!visitor->openFunctionResult(this))
        return;
    if (existingObject)
        existingObject->accept(visitor);
    resultType->accept(visitor);
    visitor->closeFunctionResult(this);
}

ExistingClause::ExistingClause(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool ExistingClause::_isExistingClause() { return true; }

void ExistingClause::accept(SyntaxVisitor* visitor) {
    visitor->visitExistingClause(this);
}

ParameterClause::ParameterClause(_Vector<Parameter>* parameters, Position* start, Position* end)
: SyntaxNode(start, end), parameters(parameters) {}

bool ParameterClause::_isParameterClause() { return true; }

void ParameterClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openParameterClause(this))
        return;
    if (parameters) {
        Parameter* node = 0;
        size_t _alength = parameters->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*parameters)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeParameterClause(this);
}

Parameter::Parameter(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool Parameter::_isConstParameter() { return false; }
bool Parameter::_isVarParameter() { return false; }
bool Parameter::_isParameter() { return true; }

void Parameter::accept(SyntaxVisitor* visitor) {
}

ConstParameter::ConstParameter(String* name, Type* parameterType, Position* start, Position* end)
: Parameter(start, end), name(name), parameterType(parameterType) {}

bool ConstParameter::_isConstParameter() { return true; }

void ConstParameter::accept(SyntaxVisitor* visitor) {
    if (!visitor->openConstParameter(this))
        return;
    parameterType->accept(visitor);
    visitor->closeConstParameter(this);
}

VarParameter::VarParameter(String* name, Type* parameterType, Position* start, Position* end)
: Parameter(start, end), name(name), parameterType(parameterType) {}

bool VarParameter::_isVarParameter() { return true; }

void VarParameter::accept(SyntaxVisitor* visitor) {
    if (!visitor->openVarParameter(this))
        return;
    parameterType->accept(visitor);
    visitor->closeVarParameter(this);
}

ThrowsClause::ThrowsClause(Type* throwsType, Position* start, Position* end)
: SyntaxNode(start, end), throwsType(throwsType) {}

bool ThrowsClause::_isThrowsClause() { return true; }

void ThrowsClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openThrowsClause(this))
        return;
    throwsType->accept(visitor);
    visitor->closeThrowsClause(this);
}

EnumMember::EnumMember(EnumCase* enumCase, _Vector<AdditionalCase>* additionalCases, ParameterClause* parameterClause, Position* start, Position* end)
: SyntaxNode(start, end), enumCase(enumCase), additionalCases(additionalCases), parameterClause(parameterClause) {}

bool EnumMember::_isEnumMember() { return true; }

void EnumMember::accept(SyntaxVisitor* visitor) {
    if (!visitor->openEnumMember(this))
        return;
    enumCase->accept(visitor);
    if (additionalCases) {
        AdditionalCase* node = 0;
        size_t _alength = additionalCases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalCases)[_a];
            node->accept(visitor);
        }
    }
    if (parameterClause)
        parameterClause->accept(visitor);
    visitor->closeEnumMember(this);
}

EnumCase::EnumCase(String* name, Position* start, Position* end)
: SyntaxNode(start, end), name(name) {}

bool EnumCase::_isEnumCase() { return true; }

void EnumCase::accept(SyntaxVisitor* visitor) {
    visitor->visitEnumCase(this);
}

AdditionalCase::AdditionalCase(EnumCase* enumCase, Position* start, Position* end)
: SyntaxNode(start, end), enumCase(enumCase) {}

bool AdditionalCase::_isAdditionalCase() { return true; }

void AdditionalCase::accept(SyntaxVisitor* visitor) {
    if (!visitor->openAdditionalCase(this))
        return;
    enumCase->accept(visitor);
    visitor->closeAdditionalCase(this);
}

ClassBody::ClassBody(_Vector<ClassMember>* members, Position* start, Position* end)
: SyntaxNode(start, end), members(members) {}

bool ClassBody::_isClassBody() { return true; }

void ClassBody::accept(SyntaxVisitor* visitor) {
    if (!visitor->openClassBody(this))
        return;
    if (members) {
        ClassMember* node = 0;
        size_t _alength = members->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*members)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeClassBody(this);
}

GenericArgumentClause::GenericArgumentClause(_Vector<GenericParameter>* genericParameters, Position* start, Position* end)
: SyntaxNode(start, end), genericParameters(genericParameters) {}

bool GenericArgumentClause::_isGenericArgumentClause() { return true; }

void GenericArgumentClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openGenericArgumentClause(this))
        return;
    if (genericParameters) {
        GenericParameter* node = 0;
        size_t _alength = genericParameters->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*genericParameters)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeGenericArgumentClause(this);
}

GenericParameter::GenericParameter(String* typeName, Position* start, Position* end)
: SyntaxNode(start, end), typeName(typeName) {}

bool GenericParameter::_isGenericParameter() { return true; }

void GenericParameter::accept(SyntaxVisitor* visitor) {
    visitor->visitGenericParameter(this);
}

ClassMember::ClassMember(Declaration* declaration, Position* start, Position* end)
: SyntaxNode(start, end), declaration(declaration) {}

bool ClassMember::_isClassMember() { return true; }

void ClassMember::accept(SyntaxVisitor* visitor) {
    if (!visitor->openClassMember(this))
        return;
    declaration->accept(visitor);
    visitor->closeClassMember(this);
}

PrefixExpression::PrefixExpression(String* prefixOperator, PostfixExpression* expression, Position* start, Position* end)
: SyntaxNode(start, end), prefixOperator(prefixOperator), expression(expression) {}

bool PrefixExpression::_isPrefixExpression() { return true; }

void PrefixExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openPrefixExpression(this))
        return;
    expression->accept(visitor);
    visitor->closePrefixExpression(this);
}

PostfixExpression::PostfixExpression(PrimaryExpression* primaryExpression, _Vector<Postfix>* postfixes, Position* start, Position* end)
: SyntaxNode(start, end), primaryExpression(primaryExpression), postfixes(postfixes) {}

bool PostfixExpression::_isPostfixExpression() { return true; }

void PostfixExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openPostfixExpression(this))
        return;
    primaryExpression->accept(visitor);
    if (postfixes) {
        Postfix* node = 0;
        size_t _alength = postfixes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*postfixes)[_a];
            node->accept(visitor);
        }
    }
    visitor->closePostfixExpression(this);
}

BinaryOp::BinaryOp(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool BinaryOp::_isBinaryOperation() { return false; }
bool BinaryOp::_isAssignment() { return false; }
bool BinaryOp::_isTypeQuery() { return false; }
bool BinaryOp::_isTypeCast() { return false; }
bool BinaryOp::_isBinaryOp() { return true; }

void BinaryOp::accept(SyntaxVisitor* visitor) {
}

BinaryOperation::BinaryOperation(String* binaryOperator, PrefixExpression* expression, Position* start, Position* end)
: BinaryOp(start, end), binaryOperator(binaryOperator), expression(expression) {}

bool BinaryOperation::_isBinaryOperation() { return true; }

void BinaryOperation::accept(SyntaxVisitor* visitor) {
    if (!visitor->openBinaryOperation(this))
        return;
    expression->accept(visitor);
    visitor->closeBinaryOperation(this);
}

Assignment::Assignment(PrefixExpression* expression, Position* start, Position* end)
: BinaryOp(start, end), expression(expression) {}

bool Assignment::_isAssignment() { return true; }

void Assignment::accept(SyntaxVisitor* visitor) {
    if (!visitor->openAssignment(this))
        return;
    expression->accept(visitor);
    visitor->closeAssignment(this);
}

TypeQuery::TypeQuery(Type* objectType, Position* start, Position* end)
: BinaryOp(start, end), objectType(objectType) {}

bool TypeQuery::_isTypeQuery() { return true; }

void TypeQuery::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTypeQuery(this))
        return;
    objectType->accept(visitor);
    visitor->closeTypeQuery(this);
}

TypeCast::TypeCast(Type* objectType, Position* start, Position* end)
: BinaryOp(start, end), objectType(objectType) {}

bool TypeCast::_isTypeCast() { return true; }

void TypeCast::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTypeCast(this))
        return;
    objectType->accept(visitor);
    visitor->closeTypeCast(this);
}

CatchClause::CatchClause(CatchPattern* catchPattern, TuplePattern* bindingPattern, Expression* expression, Position* start, Position* end)
: SyntaxNode(start, end), catchPattern(catchPattern), bindingPattern(bindingPattern), expression(expression) {}

bool CatchClause::_isCatchClause() { return true; }

void CatchClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCatchClause(this))
        return;
    catchPattern->accept(visitor);
    if (bindingPattern)
        bindingPattern->accept(visitor);
    expression->accept(visitor);
    visitor->closeCatchClause(this);
}

CatchPattern::CatchPattern(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool CatchPattern::_isWildCardCatchPattern() { return false; }
bool CatchPattern::_isPathItemCatchPattern() { return false; }
bool CatchPattern::_isCatchPattern() { return true; }

void CatchPattern::accept(SyntaxVisitor* visitor) {
}

WildCardCatchPattern::WildCardCatchPattern(WildcardPattern* pattern, Position* start, Position* end)
: CatchPattern(start, end), pattern(pattern) {}

bool WildCardCatchPattern::_isWildCardCatchPattern() { return true; }

void WildCardCatchPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openWildCardCatchPattern(this))
        return;
    pattern->accept(visitor);
    visitor->closeWildCardCatchPattern(this);
}

PathItemCatchPattern::PathItemCatchPattern(PathItem* catchCase, _Vector<PathIdentifier>* catchCaseExtensions, Position* start, Position* end)
: CatchPattern(start, end), catchCase(catchCase), catchCaseExtensions(catchCaseExtensions) {}

bool PathItemCatchPattern::_isPathItemCatchPattern() { return true; }

void PathItemCatchPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openPathItemCatchPattern(this))
        return;
    catchCase->accept(visitor);
    if (catchCaseExtensions) {
        PathIdentifier* node = 0;
        size_t _alength = catchCaseExtensions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*catchCaseExtensions)[_a];
            node->accept(visitor);
        }
    }
    visitor->closePathItemCatchPattern(this);
}

Postfix::Postfix(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool Postfix::_isOperatorPostfix() { return false; }
bool Postfix::_isFunctionCall() { return false; }
bool Postfix::_isExplicitMemberExpression() { return false; }
bool Postfix::_isSubscript() { return false; }
bool Postfix::_isPostfix() { return true; }

void Postfix::accept(SyntaxVisitor* visitor) {
}

OperatorPostfix::OperatorPostfix(String* postfixOperator, Position* start, Position* end)
: Postfix(start, end), postfixOperator(postfixOperator) {}

bool OperatorPostfix::_isOperatorPostfix() { return true; }

void OperatorPostfix::accept(SyntaxVisitor* visitor) {
    visitor->visitOperatorPostfix(this);
}

FunctionCall::FunctionCall(ParenthesizedExpression* arguments, _Vector<CatchClause>* catchClauses, Position* start, Position* end)
: Postfix(start, end), arguments(arguments), catchClauses(catchClauses) {}

bool FunctionCall::_isFunctionCall() { return true; }

void FunctionCall::accept(SyntaxVisitor* visitor) {
    if (!visitor->openFunctionCall(this))
        return;
    arguments->accept(visitor);
    if (catchClauses) {
        CatchClause* node = 0;
        size_t _alength = catchClauses->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*catchClauses)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeFunctionCall(this);
}

ExplicitMemberExpression::ExplicitMemberExpression(MemberPostfix* memberPostfix, Position* start, Position* end)
: Postfix(start, end), memberPostfix(memberPostfix) {}

bool ExplicitMemberExpression::_isExplicitMemberExpression() { return true; }

void ExplicitMemberExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openExplicitMemberExpression(this))
        return;
    memberPostfix->accept(visitor);
    visitor->closeExplicitMemberExpression(this);
}

Subscript::Subscript(_Vector<ExpressionElement>* expressions, Position* start, Position* end)
: Postfix(start, end), expressions(expressions) {}

bool Subscript::_isSubscript() { return true; }

void Subscript::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSubscript(this))
        return;
    if (expressions) {
        ExpressionElement* node = 0;
        size_t _alength = expressions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*expressions)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeSubscript(this);
}

ExpressionElement::ExpressionElement(Expression* expression, Position* start, Position* end)
: SyntaxNode(start, end), expression(expression) {}

bool ExpressionElement::_isExpressionElement() { return true; }

void ExpressionElement::accept(SyntaxVisitor* visitor) {
    if (!visitor->openExpressionElement(this))
        return;
    expression->accept(visitor);
    visitor->closeExpressionElement(this);
}

MemberPostfix::MemberPostfix(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool MemberPostfix::_isNamedMemberPostfix() { return false; }
bool MemberPostfix::_isMemberPostfix() { return true; }

void MemberPostfix::accept(SyntaxVisitor* visitor) {
}

NamedMemberPostfix::NamedMemberPostfix(IdentifierExpression* identifier, Position* start, Position* end)
: MemberPostfix(start, end), identifier(identifier) {}

bool NamedMemberPostfix::_isNamedMemberPostfix() { return true; }

void NamedMemberPostfix::accept(SyntaxVisitor* visitor) {
    if (!visitor->openNamedMemberPostfix(this))
        return;
    identifier->accept(visitor);
    visitor->closeNamedMemberPostfix(this);
}

PrimaryExpression::PrimaryExpression(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool PrimaryExpression::_isIdentifierExpression() { return false; }
bool PrimaryExpression::_isLiteralExpression() { return false; }
bool PrimaryExpression::_isIfExpression() { return false; }
bool PrimaryExpression::_isSwitchExpression() { return false; }
bool PrimaryExpression::_isForExpression() { return false; }
bool PrimaryExpression::_isWhileExpression() { return false; }
bool PrimaryExpression::_isRepeatExpression() { return false; }
bool PrimaryExpression::_isParenthesizedExpression() { return false; }
bool PrimaryExpression::_isReturnExpression() { return false; }
bool PrimaryExpression::_isThrowExpression() { return false; }
bool PrimaryExpression::_isBreakExpression() { return false; }
bool PrimaryExpression::_isInitializerCall() { return false; }
bool PrimaryExpression::_isThisExpression() { return false; }
bool PrimaryExpression::_isSuperExpression() { return false; }
bool PrimaryExpression::_isNullExpression() { return false; }
bool PrimaryExpression::_isThisDot() { return false; }
bool PrimaryExpression::_isThisSubscript() { return false; }
bool PrimaryExpression::_isThisWord() { return false; }
bool PrimaryExpression::_isSuperDot() { return false; }
bool PrimaryExpression::_isSuperSubscript() { return false; }
bool PrimaryExpression::_isPrimaryExpression() { return true; }

void PrimaryExpression::accept(SyntaxVisitor* visitor) {
}

IdentifierExpression::IdentifierExpression(String* name, Position* start, Position* end)
: PrimaryExpression(start, end), name(name) {}

bool IdentifierExpression::_isIdentifierExpression() { return true; }

void IdentifierExpression::accept(SyntaxVisitor* visitor) {
    visitor->visitIdentifierExpression(this);
}

LiteralExpression::LiteralExpression(Literal* literal, Position* start, Position* end)
: PrimaryExpression(start, end), literal(literal) {}

bool LiteralExpression::_isLiteralExpression() { return true; }

void LiteralExpression::accept(SyntaxVisitor* visitor) {
    visitor->visitLiteralExpression(this);
}

IfExpression::IfExpression(Expression* condition, Expression* consequent, ElseClause* elseClause, Position* start, Position* end)
: PrimaryExpression(start, end), condition(condition), consequent(consequent), elseClause(elseClause) {}

bool IfExpression::_isIfExpression() { return true; }

void IfExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openIfExpression(this))
        return;
    condition->accept(visitor);
    consequent->accept(visitor);
    if (elseClause)
        elseClause->accept(visitor);
    visitor->closeIfExpression(this);
}

SwitchExpression::SwitchExpression(Expression* expression, SwitchBody* body, Position* start, Position* end)
: PrimaryExpression(start, end), expression(expression), body(body) {}

bool SwitchExpression::_isSwitchExpression() { return true; }

void SwitchExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSwitchExpression(this))
        return;
    expression->accept(visitor);
    body->accept(visitor);
    visitor->closeSwitchExpression(this);
}

ForExpression::ForExpression(Pattern* pattern, Expression* expression, Expression* code, Position* start, Position* end)
: PrimaryExpression(start, end), pattern(pattern), expression(expression), code(code) {}

bool ForExpression::_isForExpression() { return true; }

void ForExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openForExpression(this))
        return;
    pattern->accept(visitor);
    expression->accept(visitor);
    code->accept(visitor);
    visitor->closeForExpression(this);
}

WhileExpression::WhileExpression(Expression* condition, Expression* code, Position* start, Position* end)
: PrimaryExpression(start, end), condition(condition), code(code) {}

bool WhileExpression::_isWhileExpression() { return true; }

void WhileExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openWhileExpression(this))
        return;
    condition->accept(visitor);
    code->accept(visitor);
    visitor->closeWhileExpression(this);
}

RepeatExpression::RepeatExpression(Expression* code, Expression* condition, Position* start, Position* end)
: PrimaryExpression(start, end), code(code), condition(condition) {}

bool RepeatExpression::_isRepeatExpression() { return true; }

void RepeatExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openRepeatExpression(this))
        return;
    code->accept(visitor);
    condition->accept(visitor);
    visitor->closeRepeatExpression(this);
}

ParenthesizedExpression::ParenthesizedExpression(_Vector<ExpressionElement>* expressionElements, Position* start, Position* end)
: PrimaryExpression(start, end), expressionElements(expressionElements) {}

bool ParenthesizedExpression::_isParenthesizedExpression() { return true; }

void ParenthesizedExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openParenthesizedExpression(this))
        return;
    if (expressionElements) {
        ExpressionElement* node = 0;
        size_t _alength = expressionElements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*expressionElements)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeParenthesizedExpression(this);
}

ReturnExpression::ReturnExpression(Expression* expression, Position* start, Position* end)
: PrimaryExpression(start, end), expression(expression) {}

bool ReturnExpression::_isReturnExpression() { return true; }

void ReturnExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openReturnExpression(this))
        return;
    if (expression)
        expression->accept(visitor);
    visitor->closeReturnExpression(this);
}

ThrowExpression::ThrowExpression(Expression* expression, Position* start, Position* end)
: PrimaryExpression(start, end), expression(expression) {}

bool ThrowExpression::_isThrowExpression() { return true; }

void ThrowExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openThrowExpression(this))
        return;
    if (expression)
        expression->accept(visitor);
    visitor->closeThrowExpression(this);
}

BreakExpression::BreakExpression(Expression* expression, Position* start, Position* end)
: PrimaryExpression(start, end), expression(expression) {}

bool BreakExpression::_isBreakExpression() { return true; }

void BreakExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openBreakExpression(this))
        return;
    if (expression)
        expression->accept(visitor);
    visitor->closeBreakExpression(this);
}

InitializerCall::InitializerCall(Type* typeToInitialize, ParenthesizedExpression* arguments, _Vector<CatchClause>* catchClauses, Position* start, Position* end)
: PrimaryExpression(start, end), typeToInitialize(typeToInitialize), arguments(arguments), catchClauses(catchClauses) {}

bool InitializerCall::_isInitializerCall() { return true; }

void InitializerCall::accept(SyntaxVisitor* visitor) {
    if (!visitor->openInitializerCall(this))
        return;
    typeToInitialize->accept(visitor);
    arguments->accept(visitor);
    if (catchClauses) {
        CatchClause* node = 0;
        size_t _alength = catchClauses->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*catchClauses)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeInitializerCall(this);
}

ThisExpression::ThisExpression(Position* start, Position* end)
: PrimaryExpression(start, end) {}

bool ThisExpression::_isThisDot() { return false; }
bool ThisExpression::_isThisSubscript() { return false; }
bool ThisExpression::_isThisWord() { return false; }
bool ThisExpression::_isThisExpression() { return true; }

void ThisExpression::accept(SyntaxVisitor* visitor) {
}

ThisDot::ThisDot(CommonThisMember* member, Position* start, Position* end)
: ThisExpression(start, end), member(member) {}

bool ThisDot::_isThisDot() { return true; }

void ThisDot::accept(SyntaxVisitor* visitor) {
    if (!visitor->openThisDot(this))
        return;
    member->accept(visitor);
    visitor->closeThisDot(this);
}

ThisSubscript::ThisSubscript(Subscript* subscript, Position* start, Position* end)
: ThisExpression(start, end), subscript(subscript) {}

bool ThisSubscript::_isThisSubscript() { return true; }

void ThisSubscript::accept(SyntaxVisitor* visitor) {
    if (!visitor->openThisSubscript(this))
        return;
    subscript->accept(visitor);
    visitor->closeThisSubscript(this);
}

ThisWord::ThisWord(Position* start, Position* end)
: ThisExpression(start, end) {}

bool ThisWord::_isThisWord() { return true; }

void ThisWord::accept(SyntaxVisitor* visitor) {
    visitor->visitThisWord(this);
}

SuperExpression::SuperExpression(Position* start, Position* end)
: PrimaryExpression(start, end) {}

bool SuperExpression::_isSuperDot() { return false; }
bool SuperExpression::_isSuperSubscript() { return false; }
bool SuperExpression::_isSuperExpression() { return true; }

void SuperExpression::accept(SyntaxVisitor* visitor) {
}

SuperDot::SuperDot(CommonSuperMember* member, Position* start, Position* end)
: SuperExpression(start, end), member(member) {}

bool SuperDot::_isSuperDot() { return true; }

void SuperDot::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSuperDot(this))
        return;
    member->accept(visitor);
    visitor->closeSuperDot(this);
}

SuperSubscript::SuperSubscript(Subscript* subscript, Position* start, Position* end)
: SuperExpression(start, end), subscript(subscript) {}

bool SuperSubscript::_isSuperSubscript() { return true; }

void SuperSubscript::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSuperSubscript(this))
        return;
    subscript->accept(visitor);
    visitor->closeSuperSubscript(this);
}

NullExpression::NullExpression(Position* start, Position* end)
: PrimaryExpression(start, end) {}

bool NullExpression::_isNullExpression() { return true; }

void NullExpression::accept(SyntaxVisitor* visitor) {
    visitor->visitNullExpression(this);
}

ElseClause::ElseClause(Expression* alternative, Position* start, Position* end)
: SyntaxNode(start, end), alternative(alternative) {}

bool ElseClause::_isElseClause() { return true; }

void ElseClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openElseClause(this))
        return;
    alternative->accept(visitor);
    visitor->closeElseClause(this);
}

SwitchBody::SwitchBody(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool SwitchBody::_isCurliedSwitchBody() { return false; }
bool SwitchBody::_isNakedSwitchBody() { return false; }
bool SwitchBody::_isSwitchBody() { return true; }

void SwitchBody::accept(SyntaxVisitor* visitor) {
}

CurliedSwitchBody::CurliedSwitchBody(_Vector<SwitchCase>* cases, Position* start, Position* end)
: SwitchBody(start, end), cases(cases) {}

bool CurliedSwitchBody::_isCurliedSwitchBody() { return true; }

void CurliedSwitchBody::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCurliedSwitchBody(this))
        return;
    if (cases) {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeCurliedSwitchBody(this);
}

NakedSwitchBody::NakedSwitchBody(_Vector<SwitchCase>* cases, Position* start, Position* end)
: SwitchBody(start, end), cases(cases) {}

bool NakedSwitchBody::_isNakedSwitchBody() { return true; }

void NakedSwitchBody::accept(SyntaxVisitor* visitor) {
    if (!visitor->openNakedSwitchBody(this))
        return;
    if (cases) {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeNakedSwitchBody(this);
}

SwitchCase::SwitchCase(CaseLabel* label, CaseContent* content, Position* start, Position* end)
: SyntaxNode(start, end), label(label), content(content) {}

bool SwitchCase::_isSwitchCase() { return true; }

void SwitchCase::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSwitchCase(this))
        return;
    label->accept(visitor);
    content->accept(visitor);
    visitor->closeSwitchCase(this);
}

CaseLabel::CaseLabel(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool CaseLabel::_isItemCaseLabel() { return false; }
bool CaseLabel::_isDefaultCaseLabel() { return false; }
bool CaseLabel::_isCaseLabel() { return true; }

void CaseLabel::accept(SyntaxVisitor* visitor) {
}

ItemCaseLabel::ItemCaseLabel(Pattern* pattern, _Vector<CaseItem>* additionalPatterns, Position* start, Position* end)
: CaseLabel(start, end), pattern(pattern), additionalPatterns(additionalPatterns) {}

bool ItemCaseLabel::_isItemCaseLabel() { return true; }

void ItemCaseLabel::accept(SyntaxVisitor* visitor) {
    if (!visitor->openItemCaseLabel(this))
        return;
    pattern->accept(visitor);
    if (additionalPatterns) {
        CaseItem* node = 0;
        size_t _alength = additionalPatterns->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalPatterns)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeItemCaseLabel(this);
}

DefaultCaseLabel::DefaultCaseLabel(Position* start, Position* end)
: CaseLabel(start, end) {}

bool DefaultCaseLabel::_isDefaultCaseLabel() { return true; }

void DefaultCaseLabel::accept(SyntaxVisitor* visitor) {
    visitor->visitDefaultCaseLabel(this);
}

CaseItem::CaseItem(Pattern* pattern, Position* start, Position* end)
: SyntaxNode(start, end), pattern(pattern) {}

bool CaseItem::_isCaseItem() { return true; }

void CaseItem::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCaseItem(this))
        return;
    pattern->accept(visitor);
    visitor->closeCaseItem(this);
}

Pattern::Pattern(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool Pattern::_isWildcardPattern() { return false; }
bool Pattern::_isIdentifierPattern() { return false; }
bool Pattern::_isTuplePattern() { return false; }
bool Pattern::_isExpressionPattern() { return false; }
bool Pattern::_isPattern() { return true; }

void Pattern::accept(SyntaxVisitor* visitor) {
}

WildcardPattern::WildcardPattern(Position* start, Position* end)
: Pattern(start, end) {}

bool WildcardPattern::_isWildcardPattern() { return true; }

void WildcardPattern::accept(SyntaxVisitor* visitor) {
    visitor->visitWildcardPattern(this);
}

IdentifierPattern::IdentifierPattern(String* identifier, TypeAnnotation* annotationForType, Position* start, Position* end)
: Pattern(start, end), identifier(identifier), annotationForType(annotationForType) {}

bool IdentifierPattern::_isIdentifierPattern() { return true; }

void IdentifierPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openIdentifierPattern(this))
        return;
    if (annotationForType)
        annotationForType->accept(visitor);
    visitor->closeIdentifierPattern(this);
}

TuplePattern::TuplePattern(_Vector<TuplePatternElement>* elements, Position* start, Position* end)
: Pattern(start, end), elements(elements) {}

bool TuplePattern::_isTuplePattern() { return true; }

void TuplePattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTuplePattern(this))
        return;
    if (elements) {
        TuplePatternElement* node = 0;
        size_t _alength = elements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*elements)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeTuplePattern(this);
}

ExpressionPattern::ExpressionPattern(Expression* expression, Position* start, Position* end)
: Pattern(start, end), expression(expression) {}

bool ExpressionPattern::_isExpressionPattern() { return true; }

void ExpressionPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openExpressionPattern(this))
        return;
    expression->accept(visitor);
    visitor->closeExpressionPattern(this);
}

TuplePatternElement::TuplePatternElement(Pattern* pattern, Position* start, Position* end)
: SyntaxNode(start, end), pattern(pattern) {}

bool TuplePatternElement::_isTuplePatternElement() { return true; }

void TuplePatternElement::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTuplePatternElement(this))
        return;
    pattern->accept(visitor);
    visitor->closeTuplePatternElement(this);
}

CaseContent::CaseContent(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool CaseContent::_isBlockCaseContent() { return false; }
bool CaseContent::_isEmptyCaseContent() { return false; }
bool CaseContent::_isCaseContent() { return true; }

void CaseContent::accept(SyntaxVisitor* visitor) {
}

BlockCaseContent::BlockCaseContent(_Vector<TerminatedStatement>* statements, Position* start, Position* end)
: CaseContent(start, end), statements(statements) {}

bool BlockCaseContent::_isBlockCaseContent() { return true; }

void BlockCaseContent::accept(SyntaxVisitor* visitor) {
    if (!visitor->openBlockCaseContent(this))
        return;
    if (statements) {
        TerminatedStatement* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeBlockCaseContent(this);
}

EmptyCaseContent::EmptyCaseContent(Position* start, Position* end)
: CaseContent(start, end) {}

bool EmptyCaseContent::_isEmptyCaseContent() { return true; }

void EmptyCaseContent::accept(SyntaxVisitor* visitor) {
    visitor->visitEmptyCaseContent(this);
}

CommonThisMember::CommonThisMember(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool CommonThisMember::_isThisInit() { return false; }
bool CommonThisMember::_isThisMember() { return false; }
bool CommonThisMember::_isCommonThisMember() { return true; }

void CommonThisMember::accept(SyntaxVisitor* visitor) {
}

ThisInit::ThisInit(Position* start, Position* end)
: CommonThisMember(start, end) {}

bool ThisInit::_isThisInit() { return true; }

void ThisInit::accept(SyntaxVisitor* visitor) {
    visitor->visitThisInit(this);
}

ThisMember::ThisMember(String* name, Position* start, Position* end)
: CommonThisMember(start, end), name(name) {}

bool ThisMember::_isThisMember() { return true; }

void ThisMember::accept(SyntaxVisitor* visitor) {
    visitor->visitThisMember(this);
}

CommonSuperMember::CommonSuperMember(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool CommonSuperMember::_isSuperInit() { return false; }
bool CommonSuperMember::_isSuperMember() { return false; }
bool CommonSuperMember::_isCommonSuperMember() { return true; }

void CommonSuperMember::accept(SyntaxVisitor* visitor) {
}

SuperInit::SuperInit(Position* start, Position* end)
: CommonSuperMember(start, end) {}

bool SuperInit::_isSuperInit() { return true; }

void SuperInit::accept(SyntaxVisitor* visitor) {
    visitor->visitSuperInit(this);
}

SuperMember::SuperMember(String* name, Position* start, Position* end)
: CommonSuperMember(start, end), name(name) {}

bool SuperMember::_isSuperMember() { return true; }

void SuperMember::accept(SyntaxVisitor* visitor) {
    visitor->visitSuperMember(this);
}

Type::Type(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool Type::_isArrayType() { return false; }
bool Type::_isTypeIdentifier() { return false; }
bool Type::_isType() { return true; }

void Type::accept(SyntaxVisitor* visitor) {
}

TypeIdentifier::TypeIdentifier(String* name, SubtypeIdentifier* subType, _Vector<TypePostfix>* postfixes, Position* start, Position* end)
: Type(start, end), name(name), subType(subType), postfixes(postfixes) {}

bool TypeIdentifier::_isTypeIdentifier() { return true; }

void TypeIdentifier::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTypeIdentifier(this))
        return;
    if (subType)
        subType->accept(visitor);
    if (postfixes) {
        TypePostfix* node = 0;
        size_t _alength = postfixes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*postfixes)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeTypeIdentifier(this);
}

ArrayType::ArrayType(Type* elementType, _Vector<TypePostfix>* postfixes, Position* start, Position* end)
: Type(start, end), elementType(elementType), postfixes(postfixes) {}

bool ArrayType::_isArrayType() { return true; }

void ArrayType::accept(SyntaxVisitor* visitor) {
    if (!visitor->openArrayType(this))
        return;
    elementType->accept(visitor);
    if (postfixes) {
        TypePostfix* node = 0;
        size_t _alength = postfixes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*postfixes)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeArrayType(this);
}

TypeAnnotation::TypeAnnotation(Type* annotationForType, Position* start, Position* end)
: SyntaxNode(start, end), annotationForType(annotationForType) {}

bool TypeAnnotation::_isTypeAnnotation() { return true; }

void TypeAnnotation::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTypeAnnotation(this))
        return;
    annotationForType->accept(visitor);
    visitor->closeTypeAnnotation(this);
}

SubtypeIdentifier::SubtypeIdentifier(TypeIdentifier* typeIdentifier, Position* start, Position* end)
: SyntaxNode(start, end), typeIdentifier(typeIdentifier) {}

bool SubtypeIdentifier::_isSubtypeIdentifier() { return true; }

void SubtypeIdentifier::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSubtypeIdentifier(this))
        return;
    typeIdentifier->accept(visitor);
    visitor->closeSubtypeIdentifier(this);
}

TypePostfix::TypePostfix(Position* start, Position* end)
: SyntaxNode(start, end) {}

bool TypePostfix::_isOptionalType() { return false; }
bool TypePostfix::_isTypePostfix() { return true; }

void TypePostfix::accept(SyntaxVisitor* visitor) {
}

OptionalType::OptionalType(Position* start, Position* end)
: TypePostfix(start, end) {}

bool OptionalType::_isOptionalType() { return true; }

void OptionalType::accept(SyntaxVisitor* visitor) {
    visitor->visitOptionalType(this);
}

TypeInheritanceClause::TypeInheritanceClause(_Vector<Inheritance>* inheritances, Position* start, Position* end)
: SyntaxNode(start, end), inheritances(inheritances) {}

bool TypeInheritanceClause::_isTypeInheritanceClause() { return true; }

void TypeInheritanceClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTypeInheritanceClause(this))
        return;
    if (inheritances) {
        Inheritance* node = 0;
        size_t _alength = inheritances->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*inheritances)[_a];
            node->accept(visitor);
        }
    }
    visitor->closeTypeInheritanceClause(this);
}

Inheritance::Inheritance(TypeIdentifier* typeIdentifier, Position* start, Position* end)
: SyntaxNode(start, end), typeIdentifier(typeIdentifier) {}

bool Inheritance::_isInheritance() { return true; }

void Inheritance::accept(SyntaxVisitor* visitor) {
    if (!visitor->openInheritance(this))
        return;
    typeIdentifier->accept(visitor);
    visitor->closeInheritance(this);
}

}
