#ifndef __scalyc__Model__
#define __scalyc__Model__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Unit;

class Model : public Object {
public:
    string* name;
    _Array<Unit>* units;
    Model(string* name);

};

class Unit : public Object {
public:

};

class ModelVisitor : public Visitor {
public:
    Model* model;
    Unit* unit;
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);

    virtual bool _isModelVisitor();
};

}
#endif // __scalyc__Model__
