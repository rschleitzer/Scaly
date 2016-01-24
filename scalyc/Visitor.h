#ifndef __scalyc__Visitor__
#define __scalyc__Visitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class SyntaxVisitor {
    virtual void VisitTopLevelDeclaration(TopLevelDeclaration)= 0;
    virtual void VisitStatementWithSemicolon(StatementWithSemicolon)= 0;
    virtual void VisitStatement(Statement)= 0;
    virtual void VisitDeclaration(Declaration)= 0;
    virtual void VisitUseDeclaration(UseDeclaration)= 0;
    virtual void VisitPathIdentifier(PathIdentifier)= 0;
    virtual void VisitPathItem(PathItem)= 0;
    virtual void VisitInitializer(Initializer)= 0;
    virtual void VisitConstantDeclaration(ConstantDeclaration)= 0;
    virtual void VisitVariableDeclaration(VariableDeclaration)= 0;
    virtual void VisitBindingInitializer(BindingInitializer)= 0;
    virtual void VisitPatternInitializer(PatternInitializer)= 0;
    virtual void VisitAdditionalInitializer(AdditionalInitializer)= 0;
    virtual void VisitFunctionDeclaration(FunctionDeclaration)= 0;
    virtual void VisitInitializerDeclaration(InitializerDeclaration)= 0;
    virtual void VisitModifier(Modifier)= 0;
    virtual void VisitStatic(Static)= 0;
    virtual void VisitFunctionName(FunctionName)= 0;
    virtual void VisitIdentifierFunctionName(IdentifierFunctionName)= 0;
    virtual void VisitFunctionSignature(FunctionSignature)= 0;
    virtual void VisitFunctionResult(FunctionResult)= 0;
    virtual void VisitParameterClause(ParameterClause)= 0;
    virtual void VisitParameter(Parameter)= 0;
    virtual void VisitThrowsClause(ThrowsClause)= 0;
    virtual void VisitClassDeclaration(ClassDeclaration)= 0;
    virtual void VisitGenericArgumentClause(GenericArgumentClause)= 0;
    virtual void VisitGenericParameter(GenericParameter)= 0;
    virtual void VisitClassBody(ClassBody)= 0;
    virtual void VisitClassMember(ClassMember)= 0;
    virtual void VisitExpression(Expression)= 0;
    virtual void VisitCodeBlock(CodeBlock)= 0;
    virtual void VisitSimpleExpression(SimpleExpression)= 0;
    virtual void VisitPrefixExpression(PrefixExpression)= 0;
    virtual void VisitPostfixExpression(PostfixExpression)= 0;
    virtual void VisitBinaryOp(BinaryOp)= 0;
    virtual void VisitBinaryOperation(BinaryOperation)= 0;
    virtual void VisitAssignment(Assignment)= 0;
    virtual void VisitTypeQuery(TypeQuery)= 0;
    virtual void VisitTypeCast(TypeCast)= 0;
    virtual void VisitCatchClause(CatchClause)= 0;
    virtual void VisitCatchPattern(CatchPattern)= 0;
    virtual void VisitWildCardCatchPattern(WildCardCatchPattern)= 0;
    virtual void VisitPathItemCatchPattern(PathItemCatchPattern)= 0;
    virtual void VisitPostfix(Postfix)= 0;
    virtual void VisitOperatorPostfix(OperatorPostfix)= 0;
    virtual void VisitFunctionCall(FunctionCall)= 0;
    virtual void VisitExplicitMemberExpression(ExplicitMemberExpression)= 0;
    virtual void VisitSubscript(Subscript)= 0;
    virtual void VisitExpressionElement(ExpressionElement)= 0;
    virtual void VisitMemberPostfix(MemberPostfix)= 0;
    virtual void VisitNamedMemberPostfix(NamedMemberPostfix)= 0;
    virtual void VisitPrimaryExpression(PrimaryExpression)= 0;
    virtual void VisitParenthesizedExpression(ParenthesizedExpression)= 0;
    virtual void VisitLiteralExpression(LiteralExpression)= 0;
    virtual void VisitIdentifierExpression(IdentifierExpression)= 0;
    virtual void VisitIfExpression(IfExpression)= 0;
    virtual void VisitElseClause(ElseClause)= 0;
    virtual void VisitSwitchExpression(SwitchExpression)= 0;
    virtual void VisitSwitchBody(SwitchBody)= 0;
    virtual void VisitCurliedSwitchBody(CurliedSwitchBody)= 0;
    virtual void VisitNakedSwitchBody(NakedSwitchBody)= 0;
    virtual void VisitSwitchCase(SwitchCase)= 0;
    virtual void VisitCaseLabel(CaseLabel)= 0;
    virtual void VisitItemCaseLabel(ItemCaseLabel)= 0;
    virtual void VisitCaseItem(CaseItem)= 0;
    virtual void VisitReturnExpression(ReturnExpression)= 0;
    virtual void VisitThrowExpression(ThrowExpression)= 0;
    virtual void VisitBreakExpression(BreakExpression)= 0;
    virtual void VisitPattern(Pattern)= 0;
    virtual void VisitWildcardPattern(WildcardPattern)= 0;
    virtual void VisitIdentifierPattern(IdentifierPattern)= 0;
    virtual void VisitTuplePattern(TuplePattern)= 0;
    virtual void VisitTuplePatternElement(TuplePatternElement)= 0;
    virtual void VisitExpressionPattern(ExpressionPattern)= 0;
    virtual void VisitDefaultCaseLabel(DefaultCaseLabel)= 0;
    virtual void VisitCaseContent(CaseContent)= 0;
    virtual void VisitBlockCaseContent(BlockCaseContent)= 0;
    virtual void VisitEmptyCaseContent(EmptyCaseContent)= 0;
    virtual void VisitInitializerCall(InitializerCall)= 0;
    virtual void VisitType(Type)= 0;
    virtual void VisitTypeAnnotation(TypeAnnotation)= 0;
    virtual void VisitTypeIdentifier(TypeIdentifier)= 0;
    virtual void VisitSubtypeIdentifier(SubtypeIdentifier)= 0;
    virtual void VisitTypePostfix(TypePostfix)= 0;
    virtual void VisitArrayType(ArrayType)= 0;
    virtual void VisitOptionalType(OptionalType)= 0;
    virtual void VisitTypeInheritanceClause(TypeInheritanceClause)= 0;
    virtual void VisitInheritance(Inheritance)= 0;
};

}
#endif // __scalyc__Visitor__
