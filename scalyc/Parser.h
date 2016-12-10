#ifndef __scalyc__Parser__
#define __scalyc__Parser__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Program;

class CompilationUnit;

class Statement;

class Declaration;

class ConstantDeclaration;

class MutableDeclaration;

class BindingInitializer;

class IdentifierInitializer;

class Initializer;

class AdditionalInitializer;

class Pattern;

class IdentifierPattern;

class TypeAnnotation;

class Type;

class TypePostfix;

class IndexedType;

class Pointer;

class LifeTime;

class Root;

class Local;

class Reference;

class Thrown;

class FunctionDeclaration;

class Modifier;

class OverrideWord;

class StaticWord;

class FunctionSignature;

class ParameterClause;

class Parameter;

class ConstParameter;

class VarParameter;

class FunctionResult;

class ThrowsClause;

class EnumDeclaration;

class EnumMember;

class EnumCase;

class AdditionalCase;

class ClassDeclaration;

class TypeInheritanceClause;

class Inheritance;

class ClassBody;

class ClassMember;

class ConstructorDeclaration;

class Expression;

class CodeBlock;

class SimpleExpression;

class PrefixExpression;

class PostfixExpression;

class PrimaryExpression;

class IdentifierExpression;

class LiteralExpression;

class IfExpression;

class ElseClause;

class SwitchExpression;

class SwitchBody;

class CurliedSwitchBody;

class NakedSwitchBody;

class SwitchCase;

class CaseLabel;

class ItemCaseLabel;

class DefaultCaseLabel;

class CaseItem;

class CaseContent;

class ForExpression;

class WhileExpression;

class DoExpression;

class ParenthesizedExpression;

class ExpressionElement;

class ReturnExpression;

class ThrowExpression;

class BreakExpression;

class ConstructorCall;

class ThisExpression;

class NullExpression;

class Postfix;

class OperatorPostfix;

class FunctionCall;

class CatchClause;

class CatchPattern;

class WildCardCatchPattern;

class IdentifierCatchPattern;

class WildcardPattern;

class TuplePattern;

class TuplePatternElement;

class ExpressionPattern;

class MemberExpression;

class Subscript;

class BinaryExpression;

class BinaryOperation;

class Assignment;

class TypeQuery;

class TypeCast;

