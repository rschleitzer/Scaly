#ifndef __scalyc__Parser__
#define __scalyc__Parser__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Program;

class CompilationUnit;

class Statement;

class Declaration;

class Expression;

class ConstantDeclaration;

class VariableDeclaration;

class MutableDeclaration;

class FunctionDeclaration;

class EnumDeclaration;

class ClassDeclaration;

class ConstructorDeclaration;

class CodeBlock;

class SimpleExpression;

class Initializer;

class BindingInitializer;

class PatternInitializer;

class AdditionalInitializer;

class Modifier;

class OverrideWord;

class StaticWord;

class FunctionSignature;

class FunctionResult;

class ParameterClause;

class Parameter;

class ConstParameter;

class VarParameter;

class ThrowsClause;

class EnumMember;

class EnumCase;

class AdditionalCase;

class ClassBody;

class ClassMember;

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

class IdentifierCatchPattern;

class Postfix;

class OperatorPostfix;

class FunctionCall;

class MemberExpression;

class Subscript;

class ExpressionElement;

class PrimaryExpression;

class IdentifierExpression;

class LiteralExpression;

class IfExpression;

class SwitchExpression;

class ForExpression;

class WhileExpression;

class DoExpression;

class ParenthesizedExpression;

class ReturnExpression;

class ThrowExpression;

class BreakExpression;

class ConstructorCall;

class ThisExpression;

class NullExpression;

class ElseClause;

class SwitchBody;

class CurliedSwitchBody;

class NakedSwitchBody;

class SwitchCase;

class CaseLabel;

class ItemCaseLabel;

class DefaultCaseLabel;

class CaseItem;

class Pattern;

class WildcardPattern;

class IdentifierPattern;

class TuplePattern;

class ExpressionPattern;

class TuplePatternElement;

class CaseContent;

class Type;

class TypeAnnotation;

class Subtype;

class TypePostfix;

class IndexedType;

class Pointer;

class LifeTime;

class Root;

class Local;

class Reference;

class Thrown;

class TypeInheritanceClause;

class Inheritance;

