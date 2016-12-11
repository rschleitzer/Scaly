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
    Model* model;

    virtual bool _isTopLevelCode();
    virtual bool _isCode();
};

class Item : public Object {
public:

    virtual bool _isDefinition();
};

class Definition : public Item {
public:

    virtual bool _isDefinition();
};

class TopLevelCode : public Unit {
public:
    _Array<Item>* items;
    TopLevelCode(Model* model);

    virtual bool _isTopLevelCode();
};

class Code : public Unit {
public:
    _Array<Definition>* definitions;
    Code(Model* model);

    virtual bool _isCode();
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