class Parser : public Object {
public:
    Parser(string* theFileName, string* text);
    virtual _Result<CompilationUnit, ParserError> parseCompilationUnit(_Page* _rp, _Page* _ep);
    virtual _Array<Statement>* parseStatementList(_Page* _rp);
    virtual Statement* parseStatement(_Page* _rp);
    virtual Declaration* parseDeclaration(_Page* _rp);
    virtual ConstantDeclaration* parseConstantDeclaration(_Page* _rp);
    virtual MutableDeclaration* parseMutableDeclaration(_Page* _rp);
    virtual BindingInitializer* parseBindingInitializer(_Page* _rp);
    virtual _Array<IdentifierInitializer>* parseIdentifierInitializerList(_Page* _rp);
    virtual IdentifierInitializer* parseIdentifierInitializer(_Page* _rp);
    virtual Initializer* parseInitializer(_Page* _rp);
    virtual _Array<AdditionalInitializer>* parseAdditionalInitializerList(_Page* _rp);
    virtual AdditionalInitializer* parseAdditionalInitializer(_Page* _rp);
    virtual Pattern* parsePattern(_Page* _rp);
    virtual IdentifierPattern* parseIdentifierPattern(_Page* _rp);
    virtual TypeAnnotation* parseTypeAnnotation(_Page* _rp);
    virtual Type* parseType(_Page* _rp);
    virtual _Array<TypePostfix>* parseTypePostfixList(_Page* _rp);
    virtual TypePostfix* parseTypePostfix(_Page* _rp);
    virtual IndexedType* parseIndexedType(_Page* _rp);
    virtual Pointer* parsePointer(_Page* _rp);
    virtual LifeTime* parseLifeTime(_Page* _rp);
    virtual Root* parseRoot(_Page* _rp);
    virtual Local* parseLocal(_Page* _rp);
    virtual Reference* parseReference(_Page* _rp);
    virtual Thrown* parseThrown(_Page* _rp);
    virtual FunctionDeclaration* parseFunctionDeclaration(_Page* _rp);
    virtual _Array<Modifier>* parseModifierList(_Page* _rp);
    virtual Modifier* parseModifier(_Page* _rp);
    virtual OverrideWord* parseOverrideWord(_Page* _rp);
    virtual StaticWord* parseStaticWord(_Page* _rp);
    virtual FunctionSignature* parseFunctionSignature(_Page* _rp);
    virtual _Array<ParameterClause>* parseParameterClauseList(_Page* _rp);
    virtual ParameterClause* parseParameterClause(_Page* _rp);
    virtual _Array<Parameter>* parseParameterList(_Page* _rp);
    virtual Parameter* parseParameter(_Page* _rp);
    virtual ConstParameter* parseConstParameter(_Page* _rp);
    virtual VarParameter* parseVarParameter(_Page* _rp);
    virtual FunctionResult* parseFunctionResult(_Page* _rp);
    virtual ThrowsClause* parseThrowsClause(_Page* _rp);
    virtual EnumDeclaration* parseEnumDeclaration(_Page* _rp);
    virtual _Array<EnumMember>* parseEnumMemberList(_Page* _rp);
    virtual EnumMember* parseEnumMember(_Page* _rp);
    virtual EnumCase* parseEnumCase(_Page* _rp);
    virtual _Array<AdditionalCase>* parseAdditionalCaseList(_Page* _rp);
    virtual AdditionalCase* parseAdditionalCase(_Page* _rp);
    virtual ClassDeclaration* parseClassDeclaration(_Page* _rp);
    virtual TypeInheritanceClause* parseTypeInheritanceClause(_Page* _rp);
    virtual _Array<Inheritance>* parseInheritanceList(_Page* _rp);
    virtual Inheritance* parseInheritance(_Page* _rp);
    virtual ClassBody* parseClassBody(_Page* _rp);
    virtual _Array<ClassMember>* parseClassMemberList(_Page* _rp);
    virtual ClassMember* parseClassMember(_Page* _rp);
    virtual ConstructorDeclaration* parseConstructorDeclaration(_Page* _rp);
    virtual Expression* parseExpression(_Page* _rp);
    virtual CodeBlock* parseCodeBlock(_Page* _rp);
    virtual SimpleExpression* parseSimpleExpression(_Page* _rp);
    virtual PrefixExpression* parsePrefixExpression(_Page* _rp);
    virtual PostfixExpression* parsePostfixExpression(_Page* _rp);
    virtual PrimaryExpression* parsePrimaryExpression(_Page* _rp);
    virtual IdentifierExpression* parseIdentifierExpression(_Page* _rp);
    virtual LiteralExpression* parseLiteralExpression(_Page* _rp);
    virtual IfExpression* parseIfExpression(_Page* _rp);
    virtual ElseClause* parseElseClause(_Page* _rp);
    virtual SwitchExpression* parseSwitchExpression(_Page* _rp);
    virtual SwitchBody* parseSwitchBody(_Page* _rp);
    virtual CurliedSwitchBody* parseCurliedSwitchBody(_Page* _rp);
    virtual NakedSwitchBody* parseNakedSwitchBody(_Page* _rp);
    virtual _Array<SwitchCase>* parseSwitchCaseList(_Page* _rp);
    virtual SwitchCase* parseSwitchCase(_Page* _rp);
    virtual CaseLabel* parseCaseLabel(_Page* _rp);
    virtual ItemCaseLabel* parseItemCaseLabel(_Page* _rp);
    virtual DefaultCaseLabel* parseDefaultCaseLabel(_Page* _rp);
    virtual _Array<CaseItem>* parseCaseItemList(_Page* _rp);
    virtual CaseItem* parseCaseItem(_Page* _rp);
    virtual CaseContent* parseCaseContent(_Page* _rp);
    virtual ForExpression* parseForExpression(_Page* _rp);
    virtual WhileExpression* parseWhileExpression(_Page* _rp);
    virtual DoExpression* parseDoExpression(_Page* _rp);
    virtual ParenthesizedExpression* parseParenthesizedExpression(_Page* _rp);
    virtual _Array<ExpressionElement>* parseExpressionElementList(_Page* _rp);
    virtual ExpressionElement* parseExpressionElement(_Page* _rp);
    virtual ReturnExpression* parseReturnExpression(_Page* _rp);
    virtual ThrowExpression* parseThrowExpression(_Page* _rp);
    virtual BreakExpression* parseBreakExpression(_Page* _rp);
    virtual ConstructorCall* parseConstructorCall(_Page* _rp);
    virtual ThisExpression* parseThisExpression(_Page* _rp);
    virtual NullExpression* parseNullExpression(_Page* _rp);
    virtual _Array<Postfix>* parsePostfixList(_Page* _rp);
    virtual Postfix* parsePostfix(_Page* _rp);
    virtual OperatorPostfix* parseOperatorPostfix(_Page* _rp);
    virtual FunctionCall* parseFunctionCall(_Page* _rp);
    virtual _Array<CatchClause>* parseCatchClauseList(_Page* _rp);
    virtual CatchClause* parseCatchClause(_Page* _rp);
    virtual CatchPattern* parseCatchPattern(_Page* _rp);
    virtual WildCardCatchPattern* parseWildCardCatchPattern(_Page* _rp);
    virtual IdentifierCatchPattern* parseIdentifierCatchPattern(_Page* _rp);
    virtual WildcardPattern* parseWildcardPattern(_Page* _rp);
    virtual TuplePattern* parseTuplePattern(_Page* _rp);
    virtual _Array<TuplePatternElement>* parseTuplePatternElementList(_Page* _rp);
    virtual TuplePatternElement* parseTuplePatternElement(_Page* _rp);
    virtual ExpressionPattern* parseExpressionPattern(_Page* _rp);
    virtual MemberExpression* parseMemberExpression(_Page* _rp);
    virtual Subscript* parseSubscript(_Page* _rp);
    virtual _Array<BinaryExpression>* parseBinaryExpressionList(_Page* _rp);
    virtual BinaryExpression* parseBinaryExpression(_Page* _rp);
    virtual BinaryOperation* parseBinaryOperation(_Page* _rp);
    virtual Assignment* parseAssignment(_Page* _rp);
    virtual TypeQuery* parseTypeQuery(_Page* _rp);
    virtual TypeCast* parseTypeCast(_Page* _rp);
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
    string* ampersand;

};