class SyntaxVisitor : public Object {
public:
    virtual bool openProgram(Program* program) = 0;
    virtual void closeProgram(Program* program) = 0;
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit) = 0;
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit) = 0;
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration) = 0;
    virtual void closeConstantDeclaration(ConstantDeclaration* constantDeclaration) = 0;
    virtual bool openVariableDeclaration(VariableDeclaration* variableDeclaration) = 0;
    virtual void closeVariableDeclaration(VariableDeclaration* variableDeclaration) = 0;
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration) = 0;
    virtual void closeMutableDeclaration(MutableDeclaration* mutableDeclaration) = 0;
    virtual bool openFunctionDeclaration(FunctionDeclaration* functionDeclaration) = 0;
    virtual void closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) = 0;
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration) = 0;
    virtual void closeEnumDeclaration(EnumDeclaration* enumDeclaration) = 0;
    virtual bool openClassDeclaration(ClassDeclaration* classDeclaration) = 0;
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration) = 0;
    virtual bool openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) = 0;
    virtual void closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) = 0;
    virtual bool openCodeBlock(CodeBlock* codeBlock) = 0;
    virtual void closeCodeBlock(CodeBlock* codeBlock) = 0;
    virtual bool openSimpleExpression(SimpleExpression* simpleExpression) = 0;
    virtual void closeSimpleExpression(SimpleExpression* simpleExpression) = 0;
    virtual bool openInitializer(Initializer* initializer) = 0;
    virtual void closeInitializer(Initializer* initializer) = 0;
    virtual bool openBindingInitializer(BindingInitializer* bindingInitializer) = 0;
    virtual void closeBindingInitializer(BindingInitializer* bindingInitializer) = 0;
    virtual bool openPatternInitializer(PatternInitializer* patternInitializer) = 0;
    virtual void closePatternInitializer(PatternInitializer* patternInitializer) = 0;
    virtual bool openAdditionalInitializer(AdditionalInitializer* additionalInitializer) = 0;
    virtual void closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) = 0;
    virtual void visitOverrideWord(OverrideWord* overrideWord) = 0;
    virtual void visitStaticWord(StaticWord* staticWord) = 0;
    virtual bool openFunctionSignature(FunctionSignature* functionSignature) = 0;
    virtual void closeFunctionSignature(FunctionSignature* functionSignature) = 0;
    virtual bool openFunctionResult(FunctionResult* functionResult) = 0;
    virtual void closeFunctionResult(FunctionResult* functionResult) = 0;
    virtual bool openParameterClause(ParameterClause* parameterClause) = 0;
    virtual void closeParameterClause(ParameterClause* parameterClause) = 0;
    virtual bool openConstParameter(ConstParameter* constParameter) = 0;
    virtual void closeConstParameter(ConstParameter* constParameter) = 0;
    virtual bool openVarParameter(VarParameter* varParameter) = 0;
    virtual void closeVarParameter(VarParameter* varParameter) = 0;
    virtual bool openThrowsClause(ThrowsClause* throwsClause) = 0;
    virtual void closeThrowsClause(ThrowsClause* throwsClause) = 0;
    virtual bool openEnumMember(EnumMember* enumMember) = 0;
    virtual void closeEnumMember(EnumMember* enumMember) = 0;
    virtual void visitEnumCase(EnumCase* enumCase) = 0;
    virtual bool openAdditionalCase(AdditionalCase* additionalCase) = 0;
    virtual void closeAdditionalCase(AdditionalCase* additionalCase) = 0;
    virtual bool openClassBody(ClassBody* classBody) = 0;
    virtual void closeClassBody(ClassBody* classBody) = 0;
    virtual bool openClassMember(ClassMember* classMember) = 0;
    virtual void closeClassMember(ClassMember* classMember) = 0;
    virtual bool openPrefixExpression(PrefixExpression* prefixExpression) = 0;
    virtual void closePrefixExpression(PrefixExpression* prefixExpression) = 0;
    virtual bool openPostfixExpression(PostfixExpression* postfixExpression) = 0;
    virtual void closePostfixExpression(PostfixExpression* postfixExpression) = 0;
    virtual bool openBinaryOperation(BinaryOperation* binaryOperation) = 0;
    virtual void closeBinaryOperation(BinaryOperation* binaryOperation) = 0;
    virtual bool openAssignment(Assignment* assignment) = 0;
    virtual void closeAssignment(Assignment* assignment) = 0;
    virtual bool openTypeQuery(TypeQuery* typeQuery) = 0;
    virtual void closeTypeQuery(TypeQuery* typeQuery) = 0;
    virtual bool openTypeCast(TypeCast* typeCast) = 0;
    virtual void closeTypeCast(TypeCast* typeCast) = 0;
    virtual bool openCatchClause(CatchClause* catchClause) = 0;
    virtual void closeCatchClause(CatchClause* catchClause) = 0;
    virtual bool openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) = 0;
    virtual void closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) = 0;
    virtual bool openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) = 0;
    virtual void closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) = 0;
    virtual void visitOperatorPostfix(OperatorPostfix* operatorPostfix) = 0;
    virtual bool openFunctionCall(FunctionCall* functionCall) = 0;
    virtual void closeFunctionCall(FunctionCall* functionCall) = 0;
    virtual void visitMemberExpression(MemberExpression* memberExpression) = 0;
    virtual bool openSubscript(Subscript* subscript) = 0;
    virtual void closeSubscript(Subscript* subscript) = 0;
    virtual bool openExpressionElement(ExpressionElement* expressionElement) = 0;
    virtual void closeExpressionElement(ExpressionElement* expressionElement) = 0;
    virtual void visitIdentifierExpression(IdentifierExpression* identifierExpression) = 0;
    virtual void visitLiteralExpression(LiteralExpression* literalExpression) = 0;
    virtual bool openIfExpression(IfExpression* ifExpression) = 0;
    virtual void closeIfExpression(IfExpression* ifExpression) = 0;
    virtual bool openSwitchExpression(SwitchExpression* switchExpression) = 0;
    virtual void closeSwitchExpression(SwitchExpression* switchExpression) = 0;
    virtual bool openForExpression(ForExpression* forExpression) = 0;
    virtual void closeForExpression(ForExpression* forExpression) = 0;
    virtual bool openWhileExpression(WhileExpression* whileExpression) = 0;
    virtual void closeWhileExpression(WhileExpression* whileExpression) = 0;
    virtual bool openDoExpression(DoExpression* doExpression) = 0;
    virtual void closeDoExpression(DoExpression* doExpression) = 0;
    virtual bool openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) = 0;
    virtual void closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) = 0;
    virtual bool openReturnExpression(ReturnExpression* returnExpression) = 0;
    virtual void closeReturnExpression(ReturnExpression* returnExpression) = 0;
    virtual bool openThrowExpression(ThrowExpression* throwExpression) = 0;
    virtual void closeThrowExpression(ThrowExpression* throwExpression) = 0;
    virtual bool openBreakExpression(BreakExpression* breakExpression) = 0;
    virtual void closeBreakExpression(BreakExpression* breakExpression) = 0;
    virtual bool openConstructorCall(ConstructorCall* constructorCall) = 0;
    virtual void closeConstructorCall(ConstructorCall* constructorCall) = 0;
    virtual void visitThisExpression(ThisExpression* thisExpression) = 0;
    virtual void visitNullExpression(NullExpression* nullExpression) = 0;
    virtual bool openElseClause(ElseClause* elseClause) = 0;
    virtual void closeElseClause(ElseClause* elseClause) = 0;
    virtual bool openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) = 0;
    virtual void closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) = 0;
    virtual bool openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) = 0;
    virtual void closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) = 0;
    virtual bool openSwitchCase(SwitchCase* switchCase) = 0;
    virtual void closeSwitchCase(SwitchCase* switchCase) = 0;
    virtual bool openItemCaseLabel(ItemCaseLabel* itemCaseLabel) = 0;
    virtual void closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) = 0;
    virtual void visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) = 0;
    virtual bool openCaseItem(CaseItem* caseItem) = 0;
    virtual void closeCaseItem(CaseItem* caseItem) = 0;
    virtual void visitWildcardPattern(WildcardPattern* wildcardPattern) = 0;
    virtual bool openIdentifierPattern(IdentifierPattern* identifierPattern) = 0;
    virtual void closeIdentifierPattern(IdentifierPattern* identifierPattern) = 0;
    virtual bool openTuplePattern(TuplePattern* tuplePattern) = 0;
    virtual void closeTuplePattern(TuplePattern* tuplePattern) = 0;
    virtual bool openExpressionPattern(ExpressionPattern* expressionPattern) = 0;
    virtual void closeExpressionPattern(ExpressionPattern* expressionPattern) = 0;
    virtual bool openTuplePatternElement(TuplePatternElement* tuplePatternElement) = 0;
    virtual void closeTuplePatternElement(TuplePatternElement* tuplePatternElement) = 0;
    virtual bool openCaseContent(CaseContent* caseContent) = 0;
    virtual void closeCaseContent(CaseContent* caseContent) = 0;
    virtual bool openType(Type* type) = 0;
    virtual void closeType(Type* type) = 0;
    virtual bool openTypeAnnotation(TypeAnnotation* typeAnnotation) = 0;
    virtual void closeTypeAnnotation(TypeAnnotation* typeAnnotation) = 0;
    virtual bool openSubtype(Subtype* subtype) = 0;
    virtual void closeSubtype(Subtype* subtype) = 0;
    virtual bool openIndexedType(IndexedType* indexedType) = 0;
    virtual void closeIndexedType(IndexedType* indexedType) = 0;
    virtual void visitPointer(Pointer* pointer) = 0;
    virtual void visitRoot(Root* root) = 0;
    virtual void visitLocal(Local* local) = 0;
    virtual void visitReference(Reference* reference) = 0;
    virtual void visitThrown(Thrown* thrown) = 0;
    virtual bool openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) = 0;
    virtual void closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) = 0;
    virtual bool openInheritance(Inheritance* inheritance) = 0;
    virtual void closeInheritance(Inheritance* inheritance) = 0;

    virtual bool _isCppVisitor();
    virtual bool _isHeaderVisitor();
    virtual bool _isSourceVisitor();
};

