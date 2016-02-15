#ifndef __scalyc__Options__
#define __scalyc__Options__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Options : Object {
public:
    Options();
    static _Result<Options, OptionsError> parseArguments(_Page* _rp, _Page* _ep, _Array<_LetString>& args);
    
    _Array<_LetString>* files;
    _LetString* outputName;
    _LetString* directory;
};

}

#endif /* defined(__scalyc__Option__) */