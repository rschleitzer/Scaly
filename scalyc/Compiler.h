#ifndef __scalyc__Compiler__
#define __scalyc__Compiler__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Compiler : public Object {
public:
    static void compileFiles(Options* options);
    static _Result<Module, CompilerError> parseUnit(_Page* _rp, _Page* _ep, string* moduleName, string* text);

};

}
#endif // __scalyc__Compiler__
