#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Inherits::Inherits(string* className) {
    name = new(_getPage()) string(className);
    inheritors = new(_getPage()->allocateExclusivePage()) _Array<string>();
}

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

bool CppVisitor::_isCppVisitor() { return (true); }

bool CppVisitor::_isHeaderVisitor() { return (false); }
bool CppVisitor::_isSourceVisitor() { return (false); }


}
