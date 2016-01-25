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

class TopLevelDeclaration;
class StatementWithSemicolon;
class Statement;
class Declaration;
class UseDeclaration;
class PathIdentifier;
class PathItem;
class Initializer;
class ConstantDeclaration;
class VariableDeclaration;
class BindingInitializer;
class PatternInitializer;
class AdditionalInitializer;
class FunctionDeclaration;
class InitializerDeclaration;
class Modifier;
class Override;
class Static;
class FunctionName;
class IdentifierFunctionName;
class FunctionSignature;
class FunctionResult;
class ParameterClause;
class Parameter;
class ConstParameter;
class VarParameter;
class ThrowsClause;
class EnumDeclaration;
class EnumMember;
class TupleType;
class AdditionalType;
class EnumCase;
class AdditionalCase;
class ClassDeclaration;
class GenericArgumentClause;
class GenericParameter;
class ClassMember;
class Expression;
class CodeBlock;
class SimpleExpression;
class PrefixExpression;
class PostfixExpression;
class BinaryOp;
class BinaryOperation;
class Assignment;
class TypeQuery;
class TypeCast;
class CatchClause;
class CatchPattern;
class WildCardCatchPattern;
class PathItemCatchPattern;
class Postfix;
class OperatorPostfix;
class FunctionCall;
class ExplicitMemberExpression;
class Subscript;
class ExpressionElement;
class MemberPostfix;
class NamedMemberPostfix;
class PrimaryExpression;
class ParenthesizedExpression;
class LiteralExpression;
class IdentifierExpression;
class IfExpression;
class ElseClause;
class SwitchExpression;
class SwitchBody;
class CurliedSwitchBody;
class NakedSwitchBody;
class SwitchCase;
class CaseLabel;
class ItemCaseLabel;
class CaseItem;
class ForInExpression;
class ReturnExpression;
class ThrowExpression;
class BreakExpression;
class Pattern;
class WildcardPattern;
class IdentifierPattern;
class TuplePattern;
class TuplePatternElement;
class ExpressionPattern;
class DefaultCaseLabel;
class CaseContent;
class BlockCaseContent;
class EmptyCaseContent;
class InitializerCall;
class ThisExpression;
class ThisDot;
class ThisSubscript;
class This;
class CommonThisMember;
class ThisInit;
class ThisMember;
class SuperExpression;
class SuperDot;
class SuperSubscript;
class CommonSuperMember;
class SuperInit;
class SuperMember;
class Type;
class TypeAnnotation;
class TypeIdentifier;
class SubtypeIdentifier;
class TypePostfix;
class ArrayType;
class OptionalType;
class TypeInheritanceClause;
class Inheritance;

class TopLevelDeclaration : public SyntaxNode {
public:
    TopLevelDeclaration(Position start, Position end);

    Array<StatementWithSemicolon>* statements;
};

class StatementWithSemicolon : public SyntaxNode {
public:
    StatementWithSemicolon(Position start, Position end);

    Statement* statement;
};

class Statement : public SyntaxNode {
public:
    Statement(Position start, Position end);

};

class Declaration : public Statement {
public:
    Declaration(Position start, Position end);

};

class UseDeclaration : public Declaration {
public:
    UseDeclaration(Position start, Position end);

    PathItem* importModule;
    Array<PathIdentifier>* importExtensions;
};

class PathIdentifier : public SyntaxNode {
public:
    PathIdentifier(Position start, Position end);

    PathItem* extension;
};

class PathItem : public SyntaxNode {
public:
    PathItem(Position start, Position end);

    String* name;
};

class Initializer : public SyntaxNode {
public:
    Initializer(Position start, Position end);

    Expression* expression;
};

class ConstantDeclaration : public Declaration {
public:
    ConstantDeclaration(Position start, Position end);

    BindingInitializer* initializer;
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(Position start, Position end);

    BindingInitializer* initializer;
};

class BindingInitializer : public SyntaxNode {
public:
    BindingInitializer(Position start, Position end);

    PatternInitializer* initializer;
    Array<AdditionalInitializer>* additionalInitializers;
};

class PatternInitializer : public SyntaxNode {
public:
    PatternInitializer(Position start, Position end);

    Pattern* pattern;
    Initializer* initializer;
};

class AdditionalInitializer : public SyntaxNode {
public:
    AdditionalInitializer(Position start, Position end);

    PatternInitializer* pattern;
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(Position start, Position end);

