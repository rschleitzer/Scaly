#include "scalyc.h"
using namespace scaly;
namespace scalyc {

SyntaxNode::SyntaxNode(Position start, Position end)
: start(start), end(end) {
}

Program::Program()
: SyntaxNode(Position(0, 0), Position(0, 0)) {
}

void Program::Accept(SyntaxVisitor& visitor) {
    visitor.OpenProgram(*this);
    {
        CompilationUnit* node = 0;
        size_t _alength = compilationUnits->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*compilationUnits)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseProgram(*this);
}

CompilationUnit::CompilationUnit(Position start, Position end)
: SyntaxNode(start, end) {
}

void CompilationUnit::Accept(SyntaxVisitor& visitor) {
    visitor.OpenCompilationUnit(*this);
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseCompilationUnit(*this);
}

StatementWithSemicolon::StatementWithSemicolon(Position start, Position end)
: SyntaxNode(start, end) {
}

void StatementWithSemicolon::Accept(SyntaxVisitor& visitor) {
    visitor.OpenStatementWithSemicolon(*this);
    statement->Accept(visitor);
    visitor.CloseStatementWithSemicolon(*this);
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

void UseDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenUseDeclaration(*this);
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
    visitor.CloseUseDeclaration(*this);
}

PathIdentifier::PathIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void PathIdentifier::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPathIdentifier(*this);
    extension->Accept(visitor);
    visitor.ClosePathIdentifier(*this);
}

PathItem::PathItem(Position start, Position end)
: SyntaxNode(start, end) {
}

void PathItem::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPathItem(*this);
    visitor.ClosePathItem(*this);
}

Initializer::Initializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void Initializer::Accept(SyntaxVisitor& visitor) {
    visitor.OpenInitializer(*this);
    expression->Accept(visitor);
    visitor.CloseInitializer(*this);
}

ConstantDeclaration::ConstantDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void ConstantDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenConstantDeclaration(*this);
    initializer->Accept(visitor);
    visitor.CloseConstantDeclaration(*this);
}

VariableDeclaration::VariableDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void VariableDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenVariableDeclaration(*this);
    initializer->Accept(visitor);
    visitor.CloseVariableDeclaration(*this);
}

BindingInitializer::BindingInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void BindingInitializer::Accept(SyntaxVisitor& visitor) {
    visitor.OpenBindingInitializer(*this);
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
    visitor.CloseBindingInitializer(*this);
}

PatternInitializer::PatternInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void PatternInitializer::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPatternInitializer(*this);
    pattern->Accept(visitor);
    if (initializer) {
        initializer->Accept(visitor);
    }
    visitor.ClosePatternInitializer(*this);
}

AdditionalInitializer::AdditionalInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalInitializer::Accept(SyntaxVisitor& visitor) {
    visitor.OpenAdditionalInitializer(*this);
    pattern->Accept(visitor);
    visitor.CloseAdditionalInitializer(*this);
}

FunctionDeclaration::FunctionDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void FunctionDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenFunctionDeclaration(*this);
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
    visitor.CloseFunctionDeclaration(*this);
}

InitializerDeclaration::InitializerDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void InitializerDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenInitializerDeclaration(*this);
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
    visitor.CloseInitializerDeclaration(*this);
}

Modifier::Modifier(Position start, Position end)
: SyntaxNode(start, end) {
}

Override::Override(Position start, Position end)
: Modifier(start, end) {
}

void Override::Accept(SyntaxVisitor& visitor) {
    visitor.OpenOverride(*this);
    visitor.CloseOverride(*this);
}

StaticWord::StaticWord(Position start, Position end)
: Modifier(start, end) {
}

void StaticWord::Accept(SyntaxVisitor& visitor) {
    visitor.OpenStaticWord(*this);
    visitor.CloseStaticWord(*this);
}

FunctionName::FunctionName(Position start, Position end)
: SyntaxNode(start, end) {
}

IdentifierFunctionName::IdentifierFunctionName(Position start, Position end)
: FunctionName(start, end) {
}

void IdentifierFunctionName::Accept(SyntaxVisitor& visitor) {
    visitor.OpenIdentifierFunctionName(*this);
    visitor.CloseIdentifierFunctionName(*this);
}

