#include "scalyc.h"
using namespace scaly;
namespace scalyc {

HeaderVisitor::HeaderVisitor() {
    moduleName = new(getPage()) string();
    headerFile = new(getPage()->allocateExclusivePage()) VarString();
    mainHeaderFile = new(getPage()->allocateExclusivePage()) VarString();
    inherits = new(getPage()->allocateExclusivePage()) _Array<Inherits>();
    classes = new(getPage()->allocateExclusivePage()) _Array<string>();
}

bool HeaderVisitor::openProgram(Program* program) {
    _Region _region; _Page* _p = _region.get();
    string* programDirectory = new(_p) string(program->directory);
    if (programDirectory == nullptr || programDirectory->equals("")) {
        programDirectory = new(getPage()) string(".");
    }
    if (!Directory::exists(programDirectory)) {
        auto _Directory_error = Directory::create(_p, programDirectory);
        if (_Directory_error) {
            switch (_Directory_error->_getErrorCode()) {
                default:
                {
                    return false;
                }
            }
        }
    }
    {
        _Region _region; _Page* _p = _region.get();
        VarString* outputFilePath = new(_p) VarString(programDirectory);
        outputFilePath->append("/");
        outputFilePath->append(program->name);
        {
            buildMainHeaderFileString(program);
            {
                _Region _region; _Page* _p = _region.get();
                VarString* headerFilePath = new(_p) VarString(outputFilePath);
                headerFilePath->append(".h");
                auto _File_error = File::writeFromString(_p, headerFilePath, mainHeaderFile);
                if (_File_error) {
                    switch (_File_error->_getErrorCode()) {
                        default:
                        {
                            return false;
                        }
                    }
                }
            }
        }
        collectInheritances(program);
    }
    return true;
}

void HeaderVisitor::closeProgram(Program* program) {
}

bool HeaderVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    moduleName = compilationUnit->fileName;
    headerIndentLevel = 0;
    if (!(compilationUnit->parent->_isProgram()))
        return false;
    string* programName = ((Program*)(compilationUnit->parent))->name;
    if (!moduleName->equals(programName)) {
        if (headerFile != nullptr)
            headerFile->getPage()->clear();
        headerFile = new(headerFile->getPage()) VarString();
        headerFile->append("#ifndef __");
        headerFile->append(programName);
        headerFile->append("__");
        headerFile->append(moduleName);
        headerFile->append("__\n");
        headerFile->append("#define __");
        headerFile->append(programName);
        headerFile->append("__");
        headerFile->append(moduleName);
        headerFile->append("__\n#include \"");
        headerFile->append(programName);
        headerFile->append(".h\"\nusing namespace scaly;\nnamespace ");
        headerFile->append(programName);
        headerFile->append(" {");
    }
    return true;
}

void HeaderVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
    _Region _region; _Page* _p = _region.get();
    if (!(compilationUnit->parent)->_isProgram())
        return;
    string* programName = ((Program*)(compilationUnit->parent))->name;
    string* programDirectory = ((Program*)(compilationUnit->parent))->directory;
    VarString* outputFilePath = new(_p) VarString(programDirectory);
    outputFilePath->append('/');
    string* fileNameWithoutExtension = Path::getFileNameWithoutExtension(_p, compilationUnit->fileName);
    outputFilePath->append(fileNameWithoutExtension);
    if (!moduleName->equals(programName)) {
        headerFile->append("\n\n}\n#endif // __scalyc__");
        headerFile->append(moduleName);
        headerFile->append("__\n");
        VarString* headerFilePath = new(_p) VarString(outputFilePath);
        headerFilePath->append(".h");
        auto _File_error = File::writeFromString(_p, headerFilePath, headerFile);
        if (_File_error) {
            switch (_File_error->_getErrorCode()) {
                default:
                {
                    return;
                }
            }
        }
    }
}

bool HeaderVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    if (constantDeclaration->parent->parent->parent == nullptr)
        return false;
    if (constantDeclaration->parent->parent->parent->_isClassDeclaration())
        return true;
    return false;
}

void HeaderVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool HeaderVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    if (variableDeclaration->parent->parent->parent == nullptr)
        return false;
    if (variableDeclaration->parent->parent->parent->_isClassDeclaration())
        return true;
    return false;
}

void HeaderVisitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
}

