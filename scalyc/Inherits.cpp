#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Inherits::Inherits(string* className) {
    name = new(getPage()) string(className);
    inheritors = new(getPage()->allocateExclusivePage()) _Array<string>();
}


}
