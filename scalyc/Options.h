#ifndef __scalyc__Options__
#define __scalyc__Options__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Options : Object {
public:
    Options();
    static _Result<Options, OptionsError> parseArguments(_Page* _rp, _Page* _ep, _Array<String>& args);
    
    _Array<String>* files;
    String* outputName;
    String* directory;
};

}

#endif /* defined(__scalyc__Option__) */