    Array<Modifier>* modifiers;
    FunctionName* name;
    FunctionSignature* signature;
    Expression* body;
};

class InitializerDeclaration : public Declaration {
public:
    InitializerDeclaration(Position start, Position end);

    Array<Modifier>* modifiers;
    ParameterClause* parameterClause;
    ThrowsClause* throwsClause;
    Expression* body;
};

class Modifier : public SyntaxNode {
public:
    Modifier(Position start, Position end);

};

class Override : public Modifier {
public:
    Override(Position start, Position end);

};

class Static : public Modifier {
public:
    Static(Position start, Position end);

};

class FunctionName : public SyntaxNode {
public:
    FunctionName(Position start, Position end);

};

class IdentifierFunctionName : public FunctionName {
public:
    IdentifierFunctionName(Position start, Position end);

    String* name;
};

class FunctionSignature : public SyntaxNode {
public:
    FunctionSignature(Position start, Position end);

    ParameterClause* parameterClause;
    FunctionResult* result;
    ThrowsClause* throwsClause;
};

class FunctionResult : public SyntaxNode {
public:
    FunctionResult(Position start, Position end);

    Type* resultType;
};

class ParameterClause : public SyntaxNode {
public:
    ParameterClause(Position start, Position end);

    Array<Parameter>* parameters;
};

class Parameter : public SyntaxNode {
public:
    Parameter(Position start, Position end);

};

class ConstParameter : public Parameter {
public:
    ConstParameter(Position start, Position end);

    String* parameterName;
    Type* parameterType;
};

class VarParameter : public Parameter {
public:
    VarParameter(Position start, Position end);

    String* parameterName;
    Type* parameterType;
};

class ThrowsClause : public SyntaxNode {
public:
    ThrowsClause(Position start, Position end);

    Type* throwsType;
};

class EnumDeclaration : public Declaration {
public:
    EnumDeclaration(Position start, Position end);

    String* name;
    Array<EnumMember>* members;
};

class EnumMember : public SyntaxNode {
public:
    EnumMember(Position start, Position end);

    EnumCase* enumCase;
    Array<AdditionalCase>* additionalCases;
    TupleType* tupleType;
};

class TupleType : public SyntaxNode {
public:
    TupleType(Position start, Position end);

    Type* tupleType;
    Array<AdditionalType>* additionalTypes;
};

class AdditionalType : public SyntaxNode {
public:
    AdditionalType(Position start, Position end);

    Type* enumCase;
};

class EnumCase : public SyntaxNode {
public:
    EnumCase(Position start, Position end);

    String* name;
};

class AdditionalCase : public SyntaxNode {
public:
    AdditionalCase(Position start, Position end);

    EnumCase* enumCase;
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(Position start, Position end);

    String* name;
    GenericArgumentClause* genericArgumentClause;
    TypeInheritanceClause* typeInheritanceClause;
    Array<ClassMember>* members;
};

class GenericArgumentClause : public SyntaxNode {
public:
    GenericArgumentClause(Position start, Position end);

    Array<GenericParameter>* genericParameters;
};

class GenericParameter : public SyntaxNode {
public:
    GenericParameter(Position start, Position end);

    String* typeName;
};

class ClassMember : public SyntaxNode {
public:
    ClassMember(Position start, Position end);

    Declaration* declaration;
};

class Expression : public Statement {
public:
    Expression(Position start, Position end);

};

class CodeBlock : public Expression {
public:
    CodeBlock(Position start, Position end);

    Array<StatementWithSemicolon>* statements;
};

class SimpleExpression : public Expression {
public:
    SimpleExpression(Position start, Position end);

    PrefixExpression* prefixExpression;
    Array<BinaryOp>* binaryOps;
};

class PrefixExpression : public SyntaxNode {
public:
    PrefixExpression(Position start, Position end);

    String* prefixOperator;
    PostfixExpression* expression;
};

class PostfixExpression : public SyntaxNode {
public:
    PostfixExpression(Position start, Position end);

    PrimaryExpression* primaryExpression;
    Array<Postfix>* postfixes;
};

class BinaryOp : public SyntaxNode {
public:
    BinaryOp(Position start, Position end);

};

class BinaryOperation : public BinaryOp {
public:
    BinaryOperation(Position start, Position end);

    String* binaryOperator;
    PrefixExpression* expression;
};