class SyntaxNode : public Object {
public:
    Position* start;
    Position* end;
    SyntaxNode* parent;

    virtual bool _isProgram();
    virtual bool _isCompilationUnit();
    virtual bool _isStatement();
    virtual bool _isDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isConstructorDeclaration();
    virtual bool _isExpression();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
    virtual bool _isInitializer();
    virtual bool _isBindingInitializer();
    virtual bool _isPatternInitializer();
    virtual bool _isAdditionalInitializer();
    virtual bool _isModifier();
    virtual bool _isOverrideWord();
    virtual bool _isStaticWord();
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
    virtual bool _isIdentifierCatchPattern();
    virtual bool _isPostfix();
    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isMemberExpression();
    virtual bool _isSubscript();
    virtual bool _isExpressionElement();
    virtual bool _isPrimaryExpression();
    virtual bool _isIdentifierExpression();
    virtual bool _isLiteralExpression();
    virtual bool _isIfExpression();
    virtual bool _isSwitchExpression();
    virtual bool _isForExpression();
    virtual bool _isWhileExpression();
    virtual bool _isDoExpression();
    virtual bool _isParenthesizedExpression();
    virtual bool _isReturnExpression();
    virtual bool _isThrowExpression();
    virtual bool _isBreakExpression();
    virtual bool _isConstructorCall();
    virtual bool _isThisExpression();
    virtual bool _isNullExpression();
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
    virtual bool _isType();
    virtual bool _isTypeAnnotation();
    virtual bool _isSubtype();
    virtual bool _isTypePostfix();
    virtual bool _isIndexedType();
    virtual bool _isPointer();
    virtual bool _isLifeTime();
    virtual bool _isRoot();
    virtual bool _isLocal();
    virtual bool _isReference();
    virtual bool _isThrown();
    virtual bool _isTypeInheritanceClause();
    virtual bool _isInheritance();
};

