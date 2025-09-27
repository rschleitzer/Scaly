#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;
namespace Plan {

struct Source : Object {
    String path;
    String name;

    Source(String path, String name);
};

struct PlanType : Object {
    String name;
    Vector<String> fields;

    PlanType(String name, Vector<String> fields);
};

struct BasicBlock : Object {
    String name;
    Vector<PlanInstruction> instructions;
    Vector<String> predecessors;
    Vector<String> successors;
    bool is_entry;
    DebugInfo* debug_info;

    BasicBlock(String name, Vector<PlanInstruction> instructions, Vector<String> predecessors, Vector<String> successors, bool is_entry, DebugInfo* debug_info);
};

struct DebugInfo : Object {
    String file_name;
    String directory;
    String producer;
    int version;
    int language;
    int line;
    int column;

    DebugInfo(String file_name, String directory, String producer, int version, int language, int line, int column);
};

struct LLVMType : Object {
    String name;
    String llvm_name;
    int size_bits;
    int alignment;
    bool is_pointer;
    bool is_array;
    LLVMType* element_type;
    int array_size;

    LLVMType(String name, String llvm_name, int size_bits, int alignment, bool is_pointer, bool is_array, LLVMType* element_type, int array_size);
};

struct Variable : Object {
    String name;
    String type;
    DebugInfo* debug_info;
    bool is_parameter;
    bool is_local;
    int scope_line;

    Variable(String name, String type, DebugInfo* debug_info, bool is_parameter, bool is_local, int scope_line);
};

struct PhiIncoming : Object {
    String value;
    String block;

    PhiIncoming(String value, String block);
};

struct SwitchCase : Object {
    String value;
    String block;

    SwitchCase(String value, String block);
};

struct CallData : Object {
    String calling_convention;
    bool tail_call;
    Vector<String> function_attributes;

    CallData(String calling_convention, bool tail_call, Vector<String> function_attributes);
};

struct LoadData : Object {
    bool is_atomic;
    String ordering;
    int alignment;

    LoadData(bool is_atomic, String ordering, int alignment);
};

struct StoreData : Object {
    bool is_atomic;
    String ordering;
    int alignment;

    StoreData(bool is_atomic, String ordering, int alignment);
};

struct AllocaData : Object {
    String allocated_type;
    String* array_size;
    int alignment;

    AllocaData(String allocated_type, String* array_size, int alignment);
};

struct GetElementPtrData : Object {
    bool is_inbounds;
    Vector<String> indices;

    GetElementPtrData(bool is_inbounds, Vector<String> indices);
};

struct PhiData : Object {
    Vector<PhiIncoming> incoming_values;

    PhiData(Vector<PhiIncoming> incoming_values);
};

struct BranchData : Object {
    bool is_conditional;
    String* condition;
    String true_block;
    String* false_block;

    BranchData(bool is_conditional, String* condition, String true_block, String* false_block);
};

struct SwitchData : Object {
    String condition;
    String default_block;
    Vector<SwitchCase> cases;

    SwitchData(String condition, String default_block, Vector<SwitchCase> cases);
};

struct ReturnData : Object {
    String* value;

    ReturnData(String* value);
};

struct IntrinsicData : Object {
    String intrinsic_id;
    Vector<String> intrinsic_args;

    IntrinsicData(String intrinsic_id, Vector<String> intrinsic_args);
};

struct CompareData : Object {
    String predicate;
    String operand_type;

    CompareData(String predicate, String operand_type);
};

struct CastData : Object {
    String source_type;
    String dest_type;
    String cast_op;

    CastData(String source_type, String dest_type, String cast_op);
};

struct BinaryData : Object {
    String operation;
    String operand_type;
    bool has_nsw;
    bool has_nuw;
    bool has_exact;

