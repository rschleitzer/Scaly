#ifndef __scalyc__Options__
#define __scalyc__Options__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Options : public Object {
public:
    _Array<string>* files;
    string* outputName;
    string* directory;
    Options(_Array<string>* input, string* output, string* dir);
    static _Result<Options, OptionsError> parseArguments(_Page* _rp, _Page* _ep, _Array<string>* args);

};

}
#endif // __scalyc__Options__