FunctionSignature::FunctionSignature(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionSignature::Accept(SyntaxVisitor& visitor) {
    visitor.OpenFunctionSignature(*this);
    parameterClause->Accept(visitor);
    if (result) {
        result->Accept(visitor);
    }
    if (throwsClause) {
        throwsClause->Accept(visitor);
    }
    visitor.CloseFunctionSignature(*this);
}

FunctionResult::FunctionResult(Position start, Position end)
: SyntaxNode(start, end) {
}

void FunctionResult::Accept(SyntaxVisitor& visitor) {
    visitor.OpenFunctionResult(*this);
    resultType->Accept(visitor);
    visitor.CloseFunctionResult(*this);
}

ParameterClause::ParameterClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ParameterClause::Accept(SyntaxVisitor& visitor) {
    visitor.OpenParameterClause(*this);
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
    visitor.CloseParameterClause(*this);
}

Parameter::Parameter(Position start, Position end)
: SyntaxNode(start, end) {
}

ConstParameter::ConstParameter(Position start, Position end)
: Parameter(start, end) {
}

void ConstParameter::Accept(SyntaxVisitor& visitor) {
    visitor.OpenConstParameter(*this);
    parameterType->Accept(visitor);
    visitor.CloseConstParameter(*this);
}

VarParameter::VarParameter(Position start, Position end)
: Parameter(start, end) {
}

void VarParameter::Accept(SyntaxVisitor& visitor) {
    visitor.OpenVarParameter(*this);
    parameterType->Accept(visitor);
    visitor.CloseVarParameter(*this);
}

ThrowsClause::ThrowsClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ThrowsClause::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThrowsClause(*this);
    throwsType->Accept(visitor);
    visitor.CloseThrowsClause(*this);
}

EnumDeclaration::EnumDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void EnumDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenEnumDeclaration(*this);
    {
        EnumMember* node = 0;
        size_t _alength = members->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*members)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseEnumDeclaration(*this);
}

EnumMember::EnumMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void EnumMember::Accept(SyntaxVisitor& visitor) {
    visitor.OpenEnumMember(*this);
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
    visitor.CloseEnumMember(*this);
}

TupleType::TupleType(Position start, Position end)
: SyntaxNode(start, end) {
}

void TupleType::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTupleType(*this);
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
    visitor.CloseTupleType(*this);
}

AdditionalType::AdditionalType(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalType::Accept(SyntaxVisitor& visitor) {
    visitor.OpenAdditionalType(*this);
    enumCase->Accept(visitor);
    visitor.CloseAdditionalType(*this);
}

EnumCase::EnumCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void EnumCase::Accept(SyntaxVisitor& visitor) {
    visitor.OpenEnumCase(*this);
    visitor.CloseEnumCase(*this);
}

AdditionalCase::AdditionalCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void AdditionalCase::Accept(SyntaxVisitor& visitor) {
    visitor.OpenAdditionalCase(*this);
    enumCase->Accept(visitor);
    visitor.CloseAdditionalCase(*this);
}

ClassDeclaration::ClassDeclaration(Position start, Position end)
: Declaration(start, end) {
}

void ClassDeclaration::Accept(SyntaxVisitor& visitor) {
    visitor.OpenClassDeclaration(*this);
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
    visitor.CloseClassDeclaration(*this);
}

GenericArgumentClause::GenericArgumentClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void GenericArgumentClause::Accept(SyntaxVisitor& visitor) {
    visitor.OpenGenericArgumentClause(*this);
    {
        GenericParameter* node = 0;
        size_t _alength = genericParameters->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*genericParameters)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseGenericArgumentClause(*this);
}

GenericParameter::GenericParameter(Position start, Position end)
: SyntaxNode(start, end) {
}

void GenericParameter::Accept(SyntaxVisitor& visitor) {
    visitor.OpenGenericParameter(*this);
    visitor.CloseGenericParameter(*this);
}

ClassMember::ClassMember(Position start, Position end)
: SyntaxNode(start, end) {
}

void ClassMember::Accept(SyntaxVisitor& visitor) {
    visitor.OpenClassMember(*this);
    declaration->Accept(visitor);
    visitor.CloseClassMember(*this);
}

Expression::Expression(Position start, Position end)
: Statement(start, end) {
}

CodeBlock::CodeBlock(Position start, Position end)
: Expression(start, end) {
}

void CodeBlock::Accept(SyntaxVisitor& visitor) {
    visitor.OpenCodeBlock(*this);
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseCodeBlock(*this);
}

SimpleExpression::SimpleExpression(Position start, Position end)
: Expression(start, end) {
}

void SimpleExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSimpleExpression(*this);
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
    visitor.CloseSimpleExpression(*this);
}

