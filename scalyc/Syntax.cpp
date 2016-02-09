#include "scalyc.h"
using namespace scaly;
namespace scalyc {

SyntaxNode::SyntaxNode(Position start, Position end)
: start(start), end(end) {
}

Program::Program()
: SyntaxNode(Position(0, 0), Position(0, 0)) {
}

void Program::accept(SyntaxVisitor& visitor) {
    if (!visitor.openProgram(*this))
        return;
    {
        CompilationUnit* node = 0;
        size_t _alength = compilationUnits->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*compilationUnits)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeProgram(*this);
}

CompilationUnit::CompilationUnit(Position start, Position end)
: SyntaxNode(start, end) {
}

void CompilationUnit::accept(SyntaxVisitor& visitor) {
    if (!visitor.openCompilationUnit(*this))
        return;
    if (statements)
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeCompilationUnit(*this);
}

StatementWithSemicolon::StatementWithSemicolon(Position start, Position end)
: SyntaxNode(start, end) {
}

void StatementWithSemicolon::accept(SyntaxVisitor& visitor) {
    if (!visitor.openStatementWithSemicolon(*this))
        return;
    statement->accept(visitor);
    visitor.closeStatementWithSemicolon(*this);
}

Statement::Statement(Position start, Position end)
: SyntaxNode(start, end) {
}

Declaration::Declaration(Position start, Position end)
: Statement(start, end) {
}

UseDeclaration::UseDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void UseDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openUseDeclaration(*this))
        return;
    importModule->accept(visitor);
    if (importExtensions)
    {
        PathIdentifier* node = 0;
        size_t _alength = importExtensions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*importExtensions)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeUseDeclaration(*this);
}

PathIdentifier::PathIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void PathIdentifier::accept(SyntaxVisitor& visitor) {
    if (!visitor.openPathIdentifier(*this))
        return;
    extension->accept(visitor);
    visitor.closePathIdentifier(*this);
}

PathItem::PathItem(Position start, Position end)
: SyntaxNode(start, end) {
}

void PathItem::accept(SyntaxVisitor& visitor) {
    visitor.visitPathItem(*this);
}

Initializer::Initializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void Initializer::accept(SyntaxVisitor& visitor) {
    if (!visitor.openInitializer(*this))
        return;
    expression->accept(visitor);
    visitor.closeInitializer(*this);
}

ConstantDeclaration::ConstantDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void ConstantDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openConstantDeclaration(*this))
        return;
    initializer->accept(visitor);
    visitor.closeConstantDeclaration(*this);
}

VariableDeclaration::VariableDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void VariableDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openVariableDeclaration(*this))
        return;
    initializer->accept(visitor);
    visitor.closeVariableDeclaration(*this);
}

BindingInitializer::BindingInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void BindingInitializer::accept(SyntaxVisitor& visitor) {
    if (!visitor.openBindingInitializer(*this))
        return;
    initializer->accept(visitor);
    if (additionalInitializers)
    {
        AdditionalInitializer* node = 0;
        size_t _alength = additionalInitializers->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalInitializers)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeBindingInitializer(*this);
}

PatternInitializer::PatternInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void PatternInitializer::accept(SyntaxVisitor& visitor) {
    if (!visitor.openPatternInitializer(*this))
        return;
    pattern->accept(visitor);
    if (initializer)
        initializer->accept(visitor);
    visitor.closePatternInitializer(*this);
}

AdditionalInitializer::AdditionalInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalInitializer::accept(SyntaxVisitor& visitor) {
    if (!visitor.openAdditionalInitializer(*this))
        return;
    pattern->accept(visitor);
    visitor.closeAdditionalInitializer(*this);
}

FunctionDeclaration::FunctionDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void FunctionDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openFunctionDeclaration(*this))
        return;
    if (modifiers)
    {
        Modifier* node = 0;
        size_t _alength = modifiers->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*modifiers)[_a];
            node->accept(visitor);
        }
    }
    name->accept(visitor);
    signature->accept(visitor);
    body->accept(visitor);
    visitor.closeFunctionDeclaration(*this);
}