bool HeaderVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    if (mutableDeclaration->parent->parent->parent == nullptr)
        return false;
    if (mutableDeclaration->parent->parent->parent->_isClassDeclaration())
        return true;
    return false;
}

void HeaderVisitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
}

bool HeaderVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (functionDeclaration->modifiers != nullptr) {
        _Array<Modifier>* modifiers = functionDeclaration->modifiers;
        Modifier* modifier = nullptr;
        size_t _modifiers_length = modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++) {
            modifier = *(*modifiers)[_i];
            {
                if (modifier->_isStaticWord())
                    staticFunction = true;
            }
        }
    }
    return true;
}

void HeaderVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    staticFunction = false;
    if (functionDeclaration->body == nullptr)
        headerFile->append(" = 0");
}

bool HeaderVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    headerFile->append("\n\nclass ");
    headerFile->append(enumDeclarationName);
    headerFile->append(";\n");
    return true;
}

void HeaderVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    _Array<EnumMember>* members = enumDeclaration->members;
    if (members != nullptr) {
        headerFile->append("enum _");
        headerFile->append(enumDeclarationName);
        headerFile->append("Code {\n");
        int i = 0;
        EnumMember* member = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            member = *(*members)[_i];
            {
                headerFile->append("    _");
                headerFile->append(enumDeclarationName);
                headerFile->append("Code_");
                headerFile->append(member->enumCase->name);
                if (i == 0)
                    headerFile->append(" = 1");
                headerFile->append(",\n");
                i++;
            }
        }
        headerFile->append("};\n\n");
    }
    headerFile->append("class ");
    headerFile->append(enumDeclarationName);
    headerFile->append(" : public Object {\npublic:\n    ");
    headerFile->append(enumDeclarationName);
    headerFile->append("(_");
    headerFile->append(enumDeclarationName);
    headerFile->append("Code errorCode)\n    : errorCode(errorCode), errorInfo(0) {}\n\n");
    if (members != nullptr) {
        EnumMember* member = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            member = *(*members)[_i];
            {
                if (member->parameterClause) {
                    headerFile->append("    ");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("(_");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("* ");
                    headerFile->append(member->enumCase->name);
                    headerFile->append(")\n    : errorCode(_");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("Code_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("), errorInfo(");
                    headerFile->append(member->enumCase->name);
                    headerFile->append(") {}\n\n");
                }
            }
        }
    }
    headerFile->append("    long _getErrorCode();\n    void* _getErrorInfo();\n\n");
    if (members != nullptr) {
        EnumMember* member = nullptr;
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            member = *(*members)[_i];
            {
                if (member->parameterClause) {
                    headerFile->append("    _");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("* get_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("();\n");
                }
            }
        }
    }
    headerFile->append("\nprivate:\n    _");
    headerFile->append(enumDeclarationName);
    headerFile->append("Code errorCode;\n    void* errorInfo;\n};");
}

bool HeaderVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    headerFile->append("\n\nclass ");
    headerFile->append(classDeclaration->name);
    if (classDeclaration->body == nullptr) {
        headerFile->append(";");
        return false;
    }
    headerFile->append(" : public ");
    if (classDeclaration->typeInheritanceClause != nullptr) {
        _Array<Inheritance>* inheritances = classDeclaration->typeInheritanceClause->inheritances;
        int i = 0;
        Inheritance* inheritance = nullptr;
        size_t _inheritances_length = inheritances->length();
        for (size_t _i = 0; _i < _inheritances_length; _i++) {
            inheritance = *(*inheritances)[_i];
            {
                if (i > 0)
                    headerFile->append(", ");
                headerFile->append(inheritance->type->name);
                i++;
            }
        }
    }
    else {
        headerFile->append("Object");
    }
    headerFile->append(" {\n");
    headerFile->append("public:");
    headerIndentLevel++;
    return true;
}

void HeaderVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
    headerFile->append("\n");
    if (classDeclaration->typeInheritanceClause != nullptr) {
        headerFile->append("\n");
        indentHeader();
        headerFile->append("virtual bool _is");
        headerFile->append(classDeclaration->name);
        headerFile->append("();");
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Array<string>* derivedClasses = new(_p) _Array<string>();
        collectDerivedClasses(derivedClasses, new(_p) string(classDeclaration->name));
        string* derivedClass = nullptr;
        size_t _derivedClasses_length = derivedClasses->length();
        for (size_t _i = 0; _i < _derivedClasses_length; _i++) {
            derivedClass = *(*derivedClasses)[_i];
            {
                headerFile->append("\n");
                indentHeader();
                headerFile->append("virtual bool _is");
                headerFile->append(derivedClass);
                headerFile->append("();");
            }
        }
    }
    headerIndentLevel--;
    headerFile->append("\n};");
}

