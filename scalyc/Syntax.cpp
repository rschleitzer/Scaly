#include "scalyc.h"
using namespace scaly;
namespace scalyc {

SyntaxNode::SyntaxNode(Position start, Position end)
: start(start), end(end) {
}

Program::Program(Position start, Position end)
: SyntaxNode(start, end) {
}

void Program::Accept(SyntaxVisitor& visitor) {
    visitor.VisitProgram(*this);
    {
        CompilationUnit* node = 0;
        size_t _alength = compilationUnits->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*compilationUnits)[_a];
            node->Accept(visitor);
        }
    }
}

CompilationUnit::CompilationUnit(Position start, Position end)
: SyntaxNode(start, end) {
}

void CompilationUnit::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCompilationUnit(*this);
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->Accept(visitor);
        }
    }
}

StatementWithSemicolon::StatementWithSemicolon(Position start, Position end)
: SyntaxNode(start, end) {
}

void StatementWithSemicolon::Accept(SyntaxVisitor& visitor) {
    visitor.VisitStatementWithSemicolon(*this);
    statement->Accept(visitor);
}

Statement::Statement(Position start, Position end)
: SyntaxNode(start, end) {
}

void Statement::Accept(SyntaxVisitor& visitor) {
    visitor.VisitStatement(*this);
}

Declaration::Declaration(Position start, Position end)
: Statement(start, end) {
}

void Declaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitDeclaration(*this);
}

UseDeclaration::UseDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void UseDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitUseDeclaration(*this);
    importModule->Accept(visitor);
    if (importExtensions) {
        {
            PathIdentifier* node = 0;
            size_t _alength = importExtensions->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*importExtensions)[_a];
                node->Accept(visitor);
            }
        }
    }
}

PathIdentifier::PathIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void PathIdentifier::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPathIdentifier(*this);
    extension->Accept(visitor);
}

PathItem::PathItem(Position start, Position end)
: SyntaxNode(start, end) {
}

void PathItem::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPathItem(*this);
}

Initializer::Initializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void Initializer::Accept(SyntaxVisitor& visitor) {
    visitor.VisitInitializer(*this);
    expression->Accept(visitor);
}

ConstantDeclaration::ConstantDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void ConstantDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitConstantDeclaration(*this);
    initializer->Accept(visitor);
}

VariableDeclaration::VariableDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void VariableDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitVariableDeclaration(*this);
    initializer->Accept(visitor);
}

BindingInitializer::BindingInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void BindingInitializer::Accept(SyntaxVisitor& visitor) {
    visitor.VisitBindingInitializer(*this);
    initializer->Accept(visitor);
    if (additionalInitializers) {
        {
            AdditionalInitializer* node = 0;
            size_t _alength = additionalInitializers->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*additionalInitializers)[_a];
                node->Accept(visitor);
            }
        }
    }
}

PatternInitializer::PatternInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void PatternInitializer::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPatternInitializer(*this);
    pattern->Accept(visitor);
    if (initializer) {
        initializer->Accept(visitor);
    }
}

AdditionalInitializer::AdditionalInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalInitializer::Accept(SyntaxVisitor& visitor) {
    visitor.VisitAdditionalInitializer(*this);
    pattern->Accept(visitor);
}

FunctionDeclaration::FunctionDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void FunctionDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitFunctionDeclaration(*this);
    if (modifiers) {
        {
            Modifier* node = 0;
            size_t _alength = modifiers->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*modifiers)[_a];
                node->Accept(visitor);
            }
        }
    }
    name->Accept(visitor);
    signature->Accept(visitor);
    body->Accept(visitor);
}

InitializerDeclaration::InitializerDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void InitializerDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitInitializerDeclaration(*this);
    if (modifiers) {
        {
            Modifier* node = 0;
            size_t _alength = modifiers->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*modifiers)[_a];
                node->Accept(visitor);
            }
        }
    }
    parameterClause->Accept(visitor);
    if (throwsClause) {
        throwsClause->Accept(visitor);
    }
    body->Accept(visitor);
}

Modifier::Modifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void Modifier::Accept(SyntaxVisitor& visitor) {
    visitor.VisitModifier(*this);
}