class Visitor : public Object {
public:
    virtual bool openProgram(Program* program);
    virtual void closeProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual void closeConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual void closeMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual bool openBindingInitializer(BindingInitializer* bindingInitializer);
    virtual void closeBindingInitializer(BindingInitializer* bindingInitializer);
    virtual bool openIdentifierInitializer(IdentifierInitializer* identifierInitializer);
    virtual void closeIdentifierInitializer(IdentifierInitializer* identifierInitializer);
    virtual bool openInitializer(Initializer* initializer);
    virtual void closeInitializer(Initializer* initializer);
    virtual bool openAdditionalInitializer(AdditionalInitializer* additionalInitializer);
    virtual void closeAdditionalInitializer(AdditionalInitializer* additionalInitializer);
    virtual bool openIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual void closeIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual bool openTypeAnnotation(TypeAnnotation* typeAnnotation);
    virtual void closeTypeAnnotation(TypeAnnotation* typeAnnotation);
    virtual bool openType(Type* type);
    virtual void closeType(Type* type);
    virtual bool openIndexedType(IndexedType* indexedType);
    virtual void closeIndexedType(IndexedType* indexedType);
    virtual void visitPointer(Pointer* pointer);
    virtual void visitRoot(Root* root);
    virtual void visitLocal(Local* local);
    virtual void visitReference(Reference* reference);
    virtual void visitThrown(Thrown* thrown);
    virtual bool openFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual void closeFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual void visitOverrideWord(OverrideWord* overrideWord);
    virtual void visitStaticWord(StaticWord* staticWord);
    virtual bool openFunctionSignature(FunctionSignature* functionSignature);
    virtual void closeFunctionSignature(FunctionSignature* functionSignature);
    virtual bool openParameterClause(ParameterClause* parameterClause);
    virtual void closeParameterClause(ParameterClause* parameterClause);
    virtual bool openConstParameter(ConstParameter* constParameter);
    virtual void closeConstParameter(ConstParameter* constParameter);
    virtual bool openVarParameter(VarParameter* varParameter);
    virtual void closeVarParameter(VarParameter* varParameter);
    virtual bool openFunctionResult(FunctionResult* functionResult);
    virtual void closeFunctionResult(FunctionResult* functionResult);
    virtual bool openThrowsClause(ThrowsClause* throwsClause);
    virtual void closeThrowsClause(ThrowsClause* throwsClause);
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual void closeEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual bool openEnumMember(EnumMember* enumMember);
    virtual void closeEnumMember(EnumMember* enumMember);
    virtual void visitEnumCase(EnumCase* enumCase);
    virtual bool openAdditionalCase(AdditionalCase* additionalCase);
    virtual void closeAdditionalCase(AdditionalCase* additionalCase);
    virtual bool openClassDeclaration(ClassDeclaration* classDeclaration);
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration);
    virtual bool openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause);
    virtual void closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause);
    virtual bool openInheritance(Inheritance* inheritance);
    virtual void closeInheritance(Inheritance* inheritance);
    virtual bool openClassBody(ClassBody* classBody);
    virtual void closeClassBody(ClassBody* classBody);
    virtual bool openClassMember(ClassMember* classMember);
    virtual void closeClassMember(ClassMember* classMember);
    virtual bool openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual void closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual bool openCodeBlock(CodeBlock* codeBlock);
    virtual void closeCodeBlock(CodeBlock* codeBlock);
    virtual bool openSimpleExpression(SimpleExpression* simpleExpression);
    virtual void closeSimpleExpression(SimpleExpression* simpleExpression);
    virtual bool openPrefixExpression(PrefixExpression* prefixExpression);
    virtual void closePrefixExpression(PrefixExpression* prefixExpression);
    virtual bool openPostfixExpression(PostfixExpression* postfixExpression);
    virtual void closePostfixExpression(PostfixExpression* postfixExpression);
    virtual void visitIdentifierExpression(IdentifierExpression* identifierExpression);
    virtual void visitLiteralExpression(LiteralExpression* literalExpression);
    virtual bool openIfExpression(IfExpression* ifExpression);
    virtual void closeIfExpression(IfExpression* ifExpression);
    virtual bool openElseClause(ElseClause* elseClause);
    virtual void closeElseClause(ElseClause* elseClause);
    virtual bool openSwitchExpression(SwitchExpression* switchExpression);
    virtual void closeSwitchExpression(SwitchExpression* switchExpression);
    virtual bool openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody);
    virtual void closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody);
    virtual bool openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody);
    virtual void closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody);
    virtual bool openSwitchCase(SwitchCase* switchCase);
    virtual void closeSwitchCase(SwitchCase* switchCase);
    virtual bool openItemCaseLabel(ItemCaseLabel* itemCaseLabel);
    virtual void closeItemCaseLabel(ItemCaseLabel* itemCaseLabel);
    virtual void visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel);
    virtual bool openCaseItem(CaseItem* caseItem);
    virtual void closeCaseItem(CaseItem* caseItem);
    virtual bool openCaseContent(CaseContent* caseContent);
    virtual void closeCaseContent(CaseContent* caseContent);
    virtual bool openForExpression(ForExpression* forExpression);
    virtual void closeForExpression(ForExpression* forExpression);
    virtual bool openWhileExpression(WhileExpression* whileExpression);
    virtual void closeWhileExpression(WhileExpression* whileExpression);
    virtual bool openDoExpression(DoExpression* doExpression);
    virtual void closeDoExpression(DoExpression* doExpression);
    virtual bool openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression);
    virtual void closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression);
    virtual bool openExpressionElement(ExpressionElement* expressionElement);
    virtual void closeExpressionElement(ExpressionElement* expressionElement);
    virtual bool openReturnExpression(ReturnExpression* returnExpression);
    virtual void closeReturnExpression(ReturnExpression* returnExpression);
    virtual bool openThrowExpression(ThrowExpression* throwExpression);
    virtual void closeThrowExpression(ThrowExpression* throwExpression);
    virtual bool openBreakExpression(BreakExpression* breakExpression);
    virtual void closeBreakExpression(BreakExpression* breakExpression);
    virtual bool openConstructorCall(ConstructorCall* constructorCall);
    virtual void closeConstructorCall(ConstructorCall* constructorCall);
    virtual void visitThisExpression(ThisExpression* thisExpression);
    virtual void visitNullExpression(NullExpression* nullExpression);
    virtual void visitOperatorPostfix(OperatorPostfix* operatorPostfix);
    virtual bool openFunctionCall(FunctionCall* functionCall);
    virtual void closeFunctionCall(FunctionCall* functionCall);
    virtual bool openCatchClause(CatchClause* catchClause);
    virtual void closeCatchClause(CatchClause* catchClause);
    virtual bool openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern);
    virtual void closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern);
    virtual bool openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern);
    virtual void closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern);
    virtual void visitWildcardPattern(WildcardPattern* wildcardPattern);
    virtual bool openTuplePattern(TuplePattern* tuplePattern);
    virtual void closeTuplePattern(TuplePattern* tuplePattern);
    virtual bool openTuplePatternElement(TuplePatternElement* tuplePatternElement);
    virtual void closeTuplePatternElement(TuplePatternElement* tuplePatternElement);
    virtual bool openExpressionPattern(ExpressionPattern* expressionPattern);
    virtual void closeExpressionPattern(ExpressionPattern* expressionPattern);
    virtual void visitMemberExpression(MemberExpression* memberExpression);
    virtual bool openSubscript(Subscript* subscript);
    virtual void closeSubscript(Subscript* subscript);
    virtual bool openBinaryOperation(BinaryOperation* binaryOperation);
    virtual void closeBinaryOperation(BinaryOperation* binaryOperation);
    virtual bool openAssignment(Assignment* assignment);
    virtual void closeAssignment(Assignment* assignment);
    virtual bool openTypeQuery(TypeQuery* typeQuery);
    virtual void closeTypeQuery(TypeQuery* typeQuery);
    virtual bool openTypeCast(TypeCast* typeCast);
    virtual void closeTypeCast(TypeCast* typeCast);

    virtual bool _isCppVisitor();
    virtual bool _isHeaderVisitor();
    virtual bool _isSourceVisitor();
    virtual bool _isModelVisitor();
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
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isConstructorDeclaration();
    virtual bool _isExpression();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
    virtual bool _isBindingInitializer();
    virtual bool _isIdentifierInitializer();
    virtual bool _isInitializer();
    virtual bool _isAdditionalInitializer();
    virtual bool _isPattern();
    virtual bool _isIdentifierPattern();
    virtual bool _isWildcardPattern();
    virtual bool _isTuplePattern();
    virtual bool _isExpressionPattern();
    virtual bool _isTypeAnnotation();
    virtual bool _isType();
    virtual bool _isTypePostfix();
    virtual bool _isIndexedType();
    virtual bool _isPointer();
    virtual bool _isLifeTime();
    virtual bool _isRoot();
    virtual bool _isLocal();
    virtual bool _isReference();
    virtual bool _isThrown();
    virtual bool _isModifier();
    virtual bool _isOverrideWord();
    virtual bool _isStaticWord();
    virtual bool _isFunctionSignature();
    virtual bool _isParameterClause();
    virtual bool _isParameter();
    virtual bool _isConstParameter();
    virtual bool _isVarParameter();
    virtual bool _isFunctionResult();
    virtual bool _isThrowsClause();
    virtual bool _isEnumMember();
    virtual bool _isEnumCase();
    virtual bool _isAdditionalCase();
    virtual bool _isTypeInheritanceClause();
    virtual bool _isInheritance();
    virtual bool _isClassBody();
    virtual bool _isClassMember();
    virtual bool _isPrefixExpression();
    virtual bool _isPostfixExpression();
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
    virtual bool _isCaseContent();
    virtual bool _isExpressionElement();
    virtual bool _isPostfix();
    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isMemberExpression();
    virtual bool _isSubscript();
    virtual bool _isCatchClause();
    virtual bool _isCatchPattern();
    virtual bool _isWildCardCatchPattern();
    virtual bool _isIdentifierCatchPattern();
    virtual bool _isTuplePatternElement();
    virtual bool _isBinaryExpression();
    virtual bool _isBinaryOperation();
    virtual bool _isAssignment();
    virtual bool _isTypeQuery();
    virtual bool _isTypeCast();
};

