#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Model::Model(string* name) {
    this->name = name;
    units = new(_getPage()) _Array<Unit>();
}

bool Unit::_isTopLevelCode() { return (false); }
bool Unit::_isCode() { return (false); }

bool Item::_isDefinition() { return (false); }

bool Definition::_isDefinition() { return (true); }

bool TopLevelCode::_isTopLevelCode() { return (true); }

bool Code::_isCode() { return (true); }

bool ModelVisitor::openProgram(Program* program) {
    if (model != nullptr)
        model->_getPage()->clear();
    model = new(model == nullptr ? _getPage()->allocateExclusivePage() : model->_getPage()) Model(program->name);
    return true;
}

bool ModelVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    if (unit != nullptr)
        unit->_getPage()->clear();
    unit = new(unit == nullptr ? _getPage()->allocateExclusivePage() : unit->_getPage()) Unit();
    return true;
}

void ModelVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
    model->units->push(unit);
}

bool ModelVisitor::_isModelVisitor() { return (true); }


}