InitializerDeclaration::InitializerDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void InitializerDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openInitializerDeclaration(*this))
        return;
    if (modifiers)
    {
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
    visitor.closeInitializerDeclaration(*this);
}

Modifier::Modifier(Position start, Position end)
: SyntaxNode(start, end) {
}

Override::Override(Position start, Position end)
: Modifier(start, end) {
}

void Override::accept(SyntaxVisitor& visitor) {
    visitor.visitOverride(*this);
}

StaticWord::StaticWord(Position start, Position end)
: Modifier(start, end) {
}

void StaticWord::accept(SyntaxVisitor& visitor) {
    visitor.visitStaticWord(*this);
}

FunctionName::FunctionName(Position start, Position end)
: SyntaxNode(start, end) {
}

IdentifierFunction::IdentifierFunction(Position start, Position end)
: FunctionName(start, end) {
}

void IdentifierFunction::accept(SyntaxVisitor& visitor) {
    visitor.visitIdentifierFunction(*this);
}

FunctionSignature::FunctionSignature(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionSignature::accept(SyntaxVisitor& visitor) {
    if (!visitor.openFunctionSignature(*this))
        return;
    parameterClause->accept(visitor);
    if (result)
        result->accept(visitor);
    if (throwsClause)
        throwsClause->accept(visitor);
    visitor.closeFunctionSignature(*this);
}

FunctionResult::FunctionResult(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionResult::accept(SyntaxVisitor& visitor) {
    if (!visitor.openFunctionResult(*this))
        return;
    resultType->accept(visitor);
    visitor.closeFunctionResult(*this);
}

ParameterClause::ParameterClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ParameterClause::accept(SyntaxVisitor& visitor) {
    if (!visitor.openParameterClause(*this))
        return;
    if (parameters)
    {
        Parameter* node = 0;
        size_t _alength = parameters->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*parameters)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeParameterClause(*this);
}

Parameter::Parameter(Position start, Position end)
: SyntaxNode(start, end) {
}

ConstParameter::ConstParameter(Position start, Position end)
: Parameter(start, end) {
}

void ConstParameter::accept(SyntaxVisitor& visitor) {
    if (!visitor.openConstParameter(*this))
        return;
    type->accept(visitor);
    visitor.closeConstParameter(*this);
}

VarParameter::VarParameter(Position start, Position end)
: Parameter(start, end) {
}

void VarParameter::accept(SyntaxVisitor& visitor) {
    if (!visitor.openVarParameter(*this))
        return;
    parameterType->accept(visitor);
    visitor.closeVarParameter(*this);
}

ThrowsClause::ThrowsClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ThrowsClause::accept(SyntaxVisitor& visitor) {
    if (!visitor.openThrowsClause(*this))
        return;
    throwsType->accept(visitor);
    visitor.closeThrowsClause(*this);
}

EnumDeclaration::EnumDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void EnumDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openEnumDeclaration(*this))
        return;
    {
        EnumMember* node = 0;
        size_t _alength = members->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*members)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeEnumDeclaration(*this);
}

EnumMember::EnumMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void EnumMember::accept(SyntaxVisitor& visitor) {
    if (!visitor.openEnumMember(*this))
        return;
    enumCase->accept(visitor);
    if (additionalCases)
    {
        AdditionalCase* node = 0;
        size_t _alength = additionalCases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalCases)[_a];
            node->accept(visitor);
        }
    }
    if (tupleType)
        tupleType->accept(visitor);
    visitor.closeEnumMember(*this);
}

TupleType::TupleType(Position start, Position end)
: SyntaxNode(start, end) {
}

void TupleType::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTupleType(*this))
        return;
    tupleType->accept(visitor);
    if (additionalTypes)
    {
        AdditionalType* node = 0;
        size_t _alength = additionalTypes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalTypes)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeTupleType(*this);
}

