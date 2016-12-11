#ifndef __scalyc__CommonVisitor__
#define __scalyc__CommonVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CommonVisitor : public Visitor {
public:
    virtual bool isTopLevelFile(CompilationUnit* compilationUnit);

    virtual bool _isCommonVisitor();
    virtual bool _isCppVisitor();
    virtual bool _isHeaderVisitor();
    virtual bool _isSourceVisitor();
    virtual bool _isModelVisitor();
};

}
#endif // __scalyc__CommonVisitor__
