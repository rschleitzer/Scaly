#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Position::Position(size_t line, size_t column)
: line(line), column(column) {
}

SyntaxNode::SyntaxNode(Position start, Position end)
: start(start), end(end) {
}

TopLevelDeclaration::TopLevelDeclaration(Position start, Position end)
: SyntaxNode(start, end) {
}

StatementWithSemicolon::StatementWithSemicolon(Position start, Position end)
: SyntaxNode(start, end) {
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

PathIdentifier::PathIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

PathItem::PathItem(Position start, Position end)
: SyntaxNode(start, end) {
}

Initializer::Initializer(Position start, Position end)
: SyntaxNode(start, end) {
}

ConstantDeclaration::ConstantDeclaration(Position start, Position end)
: Declaration(start, end) {
}

PatternInitializer::PatternInitializer(Position start, Position end)
: SyntaxNode(start, end) {
}

VariableDeclaration::VariableDeclaration(Position start, Position end)
: Declaration(start, end) {
}

FunctionDeclaration::FunctionDeclaration(Position start, Position end)
: Declaration(start, end) {
}

Modifier::Modifier(Position start, Position end)
: SyntaxNode(start, end) {
}

Static::Static(Position start, Position end)
: Modifier(start, end) {
}

FunctionName::FunctionName(Position start, Position end)
: SyntaxNode(start, end) {
}

IdentifierFunctionName::IdentifierFunctionName(Position start, Position end)
: FunctionName(start, end) {
}

FunctionSignature::FunctionSignature(Position start, Position end)
: SyntaxNode(start, end) {
}

FunctionResult::FunctionResult(Position start, Position end)
: SyntaxNode(start, end) {
}

ParameterClause::ParameterClause(Position start, Position end)
: SyntaxNode(start, end) {
}

Parameter::Parameter(Position start, Position end)
: SyntaxNode(start, end) {
}

ThrowsClause::ThrowsClause(Position start, Position end)
: SyntaxNode(start, end) {
}

ClassDeclaration::ClassDeclaration(Position start, Position end)
: Declaration(start, end) {
}

GenericArgumentClause::GenericArgumentClause(Position start, Position end)
: SyntaxNode(start, end) {
}

GenericParameter::GenericParameter(Position start, Position end)
: SyntaxNode(start, end) {
}

ClassBody::ClassBody(Position start, Position end)
: SyntaxNode(start, end) {
}

ClassMember::ClassMember(Position start, Position end)
: SyntaxNode(start, end) {
}

Expression::Expression(Position start, Position end)
: Statement(start, end) {
}

CodeBlock::CodeBlock(Position start, Position end)
: Expression(start, end) {
}

SimpleExpression::SimpleExpression(Position start, Position end)
: Expression(start, end) {
}

PrefixExpression::PrefixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

PostfixExpression::PostfixExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

BinaryOp::BinaryOp(Position start, Position end)
: SyntaxNode(start, end) {
}

BinaryExpression::BinaryExpression(Position start, Position end)
: BinaryOp(start, end) {
}

AssignmentExpression::AssignmentExpression(Position start, Position end)
: BinaryOp(start, end) {
}

CatchClause::CatchClause(Position start, Position end)
: SyntaxNode(start, end) {
}

CatchPattern::CatchPattern(Position start, Position end)
: SyntaxNode(start, end) {
}

WildCardCatchPattern::WildCardCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

PathItemCatchPattern::PathItemCatchPattern(Position start, Position end)
: CatchPattern(start, end) {
}

Postfix::Postfix(Position start, Position end)
: SyntaxNode(start, end) {
}

OperatorPostfix::OperatorPostfix(Position start, Position end)
: Postfix(start, end) {
}

FunctionCall::FunctionCall(Position start, Position end)
: Postfix(start, end) {
}

ExplicitMemberExpression::ExplicitMemberExpression(Position start, Position end)
: Postfix(start, end) {
}

Subscript::Subscript(Position start, Position end)
: Postfix(start, end) {
}

ExpressionElement::ExpressionElement(Position start, Position end)
: SyntaxNode(start, end) {
}

MemberPostfix::MemberPostfix(Position start, Position end)
: ExplicitMemberExpression(start, end) {
}

NamedMemberPostfix::NamedMemberPostfix(Position start, Position end)
: MemberPostfix(start, end) {
}

PrimaryExpression::PrimaryExpression(Position start, Position end)
: SyntaxNode(start, end) {
}

ParenthesizedExpression::ParenthesizedExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

LiteralExpression::LiteralExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

IdentifierExpression::IdentifierExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

IfExpression::IfExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

ElseClause::ElseClause(Position start, Position end)
: SyntaxNode(start, end) {
}

SwitchExpression::SwitchExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

SwitchBody::SwitchBody(Position start, Position end)
: SyntaxNode(start, end) {
}

CurliedSwitchBody::CurliedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

NakedSwitchBody::NakedSwitchBody(Position start, Position end)
: SwitchBody(start, end) {
}

SwitchCase::SwitchCase(Position start, Position end)
: SyntaxNode(start, end) {
}

CaseLabel::CaseLabel(Position start, Position end)
: SyntaxNode(start, end) {
}

ItemCaseLabel::ItemCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

CaseItem::CaseItem(Position start, Position end)
: SyntaxNode(start, end) {
}

ReturnExpression::ReturnExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

ThrowExpression::ThrowExpression(Position start, Position end)
: PrimaryExpression(start, end) {
}

Pattern::Pattern(Position start, Position end)
: SyntaxNode(start, end) {
}

WildcardPattern::WildcardPattern(Position start, Position end)
: Pattern(start, end) {
}

IdentifierPattern::IdentifierPattern(Position start, Position end)
: Pattern(start, end) {
}

TuplePattern::TuplePattern(Position start, Position end)
: Pattern(start, end) {
}

TuplePatternElement::TuplePatternElement(Position start, Position end)
: SyntaxNode(start, end) {
}

ExpressionPattern::ExpressionPattern(Position start, Position end)
: Pattern(start, end) {
}

DefaultCaseLabel::DefaultCaseLabel(Position start, Position end)
: CaseLabel(start, end) {
}

CaseContent::CaseContent(Position start, Position end)
: SyntaxNode(start, end) {
}

BlockCaseContent::BlockCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

EmptyCaseContent::EmptyCaseContent(Position start, Position end)
: CaseContent(start, end) {
}

Type::Type(Position start, Position end)
: SyntaxNode(start, end) {
}

TypeAnnotation::TypeAnnotation(Position start, Position end)
: SyntaxNode(start, end) {
}

TypeIdentifier::TypeIdentifier(Position start, Position end)
: Type(start, end) {
}

SubtypeIdentifier::SubtypeIdentifier(Position start, Position end)
: SyntaxNode(start, end) {
}

TypePostfix::TypePostfix(Position start, Position end)
: SyntaxNode(start, end) {
}

ArrayType::ArrayType(Position start, Position end)
: Type(start, end) {
}

OptionalType::OptionalType(Position start, Position end)
: TypePostfix(start, end) {
}

TypeInheritanceClause::TypeInheritanceClause(Position start, Position end)
: SyntaxNode(start, end) {
}

Inheritance::Inheritance(Position start, Position end)
: SyntaxNode(start, end) {
}

}
