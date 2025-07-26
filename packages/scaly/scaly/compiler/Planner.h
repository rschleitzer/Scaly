#ifndef _Planner_h
#define _Planner_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;

struct Planner : Object {
    Program& program;
    HashSetBuilder<String> intrinsics_builder;
    List<Plan::Type> types_list;
    HashMapBuilder<String, Plan::Type> types_builder;
    List<Plan::Function> functions_list;
    HashMapBuilder<String, Plan::Function> functions_builder;

    Planner(Program& program, HashSetBuilder<String> intrinsics_builder, List<Plan::Type> types_list, HashMapBuilder<String, Plan::Type> types_builder, List<Plan::Function> functions_list, HashMapBuilder<String, Plan::Function> functions_builder);
    Planner(Program& program);
    Result<Plan::Module, PlannerError> plan_program(Page* rp, Page* ep);
    Result<Void, PlannerError> plan_module(Page* ep, Module& module_);
    Result<Void, PlannerError> plan_symbols(Page* ep, Vector<Member> symbols);
    Result<Void, PlannerError> plan_concept(Page* ep, Concept& concept);
    Result<Void, PlannerError> plan_intrinsic(Page* ep, String name);
    String resolve_type(Page* rp, Type type);
    Result<Void, PlannerError> plan_function(Page* ep, Function& func);
};

#endif