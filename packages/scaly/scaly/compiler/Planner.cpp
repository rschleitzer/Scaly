#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;


Result<Plan, TranspilerError> planner::plan(Page* rp, Page* ep, Program& program) {
    return Result<Plan, TranspilerError>(Plan());
}

}
}