AdditionalType::AdditionalType(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalType::accept(SyntaxVisitor& visitor) {
    if (!visitor.openAdditionalType(*this))
        return;
    enumCase->accept(visitor);
    visitor.closeAdditionalType(*this);
}

EnumCase::EnumCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void EnumCase::accept(SyntaxVisitor& visitor) {
    visitor.visitEnumCase(*this);
}

AdditionalCase::AdditionalCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalCase::accept(SyntaxVisitor& visitor) {
    if (!visitor.openAdditionalCase(*this))
        return;
    enumCase->accept(visitor);
    visitor.closeAdditionalCase(*this);
}

ClassDeclaration::ClassDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void ClassDeclaration::accept(SyntaxVisitor& visitor) {
    if (!visitor.openClassDeclaration(*this))
        return;
    if (genericArgumentClause)
        genericArgumentClause->accept(visitor);
    if (typeInheritanceClause)
        typeInheritanceClause->accept(visitor);
    if (members)
    {
        ClassMember* node = 0;
        size_t _alength = members->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*members)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeClassDeclaration(*this);
}

GenericArgumentClause::GenericArgumentClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void GenericArgumentClause::accept(SyntaxVisitor& visitor) {
    if (!visitor.openGenericArgumentClause(*this))
        return;
    {
        GenericParameter* node = 0;
        size_t _alength = genericParameters->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*genericParameters)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeGenericArgumentClause(*this);
}

GenericParameter::GenericParameter(Position start, Position end)
: SyntaxNode(start, end) {
}

void GenericParameter::accept(SyntaxVisitor& visitor) {
    visitor.visitGenericParameter(*this);
}

ClassMember::ClassMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void ClassMember::accept(SyntaxVisitor& visitor) {
    if (!visitor.openClassMember(*this))
        return;
    declaration->accept(visitor);
    visitor.closeClassMember(*this);
}

Expression::Expression(Position start, Position end)
: Statement(start, end) {
}

CodeBlock::CodeBlock(Position start, Position end)
: Expression(start, end) {
}

void CodeBlock::accept(SyntaxVisitor& visitor) {
    if (!visitor.openCodeBlock(*this))
        return;
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeCodeBlock(*this);
}

SimpleExpression::SimpleExpression(Position start, Position end)
: Expression(start, end) {
}

void SimpleExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSimpleExpression(*this))
        return;
    prefixExpression->accept(visitor);
    if (binaryOps)
    {
        BinaryOp* node = 0;
        size_t _alength = binaryOps->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*binaryOps)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeSimpleExpression(*this);
}

PrefixExpression::PrefixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PrefixExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openPrefixExpression(*this))
        return;
    expression->accept(visitor);
    visitor.closePrefixExpression(*this);
}

PostfixExpression::PostfixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PostfixExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openPostfixExpression(*this))
        return;
    primaryExpression->accept(visitor);
    if (postfixes)
    {
        Postfix* node = 0;
        size_t _alength = postfixes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*postfixes)[_a];
            node->accept(visitor);
        }
    }
    visitor.closePostfixExpression(*this);
}

BinaryOp::BinaryOp(Position start, Position end)
: SyntaxNode(start, end) {
}

BinaryOperation::BinaryOperation(Position start, Position end)
: BinaryOp(start, end) {
}

void BinaryOperation::accept(SyntaxVisitor& visitor) {
    if (!visitor.openBinaryOperation(*this))
        return;
    expression->accept(visitor);
    visitor.closeBinaryOperation(*this);
}

Assignment::Assignment(Position start, Position end)
: BinaryOp(start, end) {
}

void Assignment::accept(SyntaxVisitor& visitor) {
    if (!visitor.openAssignment(*this))
        return;
    expression->accept(visitor);
    visitor.closeAssignment(*this);
}

