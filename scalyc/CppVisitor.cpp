#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CppVisitor::CppVisitor() {
    moduleName = new(getPage()->allocateExclusivePage()) string();
    sourceFile = new(getPage()->allocateExclusivePage()) VarString();
    headerFile = new(getPage()->allocateExclusivePage()) VarString();
    mainHeaderFile = new(getPage()->allocateExclusivePage()) VarString();
    projectFile = new(getPage()->allocateExclusivePage()) VarString();
    modules = new(getPage()->allocateExclusivePage()) _Array<CppModule>();
    output = new(getPage()->allocateExclusivePage()) CppProgram();
    module = new(getPage()->allocateExclusivePage()) CppModule();
    inherits = new(getPage()->allocateExclusivePage()) _Array<Inherits>();
    classes = new(getPage()->allocateExclusivePage()) _Array<string>();
}

void CppVisitor::execute(Program* program) {
    program->accept(this);
}

bool CppVisitor::openProgram(Program* program) {
    _Region _region; _Page* _p = _region.get();
    string* programDirectory = new(_p) string(program->directory);
    if (programDirectory == nullptr || programDirectory->equals("")) {
        if (programDirectory != nullptr)
            programDirectory->getPage()->clear();
        programDirectory = new(getPage()) string(".");
    }
    if (!Directory::exists(programDirectory)) {
        auto _Directory_error = Directory::create(_p, programDirectory);
        if (_Directory_error) {
            switch (_Directory_error->getErrorCode()) {
                default:
                    return (false);
            }
        }
    }
    {
        _Region _region; _Page* _p = _region.get();
        VarString* outputFilePath = new(_p) VarString(programDirectory);
        outputFilePath->append("/");
        outputFilePath->append(program->name);
        {
            buildProjectFileString(program);
            {
                _Region _region; _Page* _p = _region.get();
                VarString* projectFilePath = new(_p) VarString(outputFilePath);
                projectFilePath->append(".project");
                auto _File_error = File::writeFromString(_p, projectFilePath, projectFile);
                if (_File_error) {
                    switch (_File_error->getErrorCode()) {
                        default:
                            return (false);
                    }
                }
            }
        }
        {
            buildMainHeaderFileString(program);
            {
                _Region _region; _Page* _p = _region.get();
                VarString* headerFilePath = new(_p) VarString(outputFilePath);
                headerFilePath->append(".h");
                auto _File_error = File::writeFromString(_p, headerFilePath, mainHeaderFile);
                if (_File_error) {
                    switch (_File_error->getErrorCode()) {
                        default:
                            return (false);
                    }
                }
            }
        }
        collectInheritances(program);
    }
    return (true);
}

void CppVisitor::closeProgram(Program* program) {
    if (output != nullptr)
        output->getPage()->clear();
    output = new(output->getPage()) CppProgram(program->name, new(output->getPage()) string(projectFile), new(output->getPage()) string(mainHeaderFile), &_Vector<CppModule>::create(output->getPage(), *(modules)));
}

bool CppVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    moduleName = compilationUnit->fileName;
    headerIndentLevel = 0;
    sourceIndentLevel = 0;
    declaringClassMember = false;
    inParameterClause = false;
    if (!(compilationUnit->parent->_isProgram()))
        return (false);
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
    if (sourceFile != nullptr)
        sourceFile->getPage()->clear();
    sourceFile = new(sourceFile->getPage()) VarString(0, 4096);
    sourceFile->append("#include \"");
    sourceFile->append(programName);
    sourceFile->append(".h\"\nusing namespace scaly;\nnamespace ");
    sourceFile->append(programName);
    sourceFile->append(" {\n\n");
    if (isTopLevelFile(compilationUnit))
        sourceFile->append("int _main(_Vector<string>* args) {\n_Region _rp; _Page* _p = _rp.get();\n\n");
    return (true);
}

void CppVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
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
            switch (_File_error->getErrorCode()) {
                default:
                    return;
            }
        }
    }
    if (isTopLevelFile(compilationUnit))
        sourceFile->append("\nreturn 0;\n");
    sourceFile->append("\n}\n");
    if (isTopLevelFile(compilationUnit))
        sourceFile->append("\n}\n");
    VarString* sourceFilePath = new(_p) VarString(outputFilePath);
    sourceFilePath->append(".cpp");
    auto _File_error = File::writeFromString(_p, sourceFilePath, sourceFile);
    if (_File_error) {
        switch (_File_error->getErrorCode()) {
            default:
                return;
        }
    }
    if (module != nullptr)
        module->getPage()->clear();
    module = new(module->getPage()) CppModule(new(module->getPage()) string(moduleName), new(module->getPage()) string(headerFile), new(module->getPage()) string(sourceFile));
    modules->push(module);
}

bool CppVisitor::isTopLevelFile(CompilationUnit* compilationUnit) {
    _Vector<TerminatedStatement>* statements = compilationUnit->statements;
    TerminatedStatement* statement = nullptr;
    size_t _statements_length = statements->length();
    for (size_t _i = 0; _i < _statements_length; _i++) {
        statement = *(*statements)[_i];
        {
            if (statement->statement->_isExpression())
                return (true);
        }
    }
    return (false);
}

bool CppVisitor::openTerminatedStatement(TerminatedStatement* terminatedStatement) {
    indentSource();
    if (terminatedStatement->statement->_isSimpleExpression()) {
        SimpleExpression* simpleExpression = (SimpleExpression*)terminatedStatement->statement;
        if (simpleExpression->prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
            _Vector<BinaryOp>* binaryOps = simpleExpression->binaryOps;
            if (binaryOps != nullptr) {
                if (binaryOps->length() == 1) {
                    BinaryOp* binaryOp = *(*binaryOps)[0];
                    if (binaryOp->_isAssignment()) {
                        _Region _region; _Page* _p = _region.get();
                        Assignment* assignment = (Assignment*)binaryOp;
                        string* memberName = getMemberIfCreatingObject(_p, assignment);
                        if ((memberName != nullptr) && (!inInitializer(assignment))) {
                            sourceFile->append("if (");
                            sourceFile->append(memberName);
                            sourceFile->append(" != nullptr)\n");
                            sourceIndentLevel++;
                            this->indentSource();
                            sourceFile->append(memberName);
                            sourceFile->append("->getPage()->clear();\n");
                            sourceIndentLevel--;
                            this->indentSource();
                        }
                    }
                }
            }
            _Vector<Postfix>* postfixes;
            postfixes = simpleExpression->prefixExpression->expression->postfixes;
            if (postfixes != nullptr) {
                Postfix* postfix = nullptr;
                size_t _postfixes_length = postfixes->length();
                for (size_t _i = 0; _i < _postfixes_length; _i++) {
                    postfix = *(*postfixes)[_i];
                    {
                        if (postfix->_isFunctionCall()) {
                            FunctionCall* functionCall = (FunctionCall*)postfix;
                            if (functionCall->catchClauses != nullptr) {
                                IdentifierExpression* identifierExpression = (IdentifierExpression*)simpleExpression->prefixExpression->expression->primaryExpression;
                                sourceFile->append("auto _");
                                sourceFile->append(identifierExpression->name);
                                sourceFile->append("_error = ");
                            }
                        }
                    }
                }
            }
        }
    }
    return (true);
}

void CppVisitor::closeTerminatedStatement(TerminatedStatement* terminatedStatement) {
    if (terminatedStatement->statement->_isClassDeclaration())
        return;
    if (terminatedStatement->statement->_isEnumDeclaration())
        return;
    {
        BindingInitializer* bindingInitializer = nullptr;
        if (terminatedStatement->statement->_isConstantDeclaration()) {
            {
                ConstantDeclaration* constantDeclaration = (ConstantDeclaration*)terminatedStatement->statement;
                if (constantDeclaration->initializer->_isBindingInitializer()) {
                    bindingInitializer = constantDeclaration->initializer;
                }
            }
            {
                VariableDeclaration* variableDeclaration = (VariableDeclaration*)terminatedStatement->statement;
                if (variableDeclaration->initializer->_isBindingInitializer()) {
                    bindingInitializer = variableDeclaration->initializer;
                }
            }
            {
                MutableDeclaration* mutableDeclaration = (MutableDeclaration*)terminatedStatement->statement;
                if (mutableDeclaration->initializer->_isBindingInitializer()) {
                    bindingInitializer = mutableDeclaration->initializer;
                }
            }
        }
        if (bindingInitializer != nullptr) {
            if (bindingInitializer->initializer->_isPatternInitializer()) {
                PatternInitializer* patternInitializer = (PatternInitializer*)bindingInitializer->initializer;
                if (isCatchingFunctionCall(patternInitializer))
                    return;
            }
        }
    }
    if (terminatedStatement->statement->_isSimpleExpression()) {
        SimpleExpression* expression = (SimpleExpression*)terminatedStatement->statement;
        PrimaryExpression* primaryExpression = expression->prefixExpression->expression->primaryExpression;
        if (primaryExpression->_isIfExpression())
            return;
        if (primaryExpression->_isSwitchExpression())
            return;
        if (primaryExpression->_isForExpression())
            return;
        if (primaryExpression->_isWhileExpression())
            return;
        if (primaryExpression->_isIdentifierExpression()) {
            PostfixExpression* postfixExpression = expression->prefixExpression->expression;
            if (postfixExpression->postfixes != nullptr) {
                _Vector<Postfix>* postfixes = postfixExpression->postfixes;
                Postfix* postfix = nullptr;
                size_t _postfixes_length = postfixes->length();
                for (size_t _i = 0; _i < _postfixes_length; _i++) {
                    postfix = *(*postfixes)[_i];
                    {
                        if (postfix->_isFunctionCall()) {
                            FunctionCall* functionCall = (FunctionCall*)postfix;
                            if (catchesError(functionCall))
                                return;
                        }
                    }
                }
            }
        }
    }
    if (terminatedStatement->statement->_isCodeBlock())
        return;
    sourceFile->append(";\n");
}

bool CppVisitor::openUseDeclaration(UseDeclaration* useDeclaration) {
    return (true);
}

void CppVisitor::closeUseDeclaration(UseDeclaration* useDeclaration) {
}

bool CppVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    constDeclaration = true;
    if (constantDeclaration->parent->parent->parent->_isClassDeclaration())
        suppressSource = true;
    return (true);
}

void CppVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    constDeclaration = false;
    suppressSource = false;
}

bool CppVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    if (variableDeclaration->parent->parent->parent->_isClassDeclaration())
        suppressSource = true;
    return (true);
}

void CppVisitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
    suppressSource = false;
}

bool CppVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    if (mutableDeclaration->parent->parent->parent->_isClassDeclaration())
        suppressSource = true;
    return (true);
}

void CppVisitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    suppressSource = false;
}

bool CppVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (functionDeclaration->body == nullptr) {
        abstractFunction = true;
        suppressSource = true;
    }
    else {
        abstractFunction = false;
        suppressSource = false;
    }
    if (functionDeclaration->modifiers != nullptr) {
        _Vector<Modifier>* modifiers = functionDeclaration->modifiers;
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
    return (true);
}

void CppVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (abstractFunction) {
        headerFile->append(" = 0");
        abstractFunction = false;
    }
    staticFunction = false;
    suppressHeader = false;
    suppressSource = false;
}

bool CppVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    headerFile->append("\n\nclass ");
    headerFile->append(enumDeclarationName);
    headerFile->append(";\n");
    sourceFile->append("long ");
    sourceFile->append(enumDeclarationName);
    sourceFile->append("::getErrorCode() {\n    return (long)errorCode;\n}\n\nvoid* ");
    sourceFile->append(enumDeclarationName);
    sourceFile->append("::getErrorInfo() {\n    return errorInfo;\n}\n\n");
    return (true);
}

void CppVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    _Vector<EnumMember>* members = enumDeclaration->members;
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
    headerFile->append("    long getErrorCode();\n    void* getErrorInfo();\n\n");
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

bool CppVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    headerFile->append("\n\nclass ");
    headerFile->append(classDeclaration->name);
    if (classDeclaration->body == nullptr) {
        headerFile->append(";");
        return (false);
    }
    headerFile->append(" : public ");
    if (classDeclaration->typeInheritanceClause != nullptr) {
        _Vector<Inheritance>* inheritances = classDeclaration->typeInheritanceClause->inheritances;
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
    return (true);
}

void CppVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
    headerFile->append("\n");
    if (classDeclaration->typeInheritanceClause != nullptr) {
        headerFile->append("\n");
        indentHeader();
        headerFile->append("virtual bool _is");
        headerFile->append(classDeclaration->name);
        headerFile->append("();");
        sourceFile->append("bool ");
        sourceFile->append(classDeclaration->name);
        sourceFile->append("::_is");
        sourceFile->append(classDeclaration->name);
        sourceFile->append("() { return (true); }\n\n");
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
                sourceFile->append("bool ");
                sourceFile->append(classDeclaration->name);
                sourceFile->append("::_is");
                sourceFile->append(derivedClass);
                sourceFile->append("() { return (false); }\n");
            }
        }
        if (derivedClasses->length() > 0)
            sourceFile->append("\n");
    }
    headerIndentLevel--;
    headerFile->append("\n};");
}

bool CppVisitor::openConstructorDeclaration(ConstructorDeclaration* initializerDeclaration) {
    if (!initializerDeclaration->parent->parent->parent->_isClassDeclaration())
        return (false);
    string* classDeclarationName = ((Program*)(initializerDeclaration->parent->parent->parent))->name;
    headerFile->append(classDeclarationName);
    headerFile->append("(");
    sourceFile->append(classDeclarationName);
    sourceFile->append("::");
    sourceFile->append(classDeclarationName);
    sourceFile->append("(");
    return (true);
}

void CppVisitor::closeConstructorDeclaration(ConstructorDeclaration* initializerDeclaration) {
    sourceFile->append("\n");
    suppressHeader = false;
}

bool CppVisitor::openCodeBlock(CodeBlock* codeBlock) {
    sourceFile->append("{\n");
    sourceIndentLevel++;
    if (localAllocations(codeBlock)) {
        indentSource();
        sourceFile->append("_Region _region; _Page* _p = _region.get();\n");
    }
    suppressHeader = true;
    return (true);
}

bool CppVisitor::localAllocations(CodeBlock* codeBlock) {
    _Vector<TerminatedStatement>* terminatedStatements = codeBlock->statements;
    if (terminatedStatements != nullptr) {
        TerminatedStatement* terminatedStatement = nullptr;
        size_t _terminatedStatements_length = terminatedStatements->length();
        for (size_t _i = 0; _i < _terminatedStatements_length; _i++) {
            terminatedStatement = *(*terminatedStatements)[_i];
            {
                if (terminatedStatement->statement->_isMutableDeclaration())
                    return (true);
            }
        }
    }
    return (false);
}

FunctionCall* CppVisitor::getFunctionCall(PatternInitializer* patternInitializer) {
    if (patternInitializer->initializer != nullptr) {
        Expression* expression = patternInitializer->initializer->expression;
        if (expression->_isSimpleExpression()) {
            SimpleExpression* simpleExpression = (SimpleExpression*)expression;
            PrefixExpression* prefixExpression = simpleExpression->prefixExpression;
            PostfixExpression* postfixExpression = prefixExpression->expression;
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                if (postfixExpression->postfixes != nullptr) {
                    _Vector<Postfix>* postfixes = postfixExpression->postfixes;
                    Postfix* postfix = nullptr;
                    size_t _postfixes_length = postfixes->length();
                    for (size_t _i = 0; _i < _postfixes_length; _i++) {
                        postfix = *(*postfixes)[_i];
                        {
                            if (postfix->_isFunctionCall()) {
                                return ((FunctionCall*)postfix);
                            }
                        }
                    }
                }
            }
        }
    }
    return (nullptr);
}

bool CppVisitor::isCatchingFunctionCall(PatternInitializer* patternInitializer) {
    if (catchesError(getFunctionCall(patternInitializer)))
        return (true);
    return (false);
}

void CppVisitor::closeCodeBlock(CodeBlock* codeBlock) {
    if (codeBlock->parent->_isFunctionDeclaration()) {
        FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)codeBlock->parent;
        if ((functionDeclaration->signature->throwsClause != nullptr) && (functionDeclaration->signature->result == nullptr)) {
            indentSource();
            sourceFile->append("return nullptr;\n");
        }
    }
    sourceIndentLevel--;
    indentSource();
    sourceFile->append("}\n");
    if (codeBlock->parent->_isFunctionDeclaration())
        sourceFile->append("\n");
}

bool CppVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    if (simpleExpression->binaryOps != nullptr) {
        _Vector<BinaryOp>* binaryOps = simpleExpression->binaryOps;
        BinaryOp* binaryOp = nullptr;
        size_t _binaryOps_length = binaryOps->length();
        for (size_t _i = 0; _i < _binaryOps_length; _i++) {
            binaryOp = *(*binaryOps)[_i];
            {
                if (binaryOp->_isTypeCast()) {
                    TypeCast* typeCast = (TypeCast*)binaryOp;
                    if (typeCast->objectType->_isType()) {
                        Type* type = (Type*)typeCast->objectType;
                        sourceFile->append("(");
                        sourceFile->append(type->name);
                        sourceFile->append("*)");
                        simpleExpression->prefixExpression->accept(this);
                        return (false);
                    }
                }
            }
        }
    }
    return (true);
}

void CppVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
}

bool CppVisitor::openPathIdentifier(PathIdentifier* pathIdentifier) {
    return (true);
}

void CppVisitor::closePathIdentifier(PathIdentifier* pathIdentifier) {
}

void CppVisitor::visitPathItem(PathItem* pathItem) {
}

bool CppVisitor::openInitializer(Initializer* initializer) {
    sourceFile->append(" = ");
    return (true);
}

void CppVisitor::closeInitializer(Initializer* initializer) {
}

bool CppVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    firstBindingInitializer = true;
    return (true);
}

void CppVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool CppVisitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    if (!firstBindingInitializer)
        headerFile->append(", ");
    else
        firstBindingInitializer = false;
    return (true);
}

void CppVisitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool CppVisitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return (true);
}

void CppVisitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

void CppVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void CppVisitor::visitStaticWord(StaticWord* staticWord) {
}

void CppVisitor::visitIdentifierFunction(IdentifierFunction* identifierFunction) {
}

bool CppVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    if (!functionSignature->parent->_isFunctionDeclaration())
        return (false);
    FunctionName* functionName = ((FunctionDeclaration*)functionSignature->parent)->name;
    if (!functionName->_isIdentifierFunction())
        return (false);
    string* identifierFunctionName = ((IdentifierFunction*)functionName)->name;
    if (staticFunction)
        headerFile->append("static ");
    else
        headerFile->append("virtual ");
    inFunctionReturn = true;
    if (functionSignature->result == nullptr) {
        if (functionSignature->throwsClause == nullptr) {
            headerFile->append("void");
            if (!suppressSource)
                sourceFile->append("void");
        }
        else {
            appendCppType(headerFile, functionSignature->throwsClause->throwsType);
            if (!suppressSource)
                appendCppType(sourceFile, functionSignature->throwsClause->throwsType);
        }
    }
    else {
        if (functionSignature->throwsClause != nullptr) {
            headerFile->append("_Result<");
            if (!suppressSource)
                sourceFile->append("_Result<");
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                headerFile->append("_Vector<");
                Type* type = functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                headerFile->append(">");
                if (!suppressSource) {
                    sourceFile->append("_Vector<");
                    appendCppTypeName(sourceFile, type);
                    sourceFile->append(">");
                }
            }
            else {
                Type* type = (Type*)functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                if (!suppressSource)
                    appendCppTypeName(sourceFile, type);
            }
            headerFile->append(", ");
            appendCppTypeName(headerFile, (Type*)(functionSignature->throwsClause->throwsType));
            headerFile->append(">");
            if (!suppressSource) {
                sourceFile->append(", ");
                appendCppTypeName(sourceFile, (Type*)(functionSignature->throwsClause->throwsType));
                sourceFile->append(">");
            }
        }
        else {
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                headerFile->append("_Vector<");
                Type* type = functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                headerFile->append(">");
                if (!suppressSource) {
                    sourceFile->append("_Vector<");
                    appendCppTypeName(sourceFile, type);
                    sourceFile->append(">");
                }
            }
            else {
                Type* type = (Type*)functionSignature->result->resultType;
                appendCppTypeName(headerFile, type);
                if (!suppressSource)
                    appendCppTypeName(sourceFile, type);
                if (isClass(type->name)) {
                    headerFile->append("*");
                    if (!suppressSource)
                        sourceFile->append("*");
                }
            }
        }
    }
    inFunctionReturn = false;
    headerFile->append(" ");
    if (!suppressSource) {
        sourceFile->append(" ");
        if (functionSignature->parent->parent->parent->parent->_isClassDeclaration()) {
            ClassDeclaration* classDeclaration = (ClassDeclaration*)functionSignature->parent->parent->parent->parent;
            sourceFile->append(classDeclaration->name);
            sourceFile->append("::");
        }
    }
    headerFile->append(identifierFunctionName);
    headerFile->append("(");
    if (!suppressSource) {
        sourceFile->append(identifierFunctionName);
        sourceFile->append("(");
    }
    if (functionSignature->result != nullptr) {
        if (functionSignature->result->resultType->_isType()) {
            Type* type = (Type*)functionSignature->result->resultType;
            if (isClass(type->name)) {
                if (functionSignature->result->existingObject == nullptr) {
                    headerFile->append("_Page* _rp");
                    if (!suppressSource)
                        sourceFile->append("_Page* _rp");
                    if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                        headerFile->append(", ");
                        if (!suppressSource)
                            sourceFile->append(", ");
                    }
                }
            }
        }
        else {
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                if (functionSignature->result->existingObject == nullptr) {
                    headerFile->append("_Page* _rp");
                    if (!suppressSource)
                        sourceFile->append("_Page* _rp");
                    if ((functionSignature->parameterClause->parameters != nullptr) || (functionSignature->throwsClause != nullptr)) {
                        headerFile->append(", ");
                        if (!suppressSource)
                            sourceFile->append(", ");
                    }
                }
            }
        }
    }
    if (functionSignature->throwsClause != nullptr) {
        headerFile->append("_Page* _ep");
        if (!suppressSource)
            sourceFile->append("_Page* _ep");
        if (functionSignature->parameterClause->parameters) {
            headerFile->append(", ");
            if (!suppressSource)
                sourceFile->append(", ");
        }
    }
    return (true);
}

