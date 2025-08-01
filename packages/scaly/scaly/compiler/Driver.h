#ifndef _Driver_h
#define _Driver_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;

using namespace scaly::compiler::model;

using namespace scaly::compiler::transpiler;

using namespace scaly::compiler::generator;
namespace compiler {

Result<Void, CompilerError> transpile(Page* ep, String file_name, String program_name);
Result<Void, CompilerError> compile(Page* ep, String file_name, String program_name);
}
#endif