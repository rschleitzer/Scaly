#include "scalyc.h"
using namespace scaly;
namespace scalyc {

SourceVisitor::SourceVisitor() {
    moduleName = new(getPage()) string();
    sourceFile = new(getPage()->allocateExclusivePage()) VarString();
    projectFile = new(getPage()->allocateExclusivePage()) VarString();
    inherits = new(getPage()->allocateExclusivePage()) _Array<Inherits>();
    classes = new(getPage()->allocateExclusivePage()) _Array<string>();
}

bool SourceVisitor::openProgram(Program* program) {
    _Region _region; _Page* _p = _region.get();
    string* programDirectory = new(_p) string(program->directory);
    if (programDirectory == nullptr || programDirectory->equals("")) {
        programDirectory = new(getPage()) string(".");
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

bool SourceVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    moduleName = compilationUnit->fileName;
    sourceIndentLevel = 0;
    if (!(compilationUnit->parent->_isProgram()))
        return false;
    string* programName = ((Program*)(compilationUnit->parent))->name;
    if (sourceFile != nullptr)
        sourceFile->getPage()->clear();
    sourceFile = new(sourceFile->getPage()) VarString(0, 4096);
    sourceFile->append("#include \"");
    sourceFile->append(programName);
    sourceFile->append(".h\"\nusing namespace scaly;\n");
    if (isTopLevelFile(compilationUnit)) {
        sourceFile->append("namespace scaly {\n\n");
        sourceFile->append("extern __thread _Page* __CurrentPage;\n");
        sourceFile->append("extern __thread _Task* __CurrentTask;\n\n");
        sourceFile->append("}\n\n");
        sourceFile->append("int main(int argc, char** argv) {\n");
        sourceFile->append("    // Allocate the root page for the main thread\n");
        sourceFile->append("    _Page* page = 0;\n");
        sourceFile->append("    posix_memalign((void**)&page, _pageSize, _pageSize * _maxStackPages);\n");
        sourceFile->append("    if (!page)\n");
        sourceFile->append("        return -1;\n");
        sourceFile->append("    new (page) _Page();\n");
        sourceFile->append("    __CurrentPage = page;\n\n");
        sourceFile->append("    _Task* task = new(page) _Task();\n");
        sourceFile->append("    __CurrentTask = task;\n\n");
        sourceFile->append("    // Collect the arguments into a string Vector\n");
        sourceFile->append("    _Array<string>* arguments = new(__CurrentPage) _Array<string>(argc - 1);\n");
        sourceFile->append("    for (int i = 1; i < argc; i++)\n");
        sourceFile->append("        arguments->push(new(__CurrentPage) string(argv[i]));\n\n");
        sourceFile->append("    // Call Scaly's top-level code\n");
        sourceFile->append("    auto _File_error = ");
        sourceFile->append(programName);
        sourceFile->append("::_main(page, arguments);\n    int ret = 0;\n\n    // Convert Scaly's error enum back to OS errno values\n");
        sourceFile->append("    if (_File_error) {\n        switch(_File_error->_getErrorCode()) {\n");
        sourceFile->append("            case _FileErrorCode_noSuchFileOrDirectory:\n                ret = ENOENT;\n");
        sourceFile->append("                break;\n            default:\n                ret = -1;\n                break;\n        }\n    }\n\n");
        sourceFile->append("    // Only for monitoring, debugging and stuff\n");
        sourceFile->append("    __CurrentTask->dispose();\n\n");
        sourceFile->append("    // Give back the return code of the top-level code\n");
        sourceFile->append("    return ret;\n");
        sourceFile->append("}\n\n");
    }
    sourceFile->append("namespace ");
    sourceFile->append(programName);
    sourceFile->append(" {\n\n");
    if (isTopLevelFile(compilationUnit))
        sourceFile->append("FileError* _main(_Page* _ep,  _Array<string>* arguments) {\n_Region _rp; _Page* _p = _rp.get();\n\n");
    return true;
}

void SourceVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
    _Region _region; _Page* _p = _region.get();
    if (!(compilationUnit->parent)->_isProgram())
        return;
    string* programDirectory = ((Program*)(compilationUnit->parent))->directory;
    VarString* outputFilePath = new(_p) VarString(programDirectory);
    outputFilePath->append('/');
    string* fileNameWithoutExtension = Path::getFileNameWithoutExtension(_p, compilationUnit->fileName);
    outputFilePath->append(fileNameWithoutExtension);
    if (isTopLevelFile(compilationUnit)) {
        _Array<Statement>* statements = compilationUnit->statements;
        if (statements->length() > 0) {
            Statement* statement = *(*statements)[statements->length() - 1];
            if (statement->_isSimpleExpression()) {
                SimpleExpression* simpleExpression = (SimpleExpression*)statement;
                PrimaryExpression* primaryExpression = simpleExpression->prefixExpression->expression->primaryExpression;
                if ((!(primaryExpression->_isReturnExpression())) && (!(primaryExpression->_isLiteralExpression())))
                    sourceFile->append("\nreturn nullptr;\n");
            }
        }
        sourceFile->append("\n}\n");
    }
    sourceFile->append("\n}\n");
    VarString* sourceFilePath = new(_p) VarString(outputFilePath);
    sourceFilePath->append(".cpp");
    auto _File_error = File::writeFromString(_p, sourceFilePath, sourceFile);
    if (_File_error) {
        switch (_File_error->_getErrorCode()) {
            default:
            {
                return;
            }
        }
    }
}

bool SourceVisitor::isTopLevelFile(CompilationUnit* compilationUnit) {
    _Array<Statement>* statements = compilationUnit->statements;
    Statement* statement = nullptr;
    size_t _statements_length = statements->length();
    for (size_t _i = 0; _i < _statements_length; _i++) {
        statement = *(*statements)[_i];
        {
            if (statement->_isExpression())
                return true;
        }
    }
    return false;
}

bool SourceVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    if (constantDeclaration->parent->parent->parent == nullptr)
        return true;
    if (constantDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    return true;
}

bool SourceVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    if (variableDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    return true;
}

bool SourceVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    if (mutableDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    return true;
}

bool SourceVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (functionDeclaration->body == nullptr)
        return false;
    return true;
}