bool CppVisitor::hasArrayPostfix(Type* type) {
    if (type->postfixes == nullptr)
        return (false);
    _Vector<TypePostfix>* postfixes = type->postfixes;
    TypePostfix* typePostfix = *(*postfixes)[0];
    if (typePostfix->_isIndexedType())
        return (true);
    return (false);
}

void CppVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
    suppressHeader = true;
}

bool CppVisitor::openFunctionResult(FunctionResult* functionResult) {
    return (false);
}

void CppVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

void CppVisitor::visitExistingClause(ExistingClause* existingClause) {
}

bool CppVisitor::openParameterClause(ParameterClause* parameterClause) {
    firstParameter = true;
    inParameterClause = true;
    return (true);
}

void CppVisitor::closeParameterClause(ParameterClause* parameterClause) {
    headerFile->append(")");
    if (!suppressSource)
        sourceFile->append(") ");
    inParameterClause = false;
}

bool CppVisitor::openConstParameter(ConstParameter* constParameter) {
    string* constParameterName = constParameter->name;
    constDeclaration = true;
    writeParameter(constParameterName, constParameter->parameterType);
    constDeclaration = false;
    return (false);
}

void CppVisitor::writeParameter(string* name, Type* parameterType) {
    if (!firstParameter) {
        headerFile->append(", ");
        if (!suppressSource)
            sourceFile->append(", ");
    }
    else {
        firstParameter = false;
    }
    parameterType->accept(this);
    headerFile->append(" ");
    headerFile->append(name);
    if (!suppressSource) {
        sourceFile->append(" ");
        sourceFile->append(name);
    }
}

bool CppVisitor::isClass(string* name) {
    if ((name->equals("string") || name->equals("VarString") || name->equals("File") || name->equals("Directory") || name->equals("Path") || name->equals("DirectoryError") || name->equals("FileError") || name->equals("ParserError") || name->equals("CppError") || name->equals("CompilerError"))) {
        return (true);
    }
    string* className = nullptr;
    size_t _classes_length = classes->length();
    for (size_t _i = 0; _i < _classes_length; _i++) {
        className = *(*classes)[_i];
        {
            if (className->equals(name)) {
                return (true);
            }
        }
    }
    return (false);
}

void CppVisitor::closeConstParameter(ConstParameter* constParameter) {
    headerFile->append(constParameter->name);
}

bool CppVisitor::openVarParameter(VarParameter* varParameter) {
    writeParameter(varParameter->name, varParameter->parameterType);
    return (false);
}

void CppVisitor::closeVarParameter(VarParameter* varParameter) {
    string* varParameterName = varParameter->name;
    headerFile->append(varParameterName);
}

bool CppVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return (false);
}

void CppVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool CppVisitor::openEnumMember(EnumMember* enumMember) {
    if (!enumMember->parent->_isEnumDeclaration())
        return (false);
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
        sourceFile->append("_");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("::_");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("(");
    }
    inEnumMember = true;
    return (true);
}

void CppVisitor::closeEnumMember(EnumMember* enumMember) {
    if (enumMember->parameterClause != nullptr) {
        sourceFile->append("\n");
        headerFile->append(";\n\n");
        _Vector<Parameter>* parameters = enumMember->parameterClause->parameters;
        if (parameters != nullptr) {
            sourceFile->append(": ");
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
                        if (pos != 0)
                            sourceFile->append(", ");
                        sourceFile->append(constParameter->name);
                        sourceFile->append("(");
                        sourceFile->append(constParameter->name);
                        sourceFile->append(")");
                    }
                    pos++;
                }
            }
        }
        headerFile->append("};\n");
        sourceFile->append(" { }\n\n_");
        string* enumDeclarationName = ((EnumDeclaration*)(enumMember->parent))->name;
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("* ");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("::get_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("() {\n    return (_");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("*)errorInfo;\n}\n\n");
    }
    inEnumMember = false;
}

void CppVisitor::appendCppType(VarString* s, Type* type) {
    if (hasArrayPostfix(type)) {
        s->append("_Vector<");
        appendCppTypeName(s, type);
        s->append(">*");
    }
    else {
        appendCppTypeName(s, type);
        if (isClass(type->name)) {
            s->append("*");
        }
    }
}

void CppVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool CppVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return (true);
}

void CppVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool CppVisitor::openClassBody(ClassBody* classBody) {
    return (true);
}

void CppVisitor::closeClassBody(ClassBody* classBody) {
}

void CppVisitor::indentHeader() {
    size_t i = 0;
    while (i < headerIndentLevel) {
        headerFile->append("    ");
        i++;
    }
}

void CppVisitor::indentSource() {
    size_t i = 0;
    while (i < sourceIndentLevel) {
        sourceFile->append("    ");
        i++;
    }
}

void CppVisitor::collectDerivedClasses(_Array<string>* derivedClasses, string* className) {
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

void CppVisitor::appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors) {
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

bool CppVisitor::openGenericArgumentClause(GenericArgumentClause* genericArgumentClause) {
    return (true);
}

void CppVisitor::closeGenericArgumentClause(GenericArgumentClause* genericArgumentClause) {
}

void CppVisitor::visitGenericParameter(GenericParameter* genericParameter) {
}

bool CppVisitor::openClassMember(ClassMember* classMember) {
    headerFile->append("\n");
    indentHeader();
    declaringClassMember = true;
    return (true);
}

void CppVisitor::closeClassMember(ClassMember* classMember) {
    headerFile->append(";");
    declaringClassMember = false;
}

bool CppVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    if (prefixExpression->prefixOperator != nullptr)
        sourceFile->append(prefixExpression->prefixOperator);
    return (true);
}

void CppVisitor::closePrefixExpression(PrefixExpression* prefixExpression) {
}

bool CppVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    if (postfixExpression->postfixes != nullptr) {
        _Vector<Postfix>* postfixes = postfixExpression->postfixes;
        if ((*(*postfixes)[0])->_isSubscript()) {
            sourceFile->append("*(*");
        }
    }
    return (true);
}

void CppVisitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool CppVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    sourceFile->append(" ");
    sourceFile->append(binaryOperation->binaryOperator);
    sourceFile->append(" ");
    return (true);
}

void CppVisitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool CppVisitor::openAssignment(Assignment* assignment) {
    sourceFile->append(" = ");
    return (true);
}

bool CppVisitor::inInitializer(SyntaxNode* node) {
    if (node->_isConstructorDeclaration())
        return (true);
    if (node->parent == nullptr)
        return (false);
    return (inInitializer(node->parent));
}

bool CppVisitor::inReturn(SyntaxNode* node) {
    if (node->_isReturnExpression())
        return (true);
    if (node->parent == nullptr)
        return (false);
    return (inReturn(node->parent));
}

bool CppVisitor::inThrow(SyntaxNode* node) {
    if (node->_isThrowExpression())
        return (true);
    if (node->parent == nullptr)
        return (false);
    return (inThrow(node->parent));
}

string* CppVisitor::getMemberIfCreatingObject(_Page* _rp, Assignment* assignment) {
    string* functionName = getFunctionName(_rp, assignment);
    if (functionName == nullptr) {
        return (nullptr);
    }
    if (assignment->expression->_isSimpleExpression()) {
        if (isClass(functionName) || isCreatingObject(functionName, assignment)) {
            if (assignment->parent->_isSimpleExpression()) {
                SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
                if (simpleExpression->prefixExpression->prefixOperator == 0) {
                    PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
                    if ((leftSide->postfixes == 0) && (leftSide->primaryExpression->_isIdentifierExpression())) {
                        _Region _region; _Page* _p = _region.get();
                        IdentifierExpression* memberExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                        string* memberName = new(_p) string(memberExpression->name);
                        ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                        if (classDeclaration != nullptr) {
                            return (new(_rp) string(memberName));
                        }
                    }
                }
            }
        }
    }
    return (nullptr);
}

string* CppVisitor::getFunctionName(_Page* _rp, Assignment* assignment) {
    if (assignment->expression->_isSimpleExpression()) {
        SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->expression);
        PrefixExpression* prefixExpression = simpleExpression->prefixExpression;
        if (prefixExpression->prefixOperator == nullptr) {
            PostfixExpression* rightSide = prefixExpression->expression;
            if (rightSide->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* classExpression = (IdentifierExpression*)(rightSide->primaryExpression);
                return (new(_rp) string(classExpression->name));
            }
            else {
                if (rightSide->primaryExpression->_isConstructorCall()) {
                    ConstructorCall* initializerCall = (ConstructorCall*)(rightSide->primaryExpression);
                    if (hasArrayPostfix(initializerCall->typeToInitialize)) {
                        Type* type = initializerCall->typeToInitialize;
                        return (new(_rp) string(type->name));
                    }
                    else {
                        Type* type = initializerCall->typeToInitialize;
                        return (new(_rp) string(type->name));
                    }
                }
            }
        }
    }
    return (nullptr);
}

bool CppVisitor::isCreatingObject(string* functionName, SyntaxNode* node) {
    ClassDeclaration* classDeclaration = getClassDeclaration(node);
    if (classDeclaration == nullptr)
        return (false);
    _Vector<ClassMember>* members = classDeclaration->body->members;
    if (members == nullptr)
        return (false);
    ClassMember* member = nullptr;
    size_t _members_length = members->length();
    for (size_t _i = 0; _i < _members_length; _i++) {
        member = *(*members)[_i];
        {
            if (member->declaration->_isFunctionDeclaration()) {
                FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)(member->declaration);
                if (functionDeclaration->name->_isIdentifierFunction()) {
                    IdentifierFunction* identifierFunction = (IdentifierFunction*)(functionDeclaration->name);
                    if (identifierFunction->name->equals(functionName))
                        return (true);
                }
            }
        }
    }
    return (false);
}

ClassDeclaration* CppVisitor::getClassDeclaration(SyntaxNode* node) {
    if (node->_isClassDeclaration())
        return ((ClassDeclaration*)node);
    if (node->parent != nullptr)
        return (getClassDeclaration(node->parent));
    return (nullptr);
}