bool HeaderVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    if (!constructorDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    string* classDeclarationName = ((Program*)(constructorDeclaration->parent->parent->parent))->name;
    headerFile->append(classDeclarationName);
    headerFile->append("(");
    return true;
}

void HeaderVisitor::closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
}

bool HeaderVisitor::openCodeBlock(CodeBlock* codeBlock) {
    return false;
}

void HeaderVisitor::closeCodeBlock(CodeBlock* codeBlock) {
}

bool HeaderVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    return true;
}

void HeaderVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
}

bool HeaderVisitor::openInitializer(Initializer* initializer) {
    return true;
}

void HeaderVisitor::closeInitializer(Initializer* initializer) {
}

bool HeaderVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    firstBindingInitializer = true;
    return true;
}

void HeaderVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool HeaderVisitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    if (!firstBindingInitializer)
        headerFile->append(", ");
    else
        firstBindingInitializer = false;
    return true;
}

void HeaderVisitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool HeaderVisitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return true;
}

void HeaderVisitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

void HeaderVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void HeaderVisitor::visitStaticWord(StaticWord* staticWord) {
}

bool HeaderVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    string* functionName = ((FunctionDeclaration*)functionSignature->parent)->name;
    if (staticFunction)
        headerFile->append("static ");
    else
        headerFile->append("virtual ");
    if (functionSignature->result == nullptr) {
        if (functionSignature->throwsClause == nullptr) {
            headerFile->append("void");
        }
        else {
            appendCppType(headerFile, functionSignature->throwsClause->throwsType);
        }
    }
    else {
        if (functionSignature->throwsClause != nullptr) {
            headerFile->append("_Result<");
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                headerFile->append("_Array<");
                Type* type = functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                headerFile->append(">");
            }
            else {
                Type* type = (Type*)functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
            }
            headerFile->append(", ");
            appendCppTypeName(headerFile, (Type*)(functionSignature->throwsClause->throwsType));
            headerFile->append(">");
        }
        else {
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                headerFile->append("_Array<");
                Type* type = functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                headerFile->append(">");
            }
            else {
                Type* type = (Type*)functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                if (isClass(type->name)) {
                    headerFile->append("*");
                }
            }
        }
    }
    headerFile->append(" ");
    headerFile->append(functionName);
    headerFile->append("(");
    if (functionSignature->result != nullptr) {
        Type* type = (Type*)functionSignature->result->resultType;
        if (isClass(type->name)) {
            LifeTime* lifeTime = type->lifeTime;
            if ((lifeTime == nullptr) || !(lifeTime->_isReference())) {
                headerFile->append("_Page* _rp");
                if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                    headerFile->append(", ");
                }
            }
        }
    }
    if (functionSignature->throwsClause != nullptr) {
        headerFile->append("_Page* _ep");
        if (functionSignature->parameterClause->parameters) {
            headerFile->append(", ");
        }
    }
    return true;
}

void HeaderVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
}

bool HeaderVisitor::openFunctionResult(FunctionResult* functionResult) {
    return true;
}

void HeaderVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

bool HeaderVisitor::openParameterClause(ParameterClause* parameterClause) {
    firstParameter = true;
    return true;
}

void HeaderVisitor::closeParameterClause(ParameterClause* parameterClause) {
    headerFile->append(")");
}

bool HeaderVisitor::openConstParameter(ConstParameter* constParameter) {
    string* constParameterName = constParameter->name;
    writeParameter(constParameterName, constParameter->parameterType);
    return false;
}

void HeaderVisitor::writeParameter(string* name, Type* parameterType) {
    if (!firstParameter) {
        headerFile->append(", ");
    }
    else {
        firstParameter = false;
    }
    parameterType->accept(this);
    headerFile->append(" ");
    headerFile->append(name);
}