class Program : public SyntaxNode {
public:
    Program(string* name, _Array<CompilationUnit>* compilationUnits);
    virtual void accept(Visitor* visitor);
    string* name;
    _Array<CompilationUnit>* compilationUnits;

    virtual bool _isProgram();
};

class CompilationUnit : public SyntaxNode {
public:
    CompilationUnit(_Array<Statement>* statements, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<Statement>* statements;

    virtual bool _isCompilationUnit();
};

class Statement : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isStatement();
    virtual bool _isDeclaration();
    virtual bool _isConstantDeclaration();
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
    virtual void accept(Visitor* visitor);

    virtual bool _isDeclaration();
    virtual bool _isConstantDeclaration();
    virtual bool _isMutableDeclaration();
    virtual bool _isFunctionDeclaration();
    virtual bool _isEnumDeclaration();
    virtual bool _isClassDeclaration();
    virtual bool _isConstructorDeclaration();
};

class ConstantDeclaration : public Declaration {
public:
    ConstantDeclaration(BindingInitializer* initializer, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    BindingInitializer* initializer;

    virtual bool _isConstantDeclaration();
};

class MutableDeclaration : public Declaration {
public:
    MutableDeclaration(BindingInitializer* initializer, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    BindingInitializer* initializer;

    virtual bool _isMutableDeclaration();
};

class BindingInitializer : public SyntaxNode {
public:
    BindingInitializer(IdentifierInitializer* initializer, _Array<AdditionalInitializer>* additionalInitializers, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    IdentifierInitializer* initializer;
    _Array<AdditionalInitializer>* additionalInitializers;

    virtual bool _isBindingInitializer();
};

class IdentifierInitializer : public SyntaxNode {
public:
    IdentifierInitializer(IdentifierPattern* pattern, Initializer* initializer, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    IdentifierPattern* pattern;
    Initializer* initializer;

    virtual bool _isIdentifierInitializer();
};

class Initializer : public SyntaxNode {
public:
    Initializer(Expression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* expression;

    virtual bool _isInitializer();
};

class AdditionalInitializer : public SyntaxNode {
public:
    AdditionalInitializer(IdentifierInitializer* pattern, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    IdentifierInitializer* pattern;

    virtual bool _isAdditionalInitializer();
};

class Pattern : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isPattern();
    virtual bool _isIdentifierPattern();
    virtual bool _isWildcardPattern();
    virtual bool _isTuplePattern();
    virtual bool _isExpressionPattern();
};

class IdentifierPattern : public Pattern {
public:
    IdentifierPattern(string* identifier, TypeAnnotation* annotationForType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* identifier;
    TypeAnnotation* annotationForType;

    virtual bool _isIdentifierPattern();
};

class TypeAnnotation : public SyntaxNode {
public:
    TypeAnnotation(Type* annotationForType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* annotationForType;

    virtual bool _isTypeAnnotation();
};

class Type : public SyntaxNode {
public:
    Type(string* name, _Array<TypePostfix>* postfixes, LifeTime* lifeTime, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;
    _Array<TypePostfix>* postfixes;
    LifeTime* lifeTime;

    virtual bool _isType();
};

class TypePostfix : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isTypePostfix();
    virtual bool _isIndexedType();
    virtual bool _isPointer();
};

class IndexedType : public TypePostfix {
public:
    IndexedType(Type* key, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* key;

    virtual bool _isIndexedType();
};

class Pointer : public TypePostfix {
public:
    Pointer(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isPointer();
};

class LifeTime : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isLifeTime();
    virtual bool _isRoot();
    virtual bool _isLocal();
    virtual bool _isReference();
    virtual bool _isThrown();
};

class Root : public LifeTime {
public:
    Root(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isRoot();
};

class Local : public LifeTime {
public:
    Local(string* location, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* location;

    virtual bool _isLocal();
};

class Reference : public LifeTime {
public:
    Reference(Literal* age, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Literal* age;

    virtual bool _isReference();
};

class Thrown : public LifeTime {
public:
    Thrown(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isThrown();
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(_Array<Modifier>* modifiers, string* name, FunctionSignature* signature, Expression* body, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<Modifier>* modifiers;
    string* name;
    FunctionSignature* signature;
    Expression* body;

    virtual bool _isFunctionDeclaration();
};

class Modifier : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isModifier();
    virtual bool _isOverrideWord();
    virtual bool _isStaticWord();
};

class OverrideWord : public Modifier {
public:
    OverrideWord(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isOverrideWord();
};

class StaticWord : public Modifier {
public:
    StaticWord(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isStaticWord();
};

class FunctionSignature : public SyntaxNode {
public:
    FunctionSignature(ParameterClause* parameterClause, FunctionResult* result, ThrowsClause* throwsClause, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    ParameterClause* parameterClause;
    FunctionResult* result;
    ThrowsClause* throwsClause;

    virtual bool _isFunctionSignature();
};

class ParameterClause : public SyntaxNode {
public:
    ParameterClause(_Array<Parameter>* parameters, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<Parameter>* parameters;

    virtual bool _isParameterClause();
};

class Parameter : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isParameter();
    virtual bool _isConstParameter();
    virtual bool _isVarParameter();
};

class ConstParameter : public Parameter {
public:
    ConstParameter(string* name, Type* parameterType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;
    Type* parameterType;

    virtual bool _isConstParameter();
};

class VarParameter : public Parameter {
public:
    VarParameter(string* name, Type* parameterType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;
    Type* parameterType;

    virtual bool _isVarParameter();
};

class FunctionResult : public SyntaxNode {
public:
    FunctionResult(Type* resultType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* resultType;

    virtual bool _isFunctionResult();
};

class ThrowsClause : public SyntaxNode {
public:
    ThrowsClause(Type* throwsType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* throwsType;

    virtual bool _isThrowsClause();
};

class EnumDeclaration : public Declaration {
public:
    EnumDeclaration(string* name, _Array<EnumMember>* members, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;
    _Array<EnumMember>* members;

    virtual bool _isEnumDeclaration();
};

class EnumMember : public SyntaxNode {
public:
    EnumMember(EnumCase* enumCase, _Array<AdditionalCase>* additionalCases, ParameterClause* parameterClause, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    EnumCase* enumCase;
    _Array<AdditionalCase>* additionalCases;
    ParameterClause* parameterClause;

    virtual bool _isEnumMember();
};

class EnumCase : public SyntaxNode {
public:
    EnumCase(string* name, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;

    virtual bool _isEnumCase();
};

class AdditionalCase : public SyntaxNode {
public:
    AdditionalCase(EnumCase* enumCase, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    EnumCase* enumCase;

    virtual bool _isAdditionalCase();
};

class ClassDeclaration : public Declaration {
public:
    ClassDeclaration(string* name, TypeInheritanceClause* typeInheritanceClause, ClassBody* body, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;
    TypeInheritanceClause* typeInheritanceClause;
    ClassBody* body;

    virtual bool _isClassDeclaration();
};

class TypeInheritanceClause : public SyntaxNode {
public:
    TypeInheritanceClause(_Array<Inheritance>* inheritances, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<Inheritance>* inheritances;

    virtual bool _isTypeInheritanceClause();
};

class Inheritance : public SyntaxNode {
public:
    Inheritance(Type* type, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* type;

    virtual bool _isInheritance();
};

class ClassBody : public SyntaxNode {
public:
    ClassBody(_Array<ClassMember>* members, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<ClassMember>* members;

    virtual bool _isClassBody();
};

class ClassMember : public SyntaxNode {
public:
    ClassMember(Declaration* declaration, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Declaration* declaration;

    virtual bool _isClassMember();
};

class ConstructorDeclaration : public Declaration {
public:
    ConstructorDeclaration(ParameterClause* parameterClause, Expression* body, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    ParameterClause* parameterClause;
    Expression* body;

    virtual bool _isConstructorDeclaration();
};

class Expression : public Statement {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isExpression();
    virtual bool _isCodeBlock();
    virtual bool _isSimpleExpression();
};

class CodeBlock : public Expression {
public:
    CodeBlock(_Array<Statement>* statements, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<Statement>* statements;

    virtual bool _isCodeBlock();
};

class SimpleExpression : public Expression {
public:
    SimpleExpression(PrefixExpression* prefixExpression, _Array<BinaryExpression>* binaryExpressions, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    PrefixExpression* prefixExpression;
    _Array<BinaryExpression>* binaryExpressions;

    virtual bool _isSimpleExpression();
};

class PrefixExpression : public SyntaxNode {
public:
    PrefixExpression(string* prefixOperator, PostfixExpression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* prefixOperator;
    PostfixExpression* expression;

    virtual bool _isPrefixExpression();
};

class PostfixExpression : public SyntaxNode {
public:
    PostfixExpression(PrimaryExpression* primaryExpression, _Array<Postfix>* postfixes, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    PrimaryExpression* primaryExpression;
    _Array<Postfix>* postfixes;

    virtual bool _isPostfixExpression();
};

class PrimaryExpression : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

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
    virtual void accept(Visitor* visitor);
    string* name;

