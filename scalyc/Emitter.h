// Emitter.h - LLVM IR generation from Plan
// Generates LLVM IR for both JIT execution (tests) and AOT compilation.
// Supports DWARF debug info from the start.

#pragma once

#include "Plan.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Error.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include <map>
#include <memory>
#include <string>

namespace scaly {

// ============================================================================
// Intrinsic Type Mapping
// ============================================================================

// Maps intrinsic type names to LLVM types
// This is the backend-specific knowledge that the prelude declares
struct IntrinsicTypeInfo {
    llvm::Type *LLVMType;
    size_t Size;
    size_t Alignment;
    bool IsSigned;
};

// ============================================================================
// Emitter Configuration
// ============================================================================

struct EmitterConfig {
    bool EmitDebugInfo = true;   // Generate DWARF
    bool OptimizeForSize = false;
    int OptLevel = 0;            // 0 = none, 1-3 = increasing
    std::string TargetTriple;    // Empty = host
    std::string DataLayout;      // Empty = default for target
};

// ============================================================================
// Emitter
// ============================================================================

class Emitter {
public:
    explicit Emitter(const EmitterConfig &Config = {});
    ~Emitter();

    // Main entry: emit a complete plan to LLVM IR
    llvm::Expected<std::unique_ptr<llvm::Module>> emit(const Plan &P,
                                                        llvm::StringRef ModuleName);

    // JIT execution: compile and run, return result
    // Useful for tests: compile expression, execute, check result
    llvm::Expected<int64_t> jitExecuteInt(const Plan &P);
    llvm::Expected<double> jitExecuteFloat(const Plan &P);
    llvm::Expected<bool> jitExecuteBool(const Plan &P);

    // AOT: write to object file
    llvm::Error emitObjectFile(const Plan &P,
                               llvm::StringRef ModuleName,
                               llvm::StringRef OutputPath);

    // Access the context (for advanced use)
    llvm::LLVMContext &getContext() { return *Context; }

private:
    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::Module> Module;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::DIBuilder> DIBuilder;
    EmitterConfig Config;

    // Debug info
    llvm::DICompileUnit *DICompileUnit = nullptr;
    llvm::DIFile *DIFile = nullptr;
    std::map<std::string, llvm::DIType*> DITypes;

    // Type cache: mangled name -> LLVM type
    std::map<std::string, llvm::Type*> TypeCache;
    std::map<std::string, llvm::StructType*> StructCache;

    // Function cache: mangled name -> LLVM function
    std::map<std::string, llvm::Function*> FunctionCache;

    // Intrinsic type info (baked in)
    std::map<std::string, IntrinsicTypeInfo> IntrinsicTypes;

    // Current function state (during emission)
    llvm::Function *CurrentFunction = nullptr;
    llvm::BasicBlock *CurrentBlock = nullptr;
    std::map<std::string, llvm::Value*> LocalVariables;

    // Region/lifetime management
    struct RegionInfo {
        llvm::Value *LocalPage;    // Local ($) allocations
        llvm::Value *ReturnPage;   // Call (#) - passed by caller
        llvm::Value *ExceptionPage; // Thrown (!) - passed by caller
    };
    RegionInfo CurrentRegion;

    // Loop context for break/continue
    struct LoopContext {
        llvm::BasicBlock *ExitBlock;     // Target for 'break'
        llvm::BasicBlock *ContinueBlock; // Target for 'continue'
    };
    std::vector<LoopContext> LoopStack;

    // ========================================================================
    // Initialization
    // ========================================================================

    void initIntrinsicTypes();
    void initDebugInfo(llvm::StringRef FileName);

    // ========================================================================
    // Type Mapping
    // ========================================================================

    // Map PlannedType to LLVM type
    llvm::Type *mapType(const PlannedType &Type);

    // Map intrinsic type name to LLVM type
    llvm::Type *mapIntrinsicType(llvm::StringRef Name);

    // Create LLVM struct type for PlannedStructure
    llvm::StructType *emitStructType(const PlannedStructure &Struct);

    // Create LLVM type for PlannedUnion (tagged union)
    llvm::StructType *emitUnionType(const PlannedUnion &Union);

    // Get size/alignment for a type
    size_t getTypeSize(llvm::Type *Ty);
    size_t getTypeAlignment(llvm::Type *Ty);

    // ========================================================================
    // Debug Info
    // ========================================================================

    llvm::DIType *emitDebugType(const PlannedType &Type);
    llvm::DISubroutineType *emitDebugFunctionType(const PlannedFunction &Func);
    void emitDebugLocation(Span Loc);

    // ========================================================================
    // Function Emission
    // ========================================================================

    // Emit function declaration (signature only)
    llvm::Function *emitFunctionDecl(const PlannedFunction &Func);

    // Emit function body
    llvm::Error emitFunctionBody(const PlannedFunction &Func,
                                  llvm::Function *LLVMFunc);

    // Emit initializer (constructor)
    llvm::Function *emitInitializerDecl(const PlannedStructure &Struct,
                                         const PlannedInitializer &Init);
    llvm::Error emitInitializerBody(const PlannedStructure &Struct,
                                     const PlannedInitializer &Init,
                                     llvm::Function *LLVMFunc);