bool CppVisitor::isVariableMember(string* memberName, ClassDeclaration* classDeclaration) {
    _Vector<ClassMember>* classMembers = classDeclaration->body->members;
    ClassMember* member = nullptr;
    size_t _classMembers_length = classMembers->length();
    for (size_t _i = 0; _i < _classMembers_length; _i++) {
        member = *(*classMembers)[_i];
        {
            BindingInitializer* bindingInitializer = nullptr;
            if (member->declaration->_isMutableDeclaration()) {
                MutableDeclaration* mutableDeclaration = (MutableDeclaration*)(member->declaration);
                bindingInitializer = mutableDeclaration->initializer;
            }
            if (member->declaration->_isVariableDeclaration()) {
                VariableDeclaration* variableDeclaration = (VariableDeclaration*)(member->declaration);
                bindingInitializer = variableDeclaration->initializer;
            }
            if (bindingInitializer == nullptr)
                continue;
            PatternInitializer* patternInitializer = bindingInitializer->initializer;
            if (patternInitializer->pattern->_isIdentifierPattern()) {
                IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
                if (identifierPattern->identifier->equals(memberName))
                    return (true);
            }
        }
    }
    return (false);
}

void CppVisitor::closeAssignment(Assignment* assignment) {
}

bool CppVisitor::openTypeQuery(TypeQuery* typeQuery) {
    sourceFile->append("->_is");
    return (true);
}

void CppVisitor::closeTypeQuery(TypeQuery* typeQuery) {
}

bool CppVisitor::openTypeCast(TypeCast* typeCast) {
    return (true);
}

void CppVisitor::closeTypeCast(TypeCast* typeCast) {
}

bool CppVisitor::openCatchClause(CatchClause* catchClause) {
    if (catchClause->parent->_isFunctionCall()) {
        FunctionCall* functionCall = (FunctionCall*)(catchClause->parent);
        {
            BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
            if (bindingInitializer != nullptr) {
                PatternInitializer* patternInitializer = bindingInitializer->initializer;
                if (patternInitializer->pattern->_isIdentifierPattern()) {
                    IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
                    _Vector<CatchClause>* catchClauses = functionCall->catchClauses;
                    if (*(*catchClauses)[0] == catchClause) {
                        sourceFile->append(";\n");
                        indentSource();
                        identifierPattern->annotationForType->accept(this);
                        sourceFile->append(" ");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append(" = nullptr;\n");
                        indentSource();
                        sourceFile->append("if (_");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append("_result.succeeded()) {\n");
                        sourceIndentLevel++;
                        indentSource();
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append(" = _");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append("_result.getResult();\n");
                        sourceIndentLevel--;
                        indentSource();
                        sourceFile->append("}\n");
                    }
                    indentSource();
                    sourceFile->append("else");
                    if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                        IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)(catchClause->catchPattern);
                        sourceFile->append(" if (_");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append("_result.getErrorCode() == _");
                        sourceFile->append(identifierCatchPattern->name);
                        sourceFile->append("Code_");
                        string* errorType = getErrorType(catchClause);
                        if (errorType != nullptr)
                            sourceFile->append(errorType);
                        sourceFile->append(")");
                    }
                    sourceFile->append(" {\n");
                    if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                        if (catchClause->bindingPattern != nullptr) {
                            TuplePattern* bindingPattern = catchClause->bindingPattern;
                            if (bindingPattern->elements != nullptr) {
                                if (bindingPattern->elements->length() > 0) {
                                    _Vector<TuplePatternElement>* elements = bindingPattern->elements;
                                    TuplePatternElement* element = *(*elements)[0];
                                    if (element->pattern->_isIdentifierPattern()) {
                                        IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                        sourceIndentLevel++;
                                        indentSource();
                                        sourceFile->append("auto ");
                                        sourceFile->append(pattern->identifier);
                                        sourceFile->append(" = _");
                                        sourceFile->append(identifierPattern->identifier);
                                        sourceFile->append("_result.getError();\n");
                                        sourceIndentLevel--;
                                    }
                                }
                            }
                        }
                    }
                    sourceIndentLevel++;
                    indentSource();
                    if (catchClause->expression->_isSimpleExpression()) {
                        SimpleExpression* simpleExpression = (SimpleExpression*)(catchClause->expression);
                        PrimaryExpression* primaryExpression = simpleExpression->prefixExpression->expression->primaryExpression;
                        if ((!primaryExpression->_isReturnExpression()) && (!primaryExpression->_isBreakExpression()) && (!primaryExpression->_isThrowExpression())) {
                            sourceFile->append(identifierPattern->identifier);
                            sourceFile->append(" = ");
                        }
                    }
                    catchClause->expression->accept(this);
                    if (catchClause->expression->_isSimpleExpression())
                        sourceFile->append(";\n");
                    sourceIndentLevel--;
                    indentSource();
                    sourceFile->append("}\n");
                }
            }
        }
        {
            IdentifierExpression* identifierExpression = getIdentifierExpression((PostfixExpression*)(functionCall->parent));
            if (identifierExpression != nullptr) {
                _Vector<CatchClause>* catchClauses = functionCall->catchClauses;
                if (*(*catchClauses)[0] == catchClause) {
                    sourceFile->append(";\n");
                    indentSource();
                    sourceFile->append("if (_");
                    sourceFile->append(identifierExpression->name);
                    sourceFile->append("_error) {\n");
                    sourceIndentLevel++;
                    indentSource();
                    sourceFile->append("switch (_");
                    sourceFile->append(identifierExpression->name);
                    sourceFile->append("_error->getErrorCode()) {\n");
                    sourceIndentLevel++;
                }
                indentSource();
                if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                    sourceFile->append("case _");
                    IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)(catchClause->catchPattern);
                    sourceFile->append(identifierCatchPattern->name);
                    sourceFile->append("Code_");
                    if (identifierCatchPattern->member != nullptr) {
                        if (identifierCatchPattern->member->memberPostfix->_isNamedMemberPostfix()) {
                            NamedMemberPostfix* namedMemberPostfix = (NamedMemberPostfix*)(identifierCatchPattern->member->memberPostfix);
                            sourceFile->append(namedMemberPostfix->identifier->name);
                            sourceFile->append(":\n");
                            sourceIndentLevel++;
                            indentSource();
                            sourceFile->append("_");
                            sourceFile->append(identifierCatchPattern->name);
                            sourceFile->append("_");
                            sourceFile->append(namedMemberPostfix->identifier->name);
                            sourceFile->append("* ");
                            if (catchClause->bindingPattern != nullptr) {
                                TuplePattern* bindingPattern = catchClause->bindingPattern;
                                if (bindingPattern->elements != nullptr) {
                                    if (bindingPattern->elements->length() > 0) {
                                        _Vector<TuplePatternElement>* elements = bindingPattern->elements;
                                        TuplePatternElement* element = *(*elements)[0];
                                        if (element->pattern->_isIdentifierPattern()) {
                                            IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                            sourceFile->append(pattern->identifier);
                                            sourceFile->append(" = _");
                                            sourceFile->append(identifierExpression->name);
                                            sourceFile->append("_error.get_");
                                            sourceFile->append(namedMemberPostfix->identifier->name);
                                            sourceFile->append("();");
                                        }
                                    }
                                }
                            }
                            sourceFile->append("\n");
                            sourceIndentLevel--;
                        }
                    }
                }
                if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                    sourceFile->append("default:\n");
                }
                sourceIndentLevel++;
                indentSource();
                catchClause->expression->accept(this);
                if (catchClause->expression->_isSimpleExpression())
                    sourceFile->append(";\n");
                sourceIndentLevel--;
                _Vector<CatchClause>* clauses = functionCall->catchClauses;
                if (*(*clauses)[functionCall->catchClauses->length() - 1] == catchClause) {
                    sourceIndentLevel--;
                    indentSource();
                    sourceFile->append("}\n");
                    sourceIndentLevel--;
                    indentSource();
                    sourceFile->append("}\n");
                }
            }
        }
    }
    return (false);
}

IdentifierExpression* CppVisitor::getIdentifierExpression(PostfixExpression* postfixExpression) {
    if (postfixExpression->parent->parent->parent->parent->_isCodeBlock()) {
        if (postfixExpression->parent->_isPrefixExpression()) {
            PrefixExpression* prefixExpression = (PrefixExpression*)(postfixExpression->parent);
            if (prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
                return ((IdentifierExpression*)(prefixExpression->expression->primaryExpression));
            }
        }
    }
    return (nullptr);
}

string* CppVisitor::getErrorType(CatchClause* catchClause) {
    if (catchClause->bindingPattern != nullptr) {
        if (catchClause->bindingPattern->elements != nullptr) {
            if (catchClause->bindingPattern->elements->length() == 1) {
                _Vector<TuplePatternElement>* elements = catchClause->bindingPattern->elements;
                TuplePatternElement* element = *(*elements)[0];
                if (element->pattern->_isIdentifierPattern()) {
                    IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                    if (pattern->annotationForType != nullptr) {
                        if (pattern->annotationForType->annotationForType->_isType()) {
                            Type* type = (Type*)(pattern->annotationForType->annotationForType);
                            return (type->name);
                        }
                    }
                }
            }
        }
    }
    return (nullptr);
}

void CppVisitor::closeCatchClause(CatchClause* catchClause) {
}

bool CppVisitor::openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
    return (true);
}

void CppVisitor::closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
}

bool CppVisitor::openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
    return (true);
}

void CppVisitor::closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
}

void CppVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
    sourceFile->append(operatorPostfix->postfixOperator);
}

bool CppVisitor::openFunctionCall(FunctionCall* functionCall) {
    return (true);
}

void CppVisitor::closeFunctionCall(FunctionCall* functionCall) {
}

bool CppVisitor::openExplicitMemberExpression(ExplicitMemberExpression* explicitMemberExpression) {
    if (explicitMemberExpression->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(explicitMemberExpression->parent);
        if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
            IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
            if (postfixExpression->postfixes->length() > 1) {
                _Vector<Postfix>* postfixes = postfixExpression->postfixes;
                if (((Postfix*)*(*postfixes)[0]) && ((*(*postfixes)[1])->_isFunctionCall())) {
                    if (isClass(identifierExpression->name)) {
                        sourceFile->append("::");
                        return (true);
                    }
                }
            }
        }
    }
    sourceFile->append("->");
    return (true);
}

void CppVisitor::closeExplicitMemberExpression(ExplicitMemberExpression* explicitMemberExpression) {
}

bool CppVisitor::openSubscript(Subscript* subscript) {
    sourceFile->append("[");
    return (true);
}

void CppVisitor::closeSubscript(Subscript* subscript) {
    sourceFile->append("]");
}

bool CppVisitor::openExpressionElement(ExpressionElement* expressionElement) {
    return (true);
}

void CppVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
    if (!isLastExpressionElement(expressionElement))
        sourceFile->append(", ");
}