PrefixExpression::PrefixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PrefixExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPrefixExpression(*this);
    expression->Accept(visitor);
    visitor.ClosePrefixExpression(*this);
}

PostfixExpression::PostfixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

void PostfixExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPostfixExpression(*this);
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
    visitor.ClosePostfixExpression(*this);
}

BinaryOp::BinaryOp(Position start, Position end)
: SyntaxNode(start, end) {
}

BinaryOperation::BinaryOperation(Position start, Position end)
: BinaryOp(start, end) {
}

void BinaryOperation::Accept(SyntaxVisitor& visitor) {
    visitor.OpenBinaryOperation(*this);
    expression->Accept(visitor);
    visitor.CloseBinaryOperation(*this);
}

Assignment::Assignment(Position start, Position end)
: BinaryOp(start, end) {
}

void Assignment::Accept(SyntaxVisitor& visitor) {
    visitor.OpenAssignment(*this);
    expression->Accept(visitor);
    visitor.CloseAssignment(*this);
}

TypeQuery::TypeQuery(Position start, Position end)
: BinaryOp(start, end) {
}

void TypeQuery::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTypeQuery(*this);
    objectType->Accept(visitor);
    visitor.CloseTypeQuery(*this);
}

TypeCast::TypeCast(Position start, Position end)
: BinaryOp(start, end) {
}

void TypeCast::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTypeCast(*this);
    objectType->Accept(visitor);
    visitor.CloseTypeCast(*this);
}

CatchClause::CatchClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void CatchClause::Accept(SyntaxVisitor& visitor) {
    visitor.OpenCatchClause(*this);
    catchPattern->Accept(visitor);
    if (bindingPattern) {
        bindingPattern->Accept(visitor);
    }
    expression->Accept(visitor);
    visitor.CloseCatchClause(*this);
}

CatchPattern::CatchPattern(Position start, Position end)
: SyntaxNode(start, end) {
}

WildCardCatchPattern::WildCardCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

void WildCardCatchPattern::Accept(SyntaxVisitor& visitor) {
    visitor.OpenWildCardCatchPattern(*this);
    pattern->Accept(visitor);
    visitor.CloseWildCardCatchPattern(*this);
}

PathItemCatchPattern::PathItemCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

void PathItemCatchPattern::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPathItemCatchPattern(*this);
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
    visitor.ClosePathItemCatchPattern(*this);
}

Postfix::Postfix(Position start, Position end)
: SyntaxNode(start, end) {
}

OperatorPostfix::OperatorPostfix(Position start, Position end)
: Postfix(start, end) {
}

void OperatorPostfix::Accept(SyntaxVisitor& visitor) {
    visitor.OpenOperatorPostfix(*this);
    visitor.CloseOperatorPostfix(*this);
}

FunctionCall::FunctionCall(Position start, Position end)
: Postfix(start, end) {
}

void FunctionCall::Accept(SyntaxVisitor& visitor) {
    visitor.OpenFunctionCall(*this);
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
    visitor.CloseFunctionCall(*this);
}

ExplicitMemberExpression::ExplicitMemberExpression(Position start, Position end)
: Postfix(start, end) {
}

void ExplicitMemberExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenExplicitMemberExpression(*this);
    memberPostfix->Accept(visitor);
    visitor.CloseExplicitMemberExpression(*this);
}

Subscript::Subscript(Position start, Position end)
: Postfix(start, end) {
}