bool SourceVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    sourceFile->append("long ");
    sourceFile->append(enumDeclarationName);
    sourceFile->append("::_getErrorCode() {\n    return (long)errorCode;\n}\n\nvoid* ");
    sourceFile->append(enumDeclarationName);
    sourceFile->append("::_getErrorInfo() {\n    return errorInfo;\n}\n\n");
    return true;
}

void SourceVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
    if (classDeclaration->body == nullptr)
        return;
    if (classDeclaration->typeInheritanceClause != nullptr) {
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
}

bool SourceVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    if (!constructorDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    string* classDeclarationName = ((Program*)(constructorDeclaration->parent->parent->parent))->name;
    sourceFile->append(classDeclarationName);
    sourceFile->append("::");
    sourceFile->append(classDeclarationName);
    sourceFile->append("(");
    return true;
}

void SourceVisitor::closeConstructorDeclaration(ConstructorDeclaration* initializerDeclaration) {
    sourceFile->append("\n");
}

bool SourceVisitor::openCodeBlock(CodeBlock* codeBlock) {
    if ((codeBlock->parent->_isCaseContent()) || (codeBlock->parent->_isCodeBlock()))
        indentSource();
    sourceFile->append("{\n");
    sourceIndentLevel++;
    if (localAllocations(codeBlock)) {
        indentSource();
        sourceFile->append("_Region _region; _Page* _p = _region.get();\n");
    }
    return true;
}

bool SourceVisitor::localAllocations(CodeBlock* codeBlock) {
    _Array<Statement>* statements = codeBlock->statements;
    if (statements != nullptr) {
        Statement* statement = nullptr;
        size_t _statements_length = statements->length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            statement = *(*statements)[_i];
            {
                BindingInitializer* bindingInitializer = nullptr;
                if (statement->_isConstantDeclaration()) {
                    ConstantDeclaration* constantDeclaration = (ConstantDeclaration*)statement;
                    bindingInitializer = constantDeclaration->initializer;
                }
                if (statement->_isMutableDeclaration()) {
                    MutableDeclaration* mutableDeclaration = (MutableDeclaration*)statement;
                    bindingInitializer = mutableDeclaration->initializer;
                }
                if (statement->_isVariableDeclaration()) {
                    VariableDeclaration* variableDeclaration = (VariableDeclaration*)statement;
                    bindingInitializer = variableDeclaration->initializer;
                }
                if (isRootBinding(bindingInitializer))
                    return true;
            }
        }
    }
    return false;
}

