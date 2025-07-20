#ifndef _Coder_h
#define _Coder_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace coder {

Result<Void, TranspilerError> code_plan(Page* ep, Plan& plan);
}
#endif