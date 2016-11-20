#include "scalyc.h"
using namespace scaly;
namespace scalyc {

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
bool SyntaxNode::_isFunctionName() { return (false); }
bool SyntaxNode::_isIdentifierFunction() { return (false); }
bool SyntaxNode::_isFunctionSignature() { return (false); }
bool SyntaxNode::_isFunctionResult() { return (false); }
bool SyntaxNode::_isExistingClause() { return (false); }
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
bool SyntaxNode::_isExplicitMemberExpression() { return (false); }
bool SyntaxNode::_isSubscript() { return (false); }
bool SyntaxNode::_isExpressionElement() { return (false); }
bool SyntaxNode::_isMemberPostfix() { return (false); }
bool SyntaxNode::_isNamedMemberPostfix() { return (false); }
bool SyntaxNode::_isPrimaryExpression() { return (false); }
bool SyntaxNode::_isIdentifierExpression() { return (false); }
bool SyntaxNode::_isLiteralExpression() { return (false); }
bool SyntaxNode::_isIfExpression() { return (false); }
bool SyntaxNode::_isSwitchExpression() { return (false); }
bool SyntaxNode::_isForExpression() { return (false); }
bool SyntaxNode::_isWhileExpression() { return (false); }
bool SyntaxNode::_isRepeatExpression() { return (false); }
bool SyntaxNode::_isParenthesizedExpression() { return (false); }
bool SyntaxNode::_isReturnExpression() { return (false); }
bool SyntaxNode::_isThrowExpression() { return (false); }
bool SyntaxNode::_isBreakExpression() { return (false); }
bool SyntaxNode::_isConstructorCall() { return (false); }
bool SyntaxNode::_isThisExpression() { return (false); }
bool SyntaxNode::_isSuperExpression() { return (false); }
bool SyntaxNode::_isSuperDot() { return (false); }
bool SyntaxNode::_isSuperSubscript() { return (false); }
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
bool SyntaxNode::_isCommonSuperMember() { return (false); }
bool SyntaxNode::_isSuperConstructor() { return (false); }
bool SyntaxNode::_isSuperMember() { return (false); }
bool SyntaxNode::_isType() { return (false); }
bool SyntaxNode::_isTypeAnnotation() { return (false); }
bool SyntaxNode::_isSubtype() { return (false); }
bool SyntaxNode::_isTypePostfix() { return (false); }
bool SyntaxNode::_isOptionalType() { return (false); }
bool SyntaxNode::_isIndexedType() { return (false); }
bool SyntaxNode::_isTypeInheritanceClause() { return (false); }
bool SyntaxNode::_isInheritance() { return (false); }

Program::Program(string* name, string* directory, _Vector<CompilationUnit>* compilationUnits) {
    start = new(getPage()) Position(0, 0);
    end = new(getPage()) Position(0, 0);
    this->name = name;
    this->directory = directory;
    this->compilationUnits = compilationUnits;
    this->parent = nullptr;
}

void Program::accept(SyntaxVisitor* visitor) {
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

CompilationUnit::CompilationUnit(_Vector<Statement>* statements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->statements = statements;
}

void CompilationUnit::accept(SyntaxVisitor* visitor) {
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

void Statement::accept(SyntaxVisitor* visitor) {
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

void Declaration::accept(SyntaxVisitor* visitor) {
}

bool Declaration::_isDeclaration() { return (true); }

bool Declaration::_isConstantDeclaration() { return (false); }
bool Declaration::_isVariableDeclaration() { return (false); }
bool Declaration::_isMutableDeclaration() { return (false); }
bool Declaration::_isFunctionDeclaration() { return (false); }
bool Declaration::_isEnumDeclaration() { return (false); }
bool Declaration::_isClassDeclaration() { return (false); }
bool Declaration::_isConstructorDeclaration() { return (false); }

void Expression::accept(SyntaxVisitor* visitor) {
}

bool Expression::_isExpression() { return (true); }

bool Expression::_isCodeBlock() { return (false); }
bool Expression::_isSimpleExpression() { return (false); }

ConstantDeclaration::ConstantDeclaration(BindingInitializer* initializer, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->initializer = initializer;
}

void ConstantDeclaration::accept(SyntaxVisitor* visitor) {
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

void VariableDeclaration::accept(SyntaxVisitor* visitor) {
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

void MutableDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openMutableDeclaration(this))
        return;
    initializer->accept(visitor);
    visitor->closeMutableDeclaration(this);
}

bool MutableDeclaration::_isMutableDeclaration() { return (true); }

FunctionDeclaration::FunctionDeclaration(_Vector<Modifier>* modifiers, FunctionName* name, FunctionSignature* signature, Expression* body, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->modifiers = modifiers;
    this->name = name;
    this->signature = signature;
    this->body = body;
}

void FunctionDeclaration::accept(SyntaxVisitor* visitor) {
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
    name->accept(visitor);
    signature->accept(visitor);
    if (body != nullptr)
        body->accept(visitor);
    visitor->closeFunctionDeclaration(this);
}

bool FunctionDeclaration::_isFunctionDeclaration() { return (true); }

EnumDeclaration::EnumDeclaration(string* name, _Vector<EnumMember>* members, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->members = members;
}

void EnumDeclaration::accept(SyntaxVisitor* visitor) {
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

void ClassDeclaration::accept(SyntaxVisitor* visitor) {
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

void ConstructorDeclaration::accept(SyntaxVisitor* visitor) {
    if (!visitor->openConstructorDeclaration(this))
        return;
    parameterClause->accept(visitor);
    body->accept(visitor);
    visitor->closeConstructorDeclaration(this);
}

bool ConstructorDeclaration::_isConstructorDeclaration() { return (true); }

CodeBlock::CodeBlock(_Vector<Statement>* statements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->statements = statements;
}

void CodeBlock::accept(SyntaxVisitor* visitor) {
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

SimpleExpression::SimpleExpression(PrefixExpression* prefixExpression, _Vector<BinaryOp>* binaryOps, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->prefixExpression = prefixExpression;
    this->binaryOps = binaryOps;
}

void SimpleExpression::accept(SyntaxVisitor* visitor) {
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

void Initializer::accept(SyntaxVisitor* visitor) {
    if (!visitor->openInitializer(this))
        return;
    expression->accept(visitor);
    visitor->closeInitializer(this);
}

bool Initializer::_isInitializer() { return (true); }

BindingInitializer::BindingInitializer(PatternInitializer* initializer, _Vector<AdditionalInitializer>* additionalInitializers, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->initializer = initializer;
    this->additionalInitializers = additionalInitializers;
}

void BindingInitializer::accept(SyntaxVisitor* visitor) {
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

void PatternInitializer::accept(SyntaxVisitor* visitor) {
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

void AdditionalInitializer::accept(SyntaxVisitor* visitor) {
    if (!visitor->openAdditionalInitializer(this))
        return;
    pattern->accept(visitor);
    visitor->closeAdditionalInitializer(this);
}

bool AdditionalInitializer::_isAdditionalInitializer() { return (true); }

void Modifier::accept(SyntaxVisitor* visitor) {
}

bool Modifier::_isModifier() { return (true); }

bool Modifier::_isOverrideWord() { return (false); }
bool Modifier::_isStaticWord() { return (false); }

OverrideWord::OverrideWord(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void OverrideWord::accept(SyntaxVisitor* visitor) {
    visitor->visitOverrideWord(this);
}

bool OverrideWord::_isOverrideWord() { return (true); }

StaticWord::StaticWord(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void StaticWord::accept(SyntaxVisitor* visitor) {
    visitor->visitStaticWord(this);
}

bool StaticWord::_isStaticWord() { return (true); }

void FunctionName::accept(SyntaxVisitor* visitor) {
}

bool FunctionName::_isFunctionName() { return (true); }

bool FunctionName::_isIdentifierFunction() { return (false); }

IdentifierFunction::IdentifierFunction(string* name, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
}

void IdentifierFunction::accept(SyntaxVisitor* visitor) {
    visitor->visitIdentifierFunction(this);
}

bool IdentifierFunction::_isIdentifierFunction() { return (true); }

FunctionSignature::FunctionSignature(ParameterClause* parameterClause, FunctionResult* result, ThrowsClause* throwsClause, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->parameterClause = parameterClause;
    this->result = result;
    this->throwsClause = throwsClause;
}

void FunctionSignature::accept(SyntaxVisitor* visitor) {
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

FunctionResult::FunctionResult(ExistingClause* existingObject, Type* resultType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->existingObject = existingObject;
    this->resultType = resultType;
}

void FunctionResult::accept(SyntaxVisitor* visitor) {
    if (!visitor->openFunctionResult(this))
        return;
    if (existingObject != nullptr)
        existingObject->accept(visitor);
    resultType->accept(visitor);
    visitor->closeFunctionResult(this);
}

bool FunctionResult::_isFunctionResult() { return (true); }

ExistingClause::ExistingClause(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void ExistingClause::accept(SyntaxVisitor* visitor) {
    visitor->visitExistingClause(this);
}

bool ExistingClause::_isExistingClause() { return (true); }

ParameterClause::ParameterClause(_Vector<Parameter>* parameters, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->parameters = parameters;
}

void ParameterClause::accept(SyntaxVisitor* visitor) {
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

void Parameter::accept(SyntaxVisitor* visitor) {
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

void ConstParameter::accept(SyntaxVisitor* visitor) {
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

void VarParameter::accept(SyntaxVisitor* visitor) {
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

void ThrowsClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openThrowsClause(this))
        return;
    throwsType->accept(visitor);
    visitor->closeThrowsClause(this);
}

bool ThrowsClause::_isThrowsClause() { return (true); }

EnumMember::EnumMember(EnumCase* enumCase, _Vector<AdditionalCase>* additionalCases, ParameterClause* parameterClause, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->enumCase = enumCase;
    this->additionalCases = additionalCases;
    this->parameterClause = parameterClause;
}

void EnumMember::accept(SyntaxVisitor* visitor) {
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

void EnumCase::accept(SyntaxVisitor* visitor) {
    visitor->visitEnumCase(this);
}

bool EnumCase::_isEnumCase() { return (true); }

AdditionalCase::AdditionalCase(EnumCase* enumCase, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->enumCase = enumCase;
}

void AdditionalCase::accept(SyntaxVisitor* visitor) {
    if (!visitor->openAdditionalCase(this))
        return;
    enumCase->accept(visitor);
    visitor->closeAdditionalCase(this);
}

bool AdditionalCase::_isAdditionalCase() { return (true); }

ClassBody::ClassBody(_Vector<ClassMember>* members, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->members = members;
}

void ClassBody::accept(SyntaxVisitor* visitor) {
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

void ClassMember::accept(SyntaxVisitor* visitor) {
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

void PrefixExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openPrefixExpression(this))
        return;
    expression->accept(visitor);
    visitor->closePrefixExpression(this);
}

bool PrefixExpression::_isPrefixExpression() { return (true); }

PostfixExpression::PostfixExpression(PrimaryExpression* primaryExpression, _Vector<Postfix>* postfixes, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->primaryExpression = primaryExpression;
    this->postfixes = postfixes;
}

void PostfixExpression::accept(SyntaxVisitor* visitor) {
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

void BinaryOp::accept(SyntaxVisitor* visitor) {
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

void BinaryOperation::accept(SyntaxVisitor* visitor) {
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

void Assignment::accept(SyntaxVisitor* visitor) {
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

void TypeQuery::accept(SyntaxVisitor* visitor) {
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

void TypeCast::accept(SyntaxVisitor* visitor) {
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

void CatchClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCatchClause(this))
        return;
    catchPattern->accept(visitor);
    if (bindingPattern != nullptr)
        bindingPattern->accept(visitor);
    expression->accept(visitor);
    visitor->closeCatchClause(this);
}

bool CatchClause::_isCatchClause() { return (true); }

void CatchPattern::accept(SyntaxVisitor* visitor) {
}

bool CatchPattern::_isCatchPattern() { return (true); }

bool CatchPattern::_isWildCardCatchPattern() { return (false); }
bool CatchPattern::_isIdentifierCatchPattern() { return (false); }

WildCardCatchPattern::WildCardCatchPattern(WildcardPattern* pattern, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
}

void WildCardCatchPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openWildCardCatchPattern(this))
        return;
    pattern->accept(visitor);
    visitor->closeWildCardCatchPattern(this);
}

bool WildCardCatchPattern::_isWildCardCatchPattern() { return (true); }

IdentifierCatchPattern::IdentifierCatchPattern(string* name, ExplicitMemberExpression* member, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->member = member;
}

void IdentifierCatchPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openIdentifierCatchPattern(this))
        return;
    if (member != nullptr)
        member->accept(visitor);
    visitor->closeIdentifierCatchPattern(this);
}

bool IdentifierCatchPattern::_isIdentifierCatchPattern() { return (true); }

void Postfix::accept(SyntaxVisitor* visitor) {
}

bool Postfix::_isPostfix() { return (true); }

bool Postfix::_isOperatorPostfix() { return (false); }
bool Postfix::_isFunctionCall() { return (false); }
bool Postfix::_isExplicitMemberExpression() { return (false); }
bool Postfix::_isSubscript() { return (false); }

OperatorPostfix::OperatorPostfix(string* postfixOperator, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->postfixOperator = postfixOperator;
}

void OperatorPostfix::accept(SyntaxVisitor* visitor) {
    visitor->visitOperatorPostfix(this);
}

bool OperatorPostfix::_isOperatorPostfix() { return (true); }

FunctionCall::FunctionCall(ParenthesizedExpression* arguments, _Vector<CatchClause>* catchClauses, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->arguments = arguments;
    this->catchClauses = catchClauses;
}

void FunctionCall::accept(SyntaxVisitor* visitor) {
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

ExplicitMemberExpression::ExplicitMemberExpression(MemberPostfix* memberPostfix, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->memberPostfix = memberPostfix;
}

void ExplicitMemberExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openExplicitMemberExpression(this))
        return;
    memberPostfix->accept(visitor);
    visitor->closeExplicitMemberExpression(this);
}

bool ExplicitMemberExpression::_isExplicitMemberExpression() { return (true); }

Subscript::Subscript(_Vector<ExpressionElement>* expressions, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expressions = expressions;
}

void Subscript::accept(SyntaxVisitor* visitor) {
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

void ExpressionElement::accept(SyntaxVisitor* visitor) {
    if (!visitor->openExpressionElement(this))
        return;
    expression->accept(visitor);
    visitor->closeExpressionElement(this);
}

bool ExpressionElement::_isExpressionElement() { return (true); }

void MemberPostfix::accept(SyntaxVisitor* visitor) {
}

bool MemberPostfix::_isMemberPostfix() { return (true); }

bool MemberPostfix::_isNamedMemberPostfix() { return (false); }

NamedMemberPostfix::NamedMemberPostfix(IdentifierExpression* identifier, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->identifier = identifier;
}

void NamedMemberPostfix::accept(SyntaxVisitor* visitor) {
    if (!visitor->openNamedMemberPostfix(this))
        return;
    identifier->accept(visitor);
    visitor->closeNamedMemberPostfix(this);
}

bool NamedMemberPostfix::_isNamedMemberPostfix() { return (true); }

void PrimaryExpression::accept(SyntaxVisitor* visitor) {
}

bool PrimaryExpression::_isPrimaryExpression() { return (true); }

bool PrimaryExpression::_isIdentifierExpression() { return (false); }
bool PrimaryExpression::_isLiteralExpression() { return (false); }
bool PrimaryExpression::_isIfExpression() { return (false); }
bool PrimaryExpression::_isSwitchExpression() { return (false); }
bool PrimaryExpression::_isForExpression() { return (false); }
bool PrimaryExpression::_isWhileExpression() { return (false); }
bool PrimaryExpression::_isRepeatExpression() { return (false); }
bool PrimaryExpression::_isParenthesizedExpression() { return (false); }
bool PrimaryExpression::_isReturnExpression() { return (false); }
bool PrimaryExpression::_isThrowExpression() { return (false); }
bool PrimaryExpression::_isBreakExpression() { return (false); }
bool PrimaryExpression::_isConstructorCall() { return (false); }
bool PrimaryExpression::_isThisExpression() { return (false); }
bool PrimaryExpression::_isSuperExpression() { return (false); }
bool PrimaryExpression::_isSuperDot() { return (false); }
bool PrimaryExpression::_isSuperSubscript() { return (false); }
bool PrimaryExpression::_isNullExpression() { return (false); }

IdentifierExpression::IdentifierExpression(string* name, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
}

void IdentifierExpression::accept(SyntaxVisitor* visitor) {
    visitor->visitIdentifierExpression(this);
}

bool IdentifierExpression::_isIdentifierExpression() { return (true); }

LiteralExpression::LiteralExpression(Literal* literal, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->literal = literal;
}

void LiteralExpression::accept(SyntaxVisitor* visitor) {
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

void IfExpression::accept(SyntaxVisitor* visitor) {
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

void SwitchExpression::accept(SyntaxVisitor* visitor) {
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

void ForExpression::accept(SyntaxVisitor* visitor) {
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

void WhileExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openWhileExpression(this))
        return;
    condition->accept(visitor);
    code->accept(visitor);
    visitor->closeWhileExpression(this);
}

bool WhileExpression::_isWhileExpression() { return (true); }

RepeatExpression::RepeatExpression(Expression* code, Expression* condition, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->code = code;
    this->condition = condition;
}

void RepeatExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openRepeatExpression(this))
        return;
    code->accept(visitor);
    condition->accept(visitor);
    visitor->closeRepeatExpression(this);
}

bool RepeatExpression::_isRepeatExpression() { return (true); }

ParenthesizedExpression::ParenthesizedExpression(_Vector<ExpressionElement>* expressionElements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->expressionElements = expressionElements;
}

void ParenthesizedExpression::accept(SyntaxVisitor* visitor) {
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

void ReturnExpression::accept(SyntaxVisitor* visitor) {
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

void ThrowExpression::accept(SyntaxVisitor* visitor) {
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

void BreakExpression::accept(SyntaxVisitor* visitor) {
    if (!visitor->openBreakExpression(this))
        return;
    if (expression != nullptr)
        expression->accept(visitor);
    visitor->closeBreakExpression(this);
}

bool BreakExpression::_isBreakExpression() { return (true); }

ConstructorCall::ConstructorCall(Type* typeToInitialize, ParenthesizedExpression* arguments, _Vector<CatchClause>* catchClauses, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->typeToInitialize = typeToInitialize;
    this->arguments = arguments;
    this->catchClauses = catchClauses;
}

void ConstructorCall::accept(SyntaxVisitor* visitor) {
    if (!visitor->openConstructorCall(this))
        return;
    typeToInitialize->accept(visitor);
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

void ThisExpression::accept(SyntaxVisitor* visitor) {
    visitor->visitThisExpression(this);
}

bool ThisExpression::_isThisExpression() { return (true); }

void SuperExpression::accept(SyntaxVisitor* visitor) {
}

bool SuperExpression::_isSuperExpression() { return (true); }

bool SuperExpression::_isSuperDot() { return (false); }
bool SuperExpression::_isSuperSubscript() { return (false); }

SuperDot::SuperDot(CommonSuperMember* member, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->member = member;
}

void SuperDot::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSuperDot(this))
        return;
    member->accept(visitor);
    visitor->closeSuperDot(this);
}

bool SuperDot::_isSuperDot() { return (true); }

SuperSubscript::SuperSubscript(Subscript* subscript, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->subscript = subscript;
}

void SuperSubscript::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSuperSubscript(this))
        return;
    subscript->accept(visitor);
    visitor->closeSuperSubscript(this);
}

bool SuperSubscript::_isSuperSubscript() { return (true); }

NullExpression::NullExpression(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void NullExpression::accept(SyntaxVisitor* visitor) {
    visitor->visitNullExpression(this);
}

bool NullExpression::_isNullExpression() { return (true); }

ElseClause::ElseClause(Expression* alternative, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->alternative = alternative;
}

void ElseClause::accept(SyntaxVisitor* visitor) {
    if (!visitor->openElseClause(this))
        return;
    alternative->accept(visitor);
    visitor->closeElseClause(this);
}

bool ElseClause::_isElseClause() { return (true); }

void SwitchBody::accept(SyntaxVisitor* visitor) {
}

bool SwitchBody::_isSwitchBody() { return (true); }

bool SwitchBody::_isCurliedSwitchBody() { return (false); }
bool SwitchBody::_isNakedSwitchBody() { return (false); }

CurliedSwitchBody::CurliedSwitchBody(_Vector<SwitchCase>* cases, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->cases = cases;
}

void CurliedSwitchBody::accept(SyntaxVisitor* visitor) {
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

NakedSwitchBody::NakedSwitchBody(_Vector<SwitchCase>* cases, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->cases = cases;
}

void NakedSwitchBody::accept(SyntaxVisitor* visitor) {
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

void SwitchCase::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSwitchCase(this))
        return;
    label->accept(visitor);
    content->accept(visitor);
    visitor->closeSwitchCase(this);
}

bool SwitchCase::_isSwitchCase() { return (true); }

void CaseLabel::accept(SyntaxVisitor* visitor) {
}

bool CaseLabel::_isCaseLabel() { return (true); }

bool CaseLabel::_isItemCaseLabel() { return (false); }
bool CaseLabel::_isDefaultCaseLabel() { return (false); }

ItemCaseLabel::ItemCaseLabel(Pattern* pattern, _Vector<CaseItem>* additionalPatterns, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
    this->additionalPatterns = additionalPatterns;
}

void ItemCaseLabel::accept(SyntaxVisitor* visitor) {
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

void DefaultCaseLabel::accept(SyntaxVisitor* visitor) {
    visitor->visitDefaultCaseLabel(this);
}

bool DefaultCaseLabel::_isDefaultCaseLabel() { return (true); }

CaseItem::CaseItem(Pattern* pattern, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->pattern = pattern;
}

void CaseItem::accept(SyntaxVisitor* visitor) {
    if (!visitor->openCaseItem(this))
        return;
    pattern->accept(visitor);
    visitor->closeCaseItem(this);
}

bool CaseItem::_isCaseItem() { return (true); }

void Pattern::accept(SyntaxVisitor* visitor) {
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

void WildcardPattern::accept(SyntaxVisitor* visitor) {
    visitor->visitWildcardPattern(this);
}

bool WildcardPattern::_isWildcardPattern() { return (true); }

IdentifierPattern::IdentifierPattern(string* identifier, TypeAnnotation* annotationForType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->identifier = identifier;
    this->annotationForType = annotationForType;
}

void IdentifierPattern::accept(SyntaxVisitor* visitor) {
    if (!visitor->openIdentifierPattern(this))
        return;
    if (annotationForType != nullptr)
        annotationForType->accept(visitor);
    visitor->closeIdentifierPattern(this);
}

bool IdentifierPattern::_isIdentifierPattern() { return (true); }

TuplePattern::TuplePattern(_Vector<TuplePatternElement>* elements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->elements = elements;
}

void TuplePattern::accept(SyntaxVisitor* visitor) {
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

void ExpressionPattern::accept(SyntaxVisitor* visitor) {
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

void TuplePatternElement::accept(SyntaxVisitor* visitor) {
    if (!visitor->openTuplePatternElement(this))
        return;
    pattern->accept(visitor);
    visitor->closeTuplePatternElement(this);
}

bool TuplePatternElement::_isTuplePatternElement() { return (true); }

CaseContent::CaseContent(_Vector<Statement>* statements, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->statements = statements;
}

void CaseContent::accept(SyntaxVisitor* visitor) {
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

void CommonSuperMember::accept(SyntaxVisitor* visitor) {
}

bool CommonSuperMember::_isCommonSuperMember() { return (true); }

bool CommonSuperMember::_isSuperConstructor() { return (false); }
bool CommonSuperMember::_isSuperMember() { return (false); }

SuperConstructor::SuperConstructor(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void SuperConstructor::accept(SyntaxVisitor* visitor) {
    visitor->visitSuperConstructor(this);
}

bool SuperConstructor::_isSuperConstructor() { return (true); }

SuperMember::SuperMember(string* name, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
}

void SuperMember::accept(SyntaxVisitor* visitor) {
    visitor->visitSuperMember(this);
}

bool SuperMember::_isSuperMember() { return (true); }

Type::Type(string* name, Subtype* subType, _Vector<TypePostfix>* postfixes, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->name = name;
    this->subType = subType;
    this->postfixes = postfixes;
}

void Type::accept(SyntaxVisitor* visitor) {
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
    visitor->closeType(this);
}

bool Type::_isType() { return (true); }

TypeAnnotation::TypeAnnotation(Type* annotationForType, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->annotationForType = annotationForType;
}

void TypeAnnotation::accept(SyntaxVisitor* visitor) {
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

void Subtype::accept(SyntaxVisitor* visitor) {
    if (!visitor->openSubtype(this))
        return;
    type->accept(visitor);
    visitor->closeSubtype(this);
}

bool Subtype::_isSubtype() { return (true); }

void TypePostfix::accept(SyntaxVisitor* visitor) {
}

bool TypePostfix::_isTypePostfix() { return (true); }

bool TypePostfix::_isOptionalType() { return (false); }
bool TypePostfix::_isIndexedType() { return (false); }

OptionalType::OptionalType(Position* start, Position* end) {
    this->start = start;
    this->end = end;
}

void OptionalType::accept(SyntaxVisitor* visitor) {
    visitor->visitOptionalType(this);
}

bool OptionalType::_isOptionalType() { return (true); }

IndexedType::IndexedType(Type* key, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->key = key;
}

void IndexedType::accept(SyntaxVisitor* visitor) {
    if (!visitor->openIndexedType(this))
        return;
    if (key != nullptr)
        key->accept(visitor);
    visitor->closeIndexedType(this);
}

bool IndexedType::_isIndexedType() { return (true); }

TypeInheritanceClause::TypeInheritanceClause(_Vector<Inheritance>* inheritances, Position* start, Position* end) {
    this->start = start;
    this->end = end;
    this->inheritances = inheritances;
}

void TypeInheritanceClause::accept(SyntaxVisitor* visitor) {
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

void Inheritance::accept(SyntaxVisitor* visitor) {
    if (!visitor->openInheritance(this))
        return;
    type->accept(visitor);
    visitor->closeInheritance(this);
}

bool Inheritance::_isInheritance() { return (true); }


}
