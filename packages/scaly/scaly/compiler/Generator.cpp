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
    if (type.equals("int32"))
        return llvm::Type::getInt32Ty(context);
    if (type.equals("ptr"))
        return llvm::PointerType::get(context, 0);
    exit(1);
}

llvm::Value* get_value(String& value, HashMap<String, llvm::Value*>& argument_values_map, HashMapBuilder<String, llvm::Value*>& block_values_builder) {
    if (argument_values_map.contains(value))
        return *argument_values_map[value];

    if (block_values_builder.contains(value))
        return *block_values_builder[value];

    return nullptr;
}

void add_instruction(IRBuilder<>& builder, HashMap<String, llvm::Value*>& argument_values_map, HashMapBuilder<String, llvm::Value*>& block_values_builder, Plan::Instruction instruction) {
    auto r = Region();
    if (instruction.name.equals("fmul")) {
        auto lValue = get_value(*(instruction.args[0]), argument_values_map, block_values_builder);
        auto rValue = get_value(*(instruction.args[1]), argument_values_map, block_values_builder);
        auto result = builder.CreateFMul(lValue, rValue, instruction.result->to_c_string(r.get_page()));
        block_values_builder.add(*(instruction.result), result);
    }
    else if (instruction.name.equals("mul")) {
        auto lValue = get_value(*(instruction.args[0]), argument_values_map, block_values_builder);
        auto rValue = get_value(*(instruction.args[1]), argument_values_map, block_values_builder);
        auto result = builder.CreateMul(lValue, rValue, instruction.result->to_c_string(r.get_page()));
        block_values_builder.add(*(instruction.result), result);
    }
    else if (instruction.name.equals("ret")) {
        if (instruction.args.length)
            builder.CreateRet(get_value(*(instruction.args[0]), argument_values_map, block_values_builder));
        else
            builder.CreateRetVoid();
    }
}

llvm::Function* generate_function(llvm::Module& module, DIFile* di_file, DIBuilder& di_builder, Plan::Function& plan_function) {
    auto r = Region();
    LLVMContext& context = module.getContext();
    llvm::Type* return_type = get_Type(context, plan_function.output);

    std::vector<llvm::Type*> llvm_argument_types;
    auto _argument_iterator = plan_function.input.get_iterator();
    while (auto _argument = _argument_iterator.next()) {
        auto argument = *_argument;
        llvm::Type* argument_type = get_Type(context, argument.type);
        llvm_argument_types.push_back(argument_type);
    }

    FunctionType* llvm_function_type = FunctionType::get(return_type, llvm_argument_types, false);
    auto function_name = plan_function.name.to_c_string(r.get_page());
    llvm::Function* llvm_function = llvm::Function::Create(llvm_function_type, llvm::Function::ExternalLinkage, function_name, module);

    HashMapBuilder<String, llvm::Value*>& argument_values_builder = *new (alignof(HashMapBuilder<String, llvm::Value>*), r.get_page()) HashMapBuilder<String, llvm::Value*>();
    _argument_iterator = plan_function.input.get_iterator();
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

    IRBuilder<> builder(context);
    auto subprogram = di_builder.createFunction(di_file, function_name, StringRef(), di_file, 1,
    di_builder.createSubroutineType(di_builder.getOrCreateTypeArray({})),
    1, DINode::FlagPrototyped, DISubprogram::SPFlagDefinition);
    auto argument_values_map = HashMap<String, llvm::Value*>(r.get_page(), argument_values_builder);
    auto _block_iterator = plan_function.blocks.get_iterator();
    while (auto _block = _block_iterator.next()) {
        auto block = *_block;
        llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, block.name.to_c_string(r.get_page()), llvm_function);
        builder.SetInsertPoint(basicBlock);
        builder.SetCurrentDebugLocation(DebugLoc(DILocation::get(context, 2, 0, subprogram)));

        HashMapBuilder<String, llvm::Value*>& block_values_builder = *new (alignof(HashMapBuilder<String, llvm::Value*>), r.get_page()) HashMapBuilder<String, llvm::Value*>();
        auto _instruction_iterator = block.instructions.get_iterator();
        while (auto _instruction = _instruction_iterator.next()) {
            auto instruction = *_instruction;
            add_instruction(builder, argument_values_map, block_values_builder, instruction);
        }
    }


    llvm_function->setSubprogram(subprogram);

    return llvm_function;
}

void generate_module(Plan::Module& plan_module) {
    Region _r;
    auto context = std::make_unique<LLVMContext>();
    auto module = std::make_unique<llvm::Module>(plan_module.name.to_c_string(_r.get_page()), *context);
    module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", DEBUG_METADATA_VERSION);
    module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 5);

    DIBuilder di_builder(*module);
    DIFile* debug_file = di_builder.createFile(plan_module.name.to_c_string(_r.get_page()), plan_module.path.to_c_string(_r.get_page()));
    auto cu = di_builder.createCompileUnit(dwarf::DW_LANG_C, debug_file, "scaly", false, "", 0);

    auto _function_iterator = HashMapIterator<String, Plan::Function>(plan_module.functions);
    while (auto _function = _function_iterator.next()) {
        auto planFunction = *_function;
        auto function = generate_function(*module, debug_file, di_builder, planFunction);
    }

    di_builder.finalize();

    if (llvm::verifyModule(*module, &llvm::errs())) {
            llvm::errs() << "Error: Invalid module!\n";
            exit(1);
        }

    module->print(llvm::outs(), nullptr);

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    ExitOnError ExitOnErr;
    auto jitter = ExitOnErr(LLJITBuilder()
    // .setJITTargetMachineBuilder(
    //     ExitOnErr(JITTargetMachineBuilder::detectHost()))
    // .setObjectLinkingLayerCreator(
    //     [&](ExecutionSession &ES, const Triple &TT) {
    //         auto ObjLinkingLayer = 
    //             std::make_unique<ObjectLinkingLayer>(ES);
            
    //         // Correct debug support for LLVM 19
    //         auto DebugObjectManager = 
    //             ExitOnErr(DebugObjectManagerPlugin::Create(TT));
    //         ObjLinkingLayer->addPlugin(std::make_unique<DebugObjectManagerPlugin>(
    //             std::move(DebugObjectManager)));
            
    //         return ObjLinkingLayer;
    //     })
    .create());
    if (!jitter) {
        errs() << "Failed to create jitter\n";
        exit(1);
    }

    if (auto Err = jitter->addIRModule(orc::ThreadSafeModule(std::move(module), std::move(context)))) {
        errs() << "Failed to add IR module: " << Err << "\n";
        exit(1);
    }

    auto main_sym = jitter->lookup("main");
    if (!main_sym) {
        errs() << "Failed to lookup 'main'\n";
        exit(1);
    }
    
    // Cast to function pointer and execute
    auto *main = main_sym->toPtr<int(int, void*)>();
    double Result = main(2, nullptr);
}
}
}
}