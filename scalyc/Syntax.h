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
class PatternInitializer;
class VariableDeclaration;
class FunctionDeclaration;
class Modifier;
class Static;
class FunctionName;
class IdentifierFunctionName;
class FunctionSignature;
class FunctionResult;
class ParameterClause;
class Parameter;
class ThrowsClause;
class ClassDeclaration;
class GenericArgumentClause;
class GenericParameter;
class ClassBody;
class ClassMember;
class Expression;
class CodeBlock;
class SimpleExpression;
class PrefixExpression;
class PostfixExpression;
class BinaryOp;
class BinaryExpression;
class AssignmentExpression;
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
class ReturnExpression;
class ThrowExpression;
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

    Array<PatternInitializer>* patternInitializers;
};

class PatternInitializer : public SyntaxNode {
public:
    PatternInitializer(Position start, Position end);

    Pattern* pattern;
    Initializer* initializer;
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(Position start, Position end);

    Array<PatternInitializer>* patternInitializers;
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(Position start, Position end);

    Array<Modifier>* modifiers;
    FunctionName* name;
    FunctionSignature* signature;
    Expression* body;
};

class Modifier : public SyntaxNode {
public:
    Modifier(Position start, Position end);

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

    String* parameterName;
    Type* parameterType;
};

class ThrowsClause : public SyntaxNode {
public:
    ThrowsClause(Position start, Position end);

    Type* throwsType;
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(Position start, Position end);

    String* name;
    GenericArgumentClause* genericArgumentClause;
    TypeInheritanceClause* typeInheritanceClause;
    ClassBody* body;
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

class ClassBody : public SyntaxNode {
public:
    ClassBody(Position start, Position end);

    Array<ClassMember>* declarations;
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

class BinaryExpression : public BinaryOp {
public:
    BinaryExpression(Position start, Position end);

    String* binaryOperator;
    PrefixExpression* expression;
};

class AssignmentExpression : public BinaryOp {
public:
    AssignmentExpression(Position start, Position end);

    PrefixExpression* expression;
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

    ParenthesizedExpression* parenthesizedExpression;
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

    Array<CaseItem>* items;
};

class CaseItem : public SyntaxNode {
public:
    CaseItem(Position start, Position end);

    Pattern* pattern;
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
