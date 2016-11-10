#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Inherits::Inherits(String* className) {
    name = new(getPage()) String(className);
    inheritors = new(getPage()->allocateExclusivePage()) _Array<String>();
}


}
