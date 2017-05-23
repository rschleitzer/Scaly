#ifndef __scalycpp__Options__
#define __scalycpp__Options__
#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

class Options : public Object {
public:
    _Array<string>* files;
    string* outputName;
    string* directory;
    Options(_Array<string>* input, string* output, string* dir);
    static _Result<Options, OptionsError> parseArguments(_Page* _rp, _Page* _ep, _Array<string>* args);

};

}
#endif // __scalycpp__Options__