bool CppVisitor::isLastExpressionElement(ExpressionElement* expressionElement) {
    if (expressionElement->parent->_isParenthesizedExpression()) {
        ParenthesizedExpression* parenthesizedExpression = (ParenthesizedExpression*)(expressionElement->parent);
        _Vector<ExpressionElement>* expressionElements = parenthesizedExpression->expressionElements;
        size_t length = expressionElements->length() - 1;
        size_t i = 0;
        ExpressionElement* element = nullptr;
        size_t _expressionElements_length = expressionElements->length();
        for (size_t _i = 0; _i < _expressionElements_length; _i++) {
            element = *(*expressionElements)[_i];
            {
                if ((element == expressionElement) && (i == length))
                    return (true);
                i++;
            }
        }
    }
    if (expressionElement->parent->_isSubscript()) {
        Subscript* subscript = (Subscript*)(expressionElement->parent);
        _Vector<ExpressionElement>* expressions = subscript->expressions;
        size_t length = expressions->length() - 1;
        size_t i = 0;
        ExpressionElement* element = nullptr;
        size_t _expressions_length = expressions->length();
        for (size_t _i = 0; _i < _expressions_length; _i++) {
            element = *(*expressions)[_i];
            {
                if ((element == expressionElement) && (i == length))
                    return (true);
                i++;
            }
        }
    }
    return (false);
}

bool CppVisitor::openNamedMemberPostfix(NamedMemberPostfix* namedMemberPostfix) {
    return (true);
}

void CppVisitor::closeNamedMemberPostfix(NamedMemberPostfix* namedMemberPostfix) {
}

void CppVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
    sourceFile->append(identifierExpression->name);
    if (identifierExpression->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(identifierExpression->parent);
        if (postfixExpression->postfixes != nullptr) {
            _Vector<Postfix>* postfixes = postfixExpression->postfixes;
            if ((*(*postfixes)[0])->_isSubscript()) {
                sourceFile->append(")");
            }
        }
    }
}

bool CppVisitor::inAssignment(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isAssignment())
        return (true);
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return (false);
    return (inAssignment(parentNode));
}

Assignment* CppVisitor::getAssignment(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isAssignment())
        return ((Assignment*)syntaxNode);
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return (nullptr);
    return (getAssignment(parentNode));
}

bool CppVisitor::inRetDeclaration(SyntaxNode* syntaxNode) {
    if (syntaxNode == nullptr)
        return (false);
    BindingInitializer* bindingInitializer = nullptr;
    do {
        if (syntaxNode->_isBindingInitializer()) {
            bindingInitializer = ((BindingInitializer*)syntaxNode);
            break;
        }
        syntaxNode = syntaxNode->parent;
    }
    while (syntaxNode != nullptr);
    if (bindingInitializer == nullptr)
        return (false);
    if (bindingInitializer->initializer != nullptr) {
        PatternInitializer* patternInitializer = bindingInitializer->initializer;
        if (patternInitializer->pattern->_isIdentifierPattern()) {
            IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
            return (identifierPattern->identifier->equals("ret"));
        }
    }
    return (false);
}

void CppVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
    Literal* literal = literalExpression->literal;
    if (literal->_isNumericLiteral()) {
        NumericLiteral* numericLiteral = (NumericLiteral*)literal;
        sourceFile->append(numericLiteral->value);
    }
    else {
        if (literal->_isStringLiteral()) {
            StringLiteral* stringLiteral = (StringLiteral*)literal;
            sourceFile->append("\"");
            sourceFile->append(stringLiteral->value);
            sourceFile->append("\"");
        }
        else {
            if (literal->_isCharacterLiteral()) {
                CharacterLiteral* characterLiteral = (CharacterLiteral*)literal;
                sourceFile->append("\'");
                if (characterLiteral->value->getLength() > 0) {
                    if ((characterLiteral->value->charAt(0) == '\"') || (characterLiteral->value->charAt(0) == '\'')) {
                        sourceFile->append("\\");
                        sourceFile->append(characterLiteral->value);
                    }
                    else {
                        switch (characterLiteral->value->charAt(0)) {
                            case '\r': {
                                sourceFile->append("\\r");
                                break;
                            }

                            case '\n': {
                                sourceFile->append("\\n");
                                break;
                            }

                            case '\t': {
                                sourceFile->append("\\t");
                                break;
                            }

                            case '\0': {
                                sourceFile->append("\\0");
                                break;
                            }

                            case '\\': {
                                sourceFile->append("\\\\");
                                break;
                            }

                            default: {
                                sourceFile->append(characterLiteral->value);
                            }
                        }
                    }
                }
                sourceFile->append("\'");
            }
        }
    }
}

bool CppVisitor::openIfExpression(IfExpression* ifExpression) {
    sourceFile->append("if (");
    ifExpression->condition->accept(this);
    sourceFile->append(")");
    if (ifExpression->consequent->_isSimpleExpression()) {
        sourceFile->append("\n");
        sourceIndentLevel++;
        indentSource();
        ifExpression->consequent->accept(this);
        sourceFile->append(";\n");
        sourceIndentLevel--;
    }
    else {
        sourceFile->append(" ");
        ifExpression->consequent->accept(this);
    }
    if (ifExpression->elseClause != nullptr) {
        if (ifExpression->elseClause->alternative->_isSimpleExpression()) {
            ifExpression->elseClause->accept(this);
            sourceFile->append(";\n");
        }
        else {
            ifExpression->elseClause->accept(this);
        }
    }
    return (false);
}

void CppVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool CppVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    sourceFile->append("switch (");
    return (true);
}

void CppVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool CppVisitor::openForExpression(ForExpression* forExpression) {
    Pattern* pattern = forExpression->pattern;
    pattern->accept(this);
    if (pattern->_isIdentifierPattern()) {
        sourceFile->append(" = nullptr;\n");
        indentSource();
        sourceFile->append("size_t _");
        Expression* expression = forExpression->expression;
        if (expression->_isSimpleExpression()) {
            SimpleExpression* simpleExpression = (SimpleExpression*)expression;
            if (simpleExpression->prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)(simpleExpression->prefixExpression->expression->primaryExpression);
                string* collectionName = identifierExpression->name;
                sourceFile->append(collectionName);
                sourceFile->append("_length = ");
                sourceFile->append(collectionName);
                sourceFile->append("->length();\n");
                indentSource();
                sourceFile->append("for (size_t _i = 0; _i < _");
                sourceFile->append(collectionName);
                sourceFile->append("_length; _i++) {\n");
                sourceIndentLevel++;
                indentSource();
                if (forExpression->pattern->_isIdentifierPattern()) {
                    IdentifierPattern* identifierPattern = (IdentifierPattern*)(forExpression->pattern);
                    sourceFile->append(identifierPattern->identifier);
                    sourceFile->append(" = *(*");
                    sourceFile->append(collectionName);
                    sourceFile->append(")[_i];\n");
                    indentSource();
                    forExpression->code->accept(this);
                    if (forExpression->code->_isSimpleExpression())
                        sourceFile->append(";\n");
                    sourceIndentLevel--;
                    indentSource();
                    sourceFile->append("}\n");
                }
            }
        }
    }
    return (false);
}

void CppVisitor::closeForExpression(ForExpression* forExpression) {
}

bool CppVisitor::openWhileExpression(WhileExpression* whileExpression) {
    sourceFile->append("while (");
    whileExpression->condition->accept(this);
    sourceFile->append(")");
    if (whileExpression->code->_isSimpleExpression()) {
        sourceFile->append("\n");
        sourceIndentLevel++;
        indentSource();
        whileExpression->code->accept(this);
        sourceFile->append(";\n");
        sourceIndentLevel--;
    }
    else {
        sourceFile->append(" ");
        whileExpression->code->accept(this);
    }
    return (false);
}

void CppVisitor::closeWhileExpression(WhileExpression* whileExpression) {
}

bool CppVisitor::openRepeatExpression(RepeatExpression* repeatExpression) {
    sourceFile->append("do");
    if (repeatExpression->code->_isSimpleExpression()) {
        sourceFile->append("\n");
        sourceIndentLevel++;
        indentSource();
        repeatExpression->code->accept(this);
        sourceFile->append(";");
        sourceIndentLevel--;
    }
    else {
        sourceFile->append(" ");
        repeatExpression->code->accept(this);
    }
    indentSource();
    sourceFile->append("while (");
    repeatExpression->condition->accept(this);
    sourceFile->append(")");
    return (false);
}

void CppVisitor::closeRepeatExpression(RepeatExpression* repeatExpression) {
}

bool CppVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    sourceFile->append("(");
    if (parenthesizedExpression->parent->_isFunctionCall()) {
        FunctionCall* functionCall = (FunctionCall*)(parenthesizedExpression->parent);
        if (functionCall->parent->_isPostfixExpression()) {
            PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                if (!isClass(identifierExpression->name)) {
                    if (postfixExpression->parent->parent->parent->_isAssignment()) {
                        _Region _region; _Page* _p = _region.get();
                        Assignment* assignment = (Assignment*)(postfixExpression->parent->parent->parent);
                        string* member = getMemberIfCreatingObject(_p, assignment);
                        if (member != nullptr) {
                            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                            if (isVariableMember(member, classDeclaration)) {
                                sourceFile->append(member);
                                sourceFile->append("->getPage()");
                                if (functionCall->arguments != nullptr && functionCall->arguments->expressionElements != nullptr)
                                    sourceFile->append(", ");
                            }
                        }
                    }
                }
            }
        }
        bool parameterInserted = false;
        if (!callsInitializer(functionCall)) {
            if (assignedToMutableObject(functionCall)) {
                sourceFile->append("_p");
                parameterInserted = true;
            }
            if (assignedToConstantObject(functionCall)) {
                _Region _region; _Page* _p = _region.get();
                string* returnType = getReturnType(_p, functionCall);
                if (returnType != nullptr)
                    sourceFile->append("_rp");
                else
                    sourceFile->append("_p");
                parameterInserted = true;
            }
        }
        if (catchesError(functionCall)) {
            if (!inThrow(functionCall)) {
                _Region _region; _Page* _p = _region.get();
                if (parameterInserted)
                    sourceFile->append(", ");
                string* thrownType = getThrownType(_p, functionCall);
                if (thrownType == nullptr)
                    sourceFile->append("_p");
                else
                    sourceFile->append("_ep");
                parameterInserted = true;
            }
        }
        if (parameterInserted && (parenthesizedExpression->expressionElements != nullptr))
            sourceFile->append(", ");
    }
    return (true);
}

bool CppVisitor::assignedToMutableObject(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return (false);
    if (bindingInitializer->parent->_isMutableDeclaration()) {
        if (boundToObject(bindingInitializer))
            return (true);
    }
    return (false);
}

bool CppVisitor::assignedToConstantObject(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return (false);
    if (bindingInitializer->parent->_isConstantDeclaration()) {
        if (boundToObject(bindingInitializer))
            return (true);
    }
    return (false);
}