void Subscript::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSubscript(*this);
    {
        ExpressionElement* node = 0;
        size_t _alength = expressions->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*expressions)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseSubscript(*this);
}

ExpressionElement::ExpressionElement(Position start, Position end)
: SyntaxNode(start, end) {
}

void ExpressionElement::Accept(SyntaxVisitor& visitor) {
    visitor.OpenExpressionElement(*this);
    expression->Accept(visitor);
    visitor.CloseExpressionElement(*this);
}

MemberPostfix::MemberPostfix(Position start, Position end)
: ExplicitMemberExpression(start, end) {
}

NamedMemberPostfix::NamedMemberPostfix(Position start, Position end)
: MemberPostfix(start, end) {
}

void NamedMemberPostfix::Accept(SyntaxVisitor& visitor) {
    visitor.OpenNamedMemberPostfix(*this);
    identifier->Accept(visitor);
    visitor.CloseNamedMemberPostfix(*this);
}

PrimaryExpression::PrimaryExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

ParenthesizedExpression::ParenthesizedExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ParenthesizedExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenParenthesizedExpression(*this);
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
    visitor.CloseParenthesizedExpression(*this);
}

LiteralExpression::LiteralExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void LiteralExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenLiteralExpression(*this);
    visitor.CloseLiteralExpression(*this);
}

IdentifierExpression::IdentifierExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void IdentifierExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenIdentifierExpression(*this);
    visitor.CloseIdentifierExpression(*this);
}

IfExpression::IfExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void IfExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenIfExpression(*this);
    condition->Accept(visitor);
    consequent->Accept(visitor);
    if (elseClause) {
        elseClause->Accept(visitor);
    }
    visitor.CloseIfExpression(*this);
}

ElseClause::ElseClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void ElseClause::Accept(SyntaxVisitor& visitor) {
    visitor.OpenElseClause(*this);
    alternative->Accept(visitor);
    visitor.CloseElseClause(*this);
}

SwitchExpression::SwitchExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void SwitchExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSwitchExpression(*this);
    expression->Accept(visitor);
    body->Accept(visitor);
    visitor.CloseSwitchExpression(*this);
}

SwitchBody::SwitchBody(Position start, Position end)
: SyntaxNode(start, end) {
}

CurliedSwitchBody::CurliedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

void CurliedSwitchBody::Accept(SyntaxVisitor& visitor) {
    visitor.OpenCurliedSwitchBody(*this);
    {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseCurliedSwitchBody(*this);
}

NakedSwitchBody::NakedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

void NakedSwitchBody::Accept(SyntaxVisitor& visitor) {
    visitor.OpenNakedSwitchBody(*this);
    {
        SwitchCase* node = 0;
        size_t _alength = cases->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*cases)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseNakedSwitchBody(*this);
}

SwitchCase::SwitchCase(Position start, Position end)
: SyntaxNode(start, end) {
}

void SwitchCase::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSwitchCase(*this);
    label->Accept(visitor);
    content->Accept(visitor);
    visitor.CloseSwitchCase(*this);
}

CaseLabel::CaseLabel(Position start, Position end)
: SyntaxNode(start, end) {
}

ItemCaseLabel::ItemCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

void ItemCaseLabel::Accept(SyntaxVisitor& visitor) {
    visitor.OpenItemCaseLabel(*this);
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
    visitor.CloseItemCaseLabel(*this);
}

CaseItem::CaseItem(Position start, Position end)
: SyntaxNode(start, end) {
}

void CaseItem::Accept(SyntaxVisitor& visitor) {
    visitor.OpenCaseItem(*this);
    pattern->Accept(visitor);
    visitor.CloseCaseItem(*this);
}

ForExpression::ForExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ForExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenForExpression(*this);
    loop->Accept(visitor);
    visitor.CloseForExpression(*this);
}

ForLoop::ForLoop(Position start, Position end)
: SyntaxNode(start, end) {
}

ForEach::ForEach(Position start, Position end)
: ForLoop(start, end) {
}

void ForEach::Accept(SyntaxVisitor& visitor) {
    visitor.OpenForEach(*this);
    pattern->Accept(visitor);
    expression->Accept(visitor);
    code->Accept(visitor);
    visitor.CloseForEach(*this);
}