TypeQuery::TypeQuery(Position start, Position end)
: BinaryOp(start, end) {
}

void TypeQuery::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTypeQuery(*this))
        return;
    objectType->accept(visitor);
    visitor.closeTypeQuery(*this);
}

TypeCast::TypeCast(Position start, Position end)
: BinaryOp(start, end) {
}

void TypeCast::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTypeCast(*this))
        return;
    objectType->accept(visitor);
    visitor.closeTypeCast(*this);
}

CatchClause::CatchClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void CatchClause::accept(SyntaxVisitor& visitor) {
    if (!visitor.openCatchClause(*this))
        return;
    catchPattern->accept(visitor);
    if (bindingPattern)
        bindingPattern->accept(visitor);
    expression->accept(visitor);
    visitor.closeCatchClause(*this);
}

CatchPattern::CatchPattern(Position start, Position end)
: SyntaxNode(start, end) {
}

WildCardCatchPattern::WildCardCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

void WildCardCatchPattern::accept(SyntaxVisitor& visitor) {
    if (!visitor.openWildCardCatchPattern(*this))
        return;
    pattern->accept(visitor);
    visitor.closeWildCardCatchPattern(*this);
}

PathItemCatchPattern::PathItemCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

void PathItemCatchPattern::accept(SyntaxVisitor& visitor) {
    if (!visitor.openPathItemCatchPattern(*this))
        return;
    catchCase->accept(visitor);
    if (catchCaseExtensions)
    {
        PathIdentifier* node = 0;
        size_t _alength = catchCaseExtensions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*catchCaseExtensions)[_a];
            node->accept(visitor);
        }
    }
    visitor.closePathItemCatchPattern(*this);
}

Postfix::Postfix(Position start, Position end)
: SyntaxNode(start, end) {
}

OperatorPostfix::OperatorPostfix(Position start, Position end)
: Postfix(start, end) {
}

void OperatorPostfix::accept(SyntaxVisitor& visitor) {
    visitor.visitOperatorPostfix(*this);
}

FunctionCall::FunctionCall(Position start, Position end)
: Postfix(start, end) {
}

void FunctionCall::accept(SyntaxVisitor& visitor) {
    if (!visitor.openFunctionCall(*this))
        return;
    arguments->accept(visitor);
    if (catchClauses)
    {
        CatchClause* node = 0;
        size_t _alength = catchClauses->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*catchClauses)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeFunctionCall(*this);
}

ExplicitMemberExpression::ExplicitMemberExpression(Position start, Position end)
: Postfix(start, end) {
}

void ExplicitMemberExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openExplicitMemberExpression(*this))
        return;
    memberPostfix->accept(visitor);
    visitor.closeExplicitMemberExpression(*this);
}

Subscript::Subscript(Position start, Position end)
: Postfix(start, end) {
}

void Subscript::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSubscript(*this))
        return;
    {
        ExpressionElement* node = 0;
        size_t _alength = expressions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*expressions)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeSubscript(*this);
}

ExpressionElement::ExpressionElement(Position start, Position end)
: SyntaxNode(start, end) {
}

void ExpressionElement::accept(SyntaxVisitor& visitor) {
    if (!visitor.openExpressionElement(*this))
        return;
    expression->accept(visitor);
    visitor.closeExpressionElement(*this);
}

MemberPostfix::MemberPostfix(Position start, Position end)
: ExplicitMemberExpression(start, end) {
}

NamedMemberPostfix::NamedMemberPostfix(Position start, Position end)
: MemberPostfix(start, end) {
}

void NamedMemberPostfix::accept(SyntaxVisitor& visitor) {
    if (!visitor.openNamedMemberPostfix(*this))
        return;
    identifier->accept(visitor);
    visitor.closeNamedMemberPostfix(*this);
}

PrimaryExpression::PrimaryExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