FunctionCall* SourceVisitor::getFunctionCall(PatternInitializer* patternInitializer) {
    if (patternInitializer->initializer != nullptr) {
        Expression* expression = patternInitializer->initializer->expression;
        if (expression->_isSimpleExpression()) {
            SimpleExpression* simpleExpression = (SimpleExpression*)expression;
            PrefixExpression* prefixExpression = simpleExpression->prefixExpression;
            PostfixExpression* postfixExpression = prefixExpression->expression;
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                if (postfixExpression->postfixes != nullptr) {
                    _Array<Postfix>* postfixes = postfixExpression->postfixes;
                    Postfix* postfix = nullptr;
                    size_t _postfixes_length = postfixes->length();
                    for (size_t _i = 0; _i < _postfixes_length; _i++) {
                        postfix = *(*postfixes)[_i];
                        {
                            if (postfix->_isFunctionCall()) {
                                return (FunctionCall*)postfix;
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::isCatchingFunctionCall(PatternInitializer* patternInitializer) {
    if (catchesError(getFunctionCall(patternInitializer)))
        return true;
    return false;
}

void SourceVisitor::closeCodeBlock(CodeBlock* codeBlock) {
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

bool SourceVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    Statement* statement = (Statement*)simpleExpression;
    if (statement->parent->_isCodeBlock() || statement->parent->_isCaseContent() || statement->parent->_isCompilationUnit())
        indentSource();
    if (statement->parent->_isFunctionDeclaration()) {
        _Region _region; _Page* _p = _region.get();
        string* returnType = getReturnType(_p, statement);
        if (returnType != nullptr)
            prependReturn(simpleExpression);
    }
    if (statement->parent->_isCodeBlock()) {
        CodeBlock* block = (CodeBlock*)statement->parent;
        if (block->parent->_isFunctionDeclaration()) {
            _Array<Statement>* statements = block->statements;
            if (*(*statements)[statements->length() - 1] == statement) {
                _Region _region; _Page* _p = _region.get();
                string* returnType = getReturnType(_p, statement);
                if (returnType != nullptr)
                    prependReturn(simpleExpression);
            }
        }
    }
    if (simpleExpression->binaryOps != nullptr) {
        _Array<BinaryOp>* binaryOps = simpleExpression->binaryOps;
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
                        return false;
                    }
                }
            }
        }
    }
    if (simpleExpression->prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
        _Array<BinaryOp>* binaryOps = simpleExpression->binaryOps;
        if (binaryOps != nullptr) {
            if (binaryOps->length() == 1) {
                BinaryOp* binaryOp = *(*binaryOps)[0];
                if (binaryOp->_isAssignment()) {
                    Assignment* assignment = (Assignment*)binaryOp;
                    string* memberName = getMemberIfCreatingObject(assignment);
                    if ((memberName != nullptr) && (!inConstructor(assignment))) {
                        sourceFile->append("if (");
                        sourceFile->append(memberName);
                        sourceFile->append(" != nullptr)\n");
                        this->indentSource();
                        sourceFile->append("    ");
                        sourceFile->append(memberName);
                        sourceFile->append("->getPage()->clear();\n");
                        this->indentSource();
                    }
                }
            }
        }
        if (simpleExpression->parent->_isCodeBlock() || simpleExpression->parent->_isCaseContent() || simpleExpression->parent->_isCompilationUnit()) {
            _Array<Postfix>* postfixes = simpleExpression->prefixExpression->expression->postfixes;
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
    if (simpleExpression->parent->_isCompilationUnit()) {
        _Array<Postfix>* postfixes = simpleExpression->prefixExpression->expression->postfixes;
        if (postfixes != nullptr) {
            Postfix* postfix = nullptr;
            size_t _postfixes_length = postfixes->length();
            for (size_t _i = 0; _i < _postfixes_length; _i++) {
                postfix = *(*postfixes)[_i];
                {
                    if (postfix->_isFunctionCall()) {
                        FunctionCall* functionCall = (FunctionCall*)postfix;
                        if (functionCall->catchClauses == nullptr) {
                            if (functionCall->parent->_isPostfixExpression()) {
                                PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
                                if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                                    IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                                    if (identifierExpression->name->equals("print")) {
                                        sourceFile->append("{\n");
                                        sourceIndentLevel++;
                                        indentSource();
                                        sourceFile->append("auto _File_error = ");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void SourceVisitor::prependReturn(SimpleExpression* simpleExpression) {
    PrimaryExpression* expression = simpleExpression->prefixExpression->expression->primaryExpression;
    if ((!expression->_isReturnExpression()) && (!expression->_isThrowExpression()) && (!expression->_isWhileExpression()) && (!expression->_isDoExpression()))
        sourceFile->append("return ");
}

void SourceVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
    PrimaryExpression* primaryExpression = simpleExpression->prefixExpression->expression->primaryExpression;
    if (primaryExpression->_isIfExpression())
        return;
    if (primaryExpression->_isSwitchExpression())
        return;
    if (primaryExpression->_isForExpression())
        return;
    if (primaryExpression->_isWhileExpression())
        return;
    if (primaryExpression->_isIdentifierExpression()) {
        PostfixExpression* postfixExpression = simpleExpression->prefixExpression->expression;
        if (postfixExpression->postfixes != nullptr) {
            _Array<Postfix>* postfixes = postfixExpression->postfixes;
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
    if (simpleExpression->parent->_isCodeBlock() || simpleExpression->parent->_isCaseContent() || simpleExpression->parent->_isCompilationUnit())
        sourceFile->append(";\n");
    if (simpleExpression->parent->_isCompilationUnit()) {
        _Array<Postfix>* postfixes = simpleExpression->prefixExpression->expression->postfixes;
        if (postfixes != nullptr) {
            Postfix* postfix = nullptr;
            size_t _postfixes_length = postfixes->length();
            for (size_t _i = 0; _i < _postfixes_length; _i++) {
                postfix = *(*postfixes)[_i];
                {
                    if (postfix->_isFunctionCall()) {
                        FunctionCall* functionCall = (FunctionCall*)postfix;
                        if (functionCall->catchClauses == nullptr) {
                            if (functionCall->parent->_isPostfixExpression()) {
                                PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
                                if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                                    IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                                    if (identifierExpression->name->equals("print")) {
                                        indentSource();
                                        sourceFile->append("if (_File_error)\n");
                                        indentSource();
                                        sourceFile->append("    return _File_error;\n");
                                        sourceIndentLevel--;
                                        sourceFile->append("}\n");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool SourceVisitor::openInitializer(Initializer* initializer) {
    sourceFile->append(" = ");
    return true;
}

bool SourceVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    if (bindingInitializer->parent->parent->_isCodeBlock() || bindingInitializer->parent->parent->_isCaseContent() || bindingInitializer->parent->parent->_isCompilationUnit())
        indentSource();
    return true;
}

void SourceVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
    if (isCatchingFunctionCall(bindingInitializer->initializer))
        return;
    if (bindingInitializer->parent->parent->_isCodeBlock() || bindingInitializer->parent->parent->_isCaseContent() || bindingInitializer->parent->parent->_isCompilationUnit())
        sourceFile->append(";\n");
}

bool SourceVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    string* functionName = ((FunctionDeclaration*)functionSignature->parent)->name;
    if (functionSignature->result == nullptr) {
        if (functionSignature->throwsClause == nullptr)
            sourceFile->append("void");
        else
            appendCppType(sourceFile, functionSignature->throwsClause->throwsType);
    }
    else {
        if (functionSignature->throwsClause != nullptr) {
            sourceFile->append("_Result<");
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                Type* type = functionSignature->result->resultType;
                sourceFile->append("_Array<");
                appendCppTypeName(sourceFile, type);
                sourceFile->append(">");
            }
            else {
                Type* type = (Type*)functionSignature->result->resultType;
                appendCppTypeName(sourceFile, type);
            }
            sourceFile->append(", ");
            appendCppTypeName(sourceFile, (Type*)(functionSignature->throwsClause->throwsType));
            sourceFile->append(">");
        }
        else {
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                Type* type = functionSignature->result->resultType;
                sourceFile->append("_Array<");
                appendCppTypeName(sourceFile, type);
                sourceFile->append(">");
            }
            else {
                Type* type = (Type*)functionSignature->result->resultType;
                appendCppTypeName(sourceFile, type);
                if (isClass(type->name)) {
                    sourceFile->append("*");
                }
            }
        }
    }
    sourceFile->append(" ");
    if (functionSignature->parent->parent->parent->parent->_isClassDeclaration()) {
        ClassDeclaration* classDeclaration = (ClassDeclaration*)functionSignature->parent->parent->parent->parent;
        sourceFile->append(classDeclaration->name);
        sourceFile->append("::");
    }
    sourceFile->append(functionName);
    sourceFile->append("(");
    if (functionSignature->result != nullptr) {
        Type* type = (Type*)functionSignature->result->resultType;
        if (isClass(type->name)) {
            LifeTime* lifeTime = type->lifeTime;
            if ((lifeTime == nullptr) || !(lifeTime->_isReference())) {
                sourceFile->append("_Page* _rp");
                if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                    sourceFile->append(", ");
                }
            }
        }
    }
    if (functionSignature->throwsClause != nullptr) {
        sourceFile->append("_Page* _ep");
        if (functionSignature->parameterClause->parameters) {
            sourceFile->append(", ");
        }
    }
    return true;
}

bool SourceVisitor::openFunctionResult(FunctionResult* functionResult) {
    return false;
}

void SourceVisitor::closeParameterClause(ParameterClause* parameterClause) {
    sourceFile->append(") ");
}

bool SourceVisitor::openConstParameter(ConstParameter* constParameter) {
    string* constParameterName = constParameter->name;
    writeParameter(constParameterName, constParameter->parameterType);
    return false;
}

void SourceVisitor::writeParameter(string* name, Type* parameterType) {
    ParameterClause* parameterClause = (ParameterClause*)parameterType->parent->parent;
    _Array<Parameter>* parameters = parameterClause->parameters;
    Parameter* parameter = (Parameter*)parameterType->parent;
    if (parameter != *(*parameters)[0])
        sourceFile->append(", ");
    parameterType->accept(this);
    sourceFile->append(" ");
    sourceFile->append(name);
}

bool SourceVisitor::isClass(string* name) {
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

bool SourceVisitor::openVarParameter(VarParameter* varParameter) {
    writeParameter(varParameter->name, varParameter->parameterType);
    return false;
}

bool SourceVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return false;
}

bool SourceVisitor::openEnumMember(EnumMember* enumMember) {
    if (!enumMember->parent->_isEnumDeclaration())
        return false;
    string* enumDeclarationName = ((EnumDeclaration*)(enumMember->parent))->name;
    if (enumMember->parameterClause) {
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
    return true;
}

void SourceVisitor::closeEnumMember(EnumMember* enumMember) {
    if (enumMember->parameterClause != nullptr) {
        sourceFile->append("\n");
        _Array<Parameter>* parameters = enumMember->parameterClause->parameters;
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
}

void SourceVisitor::indentSource() {
    size_t i = 0;
    while (i < sourceIndentLevel) {
        sourceFile->append("    ");
        i++;
    }
}

void SourceVisitor::collectDerivedClasses(_Array<string>* derivedClasses, string* className) {
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

void SourceVisitor::appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors) {
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

bool SourceVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    if (prefixExpression->prefixOperator != nullptr)
        sourceFile->append(prefixExpression->prefixOperator);
    return true;
}

bool SourceVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    if (postfixExpression->postfixes != nullptr) {
        _Array<Postfix>* postfixes = postfixExpression->postfixes;
        if ((*(*postfixes)[0])->_isSubscript()) {
            sourceFile->append("*(*");
        }
    }
    return true;
}

bool SourceVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    sourceFile->append(" ");
    sourceFile->append(binaryOperation->binaryOperator);
    sourceFile->append(" ");
    return true;
}

bool SourceVisitor::openAssignment(Assignment* assignment) {
    sourceFile->append(" = ");
    return true;
}

bool SourceVisitor::inConstructor(SyntaxNode* node) {
    if (node->_isConstructorDeclaration())
        return true;
    if (node->parent == nullptr)
        return false;
    return inConstructor(node->parent);
}

bool SourceVisitor::inReturn(SyntaxNode* node) {
    if (node->_isReturnExpression())
        return true;
    if (node->parent == nullptr)
        return false;
    return inReturn(node->parent);
}

bool SourceVisitor::inThrow(SyntaxNode* node) {
    if (node->_isThrowExpression())
        return true;
    if (node->parent == nullptr)
        return false;
    return inThrow(node->parent);
}

string* SourceVisitor::getMemberIfCreatingObject(Assignment* assignment) {
    string* functionName = getFunctionName(assignment);
    if (functionName == nullptr) {
        return nullptr;
    }
    if (assignment->expression->_isSimpleExpression()) {
        if (isClass(functionName) || isCreatingObject(functionName, assignment)) {
            if (assignment->parent->_isSimpleExpression()) {
                SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
                if (simpleExpression->prefixExpression->prefixOperator == 0) {
                    PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
                    if ((leftSide->postfixes == 0) && (leftSide->primaryExpression->_isIdentifierExpression())) {
                        IdentifierExpression* memberExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                        string* memberName = memberExpression->name;
                        ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                        if (classDeclaration != nullptr) {
                            if (isVariableMember(memberName, classDeclaration))
                                return memberName;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

string* SourceVisitor::getFunctionName(Assignment* assignment) {
    if (assignment->expression->_isSimpleExpression()) {
        SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->expression);
        PrefixExpression* prefixExpression = simpleExpression->prefixExpression;
        if (prefixExpression->prefixOperator == nullptr) {
            PostfixExpression* rightSide = prefixExpression->expression;
            if (rightSide->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* classExpression = (IdentifierExpression*)(rightSide->primaryExpression);
                return classExpression->name;
            }
            else {
                if (rightSide->primaryExpression->_isConstructorCall()) {
                    ConstructorCall* constructorCall = (ConstructorCall*)(rightSide->primaryExpression);
                    Type* type = constructorCall->typeToConstruct;
                    return type->name;
                }
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::isCreatingObject(string* functionName, SyntaxNode* node) {
    ClassDeclaration* classDeclaration = getClassDeclaration(node);
    if (classDeclaration == nullptr)
        return false;
    _Array<ClassMember>* members = classDeclaration->body->members;
    if (members == nullptr)
        return false;
    ClassMember* member = nullptr;
    size_t _members_length = members->length();
    for (size_t _i = 0; _i < _members_length; _i++) {
        member = *(*members)[_i];
        {
            if (member->declaration->_isFunctionDeclaration()) {
                FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)(member->declaration);
                if (functionDeclaration->name->equals(functionName))
                    return true;
            }
        }
    }
    return false;
}

ClassDeclaration* SourceVisitor::getClassDeclaration(SyntaxNode* node) {
    if (node->_isClassDeclaration())
        return (ClassDeclaration*)node;
    if (node->parent != nullptr)
        return getClassDeclaration(node->parent);
    return nullptr;
}

bool SourceVisitor::isVariableMember(string* memberName, ClassDeclaration* classDeclaration) {
    _Array<ClassMember>* classMembers = classDeclaration->body->members;
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
                    return true;
            }
        }
    }
    return false;
}

bool SourceVisitor::openTypeQuery(TypeQuery* typeQuery) {
    sourceFile->append("->_is");
    return true;
}

bool SourceVisitor::openCatchClause(CatchClause* catchClause) {
    if (catchClause->parent->_isFunctionCall()) {
        FunctionCall* functionCall = (FunctionCall*)(catchClause->parent);
        {
            BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
            if (bindingInitializer != nullptr) {
                PatternInitializer* patternInitializer = bindingInitializer->initializer;
                if (patternInitializer->pattern->_isIdentifierPattern()) {
                    IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
                    _Array<CatchClause>* catchClauses = functionCall->catchClauses;
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
                        indentSource();
                        sourceFile->append("    ");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append(" = _");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append("_result.getResult();\n");
                        indentSource();
                        sourceFile->append("}\n");
                    }
                    indentSource();
                    sourceFile->append("else");
                    if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                        IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)(catchClause->catchPattern);
                        sourceFile->append(" if (_");
                        sourceFile->append(identifierPattern->identifier);
                        sourceFile->append("_result._getErrorCode() == _");
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
                                    _Array<TuplePatternElement>* elements = bindingPattern->elements;
                                    TuplePatternElement* element = *(*elements)[0];
                                    if (element->pattern->_isIdentifierPattern()) {
                                        IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                        indentSource();
                                        sourceFile->append("    auto ");
                                        sourceFile->append(pattern->identifier);
                                        sourceFile->append(" = _");
                                        sourceFile->append(identifierPattern->identifier);
                                        sourceFile->append("_result.getError();\n");
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
                _Array<CatchClause>* catchClauses = functionCall->catchClauses;
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
                    sourceFile->append("_error->_getErrorCode()) {\n");
                    sourceIndentLevel++;
                }
                indentSource();
                if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                    sourceFile->append("case _");
                    IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)(catchClause->catchPattern);
                    sourceFile->append(identifierCatchPattern->name);
                    sourceFile->append("Code_");
                    if (identifierCatchPattern->member != nullptr) {
                        sourceFile->append(identifierCatchPattern->member->member);
                        sourceFile->append(":\n");
                        indentSource();
                        sourceFile->append("{\n");
                        sourceIndentLevel++;
                        indentSource();
                        sourceFile->append("_");
                        sourceFile->append(identifierCatchPattern->name);
                        sourceFile->append("_");
                        sourceFile->append(identifierCatchPattern->member->member);
                        sourceFile->append("* ");
                        if (catchClause->bindingPattern != nullptr) {
                            TuplePattern* bindingPattern = catchClause->bindingPattern;
                            if (bindingPattern->elements != nullptr) {
                                if (bindingPattern->elements->length() > 0) {
                                    _Array<TuplePatternElement>* elements = bindingPattern->elements;
                                    TuplePatternElement* element = *(*elements)[0];
                                    if (element->pattern->_isIdentifierPattern()) {
                                        IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                        sourceFile->append(pattern->identifier);
                                        sourceFile->append(" = _");
                                        sourceFile->append(identifierExpression->name);
                                        sourceFile->append("_error->get_");
                                        sourceFile->append(identifierCatchPattern->member->member);
                                        sourceFile->append("();");
                                    }
                                }
                            }
                        }
                        sourceFile->append("\n");
                        sourceIndentLevel--;
                    }
                }
                if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                    sourceFile->append("default:\n");
                    indentSource();
                    sourceFile->append("{\n");
                }
                sourceIndentLevel++;
                indentSource();
                catchClause->expression->accept(this);
                {
                    bool insertBreak = true;
                    if (catchClause->expression->_isSimpleExpression()) {
                        SimpleExpression* simpleExpression = (SimpleExpression*)catchClause->expression;
                        if (simpleExpression->prefixExpression->expression->primaryExpression->_isReturnExpression()) {
                            sourceFile->append(";\n");
                            insertBreak = false;
                        }
                    }
                    if (insertBreak) {
                        indentSource();
                        sourceFile->append("break;\n");
                    }
                }
                sourceIndentLevel--;
                indentSource();
                sourceFile->append("}\n");
                _Array<CatchClause>* clauses = functionCall->catchClauses;
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
    return false;
}

IdentifierExpression* SourceVisitor::getIdentifierExpression(PostfixExpression* postfixExpression) {
    if (postfixExpression->parent->parent->parent->_isCodeBlock() || postfixExpression->parent->parent->parent->_isCompilationUnit()) {
        if (postfixExpression->parent->_isPrefixExpression()) {
            PrefixExpression* prefixExpression = (PrefixExpression*)(postfixExpression->parent);
            if (prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
                return (IdentifierExpression*)(prefixExpression->expression->primaryExpression);
            }
        }
    }
    return nullptr;
}

string* SourceVisitor::getErrorType(CatchClause* catchClause) {
    if (catchClause->bindingPattern != nullptr) {
        if (catchClause->bindingPattern->elements != nullptr) {
            if (catchClause->bindingPattern->elements->length() == 1) {
                _Array<TuplePatternElement>* elements = catchClause->bindingPattern->elements;
                TuplePatternElement* element = *(*elements)[0];
                if (element->pattern->_isIdentifierPattern()) {
                    IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                    if (pattern->annotationForType != nullptr) {
                        if (pattern->annotationForType->annotationForType->_isType()) {
                            Type* type = (Type*)(pattern->annotationForType->annotationForType);
                            return type->name;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

void SourceVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
    sourceFile->append(operatorPostfix->postfixOperator);
}

void SourceVisitor::visitMemberExpression(MemberExpression* memberExpression) {
    if (memberExpression->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(memberExpression->parent);
        if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
            IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
            if (postfixExpression->postfixes->length() > 1) {
                _Array<Postfix>* postfixes = postfixExpression->postfixes;
                if (((Postfix*)*(*postfixes)[0]) && ((*(*postfixes)[1])->_isFunctionCall())) {
                    if (isClass(identifierExpression->name)) {
                        sourceFile->append("::");
                        sourceFile->append(memberExpression->member);
                        return;
                    }
                }
            }
        }
    }
    sourceFile->append("->");
    sourceFile->append(memberExpression->member);
}

bool SourceVisitor::openSubscript(Subscript* subscript) {
    sourceFile->append("[");
    return true;
}

void SourceVisitor::closeSubscript(Subscript* subscript) {
    sourceFile->append("]");
}

void SourceVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
    if (!isLastExpressionElement(expressionElement))
        sourceFile->append(", ");
}

bool SourceVisitor::isLastExpressionElement(ExpressionElement* expressionElement) {
    if (expressionElement->parent->_isParenthesizedExpression()) {
        ParenthesizedExpression* parenthesizedExpression = (ParenthesizedExpression*)(expressionElement->parent);
        _Array<ExpressionElement>* expressionElements = parenthesizedExpression->expressionElements;
        size_t length = expressionElements->length() - 1;
        size_t i = 0;
        ExpressionElement* element = nullptr;
        size_t _expressionElements_length = expressionElements->length();
        for (size_t _i = 0; _i < _expressionElements_length; _i++) {
            element = *(*expressionElements)[_i];
            {
                if ((element == expressionElement) && (i == length))
                    return true;
                i++;
            }
        }
    }
    if (expressionElement->parent->_isSubscript()) {
        Subscript* subscript = (Subscript*)(expressionElement->parent);
        _Array<ExpressionElement>* expressions = subscript->expressions;
        size_t length = expressions->length() - 1;
        size_t i = 0;
        ExpressionElement* element = nullptr;
        size_t _expressions_length = expressions->length();
        for (size_t _i = 0; _i < _expressions_length; _i++) {
            element = *(*expressions)[_i];
            {
                if ((element == expressionElement) && (i == length))
                    return true;
                i++;
            }
        }
    }
    return false;
}

void SourceVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
    sourceFile->append(identifierExpression->name);
    if (identifierExpression->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(identifierExpression->parent);
        if (postfixExpression->postfixes != nullptr) {
            _Array<Postfix>* postfixes = postfixExpression->postfixes;
            if ((*(*postfixes)[0])->_isSubscript()) {
                sourceFile->append(")");
            }
        }
    }
}

bool SourceVisitor::inAssignment(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isAssignment())
        return true;
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return false;
    return inAssignment(parentNode);
}

Assignment* SourceVisitor::getAssignment(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isAssignment())
        return (Assignment*)syntaxNode;
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return nullptr;
    return getAssignment(parentNode);
}

bool SourceVisitor::inRetDeclaration(SyntaxNode* syntaxNode) {
    if (syntaxNode == nullptr)
        return false;
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
        return false;
    if (bindingInitializer->initializer != nullptr) {
        PatternInitializer* patternInitializer = bindingInitializer->initializer;
        if (patternInitializer->pattern->_isIdentifierPattern()) {
            IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
            return identifierPattern->identifier->equals("ret");
        }
    }
    return false;
}

void SourceVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
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

bool SourceVisitor::openIfExpression(IfExpression* ifExpression) {
    sourceFile->append("if (");
    ifExpression->condition->accept(this);
    sourceFile->append(")");
    if (ifExpression->consequent->_isSimpleExpression()) {
        sourceFile->append("\n    ");
        indentSource();
        ifExpression->consequent->accept(this);
        sourceFile->append(";\n");
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
    return false;
}

bool SourceVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    sourceFile->append("switch (");
    return true;
}

void SourceVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool SourceVisitor::openForExpression(ForExpression* forExpression) {
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
    return false;
}

bool SourceVisitor::openWhileExpression(WhileExpression* whileExpression) {
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
    return false;
}

bool SourceVisitor::openDoExpression(DoExpression* doExpression) {
    sourceFile->append("do");
    if (doExpression->code->_isSimpleExpression()) {
        sourceFile->append("\n");
        sourceIndentLevel++;
        indentSource();
        doExpression->code->accept(this);
        sourceFile->append(";");
        sourceIndentLevel--;
    }
    else {
        sourceFile->append(" ");
        doExpression->code->accept(this);
    }
    indentSource();
    sourceFile->append("while (");
    doExpression->condition->accept(this);
    sourceFile->append(")");
    return false;
}

bool SourceVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    if (!(parenthesizedExpression->parent)->_isReturnExpression())
        sourceFile->append("(");
    if (parenthesizedExpression->parent->_isFunctionCall()) {
        FunctionCall* functionCall = (FunctionCall*)(parenthesizedExpression->parent);
        if (functionCall->parent->_isPostfixExpression()) {
            PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                if (!isClass(identifierExpression->name)) {
                    if (postfixExpression->parent->parent->parent->_isAssignment()) {
                        Assignment* assignment = (Assignment*)(postfixExpression->parent->parent->parent);
                        string* member = getMemberIfCreatingObject(assignment);
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
            if (assignedToRootObject(functionCall)) {
                sourceFile->append("_p");
                parameterInserted = true;
            }
            else {
                if (assignedToReturnedObject(functionCall)) {
                    sourceFile->append("_rp");
                    parameterInserted = true;
                }
                else {
                    string* identifier = getLocalPage(functionCall);
                    if (identifier != nullptr) {
                        sourceFile->append(identifier);
                        sourceFile->append("->getPage()");
                        parameterInserted = true;
                    }
                }
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
        if (functionCall->catchClauses == nullptr) {
            if (functionCall->parent->_isPostfixExpression()) {
                PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
                if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                    IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                    if (identifierExpression->name->equals("print")) {
                        sourceFile->append("_ep");
                        parameterInserted = true;
                    }
                }
            }
        }
        if (parameterInserted && (parenthesizedExpression->expressionElements != nullptr))
            sourceFile->append(", ");
    }
    return true;
}

bool SourceVisitor::assignedToRootObject(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return false;
    return isRootBinding(bindingInitializer);
}

string* SourceVisitor::getLocalPage(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return nullptr;
    PatternInitializer* patternInitializer = bindingInitializer->initializer;
    if (patternInitializer->pattern->_isIdentifierPattern()) {
        IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
        Type* type = identifierPattern->annotationForType->annotationForType;
        if (type->lifeTime == nullptr)
            return nullptr;
        if (type->lifeTime->_isLocal()) {
            Local* local = (Local*)type->lifeTime;
            return local->location;
        }
    }
    return nullptr;
}

bool SourceVisitor::isRootBinding(BindingInitializer* bindingInitializer) {
    if (bindingInitializer == nullptr)
        return false;
    PatternInitializer* patternInitializer = bindingInitializer->initializer;
    if (patternInitializer->pattern->_isIdentifierPattern()) {
        IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
        Type* type = identifierPattern->annotationForType->annotationForType;
        if (type->lifeTime == nullptr)
            return false;
        if (type->lifeTime->_isRoot())
            return true;
    }
    return false;
}

bool SourceVisitor::assignedToReturnedObject(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return false;
    if (bindingInitializer->parent->_isConstantDeclaration()) {
        if (boundToObject(bindingInitializer))
            return true;
    }
    return false;
}

bool SourceVisitor::boundToObject(BindingInitializer* bindingInitializer) {
    if (bindingInitializer == nullptr)
        return false;
    PatternInitializer* patternInitializer = bindingInitializer->initializer;
    if (patternInitializer->pattern->_isIdentifierPattern()) {
        IdentifierPattern* identifierPattern = (IdentifierPattern*)(patternInitializer->pattern);
        Type* type = identifierPattern->annotationForType->annotationForType;
        if (type->lifeTime != nullptr)
            return false;
        if (isClass(type->name) && (getFunctionCall(patternInitializer) != nullptr))
            return true;
    }
    return false;
}

BindingInitializer* SourceVisitor::getBindingInitializer(FunctionCall* functionCall) {
    if (functionCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
        if (postfixExpression->parent->parent->parent->_isInitializer()) {
            Initializer* initializer = (Initializer*)(postfixExpression->parent->parent->parent);
            if (initializer->parent->_isPatternInitializer()) {
                PatternInitializer* patternInitializer = (PatternInitializer*)(initializer->parent);
                if (patternInitializer->parent->_isBindingInitializer()) {
                    return (BindingInitializer*)(patternInitializer->parent);
                }
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::callsInitializer(FunctionCall* functionCall) {
    if (functionCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
        if (postfixExpression->postfixes->length() == 1) {
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                if (isClass(identifierExpression->name)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool SourceVisitor::catchesError(FunctionCall* functionCall) {
    if (functionCall == nullptr)
        return false;
    if (functionCall->catchClauses != nullptr)
        return true;
    return false;
}

void SourceVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    if (!(parenthesizedExpression->parent)->_isReturnExpression())
        sourceFile->append(")");
}

bool SourceVisitor::openReturnExpression(ReturnExpression* returnExpression) {
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
            sourceFile->append(" ? new(_rp) ");
            sourceFile->append(returnType);
            sourceFile->append("(");
            returnExpression->expression->accept(this);
            sourceFile->append(") : nullptr");
        }
        if (thrownType != nullptr)
            sourceFile->append(")");
    }
    else {
        if (inTopLevelCode(returnExpression))
            sourceFile->append(" nullptr");
    }
    return false;
}

bool SourceVisitor::inTopLevelCode(SyntaxNode* syntaxNode) {
    if (syntaxNode == nullptr)
        return false;
    if (syntaxNode->_isFunctionDeclaration())
        return false;
    if (syntaxNode->_isCompilationUnit())
        return true;
    if (syntaxNode->parent == nullptr)
        return false;
    return inTopLevelCode(syntaxNode->parent);
}

void SourceVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
    _Region _region; _Page* _p = _region.get();
    string* returnType = getReturnType(_p, returnExpression);
    if (returnType != nullptr) {
        sourceFile->append(")");
    }
}

bool SourceVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    _Region _region; _Page* _p = _region.get();
    string* thrownType = getThrownType(_p, throwExpression);
    if (thrownType == nullptr)
        return false;
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
                                        _Array<TuplePatternElement>* elements = bindingPattern->elements;
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
    return false;
}

bool SourceVisitor::inWildcardCatchClause(ThrowExpression* throwExpression) {
    CatchClause* catchClause = getCatchClause(throwExpression);
    if ((catchClause != nullptr) && (catchClause->catchPattern->_isWildCardCatchPattern()))
        return true;
    return false;
}

CatchClause* SourceVisitor::getCatchClause(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isCatchClause())
        return (CatchClause*)syntaxNode;
    if (syntaxNode->parent == nullptr)
        return nullptr;
    return getCatchClause(syntaxNode->parent);
}

string* SourceVisitor::getReturnType(_Page* _rp, SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        FunctionResult* functionResult = functionDeclaration->signature->result;
        if (functionResult != nullptr) {
            VarString* ret = new(_rp) VarString();
            if (hasArrayPostfix(functionResult->resultType)) {
                Type* type = functionResult->resultType;
                ret->append("_Array<");
                appendCppTypeName(ret, type);
                ret->append(">");
                return new(_rp) string(ret);
            }
            else {
                appendCppTypeName(ret, functionResult->resultType);
                return new(_rp) string(ret);
            }
        }
    }
    return nullptr;
}

string* SourceVisitor::getThrownType(_Page* _rp, SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        ThrowsClause* throwsClause = functionDeclaration->signature->throwsClause;
        if (throwsClause != nullptr) {
            VarString* ret = new(_rp) VarString();
            if (hasArrayPostfix(throwsClause->throwsType)) {
                Type* type = throwsClause->throwsType;
                ret->append("_Array<");
                appendCppTypeName(ret, type);
                ret->append(">");
                return new(_rp) string(ret);
            }
            else {
                appendCppTypeName(ret, throwsClause->throwsType);
                return new(_rp) string(ret);
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::returnsArray(SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        FunctionResult* functionResult = functionDeclaration->signature->result;
        if (functionResult != nullptr) {
            if (hasArrayPostfix(functionResult->resultType))
                return true;
        }
    }
    return false;
}

FunctionDeclaration* SourceVisitor::getFunctionDeclaration(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isFunctionDeclaration())
        return (FunctionDeclaration*)syntaxNode;
    if (syntaxNode->parent == nullptr)
        return nullptr;
    return getFunctionDeclaration(syntaxNode->parent);
}

bool SourceVisitor::openBreakExpression(BreakExpression* breakExpression) {
    sourceFile->append("break");
    return true;
}

bool SourceVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    sourceFile->append("new(");
    if (!initializerIsBoundOrAssigned(constructorCall)) {
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
                        ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                        string* memberName = getMemberIfCreatingObject(assignment);
                        if (memberName != nullptr) {
                            if (isVariableMember(memberName, classDeclaration)) {
                                if (!inConstructor(assignment)) {
                                    sourceFile->append(memberName);
                                    sourceFile->append("->");
                                }
                                sourceFile->append("getPage()");
                                if (inConstructor(assignment)) {
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
    }
    else {
        if (constructorCall->parent->parent->parent->parent->_isAssignment()) {
            Assignment* assignment = (Assignment*)(constructorCall->parent->parent->parent->parent);
            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
            string* memberName = getMemberIfCreatingObject(assignment);
            if (memberName != nullptr) {
                if (isVariableMember(memberName, classDeclaration)) {
                    if (!inConstructor(assignment)) {
                        sourceFile->append(memberName);
                        sourceFile->append("->");
                    }
                    sourceFile->append("getPage()");
                    if (inConstructor(assignment)) {
                        sourceFile->append("->allocateExclusivePage()");
                    }
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
                        if ((classDeclaration != nullptr) && (memberName != nullptr) && !hasArrayPostfix(constructorCall->typeToConstruct)) {
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
    }
    sourceFile->append(") ");
    return true;
}

bool SourceVisitor::initializerIsBoundOrAssigned(ConstructorCall* initializerCall) {
    if (initializerCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(initializerCall->parent);
        if ((postfixExpression->parent->parent->parent->_isAssignment()) || (postfixExpression->parent->parent->parent->_isInitializer()))
            return true;
    }
    return false;
}

void SourceVisitor::visitThisExpression(ThisExpression* thisExpression) {
    sourceFile->append("this");
}

void SourceVisitor::visitNullExpression(NullExpression* nullExpression) {
    sourceFile->append("nullptr");
}

bool SourceVisitor::openElseClause(ElseClause* elseClause) {
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
    return false;
}

bool SourceVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    sourceFile->append(") {\n");
    sourceIndentLevel++;
    return true;
}

void SourceVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    sourceIndentLevel--;
    indentSource();
    sourceFile->append("}\n");
}

bool SourceVisitor::openSwitchCase(SwitchCase* switchCase) {
    indentSource();
    return true;
}

bool SourceVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    sourceFile->append("case ");
    return true;
}

void SourceVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
    sourceFile->append("default: ");
}

bool SourceVisitor::openCaseItem(CaseItem* caseItem) {
    sourceFile->append("case ");
    return true;
}

void SourceVisitor::closeCaseItem(CaseItem* caseItem) {
    sourceFile->append(": ");
}

bool SourceVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    if (identifierPattern->parent->_isPatternInitializer()) {
        PatternInitializer* patternInitializer = (PatternInitializer*)(identifierPattern->parent);
        if (isCatchingPatternInitializer(patternInitializer)) {
            sourceFile->append("auto _");
            sourceFile->append(identifierPattern->identifier);
            sourceFile->append("_result");
            return false;
        }
    }
    if (identifierPattern->annotationForType != nullptr) {
        identifierPattern->annotationForType->accept(this);
        sourceFile->append(" ");
    }
    sourceFile->append(identifierPattern->identifier);
    return false;
}

bool SourceVisitor::isCatchingPatternInitializer(PatternInitializer* patternInitializer) {
    if (patternInitializer->initializer != nullptr) {
        if (patternInitializer->initializer->expression->_isSimpleExpression()) {
            SimpleExpression* simpleExpression = (SimpleExpression*)(patternInitializer->initializer->expression);
            PostfixExpression* postfixExpression = simpleExpression->prefixExpression->expression;
            if (postfixExpression->postfixes != nullptr) {
                FunctionCall* functionCall = nullptr;
                if (postfixExpression->postfixes->length() > 0) {
                    _Array<Postfix>* postfixes = postfixExpression->postfixes;
                    Postfix* postfix = *(*postfixes)[0];
                    if (postfix->_isFunctionCall()) {
                        functionCall = (FunctionCall*)postfix;
                    }
                    else {
                        if (postfix->_isMemberExpression()) {
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
                        return true;
                }
            }
        }
    }
    return false;
}

void SourceVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
    if (expressionPattern->parent->_isItemCaseLabel())
        sourceFile->append(": ");
}

bool SourceVisitor::openCaseContent(CaseContent* caseContent) {
    sourceFile->append("{\n");
    sourceIndentLevel++;
    return true;
}

void SourceVisitor::closeCaseContent(CaseContent* caseContent) {
    bool additionalLineFeed = true;
    if (caseContent->parent->_isSwitchCase()) {
        SwitchCase* switchCase = (SwitchCase*)caseContent->parent;
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

bool SourceVisitor::openType(Type* type) {
    if (hasArrayPostfix(type)) {
        sourceFile->append("_Array<");
    }
    appendCppTypeName(sourceFile, type);
    return true;
}

void SourceVisitor::closeType(Type* type) {
    if (hasArrayPostfix(type)) {
        if (!sourceIndentLevel) {
            sourceFile->append(">");
            if (!type->parent->_isConstructorCall())
                sourceFile->append("*");
        }
        else {
            sourceFile->append(">");
            if (!type->parent->_isConstructorCall())
                sourceFile->append("*");
        }
    }
    if (isClass(type->name) && !hasArrayPostfix(type) && !inTypeQuery(type) && !type->parent->_isConstructorCall()) {
        sourceFile->append("*");
    }
    if (inTypeQuery(type))
        sourceFile->append("()");
}

bool SourceVisitor::inTypeQuery(Type* type) {
    if (type->parent->_isTypeQuery())
        return true;
    return false;
}

void SourceVisitor::visitPointer(Pointer* pointer) {
    sourceFile->append("*");
}

bool SourceVisitor::openInheritance(Inheritance* inheritance) {
    return false;
}

void SourceVisitor::buildProjectFileString(Program* program) {
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
    projectFile->append("  <VirtualDirectory Name=\"src\">\n");
    {
        _Array<CompilationUnit>* compilationUnits = program->compilationUnits;
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
                projectFile->append("    <File Name=\"");
                projectFile->append(fileName);
                projectFile->append(".scaly\"/>\n");
            }
        }
    }
    projectFile->append("  </VirtualDirectory>\n  <VirtualDirectory Name=\"include\">\n");
    {
        _Array<CompilationUnit>* compilationUnits = program->compilationUnits;
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
    projectFile->append("  </VirtualDirectory>\n  <Dependencies Name=\"Debug\"/>\n  <Dependencies Name=\"Release\"/>\n");
    projectFile->append("  <Settings Type=\"Executable\">\n    <GlobalSettings>\n");
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
    projectFile->append(" -d ../");
    projectFile->append(program->name);
    {
        _Array<CompilationUnit>* compilationUnits = program->compilationUnits;
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
    projectFile->append("        <SearchPaths/>\n      </Completion>\n    </Configuration>\n  </Settings>\n");
    projectFile->append("  <Dependencies Name=\"Debug\"/>\n  <Dependencies Name=\"Release\"/>\n</CodeLite_Project>\n");
}

void SourceVisitor::collectInheritances(Program* program) {
    _Array<CompilationUnit>* compilationUnits = program->compilationUnits;
    CompilationUnit* compilationUnit = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        compilationUnit = *(*compilationUnits)[_i];
        collectInheritancesInCompilationUnit(compilationUnit);
    }
}

void SourceVisitor::collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit) {
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

void SourceVisitor::registerInheritance(string* className, string* baseName) {
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

bool SourceVisitor::_isSourceVisitor() { return (true); }


}
