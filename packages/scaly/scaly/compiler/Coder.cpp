#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;


Result<Void, TranspilerError> coder::code_plan(Page* ep, Plan& plan) {
    return Result<Void, TranspilerError>(Void());
}

}
}