    // Emit deinitializer (destructor)
    llvm::Function *emitDeInitializerDecl(const PlannedStructure &Struct,
                                           const PlannedDeInitializer &DeInit);
    llvm::Error emitDeInitializerBody(const PlannedStructure &Struct,
                                       const PlannedDeInitializer &DeInit,
                                       llvm::Function *LLVMFunc);

    // Emit operator
    llvm::Function *emitOperator(const PlannedOperator &Op);

    // ========================================================================
    // Statement Emission
    // ========================================================================

    llvm::Error emitStatement(const PlannedStatement &Stmt);
    llvm::Expected<llvm::Value*> emitAction(const PlannedAction &Action);
    llvm::Error emitBinding(const PlannedBinding &Binding);
    llvm::Error emitReturn(const PlannedReturn &Return);
    llvm::Error emitThrow(const PlannedThrow &Throw);
    llvm::Error emitBreak(const PlannedBreak &Break);
    llvm::Error emitContinue(const PlannedContinue &Continue);

    // ========================================================================
    // Expression Emission
    // ========================================================================

    // Emit expression, returning the LLVM value
    llvm::Expected<llvm::Value*> emitExpression(const PlannedExpression &Expr);

    // Emit operand sequence (handles postfix operators)
    llvm::Expected<llvm::Value*> emitOperands(const std::vector<PlannedOperand> &Ops);

    // Emit single operand
    llvm::Expected<llvm::Value*> emitOperand(const PlannedOperand &Op);

    // Emit constant
    llvm::Value *emitConstant(const PlannedConstant &Const, const PlannedType &Type);

    // Emit tuple construction
    llvm::Expected<llvm::Value*> emitTuple(const PlannedTuple &Tuple);

    // ========================================================================
    // Control Flow Emission
    // ========================================================================

    llvm::Expected<llvm::Value*> emitBlock(const PlannedBlock &Block);
    llvm::Expected<llvm::Value*> emitIf(const PlannedIf &If);
    llvm::Expected<llvm::Value*> emitChoose(const PlannedChoose &Choose);
    llvm::Expected<llvm::Value*> emitMatch(const PlannedMatch &Match);
    llvm::Expected<llvm::Value*> emitFor(const PlannedFor &For);
    llvm::Expected<llvm::Value*> emitWhile(const PlannedWhile &While);
    llvm::Expected<llvm::Value*> emitTry(const PlannedTry &Try);
    llvm::Expected<llvm::Value*> emitSizeOf(const PlannedSizeOf &SizeOf);
    llvm::Expected<llvm::Value*> emitIs(const PlannedIs &Is);
    llvm::Expected<llvm::Value*> emitMatrix(const PlannedMatrix &Matrix);

    // ========================================================================
    // Memory/Region Management
    // ========================================================================

    // Allocate in appropriate region based on lifetime
    llvm::Value *allocate(llvm::Type *Ty, Lifetime Life, llvm::StringRef Name = "");

    // Get pointer to region page for a lifetime
    llvm::Value *getRegionPage(Lifetime Life);

    // ========================================================================
    // Intrinsic Operations
    // ========================================================================

    // Emit a call expression (function or operator)
    llvm::Expected<llvm::Value*> emitCall(const PlannedCall &Call);

    // Emit intrinsic operator (add, sub, mul, etc.)
    llvm::Expected<llvm::Value*> emitIntrinsicOp(llvm::StringRef OpName,
                                                  llvm::Value *Left,
                                                  llvm::Value *Right,
                                                  const PlannedType &ResultType);

    // Emit intrinsic unary operator (-, !, ~)
    llvm::Expected<llvm::Value*> emitIntrinsicUnaryOp(llvm::StringRef OpName,
                                                       llvm::Value *Operand,
                                                       const PlannedType &ResultType);

    // Emit intrinsic function call
    llvm::Expected<llvm::Value*> emitIntrinsicCall(llvm::StringRef FuncName,
                                                    llvm::ArrayRef<llvm::Value*> Args,
                                                    const PlannedType &ResultType);

    // ========================================================================
    // Call Emission
    // ========================================================================

    // Emit function/procedure call
    llvm::Expected<llvm::Value*> emitCall(llvm::Function *Callee,
                                           llvm::ArrayRef<llvm::Value*> Args,
                                           const PlannedType *ReturnType,
                                           Lifetime ReturnLifetime);

    // Emit method call (with implicit this)
    llvm::Expected<llvm::Value*> emitMethodCall(llvm::Value *Receiver,
                                                 const PlannedFunction &Method,
                                                 llvm::ArrayRef<llvm::Value*> Args);

    // ========================================================================
    // Utility
    // ========================================================================

    // Create a basic block in current function
    llvm::BasicBlock *createBlock(llvm::StringRef Name);

    // Look up a variable by name
    llvm::Value *lookupVariable(llvm::StringRef Name);

    // Look up a function by mangled name
    llvm::Function *lookupFunction(llvm::StringRef MangledName);

    // ========================================================================
    // JIT Support
    // ========================================================================

    // Create a wrapper function that evaluates top-level statements and returns result
    llvm::Function *createJITWrapper(llvm::Type *ReturnType);

    // Execute JIT and return raw result pointer
    llvm::Expected<uint64_t> jitExecuteRaw(const Plan &P, llvm::Type *ExpectedType);
};

} // namespace scaly