    BinaryData(String operation, String operand_type, bool has_nsw, bool has_nuw, bool has_exact);
};

struct InstructionData : Object {
    InstructionData(CallData);
    InstructionData(LoadData);
    InstructionData(StoreData);
    InstructionData(AllocaData);
    InstructionData(GetElementPtrData);
    InstructionData(PhiData);
    InstructionData(BranchData);
    InstructionData(SwitchData);
    InstructionData(ReturnData);
    InstructionData(IntrinsicData);
    InstructionData(CompareData);
    InstructionData(CastData);
    InstructionData(BinaryData);
    enum {
        Call,
        Load,
        Store,
        Alloca,
        GetElementPtr,
        Phi,
        Branch,
        Switch,
        Return,
        Intrinsic,
        Compare,
        Cast,
        Binary,
    } _tag;
    union {
        struct CallData _Call;
        struct LoadData _Load;
        struct StoreData _Store;
        struct AllocaData _Alloca;
        struct GetElementPtrData _GetElementPtr;
        struct PhiData _Phi;
        struct BranchData _Branch;
        struct SwitchData _Switch;
        struct ReturnData _Return;
        struct IntrinsicData _Intrinsic;
        struct CompareData _Compare;
        struct CastData _Cast;
        struct BinaryData _Binary;
    };
};
struct PlanInstruction : Object {
    String* result;
    String name;
    Vector<String> args;
    String type;
    DebugInfo* debug_info;
    String opcode;
    HashMap<String, String> metadata;
    bool is_volatile;
    bool is_atomic;
    String ordering;
    int alignment;
    InstructionData* data;

    PlanInstruction(String* result, String name, Vector<String> args, String type, DebugInfo* debug_info, String opcode, HashMap<String, String> metadata, bool is_volatile, bool is_atomic, String ordering, int alignment, InstructionData* data);
};

struct PlanArgument : Object {
    String name;
    String type;
    Vector<String> attributes;
    DebugInfo* debug_info;

    PlanArgument(String name, String type, Vector<String> attributes, DebugInfo* debug_info);
};

struct PlanFunction : Object {
    Source* source;
    String name;
    Vector<PlanArgument> input;
    String output;
    Vector<BasicBlock> blocks;
    DebugInfo* debug_info;
    String linkage;
    String calling_convention;
    Vector<String> function_attributes;
    Vector<Variable> variable_list;
    bool is_declaration;
    bool is_definition;
    String visibility;
    String gc_name;

    PlanFunction(Source* source, String name, Vector<PlanArgument> input, String output, Vector<BasicBlock> blocks, DebugInfo* debug_info, String linkage, String calling_convention, Vector<String> function_attributes, Vector<Variable> variable_list, bool is_declaration, bool is_definition, String visibility, String gc_name);
};

struct GlobalVariable : Object {
    String name;
    String type;
    String linkage;
    String visibility;
    bool is_constant;
    String* initializer;
    String section;
    int alignment;
    DebugInfo* debug_info;

    GlobalVariable(String name, String type, String linkage, String visibility, bool is_constant, String* initializer, String section, int alignment, DebugInfo* debug_info);
};

struct Metadata : Object {
    String kind;
    String value;
    Vector<String> operands;

    Metadata(String kind, String value, Vector<String> operands);
};

struct PlanModule : Object {
    String name;
    String source_filename;
    String target_triple;
    String data_layout;
    HashMap<String, String> module_flags;
    DebugInfo* debug_compile_unit;
    HashMap<String, Vector<Metadata>> named_metadata;

    PlanModule(String name, String source_filename, String target_triple, String data_layout, HashMap<String, String> module_flags, DebugInfo* debug_compile_unit, HashMap<String, Vector<Metadata>> named_metadata);
};

struct PlanProgram : Object {
    Source* source;
    PlanModule module_info;
    HashMap<String, PlanType> types;
    HashMap<String, LLVMType> llvm_types;
    HashMap<String, PlanFunction> functions;
    HashMap<String, GlobalVariable> global_variables;
    HashMap<String, Metadata> metadata;
    Vector<DebugInfo> debug_info_nodes;

    PlanProgram(Source* source, PlanModule module_info, HashMap<String, PlanType> types, HashMap<String, LLVMType> llvm_types, HashMap<String, PlanFunction> functions, HashMap<String, GlobalVariable> global_variables, HashMap<String, Metadata> metadata, Vector<DebugInfo> debug_info_nodes);
};

}
#endif