Override::Override(Position start, Position end)
: Modifier(start, end) {
}

void Override::Accept(SyntaxVisitor& visitor) {
    visitor.VisitOverride(*this);
}

Static::Static(Position start, Position end)
: Modifier(start, end) {
}

void Static::Accept(SyntaxVisitor& visitor) {
    visitor.VisitStatic(*this);
}

FunctionName::FunctionName(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionName::Accept(SyntaxVisitor& visitor) {
    visitor.VisitFunctionName(*this);
}

IdentifierFunctionName::IdentifierFunctionName(Position start, Position end)
: FunctionName(start, end) {
}

void IdentifierFunctionName::Accept(SyntaxVisitor& visitor) {
    visitor.VisitIdentifierFunctionName(*this);
}

FunctionSignature::FunctionSignature(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionSignature::Accept(SyntaxVisitor& visitor) {
    visitor.VisitFunctionSignature(*this);
    parameterClause->Accept(visitor);
    if (result) {
        result->Accept(visitor);
    }
    if (throwsClause) {
        throwsClause->Accept(visitor);
    }
}

FunctionResult::FunctionResult(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionResult::Accept(SyntaxVisitor& visitor) {
    visitor.VisitFunctionResult(*this);
    resultType->Accept(visitor);
}

ParameterClause::ParameterClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ParameterClause::Accept(SyntaxVisitor& visitor) {
    visitor.VisitParameterClause(*this);
    if (parameters) {
        {
            Parameter* node = 0;
            size_t _alength = parameters->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*parameters)[_a];
                node->Accept(visitor);
            }
        }
    }
}

Parameter::Parameter(Position start, Position end)
: SyntaxNode(start, end) {
}

void Parameter::Accept(SyntaxVisitor& visitor) {
    visitor.VisitParameter(*this);
}

ConstParameter::ConstParameter(Position start, Position end)
: Parameter(start, end) {
}

void ConstParameter::Accept(SyntaxVisitor& visitor) {
    visitor.VisitConstParameter(*this);
    parameterType->Accept(visitor);
}

VarParameter::VarParameter(Position start, Position end)
: Parameter(start, end) {
}

void VarParameter::Accept(SyntaxVisitor& visitor) {
    visitor.VisitVarParameter(*this);
    parameterType->Accept(visitor);
}

ThrowsClause::ThrowsClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ThrowsClause::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThrowsClause(*this);
    throwsType->Accept(visitor);
}

EnumDeclaration::EnumDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void EnumDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitEnumDeclaration(*this);
    {
        EnumMember* node = 0;
        size_t _alength = members->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*members)[_a];
            node->Accept(visitor);
        }
    }
}

EnumMember::EnumMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void EnumMember::Accept(SyntaxVisitor& visitor) {
    visitor.VisitEnumMember(*this);
    enumCase->Accept(visitor);
    if (additionalCases) {
        {
            AdditionalCase* node = 0;
            size_t _alength = additionalCases->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*additionalCases)[_a];
                node->Accept(visitor);
            }
        }
    }
    if (tupleType) {
        tupleType->Accept(visitor);
    }
}

TupleType::TupleType(Position start, Position end)
: SyntaxNode(start, end) {
}

void TupleType::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTupleType(*this);
    tupleType->Accept(visitor);
    if (additionalTypes) {
        {
            AdditionalType* node = 0;
            size_t _alength = additionalTypes->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*additionalTypes)[_a];
                node->Accept(visitor);
            }
        }
    }
}

AdditionalType::AdditionalType(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalType::Accept(SyntaxVisitor& visitor) {
    visitor.VisitAdditionalType(*this);
    enumCase->Accept(visitor);
}

EnumCase::EnumCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void EnumCase::Accept(SyntaxVisitor& visitor) {
    visitor.VisitEnumCase(*this);
}

AdditionalCase::AdditionalCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalCase::Accept(SyntaxVisitor& visitor) {
    visitor.VisitAdditionalCase(*this);
    enumCase->Accept(visitor);
}

ClassDeclaration::ClassDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void ClassDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.VisitClassDeclaration(*this);
    if (genericArgumentClause) {
        genericArgumentClause->Accept(visitor);
    }
    if (typeInheritanceClause) {
        typeInheritanceClause->Accept(visitor);
    }
    if (members) {
        {
            ClassMember* node = 0;
            size_t _alength = members->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*members)[_a];
                node->Accept(visitor);
            }
        }
    }
}

