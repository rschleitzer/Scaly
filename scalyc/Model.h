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

class Definition : public Object {
public:

};

class Unit : public Object {
public:
    Model* model;
    _Array<Definition>* definitions;
    Unit(Model* model);

    virtual bool _isTopLevel();
};

class Action : public Object {
public:

};

class TopLevel : public Unit {
public:
    _Array<Action>* items;
    TopLevel(Model* model);

    virtual bool _isTopLevel();
};

class ModelVisitor : public CommonVisitor {
public:
    Model* model;
    Unit* unit;
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual void closeConstantDeclaration(ConstantDeclaration* constantDeclaration);

    virtual bool _isModelVisitor();
};

}
#endif // __scalyc__Model__
