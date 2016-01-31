#ifndef __scalyc__CppVisitor__
#define __scalyc__CppVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CppVisitor : public SyntaxVisitor {
public:
    virtual void OpenProgram(Program& program);
    virtual void CloseProgram(Program& program);
    virtual void OpenCompilationUnit(CompilationUnit& compilationUnit);
    virtual void CloseCompilationUnit(CompilationUnit& compilationUnit);
    virtual void OpenStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon);
    virtual void CloseStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon);
    virtual void OpenUseDeclaration(UseDeclaration& useDeclaration);
    virtual void CloseUseDeclaration(UseDeclaration& useDeclaration);
    virtual void OpenPathIdentifier(PathIdentifier& pathIdentifier);
    virtual void ClosePathIdentifier(PathIdentifier& pathIdentifier);
    virtual void VisitPathItem(PathItem& pathItem);
    virtual void OpenInitializer(Initializer& initializer);
    virtual void CloseInitializer(Initializer& initializer);
    virtual void OpenConstantDeclaration(ConstantDeclaration& constantDeclaration);
    virtual void CloseConstantDeclaration(ConstantDeclaration& constantDeclaration);
    virtual void OpenVariableDeclaration(VariableDeclaration& variableDeclaration);
    virtual void CloseVariableDeclaration(VariableDeclaration& variableDeclaration);
    virtual void OpenBindingInitializer(BindingInitializer& bindingInitializer);
    virtual void CloseBindingInitializer(BindingInitializer& bindingInitializer);
    virtual void OpenPatternInitializer(PatternInitializer& patternInitializer);
    virtual void ClosePatternInitializer(PatternInitializer& patternInitializer);
    virtual void OpenAdditionalInitializer(AdditionalInitializer& additionalInitializer);
    virtual void CloseAdditionalInitializer(AdditionalInitializer& additionalInitializer);
    virtual void OpenFunctionDeclaration(FunctionDeclaration& functionDeclaration);
    virtual void CloseFunctionDeclaration(FunctionDeclaration& functionDeclaration);
    virtual void OpenInitializerDeclaration(InitializerDeclaration& initializerDeclaration);
    virtual void CloseInitializerDeclaration(InitializerDeclaration& initializerDeclaration);
    virtual void OpenOverride(Override& override);
    virtual void CloseOverride(Override& override);
    virtual void OpenStaticWord(StaticWord& staticWord);
    virtual void CloseStaticWord(StaticWord& staticWord);
    virtual void VisitIdentifierFunctionName(IdentifierFunctionName& identifierFunctionName);
    virtual void OpenFunctionSignature(FunctionSignature& functionSignature);
    virtual void CloseFunctionSignature(FunctionSignature& functionSignature);
    virtual void OpenFunctionResult(FunctionResult& functionResult);
    virtual void CloseFunctionResult(FunctionResult& functionResult);
    virtual void OpenParameterClause(ParameterClause& parameterClause);
    virtual void CloseParameterClause(ParameterClause& parameterClause);
    virtual void OpenConstParameter(ConstParameter& constParameter);
    virtual void CloseConstParameter(ConstParameter& constParameter);
    virtual void OpenVarParameter(VarParameter& varParameter);
    virtual void CloseVarParameter(VarParameter& varParameter);
    virtual void OpenThrowsClause(ThrowsClause& throwsClause);
    virtual void CloseThrowsClause(ThrowsClause& throwsClause);
    virtual void OpenEnumDeclaration(EnumDeclaration& enumDeclaration);
    virtual void CloseEnumDeclaration(EnumDeclaration& enumDeclaration);
    virtual void OpenEnumMember(EnumMember& enumMember);
    virtual void CloseEnumMember(EnumMember& enumMember);
    virtual void OpenTupleType(TupleType& tupleType);
    virtual void CloseTupleType(TupleType& tupleType);
    virtual void OpenAdditionalType(AdditionalType& additionalType);
    virtual void CloseAdditionalType(AdditionalType& additionalType);
    virtual void VisitEnumCase(EnumCase& enumCase);
    virtual void OpenAdditionalCase(AdditionalCase& additionalCase);
    virtual void CloseAdditionalCase(AdditionalCase& additionalCase);
    virtual void OpenClassDeclaration(ClassDeclaration& classDeclaration);
    virtual void CloseClassDeclaration(ClassDeclaration& classDeclaration);
    virtual void OpenGenericArgumentClause(GenericArgumentClause& genericArgumentClause);
    virtual void CloseGenericArgumentClause(GenericArgumentClause& genericArgumentClause);
    virtual void VisitGenericParameter(GenericParameter& genericParameter);
    virtual void OpenClassMember(ClassMember& classMember);
    virtual void CloseClassMember(ClassMember& classMember);
    virtual void OpenCodeBlock(CodeBlock& codeBlock);
    virtual void CloseCodeBlock(CodeBlock& codeBlock);
    virtual void OpenSimpleExpression(SimpleExpression& simpleExpression);
    virtual void CloseSimpleExpression(SimpleExpression& simpleExpression);
    virtual void OpenPrefixExpression(PrefixExpression& prefixExpression);
    virtual void ClosePrefixExpression(PrefixExpression& prefixExpression);
    virtual void OpenPostfixExpression(PostfixExpression& postfixExpression);
    virtual void ClosePostfixExpression(PostfixExpression& postfixExpression);
    virtual void OpenBinaryOperation(BinaryOperation& binaryOperation);
    virtual void CloseBinaryOperation(BinaryOperation& binaryOperation);
    virtual void OpenAssignment(Assignment& assignment);
    virtual void CloseAssignment(Assignment& assignment);
    virtual void OpenTypeQuery(TypeQuery& typeQuery);
    virtual void CloseTypeQuery(TypeQuery& typeQuery);
    virtual void OpenTypeCast(TypeCast& typeCast);
    virtual void CloseTypeCast(TypeCast& typeCast);
    virtual void OpenCatchClause(CatchClause& catchClause);
    virtual void CloseCatchClause(CatchClause& catchClause);
    virtual void OpenWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern);
    virtual void CloseWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern);
    virtual void OpenPathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern);
    virtual void ClosePathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern);
    virtual void VisitOperatorPostfix(OperatorPostfix& operatorPostfix);
    virtual void OpenFunctionCall(FunctionCall& functionCall);
    virtual void CloseFunctionCall(FunctionCall& functionCall);
    virtual void OpenExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression);
    virtual void CloseExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression);
    virtual void OpenSubscript(Subscript& subscript);
    virtual void CloseSubscript(Subscript& subscript);
    virtual void OpenExpressionElement(ExpressionElement& expressionElement);
    virtual void CloseExpressionElement(ExpressionElement& expressionElement);
    virtual void OpenNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix);
    virtual void CloseNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix);
    virtual void OpenParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression);
    virtual void CloseParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression);
    virtual void VisitLiteralExpression(LiteralExpression& literalExpression);
    virtual void VisitIdentifierExpression(IdentifierExpression& identifierExpression);
    virtual void OpenIfExpression(IfExpression& ifExpression);
    virtual void CloseIfExpression(IfExpression& ifExpression);
    virtual void OpenElseClause(ElseClause& elseClause);
    virtual void CloseElseClause(ElseClause& elseClause);
    virtual void OpenSwitchExpression(SwitchExpression& switchExpression);
    virtual void CloseSwitchExpression(SwitchExpression& switchExpression);
    virtual void OpenCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody);
    virtual void CloseCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody);
    virtual void OpenNakedSwitchBody(NakedSwitchBody& nakedSwitchBody);
    virtual void CloseNakedSwitchBody(NakedSwitchBody& nakedSwitchBody);
    virtual void OpenSwitchCase(SwitchCase& switchCase);
    virtual void CloseSwitchCase(SwitchCase& switchCase);
    virtual void OpenItemCaseLabel(ItemCaseLabel& itemCaseLabel);
    virtual void CloseItemCaseLabel(ItemCaseLabel& itemCaseLabel);
    virtual void OpenCaseItem(CaseItem& caseItem);
    virtual void CloseCaseItem(CaseItem& caseItem);
    virtual void OpenForExpression(ForExpression& forExpression);
    virtual void CloseForExpression(ForExpression& forExpression);
    virtual void OpenForEach(ForEach& forEach);
    virtual void CloseForEach(ForEach& forEach);
    virtual void OpenPlainFor(PlainFor& plainFor);
    virtual void ClosePlainFor(PlainFor& plainFor);
    virtual void OpenReturnExpression(ReturnExpression& returnExpression);
    virtual void CloseReturnExpression(ReturnExpression& returnExpression);
    virtual void OpenThrowExpression(ThrowExpression& throwExpression);
    virtual void CloseThrowExpression(ThrowExpression& throwExpression);
    virtual void OpenBreakExpression(BreakExpression& breakExpression);
    virtual void CloseBreakExpression(BreakExpression& breakExpression);
    virtual void OpenWildcardPattern(WildcardPattern& wildcardPattern);
    virtual void CloseWildcardPattern(WildcardPattern& wildcardPattern);
    virtual void OpenIdentifierPattern(IdentifierPattern& identifierPattern);
    virtual void CloseIdentifierPattern(IdentifierPattern& identifierPattern);
    virtual void OpenTuplePattern(TuplePattern& tuplePattern);
    virtual void CloseTuplePattern(TuplePattern& tuplePattern);
    virtual void OpenTuplePatternElement(TuplePatternElement& tuplePatternElement);
    virtual void CloseTuplePatternElement(TuplePatternElement& tuplePatternElement);
    virtual void OpenExpressionPattern(ExpressionPattern& expressionPattern);
    virtual void CloseExpressionPattern(ExpressionPattern& expressionPattern);
    virtual void OpenDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel);
    virtual void CloseDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel);
    virtual void OpenBlockCaseContent(BlockCaseContent& blockCaseContent);
    virtual void CloseBlockCaseContent(BlockCaseContent& blockCaseContent);
    virtual void OpenEmptyCaseContent(EmptyCaseContent& emptyCaseContent);
    virtual void CloseEmptyCaseContent(EmptyCaseContent& emptyCaseContent);
    virtual void OpenInitializerCall(InitializerCall& initializerCall);
    virtual void CloseInitializerCall(InitializerCall& initializerCall);
    virtual void OpenThisDot(ThisDot& thisDot);
    virtual void CloseThisDot(ThisDot& thisDot);
    virtual void OpenThisSubscript(ThisSubscript& thisSubscript);
    virtual void CloseThisSubscript(ThisSubscript& thisSubscript);
    virtual void OpenThisWord(ThisWord& thisWord);
    virtual void CloseThisWord(ThisWord& thisWord);
    virtual void OpenThisInit(ThisInit& thisInit);
    virtual void CloseThisInit(ThisInit& thisInit);
    virtual void VisitThisMember(ThisMember& thisMember);
    virtual void OpenSuperDot(SuperDot& superDot);
    virtual void CloseSuperDot(SuperDot& superDot);
    virtual void OpenSuperSubscript(SuperSubscript& superSubscript);
    virtual void CloseSuperSubscript(SuperSubscript& superSubscript);
    virtual void OpenSuperInit(SuperInit& superInit);
    virtual void CloseSuperInit(SuperInit& superInit);
    virtual void VisitSuperMember(SuperMember& superMember);
    virtual void OpenTypeAnnotation(TypeAnnotation& typeAnnotation);
    virtual void CloseTypeAnnotation(TypeAnnotation& typeAnnotation);
    virtual void OpenTypeIdentifier(TypeIdentifier& typeIdentifier);
    virtual void CloseTypeIdentifier(TypeIdentifier& typeIdentifier);
    virtual void OpenSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier);
    virtual void CloseSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier);
    virtual void OpenArrayType(ArrayType& arrayType);
    virtual void CloseArrayType(ArrayType& arrayType);
    virtual void OpenOptionalType(OptionalType& optionalType);
    virtual void CloseOptionalType(OptionalType& optionalType);
    virtual void OpenTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause);
    virtual void CloseTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause);
    virtual void OpenInheritance(Inheritance& inheritance);
    virtual void CloseInheritance(Inheritance& inheritance);
};

}
#endif // __scalyc__CppVisitor__