GenericArgumentClause::GenericArgumentClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void GenericArgumentClause::Accept(SyntaxVisitor& visitor) {
    visitor.VisitGenericArgumentClause(*this);
    {
        GenericParameter* node = 0;
        size_t _alength = genericParameters->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*genericParameters)[_a];
            node->Accept(visitor);
        }
    }
}

GenericParameter::GenericParameter(Position start, Position end)
: SyntaxNode(start, end) {
}

void GenericParameter::Accept(SyntaxVisitor& visitor) {
    visitor.VisitGenericParameter(*this);
}

ClassMember::ClassMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void ClassMember::Accept(SyntaxVisitor& visitor) {
    visitor.VisitClassMember(*this);
    declaration->Accept(visitor);
}

Expression::Expression(Position start, Position end)
: Statement(start, end) {
}

void Expression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitExpression(*this);
}

CodeBlock::CodeBlock(Position start, Position end)
: Expression(start, end) {
}

void CodeBlock::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCodeBlock(*this);
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->Accept(visitor);
        }
    }
}

SimpleExpression::SimpleExpression(Position start, Position end)
: Expression(start, end) {
}

void SimpleExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSimpleExpression(*this);
    prefixExpression->Accept(visitor);
    if (binaryOps) {
        {
            BinaryOp* node = 0;
            size_t _alength = binaryOps->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*binaryOps)[_a];
                node->Accept(visitor);
            }
        }
    }
}

PrefixExpression::PrefixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PrefixExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPrefixExpression(*this);
    expression->Accept(visitor);
}

PostfixExpression::PostfixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PostfixExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPostfixExpression(*this);
    primaryExpression->Accept(visitor);
    if (postfixes) {
        {
            Postfix* node = 0;
            size_t _alength = postfixes->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*postfixes)[_a];
                node->Accept(visitor);
            }
        }
    }
}

BinaryOp::BinaryOp(Position start, Position end)
: SyntaxNode(start, end) {
}

void BinaryOp::Accept(SyntaxVisitor& visitor) {
    visitor.VisitBinaryOp(*this);
}

BinaryOperation::BinaryOperation(Position start, Position end)
: BinaryOp(start, end) {
}

void BinaryOperation::Accept(SyntaxVisitor& visitor) {
    visitor.VisitBinaryOperation(*this);
    expression->Accept(visitor);
}

Assignment::Assignment(Position start, Position end)
: BinaryOp(start, end) {
}

void Assignment::Accept(SyntaxVisitor& visitor) {
    visitor.VisitAssignment(*this);
    expression->Accept(visitor);
}

TypeQuery::TypeQuery(Position start, Position end)
: BinaryOp(start, end) {
}

void TypeQuery::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTypeQuery(*this);
    objectType->Accept(visitor);
}

TypeCast::TypeCast(Position start, Position end)
: BinaryOp(start, end) {
}

void TypeCast::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTypeCast(*this);
    objectType->Accept(visitor);
}

CatchClause::CatchClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void CatchClause::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCatchClause(*this);
    catchPattern->Accept(visitor);
    if (bindingPattern) {
        bindingPattern->Accept(visitor);
    }
    expression->Accept(visitor);
}

CatchPattern::CatchPattern(Position start, Position end)
: SyntaxNode(start, end) {
}

void CatchPattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCatchPattern(*this);
}

WildCardCatchPattern::WildCardCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

void WildCardCatchPattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitWildCardCatchPattern(*this);
    pattern->Accept(visitor);
}

PathItemCatchPattern::PathItemCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

void PathItemCatchPattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPathItemCatchPattern(*this);
    catchCase->Accept(visitor);
    if (catchCaseExtensions) {
        {
            PathIdentifier* node = 0;
            size_t _alength = catchCaseExtensions->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*catchCaseExtensions)[_a];
                node->Accept(visitor);
            }
        }
    }
}

