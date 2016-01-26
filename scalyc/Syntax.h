#ifndef __scalyc__Syntax__
#define __scalyc__Syntax__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class SyntaxNode : public Object {
public:
    SyntaxNode(Position start, Position end);
    Position start;
    Position end;
};


class TopLevelDeclaration : public SyntaxNode {
public:
    TopLevelDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<StatementWithSemicolon>* statements;
};

class StatementWithSemicolon : public SyntaxNode {
public:
    StatementWithSemicolon(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Statement* statement;
};

class Statement : public SyntaxNode {
public:
    Statement(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class Declaration : public Statement {
public:
    Declaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class UseDeclaration : public Declaration {
public:
    UseDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PathItem* importModule;
    Array<PathIdentifier>* importExtensions;
};

class PathIdentifier : public SyntaxNode {
public:
    PathIdentifier(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PathItem* extension;
};

class PathItem : public SyntaxNode {
public:
    PathItem(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
};

class Initializer : public SyntaxNode {
public:
    Initializer(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class ConstantDeclaration : public Declaration {
public:
    ConstantDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    BindingInitializer* initializer;
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    BindingInitializer* initializer;
};

class BindingInitializer : public SyntaxNode {
public:
    BindingInitializer(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PatternInitializer* initializer;
    Array<AdditionalInitializer>* additionalInitializers;
};

class PatternInitializer : public SyntaxNode {
public:
    PatternInitializer(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Pattern* pattern;
    Initializer* initializer;
};

class AdditionalInitializer : public SyntaxNode {
public:
    AdditionalInitializer(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PatternInitializer* pattern;
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<Modifier>* modifiers;
    FunctionName* name;
    FunctionSignature* signature;
    Expression* body;
};

class InitializerDeclaration : public Declaration {
public:
    InitializerDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<Modifier>* modifiers;
    ParameterClause* parameterClause;
    ThrowsClause* throwsClause;
    Expression* body;
};

class Modifier : public SyntaxNode {
public:
    Modifier(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class Override : public Modifier {
public:
    Override(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class Static : public Modifier {
public:
    Static(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class FunctionName : public SyntaxNode {
public:
    FunctionName(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class IdentifierFunctionName : public FunctionName {
public:
    IdentifierFunctionName(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
};

class FunctionSignature : public SyntaxNode {
public:
    FunctionSignature(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    ParameterClause* parameterClause;
    FunctionResult* result;
    ThrowsClause* throwsClause;
};

class FunctionResult : public SyntaxNode {
public:
    FunctionResult(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* resultType;
};

class ParameterClause : public SyntaxNode {
public:
    ParameterClause(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<Parameter>* parameters;
};

class Parameter : public SyntaxNode {
public:
    Parameter(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ConstParameter : public Parameter {
public:
    ConstParameter(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* parameterName;
    Type* parameterType;
};

class VarParameter : public Parameter {
public:
    VarParameter(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* parameterName;
    Type* parameterType;
};

class ThrowsClause : public SyntaxNode {
public:
    ThrowsClause(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* throwsType;
};

class EnumDeclaration : public Declaration {
public:
    EnumDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
    Array<EnumMember>* members;
};

class EnumMember : public SyntaxNode {
public:
    EnumMember(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    EnumCase* enumCase;
    Array<AdditionalCase>* additionalCases;
    TupleType* tupleType;
};

class TupleType : public SyntaxNode {
public:
    TupleType(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* tupleType;
    Array<AdditionalType>* additionalTypes;
};

class AdditionalType : public SyntaxNode {
public:
    AdditionalType(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* enumCase;
};

class EnumCase : public SyntaxNode {
public:
    EnumCase(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
};

class AdditionalCase : public SyntaxNode {
public:
    AdditionalCase(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    EnumCase* enumCase;
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
    GenericArgumentClause* genericArgumentClause;
    TypeInheritanceClause* typeInheritanceClause;
    Array<ClassMember>* members;
};

class GenericArgumentClause : public SyntaxNode {
public:
    GenericArgumentClause(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<GenericParameter>* genericParameters;
};

class GenericParameter : public SyntaxNode {
public:
    GenericParameter(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* typeName;
};

class ClassMember : public SyntaxNode {
public:
    ClassMember(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Declaration* declaration;
};

class Expression : public Statement {
public:
    Expression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class CodeBlock : public Expression {
public:
    CodeBlock(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<StatementWithSemicolon>* statements;
};

class SimpleExpression : public Expression {
public:
    SimpleExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PrefixExpression* prefixExpression;
    Array<BinaryOp>* binaryOps;
};

class PrefixExpression : public SyntaxNode {
public:
    PrefixExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* prefixOperator;
    PostfixExpression* expression;
};

class PostfixExpression : public SyntaxNode {
public:
    PostfixExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PrimaryExpression* primaryExpression;
    Array<Postfix>* postfixes;
};

class BinaryOp : public SyntaxNode {
public:
    BinaryOp(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class BinaryOperation : public BinaryOp {
public:
    BinaryOperation(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* binaryOperator;
    PrefixExpression* expression;
};

class Assignment : public BinaryOp {
public:
    Assignment(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PrefixExpression* expression;
};

class TypeQuery : public BinaryOp {
public:
    TypeQuery(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* objectType;
};

class TypeCast : public BinaryOp {
public:
    TypeCast(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* objectType;
};

class CatchClause : public SyntaxNode {
public:
    CatchClause(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    CatchPattern* catchPattern;
    TuplePattern* bindingPattern;
    Expression* expression;
};

class CatchPattern : public SyntaxNode {
public:
    CatchPattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class WildCardCatchPattern : public CatchPattern {
public:
    WildCardCatchPattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    WildcardPattern* pattern;
};

class PathItemCatchPattern : public CatchPattern {
public:
    PathItemCatchPattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    PathItem* catchCase;
    Array<PathIdentifier>* catchCaseExtensions;
};

class Postfix : public SyntaxNode {
public:
    Postfix(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class OperatorPostfix : public Postfix {
public:
    OperatorPostfix(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* postfixOperator;
};

class FunctionCall : public Postfix {
public:
    FunctionCall(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    ParenthesizedExpression* arguments;
    Array<CatchClause>* catchClauses;
};

class ExplicitMemberExpression : public Postfix {
public:
    ExplicitMemberExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    MemberPostfix* memberPostfix;
};

class Subscript : public Postfix {
public:
    Subscript(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<ExpressionElement>* expressions;
};

class ExpressionElement : public SyntaxNode {
public:
    ExpressionElement(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class MemberPostfix : public ExplicitMemberExpression {
public:
    MemberPostfix(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class NamedMemberPostfix : public MemberPostfix {
public:
    NamedMemberPostfix(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    IdentifierExpression* identifier;
};

class PrimaryExpression : public SyntaxNode {
public:
    PrimaryExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ParenthesizedExpression : public PrimaryExpression {
public:
    ParenthesizedExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<ExpressionElement>* expressionElements;
};

class LiteralExpression : public PrimaryExpression {
public:
    LiteralExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Literal* literal;
};

class IdentifierExpression : public PrimaryExpression {
public:
    IdentifierExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
};

class IfExpression : public PrimaryExpression {
public:
    IfExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* condition;
    Expression* consequent;
    ElseClause* elseClause;
};

class ElseClause : public SyntaxNode {
public:
    ElseClause(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* alternative;
};

class SwitchExpression : public PrimaryExpression {
public:
    SwitchExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
    SwitchBody* body;
};

class SwitchBody : public SyntaxNode {
public:
    SwitchBody(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class CurliedSwitchBody : public SwitchBody {
public:
    CurliedSwitchBody(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<SwitchCase>* cases;
};

class NakedSwitchBody : public SwitchBody {
public:
    NakedSwitchBody(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<SwitchCase>* cases;
};

class SwitchCase : public SyntaxNode {
public:
    SwitchCase(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    CaseLabel* label;
    CaseContent* content;
};

class CaseLabel : public SyntaxNode {
public:
    CaseLabel(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ItemCaseLabel : public CaseLabel {
public:
    ItemCaseLabel(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Pattern* pattern;
    Array<CaseItem>* additionalPatterns;
};

class CaseItem : public SyntaxNode {
public:
    CaseItem(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Pattern* pattern;
};

class ForInExpression : public PrimaryExpression {
public:
    ForInExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Pattern* pattern;
    Expression* expression;
    Expression* code;
};

class ReturnExpression : public PrimaryExpression {
public:
    ReturnExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class ThrowExpression : public PrimaryExpression {
public:
    ThrowExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class BreakExpression : public PrimaryExpression {
public:
    BreakExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class Pattern : public SyntaxNode {
public:
    Pattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class WildcardPattern : public Pattern {
public:
    WildcardPattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class IdentifierPattern : public Pattern {
public:
    IdentifierPattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* identifier;
    TypeAnnotation* typeAnnotation;
};

class TuplePattern : public Pattern {
public:
    TuplePattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<TuplePatternElement>* elements;
};

class TuplePatternElement : public SyntaxNode {
public:
    TuplePatternElement(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Pattern* pattern;
};

class ExpressionPattern : public Pattern {
public:
    ExpressionPattern(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class DefaultCaseLabel : public CaseLabel {
public:
    DefaultCaseLabel(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class CaseContent : public SyntaxNode {
public:
    CaseContent(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class BlockCaseContent : public CaseContent {
public:
    BlockCaseContent(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<StatementWithSemicolon>* statements;
};

class EmptyCaseContent : public CaseContent {
public:
    EmptyCaseContent(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class InitializerCall : public PrimaryExpression {
public:
    InitializerCall(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* typeToInitialize;
    ParenthesizedExpression* arguments;
    Array<CatchClause>* catchClauses;
};

class ThisExpression : public PrimaryExpression {
public:
    ThisExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ThisDot : public ThisExpression {
public:
    ThisDot(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    CommonThisMember* member;
};

class ThisSubscript : public ThisExpression {
public:
    ThisSubscript(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Subscript* subscript;
};

class This : public ThisExpression {
public:
    This(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class CommonThisMember : public SyntaxNode {
public:
    CommonThisMember(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ThisInit : public CommonThisMember {
public:
    ThisInit(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ThisMember : public CommonThisMember {
public:
    ThisMember(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
};

class SuperExpression : public PrimaryExpression {
public:
    SuperExpression(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class SuperDot : public SuperExpression {
public:
    SuperDot(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    CommonSuperMember* member;
};

class SuperSubscript : public SuperExpression {
public:
    SuperSubscript(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Subscript* subscript;
};

class CommonSuperMember : public SyntaxNode {
public:
    CommonSuperMember(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class SuperInit : public CommonSuperMember {
public:
    SuperInit(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class SuperMember : public CommonSuperMember {
public:
    SuperMember(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
};

class Type : public SyntaxNode {
public:
    Type(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class TypeAnnotation : public SyntaxNode {
public:
    TypeAnnotation(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* typeAnnotation;
};

class TypeIdentifier : public Type {
public:
    TypeIdentifier(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    String* name;
    SubtypeIdentifier* subType;
    Array<TypePostfix>* postfixes;
};

class SubtypeIdentifier : public SyntaxNode {
public:
    SubtypeIdentifier(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    TypeIdentifier* typeIdentifier;
};

class TypePostfix : public SyntaxNode {
public:
    TypePostfix(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class ArrayType : public Type {
public:
    ArrayType(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Type* elementType;
    Array<TypePostfix>* postfixes;
};

class OptionalType : public TypePostfix {
public:
    OptionalType(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
};

class TypeInheritanceClause : public SyntaxNode {
public:
    TypeInheritanceClause(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    Array<Inheritance>* inheritances;
};

class Inheritance : public SyntaxNode {
public:
    Inheritance(Position start, Position end);

    virtual void Accept(SyntaxVisitor& visitor);
    TypeIdentifier* name;
};

}
#endif // __scalyc__Syntax__
