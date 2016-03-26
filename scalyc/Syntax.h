#ifndef __scalyc__Syntax__
#define __scalyc__Syntax__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class SyntaxNode : public Object {
public:
    SyntaxNode(Position* start, Position* end);
    Position* start;
    Position* end;
    SyntaxNode* parent;
    virtual bool _isSyntaxNode();
    virtual bool _isProgram();
    virtual bool _isCompilationUnit();
    virtual bool _isStatementWithSemicolon();
    virtual bool _isStatement();
    virtual bool _isDeclaration();
    virtual bool _isExpression();
    virtual bool _isUseDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isInitializerDeclaration();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
    virtual bool _isPathIdentifier();
    virtual bool _isPathItem();
    virtual bool _isInitializer();
    virtual bool _isBindingInitializer();
    virtual bool _isPatternInitializer();
    virtual bool _isAdditionalInitializer();
    virtual bool _isModifier();
    virtual bool _isOverrideWord();
    virtual bool _isStaticWord();
    virtual bool _isFunctionName();
    virtual bool _isIdentifierFunction();
    virtual bool _isFunctionSignature();
    virtual bool _isFunctionResult();
    virtual bool _isParameterClause();
    virtual bool _isParameter();
    virtual bool _isConstParameter();
    virtual bool _isVarParameter();
    virtual bool _isThrowsClause();
    virtual bool _isEnumMember();
    virtual bool _isEnumCase();
    virtual bool _isAdditionalCase();
    virtual bool _isClassBody();
    virtual bool _isGenericArgumentClause();
    virtual bool _isGenericParameter();
    virtual bool _isClassMember();
    virtual bool _isPrefixExpression();
    virtual bool _isPostfixExpression();
    virtual bool _isBinaryOp();
    virtual bool _isBinaryOperation();
    virtual bool _isAssignment();
    virtual bool _isTypeQuery();
    virtual bool _isTypeCast();
    virtual bool _isCatchClause();
    virtual bool _isCatchPattern();
    virtual bool _isWildCardCatchPattern();
    virtual bool _isPathItemCatchPattern();
    virtual bool _isPostfix();
    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isExplicitMemberExpression();
    virtual bool _isSubscript();
    virtual bool _isExpressionElement();
    virtual bool _isMemberPostfix();
    virtual bool _isNamedMemberPostfix();
    virtual bool _isPrimaryExpression();
    virtual bool _isIdentifierExpression();
    virtual bool _isLiteralExpression();
    virtual bool _isIfExpression();
    virtual bool _isSwitchExpression();
    virtual bool _isForExpression();
    virtual bool _isWhileExpression();
    virtual bool _isRepeatExpression();
    virtual bool _isParenthesizedExpression();
    virtual bool _isReturnExpression();
    virtual bool _isThrowExpression();
    virtual bool _isBreakExpression();
    virtual bool _isInitializerCall();
    virtual bool _isThisExpression();
    virtual bool _isThisDot();
    virtual bool _isThisSubscript();
    virtual bool _isThisWord();
    virtual bool _isSuperExpression();
    virtual bool _isSuperDot();
    virtual bool _isSuperSubscript();
    virtual bool _isElseClause();
    virtual bool _isSwitchBody();
    virtual bool _isCurliedSwitchBody();
    virtual bool _isNakedSwitchBody();
    virtual bool _isSwitchCase();
    virtual bool _isCaseLabel();
    virtual bool _isItemCaseLabel();
    virtual bool _isDefaultCaseLabel();
    virtual bool _isCaseItem();
    virtual bool _isPattern();
    virtual bool _isWildcardPattern();
    virtual bool _isIdentifierPattern();
    virtual bool _isTuplePattern();
    virtual bool _isExpressionPattern();
    virtual bool _isTuplePatternElement();
    virtual bool _isCaseContent();
    virtual bool _isBlockCaseContent();
    virtual bool _isEmptyCaseContent();
    virtual bool _isCommonThisMember();
    virtual bool _isThisInit();
    virtual bool _isThisMember();
    virtual bool _isCommonSuperMember();
    virtual bool _isSuperInit();
    virtual bool _isSuperMember();
    virtual bool _isType();
    virtual bool _isTypeIdentifier();
    virtual bool _isArrayType();
    virtual bool _isTypeAnnotation();
    virtual bool _isSubtypeIdentifier();
    virtual bool _isTypePostfix();
    virtual bool _isOptionalType();
    virtual bool _isTypeInheritanceClause();
    virtual bool _isInheritance();
};