PlainFor::PlainFor(Position start, Position end)
: ForLoop(start, end) {
}

void PlainFor::Accept(SyntaxVisitor& visitor) {
    visitor.OpenPlainFor(*this);
    if (forInit) {
        forInit->Accept(visitor);
    }
    if (forCheck) {
        forCheck->Accept(visitor);
    }
    forNext->Accept(visitor);
    code->Accept(visitor);
    visitor.ClosePlainFor(*this);
}

ReturnExpression::ReturnExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ReturnExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenReturnExpression(*this);
    if (expression) {
        expression->Accept(visitor);
    }
    visitor.CloseReturnExpression(*this);
}

ThrowExpression::ThrowExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void ThrowExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThrowExpression(*this);
    if (expression) {
        expression->Accept(visitor);
    }
    visitor.CloseThrowExpression(*this);
}

BreakExpression::BreakExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

void BreakExpression::Accept(SyntaxVisitor& visitor) {
    visitor.OpenBreakExpression(*this);
    if (expression) {
        expression->Accept(visitor);
    }
    visitor.CloseBreakExpression(*this);
}

Pattern::Pattern(Position start, Position end)
: SyntaxNode(start, end) {
}

WildcardPattern::WildcardPattern(Position start, Position end)
: Pattern(start, end) {
}

void WildcardPattern::Accept(SyntaxVisitor& visitor) {
    visitor.OpenWildcardPattern(*this);
    visitor.CloseWildcardPattern(*this);
}

IdentifierPattern::IdentifierPattern(Position start, Position end)
: Pattern(start, end) {
}

void IdentifierPattern::Accept(SyntaxVisitor& visitor) {
    visitor.OpenIdentifierPattern(*this);
    if (typeAnnotation) {
        typeAnnotation->Accept(visitor);
    }
    visitor.CloseIdentifierPattern(*this);
}

TuplePattern::TuplePattern(Position start, Position end)
: Pattern(start, end) {
}

void TuplePattern::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTuplePattern(*this);
    {
        TuplePatternElement* node = 0;
        size_t _alength = elements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*elements)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseTuplePattern(*this);
}

TuplePatternElement::TuplePatternElement(Position start, Position end)
: SyntaxNode(start, end) {
}

void TuplePatternElement::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTuplePatternElement(*this);
    pattern->Accept(visitor);
    visitor.CloseTuplePatternElement(*this);
}

ExpressionPattern::ExpressionPattern(Position start, Position end)
: Pattern(start, end) {
}

void ExpressionPattern::Accept(SyntaxVisitor& visitor) {
    visitor.OpenExpressionPattern(*this);
    expression->Accept(visitor);
    visitor.CloseExpressionPattern(*this);
}

DefaultCaseLabel::DefaultCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

void DefaultCaseLabel::Accept(SyntaxVisitor& visitor) {
    visitor.OpenDefaultCaseLabel(*this);
    visitor.CloseDefaultCaseLabel(*this);
}

CaseContent::CaseContent(Position start, Position end)
: SyntaxNode(start, end) {
}

BlockCaseContent::BlockCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

void BlockCaseContent::Accept(SyntaxVisitor& visitor) {
    visitor.OpenBlockCaseContent(*this);
    {
        StatementWithSemicolon* node = 0;
        size_t _alength = statements->length();
        for (size_t _a = 0; _a < _alength; _a++) {
            node = *(*statements)[_a];
            node->Accept(visitor);
        }
    }
    visitor.CloseBlockCaseContent(*this);
}

EmptyCaseContent::EmptyCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

void EmptyCaseContent::Accept(SyntaxVisitor& visitor) {
    visitor.OpenEmptyCaseContent(*this);
    visitor.CloseEmptyCaseContent(*this);
}

InitializerCall::InitializerCall(Position start, Position end)
: PrimaryExpression(start, end) {
}

void InitializerCall::Accept(SyntaxVisitor& visitor) {
    visitor.OpenInitializerCall(*this);
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
    visitor.CloseInitializerCall(*this);
}