bool CppVisitor::boundToObject(BindingInitializer* bindingInitializer) {
    PatternInitializer* patternInitializer = bindingInitializer->initializer;
    if (patternInitializer->pattern->_isIdentifierPattern()) {
        IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
        Type* type = identifierPattern->annotationForType->annotationForType;
        if (hasArrayPostfix(type))
            return (true);
        if (isClass(type->name) && (getFunctionCall(patternInitializer) != nullptr))
            return (true);
    }
    return (false);
}

BindingInitializer* CppVisitor::getBindingInitializer(FunctionCall* functionCall) {
    if (functionCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
        if (postfixExpression->parent->parent->parent->_isInitializer()) {
            Initializer* initializer = (Initializer*)(postfixExpression->parent->parent->parent);
            if (initializer->parent->_isPatternInitializer()) {
                PatternInitializer* patternInitializer = (PatternInitializer*)(initializer->parent);
                if (patternInitializer->parent->_isBindingInitializer()) {
                    return ((BindingInitializer*)(patternInitializer->parent));
                }
            }
        }
    }
    return (nullptr);
}

bool CppVisitor::callsInitializer(FunctionCall* functionCall) {
    if (functionCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
        if (postfixExpression->postfixes->length() == 1) {
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                if (isClass(identifierExpression->name)) {
                    return (true);
                }
            }
        }
    }
    return (false);
}

bool CppVisitor::catchesError(FunctionCall* functionCall) {
    if (functionCall == nullptr)
        return (false);
    if (functionCall->catchClauses != nullptr)
        return (true);
    return (false);
}

void CppVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    sourceFile->append(")");
}

bool CppVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    sourceFile->append("return");
    if (returnExpression->expression != nullptr) {
        _Region _region; _Page* _p = _region.get();
        sourceFile->append(" ");
        string* returnType = getReturnType(_p, returnExpression);
        string* thrownType = getThrownType(_p, returnExpression);
        if (thrownType != nullptr) {
            sourceFile->append("_Result<");
            if (returnType != nullptr)
                sourceFile->append(returnType);
            sourceFile->append(", ");
            sourceFile->append(thrownType);
            sourceFile->append(">(");
        }
        returnExpression->expression->accept(this);
        if (returnsArray(returnExpression)) {
            sourceFile->append(" ? &");
            sourceFile->append(returnType);
            sourceFile->append("::create(_rp, *");
            returnExpression->expression->accept(this);
            sourceFile->append(") : 0");
        }
        if (thrownType != nullptr)
            sourceFile->append(")");
    }
    return (false);
}

void CppVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
    _Region _region; _Page* _p = _region.get();
    string* returnType = getReturnType(_p, returnExpression);
    if (returnType != nullptr) {
        sourceFile->append(")");
    }
}

bool CppVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    _Region _region; _Page* _p = _region.get();
    string* thrownType = getThrownType(_p, throwExpression);
    if (thrownType == nullptr)
        return (false);
    sourceFile->append("return ");
    string* returnType = getReturnType(_p, throwExpression);
    if (returnType != nullptr) {
        sourceFile->append("_Result<");
        sourceFile->append(returnType);
        sourceFile->append(", ");
        sourceFile->append(thrownType);
        sourceFile->append(">(");
    }
    {
        bool buildError = true;
        if (throwExpression->arguments == nullptr) {
            if (throwExpression->error->_isIdentifierExpression()) {
                IdentifierExpression* errorExpression = (IdentifierExpression*)(throwExpression->error);
                string* errorName = errorExpression->name;
                CatchClause* catchClause = getCatchClause(throwExpression);
                if (catchClause != nullptr) {
                    if (catchClause->catchPattern != nullptr) {
                        if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                            if (catchClause->bindingPattern != nullptr) {
                                TuplePattern* bindingPattern;
                                bindingPattern = catchClause->bindingPattern;
                                if (bindingPattern->elements != nullptr) {
                                    if (bindingPattern->elements->length() > 0) {
                                        _Vector<TuplePatternElement>* elements = bindingPattern->elements;
                                        TuplePatternElement* element = *(*elements)[0];
                                        if (element->pattern->_isIdentifierPattern()) {
                                            IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                            if (pattern->identifier->equals(errorName))
                                                buildError = false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (buildError) {
            sourceFile->append("new(_ep) ");
            sourceFile->append(thrownType);
            sourceFile->append("(");
            if (throwExpression->arguments != nullptr)
                sourceFile->append("new(_ep) ");
            sourceFile->append("_");
            sourceFile->append(thrownType);
            if (throwExpression->arguments == nullptr)
                sourceFile->append("Code");
            sourceFile->append("_");
        }
        if (throwExpression->error != nullptr)
            throwExpression->error->accept(this);
        if (throwExpression->arguments != nullptr)
            throwExpression->arguments->accept(this);
        if (buildError)
            sourceFile->append(")");
    }
    if (returnType != nullptr)
        sourceFile->append(")");
    return (false);
}

bool CppVisitor::inWildcardCatchClause(ThrowExpression* throwExpression) {
    CatchClause* catchClause = getCatchClause(throwExpression);
    if ((catchClause != nullptr) && (catchClause->catchPattern->_isWildCardCatchPattern()))
        return (true);
    return (false);
}

CatchClause* CppVisitor::getCatchClause(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isCatchClause())
        return ((CatchClause*)syntaxNode);
    if (syntaxNode->parent == nullptr)
        return (nullptr);
    return (getCatchClause(syntaxNode->parent));
}

string* CppVisitor::getReturnType(_Page* _rp, SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        FunctionResult* functionResult = functionDeclaration->signature->result;
        if (functionResult != nullptr) {
            VarString* ret = new(_rp) VarString();
            if (hasArrayPostfix(functionResult->resultType)) {
                Type* type = functionResult->resultType;
                ret->append("_Vector<");
                appendCppTypeName(ret, type);
                ret->append(">");
                return (new(_rp) string(ret));
            }
            else {
                appendCppTypeName(ret, functionResult->resultType);
                return (new(_rp) string(ret));
            }
        }
    }
    return (nullptr);
}

string* CppVisitor::getThrownType(_Page* _rp, SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        ThrowsClause* throwsClause = functionDeclaration->signature->throwsClause;
        if (throwsClause != nullptr) {
            VarString* ret = new(_rp) VarString();
            if (hasArrayPostfix(throwsClause->throwsType)) {
                Type* type = throwsClause->throwsType;
                ret->append("_Vector<");
                appendCppTypeName(ret, type);
                ret->append(">");
                return (new(_rp) string(ret));
            }
            else {
                appendCppTypeName(ret, throwsClause->throwsType);
                return (new(_rp) string(ret));
            }
        }
    }
    return (nullptr);
}

bool CppVisitor::returnsArray(SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        FunctionResult* functionResult = functionDeclaration->signature->result;
        if (functionResult != nullptr) {
            if (hasArrayPostfix(functionResult->resultType))
                return (true);
        }
    }
    return (false);
}

FunctionDeclaration* CppVisitor::getFunctionDeclaration(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isFunctionDeclaration())
        return ((FunctionDeclaration*)syntaxNode);
    if (syntaxNode->parent == nullptr)
        return (nullptr);
    return (getFunctionDeclaration(syntaxNode->parent));
}

void CppVisitor::closeThrowExpression(ThrowExpression* throwExpression) {
}

bool CppVisitor::openBreakExpression(BreakExpression* breakExpression) {
    sourceFile->append("break");
    return (true);
}

void CppVisitor::closeBreakExpression(BreakExpression* breakExpression) {
}

bool CppVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    if (hasArrayPostfix(constructorCall->typeToInitialize)) {
        if (!initializerIsBoundOrAssigned(constructorCall)) {
            Type* type = constructorCall->typeToInitialize;
            sourceFile->append("&_Vector<");
            sourceFile->append(type->name);
            sourceFile->append(">::create(");
            if ((inReturn(constructorCall)) || (inRetDeclaration(constructorCall))) {
                sourceFile->append("_rp");
            }
            else {
                if (inThrow(constructorCall)) {
                    sourceFile->append("_ep");
                }
                else {
                    if (inAssignment(constructorCall)) {
                        Assignment* assignment = getAssignment(constructorCall);
                        if (assignment != nullptr) {
                            _Region _region; _Page* _p = _region.get();
                            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                            string* memberName = getMemberIfCreatingObject(_p, assignment);
                            if (memberName != nullptr) {
                                if (isVariableMember(memberName, classDeclaration)) {
                                    if (!inInitializer(assignment)) {
                                        sourceFile->append(memberName);
                                        sourceFile->append("->");
                                    }
                                    sourceFile->append("getPage()");
                                    if (inInitializer(assignment)) {
                                        sourceFile->append("->allocateExclusivePage()");
                                    }
                                }
                                else {
                                    sourceFile->append("getPage()");
                                }
                            }
                        }
                    }
                    else {
                        sourceFile->append("_p");
                    }
                }
            }
            sourceFile->append(", *");
            constructorCall->arguments->accept(this);
            sourceFile->append(")");
            return (false);
        }
        else {
            sourceFile->append("new(");
            if (inInitializer(constructorCall)) {
                sourceFile->append("getPage()");
                if (constructorCall->parent->parent->parent->parent->_isAssignment()) {
                    Assignment* assignment = (Assignment*)(constructorCall->parent->parent->parent->parent);
                    if (inInitializer(assignment)) {
                        sourceFile->append("->allocateExclusivePage()");
                    }
                }
            }
            else {
                if (constructorCall->parent->parent->parent->parent->_isAssignment()) {
                    Assignment* assignment = (Assignment*)(constructorCall->parent->parent->parent->parent);
                    SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
                    if (simpleExpression->prefixExpression->prefixOperator == nullptr) {
                        PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
                        if ((leftSide->postfixes == nullptr) && (leftSide->primaryExpression->_isIdentifierExpression())) {
                            IdentifierExpression* memberExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                            string* memberName = memberExpression->name;
                            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                            if ((classDeclaration != nullptr) && (memberName != nullptr) && (isVariableMember(memberName, classDeclaration))) {
                                sourceFile->append(memberName);
                                sourceFile->append("->getPage()");
                            }
                            else {
                                sourceFile->append("_p");
                            }
                        }
                        else {
                            sourceFile->append("_p");
                        }
                    }
                    else {
                        sourceFile->append("_p");
                    }
                }
                else {
                    sourceFile->append("_p");
                }
            }
            sourceFile->append(") ");
        }
    }
    else {
        if (!initializerIsBoundOrAssigned(constructorCall)) {
            if (constructorCall->typeToInitialize->_isType()) {
                Type* type = (Type*)(constructorCall->typeToInitialize);
                sourceFile->append("new(");
                if ((inReturn(constructorCall)) || (inRetDeclaration(constructorCall))) {
                    sourceFile->append("_rp");
                }
                else {
                    if (inThrow(constructorCall)) {
                        sourceFile->append("_ep");
                    }
                    else {
                        if (inAssignment(constructorCall)) {
                            Assignment* assignment = getAssignment(constructorCall);
                            if (assignment != nullptr) {
                                _Region _region; _Page* _p = _region.get();
                                ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                                string* memberName = getMemberIfCreatingObject(_p, assignment);
                                if (memberName != nullptr) {
                                    if (isVariableMember(memberName, classDeclaration)) {
                                        if (!inInitializer(assignment)) {
                                            sourceFile->append(memberName);
                                            sourceFile->append("->");
                                        }
                                        sourceFile->append("getPage()");
                                        if (inInitializer(assignment)) {
                                            sourceFile->append("->allocateExclusivePage()");
                                        }
                                    }
                                    else {
                                        sourceFile->append("getPage()");
                                    }
                                }
                            }
                        }
                        else {
                            sourceFile->append("_p");
                        }
                    }
                }
                sourceFile->append(") ");
                sourceFile->append(type->name);
                constructorCall->arguments->accept(this);
                return (false);
            }
        }
        else {
            if (constructorCall->parent->parent->parent->parent->_isAssignment()) {
                _Region _region; _Page* _p = _region.get();
                Assignment* assignment = (Assignment*)(constructorCall->parent->parent->parent->parent);
                sourceFile->append("new(");
                ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                string* memberName = getMemberIfCreatingObject(_p, assignment);
                if (memberName != nullptr) {
                    if (isVariableMember(memberName, classDeclaration)) {
                        if (!inInitializer(assignment)) {
                            sourceFile->append(memberName);
                            sourceFile->append("->");
                        }
                        sourceFile->append("getPage()");
                        if (inInitializer(assignment)) {
                            sourceFile->append("->allocateExclusivePage()");
                        }
                    }
                    else {
                        sourceFile->append("getPage()");
                    }
                }
                else {
                    SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
                    if (simpleExpression->prefixExpression->prefixOperator == nullptr) {
                        PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
                        if ((leftSide->postfixes == nullptr) && (leftSide->primaryExpression->_isIdentifierExpression())) {
                            IdentifierExpression* memberExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                            string* memberName = memberExpression->name;
                            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                            if ((classDeclaration != nullptr) && (memberName != nullptr)) {
                                if (isVariableMember(memberName, classDeclaration)) {
                                    sourceFile->append(memberName);
                                    sourceFile->append("->");
                                }
                                sourceFile->append("getPage()");
                            }
                            else {
                                sourceFile->append("_p");
                            }
                        }
                        else {
                            sourceFile->append("_p");
                        }
                    }
                    else {
                        sourceFile->append("_p");
                    }
                }
            }
            else {
                if (constructorCall->parent->parent->parent->parent->_isInitializer()) {
                    sourceFile->append("new(");
                    if (inReturn(constructorCall) || inRetDeclaration(constructorCall)) {
                        sourceFile->append("_rp");
                    }
                    else {
                        if (inThrow(constructorCall))
                            sourceFile->append("_ep");
                        else
                            sourceFile->append("_p");
                    }
                }
            }
            sourceFile->append(") ");
            Type* type = (Type*)(constructorCall->typeToInitialize);
            sourceFile->append(type->name);
            constructorCall->arguments->accept(this);
            return (false);
        }
    }
    return (true);
}

void CppVisitor::closeConstructorCall(ConstructorCall* constructorCall) {
}

bool CppVisitor::initializerIsBoundOrAssigned(ConstructorCall* initializerCall) {
    if (initializerCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(initializerCall->parent);
        if ((postfixExpression->parent->parent->parent->_isAssignment()) || (postfixExpression->parent->parent->parent->_isInitializer()))
            return (true);
    }
    return (false);
}

void CppVisitor::visitThisExpression(ThisExpression* thisExpression) {
    sourceFile->append("this");
}

bool CppVisitor::openSuperDot(SuperDot* superDot) {
    return (true);
}

void CppVisitor::closeSuperDot(SuperDot* superDot) {
}

bool CppVisitor::openSuperSubscript(SuperSubscript* superSubscript) {
    return (true);
}

void CppVisitor::closeSuperSubscript(SuperSubscript* superSubscript) {
}

void CppVisitor::visitNullExpression(NullExpression* nullExpression) {
    sourceFile->append("nullptr");
}

bool CppVisitor::openElseClause(ElseClause* elseClause) {
    indentSource();
    sourceFile->append("else");
    if (elseClause->alternative->_isSimpleExpression()) {
        sourceFile->append("\n");
        sourceIndentLevel++;
        indentSource();
        elseClause->alternative->accept(this);
        sourceIndentLevel--;
    }
    else {
        sourceFile->append(" ");
        elseClause->alternative->accept(this);
    }
    return (false);
}

void CppVisitor::closeElseClause(ElseClause* elseClause) {
}

bool CppVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    sourceFile->append(") {\n");
    sourceIndentLevel++;
    return (true);
}

void CppVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    sourceIndentLevel--;
    indentSource();
    sourceFile->append("}\n");
}

bool CppVisitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return (true);
}

void CppVisitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool CppVisitor::openSwitchCase(SwitchCase* switchCase) {
    indentSource();
    return (true);
}

void CppVisitor::closeSwitchCase(SwitchCase* switchCase) {
}

bool CppVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    sourceFile->append("case ");
    return (true);
}

void CppVisitor::closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
}

void CppVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
    sourceFile->append("default: ");
}

bool CppVisitor::openCaseItem(CaseItem* caseItem) {
    sourceFile->append("case ");
    return (true);
}

void CppVisitor::closeCaseItem(CaseItem* caseItem) {
    sourceFile->append(": ");
}

void CppVisitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool CppVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    if (identifierPattern->parent->_isPatternInitializer()) {
        PatternInitializer* patternInitializer = (PatternInitializer*)(identifierPattern->parent);
        if (isCatchingPatternInitializer(patternInitializer)) {
            sourceFile->append("auto _");
            sourceFile->append(identifierPattern->identifier);
            sourceFile->append("_result");
            return (false);
        }
    }
    if (identifierPattern->annotationForType != nullptr) {
        identifierPattern->annotationForType->accept(this);
        if (!suppressHeader)
            headerFile->append(" ");
        if (!suppressSource)
            sourceFile->append(" ");
    }
    if (!suppressHeader)
        headerFile->append(identifierPattern->identifier);
    if (!suppressSource)
        sourceFile->append(identifierPattern->identifier);
    return (false);
}

bool CppVisitor::isCatchingPatternInitializer(PatternInitializer* patternInitializer) {
    if (patternInitializer->initializer != nullptr) {
        if (patternInitializer->initializer->expression->_isSimpleExpression()) {
            SimpleExpression* simpleExpression = (SimpleExpression*)(patternInitializer->initializer->expression);
            PostfixExpression* postfixExpression = simpleExpression->prefixExpression->expression;
            if (postfixExpression->postfixes != nullptr) {
                FunctionCall* functionCall = nullptr;
                if (postfixExpression->postfixes->length() > 0) {
                    _Vector<Postfix>* postfixes = postfixExpression->postfixes;
                    Postfix* postfix = *(*postfixes)[0];
                    if (postfix->_isFunctionCall()) {
                        functionCall = (FunctionCall*)postfix;
                    }
                    else {
                        if (postfix->_isExplicitMemberExpression()) {
                            if (postfixExpression->postfixes->length() > 1) {
                                Postfix* postfix = *(*postfixes)[1];
                                if (postfix->_isFunctionCall()) {
                                    functionCall = (FunctionCall*)postfix;
                                }
                            }
                        }
                    }
                }
                if (functionCall != nullptr) {
                    if (functionCall->catchClauses != nullptr)
                        return (true);
                }
            }
        }
    }
    return (false);
}

void CppVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
}

bool CppVisitor::openTuplePattern(TuplePattern* tuplePattern) {
    return (true);
}

void CppVisitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool CppVisitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return (true);
}

void CppVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
    if (expressionPattern->parent->_isItemCaseLabel())
        sourceFile->append(": ");
}

bool CppVisitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return (true);
}

void CppVisitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool CppVisitor::openBlockCaseContent(BlockCaseContent* blockCaseContent) {
    sourceFile->append("{\n");
    sourceIndentLevel++;
    return (true);
}

void CppVisitor::closeBlockCaseContent(BlockCaseContent* blockCaseContent) {
    bool additionalLineFeed = true;
    if (blockCaseContent->parent->_isSwitchCase()) {
        SwitchCase* switchCase = (SwitchCase*)blockCaseContent->parent;
        if (!switchCase->label->_isDefaultCaseLabel()) {
            indentSource();
            sourceFile->append("break;\n");
        }
        else
            additionalLineFeed = false;
    }
    sourceIndentLevel--;
    indentSource();
    sourceFile->append("}\n");
    if (additionalLineFeed)
        sourceFile->append("\n");
}

void CppVisitor::visitEmptyCaseContent(EmptyCaseContent* emptyCaseContent) {
}

void CppVisitor::visitSuperConstructor(SuperConstructor* superInit) {
}

void CppVisitor::visitSuperMember(SuperMember* superMember) {
}

bool CppVisitor::openType(Type* type) {
    if (hasArrayPostfix(type)) {
        if (!sourceIndentLevel) {
            if (constDeclaration) {
                headerFile->append("_Vector<");
                if (!suppressSource)
                    sourceFile->append("_Vector<");
            }
            else {
                headerFile->append("_Array<");
                if (!suppressSource)
                    sourceFile->append("_Array<");
            }
        }
        else {
            if (constDeclaration) {
                sourceFile->append("_Vector<");
            }
            else {
                if (!inThrow(type))
                    sourceFile->append("_Array<");
            }
        }
    }
    if (!suppressHeader)
        appendCppTypeName(headerFile, type);
    if ((!suppressSource))
        appendCppTypeName(sourceFile, type);
    return (true);
}

void CppVisitor::closeType(Type* type) {
    if (hasArrayPostfix(type)) {
        if (!sourceIndentLevel) {
            headerFile->append(">*");
            if (!suppressSource) {
                sourceFile->append(">");
                if (!type->parent->_isConstructorCall())
                    sourceFile->append("*");
            }
        }
        else {
            sourceFile->append(">");
            if (!type->parent->_isConstructorCall())
                sourceFile->append("*");
        }
    }
    if (isClass(type->name) && !hasArrayPostfix(type) && !inTypeQuery(type)) {
        if (!suppressHeader)
            headerFile->append("*");
        if (!suppressSource)
            sourceFile->append("*");
    }
    if (inTypeQuery(type))
        sourceFile->append("()");
}

bool CppVisitor::inTypeQuery(Type* type) {
    if (type->parent->_isTypeQuery())
        return (true);
    return (false);
}

void CppVisitor::appendCppTypeName(VarString* s, Type* type) {
    string* typeName = type->name;
    if (typeName->equals("ulong")) {
        s->append("size_t");
        return;
    }
    else {
        if (typeName->equals("char")) {
            s->append("char");
            return;
        }
    }
    s->append(typeName);
}