class Program : public SyntaxNode {
public:
    Program(_LetString* name, _LetString* directory, _Vector<CompilationUnit>* compilationUnits);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;
    _LetString* directory;
    _Vector<CompilationUnit>* compilationUnits;

    virtual bool _isProgram();
};

class CompilationUnit : public SyntaxNode {
public:
    CompilationUnit(_Vector<StatementWithSemicolon>* statements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<StatementWithSemicolon>* statements;
    _LetString* fileName;

    virtual bool _isCompilationUnit();
};

class StatementWithSemicolon : public SyntaxNode {
public:
    StatementWithSemicolon(Statement* statement, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Statement* statement;

    virtual bool _isStatementWithSemicolon();
};

class Statement : public SyntaxNode {
public:
    Statement(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isStatement();
    virtual bool _isDeclaration();
    virtual bool _isExpression();
    virtual bool _isUseDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isInitializerDeclaration();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
};

class Declaration : public Statement {
public:
    Declaration(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isDeclaration();
    virtual bool _isUseDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isInitializerDeclaration();
};

class Expression : public Statement {
public:
    Expression(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isExpression();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
};

class UseDeclaration : public Declaration {
public:
    UseDeclaration(PathItem* importModule, _Vector<PathIdentifier>* importExtensions, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PathItem* importModule;
    _Vector<PathIdentifier>* importExtensions;

    virtual bool _isUseDeclaration();
};

class ConstantDeclaration : public Declaration {
public:
    ConstantDeclaration(BindingInitializer* initializer, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    BindingInitializer* initializer;

    virtual bool _isConstantDeclaration();
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(BindingInitializer* initializer, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    BindingInitializer* initializer;

    virtual bool _isVariableDeclaration();
};

class MutableDeclaration : public Declaration {
public:
    MutableDeclaration(BindingInitializer* initializer, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    BindingInitializer* initializer;

    virtual bool _isMutableDeclaration();
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(_Vector<Modifier>* modifiers, FunctionName* name, FunctionSignature* signature, Expression* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<Modifier>* modifiers;
    FunctionName* name;
    FunctionSignature* signature;
    Expression* body;

    virtual bool _isFunctionDeclaration();
};

class EnumDeclaration : public Declaration {
public:
    EnumDeclaration(_LetString* name, _Vector<EnumMember>* members, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;
    _Vector<EnumMember>* members;

    virtual bool _isEnumDeclaration();
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(_LetString* name, GenericArgumentClause* genericArgumentClause, TypeInheritanceClause* typeInheritanceClause, ClassBody* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;
    GenericArgumentClause* genericArgumentClause;
    TypeInheritanceClause* typeInheritanceClause;
    ClassBody* body;

    virtual bool _isClassDeclaration();
};

class InitializerDeclaration : public Declaration {
public:
    InitializerDeclaration(_Vector<Modifier>* modifiers, ParameterClause* parameterClause, ThrowsClause* throwsClause, Expression* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<Modifier>* modifiers;
    ParameterClause* parameterClause;
    ThrowsClause* throwsClause;
    Expression* body;

    virtual bool _isInitializerDeclaration();
};

class CodeBlock : public Expression {
public:
    CodeBlock(_Vector<StatementWithSemicolon>* statements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<StatementWithSemicolon>* statements;

    virtual bool _isCodeBlock();
};

class SimpleExpression : public Expression {
public:
    SimpleExpression(PrefixExpression* prefixExpression, _Vector<BinaryOp>* binaryOps, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PrefixExpression* prefixExpression;
    _Vector<BinaryOp>* binaryOps;

    virtual bool _isSimpleExpression();
};

class PathIdentifier : public SyntaxNode {
public:
    PathIdentifier(PathItem* extension, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PathItem* extension;

    virtual bool _isPathIdentifier();
};

class PathItem : public SyntaxNode {
public:
    PathItem(_LetString* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;

    virtual bool _isPathItem();
};

class Initializer : public SyntaxNode {
public:
    Initializer(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isInitializer();
};

class BindingInitializer : public SyntaxNode {
public:
    BindingInitializer(PatternInitializer* initializer, _Vector<AdditionalInitializer>* additionalInitializers, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PatternInitializer* initializer;
    _Vector<AdditionalInitializer>* additionalInitializers;

    virtual bool _isBindingInitializer();
};

class PatternInitializer : public SyntaxNode {
public:
    PatternInitializer(Pattern* pattern, Initializer* initializer, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Pattern* pattern;
    Initializer* initializer;

    virtual bool _isPatternInitializer();
};

class AdditionalInitializer : public SyntaxNode {
public:
    AdditionalInitializer(PatternInitializer* pattern, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PatternInitializer* pattern;

    virtual bool _isAdditionalInitializer();
};

class Modifier : public SyntaxNode {
public:
    Modifier(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isModifier();
    virtual bool _isOverrideWord();
    virtual bool _isStaticWord();
};

class OverrideWord : public Modifier {
public:
    OverrideWord(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isOverrideWord();
};

class StaticWord : public Modifier {
public:
    StaticWord(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isStaticWord();
};

class FunctionName : public SyntaxNode {
public:
    FunctionName(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isFunctionName();
    virtual bool _isIdentifierFunction();
};

class IdentifierFunction : public FunctionName {
public:
    IdentifierFunction(_LetString* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;

    virtual bool _isIdentifierFunction();
};

class FunctionSignature : public SyntaxNode {
public:
    FunctionSignature(ParameterClause* parameterClause, FunctionResult* result, ThrowsClause* throwsClause, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    ParameterClause* parameterClause;
    FunctionResult* result;
    ThrowsClause* throwsClause;

    virtual bool _isFunctionSignature();
};

class FunctionResult : public SyntaxNode {
public:
    FunctionResult(Type* resultType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* resultType;

    virtual bool _isFunctionResult();
};

class ParameterClause : public SyntaxNode {
public:
    ParameterClause(_Vector<Parameter>* parameters, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<Parameter>* parameters;

    virtual bool _isParameterClause();
};

class Parameter : public SyntaxNode {
public:
    Parameter(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isParameter();
    virtual bool _isConstParameter();
    virtual bool _isVarParameter();
};

class ConstParameter : public Parameter {
public:
    ConstParameter(_LetString* name, Type* parameterType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;
    Type* parameterType;

    virtual bool _isConstParameter();
};

class VarParameter : public Parameter {
public:
    VarParameter(_LetString* name, Type* parameterType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;
    Type* parameterType;

    virtual bool _isVarParameter();
};

class ThrowsClause : public SyntaxNode {
public:
    ThrowsClause(Type* throwsType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* throwsType;

    virtual bool _isThrowsClause();
};

class EnumMember : public SyntaxNode {
public:
    EnumMember(EnumCase* enumCase, _Vector<AdditionalCase>* additionalCases, ParameterClause* parameterClause, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    EnumCase* enumCase;
    _Vector<AdditionalCase>* additionalCases;
    ParameterClause* parameterClause;

    virtual bool _isEnumMember();
};

class EnumCase : public SyntaxNode {
public:
    EnumCase(_LetString* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;

    virtual bool _isEnumCase();
};

class AdditionalCase : public SyntaxNode {
public:
    AdditionalCase(EnumCase* enumCase, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    EnumCase* enumCase;

    virtual bool _isAdditionalCase();
};

class ClassBody : public SyntaxNode {
public:
    ClassBody(_Vector<ClassMember>* members, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<ClassMember>* members;

    virtual bool _isClassBody();
};

class GenericArgumentClause : public SyntaxNode {
public:
    GenericArgumentClause(_Vector<GenericParameter>* genericParameters, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<GenericParameter>* genericParameters;

    virtual bool _isGenericArgumentClause();
};

class GenericParameter : public SyntaxNode {
public:
    GenericParameter(_LetString* typeName, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* typeName;

    virtual bool _isGenericParameter();
};

class ClassMember : public SyntaxNode {
public:
    ClassMember(Declaration* declaration, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Declaration* declaration;

    virtual bool _isClassMember();
};

class PrefixExpression : public SyntaxNode {
public:
    PrefixExpression(_LetString* prefixOperator, PostfixExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* prefixOperator;
    PostfixExpression* expression;

    virtual bool _isPrefixExpression();
};

class PostfixExpression : public SyntaxNode {
public:
    PostfixExpression(PrimaryExpression* primaryExpression, _Vector<Postfix>* postfixes, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PrimaryExpression* primaryExpression;
    _Vector<Postfix>* postfixes;

    virtual bool _isPostfixExpression();
};

class BinaryOp : public SyntaxNode {
public:
    BinaryOp(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isBinaryOp();
    virtual bool _isBinaryOperation();
    virtual bool _isAssignment();
    virtual bool _isTypeQuery();
    virtual bool _isTypeCast();
};

class BinaryOperation : public BinaryOp {
public:
    BinaryOperation(_LetString* binaryOperator, PrefixExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* binaryOperator;
    PrefixExpression* expression;

    virtual bool _isBinaryOperation();
};

class Assignment : public BinaryOp {
public:
    Assignment(PrefixExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PrefixExpression* expression;

    virtual bool _isAssignment();
};

class TypeQuery : public BinaryOp {
public:
    TypeQuery(Type* objectType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* objectType;

    virtual bool _isTypeQuery();
};

class TypeCast : public BinaryOp {
public:
    TypeCast(Type* objectType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* objectType;

    virtual bool _isTypeCast();
};

class CatchClause : public SyntaxNode {
public:
    CatchClause(CatchPattern* catchPattern, TuplePattern* bindingPattern, Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    CatchPattern* catchPattern;
    TuplePattern* bindingPattern;
    Expression* expression;

    virtual bool _isCatchClause();
};

class CatchPattern : public SyntaxNode {
public:
    CatchPattern(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCatchPattern();
    virtual bool _isWildCardCatchPattern();
    virtual bool _isPathItemCatchPattern();
};

class WildCardCatchPattern : public CatchPattern {
public:
    WildCardCatchPattern(WildcardPattern* pattern, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    WildcardPattern* pattern;

    virtual bool _isWildCardCatchPattern();
};

class PathItemCatchPattern : public CatchPattern {
public:
    PathItemCatchPattern(PathItem* catchCase, _Vector<PathIdentifier>* catchCaseExtensions, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PathItem* catchCase;
    _Vector<PathIdentifier>* catchCaseExtensions;

    virtual bool _isPathItemCatchPattern();
};

class Postfix : public SyntaxNode {
public:
    Postfix(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isPostfix();
    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isExplicitMemberExpression();
    virtual bool _isSubscript();
};

class OperatorPostfix : public Postfix {
public:
    OperatorPostfix(_LetString* postfixOperator, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* postfixOperator;

    virtual bool _isOperatorPostfix();
};

class FunctionCall : public Postfix {
public:
    FunctionCall(ParenthesizedExpression* arguments, _Vector<CatchClause>* catchClauses, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    ParenthesizedExpression* arguments;
    _Vector<CatchClause>* catchClauses;

    virtual bool _isFunctionCall();
};

class ExplicitMemberExpression : public Postfix {
public:
    ExplicitMemberExpression(MemberPostfix* memberPostfix, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    MemberPostfix* memberPostfix;

    virtual bool _isExplicitMemberExpression();
};

class Subscript : public Postfix {
public:
    Subscript(_Vector<ExpressionElement>* expressions, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<ExpressionElement>* expressions;

    virtual bool _isSubscript();
};

class ExpressionElement : public SyntaxNode {
public:
    ExpressionElement(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isExpressionElement();
};

class MemberPostfix : public SyntaxNode {
public:
    MemberPostfix(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isMemberPostfix();
    virtual bool _isNamedMemberPostfix();
};

class NamedMemberPostfix : public MemberPostfix {
public:
    NamedMemberPostfix(IdentifierExpression* identifier, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    IdentifierExpression* identifier;

    virtual bool _isNamedMemberPostfix();
};

class PrimaryExpression : public SyntaxNode {
public:
    PrimaryExpression(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isPrimaryExpression();
    virtual bool _isIdentifierExpression();
    virtual bool _isLiteralExpression();
    virtual bool _isIfExpression();
    virtual bool _isSwitchExpression();
    virtual bool _isForExpression();
    virtual bool _isWhileExpression();
    virtual bool _isRepeatExpression();
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

class IdentifierExpression : public PrimaryExpression {
public:
    IdentifierExpression(_LetString* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;

    virtual bool _isIdentifierExpression();
};

class LiteralExpression : public PrimaryExpression {
public:
    LiteralExpression(Literal* literal, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Literal* literal;

    virtual bool _isLiteralExpression();
};

class IfExpression : public PrimaryExpression {
public:
    IfExpression(Expression* condition, Expression* consequent, ElseClause* elseClause, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* condition;
    Expression* consequent;
    ElseClause* elseClause;

    virtual bool _isIfExpression();
};

class SwitchExpression : public PrimaryExpression {
public:
    SwitchExpression(Expression* expression, SwitchBody* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;
    SwitchBody* body;

    virtual bool _isSwitchExpression();
};

class ForExpression : public PrimaryExpression {
public:
    ForExpression(Pattern* pattern, Expression* expression, Expression* code, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Pattern* pattern;
    Expression* expression;
    Expression* code;

    virtual bool _isForExpression();
};

class WhileExpression : public PrimaryExpression {
public:
    WhileExpression(Expression* condition, Expression* code, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* condition;
    Expression* code;

    virtual bool _isWhileExpression();
};

class RepeatExpression : public PrimaryExpression {
public:
    RepeatExpression(Expression* condition, Expression* code, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* condition;
    Expression* code;

    virtual bool _isRepeatExpression();
};

class ParenthesizedExpression : public PrimaryExpression {
public:
    ParenthesizedExpression(_Vector<ExpressionElement>* expressionElements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<ExpressionElement>* expressionElements;

    virtual bool _isParenthesizedExpression();
};

class ReturnExpression : public PrimaryExpression {
public:
    ReturnExpression(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isReturnExpression();
};

class ThrowExpression : public PrimaryExpression {
public:
    ThrowExpression(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isThrowExpression();
};

class BreakExpression : public PrimaryExpression {
public:
    BreakExpression(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isBreakExpression();
};

class InitializerCall : public PrimaryExpression {
public:
    InitializerCall(Type* typeToInitialize, ParenthesizedExpression* arguments, _Vector<CatchClause>* catchClauses, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* typeToInitialize;
    ParenthesizedExpression* arguments;
    _Vector<CatchClause>* catchClauses;

    virtual bool _isInitializerCall();
};

class ThisExpression : public PrimaryExpression {
public:
    ThisExpression(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isThisExpression();
    virtual bool _isThisDot();
    virtual bool _isThisSubscript();
    virtual bool _isThisWord();
};

class ThisDot : public ThisExpression {
public:
    ThisDot(CommonThisMember* member, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    CommonThisMember* member;

    virtual bool _isThisDot();
};

class ThisSubscript : public ThisExpression {
public:
    ThisSubscript(Subscript* subscript, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Subscript* subscript;

    virtual bool _isThisSubscript();
};

class ThisWord : public ThisExpression {
public:
    ThisWord(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isThisWord();
};

class SuperExpression : public PrimaryExpression {
public:
    SuperExpression(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isSuperExpression();
    virtual bool _isSuperDot();
    virtual bool _isSuperSubscript();
};

class SuperDot : public SuperExpression {
public:
    SuperDot(CommonSuperMember* member, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    CommonSuperMember* member;

    virtual bool _isSuperDot();
};

class SuperSubscript : public SuperExpression {
public:
    SuperSubscript(Subscript* subscript, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Subscript* subscript;

    virtual bool _isSuperSubscript();
};

class ElseClause : public SyntaxNode {
public:
    ElseClause(Expression* alternative, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* alternative;

    virtual bool _isElseClause();
};

class SwitchBody : public SyntaxNode {
public:
    SwitchBody(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isSwitchBody();
    virtual bool _isCurliedSwitchBody();
    virtual bool _isNakedSwitchBody();
};

class CurliedSwitchBody : public SwitchBody {
public:
    CurliedSwitchBody(_Vector<SwitchCase>* cases, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<SwitchCase>* cases;

    virtual bool _isCurliedSwitchBody();
};

class NakedSwitchBody : public SwitchBody {
public:
    NakedSwitchBody(_Vector<SwitchCase>* cases, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<SwitchCase>* cases;

    virtual bool _isNakedSwitchBody();
};

class SwitchCase : public SyntaxNode {
public:
    SwitchCase(CaseLabel* label, CaseContent* content, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    CaseLabel* label;
    CaseContent* content;

    virtual bool _isSwitchCase();
};

class CaseLabel : public SyntaxNode {
public:
    CaseLabel(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCaseLabel();
    virtual bool _isItemCaseLabel();
    virtual bool _isDefaultCaseLabel();
};

class ItemCaseLabel : public CaseLabel {
public:
    ItemCaseLabel(Pattern* pattern, _Vector<CaseItem>* additionalPatterns, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Pattern* pattern;
    _Vector<CaseItem>* additionalPatterns;

    virtual bool _isItemCaseLabel();
};

class DefaultCaseLabel : public CaseLabel {
public:
    DefaultCaseLabel(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isDefaultCaseLabel();
};

class CaseItem : public SyntaxNode {
public:
    CaseItem(Pattern* pattern, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Pattern* pattern;

    virtual bool _isCaseItem();
};

class Pattern : public SyntaxNode {
public:
    Pattern(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isPattern();
    virtual bool _isWildcardPattern();
    virtual bool _isIdentifierPattern();
    virtual bool _isTuplePattern();
    virtual bool _isExpressionPattern();
};

class WildcardPattern : public Pattern {
public:
    WildcardPattern(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isWildcardPattern();
};

class IdentifierPattern : public Pattern {
public:
    IdentifierPattern(_LetString* identifier, TypeAnnotation* annotationForType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* identifier;
    TypeAnnotation* annotationForType;

    virtual bool _isIdentifierPattern();
};

class TuplePattern : public Pattern {
public:
    TuplePattern(_Vector<TuplePatternElement>* elements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<TuplePatternElement>* elements;

    virtual bool _isTuplePattern();
};

class ExpressionPattern : public Pattern {
public:
    ExpressionPattern(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isExpressionPattern();
};

class TuplePatternElement : public SyntaxNode {
public:
    TuplePatternElement(Pattern* pattern, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Pattern* pattern;

    virtual bool _isTuplePatternElement();
};

class CaseContent : public SyntaxNode {
public:
    CaseContent(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCaseContent();
    virtual bool _isBlockCaseContent();
    virtual bool _isEmptyCaseContent();
};

class BlockCaseContent : public CaseContent {
public:
    BlockCaseContent(_Vector<StatementWithSemicolon>* statements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<StatementWithSemicolon>* statements;

    virtual bool _isBlockCaseContent();
};

class EmptyCaseContent : public CaseContent {
public:
    EmptyCaseContent(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isEmptyCaseContent();
};

class CommonThisMember : public SyntaxNode {
public:
    CommonThisMember(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCommonThisMember();
    virtual bool _isThisInit();
    virtual bool _isThisMember();
};

class ThisInit : public CommonThisMember {
public:
    ThisInit(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isThisInit();
};

class ThisMember : public CommonThisMember {
public:
    ThisMember(_LetString* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;

    virtual bool _isThisMember();
};

class CommonSuperMember : public SyntaxNode {
public:
    CommonSuperMember(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCommonSuperMember();
    virtual bool _isSuperInit();
    virtual bool _isSuperMember();
};

class SuperInit : public CommonSuperMember {
public:
    SuperInit(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isSuperInit();
};

class SuperMember : public CommonSuperMember {
public:
    SuperMember(_LetString* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;

    virtual bool _isSuperMember();
};

class Type : public SyntaxNode {
public:
    Type(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isType();
    virtual bool _isArrayType();
    virtual bool _isTypeIdentifier();
};

class TypeIdentifier : public Type {
public:
    TypeIdentifier(_LetString* name, SubtypeIdentifier* subType, _Vector<TypePostfix>* postfixes, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _LetString* name;
    SubtypeIdentifier* subType;
    _Vector<TypePostfix>* postfixes;

    virtual bool _isTypeIdentifier();
};

class ArrayType : public Type {
public:
    ArrayType(Type* elementType, _Vector<TypePostfix>* postfixes, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* elementType;
    _Vector<TypePostfix>* postfixes;

    virtual bool _isArrayType();
};

class TypeAnnotation : public SyntaxNode {
public:
    TypeAnnotation(Type* annotationForType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* annotationForType;

    virtual bool _isTypeAnnotation();
};

class SubtypeIdentifier : public SyntaxNode {
public:
    SubtypeIdentifier(TypeIdentifier* typeIdentifier, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    TypeIdentifier* typeIdentifier;

    virtual bool _isSubtypeIdentifier();
};

class TypePostfix : public SyntaxNode {
public:
    TypePostfix(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isTypePostfix();
    virtual bool _isOptionalType();
};

class OptionalType : public TypePostfix {
public:
    OptionalType(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isOptionalType();
};

class TypeInheritanceClause : public SyntaxNode {
public:
    TypeInheritanceClause(_Vector<Inheritance>* inheritances, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Vector<Inheritance>* inheritances;

    virtual bool _isTypeInheritanceClause();
};

class Inheritance : public SyntaxNode {
public:
    Inheritance(TypeIdentifier* typeIdentifier, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    TypeIdentifier* typeIdentifier;

    virtual bool _isInheritance();
};

}
#endif // __scalyc__Syntax__