Postfix::Postfix(Position start, Position end)
: SyntaxNode(start, end) {
}

void Postfix::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPostfix(*this);
}

OperatorPostfix::OperatorPostfix(Position start, Position end)
: Postfix(start, end) {
}

void OperatorPostfix::Accept(SyntaxVisitor& visitor) {
    visitor.VisitOperatorPostfix(*this);
}

FunctionCall::FunctionCall(Position start, Position end)
: Postfix(start, end) {
}

void FunctionCall::Accept(SyntaxVisitor& visitor) {
    visitor.VisitFunctionCall(*this);
    arguments->Accept(visitor);
    if (catchClauses) {
        {
            CatchClause* node = 0;
            size_t _alength = catchClauses->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*catchClauses)[_a];
                node->Accept(visitor);
            }
        }
    }
}

ExplicitMemberExpression::ExplicitMemberExpression(Position start, Position end)
: Postfix(start, end) {
}

void ExplicitMemberExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitExplicitMemberExpression(*this);
    memberPostfix->Accept(visitor);
}

Subscript::Subscript(Position start, Position end)
: Postfix(start, end) {
}

void Subscript::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSubscript(*this);
    {
        ExpressionElement* node = 0;
        size_t _alength = expressions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*expressions)[_a];
            node->Accept(visitor);
        }
    }
}

ExpressionElement::ExpressionElement(Position start, Position end)
: SyntaxNode(start, end) {
}

void ExpressionElement::Accept(SyntaxVisitor& visitor) {
    visitor.VisitExpressionElement(*this);
    expression->Accept(visitor);
}

MemberPostfix::MemberPostfix(Position start, Position end)
: ExplicitMemberExpression(start, end) {
}

void MemberPostfix::Accept(SyntaxVisitor& visitor) {
    visitor.VisitMemberPostfix(*this);
}

NamedMemberPostfix::NamedMemberPostfix(Position start, Position end)
: MemberPostfix(start, end) {
}

void NamedMemberPostfix::Accept(SyntaxVisitor& visitor) {
    visitor.VisitNamedMemberPostfix(*this);
    identifier->Accept(visitor);
}

PrimaryExpression::PrimaryExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PrimaryExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPrimaryExpression(*this);
}

ParenthesizedExpression::ParenthesizedExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ParenthesizedExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitParenthesizedExpression(*this);
    if (expressionElements) {
        {
            ExpressionElement* node = 0;
            size_t _alength = expressionElements->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*expressionElements)[_a];
                node->Accept(visitor);
            }
        }
    }
}

LiteralExpression::LiteralExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void LiteralExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitLiteralExpression(*this);
}

IdentifierExpression::IdentifierExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void IdentifierExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitIdentifierExpression(*this);
}

IfExpression::IfExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void IfExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitIfExpression(*this);
    condition->Accept(visitor);
    consequent->Accept(visitor);
    if (elseClause) {
        elseClause->Accept(visitor);
    }
}

ElseClause::ElseClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ElseClause::Accept(SyntaxVisitor& visitor) {
    visitor.VisitElseClause(*this);
    alternative->Accept(visitor);
}

SwitchExpression::SwitchExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void SwitchExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSwitchExpression(*this);
    expression->Accept(visitor);
    body->Accept(visitor);
}

SwitchBody::SwitchBody(Position start, Position end)
: SyntaxNode(start, end) {
}

void SwitchBody::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSwitchBody(*this);
}

CurliedSwitchBody::CurliedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

void CurliedSwitchBody::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCurliedSwitchBody(*this);
    {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->Accept(visitor);
        }
    }
}

NakedSwitchBody::NakedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

void NakedSwitchBody::Accept(SyntaxVisitor& visitor) {
    visitor.VisitNakedSwitchBody(*this);
    {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->Accept(visitor);
        }
    }
}

SwitchCase::SwitchCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void SwitchCase::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSwitchCase(*this);
    label->Accept(visitor);
    content->Accept(visitor);
}