    virtual bool _isIdentifierExpression();
};

class LiteralExpression : public PrimaryExpression {
public:
    LiteralExpression(Literal* literal, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Literal* literal;

    virtual bool _isLiteralExpression();
};

class IfExpression : public PrimaryExpression {
public:
    IfExpression(Expression* condition, Expression* consequent, ElseClause* elseClause, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* condition;
    Expression* consequent;
    ElseClause* elseClause;

    virtual bool _isIfExpression();
};

class ElseClause : public SyntaxNode {
public:
    ElseClause(Expression* alternative, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* alternative;

    virtual bool _isElseClause();
};

class SwitchExpression : public PrimaryExpression {
public:
    SwitchExpression(Expression* expression, SwitchBody* body, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* expression;
    SwitchBody* body;

    virtual bool _isSwitchExpression();
};

class SwitchBody : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isSwitchBody();
    virtual bool _isCurliedSwitchBody();
    virtual bool _isNakedSwitchBody();
};

class CurliedSwitchBody : public SwitchBody {
public:
    CurliedSwitchBody(_Array<SwitchCase>* cases, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<SwitchCase>* cases;

    virtual bool _isCurliedSwitchBody();
};

class NakedSwitchBody : public SwitchBody {
public:
    NakedSwitchBody(_Array<SwitchCase>* cases, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<SwitchCase>* cases;

    virtual bool _isNakedSwitchBody();
};

class SwitchCase : public SyntaxNode {
public:
    SwitchCase(CaseLabel* label, CaseContent* content, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    CaseLabel* label;
    CaseContent* content;

    virtual bool _isSwitchCase();
};

class CaseLabel : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isCaseLabel();
    virtual bool _isItemCaseLabel();
    virtual bool _isDefaultCaseLabel();
};

class ItemCaseLabel : public CaseLabel {
public:
    ItemCaseLabel(Pattern* pattern, _Array<CaseItem>* additionalPatterns, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Pattern* pattern;
    _Array<CaseItem>* additionalPatterns;

    virtual bool _isItemCaseLabel();
};

class DefaultCaseLabel : public CaseLabel {
public:
    DefaultCaseLabel(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isDefaultCaseLabel();
};

class CaseItem : public SyntaxNode {
public:
    CaseItem(Pattern* pattern, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Pattern* pattern;

    virtual bool _isCaseItem();
};

class CaseContent : public SyntaxNode {
public:
    CaseContent(_Array<Statement>* statements, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<Statement>* statements;

    virtual bool _isCaseContent();
};

class ForExpression : public PrimaryExpression {
public:
    ForExpression(IdentifierPattern* pattern, Expression* expression, Expression* code, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    IdentifierPattern* pattern;
    Expression* expression;
    Expression* code;

    virtual bool _isForExpression();
};

class WhileExpression : public PrimaryExpression {
public:
    WhileExpression(Expression* condition, Expression* code, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* condition;
    Expression* code;

    virtual bool _isWhileExpression();
};

class DoExpression : public PrimaryExpression {
public:
    DoExpression(Expression* code, Expression* condition, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* code;
    Expression* condition;

    virtual bool _isDoExpression();
};

class ParenthesizedExpression : public PrimaryExpression {
public:
    ParenthesizedExpression(_Array<ExpressionElement>* expressionElements, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<ExpressionElement>* expressionElements;

    virtual bool _isParenthesizedExpression();
};

class ExpressionElement : public SyntaxNode {
public:
    ExpressionElement(Expression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* expression;

    virtual bool _isExpressionElement();
};

class ReturnExpression : public PrimaryExpression {
public:
    ReturnExpression(ParenthesizedExpression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    ParenthesizedExpression* expression;

    virtual bool _isReturnExpression();
};

class ThrowExpression : public PrimaryExpression {
public:
    ThrowExpression(IdentifierExpression* error, ParenthesizedExpression* arguments, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    IdentifierExpression* error;
    ParenthesizedExpression* arguments;

    virtual bool _isThrowExpression();
};

class BreakExpression : public PrimaryExpression {
public:
    BreakExpression(ParenthesizedExpression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    ParenthesizedExpression* expression;

    virtual bool _isBreakExpression();
};

class ConstructorCall : public PrimaryExpression {
public:
    ConstructorCall(Type* typeToConstruct, ParenthesizedExpression* arguments, _Array<CatchClause>* catchClauses, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* typeToConstruct;
    ParenthesizedExpression* arguments;
    _Array<CatchClause>* catchClauses;

    virtual bool _isConstructorCall();
};

class ThisExpression : public PrimaryExpression {
public:
    ThisExpression(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isThisExpression();
};

class NullExpression : public PrimaryExpression {
public:
    NullExpression(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isNullExpression();
};

class Postfix : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isPostfix();
    virtual bool _isOperatorPostfix();
    virtual bool _isFunctionCall();
    virtual bool _isMemberExpression();
    virtual bool _isSubscript();
};

class OperatorPostfix : public Postfix {
public:
    OperatorPostfix(string* postfixOperator, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* postfixOperator;

    virtual bool _isOperatorPostfix();
};

class FunctionCall : public Postfix {
public:
    FunctionCall(ParenthesizedExpression* arguments, _Array<CatchClause>* catchClauses, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    ParenthesizedExpression* arguments;
    _Array<CatchClause>* catchClauses;

    virtual bool _isFunctionCall();
};

class CatchClause : public SyntaxNode {
public:
    CatchClause(CatchPattern* catchPattern, TuplePattern* bindingPattern, Expression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    CatchPattern* catchPattern;
    TuplePattern* bindingPattern;
    Expression* expression;

    virtual bool _isCatchClause();
};

class CatchPattern : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isCatchPattern();
    virtual bool _isWildCardCatchPattern();
    virtual bool _isIdentifierCatchPattern();
};

class WildCardCatchPattern : public CatchPattern {
public:
    WildCardCatchPattern(WildcardPattern* pattern, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    WildcardPattern* pattern;

    virtual bool _isWildCardCatchPattern();
};

class IdentifierCatchPattern : public CatchPattern {
public:
    IdentifierCatchPattern(string* name, MemberExpression* member, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* name;
    MemberExpression* member;

    virtual bool _isIdentifierCatchPattern();
};

class WildcardPattern : public Pattern {
public:
    WildcardPattern(Position* start, Position* end);
    virtual void accept(Visitor* visitor);

