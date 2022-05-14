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

using namespace llvm;

static void InitializeModule() {
  // Open a new context and module.
  auto TheContext = std::make_unique<LLVMContext>();
  auto TheModule = std::make_unique<Module>("my cool jit", *TheContext);

  // Create a new builder for the module.
  auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
}


}
}