CaseLabel::CaseLabel(Position start, Position end)
: SyntaxNode(start, end) {
}

void CaseLabel::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCaseLabel(*this);
}

ItemCaseLabel::ItemCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

void ItemCaseLabel::Accept(SyntaxVisitor& visitor) {
    visitor.VisitItemCaseLabel(*this);
    pattern->Accept(visitor);
    if (additionalPatterns) {
        {
            CaseItem* node = 0;
            size_t _alength = additionalPatterns->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*additionalPatterns)[_a];
                node->Accept(visitor);
            }
        }
    }
}

CaseItem::CaseItem(Position start, Position end)
: SyntaxNode(start, end) {
}

void CaseItem::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCaseItem(*this);
    pattern->Accept(visitor);
}

ForInExpression::ForInExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ForInExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitForInExpression(*this);
    pattern->Accept(visitor);
    if (expression) {
        expression->Accept(visitor);
    }
    if (code) {
        code->Accept(visitor);
    }
}

ReturnExpression::ReturnExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ReturnExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitReturnExpression(*this);
    if (expression) {
        expression->Accept(visitor);
    }
}

ThrowExpression::ThrowExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ThrowExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThrowExpression(*this);
    if (expression) {
        expression->Accept(visitor);
    }
}

BreakExpression::BreakExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void BreakExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitBreakExpression(*this);
    if (expression) {
        expression->Accept(visitor);
    }
}

Pattern::Pattern(Position start, Position end)
: SyntaxNode(start, end) {
}

void Pattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitPattern(*this);
}

WildcardPattern::WildcardPattern(Position start, Position end)
: Pattern(start, end) {
}

void WildcardPattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitWildcardPattern(*this);
}

IdentifierPattern::IdentifierPattern(Position start, Position end)
: Pattern(start, end) {
}

void IdentifierPattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitIdentifierPattern(*this);
    if (typeAnnotation) {
        typeAnnotation->Accept(visitor);
    }
}

TuplePattern::TuplePattern(Position start, Position end)
: Pattern(start, end) {
}

void TuplePattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTuplePattern(*this);
    {
        TuplePatternElement* node = 0;
        size_t _alength = elements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*elements)[_a];
            node->Accept(visitor);
        }
    }
}

TuplePatternElement::TuplePatternElement(Position start, Position end)
: SyntaxNode(start, end) {
}

void TuplePatternElement::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTuplePatternElement(*this);
    pattern->Accept(visitor);
}

ExpressionPattern::ExpressionPattern(Position start, Position end)
: Pattern(start, end) {
}

void ExpressionPattern::Accept(SyntaxVisitor& visitor) {
    visitor.VisitExpressionPattern(*this);
    expression->Accept(visitor);
}

DefaultCaseLabel::DefaultCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

void DefaultCaseLabel::Accept(SyntaxVisitor& visitor) {
    visitor.VisitDefaultCaseLabel(*this);
}

CaseContent::CaseContent(Position start, Position end)
: SyntaxNode(start, end) {
}

void CaseContent::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCaseContent(*this);
}

BlockCaseContent::BlockCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

void BlockCaseContent::Accept(SyntaxVisitor& visitor) {
    visitor.VisitBlockCaseContent(*this);
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->Accept(visitor);
        }
    }
}

EmptyCaseContent::EmptyCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

void EmptyCaseContent::Accept(SyntaxVisitor& visitor) {
    visitor.VisitEmptyCaseContent(*this);
}

InitializerCall::InitializerCall(Position start, Position end)
: PrimaryExpression(start, end) {
}

void InitializerCall::Accept(SyntaxVisitor& visitor) {
    visitor.VisitInitializerCall(*this);
    typeToInitialize->Accept(visitor);
    arguments->Accept(visitor);
    if (catchClauses) {
        {
            CatchClause* node = 0;
            size_t _alength = catchClauses->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*catchClauses)[_a];
                node->Accept(visitor);
            }
        }
    }
}

ThisExpression::ThisExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ThisExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThisExpression(*this);
}

