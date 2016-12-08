#ifndef __scalyc__Compiler__
#define __scalyc__Compiler__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Compiler : public Object {
public:
    static void compileFiles(Options* options);
    static CompilationUnit* parseUnit(_Page* _rp, string* moduleName, string* text);

};

}
#endif // __scalyc__Compiler__
