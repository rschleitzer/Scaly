#ifndef __scalyc__Inherits__
#define __scalyc__Inherits__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Inherits : public Object {
public:
    string* name;
    _Array<string>* inheritors;
    Inherits(string* className);

};

}
#endif // __scalyc__Inherits__
