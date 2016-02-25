#ifndef __scalyc__Parser__
#define __scalyc__Parser__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Parser : public Object {
public:
    Parser(_LetString* fileName, _LetString* text);

    _Result<CompilationUnit, ParserError> parseCompilationUnit(_Page* _rp, _Page* _ep);

    _Result<_Vector<StatementWithSemicolon>, ParserError> parseStatementWithSemicolonList(_Page* _rp, _Page* _ep);
    _Result<StatementWithSemicolon, ParserError> parseStatementWithSemicolon(_Page* _rp, _Page* _ep);

    _Result<Statement, ParserError> parseStatement(_Page* _rp, _Page* _ep);

    _Result<Declaration, ParserError> parseDeclaration(_Page* _rp, _Page* _ep);

    _Result<UseDeclaration, ParserError> parseUseDeclaration(_Page* _rp, _Page* _ep);

    _Result<ConstantDeclaration, ParserError> parseConstantDeclaration(_Page* _rp, _Page* _ep);

    _Result<VariableDeclaration, ParserError> parseVariableDeclaration(_Page* _rp, _Page* _ep);

    _Result<FunctionDeclaration, ParserError> parseFunctionDeclaration(_Page* _rp, _Page* _ep);

    _Result<EnumDeclaration, ParserError> parseEnumDeclaration(_Page* _rp, _Page* _ep);

    _Result<ClassDeclaration, ParserError> parseClassDeclaration(_Page* _rp, _Page* _ep);

    _Result<InitializerDeclaration, ParserError> parseInitializerDeclaration(_Page* _rp, _Page* _ep);

    _Result<Expression, ParserError> parseExpression(_Page* _rp, _Page* _ep);

    _Result<CodeBlock, ParserError> parseCodeBlock(_Page* _rp, _Page* _ep);

    _Result<SimpleExpression, ParserError> parseSimpleExpression(_Page* _rp, _Page* _ep);

    _Result<_Vector<PathIdentifier>, ParserError> parsePathIdentifierList(_Page* _rp, _Page* _ep);
    _Result<PathIdentifier, ParserError> parsePathIdentifier(_Page* _rp, _Page* _ep);

    _Result<PathItem, ParserError> parsePathItem(_Page* _rp, _Page* _ep);

    _Result<Initializer, ParserError> parseInitializer(_Page* _rp, _Page* _ep);

    _Result<BindingInitializer, ParserError> parseBindingInitializer(_Page* _rp, _Page* _ep);

    _Result<_Vector<PatternInitializer>, ParserError> parsePatternInitializerList(_Page* _rp, _Page* _ep);
    _Result<PatternInitializer, ParserError> parsePatternInitializer(_Page* _rp, _Page* _ep);

    _Result<_Vector<AdditionalInitializer>, ParserError> parseAdditionalInitializerList(_Page* _rp, _Page* _ep);
    _Result<AdditionalInitializer, ParserError> parseAdditionalInitializer(_Page* _rp, _Page* _ep);

    _Result<_Vector<Modifier>, ParserError> parseModifierList(_Page* _rp, _Page* _ep);
    _Result<Modifier, ParserError> parseModifier(_Page* _rp, _Page* _ep);

    _Result<OverrideWord, ParserError> parseOverrideWord(_Page* _rp, _Page* _ep);

    _Result<StaticWord, ParserError> parseStaticWord(_Page* _rp, _Page* _ep);

    _Result<FunctionName, ParserError> parseFunctionName(_Page* _rp, _Page* _ep);

    _Result<IdentifierFunction, ParserError> parseIdentifierFunction(_Page* _rp, _Page* _ep);

    _Result<FunctionSignature, ParserError> parseFunctionSignature(_Page* _rp, _Page* _ep);

    _Result<FunctionResult, ParserError> parseFunctionResult(_Page* _rp, _Page* _ep);

    _Result<_Vector<ParameterClause>, ParserError> parseParameterClauseList(_Page* _rp, _Page* _ep);
    _Result<ParameterClause, ParserError> parseParameterClause(_Page* _rp, _Page* _ep);

    _Result<_Vector<Parameter>, ParserError> parseParameterList(_Page* _rp, _Page* _ep);
    _Result<Parameter, ParserError> parseParameter(_Page* _rp, _Page* _ep);

    _Result<ConstParameter, ParserError> parseConstParameter(_Page* _rp, _Page* _ep);

    _Result<VarParameter, ParserError> parseVarParameter(_Page* _rp, _Page* _ep);

    _Result<ThrowsClause, ParserError> parseThrowsClause(_Page* _rp, _Page* _ep);

    _Result<_Vector<EnumMember>, ParserError> parseEnumMemberList(_Page* _rp, _Page* _ep);
    _Result<EnumMember, ParserError> parseEnumMember(_Page* _rp, _Page* _ep);

    _Result<TupleType, ParserError> parseTupleType(_Page* _rp, _Page* _ep);

    _Result<_Vector<AdditionalType>, ParserError> parseAdditionalTypeList(_Page* _rp, _Page* _ep);
    _Result<AdditionalType, ParserError> parseAdditionalType(_Page* _rp, _Page* _ep);

    _Result<EnumCase, ParserError> parseEnumCase(_Page* _rp, _Page* _ep);

    _Result<_Vector<AdditionalCase>, ParserError> parseAdditionalCaseList(_Page* _rp, _Page* _ep);
    _Result<AdditionalCase, ParserError> parseAdditionalCase(_Page* _rp, _Page* _ep);

    _Result<ClassBody, ParserError> parseClassBody(_Page* _rp, _Page* _ep);

    _Result<GenericArgumentClause, ParserError> parseGenericArgumentClause(_Page* _rp, _Page* _ep);

    _Result<_Vector<GenericParameter>, ParserError> parseGenericParameterList(_Page* _rp, _Page* _ep);
    _Result<GenericParameter, ParserError> parseGenericParameter(_Page* _rp, _Page* _ep);

    _Result<_Vector<ClassMember>, ParserError> parseClassMemberList(_Page* _rp, _Page* _ep);
    _Result<ClassMember, ParserError> parseClassMember(_Page* _rp, _Page* _ep);

    _Result<PrefixExpression, ParserError> parsePrefixExpression(_Page* _rp, _Page* _ep);

    _Result<PostfixExpression, ParserError> parsePostfixExpression(_Page* _rp, _Page* _ep);

    _Result<_Vector<BinaryOp>, ParserError> parseBinaryOpList(_Page* _rp, _Page* _ep);
    _Result<BinaryOp, ParserError> parseBinaryOp(_Page* _rp, _Page* _ep);

    _Result<BinaryOperation, ParserError> parseBinaryOperation(_Page* _rp, _Page* _ep);

    _Result<Assignment, ParserError> parseAssignment(_Page* _rp, _Page* _ep);

    _Result<TypeQuery, ParserError> parseTypeQuery(_Page* _rp, _Page* _ep);

    _Result<TypeCast, ParserError> parseTypeCast(_Page* _rp, _Page* _ep);

    _Result<_Vector<CatchClause>, ParserError> parseCatchClauseList(_Page* _rp, _Page* _ep);
    _Result<CatchClause, ParserError> parseCatchClause(_Page* _rp, _Page* _ep);

    _Result<CatchPattern, ParserError> parseCatchPattern(_Page* _rp, _Page* _ep);

    _Result<WildCardCatchPattern, ParserError> parseWildCardCatchPattern(_Page* _rp, _Page* _ep);

    _Result<PathItemCatchPattern, ParserError> parsePathItemCatchPattern(_Page* _rp, _Page* _ep);

    _Result<_Vector<Postfix>, ParserError> parsePostfixList(_Page* _rp, _Page* _ep);
    _Result<Postfix, ParserError> parsePostfix(_Page* _rp, _Page* _ep);

    _Result<OperatorPostfix, ParserError> parseOperatorPostfix(_Page* _rp, _Page* _ep);

    _Result<FunctionCall, ParserError> parseFunctionCall(_Page* _rp, _Page* _ep);

    _Result<ExplicitMemberExpression, ParserError> parseExplicitMemberExpression(_Page* _rp, _Page* _ep);

    _Result<Subscript, ParserError> parseSubscript(_Page* _rp, _Page* _ep);

    _Result<_Vector<ExpressionElement>, ParserError> parseExpressionElementList(_Page* _rp, _Page* _ep);
    _Result<ExpressionElement, ParserError> parseExpressionElement(_Page* _rp, _Page* _ep);

    _Result<MemberPostfix, ParserError> parseMemberPostfix(_Page* _rp, _Page* _ep);

    _Result<NamedMemberPostfix, ParserError> parseNamedMemberPostfix(_Page* _rp, _Page* _ep);

    _Result<PrimaryExpression, ParserError> parsePrimaryExpression(_Page* _rp, _Page* _ep);

    _Result<ParenthesizedExpression, ParserError> parseParenthesizedExpression(_Page* _rp, _Page* _ep);

    _Result<LiteralExpression, ParserError> parseLiteralExpression(_Page* _rp, _Page* _ep);

    _Result<IdentifierExpression, ParserError> parseIdentifierExpression(_Page* _rp, _Page* _ep);

    _Result<IfExpression, ParserError> parseIfExpression(_Page* _rp, _Page* _ep);

    _Result<SwitchExpression, ParserError> parseSwitchExpression(_Page* _rp, _Page* _ep);

    _Result<ForExpression, ParserError> parseForExpression(_Page* _rp, _Page* _ep);

    _Result<ReturnExpression, ParserError> parseReturnExpression(_Page* _rp, _Page* _ep);

    _Result<ThrowExpression, ParserError> parseThrowExpression(_Page* _rp, _Page* _ep);

    _Result<BreakExpression, ParserError> parseBreakExpression(_Page* _rp, _Page* _ep);

    _Result<InitializerCall, ParserError> parseInitializerCall(_Page* _rp, _Page* _ep);

    _Result<ThisExpression, ParserError> parseThisExpression(_Page* _rp, _Page* _ep);

    _Result<ThisDot, ParserError> parseThisDot(_Page* _rp, _Page* _ep);

    _Result<ThisSubscript, ParserError> parseThisSubscript(_Page* _rp, _Page* _ep);

    _Result<ThisWord, ParserError> parseThisWord(_Page* _rp, _Page* _ep);

    _Result<SuperExpression, ParserError> parseSuperExpression(_Page* _rp, _Page* _ep);

    _Result<SuperDot, ParserError> parseSuperDot(_Page* _rp, _Page* _ep);

    _Result<SuperSubscript, ParserError> parseSuperSubscript(_Page* _rp, _Page* _ep);

    _Result<ElseClause, ParserError> parseElseClause(_Page* _rp, _Page* _ep);

    _Result<SwitchBody, ParserError> parseSwitchBody(_Page* _rp, _Page* _ep);

    _Result<CurliedSwitchBody, ParserError> parseCurliedSwitchBody(_Page* _rp, _Page* _ep);

    _Result<NakedSwitchBody, ParserError> parseNakedSwitchBody(_Page* _rp, _Page* _ep);

    _Result<_Vector<SwitchCase>, ParserError> parseSwitchCaseList(_Page* _rp, _Page* _ep);
    _Result<SwitchCase, ParserError> parseSwitchCase(_Page* _rp, _Page* _ep);

    _Result<CaseLabel, ParserError> parseCaseLabel(_Page* _rp, _Page* _ep);

    _Result<ItemCaseLabel, ParserError> parseItemCaseLabel(_Page* _rp, _Page* _ep);

    _Result<DefaultCaseLabel, ParserError> parseDefaultCaseLabel(_Page* _rp, _Page* _ep);

    _Result<_Vector<CaseItem>, ParserError> parseCaseItemList(_Page* _rp, _Page* _ep);
    _Result<CaseItem, ParserError> parseCaseItem(_Page* _rp, _Page* _ep);

    _Result<ForLoop, ParserError> parseForLoop(_Page* _rp, _Page* _ep);

    _Result<ForEach, ParserError> parseForEach(_Page* _rp, _Page* _ep);

    _Result<PlainFor, ParserError> parsePlainFor(_Page* _rp, _Page* _ep);

    _Result<Pattern, ParserError> parsePattern(_Page* _rp, _Page* _ep);

    _Result<WildcardPattern, ParserError> parseWildcardPattern(_Page* _rp, _Page* _ep);

    _Result<IdentifierPattern, ParserError> parseIdentifierPattern(_Page* _rp, _Page* _ep);

    _Result<TuplePattern, ParserError> parseTuplePattern(_Page* _rp, _Page* _ep);

    _Result<ExpressionPattern, ParserError> parseExpressionPattern(_Page* _rp, _Page* _ep);

    _Result<_Vector<TuplePatternElement>, ParserError> parseTuplePatternElementList(_Page* _rp, _Page* _ep);
    _Result<TuplePatternElement, ParserError> parseTuplePatternElement(_Page* _rp, _Page* _ep);

    _Result<CaseContent, ParserError> parseCaseContent(_Page* _rp, _Page* _ep);

    _Result<BlockCaseContent, ParserError> parseBlockCaseContent(_Page* _rp, _Page* _ep);

    _Result<EmptyCaseContent, ParserError> parseEmptyCaseContent(_Page* _rp, _Page* _ep);

    _Result<CommonThisMember, ParserError> parseCommonThisMember(_Page* _rp, _Page* _ep);

    _Result<ThisInit, ParserError> parseThisInit(_Page* _rp, _Page* _ep);

    _Result<ThisMember, ParserError> parseThisMember(_Page* _rp, _Page* _ep);

    _Result<CommonSuperMember, ParserError> parseCommonSuperMember(_Page* _rp, _Page* _ep);

    _Result<SuperInit, ParserError> parseSuperInit(_Page* _rp, _Page* _ep);

    _Result<SuperMember, ParserError> parseSuperMember(_Page* _rp, _Page* _ep);

    _Result<Type, ParserError> parseType(_Page* _rp, _Page* _ep);

    _Result<TypeIdentifier, ParserError> parseTypeIdentifier(_Page* _rp, _Page* _ep);

    _Result<ArrayType, ParserError> parseArrayType(_Page* _rp, _Page* _ep);

    _Result<TypeAnnotation, ParserError> parseTypeAnnotation(_Page* _rp, _Page* _ep);

    _Result<SubtypeIdentifier, ParserError> parseSubtypeIdentifier(_Page* _rp, _Page* _ep);

    _Result<_Vector<TypePostfix>, ParserError> parseTypePostfixList(_Page* _rp, _Page* _ep);
    _Result<TypePostfix, ParserError> parseTypePostfix(_Page* _rp, _Page* _ep);

    _Result<OptionalType, ParserError> parseOptionalType(_Page* _rp, _Page* _ep);

    _Result<TypeInheritanceClause, ParserError> parseTypeInheritanceClause(_Page* _rp, _Page* _ep);

    _Result<_Vector<Inheritance>, ParserError> parseInheritanceList(_Page* _rp, _Page* _ep);
    _Result<Inheritance, ParserError> parseInheritance(_Page* _rp, _Page* _ep);

    bool isAtEnd();
    bool isIdentifier(_LetString& id);