bool HeaderVisitor::isClass(string* name) {
    if ((name->equals("string") || name->equals("VarString") || name->equals("File") || name->equals("Directory") || name->equals("Path") || name->equals("DirectoryError") || name->equals("FileError") || name->equals("ParserError") || name->equals("CppError") || name->equals("CompilerError"))) {
        return true;
    }
    string* className = nullptr;
    size_t _classes_length = classes->length();
    for (size_t _i = 0; _i < _classes_length; _i++) {
        className = *(*classes)[_i];
        {
            if (className->equals(name)) {
                return true;
            }
        }
    }
    return false;
}

void HeaderVisitor::closeConstParameter(ConstParameter* constParameter) {
    headerFile->append(constParameter->name);
}

bool HeaderVisitor::openVarParameter(VarParameter* varParameter) {
    writeParameter(varParameter->name, varParameter->parameterType);
    return false;
}

void HeaderVisitor::closeVarParameter(VarParameter* varParameter) {
    string* varParameterName = varParameter->name;
    headerFile->append(varParameterName);
}

bool HeaderVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return true;
}

void HeaderVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool HeaderVisitor::openEnumMember(EnumMember* enumMember) {
    if (!enumMember->parent->_isEnumDeclaration())
        return false;
    string* enumDeclarationName = ((EnumDeclaration*)(enumMember->parent))->name;
    if (enumMember->parameterClause) {
        headerFile->append("\nclass _");
        headerFile->append(enumDeclarationName);
        headerFile->append("_");
        headerFile->append(enumMember->enumCase->name);
        headerFile->append(" : public Object {\npublic:\n    _");
        headerFile->append(enumDeclarationName);
        headerFile->append("_");
        headerFile->append(enumMember->enumCase->name);
        headerFile->append("(");
    }
    return true;
}

void HeaderVisitor::closeEnumMember(EnumMember* enumMember) {
    if (enumMember->parameterClause != nullptr) {
        headerFile->append(";\n\n");
        _Array<Parameter>* parameters = enumMember->parameterClause->parameters;
        if (parameters != nullptr) {
            size_t pos = 0;
            Parameter* parameter = nullptr;
            size_t _parameters_length = parameters->length();
            for (size_t _i = 0; _i < _parameters_length; _i++) {
                parameter = *(*parameters)[_i];
                {
                    if (parameter->_isConstParameter()) {
                        ConstParameter* constParameter = (ConstParameter*)parameter;
                        headerFile->append("    ");
                        appendCppType(headerFile, constParameter->parameterType);
                        headerFile->append(" ");
                        headerFile->append(constParameter->name);
                        headerFile->append(";\n");
                    }
                    pos++;
                }
            }
        }
        headerFile->append("};\n");
    }
}

void HeaderVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool HeaderVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void HeaderVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool HeaderVisitor::openClassBody(ClassBody* classBody) {
    return true;
}

void HeaderVisitor::closeClassBody(ClassBody* classBody) {
}

void HeaderVisitor::indentHeader() {
    size_t i = 0;
    while (i < headerIndentLevel) {
        headerFile->append("    ");
        i++;
    }
}

void HeaderVisitor::collectDerivedClasses(_Array<string>* derivedClasses, string* className) {
    Inherits* inherit = nullptr;
    size_t _inherits_length = inherits->length();
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        inherit = *(*inherits)[_i];
        {
            if (inherit->name->equals(className))
                appendDerivedClasses(derivedClasses, inherit->inheritors);
        }
    }
}

void HeaderVisitor::appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors) {
    string* inheritor = nullptr;
    size_t _inheritors_length = inheritors->length();
    for (size_t _i = 0; _i < _inheritors_length; _i++) {
        inheritor = *(*inheritors)[_i];
        {
            derivedClasses->push(inheritor);
            collectDerivedClasses(derivedClasses, inheritor);
        }
    }
}

bool HeaderVisitor::openClassMember(ClassMember* classMember) {
    headerFile->append("\n");
    indentHeader();
    return true;
}

void HeaderVisitor::closeClassMember(ClassMember* classMember) {
    headerFile->append(";");
}

bool HeaderVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    return true;
}

void HeaderVisitor::closePrefixExpression(PrefixExpression* prefixExpression) {
}

bool HeaderVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    return true;
}

void HeaderVisitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool HeaderVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    return true;
}

void HeaderVisitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool HeaderVisitor::openAssignment(Assignment* assignment) {
    return true;
}

void HeaderVisitor::closeAssignment(Assignment* assignment) {
}

bool HeaderVisitor::openTypeQuery(TypeQuery* typeQuery) {
    return true;
}