ParenthesizedExpression::ParenthesizedExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ParenthesizedExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openParenthesizedExpression(*this))
        return;
    if (expressionElements)
    {
        ExpressionElement* node = 0;
        size_t _alength = expressionElements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*expressionElements)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeParenthesizedExpression(*this);
}

LiteralExpression::LiteralExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void LiteralExpression::accept(SyntaxVisitor& visitor) {
    visitor.visitLiteralExpression(*this);
}

IdentifierExpression::IdentifierExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void IdentifierExpression::accept(SyntaxVisitor& visitor) {
    visitor.visitIdentifierExpression(*this);
}

IfExpression::IfExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void IfExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openIfExpression(*this))
        return;
    condition->accept(visitor);
    consequent->accept(visitor);
    if (elseClause)
        elseClause->accept(visitor);
    visitor.closeIfExpression(*this);
}

ElseClause::ElseClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ElseClause::accept(SyntaxVisitor& visitor) {
    if (!visitor.openElseClause(*this))
        return;
    alternative->accept(visitor);
    visitor.closeElseClause(*this);
}

SwitchExpression::SwitchExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void SwitchExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSwitchExpression(*this))
        return;
    expression->accept(visitor);
    body->accept(visitor);
    visitor.closeSwitchExpression(*this);
}

SwitchBody::SwitchBody(Position start, Position end)
: SyntaxNode(start, end) {
}

CurliedSwitchBody::CurliedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

void CurliedSwitchBody::accept(SyntaxVisitor& visitor) {
    if (!visitor.openCurliedSwitchBody(*this))
        return;
    {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeCurliedSwitchBody(*this);
}

NakedSwitchBody::NakedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

void NakedSwitchBody::accept(SyntaxVisitor& visitor) {
    if (!visitor.openNakedSwitchBody(*this))
        return;
    {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeNakedSwitchBody(*this);
}

SwitchCase::SwitchCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void SwitchCase::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSwitchCase(*this))
        return;
    label->accept(visitor);
    content->accept(visitor);
    visitor.closeSwitchCase(*this);
}

CaseLabel::CaseLabel(Position start, Position end)
: SyntaxNode(start, end) {
}

ItemCaseLabel::ItemCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

void ItemCaseLabel::accept(SyntaxVisitor& visitor) {
    if (!visitor.openItemCaseLabel(*this))
        return;
    pattern->accept(visitor);
    if (additionalPatterns)
    {
        CaseItem* node = 0;
        size_t _alength = additionalPatterns->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*additionalPatterns)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeItemCaseLabel(*this);
}

CaseItem::CaseItem(Position start, Position end)
: SyntaxNode(start, end) {
}

void CaseItem::accept(SyntaxVisitor& visitor) {
    if (!visitor.openCaseItem(*this))
        return;
    pattern->accept(visitor);
    visitor.closeCaseItem(*this);
}

ForExpression::ForExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ForExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openForExpression(*this))
        return;
    loop->accept(visitor);
    visitor.closeForExpression(*this);
}

ForLoop::ForLoop(Position start, Position end)
: SyntaxNode(start, end) {
}

ForEach::ForEach(Position start, Position end)
: ForLoop(start, end) {
}

void ForEach::accept(SyntaxVisitor& visitor) {
    if (!visitor.openForEach(*this))
        return;
    pattern->accept(visitor);
    expression->accept(visitor);
    code->accept(visitor);
    visitor.closeForEach(*this);
}

PlainFor::PlainFor(Position start, Position end)
: ForLoop(start, end) {
}

void PlainFor::accept(SyntaxVisitor& visitor) {
    if (!visitor.openPlainFor(*this))
        return;
    if (forInit)
        forInit->accept(visitor);
    if (forCheck)
        forCheck->accept(visitor);
    forNext->accept(visitor);
    code->accept(visitor);
    visitor.closePlainFor(*this);
}

ReturnExpression::ReturnExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ReturnExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openReturnExpression(*this))
        return;
    if (expression)
        expression->accept(visitor);
    visitor.closeReturnExpression(*this);
}

