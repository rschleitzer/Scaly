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

namespace scaly {
namespace compiler {
namespace generator {

    void generate_module(Planner::Plan& plan) {
        Region _r;
        LLVMContext context;
        llvm::Module module(plan.name.to_c_string(_r.get_page()), context);
        IRBuilder<> Builder(context);

        llvm::FunctionType *funcType = llvm::FunctionType::get
        (
            Builder.getInt32Ty(),
            {
                Builder.getInt32Ty(),
                Builder.getInt32Ty()
            },
            false
        );

        llvm::Function *func = llvm::Function::Create
        (
            funcType,
            llvm::Function::ExternalLinkage,
            "add",
            module
        );

        llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func);
        Builder.SetInsertPoint(entry);

        llvm::Value *sum = Builder.CreateAdd(func->getArg(0), func->getArg(1), "sum");
        Builder.CreateRet(sum);
        if (llvm::verifyModule(module, &llvm::errs())) {
                llvm::errs() << "Error: Invalid module!\n";
                exit(1);
            }
        module.print(llvm::outs(), nullptr);
    }
}
}
}