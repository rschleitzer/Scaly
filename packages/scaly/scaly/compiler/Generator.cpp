#ifndef _scaly_h
#define _scaly_h
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <libgen.h>
#include <fcntl.h>
#include "../../forwards.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/DebugObjectManagerPlugin.h"
#include "llvm/Support/TargetSelect.h"
#include <llvm/IR/DIBuilder.h>

struct Void {};
enum Success { Ok, Error };

template<class OK, class ERROR>
struct Result {
    Result(OK _Ok) : _tag(Ok), _Ok(_Ok) {}
    Result(ERROR _ERROR) : _tag(Error), _Error(_ERROR) {}
    Success _tag;
    union {
        OK _Ok;
        ERROR _Error;
    };
};

namespace scaly {
#include "../../scaly/memory.h"
#include "../../scaly/containers.h"
#include "../../scaly/io.h"
#include "../../scaly/json.h"
#include "../../scaly/compiler.h"

}
#endif

using namespace llvm;
using namespace llvm::orc;

namespace scaly {
namespace compiler {
namespace generator {

llvm::Type* get_Type(LLVMContext& context, String& type) {
    if (type.equals("void"))
        return llvm::Type::getVoidTy(context);
    if (type.equals("double"))
        return llvm::Type::getDoubleTy(context);
    if (type.equals("float"))
        return llvm::Type::getFloatTy(context);
    if (type.equals("int32"))
        return llvm::Type::getInt32Ty(context);
    if (type.equals("int64"))
        return llvm::Type::getInt64Ty(context);
    if (type.equals("int16"))
        return llvm::Type::getInt16Ty(context);
    if (type.equals("int8"))
        return llvm::Type::getInt8Ty(context);
    if (type.equals("int1"))
        return llvm::Type::getInt1Ty(context);
    if (type.equals("ptr"))
        return llvm::PointerType::get(context, 0);

    // Handle basic array and struct types with simple string comparisons
    auto type_str = type.to_c_string(Region().get_page());

    // Check for specific known composite types
    if (strcmp(type_str, "[4 x i32]") == 0) {
        return llvm::ArrayType::get(llvm::Type::getInt32Ty(context), 4);
    }
    if (strcmp(type_str, "[8 x i8]") == 0) {
        return llvm::ArrayType::get(llvm::Type::getInt8Ty(context), 8);
    }

    // Simple struct types
    if (strcmp(type_str, "{i32, i32}") == 0) {
        std::vector<llvm::Type*> field_types = {
            llvm::Type::getInt32Ty(context),
            llvm::Type::getInt32Ty(context)
        };
        return llvm::StructType::get(context, field_types);
    }

    exit(1);
}

llvm::Value* get_value(String& value, HashMap<String, llvm::Value*>& argument_values_map, HashMapBuilder<String, llvm::Value*>& block_values_builder, LLVMContext& context) {
    if (argument_values_map.contains(value))
        return *argument_values_map[value];

    if (block_values_builder.contains(value))
        return *block_values_builder[value];

    // Handle constants
    auto r = Region();
    auto value_str = value.to_c_string(r.get_page());

    // Check for specific constant patterns using string comparisons
    if (strncmp(value_str, "i32:", 4) == 0) {
        auto number = atoi(value_str + 4);
        return ConstantInt::get(llvm::Type::getInt32Ty(context), number);
    }
    if (strncmp(value_str, "i64:", 4) == 0) {
        auto number = atoll(value_str + 4);
        return ConstantInt::get(llvm::Type::getInt64Ty(context), number);
    }
    if (strncmp(value_str, "i1:", 3) == 0) {
        auto bool_val = (strcmp(value_str + 3, "true") == 0) || (strcmp(value_str + 3, "1") == 0);
        return ConstantInt::get(llvm::Type::getInt1Ty(context), bool_val);
    }

    // Floating point constants
    if (strncmp(value_str, "f64:", 4) == 0) {
        auto number = atof(value_str + 4);
        return ConstantFP::get(llvm::Type::getDoubleTy(context), number);
    }
    if (strncmp(value_str, "f32:", 4) == 0) {
        auto number = atof(value_str + 4);
        return ConstantFP::get(llvm::Type::getFloatTy(context), number);
    }

    // Null pointer
    if (strcmp(value_str, "null") == 0) {
        return ConstantPointerNull::get(PointerType::get(context, 0));
    }

    // Try parsing as plain integer
    char* endptr;
    long int_val = strtol(value_str, &endptr, 10);
    if (*endptr == '\0') {
        return ConstantInt::get(llvm::Type::getInt32Ty(context), int_val);
    }

    // Try parsing as plain float
    double float_val = strtod(value_str, &endptr);
    if (*endptr == '\0') {
        return ConstantFP::get(llvm::Type::getDoubleTy(context), float_val);
    }

    return nullptr;
}

void add_instruction(IRBuilder<>& builder, HashMap<String, llvm::Value*>& argument_values_map, HashMapBuilder<String, llvm::Value*>& block_values_builder, Plan::PlanInstruction instruction, LLVMContext& context) {
    auto r = Region();

    // Handle different instruction types based on the data field
    if (instruction.data != nullptr) {
        switch (instruction.data->_tag) {
            case Plan::InstructionData::Call: {
                auto call_data = instruction.data->_Call;
                // Handle function calls
                std::vector<llvm::Value*> args;
                for (size_t i = 1; i < instruction.args.length; i++) {
                    args.push_back(get_value(*(instruction.args[i]), argument_values_map, block_values_builder, context));
                }

                // Get function name from first argument
                auto func_name = instruction.args[0]->to_c_string(r.get_page());
                auto module = builder.GetInsertBlock()->getParent()->getParent();
                auto function = module->getFunction(func_name);

                if (function) {
                    auto result = builder.CreateCall(function, args, instruction.result ? instruction.result->to_c_string(r.get_page()) : "");
                    if (instruction.result) {
                        block_values_builder.add(*(instruction.result), result);
                    }
                }
                break;
            }

            case Plan::InstructionData::Load: {
                auto load_data = instruction.data->_Load;
                auto ptr = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
                auto result = builder.CreateLoad(get_Type(context, instruction.type), ptr, instruction.result->to_c_string(r.get_page()));
                block_values_builder.add(*(instruction.result), result);
                break;
            }

            case Plan::InstructionData::Store: {
                auto store_data = instruction.data->_Store;
                auto value = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
                auto ptr = get_value(*(instruction.args[1]), argument_values_map, block_values_builder, context);
                builder.CreateStore(value, ptr);
                break;
            }

            case Plan::InstructionData::Alloca: {
                auto alloca_data = instruction.data->_Alloca;
                auto type = get_Type(context, alloca_data.allocated_type);
                auto result = builder.CreateAlloca(type, nullptr, instruction.result->to_c_string(r.get_page()));
                block_values_builder.add(*(instruction.result), result);
                break;
            }

            case Plan::InstructionData::GetElementPtr: {
                auto gep_data = instruction.data->_GetElementPtr;
                auto ptr = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
                std::vector<llvm::Value*> indices;
                for (size_t i = 1; i < instruction.args.length; i++) {
                    indices.push_back(get_value(*(instruction.args[i]), argument_values_map, block_values_builder, context));
                }
                auto result = builder.CreateGEP(get_Type(context, instruction.type), ptr, indices, instruction.result->to_c_string(r.get_page()));
                block_values_builder.add(*(instruction.result), result);
                break;
            }

            case Plan::InstructionData::Phi: {
                auto phi_data = instruction.data->_Phi;
                auto phi = builder.CreatePHI(get_Type(context, instruction.type), phi_data.incoming_values.length, instruction.result->to_c_string(r.get_page()));
                // Note: Phi nodes need to be completed after all blocks are created
                block_values_builder.add(*(instruction.result), phi);
                break;
            }

            case Plan::InstructionData::Branch: {
                auto branch_data = instruction.data->_Branch;
                auto function = builder.GetInsertBlock()->getParent();

                if (branch_data.is_conditional && branch_data.condition) {
                    auto condition = get_value(*(branch_data.condition), argument_values_map, block_values_builder, context);

                    // Find or create basic blocks
                    llvm::BasicBlock* true_bb = nullptr;
                    llvm::BasicBlock* false_bb = nullptr;

                    for (auto& bb : *function) {
                        if (bb.getName() == branch_data.true_block.to_c_string(r.get_page())) {
                            true_bb = &bb;
                        }
                        if (branch_data.false_block && bb.getName() == branch_data.false_block->to_c_string(r.get_page())) {
                            false_bb = &bb;
                        }
                    }

                    if (!true_bb) {
                        true_bb = llvm::BasicBlock::Create(context, branch_data.true_block.to_c_string(r.get_page()), function);
                    }
                    if (branch_data.false_block && !false_bb) {
                        false_bb = llvm::BasicBlock::Create(context, branch_data.false_block->to_c_string(r.get_page()), function);
                    }

                    builder.CreateCondBr(condition, true_bb, false_bb);
                } else {
                    // Unconditional branch
                    llvm::BasicBlock* target_bb = nullptr;
                    for (auto& bb : *function) {
                        if (bb.getName() == branch_data.true_block.to_c_string(r.get_page())) {
                            target_bb = &bb;
                            break;
                        }
                    }
                    if (!target_bb) {
                        target_bb = llvm::BasicBlock::Create(context, branch_data.true_block.to_c_string(r.get_page()), function);
                    }
                    builder.CreateBr(target_bb);
                }
                break;
            }

            case Plan::InstructionData::Switch: {
                auto switch_data = instruction.data->_Switch;
                auto condition = get_value(switch_data.condition, argument_values_map, block_values_builder, context);
                auto default_bb = llvm::BasicBlock::Create(context, switch_data.default_block.to_c_string(r.get_page()));
                auto switch_inst = builder.CreateSwitch(condition, default_bb, switch_data.cases.length);

                auto _case_iterator = switch_data.cases.get_iterator();
                while (auto _case = _case_iterator.next()) {
                    auto case_data = *_case;
                    auto case_value = get_value(case_data.value, argument_values_map, block_values_builder, context);
                    auto case_bb = llvm::BasicBlock::Create(context, case_data.block.to_c_string(r.get_page()));
                    switch_inst->addCase(dyn_cast<ConstantInt>(case_value), case_bb);
                }
                break;
            }

            case Plan::InstructionData::Return: {
                auto return_data = instruction.data->_Return;
                if (return_data.value) {
                    auto value = get_value(*(return_data.value), argument_values_map, block_values_builder, context);
                    builder.CreateRet(value);
                } else {
                    builder.CreateRetVoid();
                }
                break;
            }

            case Plan::InstructionData::Compare: {
                auto compare_data = instruction.data->_Compare;
                auto lhs = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
                auto rhs = get_value(*(instruction.args[1]), argument_values_map, block_values_builder, context);

                llvm::Value* result = nullptr;
                if (compare_data.predicate.equals("eq")) {
                    result = builder.CreateICmpEQ(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (compare_data.predicate.equals("ne")) {
                    result = builder.CreateICmpNE(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (compare_data.predicate.equals("slt")) {
                    result = builder.CreateICmpSLT(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (compare_data.predicate.equals("sle")) {
                    result = builder.CreateICmpSLE(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (compare_data.predicate.equals("sgt")) {
                    result = builder.CreateICmpSGT(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (compare_data.predicate.equals("sge")) {
                    result = builder.CreateICmpSGE(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                }

                if (result && instruction.result) {
                    block_values_builder.add(*(instruction.result), result);
                }
                break;
            }

            case Plan::InstructionData::Cast: {
                auto cast_data = instruction.data->_Cast;
                auto value = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
                auto src_type = get_Type(context, cast_data.source_type);
                auto dest_type = get_Type(context, cast_data.dest_type);

                llvm::Value* result = nullptr;
                if (cast_data.cast_op.equals("trunc")) {
                    result = builder.CreateTrunc(value, dest_type, instruction.result->to_c_string(r.get_page()));
                } else if (cast_data.cast_op.equals("zext")) {
                    result = builder.CreateZExt(value, dest_type, instruction.result->to_c_string(r.get_page()));
                } else if (cast_data.cast_op.equals("sext")) {
                    result = builder.CreateSExt(value, dest_type, instruction.result->to_c_string(r.get_page()));
                } else if (cast_data.cast_op.equals("bitcast")) {
                    result = builder.CreateBitCast(value, dest_type, instruction.result->to_c_string(r.get_page()));
                }

                if (result && instruction.result) {
                    block_values_builder.add(*(instruction.result), result);
                }
                break;
            }

            case Plan::InstructionData::Binary: {
                auto binary_data = instruction.data->_Binary;
                auto lhs = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
                auto rhs = get_value(*(instruction.args[1]), argument_values_map, block_values_builder, context);

                llvm::Value* result = nullptr;
                if (binary_data.operation.equals("add")) {
                    result = builder.CreateAdd(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("sub")) {
                    result = builder.CreateSub(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("mul")) {
                    result = builder.CreateMul(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("udiv")) {
                    result = builder.CreateUDiv(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("sdiv")) {
                    result = builder.CreateSDiv(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("urem")) {
                    result = builder.CreateURem(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("srem")) {
                    result = builder.CreateSRem(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("shl")) {
                    result = builder.CreateShl(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("lshr")) {
                    result = builder.CreateLShr(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("ashr")) {
                    result = builder.CreateAShr(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("and")) {
                    result = builder.CreateAnd(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("or")) {
                    result = builder.CreateOr(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("xor")) {
                    result = builder.CreateXor(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("fadd")) {
                    result = builder.CreateFAdd(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("fsub")) {
                    result = builder.CreateFSub(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("fmul")) {
                    result = builder.CreateFMul(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("fdiv")) {
                    result = builder.CreateFDiv(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                } else if (binary_data.operation.equals("frem")) {
                    result = builder.CreateFRem(lhs, rhs, instruction.result->to_c_string(r.get_page()));
                }

                if (result && instruction.result) {
                    block_values_builder.add(*(instruction.result), result);
                }
                break;
            }

            case Plan::InstructionData::Intrinsic: {
                auto intrinsic_data = instruction.data->_Intrinsic;
                auto module = builder.GetInsertBlock()->getParent()->getParent();
                std::vector<llvm::Value*> args;
                std::vector<llvm::Type*> arg_types;

                for (size_t i = 0; i < instruction.args.length; i++) {
                    auto arg = get_value(*(instruction.args[i]), argument_values_map, block_values_builder, context);
                    args.push_back(arg);
                    if (arg) arg_types.push_back(arg->getType());
                }

                // Handle common LLVM intrinsics
                llvm::Value* result = nullptr;
                if (intrinsic_data.intrinsic_id.equals("llvm.memcpy")) {
                    auto intrinsic_func = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::memcpy, arg_types);
                    result = builder.CreateCall(intrinsic_func, args);
                } else if (intrinsic_data.intrinsic_id.equals("llvm.memset")) {
                    auto intrinsic_func = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::memset, arg_types);
                    result = builder.CreateCall(intrinsic_func, args);
                } else if (intrinsic_data.intrinsic_id.equals("llvm.sqrt")) {
                    auto intrinsic_func = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::sqrt, arg_types);
                    result = builder.CreateCall(intrinsic_func, args,
                        instruction.result ? instruction.result->to_c_string(r.get_page()) : "");
                } else if (intrinsic_data.intrinsic_id.equals("llvm.sin")) {
                    auto intrinsic_func = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::sin, arg_types);
                    result = builder.CreateCall(intrinsic_func, args,
                        instruction.result ? instruction.result->to_c_string(r.get_page()) : "");
                } else if (intrinsic_data.intrinsic_id.equals("llvm.cos")) {
                    auto intrinsic_func = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::cos, arg_types);
                    result = builder.CreateCall(intrinsic_func, args,
                        instruction.result ? instruction.result->to_c_string(r.get_page()) : "");
                } else if (intrinsic_data.intrinsic_id.equals("llvm.pow")) {
                    auto intrinsic_func = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::pow, arg_types);
                    result = builder.CreateCall(intrinsic_func, args,
                        instruction.result ? instruction.result->to_c_string(r.get_page()) : "");
                }

                if (result && instruction.result) {
                    block_values_builder.add(*(instruction.result), result);
                }
                break;
            }
        }
    }

    // Fallback to old string-based instruction handling for backward compatibility
    else if (instruction.name.equals("fmul")) {
        auto lValue = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
        auto rValue = get_value(*(instruction.args[1]), argument_values_map, block_values_builder, context);
        auto result = builder.CreateFMul(lValue, rValue, instruction.result->to_c_string(r.get_page()));
        block_values_builder.add(*(instruction.result), result);
    }
    else if (instruction.name.equals("mul")) {
        auto lValue = get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context);
        auto rValue = get_value(*(instruction.args[1]), argument_values_map, block_values_builder, context);
        auto result = builder.CreateMul(lValue, rValue, instruction.result->to_c_string(r.get_page()));
        block_values_builder.add(*(instruction.result), result);
    }
    else if (instruction.name.equals("ret")) {
        if (instruction.args.length)
            builder.CreateRet(get_value(*(instruction.args[0]), argument_values_map, block_values_builder, context));
        else
            builder.CreateRetVoid();
    }
}

llvm::Function* generate_function(llvm::Module& module, DIBuilder& di_builder, Plan::PlanFunction& plan_function, HashMap<String, llvm::Function*>& function_map) {
    auto r = Region();
    LLVMContext& context = module.getContext();

    // Use the existing function declaration instead of creating a new one
    llvm::Function* llvm_function = *function_map[plan_function.name];

    // Set up function arguments
    HashMapBuilder<String, llvm::Value*>& argument_values_builder = *new (alignof(HashMapBuilder<String, llvm::Value>*), r.get_page()) HashMapBuilder<String, llvm::Value*>();
    auto _argument_iterator = plan_function.input.get_iterator();
    {
        int i = 0;
        while (auto _argument = _argument_iterator.next()) {
            auto argument = *_argument;
            auto llvm_argument = llvm_function->getArg(i);
            llvm_argument->setName(argument.name.to_c_string(r.get_page()));
            argument_values_builder.add(argument.name, llvm_argument);
            i++;
        }
    }
    auto argument_values_map = HashMap<String, llvm::Value*>(r.get_page(), argument_values_builder);

    // Create debug info
    auto function_name = plan_function.name.to_c_string(r.get_page());
    DIFile* di_file = di_builder.createFile(plan_function.source->name.to_c_string(r.get_page()), plan_function.source->path.to_c_string(r.get_page()));
    auto subprogram = di_builder.createFunction(di_file, function_name, StringRef(), di_file, 1,
        di_builder.createSubroutineType(di_builder.getOrCreateTypeArray({})),
        1, DINode::FlagPrototyped, DISubprogram::SPFlagDefinition);

    // Create all basic blocks first to enable forward references
    HashMapBuilder<String, llvm::BasicBlock*> block_map_builder;
    auto _block_iterator = plan_function.blocks.get_iterator();
    while (auto _block = _block_iterator.next()) {
        auto block = *_block;
        llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, block.name.to_c_string(r.get_page()), llvm_function);
        block_map_builder.add(block.name, basicBlock);
    }
    auto block_map = HashMap<String, llvm::BasicBlock*>(r.get_page(), block_map_builder);

    // Store phi nodes for later completion
    std::vector<std::pair<llvm::PHINode*, Plan::PhiData>> incomplete_phis;

    IRBuilder<> builder(context);

    // Generate instructions for each block
    _block_iterator = plan_function.blocks.get_iterator();
    while (auto _block = _block_iterator.next()) {
        auto block = *_block;
        llvm::BasicBlock* basicBlock = *block_map[block.name];
        builder.SetInsertPoint(basicBlock);
        builder.SetCurrentDebugLocation(DebugLoc(DILocation::get(context, 2, 0, subprogram)));

        HashMapBuilder<String, llvm::Value*>& block_values_builder = *new (alignof(HashMapBuilder<String, llvm::Value*>), r.get_page()) HashMapBuilder<String, llvm::Value*>();

        auto _instruction_iterator = block.instructions.get_iterator();
        while (auto _instruction = _instruction_iterator.next()) {
            auto instruction = *_instruction;

            // Special handling for phi nodes to collect them for later completion
            if (instruction.data && instruction.data->_tag == Plan::InstructionData::Phi) {
                auto phi_data = instruction.data->_Phi;
                auto phi = builder.CreatePHI(get_Type(context, instruction.type), phi_data.incoming_values.length,
                    instruction.result ? instruction.result->to_c_string(r.get_page()) : "");
                if (instruction.result) {
                    block_values_builder.add(*(instruction.result), phi);
                }
                incomplete_phis.push_back(std::make_pair(phi, phi_data));
            } else {
                add_instruction(builder, argument_values_map, block_values_builder, instruction, context);
            }
        }
    }

    // Complete phi nodes now that all blocks and values are created
    for (auto& phi_pair : incomplete_phis) {
        auto phi = phi_pair.first;
        auto phi_data = phi_pair.second;

        auto _incoming_iterator = phi_data.incoming_values.get_iterator();
        while (auto _incoming = _incoming_iterator.next()) {
            auto incoming = *_incoming;
            auto value = get_value(incoming.value, argument_values_map,
                *new (alignof(HashMapBuilder<String, llvm::Value*>), r.get_page()) HashMapBuilder<String, llvm::Value*>(), context);
            auto block = *block_map[incoming.block];
            if (value && block) {
                phi->addIncoming(value, block);
            }
        }
    }

    llvm_function->setSubprogram(subprogram);

    return llvm_function;
}

void generate_module(Plan::PlanProgram& compilation) {
    Region _r;
    auto context = std::make_unique<LLVMContext>();
    auto module = std::make_unique<llvm::Module>(compilation.source->name.to_c_string(_r.get_page()), *context);

    // Set up module flags for optimization and debug info
    module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", DEBUG_METADATA_VERSION);
    module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 5);
    module->addModuleFlag(llvm::Module::Error, "wchar_size", 4);
    module->addModuleFlag(llvm::Module::Error, "PIC Level", 2);

    // Set up debug info builder
    DIBuilder di_builder(*module);
    DIFile* debug_file = di_builder.createFile(
        compilation.source->name.to_c_string(_r.get_page()),
        compilation.source->path.to_c_string(_r.get_page())
    );
    auto cu = di_builder.createCompileUnit(
        dwarf::DW_LANG_C, debug_file, "scaly compiler",
        false,  // isOptimized
        "",     // flags
        0       // runtimeVersion
    );

    // Generate global variables first
    auto _global_iterator = HashMapIterator<String, Plan::GlobalVariable>(compilation.global_variables);
    while (auto _global = _global_iterator.next()) {
        auto global_var = *_global;
        auto global_type = get_Type(*context, global_var.type);

        // Create the global variable
        auto llvm_global = new llvm::GlobalVariable(
            *module,
            global_type,
            global_var.is_constant,
            llvm::GlobalValue::ExternalLinkage,
            nullptr,  // initializer - would need to be set based on global_var.initializer
            global_var.name.to_c_string(_r.get_page())
        );

        // Set up debug info for global
        auto global_debug = di_builder.createGlobalVariableExpression(
            cu,
            global_var.name.to_c_string(_r.get_page()),
            "",  // linkage name
            debug_file,
            1,   // line number
            di_builder.createBasicType("type", 32, dwarf::DW_ATE_signed),
            false  // isLocalToUnit
        );
        llvm_global->addDebugInfo(global_debug);
    }

    // Generate function declarations first (for forward references)
    HashMapBuilder<String, llvm::Function*> function_map_builder;
    auto _function_iterator = HashMapIterator<String, Plan::PlanFunction>(compilation.functions);
    while (auto _function = _function_iterator.next()) {
        auto planFunction = *_function;

        // Create function type
        llvm::Type* return_type = get_Type(*context, planFunction.output);
        std::vector<llvm::Type*> llvm_argument_types;
        auto _argument_iterator = planFunction.input.get_iterator();
        while (auto _argument = _argument_iterator.next()) {
            auto argument = *_argument;
            llvm::Type* argument_type = get_Type(*context, argument.type);
            llvm_argument_types.push_back(argument_type);
        }

        FunctionType* llvm_function_type = FunctionType::get(return_type, llvm_argument_types, false);
        auto function_name = planFunction.name.to_c_string(_r.get_page());
        llvm::Function* llvm_function = llvm::Function::Create(
            llvm_function_type,
            llvm::Function::ExternalLinkage,
            function_name,
            *module
        );

        function_map_builder.add(planFunction.name, llvm_function);
    }
    auto function_map = HashMap<String, llvm::Function*>(_r.get_page(), function_map_builder);

    // Generate function bodies
    _function_iterator = HashMapIterator<String, Plan::PlanFunction>(compilation.functions);
    while (auto _function = _function_iterator.next()) {
        auto planFunction = *_function;
        auto function = generate_function(*module, di_builder, planFunction, function_map);
    }

    // Finalize debug info
    di_builder.finalize();

    // Verify the module
    if (llvm::verifyModule(*module, &llvm::errs())) {
        llvm::errs() << "Error: Invalid module!\n";
        exit(1);
    }

    // Print the module IR
    module->print(llvm::outs(), nullptr);

    // Initialize LLVM for JIT execution
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    ExitOnError ExitOnErr;
    auto jitter = ExitOnErr(LLJITBuilder()
        .create());
    if (!jitter) {
        errs() << "Failed to create jitter\n";
        exit(1);
    }

    // Add the module to the JIT
    if (auto Err = jitter->addIRModule(orc::ThreadSafeModule(std::move(module), std::move(context)))) {
        errs() << "Failed to add IR module: " << Err << "\n";
        exit(1);
    }

    // Look up and execute main function if it exists
    auto main_sym = jitter->lookup("main");
    if (!main_sym) {
        errs() << "Failed to lookup 'main'\n";
        exit(1);
    }

    // Cast to function pointer and execute
    auto *main = main_sym->toPtr<int(int, void*)>();
    int result = main(2, nullptr);

    llvm::outs() << "Program executed with result: " << result << "\n";
}
}
}
}