ThrowExpression::ThrowExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ThrowExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openThrowExpression(*this))
        return;
    if (expression)
        expression->accept(visitor);
    visitor.closeThrowExpression(*this);
}

BreakExpression::BreakExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void BreakExpression::accept(SyntaxVisitor& visitor) {
    if (!visitor.openBreakExpression(*this))
        return;
    if (expression)
        expression->accept(visitor);
    visitor.closeBreakExpression(*this);
}

Pattern::Pattern(Position start, Position end)
: SyntaxNode(start, end) {
}

WildcardPattern::WildcardPattern(Position start, Position end)
: Pattern(start, end) {
}

void WildcardPattern::accept(SyntaxVisitor& visitor) {
    visitor.visitWildcardPattern(*this);
}

IdentifierPattern::IdentifierPattern(Position start, Position end)
: Pattern(start, end) {
}

void IdentifierPattern::accept(SyntaxVisitor& visitor) {
    if (!visitor.openIdentifierPattern(*this))
        return;
    if (typeAnnotation)
        typeAnnotation->accept(visitor);
    visitor.closeIdentifierPattern(*this);
}

TuplePattern::TuplePattern(Position start, Position end)
: Pattern(start, end) {
}

void TuplePattern::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTuplePattern(*this))
        return;
    {
        TuplePatternElement* node = 0;
        size_t _alength = elements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*elements)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeTuplePattern(*this);
}

TuplePatternElement::TuplePatternElement(Position start, Position end)
: SyntaxNode(start, end) {
}

void TuplePatternElement::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTuplePatternElement(*this))
        return;
    pattern->accept(visitor);
    visitor.closeTuplePatternElement(*this);
}

ExpressionPattern::ExpressionPattern(Position start, Position end)
: Pattern(start, end) {
}

void ExpressionPattern::accept(SyntaxVisitor& visitor) {
    if (!visitor.openExpressionPattern(*this))
        return;
    expression->accept(visitor);
    visitor.closeExpressionPattern(*this);
}

DefaultCaseLabel::DefaultCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

void DefaultCaseLabel::accept(SyntaxVisitor& visitor) {
    visitor.visitDefaultCaseLabel(*this);
}

CaseContent::CaseContent(Position start, Position end)
: SyntaxNode(start, end) {
}

BlockCaseContent::BlockCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

void BlockCaseContent::accept(SyntaxVisitor& visitor) {
    if (!visitor.openBlockCaseContent(*this))
        return;
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeBlockCaseContent(*this);
}

EmptyCaseContent::EmptyCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

void EmptyCaseContent::accept(SyntaxVisitor& visitor) {
    visitor.visitEmptyCaseContent(*this);
}

InitializerCall::InitializerCall(Position start, Position end)
: PrimaryExpression(start, end) {
}

void InitializerCall::accept(SyntaxVisitor& visitor) {
    if (!visitor.openInitializerCall(*this))
        return;
    typeToInitialize->accept(visitor);
    arguments->accept(visitor);
    if (catchClauses)
    {
        CatchClause* node = 0;
        size_t _alength = catchClauses->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*catchClauses)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeInitializerCall(*this);
}

ThisExpression::ThisExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

