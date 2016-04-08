#ifndef __scalyc__Options__
#define __scalyc__Options__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Options : public Object {
public:
    Options();
    static _Result<Options, OptionsError> parseArguments(_Page* _rp, _Page* _ep, _Vector<String>* args);
    _Vector<String>* files;
    String* outputName;
    String* directory;

};

}
#endif // __scalyc__Options__
