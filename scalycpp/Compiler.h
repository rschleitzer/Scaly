#ifndef __scalycpp__Compiler__
#define __scalycpp__Compiler__
#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

class Compiler : public Object {
public:
    static void compileFiles(Options* options);
    static _Result<Module, CompilerError> parseUnit(_Page* _rp, _Page* _ep, string* moduleName, string* text);

};

}
#endif // __scalycpp__Compiler__
