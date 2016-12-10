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

bool CommonVisitor::_isCommonVisitor() { return (true); }

bool CommonVisitor::_isCppVisitor() { return (false); }
bool CommonVisitor::_isHeaderVisitor() { return (false); }
bool CommonVisitor::_isSourceVisitor() { return (false); }


}
