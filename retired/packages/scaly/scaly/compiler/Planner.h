#ifndef _Planner_h
#define _Planner_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;

using namespace scaly::compiler::Plan;

struct Planner : Object {
    Program& program;
    HashSetBuilder<String> intrinsics_builder;
    HashMapBuilder<String, Plan::PlanType> types_builder;
    HashMapBuilder<String, Plan::PlanFunction> functions_builder;
    List<Plan::Source> module_list;

    Planner(Program& program, HashSetBuilder<String> intrinsics_builder, HashMapBuilder<String, Plan::PlanType> types_builder, HashMapBuilder<String, Plan::PlanFunction> functions_builder, List<Plan::Source> module_list);
    Planner(Program& program);
    Result<Plan::PlanProgram, PlannerError> plan_program(Page* rp, Page* ep);
    Result<Void, PlannerError> plan_main_function(Page* ep, Plan::Source* source, Program& program);
    Result<Plan::Source*, PlannerError> plan_module(Page* rp, Page* ep, Module& module_);
    Result<Void, PlannerError> plan_symbols(Page* ep, Plan::Source* source, String file, HashMap<String, Nameable>& symbols, Vector<Member> members);
    Result<Void, PlannerError> plan_concept(Page* ep, String file, HashMap<String, Nameable>& symbols, Concept& concept);
    Result<Void, PlannerError> plan_intrinsic(Page* ep, String name);
    Result<Void, PlannerError> plan_namespace(Page* ep, String namespace_name, String file, Namespace& ns);
    Result<Void, PlannerError> plan_type_definition(Page* ep, String name, Type& type_def);
    Result<String, PlannerError> resolve_type(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Type type);
    Result<String, PlannerError> resolve_qualified_type(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Vector<String> name_path, Span span);
    static String* get_type_from_environment(String name, List<HashMap<String, String>>& environment);
    Result<List<PlanInstruction>*, PlannerError> plan_function_arguments(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, Function& function_, Tuple& tuple, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values);
    String allocate_value_name(Page* rp, List<BasicBlock>& blocks, List<PlanInstruction>& instructions);
    Result<List<PlanInstruction>*, PlannerError> plan_instruction_call(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Function& function_, Tuple& tuple, List<BasicBlock>& blocks, List<PlanInstruction>* instructions);
    Result<List<PlanInstruction>*, PlannerError> plan_type_expression(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Type& type, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values);
    Result<List<PlanInstruction>*, PlannerError> plan_statement(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Statement& statement, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions);
    Result<List<PlanInstruction>*, PlannerError> plan_block(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Block& block, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions);
    Result<List<PlanInstruction>*, PlannerError> plan_operand(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Operand& operand, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values);
    Result<List<PlanInstruction>*, PlannerError> plan_operation(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, Vector<Operand>& operation, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values);
    Result<List<PlanInstruction>*, PlannerError> plan_action(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Action& action, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values);
    Result<Void, PlannerError> plan_function(Page* ep, Plan::Source* source, String file, HashMap<String, Nameable>& symbols, Function& func);
};

#endif