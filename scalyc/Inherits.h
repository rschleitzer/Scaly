#ifndef __scalyc__Inherits__
#define __scalyc__Inherits__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Inherits : public Object {
public:
    String* name;
    _Array<String>* inheritors;
    Inherits(String* className);

};

}
#endif // __scalyc__Inherits__