ThisDot::ThisDot(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisDot::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThisDot(*this);
    member->Accept(visitor);
}

ThisSubscript::ThisSubscript(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisSubscript::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThisSubscript(*this);
    subscript->Accept(visitor);
}

This::This(Position start, Position end)
: ThisExpression(start, end) {
}

void This::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThis(*this);
}

CommonThisMember::CommonThisMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void CommonThisMember::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCommonThisMember(*this);
}

ThisInit::ThisInit(Position start, Position end)
: CommonThisMember(start, end) {
}

void ThisInit::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThisInit(*this);
}

ThisMember::ThisMember(Position start, Position end)
: CommonThisMember(start, end) {
}

void ThisMember::Accept(SyntaxVisitor& visitor) {
    visitor.VisitThisMember(*this);
}

SuperExpression::SuperExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void SuperExpression::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSuperExpression(*this);
}

SuperDot::SuperDot(Position start, Position end)
: SuperExpression(start, end) {
}

void SuperDot::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSuperDot(*this);
    member->Accept(visitor);
}

SuperSubscript::SuperSubscript(Position start, Position end)
: SuperExpression(start, end) {
}

void SuperSubscript::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSuperSubscript(*this);
    subscript->Accept(visitor);
}

CommonSuperMember::CommonSuperMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void CommonSuperMember::Accept(SyntaxVisitor& visitor) {
    visitor.VisitCommonSuperMember(*this);
}

SuperInit::SuperInit(Position start, Position end)
: CommonSuperMember(start, end) {
}

void SuperInit::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSuperInit(*this);
}

SuperMember::SuperMember(Position start, Position end)
: CommonSuperMember(start, end) {
}

void SuperMember::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSuperMember(*this);
}

Type::Type(Position start, Position end)
: SyntaxNode(start, end) {
}

void Type::Accept(SyntaxVisitor& visitor) {
    visitor.VisitType(*this);
}

TypeAnnotation::TypeAnnotation(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypeAnnotation::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTypeAnnotation(*this);
    typeAnnotation->Accept(visitor);
}

TypeIdentifier::TypeIdentifier(Position start, Position end)
: Type(start, end) {
}

void TypeIdentifier::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTypeIdentifier(*this);
    if (subType) {
        subType->Accept(visitor);
    }
    if (postfixes) {
        {
            TypePostfix* node = 0;
            size_t _alength = postfixes->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*postfixes)[_a];
                node->Accept(visitor);
            }
        }
    }
}

SubtypeIdentifier::SubtypeIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void SubtypeIdentifier::Accept(SyntaxVisitor& visitor) {
    visitor.VisitSubtypeIdentifier(*this);
    typeIdentifier->Accept(visitor);
}

TypePostfix::TypePostfix(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypePostfix::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTypePostfix(*this);
}

ArrayType::ArrayType(Position start, Position end)
: Type(start, end) {
}

void ArrayType::Accept(SyntaxVisitor& visitor) {
    visitor.VisitArrayType(*this);
    elementType->Accept(visitor);
    if (postfixes) {
        {
            TypePostfix* node = 0;
            size_t _alength = postfixes->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*postfixes)[_a];
                node->Accept(visitor);
            }
        }
    }
}

OptionalType::OptionalType(Position start, Position end)
: TypePostfix(start, end) {
}

void OptionalType::Accept(SyntaxVisitor& visitor) {
    visitor.VisitOptionalType(*this);
}

TypeInheritanceClause::TypeInheritanceClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypeInheritanceClause::Accept(SyntaxVisitor& visitor) {
    visitor.VisitTypeInheritanceClause(*this);
    if (inheritances) {
        {
            Inheritance* node = 0;
            size_t _alength = inheritances->length();
            for (size_t _a = 0; _a < _alength; _a++) {
                node = *(*inheritances)[_a];
                node->Accept(visitor);
            }
        }
    }
}

Inheritance::Inheritance(Position start, Position end)
: SyntaxNode(start, end) {
}

void Inheritance::Accept(SyntaxVisitor& visitor) {
    visitor.VisitInheritance(*this);
    name->Accept(visitor);
}

}
