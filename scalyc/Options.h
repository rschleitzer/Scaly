#ifndef __scalyc__Options__
#define __scalyc__Options__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Options : Object {
public:
    Options(Array<String>& args);
    static _Result<Options, OptionsError> ParseArguments(_Page* _rp, _Page* _ep, Array<String>& args);
    
    Array<String>& files;
};

}

#endif /* defined(__scalyc__Option__) */