void HeaderVisitor::closeTypeQuery(TypeQuery* typeQuery) {
}

bool HeaderVisitor::openTypeCast(TypeCast* typeCast) {
    return true;
}

void HeaderVisitor::closeTypeCast(TypeCast* typeCast) {
}

bool HeaderVisitor::openCatchClause(CatchClause* catchClause) {
    return true;
}

void HeaderVisitor::closeCatchClause(CatchClause* catchClause) {
}

bool HeaderVisitor::openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
    return true;
}

void HeaderVisitor::closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
}

bool HeaderVisitor::openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
    return true;
}

void HeaderVisitor::closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
}

void HeaderVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
}

bool HeaderVisitor::openFunctionCall(FunctionCall* functionCall) {
    return true;
}

void HeaderVisitor::closeFunctionCall(FunctionCall* functionCall) {
}

void HeaderVisitor::visitMemberExpression(MemberExpression* memberExpression) {
}

bool HeaderVisitor::openSubscript(Subscript* subscript) {
    return true;
}

void HeaderVisitor::closeSubscript(Subscript* subscript) {
}

bool HeaderVisitor::openExpressionElement(ExpressionElement* expressionElement) {
    return true;
}

void HeaderVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
}

void HeaderVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
}

void HeaderVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
}

bool HeaderVisitor::openIfExpression(IfExpression* ifExpression) {
    return true;
}

void HeaderVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool HeaderVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    return true;
}

void HeaderVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool HeaderVisitor::openForExpression(ForExpression* forExpression) {
    return true;
}

void HeaderVisitor::closeForExpression(ForExpression* forExpression) {
}

bool HeaderVisitor::openWhileExpression(WhileExpression* whileExpression) {
    return true;
}

void HeaderVisitor::closeWhileExpression(WhileExpression* whileExpression) {
}

bool HeaderVisitor::openDoExpression(DoExpression* doExpression) {
    return true;
}

void HeaderVisitor::closeDoExpression(DoExpression* doExpression) {
}

bool HeaderVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    return true;
}

void HeaderVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
}

bool HeaderVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    return true;
}

void HeaderVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
}

bool HeaderVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    return true;
}

void HeaderVisitor::closeThrowExpression(ThrowExpression* throwExpression) {
}

bool HeaderVisitor::openBreakExpression(BreakExpression* breakExpression) {
    return true;
}

void HeaderVisitor::closeBreakExpression(BreakExpression* breakExpression) {
}

bool HeaderVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    return true;
}

void HeaderVisitor::closeConstructorCall(ConstructorCall* constructorCall) {
}

void HeaderVisitor::visitThisExpression(ThisExpression* thisExpression) {
}

void HeaderVisitor::visitNullExpression(NullExpression* nullExpression) {
}

bool HeaderVisitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void HeaderVisitor::closeElseClause(ElseClause* elseClause) {
}

bool HeaderVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    return true;
}

void HeaderVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
}

bool HeaderVisitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return true;
}

void HeaderVisitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool HeaderVisitor::openSwitchCase(SwitchCase* switchCase) {
    return true;
}

void HeaderVisitor::closeSwitchCase(SwitchCase* switchCase) {
}

bool HeaderVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    return true;
}

void HeaderVisitor::closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
}

void HeaderVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool HeaderVisitor::openCaseItem(CaseItem* caseItem) {
    return true;
}

void HeaderVisitor::closeCaseItem(CaseItem* caseItem) {
}

void HeaderVisitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool HeaderVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    if (identifierPattern->annotationForType != nullptr) {
        identifierPattern->annotationForType->accept(this);
        headerFile->append(" ");
    }
    headerFile->append(identifierPattern->identifier);
    return false;
}

void HeaderVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
}

bool HeaderVisitor::openTuplePattern(TuplePattern* tuplePattern) {
    return true;
}

void HeaderVisitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool HeaderVisitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return true;
}

void HeaderVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
}

bool HeaderVisitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return true;
}

void HeaderVisitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool HeaderVisitor::openCaseContent(CaseContent* caseContent) {
    return true;
}

void HeaderVisitor::closeCaseContent(CaseContent* caseContent) {
}

