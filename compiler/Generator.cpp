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

namespace scaly {
namespace compiler {

struct Generator : Object {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    Generator() : context(std::make_unique<llvm::LLVMContext>()) {}

    void initialize_module(const String& name) {
        Region _r;
        module = std::make_unique<llvm::Module>(name.to_c_string(_r.get_page()), *context);
        builder = std::make_unique<llvm::IRBuilder<>>(*context);
    }

    void print_module() {
        module->print(llvm::outs(), nullptr);
    }

    void save_module_to_file(const String& file_name) {
        Region _r;
        std::error_code error_code;
        llvm::raw_fd_ostream outLL(file_name.to_c_string(_r.get_page()), error_code);
        module->print(outLL, nullptr);
    }

};

}
}