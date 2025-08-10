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
    Result<Void, PlannerError> plan_main_function(Page* ep, Program& program);
    Result<Void, PlannerError> plan_module(Page* ep, Module& module_);
    Result<Void, PlannerError> plan_symbols(Page* ep, String file, HashMap<String, Nameable>& symbols, Vector<Member> members);
    Result<Void, PlannerError> plan_concept(Page* ep, String file, HashMap<String, Nameable>& symbols, Concept& concept);
    Result<Void, PlannerError> plan_intrinsic(Page* ep, String name);
    Result<String, PlannerError> resolve_type(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Type type);
    Result<List<Plan::Instruction>*, PlannerError> plan_function_arguments(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Function& function_, Tuple& tuple, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values);
    String allocate_value_name(Page* rp, List<Plan::Block>& blocks, List<Plan::Instruction>& instructions);
    Result<List<Plan::Instruction>*, PlannerError> plan_instruction_call(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Function& function_, Tuple& tuple, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions);
    Result<List<Plan::Instruction>*, PlannerError> plan_type(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Type& type, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values);
    Result<List<Plan::Instruction>*, PlannerError> plan_statement(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Statement& statement, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions);
    Result<List<Plan::Instruction>*, PlannerError> plan_block(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Block& block, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions);
    Result<List<Plan::Instruction>*, PlannerError> plan_operand(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Operand& operand, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values);
    Result<List<Plan::Instruction>*, PlannerError> plan_operation(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Vector<Operand>& operation, String* required_type, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values);
    Result<List<Plan::Instruction>*, PlannerError> plan_action(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, String* required_type, Action& action, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values);
    Result<Void, PlannerError> plan_function(Page* ep, String file, HashMap<String, Nameable>& symbols, Function& func);
};

#endif