class Assignment : public BinaryOp {
public:
    Assignment(Position start, Position end);

    PrefixExpression* expression;
};

class TypeQuery : public BinaryOp {
public:
    TypeQuery(Position start, Position end);

    Type* objectType;
};

class TypeCast : public BinaryOp {
public:
    TypeCast(Position start, Position end);

    Type* objectType;
};

class CatchClause : public SyntaxNode {
public:
    CatchClause(Position start, Position end);

    CatchPattern* catchPattern;
    TuplePattern* bindingPattern;
    Expression* expression;
};

class CatchPattern : public SyntaxNode {
public:
    CatchPattern(Position start, Position end);

};

class WildCardCatchPattern : public CatchPattern {
public:
    WildCardCatchPattern(Position start, Position end);

    WildcardPattern* pattern;
};

class PathItemCatchPattern : public CatchPattern {
public:
    PathItemCatchPattern(Position start, Position end);

    PathItem* catchCase;
    Array<PathIdentifier>* catchCaseExtensions;
};

class Postfix : public SyntaxNode {
public:
    Postfix(Position start, Position end);

};

class OperatorPostfix : public Postfix {
public:
    OperatorPostfix(Position start, Position end);

    String* postfixOperator;
};

class FunctionCall : public Postfix {
public:
    FunctionCall(Position start, Position end);

    ParenthesizedExpression* arguments;
    Array<CatchClause>* catchClauses;
};

class ExplicitMemberExpression : public Postfix {
public:
    ExplicitMemberExpression(Position start, Position end);

    MemberPostfix* memberPostfix;
};

class Subscript : public Postfix {
public:
    Subscript(Position start, Position end);

    Array<ExpressionElement>* expressions;
};

class ExpressionElement : public SyntaxNode {
public:
    ExpressionElement(Position start, Position end);

    Expression* expression;
};

class MemberPostfix : public ExplicitMemberExpression {
public:
    MemberPostfix(Position start, Position end);

};

class NamedMemberPostfix : public MemberPostfix {
public:
    NamedMemberPostfix(Position start, Position end);

    IdentifierExpression* identifier;
};

class PrimaryExpression : public SyntaxNode {
public:
    PrimaryExpression(Position start, Position end);

};

class ParenthesizedExpression : public PrimaryExpression {
public:
    ParenthesizedExpression(Position start, Position end);

    Array<ExpressionElement>* expressionElements;
};

class LiteralExpression : public PrimaryExpression {
public:
    LiteralExpression(Position start, Position end);

    Literal* literal;
};

class IdentifierExpression : public PrimaryExpression {
public:
    IdentifierExpression(Position start, Position end);

    String* name;
};

class IfExpression : public PrimaryExpression {
public:
    IfExpression(Position start, Position end);

    Expression* condition;
    Expression* consequent;
    ElseClause* elseClause;
};

class ElseClause : public SyntaxNode {
public:
    ElseClause(Position start, Position end);

    Expression* alternative;
};

class SwitchExpression : public PrimaryExpression {
public:
    SwitchExpression(Position start, Position end);

    Expression* expression;
    SwitchBody* body;
};

class SwitchBody : public SyntaxNode {
public:
    SwitchBody(Position start, Position end);

};

class CurliedSwitchBody : public SwitchBody {
public:
    CurliedSwitchBody(Position start, Position end);

    Array<SwitchCase>* cases;
};

class NakedSwitchBody : public SwitchBody {
public:
    NakedSwitchBody(Position start, Position end);

    Array<SwitchCase>* cases;
};

class SwitchCase : public SyntaxNode {
public:
    SwitchCase(Position start, Position end);

    CaseLabel* label;
    CaseContent* content;
};

class CaseLabel : public SyntaxNode {
public:
    CaseLabel(Position start, Position end);

};

class ItemCaseLabel : public CaseLabel {
public:
    ItemCaseLabel(Position start, Position end);

    Pattern* pattern;
    Array<CaseItem>* additionalPatterns;
};

class CaseItem : public SyntaxNode {
public:
    CaseItem(Position start, Position end);

    Pattern* pattern;
};

class ForInExpression : public PrimaryExpression {
public:
    ForInExpression(Position start, Position end);

    Pattern* pattern;
    Expression* expression;
    Expression* code;
};

class ReturnExpression : public PrimaryExpression {
public:
    ReturnExpression(Position start, Position end);

    Expression* expression;
};

