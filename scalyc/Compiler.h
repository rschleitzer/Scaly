#ifndef __scalyc__Compiler__
#define __scalyc__Compiler__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Compiler {
public:
    static CompilerError* compileFiles(_Page* _ep, Array<String>& files);
    static _Result<CompilationUnit, ParserError> compileUnit(_Page* _rp, _Page* _ep, String& text);
};

}

#endif /* defined(__scalyc__Compiler__) */