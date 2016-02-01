#ifndef __scalyc__Visitor__
#define __scalyc__Visitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Program;
class CompilationUnit;
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
class StaticWord;
class FunctionName;
class IdentifierFunction;
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
class ForExpression;
class ForLoop;
class ForEach;
class PlainFor;
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
class ThisWord;
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

class SyntaxVisitor : public Object{
public:
    virtual void OpenProgram(Program& program)= 0;
    virtual void CloseProgram(Program& program)= 0;
    virtual void OpenCompilationUnit(CompilationUnit& compilationUnit)= 0;
    virtual void CloseCompilationUnit(CompilationUnit& compilationUnit)= 0;
    virtual void OpenStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon)= 0;
    virtual void CloseStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon)= 0;
    virtual void OpenUseDeclaration(UseDeclaration& useDeclaration)= 0;
    virtual void CloseUseDeclaration(UseDeclaration& useDeclaration)= 0;
    virtual void OpenPathIdentifier(PathIdentifier& pathIdentifier)= 0;
    virtual void ClosePathIdentifier(PathIdentifier& pathIdentifier)= 0;
    virtual void VisitPathItem(PathItem& pathItem)= 0;
    virtual void OpenInitializer(Initializer& initializer)= 0;
    virtual void CloseInitializer(Initializer& initializer)= 0;
    virtual void OpenConstantDeclaration(ConstantDeclaration& constantDeclaration)= 0;
    virtual void CloseConstantDeclaration(ConstantDeclaration& constantDeclaration)= 0;
    virtual void OpenVariableDeclaration(VariableDeclaration& variableDeclaration)= 0;
    virtual void CloseVariableDeclaration(VariableDeclaration& variableDeclaration)= 0;
    virtual void OpenBindingInitializer(BindingInitializer& bindingInitializer)= 0;
    virtual void CloseBindingInitializer(BindingInitializer& bindingInitializer)= 0;
    virtual void OpenPatternInitializer(PatternInitializer& patternInitializer)= 0;
    virtual void ClosePatternInitializer(PatternInitializer& patternInitializer)= 0;
    virtual void OpenAdditionalInitializer(AdditionalInitializer& additionalInitializer)= 0;
    virtual void CloseAdditionalInitializer(AdditionalInitializer& additionalInitializer)= 0;
    virtual void OpenFunctionDeclaration(FunctionDeclaration& functionDeclaration)= 0;
    virtual void CloseFunctionDeclaration(FunctionDeclaration& functionDeclaration)= 0;
    virtual void OpenInitializerDeclaration(InitializerDeclaration& initializerDeclaration)= 0;
    virtual void CloseInitializerDeclaration(InitializerDeclaration& initializerDeclaration)= 0;
    virtual void VisitOverride(Override& override)= 0;
    virtual void VisitStaticWord(StaticWord& staticWord)= 0;
    virtual void VisitIdentifierFunction(IdentifierFunction& identifierFunction)= 0;
    virtual void OpenFunctionSignature(FunctionSignature& functionSignature)= 0;
    virtual void CloseFunctionSignature(FunctionSignature& functionSignature)= 0;
    virtual void OpenFunctionResult(FunctionResult& functionResult)= 0;
    virtual void CloseFunctionResult(FunctionResult& functionResult)= 0;
    virtual void OpenParameterClause(ParameterClause& parameterClause)= 0;
    virtual void CloseParameterClause(ParameterClause& parameterClause)= 0;
    virtual void OpenConstParameter(ConstParameter& constParameter)= 0;
    virtual void CloseConstParameter(ConstParameter& constParameter)= 0;
    virtual void OpenVarParameter(VarParameter& varParameter)= 0;
    virtual void CloseVarParameter(VarParameter& varParameter)= 0;
    virtual void OpenThrowsClause(ThrowsClause& throwsClause)= 0;
    virtual void CloseThrowsClause(ThrowsClause& throwsClause)= 0;
    virtual void OpenEnumDeclaration(EnumDeclaration& enumDeclaration)= 0;
    virtual void CloseEnumDeclaration(EnumDeclaration& enumDeclaration)= 0;
    virtual void OpenEnumMember(EnumMember& enumMember)= 0;
    virtual void CloseEnumMember(EnumMember& enumMember)= 0;
    virtual void OpenTupleType(TupleType& tupleType)= 0;
    virtual void CloseTupleType(TupleType& tupleType)= 0;
    virtual void OpenAdditionalType(AdditionalType& additionalType)= 0;
    virtual void CloseAdditionalType(AdditionalType& additionalType)= 0;
    virtual void VisitEnumCase(EnumCase& enumCase)= 0;
    virtual void OpenAdditionalCase(AdditionalCase& additionalCase)= 0;
    virtual void CloseAdditionalCase(AdditionalCase& additionalCase)= 0;
    virtual void OpenClassDeclaration(ClassDeclaration& classDeclaration)= 0;
    virtual void CloseClassDeclaration(ClassDeclaration& classDeclaration)= 0;
    virtual void OpenGenericArgumentClause(GenericArgumentClause& genericArgumentClause)= 0;
    virtual void CloseGenericArgumentClause(GenericArgumentClause& genericArgumentClause)= 0;
    virtual void VisitGenericParameter(GenericParameter& genericParameter)= 0;
    virtual void OpenClassMember(ClassMember& classMember)= 0;
    virtual void CloseClassMember(ClassMember& classMember)= 0;
    virtual void OpenCodeBlock(CodeBlock& codeBlock)= 0;
    virtual void CloseCodeBlock(CodeBlock& codeBlock)= 0;
    virtual void OpenSimpleExpression(SimpleExpression& simpleExpression)= 0;
    virtual void CloseSimpleExpression(SimpleExpression& simpleExpression)= 0;
    virtual void OpenPrefixExpression(PrefixExpression& prefixExpression)= 0;
    virtual void ClosePrefixExpression(PrefixExpression& prefixExpression)= 0;
    virtual void OpenPostfixExpression(PostfixExpression& postfixExpression)= 0;
    virtual void ClosePostfixExpression(PostfixExpression& postfixExpression)= 0;
    virtual void OpenBinaryOperation(BinaryOperation& binaryOperation)= 0;
    virtual void CloseBinaryOperation(BinaryOperation& binaryOperation)= 0;
    virtual void OpenAssignment(Assignment& assignment)= 0;
    virtual void CloseAssignment(Assignment& assignment)= 0;
    virtual void OpenTypeQuery(TypeQuery& typeQuery)= 0;
    virtual void CloseTypeQuery(TypeQuery& typeQuery)= 0;
    virtual void OpenTypeCast(TypeCast& typeCast)= 0;
    virtual void CloseTypeCast(TypeCast& typeCast)= 0;
    virtual void OpenCatchClause(CatchClause& catchClause)= 0;
    virtual void CloseCatchClause(CatchClause& catchClause)= 0;
    virtual void OpenWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern)= 0;
    virtual void CloseWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern)= 0;
    virtual void OpenPathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern)= 0;
    virtual void ClosePathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern)= 0;
    virtual void VisitOperatorPostfix(OperatorPostfix& operatorPostfix)= 0;
    virtual void OpenFunctionCall(FunctionCall& functionCall)= 0;
    virtual void CloseFunctionCall(FunctionCall& functionCall)= 0;
    virtual void OpenExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression)= 0;
    virtual void CloseExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression)= 0;
    virtual void OpenSubscript(Subscript& subscript)= 0;
    virtual void CloseSubscript(Subscript& subscript)= 0;
    virtual void OpenExpressionElement(ExpressionElement& expressionElement)= 0;
    virtual void CloseExpressionElement(ExpressionElement& expressionElement)= 0;
    virtual void OpenNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix)= 0;
    virtual void CloseNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix)= 0;
    virtual void OpenParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression)= 0;
    virtual void CloseParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression)= 0;
    virtual void VisitLiteralExpression(LiteralExpression& literalExpression)= 0;
    virtual void VisitIdentifierExpression(IdentifierExpression& identifierExpression)= 0;
    virtual void OpenIfExpression(IfExpression& ifExpression)= 0;
    virtual void CloseIfExpression(IfExpression& ifExpression)= 0;
    virtual void OpenElseClause(ElseClause& elseClause)= 0;
    virtual void CloseElseClause(ElseClause& elseClause)= 0;
    virtual void OpenSwitchExpression(SwitchExpression& switchExpression)= 0;
    virtual void CloseSwitchExpression(SwitchExpression& switchExpression)= 0;
    virtual void OpenCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody)= 0;
    virtual void CloseCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody)= 0;
    virtual void OpenNakedSwitchBody(NakedSwitchBody& nakedSwitchBody)= 0;
    virtual void CloseNakedSwitchBody(NakedSwitchBody& nakedSwitchBody)= 0;
    virtual void OpenSwitchCase(SwitchCase& switchCase)= 0;
    virtual void CloseSwitchCase(SwitchCase& switchCase)= 0;
    virtual void OpenItemCaseLabel(ItemCaseLabel& itemCaseLabel)= 0;
    virtual void CloseItemCaseLabel(ItemCaseLabel& itemCaseLabel)= 0;
    virtual void OpenCaseItem(CaseItem& caseItem)= 0;
    virtual void CloseCaseItem(CaseItem& caseItem)= 0;
    virtual void OpenForExpression(ForExpression& forExpression)= 0;
    virtual void CloseForExpression(ForExpression& forExpression)= 0;
    virtual void OpenForEach(ForEach& forEach)= 0;
    virtual void CloseForEach(ForEach& forEach)= 0;
    virtual void OpenPlainFor(PlainFor& plainFor)= 0;
    virtual void ClosePlainFor(PlainFor& plainFor)= 0;
    virtual void OpenReturnExpression(ReturnExpression& returnExpression)= 0;
    virtual void CloseReturnExpression(ReturnExpression& returnExpression)= 0;
    virtual void OpenThrowExpression(ThrowExpression& throwExpression)= 0;
    virtual void CloseThrowExpression(ThrowExpression& throwExpression)= 0;
    virtual void OpenBreakExpression(BreakExpression& breakExpression)= 0;
    virtual void CloseBreakExpression(BreakExpression& breakExpression)= 0;
    virtual void VisitWildcardPattern(WildcardPattern& wildcardPattern)= 0;
    virtual void OpenIdentifierPattern(IdentifierPattern& identifierPattern)= 0;
    virtual void CloseIdentifierPattern(IdentifierPattern& identifierPattern)= 0;
    virtual void OpenTuplePattern(TuplePattern& tuplePattern)= 0;
    virtual void CloseTuplePattern(TuplePattern& tuplePattern)= 0;
    virtual void OpenTuplePatternElement(TuplePatternElement& tuplePatternElement)= 0;
    virtual void CloseTuplePatternElement(TuplePatternElement& tuplePatternElement)= 0;
    virtual void OpenExpressionPattern(ExpressionPattern& expressionPattern)= 0;
    virtual void CloseExpressionPattern(ExpressionPattern& expressionPattern)= 0;
    virtual void VisitDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel)= 0;
    virtual void OpenBlockCaseContent(BlockCaseContent& blockCaseContent)= 0;
    virtual void CloseBlockCaseContent(BlockCaseContent& blockCaseContent)= 0;
    virtual void VisitEmptyCaseContent(EmptyCaseContent& emptyCaseContent)= 0;
    virtual void OpenInitializerCall(InitializerCall& initializerCall)= 0;
    virtual void CloseInitializerCall(InitializerCall& initializerCall)= 0;
    virtual void OpenThisDot(ThisDot& thisDot)= 0;
    virtual void CloseThisDot(ThisDot& thisDot)= 0;
    virtual void OpenThisSubscript(ThisSubscript& thisSubscript)= 0;
    virtual void CloseThisSubscript(ThisSubscript& thisSubscript)= 0;
    virtual void VisitThisWord(ThisWord& thisWord)= 0;
    virtual void VisitThisInit(ThisInit& thisInit)= 0;
    virtual void VisitThisMember(ThisMember& thisMember)= 0;
    virtual void OpenSuperDot(SuperDot& superDot)= 0;
    virtual void CloseSuperDot(SuperDot& superDot)= 0;
    virtual void OpenSuperSubscript(SuperSubscript& superSubscript)= 0;
    virtual void CloseSuperSubscript(SuperSubscript& superSubscript)= 0;
    virtual void VisitSuperInit(SuperInit& superInit)= 0;
    virtual void VisitSuperMember(SuperMember& superMember)= 0;
    virtual void OpenTypeAnnotation(TypeAnnotation& typeAnnotation)= 0;
    virtual void CloseTypeAnnotation(TypeAnnotation& typeAnnotation)= 0;
    virtual void OpenTypeIdentifier(TypeIdentifier& typeIdentifier)= 0;
    virtual void CloseTypeIdentifier(TypeIdentifier& typeIdentifier)= 0;
    virtual void OpenSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier)= 0;
    virtual void CloseSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier)= 0;
    virtual void OpenArrayType(ArrayType& arrayType)= 0;
    virtual void CloseArrayType(ArrayType& arrayType)= 0;
    virtual void VisitOptionalType(OptionalType& optionalType)= 0;
    virtual void OpenTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause)= 0;
    virtual void CloseTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause)= 0;
    virtual void OpenInheritance(Inheritance& inheritance)= 0;
    virtual void CloseInheritance(Inheritance& inheritance)= 0;
};

}
#endif // __scalyc__Visitor__
