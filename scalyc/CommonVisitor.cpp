#include "scalyc.h"
using namespace scaly;
namespace scalyc {

bool CommonVisitor::isTopLevelFile(CompilationUnit* compilationUnit) {
    Statement* statement = nullptr;
    size_t _compilationUnit_length = compilationUnit->statements->length();
    for (size_t _i = 0; _i < _compilationUnit_length; _i++) {
        statement = *(*compilationUnit->statements)[_i];
        if (statement->_isExpression())
            return true;
;
    }
    return false;
}

string* CommonVisitor::getFileName(_Page* _rp, CompilationUnit* compilationUnit) {
    if (compilationUnit->statements == nullptr)
        return nullptr;
    Statement* statement = nullptr;
    size_t _compilationUnit_length = compilationUnit->statements->length();
    for (size_t _i = 0; _i < _compilationUnit_length; _i++) {
        statement = *(*compilationUnit->statements)[_i];
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

bool CommonVisitor::_isCommonVisitor() { return (true); }

bool CommonVisitor::_isCppVisitor() { return (false); }
bool CommonVisitor::_isHeaderVisitor() { return (false); }
bool CommonVisitor::_isSourceVisitor() { return (false); }
bool CommonVisitor::_isModelVisitor() { return (false); }


}
