#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

namespace Plan {


Source::Source(String path, String name) : path(path), name(name) {}

PlanType::PlanType(String name, Vector<String> fields) : name(name), fields(fields) {}

BasicBlock::BasicBlock(String name, Vector<PlanInstruction> instructions, Vector<String> predecessors, Vector<String> successors, bool is_entry, DebugInfo* debug_info) : name(name), instructions(instructions), predecessors(predecessors), successors(successors), is_entry(is_entry), debug_info(debug_info) {}

DebugInfo::DebugInfo(String file_name, String directory, String producer, int version, int language, int line, int column) : file_name(file_name), directory(directory), producer(producer), version(version), language(language), line(line), column(column) {}

LLVMType::LLVMType(String name, String llvm_name, int size_bits, int alignment, bool is_pointer, bool is_array, LLVMType* element_type, int array_size) : name(name), llvm_name(llvm_name), size_bits(size_bits), alignment(alignment), is_pointer(is_pointer), is_array(is_array), element_type(element_type), array_size(array_size) {}

Variable::Variable(String name, String type, DebugInfo* debug_info, bool is_parameter, bool is_local, int scope_line) : name(name), type(type), debug_info(debug_info), is_parameter(is_parameter), is_local(is_local), scope_line(scope_line) {}

PhiIncoming::PhiIncoming(String value, String block) : value(value), block(block) {}

SwitchCase::SwitchCase(String value, String block) : value(value), block(block) {}

CallData::CallData(String calling_convention, bool tail_call, Vector<String> function_attributes) : calling_convention(calling_convention), tail_call(tail_call), function_attributes(function_attributes) {}

LoadData::LoadData(bool is_atomic, String ordering, int alignment) : is_atomic(is_atomic), ordering(ordering), alignment(alignment) {}

StoreData::StoreData(bool is_atomic, String ordering, int alignment) : is_atomic(is_atomic), ordering(ordering), alignment(alignment) {}

AllocaData::AllocaData(String allocated_type, String* array_size, int alignment) : allocated_type(allocated_type), array_size(array_size), alignment(alignment) {}

GetElementPtrData::GetElementPtrData(bool is_inbounds, Vector<String> indices) : is_inbounds(is_inbounds), indices(indices) {}

PhiData::PhiData(Vector<PhiIncoming> incoming_values) : incoming_values(incoming_values) {}

BranchData::BranchData(bool is_conditional, String* condition, String true_block, String* false_block) : is_conditional(is_conditional), condition(condition), true_block(true_block), false_block(false_block) {}

SwitchData::SwitchData(String condition, String default_block, Vector<SwitchCase> cases) : condition(condition), default_block(default_block), cases(cases) {}

ReturnData::ReturnData(String* value) : value(value) {}

IntrinsicData::IntrinsicData(String intrinsic_id, Vector<String> intrinsic_args) : intrinsic_id(intrinsic_id), intrinsic_args(intrinsic_args) {}

CompareData::CompareData(String predicate, String operand_type) : predicate(predicate), operand_type(operand_type) {}

CastData::CastData(String source_type, String dest_type, String cast_op) : source_type(source_type), dest_type(dest_type), cast_op(cast_op) {}

BinaryData::BinaryData(String operation, String operand_type, bool has_nsw, bool has_nuw, bool has_exact) : operation(operation), operand_type(operand_type), has_nsw(has_nsw), has_nuw(has_nuw), has_exact(has_exact) {}
InstructionData::InstructionData(struct CallData _Call) : _tag(Call), _Call(_Call) {}

InstructionData::InstructionData(struct LoadData _Load) : _tag(Load), _Load(_Load) {}

InstructionData::InstructionData(struct StoreData _Store) : _tag(Store), _Store(_Store) {}

InstructionData::InstructionData(struct AllocaData _Alloca) : _tag(Alloca), _Alloca(_Alloca) {}

InstructionData::InstructionData(struct GetElementPtrData _GetElementPtr) : _tag(GetElementPtr), _GetElementPtr(_GetElementPtr) {}

InstructionData::InstructionData(struct PhiData _Phi) : _tag(Phi), _Phi(_Phi) {}

InstructionData::InstructionData(struct BranchData _Branch) : _tag(Branch), _Branch(_Branch) {}

InstructionData::InstructionData(struct SwitchData _Switch) : _tag(Switch), _Switch(_Switch) {}

InstructionData::InstructionData(struct ReturnData _Return) : _tag(Return), _Return(_Return) {}

InstructionData::InstructionData(struct IntrinsicData _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}

InstructionData::InstructionData(struct CompareData _Compare) : _tag(Compare), _Compare(_Compare) {}

InstructionData::InstructionData(struct CastData _Cast) : _tag(Cast), _Cast(_Cast) {}

InstructionData::InstructionData(struct BinaryData _Binary) : _tag(Binary), _Binary(_Binary) {}


PlanInstruction::PlanInstruction(String* result, String name, Vector<String> args, String type, DebugInfo* debug_info, String opcode, HashMap<String, String> metadata, bool is_volatile, bool is_atomic, String ordering, int alignment, InstructionData* data) : result(result), name(name), args(args), type(type), debug_info(debug_info), opcode(opcode), metadata(metadata), is_volatile(is_volatile), is_atomic(is_atomic), ordering(ordering), alignment(alignment), data(data) {}

PlanArgument::PlanArgument(String name, String type, Vector<String> attributes, DebugInfo* debug_info) : name(name), type(type), attributes(attributes), debug_info(debug_info) {}

PlanFunction::PlanFunction(Source* source, String name, Vector<PlanArgument> input, String output, Vector<BasicBlock> blocks, DebugInfo* debug_info, String linkage, String calling_convention, Vector<String> function_attributes, Vector<Variable> variable_list, bool is_declaration, bool is_definition, String visibility, String gc_name) : source(source), name(name), input(input), output(output), blocks(blocks), debug_info(debug_info), linkage(linkage), calling_convention(calling_convention), function_attributes(function_attributes), variable_list(variable_list), is_declaration(is_declaration), is_definition(is_definition), visibility(visibility), gc_name(gc_name) {}

GlobalVariable::GlobalVariable(String name, String type, String linkage, String visibility, bool is_constant, String* initializer, String section, int alignment, DebugInfo* debug_info) : name(name), type(type), linkage(linkage), visibility(visibility), is_constant(is_constant), initializer(initializer), section(section), alignment(alignment), debug_info(debug_info) {}

Metadata::Metadata(String kind, String value, Vector<String> operands) : kind(kind), value(value), operands(operands) {}

PlanModule::PlanModule(String name, String source_filename, String target_triple, String data_layout, HashMap<String, String> module_flags, DebugInfo* debug_compile_unit, HashMap<String, Vector<Metadata>> named_metadata) : name(name), source_filename(source_filename), target_triple(target_triple), data_layout(data_layout), module_flags(module_flags), debug_compile_unit(debug_compile_unit), named_metadata(named_metadata) {}

PlanProgram::PlanProgram(Source* source, PlanModule module_info, HashMap<String, PlanType> types, HashMap<String, LLVMType> llvm_types, HashMap<String, PlanFunction> functions, HashMap<String, GlobalVariable> global_variables, HashMap<String, Metadata> metadata, Vector<DebugInfo> debug_info_nodes) : source(source), module_info(module_info), types(types), llvm_types(llvm_types), functions(functions), global_variables(global_variables), metadata(metadata), debug_info_nodes(debug_info_nodes) {}

}

}
}