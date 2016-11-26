#ifndef __scalyc__Visitor__
#define __scalyc__Visitor__
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
    virtual void visitRoot(Root* local) = 0;
    virtual void visitReference(Reference* reference) = 0;
    virtual void visitThrown(Thrown* thrown) = 0;
    virtual bool openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) = 0;
    virtual void closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) = 0;
    virtual bool openInheritance(Inheritance* inheritance) = 0;
    virtual void closeInheritance(Inheritance* inheritance) = 0;

    virtual bool _isMyVisitor();
    virtual bool _isCppVisitor();
};

}
#endif // __scalyc__Visitor__