bool CppVisitor::openTypeAnnotation(TypeAnnotation* typeAnnotation) {
    return (true);
}

void CppVisitor::closeTypeAnnotation(TypeAnnotation* typeAnnotation) {
}

bool CppVisitor::openSubtype(Subtype* subtype) {
    return (true);
}

void CppVisitor::closeSubtype(Subtype* subtype) {
}

void CppVisitor::visitOptionalType(OptionalType* optionalType) {
}

bool CppVisitor::openIndexedType(IndexedType* indexedType) {
    return (true);
}

void CppVisitor::closeIndexedType(IndexedType* indexedType) {
}

bool CppVisitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return (true);
}

void CppVisitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool CppVisitor::openInheritance(Inheritance* inheritance) {
    return (false);
}

void CppVisitor::closeInheritance(Inheritance* inheritance) {
}

void CppVisitor::buildMainHeaderFileString(Program* program) {
    mainHeaderFile->append("#ifndef __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n#define __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n\n#include \"Scaly.h\"\n");
    _Vector<CompilationUnit>* compilationUnits = program->compilationUnits;
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
    mainHeaderFile->append(" {\nint _main(_Vector<string>* arguments);\n}\n\n#endif // __scaly__scalyc__\n");
}

void CppVisitor::buildProjectFileString(Program* program) {
    projectFile->append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    projectFile->append("<CodeLite_Project Name=\"");
    projectFile->append(program->name);
    projectFile->append("\" InternalType=\"Console\">\n  <Plugins>\n    <Plugin Name=\"qmake\">\n");
    projectFile->append("      <![CDATA[00020001N0005Debug0000000000000001N0007Release000000000000]]>\n    </Plugin>\n");
    projectFile->append("    <Plugin Name=\"CMakePlugin\">\n      <![CDATA[[{\n");
    projectFile->append("  \"name\": \"Debug\",\n  \"enabled\": false,\n  \"buildDirectory\": \"build\",\n");
    projectFile->append("  \"sourceDirectory\": \"$(ProjectPath)\",\n  \"generator\": \"\",\n  \"buildType\": \"\",\n");
    projectFile->append("  \"arguments\": [],\n  \"parentProject\": \"\"\n");
    projectFile->append(" }, {\n");
    projectFile->append("  \"name\": \"Release\",\n");
    projectFile->append("  \"enabled\": false,\n");
    projectFile->append("  \"buildDirectory\": \"build\",\n");
    projectFile->append("  \"sourceDirectory\": \"$(ProjectPath)\",\n");
    projectFile->append("  \"generator\": \"\",\n");
    projectFile->append("  \"buildType\": \"\",\n");
    projectFile->append("  \"arguments\": [],\n");
    projectFile->append("  \"parentProject\": \"\"\n");
    projectFile->append(" }]]]>\n    </Plugin>\n  </Plugins>\n");
    projectFile->append("  <Description/>\n  <Dependencies/>\n");
    projectFile->append("  <VirtualDirectory Name=\"src\">\n    <File Name=\"main.cpp\"/>\n");
    {
        _Vector<CompilationUnit>* compilationUnits = program->compilationUnits;
        CompilationUnit* compilationUnit = nullptr;
        size_t _compilationUnits_length = compilationUnits->length();
        for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
            compilationUnit = *(*compilationUnits)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                projectFile->append("    <File Name=\"");
                string* fileName = Path::getFileNameWithoutExtension(_p, compilationUnit->fileName);
                projectFile->append(fileName);
                projectFile->append(".cpp\"/>\n");
            }
        }
    }
    projectFile->append("  </VirtualDirectory>\n  <VirtualDirectory Name=\"include\">\n");
    {
        _Vector<CompilationUnit>* compilationUnits = program->compilationUnits;
        CompilationUnit* compilationUnit = nullptr;
        size_t _compilationUnits_length = compilationUnits->length();
        for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
            compilationUnit = *(*compilationUnits)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                projectFile->append("    <File Name=\"");
                string* fileName = Path::getFileNameWithoutExtension(_p, compilationUnit->fileName);
                projectFile->append(fileName);
                projectFile->append(".h\"/>\n");
            }
        }
    }
    projectFile->append("  </VirtualDirectory>\n  <Settings Type=\"Executable\">\n    <GlobalSettings>\n");
    projectFile->append("      <Compiler Options=\"\" C_Options=\"\" Assembler=\"\">\n");
    projectFile->append("        <IncludePath Value=\".\"/>\n      </Compiler>\n      <Linker Options=\"\">\n");
    projectFile->append("        <LibraryPath Value=\".\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\"/>\n");
    projectFile->append("    </GlobalSettings>\n");
    projectFile->append("    <Configuration Name=\"Debug\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\"");
    projectFile->append(" Type=\"Executable\" BuildCmpWithGlobalSettings=\"append\" BuildLnkWithGlobalSettings=\"append\"");
    projectFile->append(" BuildResWithGlobalSettings=\"append\">\n");
    projectFile->append("      <Compiler Options=\"-g;-O0;-std=c++11;-Wall\" C_Options=\"-g;-O0;-Wall\" Assembler=\"\"");
    projectFile->append(" Required=\"yes\" PreCompiledHeader=\"\" PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n");
    projectFile->append("        <IncludePath Value=\".\"/>\n        <IncludePath Value=\"../scaly\"/>\n      </Compiler>\n");
    projectFile->append("      <Linker Options=\"\" Required=\"yes\">\n        <LibraryPath Value=\"../Debug\"/>\n");
    projectFile->append("        <Library Value=\"libscaly\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\" Required=\"no\"/>\n");
    projectFile->append("      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Debug\" ");
    projectFile->append("Command=\"./$(ProjectName)\" CommandArguments=\"-o ");
    projectFile->append(program->name);
    projectFile->append(" -d output");
    {
        _Vector<CompilationUnit>* compilationUnits = program->compilationUnits;
        CompilationUnit* compilationUnit = nullptr;
        size_t _compilationUnits_length = compilationUnits->length();
        for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
            compilationUnit = *(*compilationUnits)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                projectFile->append(" ../");
                projectFile->append(program->name);
                projectFile->append("/");
                string* fileName = Path::getFileNameWithoutExtension(_p, compilationUnit->fileName);
                projectFile->append(fileName);
                projectFile->append(".scaly");
            }
        }
    }
    projectFile->append("\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\" WorkingDirectory=\"$(IntermediateDirectory)\"");
    projectFile->append(" PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n");
    projectFile->append("      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n");
    projectFile->append("        <![CDATA[]]>\n      </Environment>\n");
    projectFile->append("      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n");
    projectFile->append("        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n      </Debugger>\n");
    projectFile->append("      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n        <RebuildCommand/>\n");
    projectFile->append("        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n        <SingleFileCommand/>\n");
    projectFile->append("        <MakefileGenerationCommand/>\n        <ThirdPartyToolName>None</ThirdPartyToolName>\n");
    projectFile->append("        <WorkingDirectory/>\n      </CustomBuild>\n      <AdditionalRules>\n");
    projectFile->append("        <CustomPostBuild/>\n        <CustomPreBuild/>\n      </AdditionalRules>\n");
    projectFile->append("      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n");
    projectFile->append("        <ClangPP/>\n        <SearchPaths/>\n      </Completion>\n");
    projectFile->append("    </Configuration>\n");
    projectFile->append("    <Configuration Name=\"Release\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\" Type=\"Executable\"");
    projectFile->append(" BuildCmpWithGlobalSettings=\"append\" BuildLnkWithGlobalSettings=\"append\" BuildResWithGlobalSettings=\"append\">\n");
    projectFile->append("      <Compiler Options=\"-O2;-Wall\" C_Options=\"-O2;-Wall\" Assembler=\"\" Required=\"yes\" PreCompiledHeader=\"\"");
    projectFile->append(" PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n");
    projectFile->append("        <IncludePath Value=\".\"/>\n        <Preprocessor Value=\"NDEBUG\"/>\n");
    projectFile->append("      </Compiler>\n      <Linker Options=\"\" Required=\"yes\">\n");
    projectFile->append("        <LibraryPath Value=\"../Release\"/>\n        <Library Value=\"libscaly\"/>\n      </Linker>\n");
    projectFile->append("      <ResourceCompiler Options=\"\" Required=\"no\"/>\n");
    projectFile->append("      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Release\"");
    projectFile->append(" Command=\"./$(ProjectName)\" CommandArguments=\"\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\"");
    projectFile->append(" WorkingDirectory=\"$(IntermediateDirectory)\" PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n");
    projectFile->append("      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n");
    projectFile->append("        <![CDATA[]]>\n      </Environment>\n");
    projectFile->append("      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n");
    projectFile->append("        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n");
    projectFile->append("      </Debugger>\n      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n");
    projectFile->append("        <RebuildCommand/>\n        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n");
    projectFile->append("        <SingleFileCommand/>\n        <MakefileGenerationCommand/>\n");
    projectFile->append("        <ThirdPartyToolName>None</ThirdPartyToolName>\n        <WorkingDirectory/>\n");
    projectFile->append("      </CustomBuild>\n      <AdditionalRules>\n        <CustomPostBuild/>\n");
    projectFile->append("        <CustomPreBuild/>\n      </AdditionalRules>\n      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n");
    projectFile->append("        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n        <ClangPP/>\n");
    projectFile->append("        <SearchPaths/>\n      </Completion>\n    </Configuration>\n  </Settings>\n</CodeLite_Project>\n");
}

void CppVisitor::collectInheritances(Program* program) {
    _Vector<CompilationUnit>* compilationUnits = program->compilationUnits;
    CompilationUnit* compilationUnit = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        compilationUnit = *(*compilationUnits)[_i];
        collectInheritancesInCompilationUnit(compilationUnit);
    }
}

void CppVisitor::collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit) {
    if (compilationUnit->statements != nullptr) {
        _Vector<TerminatedStatement>* statements = compilationUnit->statements;
        TerminatedStatement* terminatedStatement = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            terminatedStatement = *(*statements)[_i];
            {
                if (terminatedStatement->statement != nullptr) {
                    if (terminatedStatement->statement->_isClassDeclaration()) {
                        ClassDeclaration* classDeclaration = (ClassDeclaration*)terminatedStatement->statement;
                        classes->push(classDeclaration->name);
                        if (classDeclaration->typeInheritanceClause != nullptr) {
                            TypeInheritanceClause* inheritanceClause = classDeclaration->typeInheritanceClause;
                            _Vector<Inheritance>* inheritances = inheritanceClause->inheritances;
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
}

void CppVisitor::registerInheritance(string* className, string* baseName) {
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
        if (inherit != nullptr)
            inherit->getPage()->clear();
        inherit = new(getPage()) Inherits(baseName);
        inherits->push(inherit);
    }
    inherit->inheritors->push(className);
}

bool CppVisitor::_isCppVisitor() { return (true); }


}