class Program : public SyntaxNode {
public:
    Program(string* name, string* directory, _Array<CompilationUnit>* compilationUnits);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
    string* directory;
    _Array<CompilationUnit>* compilationUnits;

    virtual bool _isProgram();
};

class CompilationUnit : public SyntaxNode {
public:
    CompilationUnit(_Array<Statement>* statements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<Statement>* statements;
    string* fileName;

    virtual bool _isCompilationUnit();
};

class Statement : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isStatement();
    virtual bool _isDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isConstructorDeclaration();
    virtual bool _isExpression();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
};

class Declaration : public Statement {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isVariableDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isConstructorDeclaration();
};

class Expression : public Statement {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isExpression();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
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
    FunctionDeclaration(_Array<Modifier>* modifiers, string* name, FunctionSignature* signature, Expression* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<Modifier>* modifiers;
    string* name;
    FunctionSignature* signature;
    Expression* body;

    virtual bool _isFunctionDeclaration();
};

class EnumDeclaration : public Declaration {
public:
    EnumDeclaration(string* name, _Array<EnumMember>* members, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
    _Array<EnumMember>* members;

    virtual bool _isEnumDeclaration();
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(string* name, TypeInheritanceClause* typeInheritanceClause, ClassBody* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
    TypeInheritanceClause* typeInheritanceClause;
    ClassBody* body;

    virtual bool _isClassDeclaration();
};

class ConstructorDeclaration : public Declaration {
public:
    ConstructorDeclaration(ParameterClause* parameterClause, Expression* body, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    ParameterClause* parameterClause;
    Expression* body;

    virtual bool _isConstructorDeclaration();
};

class CodeBlock : public Expression {
public:
    CodeBlock(_Array<Statement>* statements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<Statement>* statements;

    virtual bool _isCodeBlock();
};

class SimpleExpression : public Expression {
public:
    SimpleExpression(PrefixExpression* prefixExpression, _Array<BinaryOp>* binaryOps, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PrefixExpression* prefixExpression;
    _Array<BinaryOp>* binaryOps;

    virtual bool _isSimpleExpression();
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
    BindingInitializer(PatternInitializer* initializer, _Array<AdditionalInitializer>* additionalInitializers, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PatternInitializer* initializer;
    _Array<AdditionalInitializer>* additionalInitializers;

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
    ParameterClause(_Array<Parameter>* parameters, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<Parameter>* parameters;

    virtual bool _isParameterClause();
};

class Parameter : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isParameter();
    virtual bool _isConstParameter();
    virtual bool _isVarParameter();
};

class ConstParameter : public Parameter {
public:
    ConstParameter(string* name, Type* parameterType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
    Type* parameterType;

    virtual bool _isConstParameter();
};

class VarParameter : public Parameter {
public:
    VarParameter(string* name, Type* parameterType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
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
    EnumMember(EnumCase* enumCase, _Array<AdditionalCase>* additionalCases, ParameterClause* parameterClause, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    EnumCase* enumCase;
    _Array<AdditionalCase>* additionalCases;
    ParameterClause* parameterClause;

    virtual bool _isEnumMember();
};

class EnumCase : public SyntaxNode {
public:
    EnumCase(string* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;

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
    ClassBody(_Array<ClassMember>* members, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<ClassMember>* members;

    virtual bool _isClassBody();
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
    PrefixExpression(string* prefixOperator, PostfixExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* prefixOperator;
    PostfixExpression* expression;

    virtual bool _isPrefixExpression();
};

class PostfixExpression : public SyntaxNode {
public:
    PostfixExpression(PrimaryExpression* primaryExpression, _Array<Postfix>* postfixes, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    PrimaryExpression* primaryExpression;
    _Array<Postfix>* postfixes;

    virtual bool _isPostfixExpression();
};

class BinaryOp : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isBinaryOp();
    virtual bool _isBinaryOperation();
    virtual bool _isAssignment();
    virtual bool _isTypeQuery();
    virtual bool _isTypeCast();
};

class BinaryOperation : public BinaryOp {
public:
    BinaryOperation(string* binaryOperator, PrefixExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* binaryOperator;
    PrefixExpression* expression;

    virtual bool _isBinaryOperation();
};

class Assignment : public BinaryOp {
public:
    Assignment(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

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
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCatchPattern();
    virtual bool _isWildCardCatchPattern();
    virtual bool _isIdentifierCatchPattern();
};

class WildCardCatchPattern : public CatchPattern {
public:
    WildCardCatchPattern(WildcardPattern* pattern, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    WildcardPattern* pattern;

    virtual bool _isWildCardCatchPattern();
};

class IdentifierCatchPattern : public CatchPattern {
public:
    IdentifierCatchPattern(string* name, MemberExpression* member, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
    MemberExpression* member;

    virtual bool _isIdentifierCatchPattern();
};

class Postfix : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isPostfix();
    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isMemberExpression();
    virtual bool _isSubscript();
};

class OperatorPostfix : public Postfix {
public:
    OperatorPostfix(string* postfixOperator, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* postfixOperator;

    virtual bool _isOperatorPostfix();
};

class FunctionCall : public Postfix {
public:
    FunctionCall(ParenthesizedExpression* arguments, _Array<CatchClause>* catchClauses, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    ParenthesizedExpression* arguments;
    _Array<CatchClause>* catchClauses;

    virtual bool _isFunctionCall();
};

class MemberExpression : public Postfix {
public:
    MemberExpression(string* member, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* member;

    virtual bool _isMemberExpression();
};

class Subscript : public Postfix {
public:
    Subscript(_Array<ExpressionElement>* expressions, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<ExpressionElement>* expressions;

    virtual bool _isSubscript();
};

class ExpressionElement : public SyntaxNode {
public:
    ExpressionElement(Expression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* expression;

    virtual bool _isExpressionElement();
};

class PrimaryExpression : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isPrimaryExpression();
    virtual bool _isIdentifierExpression();
    virtual bool _isLiteralExpression();
    virtual bool _isIfExpression();
    virtual bool _isSwitchExpression();
    virtual bool _isForExpression();
    virtual bool _isWhileExpression();
    virtual bool _isDoExpression();
    virtual bool _isParenthesizedExpression();
    virtual bool _isReturnExpression();
    virtual bool _isThrowExpression();
    virtual bool _isBreakExpression();
    virtual bool _isConstructorCall();
    virtual bool _isThisExpression();
    virtual bool _isNullExpression();
};

class IdentifierExpression : public PrimaryExpression {
public:
    IdentifierExpression(string* name, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;

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

class DoExpression : public PrimaryExpression {
public:
    DoExpression(Expression* code, Expression* condition, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Expression* code;
    Expression* condition;

    virtual bool _isDoExpression();
};

class ParenthesizedExpression : public PrimaryExpression {
public:
    ParenthesizedExpression(_Array<ExpressionElement>* expressionElements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<ExpressionElement>* expressionElements;

    virtual bool _isParenthesizedExpression();
};

class ReturnExpression : public PrimaryExpression {
public:
    ReturnExpression(ParenthesizedExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    ParenthesizedExpression* expression;

    virtual bool _isReturnExpression();
};

class ThrowExpression : public PrimaryExpression {
public:
    ThrowExpression(IdentifierExpression* error, ParenthesizedExpression* arguments, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    IdentifierExpression* error;
    ParenthesizedExpression* arguments;

    virtual bool _isThrowExpression();
};

class BreakExpression : public PrimaryExpression {
public:
    BreakExpression(ParenthesizedExpression* expression, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    ParenthesizedExpression* expression;

    virtual bool _isBreakExpression();
};

class ConstructorCall : public PrimaryExpression {
public:
    ConstructorCall(Type* typeToInitialize, ParenthesizedExpression* arguments, _Array<CatchClause>* catchClauses, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* typeToInitialize;
    ParenthesizedExpression* arguments;
    _Array<CatchClause>* catchClauses;

    virtual bool _isConstructorCall();
};

class ThisExpression : public PrimaryExpression {
public:
    ThisExpression(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isThisExpression();
};

class NullExpression : public PrimaryExpression {
public:
    NullExpression(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isNullExpression();
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
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isSwitchBody();
    virtual bool _isCurliedSwitchBody();
    virtual bool _isNakedSwitchBody();
};

class CurliedSwitchBody : public SwitchBody {
public:
    CurliedSwitchBody(_Array<SwitchCase>* cases, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<SwitchCase>* cases;

    virtual bool _isCurliedSwitchBody();
};

class NakedSwitchBody : public SwitchBody {
public:
    NakedSwitchBody(_Array<SwitchCase>* cases, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<SwitchCase>* cases;

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
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isCaseLabel();
    virtual bool _isItemCaseLabel();
    virtual bool _isDefaultCaseLabel();
};

class ItemCaseLabel : public CaseLabel {
public:
    ItemCaseLabel(Pattern* pattern, _Array<CaseItem>* additionalPatterns, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Pattern* pattern;
    _Array<CaseItem>* additionalPatterns;

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
    IdentifierPattern(string* identifier, TypeAnnotation* annotationForType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* identifier;
    TypeAnnotation* annotationForType;

    virtual bool _isIdentifierPattern();
};

class TuplePattern : public Pattern {
public:
    TuplePattern(_Array<TuplePatternElement>* elements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<TuplePatternElement>* elements;

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
    CaseContent(_Array<Statement>* statements, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<Statement>* statements;

    virtual bool _isCaseContent();
};

class Type : public SyntaxNode {
public:
    Type(string* name, Subtype* subType, _Array<TypePostfix>* postfixes, LifeTime* lifeTime, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* name;
    Subtype* subType;
    _Array<TypePostfix>* postfixes;
    LifeTime* lifeTime;

    virtual bool _isType();
};

class TypeAnnotation : public SyntaxNode {
public:
    TypeAnnotation(Type* annotationForType, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* annotationForType;

    virtual bool _isTypeAnnotation();
};

class Subtype : public SyntaxNode {
public:
    Subtype(Type* type, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* type;

    virtual bool _isSubtype();
};

class TypePostfix : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isTypePostfix();
    virtual bool _isIndexedType();
    virtual bool _isPointer();
};

class IndexedType : public TypePostfix {
public:
    IndexedType(Type* key, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* key;

    virtual bool _isIndexedType();
};

class Pointer : public TypePostfix {
public:
    Pointer(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isPointer();
};

class LifeTime : public SyntaxNode {
public:
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isLifeTime();
    virtual bool _isRoot();
    virtual bool _isLocal();
    virtual bool _isReference();
    virtual bool _isThrown();
};

class Root : public LifeTime {
public:
    Root(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isRoot();
};

class Local : public LifeTime {
public:
    Local(string* location, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    string* location;

    virtual bool _isLocal();
};

class Reference : public LifeTime {
public:
    Reference(Literal* age, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Literal* age;

    virtual bool _isReference();
};

class Thrown : public LifeTime {
public:
    Thrown(Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);

    virtual bool _isThrown();
};

class TypeInheritanceClause : public SyntaxNode {
public:
    TypeInheritanceClause(_Array<Inheritance>* inheritances, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    _Array<Inheritance>* inheritances;

    virtual bool _isTypeInheritanceClause();
};

class Inheritance : public SyntaxNode {
public:
    Inheritance(Type* type, Position* start, Position* end);
    virtual void accept(SyntaxVisitor* visitor);
    Type* type;

    virtual bool _isInheritance();
};

class Parser : public Object {
public:
    Parser(string* theFileName, string* text);
    virtual _Result<CompilationUnit, ParserError> parseCompilationUnit(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<Statement>, ParserError> parseStatementList(_Page* _rp, _Page* _ep);
    virtual _Result<Statement, ParserError> parseStatement(_Page* _rp, _Page* _ep);
    virtual _Result<Declaration, ParserError> parseDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<Expression, ParserError> parseExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ConstantDeclaration, ParserError> parseConstantDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<VariableDeclaration, ParserError> parseVariableDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<MutableDeclaration, ParserError> parseMutableDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<FunctionDeclaration, ParserError> parseFunctionDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<EnumDeclaration, ParserError> parseEnumDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<ClassDeclaration, ParserError> parseClassDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<ConstructorDeclaration, ParserError> parseConstructorDeclaration(_Page* _rp, _Page* _ep);
    virtual _Result<CodeBlock, ParserError> parseCodeBlock(_Page* _rp, _Page* _ep);
    virtual _Result<SimpleExpression, ParserError> parseSimpleExpression(_Page* _rp, _Page* _ep);
    virtual _Result<Initializer, ParserError> parseInitializer(_Page* _rp, _Page* _ep);
    virtual _Result<BindingInitializer, ParserError> parseBindingInitializer(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<PatternInitializer>, ParserError> parsePatternInitializerList(_Page* _rp, _Page* _ep);
    virtual _Result<PatternInitializer, ParserError> parsePatternInitializer(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<AdditionalInitializer>, ParserError> parseAdditionalInitializerList(_Page* _rp, _Page* _ep);
    virtual _Result<AdditionalInitializer, ParserError> parseAdditionalInitializer(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<Modifier>, ParserError> parseModifierList(_Page* _rp, _Page* _ep);
    virtual _Result<Modifier, ParserError> parseModifier(_Page* _rp, _Page* _ep);
    virtual _Result<OverrideWord, ParserError> parseOverrideWord(_Page* _rp, _Page* _ep);
    virtual _Result<StaticWord, ParserError> parseStaticWord(_Page* _rp, _Page* _ep);
    virtual _Result<FunctionSignature, ParserError> parseFunctionSignature(_Page* _rp, _Page* _ep);
    virtual _Result<FunctionResult, ParserError> parseFunctionResult(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<ParameterClause>, ParserError> parseParameterClauseList(_Page* _rp, _Page* _ep);
    virtual _Result<ParameterClause, ParserError> parseParameterClause(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<Parameter>, ParserError> parseParameterList(_Page* _rp, _Page* _ep);
    virtual _Result<Parameter, ParserError> parseParameter(_Page* _rp, _Page* _ep);
    virtual _Result<ConstParameter, ParserError> parseConstParameter(_Page* _rp, _Page* _ep);
    virtual _Result<VarParameter, ParserError> parseVarParameter(_Page* _rp, _Page* _ep);
    virtual _Result<ThrowsClause, ParserError> parseThrowsClause(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<EnumMember>, ParserError> parseEnumMemberList(_Page* _rp, _Page* _ep);
    virtual _Result<EnumMember, ParserError> parseEnumMember(_Page* _rp, _Page* _ep);
    virtual _Result<EnumCase, ParserError> parseEnumCase(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<AdditionalCase>, ParserError> parseAdditionalCaseList(_Page* _rp, _Page* _ep);
    virtual _Result<AdditionalCase, ParserError> parseAdditionalCase(_Page* _rp, _Page* _ep);
    virtual _Result<ClassBody, ParserError> parseClassBody(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<ClassMember>, ParserError> parseClassMemberList(_Page* _rp, _Page* _ep);
    virtual _Result<ClassMember, ParserError> parseClassMember(_Page* _rp, _Page* _ep);
    virtual _Result<PrefixExpression, ParserError> parsePrefixExpression(_Page* _rp, _Page* _ep);
    virtual _Result<PostfixExpression, ParserError> parsePostfixExpression(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<BinaryOp>, ParserError> parseBinaryOpList(_Page* _rp, _Page* _ep);
    virtual _Result<BinaryOp, ParserError> parseBinaryOp(_Page* _rp, _Page* _ep);
    virtual _Result<BinaryOperation, ParserError> parseBinaryOperation(_Page* _rp, _Page* _ep);
    virtual _Result<Assignment, ParserError> parseAssignment(_Page* _rp, _Page* _ep);
    virtual _Result<TypeQuery, ParserError> parseTypeQuery(_Page* _rp, _Page* _ep);
    virtual _Result<TypeCast, ParserError> parseTypeCast(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<CatchClause>, ParserError> parseCatchClauseList(_Page* _rp, _Page* _ep);
    virtual _Result<CatchClause, ParserError> parseCatchClause(_Page* _rp, _Page* _ep);
    virtual _Result<CatchPattern, ParserError> parseCatchPattern(_Page* _rp, _Page* _ep);
    virtual _Result<WildCardCatchPattern, ParserError> parseWildCardCatchPattern(_Page* _rp, _Page* _ep);
    virtual _Result<IdentifierCatchPattern, ParserError> parseIdentifierCatchPattern(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<Postfix>, ParserError> parsePostfixList(_Page* _rp, _Page* _ep);
    virtual _Result<Postfix, ParserError> parsePostfix(_Page* _rp, _Page* _ep);
    virtual _Result<OperatorPostfix, ParserError> parseOperatorPostfix(_Page* _rp, _Page* _ep);
    virtual _Result<FunctionCall, ParserError> parseFunctionCall(_Page* _rp, _Page* _ep);
    virtual _Result<MemberExpression, ParserError> parseMemberExpression(_Page* _rp, _Page* _ep);
    virtual _Result<Subscript, ParserError> parseSubscript(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<ExpressionElement>, ParserError> parseExpressionElementList(_Page* _rp, _Page* _ep);
    virtual _Result<ExpressionElement, ParserError> parseExpressionElement(_Page* _rp, _Page* _ep);
    virtual _Result<PrimaryExpression, ParserError> parsePrimaryExpression(_Page* _rp, _Page* _ep);
    virtual _Result<IdentifierExpression, ParserError> parseIdentifierExpression(_Page* _rp, _Page* _ep);
    virtual _Result<LiteralExpression, ParserError> parseLiteralExpression(_Page* _rp, _Page* _ep);
    virtual _Result<IfExpression, ParserError> parseIfExpression(_Page* _rp, _Page* _ep);
    virtual _Result<SwitchExpression, ParserError> parseSwitchExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ForExpression, ParserError> parseForExpression(_Page* _rp, _Page* _ep);
    virtual _Result<WhileExpression, ParserError> parseWhileExpression(_Page* _rp, _Page* _ep);
    virtual _Result<DoExpression, ParserError> parseDoExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ParenthesizedExpression, ParserError> parseParenthesizedExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ReturnExpression, ParserError> parseReturnExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ThrowExpression, ParserError> parseThrowExpression(_Page* _rp, _Page* _ep);
    virtual _Result<BreakExpression, ParserError> parseBreakExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ConstructorCall, ParserError> parseConstructorCall(_Page* _rp, _Page* _ep);
    virtual _Result<ThisExpression, ParserError> parseThisExpression(_Page* _rp, _Page* _ep);
    virtual _Result<NullExpression, ParserError> parseNullExpression(_Page* _rp, _Page* _ep);
    virtual _Result<ElseClause, ParserError> parseElseClause(_Page* _rp, _Page* _ep);
    virtual _Result<SwitchBody, ParserError> parseSwitchBody(_Page* _rp, _Page* _ep);
    virtual _Result<CurliedSwitchBody, ParserError> parseCurliedSwitchBody(_Page* _rp, _Page* _ep);
    virtual _Result<NakedSwitchBody, ParserError> parseNakedSwitchBody(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<SwitchCase>, ParserError> parseSwitchCaseList(_Page* _rp, _Page* _ep);
    virtual _Result<SwitchCase, ParserError> parseSwitchCase(_Page* _rp, _Page* _ep);
    virtual _Result<CaseLabel, ParserError> parseCaseLabel(_Page* _rp, _Page* _ep);
    virtual _Result<ItemCaseLabel, ParserError> parseItemCaseLabel(_Page* _rp, _Page* _ep);
    virtual _Result<DefaultCaseLabel, ParserError> parseDefaultCaseLabel(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<CaseItem>, ParserError> parseCaseItemList(_Page* _rp, _Page* _ep);
    virtual _Result<CaseItem, ParserError> parseCaseItem(_Page* _rp, _Page* _ep);
    virtual _Result<Pattern, ParserError> parsePattern(_Page* _rp, _Page* _ep);
    virtual _Result<WildcardPattern, ParserError> parseWildcardPattern(_Page* _rp, _Page* _ep);
    virtual _Result<IdentifierPattern, ParserError> parseIdentifierPattern(_Page* _rp, _Page* _ep);
    virtual _Result<TuplePattern, ParserError> parseTuplePattern(_Page* _rp, _Page* _ep);
    virtual _Result<ExpressionPattern, ParserError> parseExpressionPattern(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<TuplePatternElement>, ParserError> parseTuplePatternElementList(_Page* _rp, _Page* _ep);
    virtual _Result<TuplePatternElement, ParserError> parseTuplePatternElement(_Page* _rp, _Page* _ep);
    virtual _Result<CaseContent, ParserError> parseCaseContent(_Page* _rp, _Page* _ep);
    virtual _Result<Type, ParserError> parseType(_Page* _rp, _Page* _ep);
    virtual _Result<TypeAnnotation, ParserError> parseTypeAnnotation(_Page* _rp, _Page* _ep);
    virtual _Result<Subtype, ParserError> parseSubtype(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<TypePostfix>, ParserError> parseTypePostfixList(_Page* _rp, _Page* _ep);
    virtual _Result<TypePostfix, ParserError> parseTypePostfix(_Page* _rp, _Page* _ep);
    virtual _Result<IndexedType, ParserError> parseIndexedType(_Page* _rp, _Page* _ep);
    virtual _Result<Pointer, ParserError> parsePointer(_Page* _rp, _Page* _ep);
    virtual _Result<LifeTime, ParserError> parseLifeTime(_Page* _rp, _Page* _ep);
    virtual _Result<Root, ParserError> parseRoot(_Page* _rp, _Page* _ep);
    virtual _Result<Local, ParserError> parseLocal(_Page* _rp, _Page* _ep);
    virtual _Result<Reference, ParserError> parseReference(_Page* _rp, _Page* _ep);
    virtual _Result<Thrown, ParserError> parseThrown(_Page* _rp, _Page* _ep);
    virtual _Result<TypeInheritanceClause, ParserError> parseTypeInheritanceClause(_Page* _rp, _Page* _ep);
    virtual _Result<_Array<Inheritance>, ParserError> parseInheritanceList(_Page* _rp, _Page* _ep);
    virtual _Result<Inheritance, ParserError> parseInheritance(_Page* _rp, _Page* _ep);
    virtual bool isAtEnd();
    virtual bool isIdentifier(string* id);
    Lexer* lexer;
    string* fileName;
    string* classKeyword;
    string* functionKeyword;
    string* ifKeyword;
    string* elseKeyword;
    string* switchKeyword;
    string* caseKeyword;
    string* defaultKeyword;
    string* catchKeyword;
    string* forKeyword;
    string* inKeyword;
    string* whileKeyword;
    string* doKeyword;
    string* returnKeyword;
    string* throwKeyword;
    string* breakKeyword;
    string* throwsKeyword;
    string* overrideKeyword;
    string* staticKeyword;
    string* letKeyword;
    string* varKeyword;
    string* mutableKeyword;
    string* isKeyword;
    string* asKeyword;
    string* constructorKeyword;
    string* enumKeyword;
    string* thisKeyword;
    string* nullKeyword;
    string* newKeyword;
    string* extendsKeyword;
    string* equal;
    string* comma;
    string* leftParen;
    string* rightParen;
    string* leftCurly;
    string* rightCurly;
    string* leftBracket;
    string* rightBracket;
    string* colon;
    string* dot;
    string* underscore;
    string* circumflex;
    string* dollar;
    string* at;
    string* hash;
    string* tilde;

};

}
#endif // __scalyc__Parser__