private:
    Lexer lexer;
    _LetString* fileName;

    _LetString* useKeyword;
    _LetString* classKeyword;
    _LetString* functionKeyword;
    _LetString* ifKeyword;
    _LetString* elseKeyword;
    _LetString* switchKeyword;
    _LetString* caseKeyword;
    _LetString* defaultKeyword;
    _LetString* catchKeyword;
    _LetString* forKeyword;
    _LetString* inKeyword;
    _LetString* returnKeyword;
    _LetString* throwKeyword;
    _LetString* breakKeyword;
    _LetString* throwsKeyword;
    _LetString* overrideKeyword;
    _LetString* staticKeyword;
    _LetString* letKeyword;
    _LetString* mutableKeyword;
    _LetString* isKeyword;
    _LetString* asKeyword;
    _LetString* initKeyword;
    _LetString* enumKeyword;
    _LetString* superKeyword;
    _LetString* thisKeyword;
    _LetString* semicolon;
    _LetString* equal;
    _LetString* leftAngular;
    _LetString* rightAngular;
    _LetString* comma;
    _LetString* leftParen;
    _LetString* rightParen;
    _LetString* leftCurly;
    _LetString* rightCurly;
    _LetString* leftBracket;
    _LetString* rightBracket;
    _LetString* colon;
    _LetString* dot;
    _LetString* questionMark;
    _LetString* underscore;
    _LetString* evaluatesTo;
};

}

#endif /* defined(__scalyc__Parser__) */
