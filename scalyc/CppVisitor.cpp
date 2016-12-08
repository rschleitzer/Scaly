#include "scalyc.h"
using namespace scaly;
namespace scalyc {

void CppVisitor::execute(Program* program) {
    program->accept(this);
}

bool CppVisitor::hasArrayPostfix(Type* type) {
    if (type->postfixes == nullptr)
        return false;
    _Array<TypePostfix>* postfixes = type->postfixes;
    TypePostfix* typePostfix = *(*postfixes)[0];
    if (typePostfix->_isIndexedType())
        return true;
    return false;
}

void CppVisitor::appendCppType(VarString* s, Type* type) {
    if (hasArrayPostfix(type)) {
        s->append("_Array<");
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

void CppVisitor::appendCppTypeName(VarString* s, Type* type) {
    string* typeName = type->name;
    if (typeName->equals("number")) {
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

void CppVisitor::collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit) {
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

void CppVisitor::registerInheritance(string* className, string* baseName) {
    Inherits* inherit = nullptr;
    Inherits* inh = nullptr;
    size_t _inherits_length = inherits->length();
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        inh = *(*inherits)[_i];
        {
            if (inh->name->equals(baseName)) {
                inherit = inh;
            }
        }
    }
    if (inherit == nullptr) {
        Inherits* newInherit = new(inherits->_getPage()) Inherits(baseName);
        inherit = newInherit;
        inherits->push(inherit);
    }
    inherit->inheritors->push(className);
}

string* CppVisitor::getFileName(_Page* _rp, CompilationUnit* compilationUnit) {
    _Array<Statement>* statements = compilationUnit->statements;
    Statement* statement = nullptr;
    size_t _statements_length = statements->length();
    for (size_t _i = 0; _i < _statements_length; _i++) {
        statement = *(*statements)[_i];
        {
            if (statement->_isClassDeclaration()) {
                ClassDeclaration* classDeclaration = (ClassDeclaration*)statement;
                if (classDeclaration->body != nullptr)
                    return new(_rp) string(classDeclaration->name);
            }
            if (statement->_isEnumDeclaration()) {
                EnumDeclaration* enumDeclaration = (EnumDeclaration*)statement;
                return new(_rp) string(enumDeclaration->name);
            }
        }
    }
    if (compilationUnit->parent->_isProgram()) {
        Program* program = (Program*)compilationUnit->parent;
        return new(_rp) string(program->name);
    }
    return nullptr;
}

bool CppVisitor::_isCppVisitor() { return (true); }

bool CppVisitor::_isHeaderVisitor() { return (false); }
bool CppVisitor::_isSourceVisitor() { return (false); }

Inherits::Inherits(string* className) {
    name = new(_getPage()) string(className);
    inheritors = new(_getPage()->allocateExclusivePage()) _Array<string>();
}


}
