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


class Program : public SyntaxNode {
public:
    Program();

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
    String* directory;
    Array<CompilationUnit>* compilationUnits;
};

class CompilationUnit : public SyntaxNode {
public:
    CompilationUnit(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<StatementWithSemicolon>* statements;
    String* fileName;
};

class StatementWithSemicolon : public SyntaxNode {
public:
    StatementWithSemicolon(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Statement* statement;
};

class Statement : public SyntaxNode {
public:
    Statement(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isDeclaration();
    virtual bool _isExpression();
    virtual bool _isUseDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isInitializerDeclaration();
    virtual bool _isSimpleExpression();
    virtual bool _isCodeBlock();
};

class Declaration : public Statement {
public:
    Declaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isUseDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isInitializerDeclaration();

    virtual bool _isDeclaration();
};

class UseDeclaration : public Declaration {
public:
    UseDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PathItem* importModule;
    Array<PathIdentifier>* importExtensions;

    virtual bool _isUseDeclaration();
};

class PathIdentifier : public SyntaxNode {
public:
    PathIdentifier(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PathItem* extension;
};

class PathItem : public SyntaxNode {
public:
    PathItem(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
};

class Initializer : public SyntaxNode {
public:
    Initializer(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class ConstantDeclaration : public Declaration {
public:
    ConstantDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    BindingInitializer* initializer;

    virtual bool _isConstantDeclaration();
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    BindingInitializer* initializer;

    virtual bool _isVariableDeclaration();
};

class BindingInitializer : public SyntaxNode {
public:
    BindingInitializer(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PatternInitializer* initializer;
    Array<AdditionalInitializer>* additionalInitializers;
};

class PatternInitializer : public SyntaxNode {
public:
    PatternInitializer(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Pattern* pattern;
    Initializer* initializer;
};

class AdditionalInitializer : public SyntaxNode {
public:
    AdditionalInitializer(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PatternInitializer* pattern;
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<Modifier>* modifiers;
    FunctionName* name;
    FunctionSignature* signature;
    Expression* body;

    virtual bool _isFunctionDeclaration();
};

class InitializerDeclaration : public Declaration {
public:
    InitializerDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<Modifier>* modifiers;
    ParameterClause* parameterClause;
    ThrowsClause* throwsClause;
    Expression* body;

    virtual bool _isInitializerDeclaration();
};

class Modifier : public SyntaxNode {
public:
    Modifier(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isOverride();
    virtual bool _isStaticWord();
};

class Override : public Modifier {
public:
    Override(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isOverride();
};

class StaticWord : public Modifier {
public:
    StaticWord(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isStaticWord();
};

class FunctionName : public SyntaxNode {
public:
    FunctionName(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isIdentifierFunction();
};

class IdentifierFunction : public FunctionName {
public:
    IdentifierFunction(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;

    virtual bool _isIdentifierFunction();
};

class FunctionSignature : public SyntaxNode {
public:
    FunctionSignature(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    ParameterClause* parameterClause;
    FunctionResult* result;
    ThrowsClause* throwsClause;
};

class FunctionResult : public SyntaxNode {
public:
    FunctionResult(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* resultType;
};

class ParameterClause : public SyntaxNode {
public:
    ParameterClause(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<Parameter>* parameters;
};

class Parameter : public SyntaxNode {
public:
    Parameter(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isConstParameter();
    virtual bool _isVarParameter();
};

class ConstParameter : public Parameter {
public:
    ConstParameter(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
    Type* type;

    virtual bool _isConstParameter();
};

class VarParameter : public Parameter {
public:
    VarParameter(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
    Type* parameterType;

    virtual bool _isVarParameter();
};

class ThrowsClause : public SyntaxNode {
public:
    ThrowsClause(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* throwsType;
};

class EnumDeclaration : public Declaration {
public:
    EnumDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
    Array<EnumMember>* members;

    virtual bool _isEnumDeclaration();
};

class EnumMember : public SyntaxNode {
public:
    EnumMember(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    EnumCase* enumCase;
    Array<AdditionalCase>* additionalCases;
    TupleType* tupleType;
};

class TupleType : public SyntaxNode {
public:
    TupleType(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* tupleType;
    Array<AdditionalType>* additionalTypes;
};

class AdditionalType : public SyntaxNode {
public:
    AdditionalType(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* enumCase;
};

class EnumCase : public SyntaxNode {
public:
    EnumCase(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
};

class AdditionalCase : public SyntaxNode {
public:
    AdditionalCase(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    EnumCase* enumCase;
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
    GenericArgumentClause* genericArgumentClause;
    TypeInheritanceClause* typeInheritanceClause;
    Array<ClassMember>* members;

    virtual bool _isClassDeclaration();
};

class GenericArgumentClause : public SyntaxNode {
public:
    GenericArgumentClause(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<GenericParameter>* genericParameters;
};

class GenericParameter : public SyntaxNode {
public:
    GenericParameter(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* typeName;
};

class ClassMember : public SyntaxNode {
public:
    ClassMember(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Declaration* declaration;
};

class Expression : public Statement {
public:
    Expression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isSimpleExpression();
    virtual bool _isCodeBlock();

    virtual bool _isExpression();
};

class CodeBlock : public Expression {
public:
    CodeBlock(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<StatementWithSemicolon>* statements;

    virtual bool _isCodeBlock();
};

class SimpleExpression : public Expression {
public:
    SimpleExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PrefixExpression* prefixExpression;
    Array<BinaryOp>* binaryOps;

    virtual bool _isSimpleExpression();
};

class PrefixExpression : public SyntaxNode {
public:
    PrefixExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* prefixOperator;
    PostfixExpression* expression;
};

class PostfixExpression : public SyntaxNode {
public:
    PostfixExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PrimaryExpression* primaryExpression;
    Array<Postfix>* postfixes;
};

class BinaryOp : public SyntaxNode {
public:
    BinaryOp(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isBinaryOperation();
    virtual bool _isAssignment();
    virtual bool _isTypeQuery();
    virtual bool _isTypeCast();
};

class BinaryOperation : public BinaryOp {
public:
    BinaryOperation(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* binaryOperator;
    PrefixExpression* expression;

    virtual bool _isBinaryOperation();
};

class Assignment : public BinaryOp {
public:
    Assignment(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PrefixExpression* expression;

    virtual bool _isAssignment();
};

class TypeQuery : public BinaryOp {
public:
    TypeQuery(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* objectType;

    virtual bool _isTypeQuery();
};

class TypeCast : public BinaryOp {
public:
    TypeCast(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* objectType;

    virtual bool _isTypeCast();
};

class CatchClause : public SyntaxNode {
public:
    CatchClause(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    CatchPattern* catchPattern;
    TuplePattern* bindingPattern;
    Expression* expression;
};

class CatchPattern : public SyntaxNode {
public:
    CatchPattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isWildCardCatchPattern();
    virtual bool _isPathItemCatchPattern();
};

class WildCardCatchPattern : public CatchPattern {
public:
    WildCardCatchPattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    WildcardPattern* pattern;

    virtual bool _isWildCardCatchPattern();
};

class PathItemCatchPattern : public CatchPattern {
public:
    PathItemCatchPattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    PathItem* catchCase;
    Array<PathIdentifier>* catchCaseExtensions;

    virtual bool _isPathItemCatchPattern();
};

class Postfix : public SyntaxNode {
public:
    Postfix(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isExplicitMemberExpression();
    virtual bool _isSubscript();
};

class OperatorPostfix : public Postfix {
public:
    OperatorPostfix(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* postfixOperator;

    virtual bool _isOperatorPostfix();
};

class FunctionCall : public Postfix {
public:
    FunctionCall(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    ParenthesizedExpression* arguments;
    Array<CatchClause>* catchClauses;

    virtual bool _isFunctionCall();
};

class ExplicitMemberExpression : public Postfix {
public:
    ExplicitMemberExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    MemberPostfix* memberPostfix;

    virtual bool _isExplicitMemberExpression();
};

class Subscript : public Postfix {
public:
    Subscript(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<ExpressionElement>* expressions;

    virtual bool _isSubscript();
};

class ExpressionElement : public SyntaxNode {
public:
    ExpressionElement(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;
};

class MemberPostfix : public ExplicitMemberExpression {
public:
    MemberPostfix(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isNamedMemberPostfix();

    virtual bool _isMemberPostfix();
};

class NamedMemberPostfix : public MemberPostfix {
public:
    NamedMemberPostfix(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    IdentifierExpression* identifier;

    virtual bool _isNamedMemberPostfix();
};

class PrimaryExpression : public SyntaxNode {
public:
    PrimaryExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isIdentifierExpression();
    virtual bool _isLiteralExpression();
    virtual bool _isIfExpression();
    virtual bool _isSwitchExpression();
    virtual bool _isForExpression();
    virtual bool _isParenthesizedExpression();
    virtual bool _isReturnExpression();
    virtual bool _isThrowExpression();
    virtual bool _isBreakExpression();
    virtual bool _isInitializerCall();
    virtual bool _isThisExpression();
    virtual bool _isSuperExpression();
    virtual bool _isThisDot();
    virtual bool _isThisSubscript();
    virtual bool _isThisWord();
    virtual bool _isSuperDot();
    virtual bool _isSuperSubscript();
};

class ParenthesizedExpression : public PrimaryExpression {
public:
    ParenthesizedExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<ExpressionElement>* expressionElements;

    virtual bool _isParenthesizedExpression();
};

class LiteralExpression : public PrimaryExpression {
public:
    LiteralExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Literal* literal;

    virtual bool _isLiteralExpression();
};

class IdentifierExpression : public PrimaryExpression {
public:
    IdentifierExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;

    virtual bool _isIdentifierExpression();
};

class IfExpression : public PrimaryExpression {
public:
    IfExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* condition;
    Expression* consequent;
    ElseClause* elseClause;

    virtual bool _isIfExpression();
};

class ElseClause : public SyntaxNode {
public:
    ElseClause(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* alternative;
};

class SwitchExpression : public PrimaryExpression {
public:
    SwitchExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;
    SwitchBody* body;

    virtual bool _isSwitchExpression();
};

class SwitchBody : public SyntaxNode {
public:
    SwitchBody(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isCurliedSwitchBody();
    virtual bool _isNakedSwitchBody();
};

class CurliedSwitchBody : public SwitchBody {
public:
    CurliedSwitchBody(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<SwitchCase>* cases;

    virtual bool _isCurliedSwitchBody();
};

class NakedSwitchBody : public SwitchBody {
public:
    NakedSwitchBody(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<SwitchCase>* cases;

    virtual bool _isNakedSwitchBody();
};

class SwitchCase : public SyntaxNode {
public:
    SwitchCase(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    CaseLabel* label;
    CaseContent* content;
};

class CaseLabel : public SyntaxNode {
public:
    CaseLabel(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isItemCaseLabel();
    virtual bool _isDefaultCaseLabel();
};

class ItemCaseLabel : public CaseLabel {
public:
    ItemCaseLabel(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Pattern* pattern;
    Array<CaseItem>* additionalPatterns;

    virtual bool _isItemCaseLabel();
};

class CaseItem : public SyntaxNode {
public:
    CaseItem(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Pattern* pattern;
};

class ForExpression : public PrimaryExpression {
public:
    ForExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    ForLoop* loop;

    virtual bool _isForExpression();
};

class ForLoop : public SyntaxNode {
public:
    ForLoop(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isForEach();
    virtual bool _isPlainFor();
};

class ForEach : public ForLoop {
public:
    ForEach(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Pattern* pattern;
    Expression* expression;
    Expression* code;

    virtual bool _isForEach();
};

class PlainFor : public ForLoop {
public:
    PlainFor(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    VariableDeclaration* forInit;
    Expression* forCheck;
    Expression* forNext;
    Expression* code;

    virtual bool _isPlainFor();
};

class ReturnExpression : public PrimaryExpression {
public:
    ReturnExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;

    virtual bool _isReturnExpression();
};

class ThrowExpression : public PrimaryExpression {
public:
    ThrowExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;

    virtual bool _isThrowExpression();
};

class BreakExpression : public PrimaryExpression {
public:
    BreakExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;

    virtual bool _isBreakExpression();
};

class Pattern : public SyntaxNode {
public:
    Pattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isWildcardPattern();
    virtual bool _isIdentifierPattern();
    virtual bool _isTuplePattern();
    virtual bool _isExpressionPattern();
};

class WildcardPattern : public Pattern {
public:
    WildcardPattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isWildcardPattern();
};

class IdentifierPattern : public Pattern {
public:
    IdentifierPattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* identifier;
    TypeAnnotation* typeAnnotation;

    virtual bool _isIdentifierPattern();
};

class TuplePattern : public Pattern {
public:
    TuplePattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<TuplePatternElement>* elements;

    virtual bool _isTuplePattern();
};

class TuplePatternElement : public SyntaxNode {
public:
    TuplePatternElement(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Pattern* pattern;
};

class ExpressionPattern : public Pattern {
public:
    ExpressionPattern(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Expression* expression;

    virtual bool _isExpressionPattern();
};

class DefaultCaseLabel : public CaseLabel {
public:
    DefaultCaseLabel(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isDefaultCaseLabel();
};

class CaseContent : public SyntaxNode {
public:
    CaseContent(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isBlockCaseContent();
    virtual bool _isEmptyCaseContent();
};

class BlockCaseContent : public CaseContent {
public:
    BlockCaseContent(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<StatementWithSemicolon>* statements;

    virtual bool _isBlockCaseContent();
};

class EmptyCaseContent : public CaseContent {
public:
    EmptyCaseContent(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isEmptyCaseContent();
};

class InitializerCall : public PrimaryExpression {
public:
    InitializerCall(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* typeToInitialize;
    ParenthesizedExpression* arguments;
    Array<CatchClause>* catchClauses;

    virtual bool _isInitializerCall();
};

class ThisExpression : public PrimaryExpression {
public:
    ThisExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isThisDot();
    virtual bool _isThisSubscript();
    virtual bool _isThisWord();

    virtual bool _isThisExpression();
};

class ThisDot : public ThisExpression {
public:
    ThisDot(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    CommonThisMember* member;

    virtual bool _isThisDot();
};

class ThisSubscript : public ThisExpression {
public:
    ThisSubscript(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Subscript* subscript;

    virtual bool _isThisSubscript();
};

class ThisWord : public ThisExpression {
public:
    ThisWord(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isThisWord();
};

class CommonThisMember : public SyntaxNode {
public:
    CommonThisMember(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isThisInit();
    virtual bool _isThisMember();
};

class ThisInit : public CommonThisMember {
public:
    ThisInit(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isThisInit();
};

class ThisMember : public CommonThisMember {
public:
    ThisMember(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;

    virtual bool _isThisMember();
};

class SuperExpression : public PrimaryExpression {
public:
    SuperExpression(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isSuperDot();
    virtual bool _isSuperSubscript();

    virtual bool _isSuperExpression();
};

class SuperDot : public SuperExpression {
public:
    SuperDot(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    CommonSuperMember* member;

    virtual bool _isSuperDot();
};

class SuperSubscript : public SuperExpression {
public:
    SuperSubscript(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Subscript* subscript;

    virtual bool _isSuperSubscript();
};

class CommonSuperMember : public SyntaxNode {
public:
    CommonSuperMember(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isSuperInit();
    virtual bool _isSuperMember();
};

class SuperInit : public CommonSuperMember {
public:
    SuperInit(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isSuperInit();
};

class SuperMember : public CommonSuperMember {
public:
    SuperMember(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;

    virtual bool _isSuperMember();
};

class Type : public SyntaxNode {
public:
    Type(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isArrayType();
    virtual bool _isTypeIdentifier();
};

class TypeAnnotation : public SyntaxNode {
public:
    TypeAnnotation(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* typeAnnotation;
};

class TypeIdentifier : public Type {
public:
    TypeIdentifier(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    String* name;
    SubtypeIdentifier* subType;
    Array<TypePostfix>* postfixes;

    virtual bool _isTypeIdentifier();
};

class SubtypeIdentifier : public SyntaxNode {
public:
    SubtypeIdentifier(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    TypeIdentifier* typeIdentifier;
};

class TypePostfix : public SyntaxNode {
public:
    TypePostfix(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor) = 0;

    virtual bool _isOptionalType();
};

class ArrayType : public Type {
public:
    ArrayType(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Type* elementType;
    Array<TypePostfix>* postfixes;

    virtual bool _isArrayType();
};

class OptionalType : public TypePostfix {
public:
    OptionalType(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);

    virtual bool _isOptionalType();
};

class TypeInheritanceClause : public SyntaxNode {
public:
    TypeInheritanceClause(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    Array<Inheritance>* inheritances;
};

class Inheritance : public SyntaxNode {
public:
    Inheritance(Position start, Position end);

    virtual void accept(SyntaxVisitor& visitor);
    TypeIdentifier* typeIdentifier;
};

}
#endif // __scalyc__Syntax__
