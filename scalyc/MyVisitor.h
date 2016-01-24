#ifndef __scalyc__MyVisitor__
#define __scalyc__MyVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class MyVisitor : public SyntaxVisitor {
    virtual void VisitTopLevelDeclaration(TopLevelDeclaration);
    virtual void VisitStatementWithSemicolon(StatementWithSemicolon);
    virtual void VisitStatement(Statement);
    virtual void VisitDeclaration(Declaration);
    virtual void VisitUseDeclaration(UseDeclaration);
    virtual void VisitPathIdentifier(PathIdentifier);
    virtual void VisitPathItem(PathItem);
    virtual void VisitInitializer(Initializer);
    virtual void VisitConstantDeclaration(ConstantDeclaration);
    virtual void VisitVariableDeclaration(VariableDeclaration);
    virtual void VisitBindingInitializer(BindingInitializer);
    virtual void VisitPatternInitializer(PatternInitializer);
    virtual void VisitAdditionalInitializer(AdditionalInitializer);
    virtual void VisitFunctionDeclaration(FunctionDeclaration);
    virtual void VisitInitializerDeclaration(InitializerDeclaration);
    virtual void VisitModifier(Modifier);
    virtual void VisitStatic(Static);
    virtual void VisitFunctionName(FunctionName);
    virtual void VisitIdentifierFunctionName(IdentifierFunctionName);
    virtual void VisitFunctionSignature(FunctionSignature);
    virtual void VisitFunctionResult(FunctionResult);
    virtual void VisitParameterClause(ParameterClause);
    virtual void VisitParameter(Parameter);
    virtual void VisitThrowsClause(ThrowsClause);
    virtual void VisitEnumDeclaration(EnumDeclaration);
    virtual void VisitEnumMember(EnumMember);
    virtual void VisitTupleType(TupleType);
    virtual void VisitAdditionalType(AdditionalType);
    virtual void VisitEnumCase(EnumCase);
    virtual void VisitAdditionalCase(AdditionalCase);
    virtual void VisitClassDeclaration(ClassDeclaration);
    virtual void VisitGenericArgumentClause(GenericArgumentClause);
    virtual void VisitGenericParameter(GenericParameter);
    virtual void VisitClassMember(ClassMember);
    virtual void VisitExpression(Expression);
    virtual void VisitCodeBlock(CodeBlock);
    virtual void VisitSimpleExpression(SimpleExpression);
    virtual void VisitPrefixExpression(PrefixExpression);
    virtual void VisitPostfixExpression(PostfixExpression);
    virtual void VisitBinaryOp(BinaryOp);
    virtual void VisitBinaryOperation(BinaryOperation);
    virtual void VisitAssignment(Assignment);
    virtual void VisitTypeQuery(TypeQuery);
    virtual void VisitTypeCast(TypeCast);
    virtual void VisitCatchClause(CatchClause);
    virtual void VisitCatchPattern(CatchPattern);
    virtual void VisitWildCardCatchPattern(WildCardCatchPattern);
    virtual void VisitPathItemCatchPattern(PathItemCatchPattern);
    virtual void VisitPostfix(Postfix);
    virtual void VisitOperatorPostfix(OperatorPostfix);
    virtual void VisitFunctionCall(FunctionCall);
    virtual void VisitExplicitMemberExpression(ExplicitMemberExpression);
    virtual void VisitSubscript(Subscript);
    virtual void VisitExpressionElement(ExpressionElement);
    virtual void VisitMemberPostfix(MemberPostfix);
    virtual void VisitNamedMemberPostfix(NamedMemberPostfix);
    virtual void VisitPrimaryExpression(PrimaryExpression);
    virtual void VisitParenthesizedExpression(ParenthesizedExpression);
    virtual void VisitLiteralExpression(LiteralExpression);
    virtual void VisitIdentifierExpression(IdentifierExpression);
    virtual void VisitIfExpression(IfExpression);
    virtual void VisitElseClause(ElseClause);
    virtual void VisitSwitchExpression(SwitchExpression);
    virtual void VisitSwitchBody(SwitchBody);
    virtual void VisitCurliedSwitchBody(CurliedSwitchBody);
    virtual void VisitNakedSwitchBody(NakedSwitchBody);
    virtual void VisitSwitchCase(SwitchCase);
    virtual void VisitCaseLabel(CaseLabel);
    virtual void VisitItemCaseLabel(ItemCaseLabel);
    virtual void VisitCaseItem(CaseItem);
    virtual void VisitReturnExpression(ReturnExpression);
    virtual void VisitThrowExpression(ThrowExpression);
    virtual void VisitBreakExpression(BreakExpression);
    virtual void VisitPattern(Pattern);
    virtual void VisitWildcardPattern(WildcardPattern);
    virtual void VisitIdentifierPattern(IdentifierPattern);
    virtual void VisitTuplePattern(TuplePattern);
    virtual void VisitTuplePatternElement(TuplePatternElement);
    virtual void VisitExpressionPattern(ExpressionPattern);
    virtual void VisitDefaultCaseLabel(DefaultCaseLabel);
    virtual void VisitCaseContent(CaseContent);
    virtual void VisitBlockCaseContent(BlockCaseContent);
    virtual void VisitEmptyCaseContent(EmptyCaseContent);
    virtual void VisitInitializerCall(InitializerCall);
    virtual void VisitThisExpression(ThisExpression);
    virtual void VisitThisDot(ThisDot);
    virtual void VisitThisSubscript(ThisSubscript);
    virtual void VisitThis(This);
    virtual void VisitCommonThisMember(CommonThisMember);
    virtual void VisitThisInit(ThisInit);
    virtual void VisitThisMember(ThisMember);
    virtual void VisitSuperExpression(SuperExpression);
    virtual void VisitSuperDot(SuperDot);
    virtual void VisitSuperSubscript(SuperSubscript);
    virtual void VisitCommonSuperMember(CommonSuperMember);
    virtual void VisitSuperInit(SuperInit);
    virtual void VisitSuperMember(SuperMember);
    virtual void VisitType(Type);
    virtual void VisitTypeAnnotation(TypeAnnotation);
    virtual void VisitTypeIdentifier(TypeIdentifier);
    virtual void VisitSubtypeIdentifier(SubtypeIdentifier);
    virtual void VisitTypePostfix(TypePostfix);
    virtual void VisitArrayType(ArrayType);
    virtual void VisitOptionalType(OptionalType);
    virtual void VisitTypeInheritanceClause(TypeInheritanceClause);
    virtual void VisitInheritance(Inheritance);
};

}
#endif // __scalyc__MyVisitor__
