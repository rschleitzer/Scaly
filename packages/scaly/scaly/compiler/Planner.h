#ifndef _Planner_h
#define _Planner_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace planner {

Result<Plan::Module, PlannerError> plan_program(Page* rp, Page* ep, Program& program);
Result<Void, PlannerError> plan_module(Page* ep, Module& module_);
Result<Void, PlannerError> plan_symbols(Page* ep, Vector<Member> symbols);
Result<Void, PlannerError> plan_concept(Page* ep, Concept& concept);
Result<Void, PlannerError> plan_intrinsic(Page* ep, String name);
Result<Void, PlannerError> plan_function(Page* ep, Function& func);
}
#endif