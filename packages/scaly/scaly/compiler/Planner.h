#ifndef _Planner_h
#define _Planner_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace planner {

    Result<Plan, TranspilerError> plan_program(Page* rp, Page* ep, Program& program);
    Result<Void, TranspilerError> plan_module(Page* ep, String path, Module& module_, String main_header, String namespace_open, String namespace_close);
}
#endif