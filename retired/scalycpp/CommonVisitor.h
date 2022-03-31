#ifndef __scalycpp__CommonVisitor__
#define __scalycpp__CommonVisitor__
#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

class CommonVisitor : public Visitor {
public:
    virtual bool isTopLevelFile(Module* module);
    virtual string* getFileName(_Page* _rp, Module* module);

    virtual bool _isCommonVisitor();
    virtual bool _isCppVisitor();
    virtual bool _isHeaderVisitor();
    virtual bool _isSourceVisitor();
    virtual bool _isModelVisitor();
};

}
#endif // __scalycpp__CommonVisitor__