class ThrowExpression : public PrimaryExpression {
public:
    ThrowExpression(Position start, Position end);

    Expression* expression;
};

class BreakExpression : public PrimaryExpression {
public:
    BreakExpression(Position start, Position end);

    Expression* expression;
};

class Pattern : public SyntaxNode {
public:
    Pattern(Position start, Position end);

};

class WildcardPattern : public Pattern {
public:
    WildcardPattern(Position start, Position end);

};

class IdentifierPattern : public Pattern {
public:
    IdentifierPattern(Position start, Position end);

    String* identifier;
    TypeAnnotation* typeAnnotation;
};

class TuplePattern : public Pattern {
public:
    TuplePattern(Position start, Position end);

    Array<TuplePatternElement>* elements;
};

class TuplePatternElement : public SyntaxNode {
public:
    TuplePatternElement(Position start, Position end);

    Pattern* pattern;
};

class ExpressionPattern : public Pattern {
public:
    ExpressionPattern(Position start, Position end);

    Expression* expression;
};

class DefaultCaseLabel : public CaseLabel {
public:
    DefaultCaseLabel(Position start, Position end);

};

class CaseContent : public SyntaxNode {
public:
    CaseContent(Position start, Position end);

};

class BlockCaseContent : public CaseContent {
public:
    BlockCaseContent(Position start, Position end);

    Array<StatementWithSemicolon>* statements;
};

class EmptyCaseContent : public CaseContent {
public:
    EmptyCaseContent(Position start, Position end);

};

class InitializerCall : public PrimaryExpression {
public:
    InitializerCall(Position start, Position end);

    Type* typeToInitialize;
    ParenthesizedExpression* arguments;
    Array<CatchClause>* catchClauses;
};

class ThisExpression : public PrimaryExpression {
public:
    ThisExpression(Position start, Position end);

};

class ThisDot : public ThisExpression {
public:
    ThisDot(Position start, Position end);

    CommonThisMember* member;
};

class ThisSubscript : public ThisExpression {
public:
    ThisSubscript(Position start, Position end);

    Subscript* subscript;
};

class This : public ThisExpression {
public:
    This(Position start, Position end);

};

class CommonThisMember : public SyntaxNode {
public:
    CommonThisMember(Position start, Position end);

};

class ThisInit : public CommonThisMember {
public:
    ThisInit(Position start, Position end);

};

class ThisMember : public CommonThisMember {
public:
    ThisMember(Position start, Position end);

    String* name;
};

class SuperExpression : public PrimaryExpression {
public:
    SuperExpression(Position start, Position end);

};

class SuperDot : public SuperExpression {
public:
    SuperDot(Position start, Position end);

    CommonSuperMember* member;
};

class SuperSubscript : public SuperExpression {
public:
    SuperSubscript(Position start, Position end);

    Subscript* subscript;
};

class CommonSuperMember : public SyntaxNode {
public:
    CommonSuperMember(Position start, Position end);

};

class SuperInit : public CommonSuperMember {
public:
    SuperInit(Position start, Position end);

};

class SuperMember : public CommonSuperMember {
public:
    SuperMember(Position start, Position end);

    String* name;
};

class Type : public SyntaxNode {
public:
    Type(Position start, Position end);

};

class TypeAnnotation : public SyntaxNode {
public:
    TypeAnnotation(Position start, Position end);

    Type* typeAnnotation;
};

class TypeIdentifier : public Type {
public:
    TypeIdentifier(Position start, Position end);

    String* name;
    SubtypeIdentifier* subType;
    Array<TypePostfix>* postfixes;
};

class SubtypeIdentifier : public SyntaxNode {
public:
    SubtypeIdentifier(Position start, Position end);

    TypeIdentifier* typeIdentifier;
};

class TypePostfix : public SyntaxNode {
public:
    TypePostfix(Position start, Position end);

};

class ArrayType : public Type {
public:
    ArrayType(Position start, Position end);

    Type* elementType;
    Array<TypePostfix>* postfixes;
};

class OptionalType : public TypePostfix {
public:
    OptionalType(Position start, Position end);

};

class TypeInheritanceClause : public SyntaxNode {
public:
    TypeInheritanceClause(Position start, Position end);

    Array<Inheritance>* inheritances;
};

class Inheritance : public SyntaxNode {
public:
    Inheritance(Position start, Position end);

    TypeIdentifier* name;
};

}
#endif // __scalyc__Syntax__