ThisExpression::ThisExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

ThisDot::ThisDot(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisDot::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThisDot(*this);
    member->Accept(visitor);
    visitor.CloseThisDot(*this);
}

ThisSubscript::ThisSubscript(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisSubscript::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThisSubscript(*this);
    subscript->Accept(visitor);
    visitor.CloseThisSubscript(*this);
}

ThisWord::ThisWord(Position start, Position end)
: ThisExpression(start, end) {
}

void ThisWord::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThisWord(*this);
    visitor.CloseThisWord(*this);
}

CommonThisMember::CommonThisMember(Position start, Position end)
: SyntaxNode(start, end) {
}

ThisInit::ThisInit(Position start, Position end)
: CommonThisMember(start, end) {
}

void ThisInit::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThisInit(*this);
    visitor.CloseThisInit(*this);
}

ThisMember::ThisMember(Position start, Position end)
: CommonThisMember(start, end) {
}

void ThisMember::Accept(SyntaxVisitor& visitor) {
    visitor.OpenThisMember(*this);
    visitor.CloseThisMember(*this);
}

SuperExpression::SuperExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

SuperDot::SuperDot(Position start, Position end)
: SuperExpression(start, end) {
}

void SuperDot::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSuperDot(*this);
    member->Accept(visitor);
    visitor.CloseSuperDot(*this);
}

SuperSubscript::SuperSubscript(Position start, Position end)
: SuperExpression(start, end) {
}

void SuperSubscript::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSuperSubscript(*this);
    subscript->Accept(visitor);
    visitor.CloseSuperSubscript(*this);
}

CommonSuperMember::CommonSuperMember(Position start, Position end)
: SyntaxNode(start, end) {
}

SuperInit::SuperInit(Position start, Position end)
: CommonSuperMember(start, end) {
}

void SuperInit::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSuperInit(*this);
    visitor.CloseSuperInit(*this);
}

SuperMember::SuperMember(Position start, Position end)
: CommonSuperMember(start, end) {
}

void SuperMember::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSuperMember(*this);
    visitor.CloseSuperMember(*this);
}

Type::Type(Position start, Position end)
: SyntaxNode(start, end) {
}

TypeAnnotation::TypeAnnotation(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypeAnnotation::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTypeAnnotation(*this);
    typeAnnotation->Accept(visitor);
    visitor.CloseTypeAnnotation(*this);
}

TypeIdentifier::TypeIdentifier(Position start, Position end)
: Type(start, end) {
}

void TypeIdentifier::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTypeIdentifier(*this);
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
    visitor.CloseTypeIdentifier(*this);
}

SubtypeIdentifier::SubtypeIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

void SubtypeIdentifier::Accept(SyntaxVisitor& visitor) {
    visitor.OpenSubtypeIdentifier(*this);
    typeIdentifier->Accept(visitor);
    visitor.CloseSubtypeIdentifier(*this);
}

TypePostfix::TypePostfix(Position start, Position end)
: SyntaxNode(start, end) {
}

ArrayType::ArrayType(Position start, Position end)
: Type(start, end) {
}

void ArrayType::Accept(SyntaxVisitor& visitor) {
    visitor.OpenArrayType(*this);
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
    visitor.CloseArrayType(*this);
}

OptionalType::OptionalType(Position start, Position end)
: TypePostfix(start, end) {
}

void OptionalType::Accept(SyntaxVisitor& visitor) {
    visitor.OpenOptionalType(*this);
    visitor.CloseOptionalType(*this);
}

TypeInheritanceClause::TypeInheritanceClause(Position start, Position end)
: SyntaxNode(start, end) {
}

void TypeInheritanceClause::Accept(SyntaxVisitor& visitor) {
    visitor.OpenTypeInheritanceClause(*this);
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
    visitor.CloseTypeInheritanceClause(*this);
}

Inheritance::Inheritance(Position start, Position end)
: SyntaxNode(start, end) {
}

void Inheritance::Accept(SyntaxVisitor& visitor) {
    visitor.OpenInheritance(*this);
    name->Accept(visitor);
    visitor.CloseInheritance(*this);
}

}