ThisDot::ThisDot(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisDot::accept(SyntaxVisitor& visitor) {
    if (!visitor.openThisDot(*this))
        return;
    member->accept(visitor);
    visitor.closeThisDot(*this);
}

ThisSubscript::ThisSubscript(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisSubscript::accept(SyntaxVisitor& visitor) {
    if (!visitor.openThisSubscript(*this))
        return;
    subscript->accept(visitor);
    visitor.closeThisSubscript(*this);
}

ThisWord::ThisWord(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisWord::accept(SyntaxVisitor& visitor) {
    visitor.visitThisWord(*this);
}

CommonThisMember::CommonThisMember(Position start, Position end)
: SyntaxNode(start, end) {
}

ThisInit::ThisInit(Position start, Position end)
: CommonThisMember(start, end) {
}

void ThisInit::accept(SyntaxVisitor& visitor) {
    visitor.visitThisInit(*this);
}

ThisMember::ThisMember(Position start, Position end)
: CommonThisMember(start, end) {
}

void ThisMember::accept(SyntaxVisitor& visitor) {
    visitor.visitThisMember(*this);
}

SuperExpression::SuperExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

SuperDot::SuperDot(Position start, Position end)
: SuperExpression(start, end) {
}

void SuperDot::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSuperDot(*this))
        return;
    member->accept(visitor);
    visitor.closeSuperDot(*this);
}

SuperSubscript::SuperSubscript(Position start, Position end)
: SuperExpression(start, end) {
}

void SuperSubscript::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSuperSubscript(*this))
        return;
    subscript->accept(visitor);
    visitor.closeSuperSubscript(*this);
}

CommonSuperMember::CommonSuperMember(Position start, Position end)
: SyntaxNode(start, end) {
}

SuperInit::SuperInit(Position start, Position end)
: CommonSuperMember(start, end) {
}

void SuperInit::accept(SyntaxVisitor& visitor) {
    visitor.visitSuperInit(*this);
}

SuperMember::SuperMember(Position start, Position end)
: CommonSuperMember(start, end) {
}

void SuperMember::accept(SyntaxVisitor& visitor) {
    visitor.visitSuperMember(*this);
}

Type::Type(Position start, Position end)
: SyntaxNode(start, end) {
}

TypeAnnotation::TypeAnnotation(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypeAnnotation::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTypeAnnotation(*this))
        return;
    typeAnnotation->accept(visitor);
    visitor.closeTypeAnnotation(*this);
}

TypeIdentifier::TypeIdentifier(Position start, Position end)
: Type(start, end) {
}

void TypeIdentifier::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTypeIdentifier(*this))
        return;
    if (subType)
        subType->accept(visitor);
    if (postfixes)
    {
        TypePostfix* node = 0;
        size_t _alength = postfixes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*postfixes)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeTypeIdentifier(*this);
}

SubtypeIdentifier::SubtypeIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void SubtypeIdentifier::accept(SyntaxVisitor& visitor) {
    if (!visitor.openSubtypeIdentifier(*this))
        return;
    typeIdentifier->accept(visitor);
    visitor.closeSubtypeIdentifier(*this);
}

TypePostfix::TypePostfix(Position start, Position end)
: SyntaxNode(start, end) {
}

ArrayType::ArrayType(Position start, Position end)
: Type(start, end) {
}

void ArrayType::accept(SyntaxVisitor& visitor) {
    if (!visitor.openArrayType(*this))
        return;
    elementType->accept(visitor);
    if (postfixes)
    {
        TypePostfix* node = 0;
        size_t _alength = postfixes->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*postfixes)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeArrayType(*this);
}

OptionalType::OptionalType(Position start, Position end)
: TypePostfix(start, end) {
}

void OptionalType::accept(SyntaxVisitor& visitor) {
    visitor.visitOptionalType(*this);
}

TypeInheritanceClause::TypeInheritanceClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypeInheritanceClause::accept(SyntaxVisitor& visitor) {
    if (!visitor.openTypeInheritanceClause(*this))
        return;
    if (inheritances)
    {
        Inheritance* node = 0;
        size_t _alength = inheritances->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*inheritances)[_a];
            node->accept(visitor);
        }
    }
    visitor.closeTypeInheritanceClause(*this);
}

Inheritance::Inheritance(Position start, Position end)
: SyntaxNode(start, end) {
}

void Inheritance::accept(SyntaxVisitor& visitor) {
    if (!visitor.openInheritance(*this))
        return;
    name->accept(visitor);
    visitor.closeInheritance(*this);
}

}
