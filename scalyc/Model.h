#ifndef __scalyc__Model__
#define __scalyc__Model__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Model : public Object {
public:

};

class ModelVisitor : public Visitor {
public:

    virtual bool _isModelVisitor();
};

}
#endif // __scalyc__Model__