    virtual bool _isWildcardPattern();
};

class TuplePattern : public Pattern {
public:
    TuplePattern(_Array<TuplePatternElement>* elements, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<TuplePatternElement>* elements;

    virtual bool _isTuplePattern();
};

class TuplePatternElement : public SyntaxNode {
public:
    TuplePatternElement(Pattern* pattern, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Pattern* pattern;

    virtual bool _isTuplePatternElement();
};

class ExpressionPattern : public Pattern {
public:
    ExpressionPattern(Expression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* expression;

    virtual bool _isExpressionPattern();
};

class MemberExpression : public Postfix {
public:
    MemberExpression(string* member, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* member;

    virtual bool _isMemberExpression();
};

class Subscript : public Postfix {
public:
    Subscript(_Array<ExpressionElement>* expressions, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    _Array<ExpressionElement>* expressions;

    virtual bool _isSubscript();
};

class BinaryExpression : public SyntaxNode {
public:
    virtual void accept(Visitor* visitor);

    virtual bool _isBinaryExpression();
    virtual bool _isBinaryOperation();
    virtual bool _isAssignment();
    virtual bool _isTypeQuery();
    virtual bool _isTypeCast();
};

class BinaryOperation : public BinaryExpression {
public:
    BinaryOperation(string* binaryOperator, PrefixExpression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    string* binaryOperator;
    PrefixExpression* expression;

    virtual bool _isBinaryOperation();
};

class Assignment : public BinaryExpression {
public:
    Assignment(Expression* expression, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Expression* expression;

    virtual bool _isAssignment();
};

class TypeQuery : public BinaryExpression {
public:
    TypeQuery(Type* objectType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* objectType;

    virtual bool _isTypeQuery();
};

class TypeCast : public BinaryExpression {
public:
    TypeCast(Type* objectType, Position* start, Position* end);
    virtual void accept(Visitor* visitor);
    Type* objectType;

    virtual bool _isTypeCast();
};

}
#endif // __scalyc__Parser__