bool HeaderVisitor::openType(Type* type) {
    if ((type->parent->_isFunctionResult()) || (type->parent->_isInheritance()) || (type->parent->_isThrowsClause()))
        return false;
    if (hasArrayPostfix(type)) {
        headerFile->append("_Array<");
    }
    appendCppTypeName(headerFile, type);
    return true;
}

void HeaderVisitor::closeType(Type* type) {
    if (hasArrayPostfix(type))
        headerFile->append(">*");
    if (isClass(type->name) && !hasArrayPostfix(type) && !type->parent->_isConstructorCall()) {
        headerFile->append("*");
    }
}

bool HeaderVisitor::openTypeAnnotation(TypeAnnotation* typeAnnotation) {
    return true;
}

void HeaderVisitor::closeTypeAnnotation(TypeAnnotation* typeAnnotation) {
}

bool HeaderVisitor::openSubtype(Subtype* subtype) {
    return true;
}

void HeaderVisitor::closeSubtype(Subtype* subtype) {
}

bool HeaderVisitor::openIndexedType(IndexedType* indexedType) {
    return true;
}

void HeaderVisitor::closeIndexedType(IndexedType* indexedType) {
}

void HeaderVisitor::visitPointer(Pointer* pointer) {
}

void HeaderVisitor::visitRoot(Root* root) {
}

void HeaderVisitor::visitLocal(Local* local) {
}

void HeaderVisitor::visitReference(Reference* reference) {
}

void HeaderVisitor::visitThrown(Thrown* thrown) {
}

bool HeaderVisitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return true;
}

void HeaderVisitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool HeaderVisitor::openInheritance(Inheritance* inheritance) {
    return true;
}

void HeaderVisitor::closeInheritance(Inheritance* inheritance) {
}

void HeaderVisitor::buildMainHeaderFileString(Program* program) {
    mainHeaderFile->append("#ifndef __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n#define __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n\n#include \"Scaly.h\"\n");
    _Array<CompilationUnit>* compilationUnits = program->compilationUnits;
    CompilationUnit* compilationUnit = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        compilationUnit = *(*compilationUnits)[_i];
        {
            _Region _region; _Page* _p = _region.get();
            mainHeaderFile->append("#include \"");
            string* fileName = Path::getFileNameWithoutExtension(_p, compilationUnit->fileName);
            mainHeaderFile->append(fileName);
            mainHeaderFile->append(".h\"\n");
        }
    }
    mainHeaderFile->append("\nusing namespace scaly;\nnamespace ");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append(" {\nFileError* _main(_Page* page, _Array<string>* arguments);\n}\n\n#endif // __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n");
}

void HeaderVisitor::collectInheritances(Program* program) {
    _Array<CompilationUnit>* compilationUnits = program->compilationUnits;
    CompilationUnit* compilationUnit = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        compilationUnit = *(*compilationUnits)[_i];
        collectInheritancesInCompilationUnit(compilationUnit);
    }
}

void HeaderVisitor::collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit) {
    if (compilationUnit->statements != nullptr) {
        _Array<Statement>* statements = compilationUnit->statements;
        Statement* statement = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            statement = *(*statements)[_i];
            {
                if (statement->_isClassDeclaration()) {
                    ClassDeclaration* classDeclaration = (ClassDeclaration*)statement;
                    classes->push(classDeclaration->name);
                    if (classDeclaration->typeInheritanceClause != nullptr) {
                        TypeInheritanceClause* inheritanceClause = classDeclaration->typeInheritanceClause;
                        _Array<Inheritance>* inheritances = inheritanceClause->inheritances;
                        Inheritance* inheritance = nullptr;
                        size_t _inheritances_length = inheritances->length();
                        for (size_t _i = 0; _i < _inheritances_length; _i++) {
                            inheritance = *(*inheritances)[_i];
                            {
                                registerInheritance(classDeclaration->name, inheritance->type->name);
                            }
                        }
                    }
                }
            }
        }
    }
}

void HeaderVisitor::registerInheritance(string* className, string* baseName) {
    Inherits* inherit = nullptr;
    Inherits* inh = nullptr;
    size_t _inherits_length = inherits->length();
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        inh = *(*inherits)[_i];
        {
            if (inh->name->equals(baseName))
                inherit = inh;
        }
    }
    if (inherit == nullptr) {
        inherit = new(getPage()) Inherits(baseName);
        inherits->push(inherit);
    }
    inherit->inheritors->push(className);
}

bool HeaderVisitor::_isHeaderVisitor() { return (true); }


}
