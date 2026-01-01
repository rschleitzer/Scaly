// Emitter.cpp - LLVM IR generation from Plan

#include "Emitter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include <cstring>

namespace scaly {

// ============================================================================
// Constructor / Destructor
// ============================================================================

Emitter::Emitter(const EmitterConfig &Cfg) : Config(Cfg) {
    // Initialize LLVM targets for JIT and AOT
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    Context = std::make_unique<llvm::LLVMContext>();
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);

    initIntrinsicTypes();
}

Emitter::~Emitter() = default;

// ============================================================================
// Intrinsic Type Initialization
// ============================================================================

void Emitter::initIntrinsicTypes() {
    // Integer types
    IntrinsicTypes["i1"] = {
        llvm::Type::getInt1Ty(*Context), 1, 1, false
    };
    IntrinsicTypes["i8"] = {
        llvm::Type::getInt8Ty(*Context), 1, 1, true
    };
    IntrinsicTypes["i16"] = {
        llvm::Type::getInt16Ty(*Context), 2, 2, true
    };
    IntrinsicTypes["i32"] = {
        llvm::Type::getInt32Ty(*Context), 4, 4, true
    };
    IntrinsicTypes["i64"] = {
        llvm::Type::getInt64Ty(*Context), 8, 8, true
    };

    // Unsigned integer types
    IntrinsicTypes["u8"] = {
        llvm::Type::getInt8Ty(*Context), 1, 1, false
    };
    IntrinsicTypes["u16"] = {
        llvm::Type::getInt16Ty(*Context), 2, 2, false
    };
    IntrinsicTypes["u32"] = {
        llvm::Type::getInt32Ty(*Context), 4, 4, false
    };
    IntrinsicTypes["u64"] = {
        llvm::Type::getInt64Ty(*Context), 8, 8, false
    };

    // Floating point types
    IntrinsicTypes["f32"] = {
        llvm::Type::getFloatTy(*Context), 4, 4, true
    };
    IntrinsicTypes["f64"] = {
        llvm::Type::getDoubleTy(*Context), 8, 8, true
    };

    // Pointer type (opaque in LLVM 15+)
    IntrinsicTypes["ptr"] = {
        llvm::PointerType::get(*Context, 0), 8, 8, false
    };

    // Common aliases (prelude may override)
    IntrinsicTypes["bool"] = IntrinsicTypes["i1"];
    IntrinsicTypes["int"] = IntrinsicTypes["i64"];
    IntrinsicTypes["size_t"] = IntrinsicTypes["u64"];
}

// ============================================================================
// Debug Info Initialization
// ============================================================================

void Emitter::initDebugInfo(llvm::StringRef FileName) {
    if (!Config.EmitDebugInfo)
        return;

    DIBuilder = std::make_unique<llvm::DIBuilder>(*Module);

    // Create compile unit
    DIFile = DIBuilder->createFile(FileName, ".");
    DICompileUnit = DIBuilder->createCompileUnit(
        llvm::dwarf::DW_LANG_C_plus_plus,  // Closest to Scaly semantics
        DIFile,
        "scalyc",      // Producer
        Config.OptLevel > 0,  // isOptimized
        "",            // Flags
        0              // Runtime version
    );
}

// ============================================================================
// Main Emission Entry Point
// ============================================================================

llvm::Expected<std::unique_ptr<llvm::Module>> Emitter::emit(const Plan &P,
                                                             llvm::StringRef ModuleName) {
    // Create new module
    Module = std::make_unique<llvm::Module>(ModuleName, *Context);

    // Set target triple
    if (Config.TargetTriple.empty()) {
        Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());
    } else {
        Module->setTargetTriple(Config.TargetTriple);
    }

    // Initialize debug info
    initDebugInfo(P.MainModule.File);

    // Clear caches for fresh emission
    TypeCache.clear();
    StructCache.clear();
    FunctionCache.clear();

    // Phase 1: Emit all type declarations (forward declarations for structs)
    for (const auto &[name, structPtr] : P.Structures) {
        if (auto *Struct = structPtr) {
            emitStructType(*Struct);
        }
    }
    for (const auto &[name, unionPtr] : P.Unions) {
        if (auto *Union = unionPtr) {
            emitUnionType(*Union);
        }
    }

    // Phase 2: Emit all function declarations
    for (const auto &[name, funcPtr] : P.Functions) {
        if (auto *Func = funcPtr) {
            emitFunctionDecl(*Func);
        }
    }

    // Phase 3: Emit function bodies
    for (const auto &[name, funcPtr] : P.Functions) {
        if (auto *Func = funcPtr) {
            if (auto *LLVMFunc = FunctionCache[Func->MangledName]) {
                if (auto Err = emitFunctionBody(*Func, LLVMFunc))
                    return std::move(Err);
            }
        }
    }

    // Phase 4: Emit top-level statements (program entry)
    // TODO: Create main function wrapper if needed

    // Finalize debug info
    if (DIBuilder) {
        DIBuilder->finalize();
    }

    // Verify module
    std::string VerifyError;
    llvm::raw_string_ostream VerifyStream(VerifyError);
    if (llvm::verifyModule(*Module, &VerifyStream)) {
        return llvm::make_error<llvm::StringError>(
            "Module verification failed: " + VerifyError,
            llvm::inconvertibleErrorCode()
        );
    }

    return std::move(Module);
}

// ============================================================================
// Type Mapping
// ============================================================================

llvm::Type *Emitter::mapType(const PlannedType &Type) {
    // Check cache first
    if (auto It = TypeCache.find(Type.MangledName); It != TypeCache.end()) {
        return It->second;
    }

    // Check if it's an intrinsic type
    if (auto It = IntrinsicTypes.find(Type.Name); It != IntrinsicTypes.end()) {
        TypeCache[Type.MangledName] = It->second.LLVMType;
        return It->second.LLVMType;
    }

    // Check if it's a struct we've already emitted
    if (auto It = StructCache.find(Type.MangledName); It != StructCache.end()) {
        return It->second;
    }

    // For pointer types: pointer[T]
    if (Type.Name == "pointer" && !Type.Generics.empty()) {
        // In LLVM 15+, all pointers are opaque (ptr)
        auto *PtrTy = llvm::PointerType::get(*Context, 0);
        TypeCache[Type.MangledName] = PtrTy;
        return PtrTy;
    }

    // For Option[T]: optimize to pointer when T is not a pointer
    if (Type.Name == "Option" && !Type.Generics.empty()) {
        const auto &Inner = Type.Generics[0];
        if (Inner.Name != "pointer") {
            // Option[NonPointer] -> ptr (null = None, non-null = Some)
            auto *PtrTy = llvm::PointerType::get(*Context, 0);
            TypeCache[Type.MangledName] = PtrTy;
            return PtrTy;
        }
        // Option[pointer[T]] -> ptr to ptr (need full tagged union)
        // Fall through to struct lookup
    }

    // Unknown type - should have been planned
    // Return opaque pointer as fallback
    return llvm::PointerType::get(*Context, 0);
}

llvm::Type *Emitter::mapIntrinsicType(llvm::StringRef Name) {
    if (auto It = IntrinsicTypes.find(Name.str()); It != IntrinsicTypes.end()) {
        return It->second.LLVMType;
    }
    return nullptr;
}

// ============================================================================
// Struct Type Emission
// ============================================================================

llvm::StructType *Emitter::emitStructType(const PlannedStructure &Struct) {
    // Check cache
    if (auto It = StructCache.find(Struct.MangledName); It != StructCache.end()) {
        return It->second;
    }

    // Create named struct type (allows recursive types)
    auto *StructTy = llvm::StructType::create(*Context, Struct.MangledName);
    StructCache[Struct.MangledName] = StructTy;

    // Map properties to LLVM types
    std::vector<llvm::Type*> Elements;
    for (const auto &Prop : Struct.Properties) {
        Elements.push_back(mapType(Prop.PropType));
    }

    // Set struct body
    StructTy->setBody(Elements, /*isPacked=*/false);

    return StructTy;
}

// ============================================================================
// Union Type Emission
// ============================================================================

llvm::StructType *Emitter::emitUnionType(const PlannedUnion &Union) {
    // Check cache
    if (auto It = StructCache.find(Union.MangledName); It != StructCache.end()) {
        return It->second;
    }

    // Tagged union layout: { i8 tag, [maxSize x i8] data }
    // Find max variant size
    size_t MaxSize = 0;
    for (const auto &Var : Union.Variants) {
        if (Var.VarType) {
            size_t VarSize = getTypeSize(mapType(*Var.VarType));
            MaxSize = std::max(MaxSize, VarSize);
        }
    }

    auto *TagTy = llvm::Type::getInt8Ty(*Context);
    auto *DataTy = llvm::ArrayType::get(llvm::Type::getInt8Ty(*Context), MaxSize);

    auto *UnionTy = llvm::StructType::create(*Context, {TagTy, DataTy}, Union.MangledName);
    StructCache[Union.MangledName] = UnionTy;

    return UnionTy;
}

// ============================================================================
// Function Declaration Emission
// ============================================================================

llvm::Function *Emitter::emitFunctionDecl(const PlannedFunction &Func) {
    // Check cache
    if (auto It = FunctionCache.find(Func.MangledName); It != FunctionCache.end()) {
        return It->second;
    }

    // Build parameter types
    std::vector<llvm::Type*> ParamTypes;

    // If returns non-primitive, add sret parameter first
    llvm::Type *ReturnLLVMType = nullptr;
    bool UseSret = false;
    if (Func.Returns) {
        ReturnLLVMType = mapType(*Func.Returns);
        // Use sret for non-primitive types (structs, unions)
        if (ReturnLLVMType->isStructTy() || ReturnLLVMType->isArrayTy()) {
            UseSret = true;
            ParamTypes.push_back(llvm::PointerType::get(*Context, 0));
        }
    }

    // If can throw, add exception page parameter
    if (Func.Throws) {
        ParamTypes.push_back(llvm::PointerType::get(*Context, 0));
    }

    // Add regular parameters (all passed by pointer for non-primitives)
    for (const auto &Item : Func.Input) {
        if (Item.ItemType) {
            llvm::Type *ItemTy = mapType(*Item.ItemType);
            if (ItemTy->isStructTy() || ItemTy->isArrayTy()) {
                // Pass struct/array by pointer (borrowed)
                ParamTypes.push_back(llvm::PointerType::get(*Context, 0));
            } else {
                // Pass primitives by value
                ParamTypes.push_back(ItemTy);
            }
        }
    }

    // Determine return type
    llvm::Type *LLVMReturnTy;
    if (UseSret) {
        // sret functions return void (result via first param)
        LLVMReturnTy = llvm::Type::getVoidTy(*Context);
    } else if (ReturnLLVMType) {
        LLVMReturnTy = ReturnLLVMType;
    } else {
        LLVMReturnTy = llvm::Type::getVoidTy(*Context);
    }

    // Create function type
    auto *FuncTy = llvm::FunctionType::get(LLVMReturnTy, ParamTypes, /*isVarArg=*/false);

    // Create function
    auto *LLVMFunc = llvm::Function::Create(
        FuncTy,
        llvm::GlobalValue::ExternalLinkage,
        Func.MangledName,
        *Module
    );

    // Set parameter attributes
    unsigned ParamIdx = 0;
    if (UseSret) {
        LLVMFunc->addParamAttr(ParamIdx, llvm::Attribute::StructRet);
        LLVMFunc->addParamAttr(ParamIdx, llvm::Attribute::NoAlias);
        ParamIdx++;
    }
    if (Func.Throws) {
        // Exception page parameter
        ParamIdx++;
    }
    for (const auto &Item : Func.Input) {
        if (Item.ItemType) {
            llvm::Type *ItemTy = mapType(*Item.ItemType);
            if (ItemTy->isStructTy() || ItemTy->isArrayTy()) {
                // Borrowed parameter: noalias, readonly (for functions)
                if (Func.Pure) {
                    LLVMFunc->addParamAttr(ParamIdx, llvm::Attribute::ReadOnly);
                }
            }
        }
        ParamIdx++;
    }

    // Set function attributes
    if (Func.Pure) {
        LLVMFunc->setDoesNotThrow();
        // Pure functions don't access global state (for functions without exceptions)
        if (!Func.Throws) {
            LLVMFunc->setOnlyAccessesArgMemory();
        }
    }

    FunctionCache[Func.MangledName] = LLVMFunc;
    return LLVMFunc;
}

// ============================================================================
// Function Body Emission
// ============================================================================

llvm::Error Emitter::emitFunctionBody(const PlannedFunction &Func,
                                       llvm::Function *LLVMFunc) {
    // Skip extern/intrinsic functions
    if (std::holds_alternative<PlannedExternImpl>(Func.Impl) ||
        std::holds_alternative<PlannedIntrinsicImpl>(Func.Impl)) {
        return llvm::Error::success();
    }

    // Create entry block
    auto *EntryBB = llvm::BasicBlock::Create(*Context, "entry", LLVMFunc);
    Builder->SetInsertPoint(EntryBB);

    // Set up current function state
    CurrentFunction = LLVMFunc;
    CurrentBlock = EntryBB;
    LocalVariables.clear();

    // Bind parameters to names
    unsigned ArgIdx = 0;
    auto ArgIt = LLVMFunc->arg_begin();

    // Skip sret if present
    if (Func.Returns) {
        llvm::Type *RetTy = mapType(*Func.Returns);
        if (RetTy->isStructTy() || RetTy->isArrayTy()) {
            CurrentRegion.ReturnPage = &*ArgIt;
            ArgIt++;
            ArgIdx++;
        }
    }

    // Skip exception page if present
    if (Func.Throws) {
        CurrentRegion.ExceptionPage = &*ArgIt;
        ArgIt++;
        ArgIdx++;
    }

    // Bind regular parameters
    for (const auto &Item : Func.Input) {
        if (Item.Name) {
            LocalVariables[*Item.Name] = &*ArgIt;
        }
        ArgIt++;
        ArgIdx++;
    }

    // Emit function body
    if (auto *Action = std::get_if<PlannedAction>(&Func.Impl)) {
        if (auto Err = emitAction(*Action)) {
            return Err;
        }
    }

    // Add return if block doesn't end with terminator
    if (!CurrentBlock->getTerminator()) {
        if (LLVMFunc->getReturnType()->isVoidTy()) {
            Builder->CreateRetVoid();
        } else {
            // Should not happen in well-formed code
            Builder->CreateUnreachable();
        }
    }

    return llvm::Error::success();
}

// ============================================================================
// Statement Emission
// ============================================================================

llvm::Error Emitter::emitStatement(const PlannedStatement &Stmt) {
    return std::visit([this](const auto &S) -> llvm::Error {
        using T = std::decay_t<decltype(S)>;
        if constexpr (std::is_same_v<T, PlannedAction>) {
            return emitAction(S);
        } else if constexpr (std::is_same_v<T, PlannedBinding>) {
            return emitBinding(S);
        } else if constexpr (std::is_same_v<T, PlannedReturn>) {
            return emitReturn(S);
        } else if constexpr (std::is_same_v<T, PlannedThrow>) {
            return emitThrow(S);
        } else if constexpr (std::is_same_v<T, PlannedBreak>) {
            return emitBreak(S);
        } else if constexpr (std::is_same_v<T, PlannedContinue>) {
            return emitContinue(S);
        } else {
            return llvm::make_error<llvm::StringError>(
                "Unknown statement type",
                llvm::inconvertibleErrorCode()
            );
        }
    }, Stmt);
}

llvm::Error Emitter::emitAction(const PlannedAction &Action) {
    // Emit source operands
    auto ValueOrErr = emitOperands(Action.Source);
    if (!ValueOrErr)
        return ValueOrErr.takeError();

    // If there are target operands, this is an assignment
    if (!Action.Target.empty()) {
        // TODO: implement assignment
    }

    return llvm::Error::success();
}

llvm::Error Emitter::emitBinding(const PlannedBinding &Binding) {
    // Emit initialization expression
    if (Binding.Operation.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Binding has no initialization expression",
            llvm::inconvertibleErrorCode()
        );
    }

    auto ValueOrErr = emitOperands(Binding.Operation);
    if (!ValueOrErr)
        return ValueOrErr.takeError();

    llvm::Value *Value = *ValueOrErr;
    if (!Value) {
        return llvm::make_error<llvm::StringError>(
            "Binding initialization expression produced null value",
            llvm::inconvertibleErrorCode()
        );
    }

    // Create alloca for the binding
    if (Binding.BindingItem.Name) {
        llvm::Type *Ty = Value->getType();
        if (Binding.BindingType == "var" || Binding.BindingType == "mutable") {
            // Mutable binding: allocate on stack
            auto *Alloca = Builder->CreateAlloca(Ty, nullptr, *Binding.BindingItem.Name);
            Builder->CreateStore(Value, Alloca);
            LocalVariables[*Binding.BindingItem.Name] = Alloca;
        } else {
            // Immutable binding: just use the value directly
            LocalVariables[*Binding.BindingItem.Name] = Value;
        }
    }

    return llvm::Error::success();
}

llvm::Error Emitter::emitReturn(const PlannedReturn &Return) {
    if (Return.Result.empty()) {
        Builder->CreateRetVoid();
    } else {
        auto ValueOrErr = emitOperands(Return.Result);
        if (!ValueOrErr)
            return ValueOrErr.takeError();
        Builder->CreateRet(*ValueOrErr);
    }
    return llvm::Error::success();
}

llvm::Error Emitter::emitThrow(const PlannedThrow &Throw) {
    // TODO: implement throw (store to exception page, return error indicator)
    return llvm::Error::success();
}

llvm::Error Emitter::emitBreak(const PlannedBreak &Break) {
    // TODO: implement break (needs loop context)
    return llvm::Error::success();
}

llvm::Error Emitter::emitContinue(const PlannedContinue &Continue) {
    // TODO: implement continue (needs loop context)
    return llvm::Error::success();
}

// ============================================================================
// Expression Emission
// ============================================================================

llvm::Expected<llvm::Value*> Emitter::emitOperands(const std::vector<PlannedOperand> &Ops) {
    if (Ops.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Empty operand sequence",
            llvm::inconvertibleErrorCode()
        );
    }

    // For now, just emit the first operand
    // TODO: handle postfix operator chains
    return emitOperand(Ops[0]);
}

llvm::Expected<llvm::Value*> Emitter::emitOperand(const PlannedOperand &Op) {
    // Handle constants specially since they need type info
    if (auto *Const = std::get_if<PlannedConstant>(&Op.Expr)) {
        return emitConstant(*Const, Op.ResultType);
    }
    return emitExpression(Op.Expr);
}

llvm::Expected<llvm::Value*> Emitter::emitExpression(const PlannedExpression &Expr) {
    return std::visit([this](const auto &E) -> llvm::Expected<llvm::Value*> {
        using T = std::decay_t<decltype(E)>;
        if constexpr (std::is_same_v<T, PlannedConstant>) {
            // Fallback: type info not available, use defaults
            return emitConstant(E, PlannedType{});
        } else if constexpr (std::is_same_v<T, PlannedType>) {
            // Type expression (e.g., for sizeof)
            return nullptr;  // TODO
        } else if constexpr (std::is_same_v<T, PlannedVariable>) {
            // Variable reference - look up and load
            llvm::Value *VarPtr = lookupVariable(E.Name);
            if (!VarPtr) {
                return llvm::make_error<llvm::StringError>(
                    "Undefined variable: " + E.Name,
                    llvm::inconvertibleErrorCode()
                );
            }
            if (E.IsMutable) {
                // Mutable binding stored as pointer - load the value
                llvm::Type *Ty = mapType(E.VariableType);
                return Builder->CreateLoad(Ty, VarPtr, E.Name);
            } else {
                // Immutable binding stored as value directly
                return VarPtr;
            }
        } else if constexpr (std::is_same_v<T, PlannedCall>) {
            return emitCall(E);
        } else if constexpr (std::is_same_v<T, PlannedTuple>) {
            // Handle grouped expressions (single anonymous component)
            // and actual tuples (multiple or named components)
            if (E.Components.size() == 1 && !E.Components[0].Name) {
                // Grouped expression like (2 + 3) - evaluate the inner value
                if (!E.Components[0].Value.empty()) {
                    // The tuple's TupleType already contains the collapsed result
                    // We need to emit the operations within the component
                    // For now, emit the last operand which should have the collapsed call
                    return emitOperand(E.Components[0].Value.back());
                }
                return nullptr;
            } else {
                // Actual tuple - TODO: emit struct construction
                return llvm::make_error<llvm::StringError>(
                    "Tuple construction not yet implemented",
                    llvm::inconvertibleErrorCode()
                );
            }
        } else if constexpr (std::is_same_v<T, PlannedBlock>) {
            return emitBlock(E);
        } else if constexpr (std::is_same_v<T, PlannedIf>) {
            return emitIf(E);
        } else if constexpr (std::is_same_v<T, PlannedChoose>) {
            return emitChoose(E);
        } else {
            return llvm::make_error<llvm::StringError>(
                "Unimplemented expression type",
                llvm::inconvertibleErrorCode()
            );
        }
    }, Expr);
}

llvm::Expected<llvm::Value*> Emitter::emitCall(const PlannedCall &Call) {
    // Emit arguments first
    std::vector<llvm::Value*> Args;
    if (Call.Args) {
        for (const auto &Arg : *Call.Args) {
            auto ArgVal = emitOperand(Arg);
            if (!ArgVal)
                return ArgVal.takeError();
            Args.push_back(*ArgVal);
        }
    }

    // Handle intrinsic operators
    if (Call.IsIntrinsic && Call.IsOperator) {
        if (Args.size() == 2) {
            // Binary operator
            return emitIntrinsicOp(Call.Name, Args[0], Args[1], Call.ResultType);
        } else if (Args.size() == 1) {
            // Unary operator
            return emitIntrinsicUnaryOp(Call.Name, Args[0], Call.ResultType);
        }
    }

    // Non-intrinsic call: look up function and call it
    auto *Func = lookupFunction(Call.MangledName);
    if (!Func) {
        return llvm::make_error<llvm::StringError>(
            "Function not found: " + Call.MangledName,
            llvm::inconvertibleErrorCode()
        );
    }

    return Builder->CreateCall(Func, Args);
}

llvm::Expected<llvm::Value*> Emitter::emitIntrinsicOp(
    llvm::StringRef OpName, llvm::Value *Left, llvm::Value *Right,
    const PlannedType &ResultType) {

    llvm::Type *Ty = mapType(ResultType);
    bool IsFloat = Ty->isFloatingPointTy();
    bool IsSigned = true;  // Default to signed for now

    // Check if dealing with unsigned types
    if (ResultType.Name.find('u') == 0) {  // u8, u16, u32, u64
        IsSigned = false;
    }

    // Arithmetic operators
    if (OpName == "+") {
        if (IsFloat)
            return Builder->CreateFAdd(Left, Right, "fadd");
        return Builder->CreateAdd(Left, Right, "add");
    }
    if (OpName == "-") {
        if (IsFloat)
            return Builder->CreateFSub(Left, Right, "fsub");
        return Builder->CreateSub(Left, Right, "sub");
    }
    if (OpName == "*") {
        if (IsFloat)
            return Builder->CreateFMul(Left, Right, "fmul");
        return Builder->CreateMul(Left, Right, "mul");
    }
    if (OpName == "/") {
        if (IsFloat)
            return Builder->CreateFDiv(Left, Right, "fdiv");
        if (IsSigned)
            return Builder->CreateSDiv(Left, Right, "sdiv");
        return Builder->CreateUDiv(Left, Right, "udiv");
    }
    if (OpName == "%") {
        if (IsFloat)
            return Builder->CreateFRem(Left, Right, "frem");
        if (IsSigned)
            return Builder->CreateSRem(Left, Right, "srem");
        return Builder->CreateURem(Left, Right, "urem");
    }

    // Comparison operators
    if (OpName == "=") {
        if (IsFloat)
            return Builder->CreateFCmpOEQ(Left, Right, "fcmp_eq");
        return Builder->CreateICmpEQ(Left, Right, "icmp_eq");
    }
    if (OpName == "<>") {
        if (IsFloat)
            return Builder->CreateFCmpONE(Left, Right, "fcmp_ne");
        return Builder->CreateICmpNE(Left, Right, "icmp_ne");
    }
    if (OpName == "<") {
        if (IsFloat)
            return Builder->CreateFCmpOLT(Left, Right, "fcmp_lt");
        if (IsSigned)
            return Builder->CreateICmpSLT(Left, Right, "icmp_slt");
        return Builder->CreateICmpULT(Left, Right, "icmp_ult");
    }
    if (OpName == ">") {
        if (IsFloat)
            return Builder->CreateFCmpOGT(Left, Right, "fcmp_gt");
        if (IsSigned)
            return Builder->CreateICmpSGT(Left, Right, "icmp_sgt");
        return Builder->CreateICmpUGT(Left, Right, "icmp_ugt");
    }
    if (OpName == "<=") {
        if (IsFloat)
            return Builder->CreateFCmpOLE(Left, Right, "fcmp_le");
        if (IsSigned)
            return Builder->CreateICmpSLE(Left, Right, "icmp_sle");
        return Builder->CreateICmpULE(Left, Right, "icmp_ule");
    }
    if (OpName == ">=") {
        if (IsFloat)
            return Builder->CreateFCmpOGE(Left, Right, "fcmp_ge");
        if (IsSigned)
            return Builder->CreateICmpSGE(Left, Right, "icmp_sge");
        return Builder->CreateICmpUGE(Left, Right, "icmp_uge");
    }

    // Bitwise operators
    if (OpName == "&") {
        return Builder->CreateAnd(Left, Right, "and");
    }
    if (OpName == "|") {
        return Builder->CreateOr(Left, Right, "or");
    }
    if (OpName == "^") {
        return Builder->CreateXor(Left, Right, "xor");
    }

    return llvm::make_error<llvm::StringError>(
        "Unknown intrinsic operator: " + OpName.str(),
        llvm::inconvertibleErrorCode()
    );
}

llvm::Expected<llvm::Value*> Emitter::emitIntrinsicUnaryOp(
    llvm::StringRef OpName, llvm::Value *Operand,
    const PlannedType &ResultType) {

    llvm::Type *Ty = mapType(ResultType);
    bool IsFloat = Ty->isFloatingPointTy();

    if (OpName == "-") {
        if (IsFloat)
            return Builder->CreateFNeg(Operand, "fneg");
        return Builder->CreateNeg(Operand, "neg");
    }
    if (OpName == "!") {
        return Builder->CreateNot(Operand, "not");
    }
    if (OpName == "~") {
        return Builder->CreateNot(Operand, "bitnot");
    }

    return llvm::make_error<llvm::StringError>(
        "Unknown intrinsic unary operator: " + OpName.str(),
        llvm::inconvertibleErrorCode()
    );
}

llvm::Value *Emitter::emitConstant(const PlannedConstant &Const, const PlannedType &Type) {
    return std::visit([this, &Type](const auto &C) -> llvm::Value* {
        using T = std::decay_t<decltype(C)>;
        if constexpr (std::is_same_v<T, IntegerConstant>) {
            // Default to i64 if type not specified
            llvm::Type *Ty = Type.isResolved() ? mapType(Type) : llvm::Type::getInt64Ty(*Context);
            return llvm::ConstantInt::get(Ty, C.Value, /*isSigned=*/true);
        } else if constexpr (std::is_same_v<T, FloatingPointConstant>) {
            llvm::Type *Ty = Type.isResolved() ? mapType(Type) : llvm::Type::getDoubleTy(*Context);
            return llvm::ConstantFP::get(Ty, C.Value);
        } else if constexpr (std::is_same_v<T, BooleanConstant>) {
            return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*Context), C.Value ? 1 : 0);
        } else if constexpr (std::is_same_v<T, StringConstant>) {
            return Builder->CreateGlobalStringPtr(C.Value);
        } else {
            return llvm::UndefValue::get(llvm::Type::getInt64Ty(*Context));
        }
    }, Const);
}

// ============================================================================
// Control Flow (Stubs)
// ============================================================================

llvm::Expected<llvm::Value*> Emitter::emitBlock(const PlannedBlock &Block) {
    llvm::Value *LastValue = nullptr;
    for (const auto &Stmt : Block.Statements) {
        if (auto Err = emitStatement(Stmt))
            return std::move(Err);
    }
    return LastValue;
}

llvm::Expected<llvm::Value*> Emitter::emitIf(const PlannedIf &If) {
    // TODO: implement if emission
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitChoose(const PlannedChoose &Choose) {
    // TODO: implement choose emission (switch on union tag)
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitMatch(const PlannedMatch &Match) {
    // TODO: implement match emission
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitFor(const PlannedFor &For) {
    // TODO: implement for emission
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitWhile(const PlannedWhile &While) {
    // TODO: implement while emission
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitTry(const PlannedTry &Try) {
    // TODO: implement try emission
    return nullptr;
}

// ============================================================================
// Utility Functions
// ============================================================================

size_t Emitter::getTypeSize(llvm::Type *Ty) {
    if (!Module)
        return 0;
    auto &DL = Module->getDataLayout();
    return DL.getTypeAllocSize(Ty);
}

size_t Emitter::getTypeAlignment(llvm::Type *Ty) {
    if (!Module)
        return 0;
    auto &DL = Module->getDataLayout();
    return DL.getABITypeAlign(Ty).value();
}

llvm::BasicBlock *Emitter::createBlock(llvm::StringRef Name) {
    return llvm::BasicBlock::Create(*Context, Name, CurrentFunction);
}

llvm::Value *Emitter::lookupVariable(llvm::StringRef Name) {
    auto It = LocalVariables.find(Name.str());
    if (It != LocalVariables.end())
        return It->second;
    return nullptr;
}

llvm::Function *Emitter::lookupFunction(llvm::StringRef MangledName) {
    auto It = FunctionCache.find(MangledName.str());
    if (It != FunctionCache.end())
        return It->second;
    return Module->getFunction(MangledName);
}

llvm::Value *Emitter::allocate(llvm::Type *Ty, Lifetime Life, llvm::StringRef Name) {
    // For now, all allocations go on the stack
    // TODO: implement region-based allocation
    return Builder->CreateAlloca(Ty, nullptr, Name);
}

llvm::Value *Emitter::getRegionPage(Lifetime Life) {
    if (std::holds_alternative<LocalLifetime>(Life)) {
        return CurrentRegion.LocalPage;
    } else if (std::holds_alternative<CallLifetime>(Life)) {
        return CurrentRegion.ReturnPage;
    } else if (std::holds_alternative<ThrownLifetime>(Life)) {
        return CurrentRegion.ExceptionPage;
    } else if (std::holds_alternative<ReferenceLifetime>(Life)) {
        // TODO: look up referenced container's region
        return nullptr;
    }
    // UnspecifiedLifetime or unknown
    return nullptr;
}

// ============================================================================
// JIT Execution
// ============================================================================

llvm::Function *Emitter::createJITWrapper(llvm::Type *ReturnType) {
    // Create a wrapper function: i64 __scaly_jit_main()
    auto *WrapperTy = llvm::FunctionType::get(ReturnType, {}, false);
    auto *Wrapper = llvm::Function::Create(
        WrapperTy,
        llvm::GlobalValue::ExternalLinkage,
        "__scaly_jit_main",
        *Module
    );

    // Create entry block
    auto *Entry = llvm::BasicBlock::Create(*Context, "entry", Wrapper);
    Builder->SetInsertPoint(Entry);

    return Wrapper;
}

llvm::Expected<uint64_t> Emitter::jitExecuteRaw(const Plan &P, llvm::Type *ExpectedType) {
    // Create fresh module for JIT
    Module = std::make_unique<llvm::Module>("jit_module", *Context);
    Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    // Clear caches
    TypeCache.clear();
    StructCache.clear();
    FunctionCache.clear();

    // Emit type declarations
    for (const auto &[name, structPtr] : P.Structures) {
        if (auto *Struct = structPtr) {
            emitStructType(*Struct);
        }
    }
    for (const auto &[name, unionPtr] : P.Unions) {
        if (auto *Union = unionPtr) {
            emitUnionType(*Union);
        }
    }

    // Emit function declarations
    for (const auto &[name, funcPtr] : P.Functions) {
        if (auto *Func = funcPtr) {
            emitFunctionDecl(*Func);
        }
    }

    // Emit function bodies
    for (const auto &[name, funcPtr] : P.Functions) {
        if (auto *Func = funcPtr) {
            if (auto *LLVMFunc = FunctionCache[Func->MangledName]) {
                if (auto Err = emitFunctionBody(*Func, LLVMFunc))
                    return std::move(Err);
            }
        }
    }

    // Create JIT wrapper function
    auto *Wrapper = createJITWrapper(ExpectedType);
    CurrentFunction = Wrapper;
    LocalVariables.clear();

    // Emit top-level statements into wrapper
    llvm::Value *LastValue = nullptr;
    for (const auto &Stmt : P.Statements) {
        if (auto *Action = std::get_if<PlannedAction>(&Stmt)) {
            auto ValueOrErr = emitOperands(Action->Source);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            LastValue = *ValueOrErr;
        } else if (auto *Binding = std::get_if<PlannedBinding>(&Stmt)) {
            if (auto Err = emitBinding(*Binding))
                return std::move(Err);
        } else if (auto *Return = std::get_if<PlannedReturn>(&Stmt)) {
            auto ValueOrErr = emitOperands(Return->Result);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            LastValue = *ValueOrErr;
        }
    }

    // Return the last value (or default)
    if (LastValue) {
        // Cast to expected type if needed
        if (LastValue->getType() != ExpectedType) {
            if (ExpectedType->isIntegerTy() && LastValue->getType()->isIntegerTy()) {
                LastValue = Builder->CreateIntCast(LastValue, ExpectedType, true);
            } else if (ExpectedType->isDoubleTy() && LastValue->getType()->isFloatTy()) {
                LastValue = Builder->CreateFPExt(LastValue, ExpectedType);
            } else if (ExpectedType->isFloatTy() && LastValue->getType()->isDoubleTy()) {
                LastValue = Builder->CreateFPTrunc(LastValue, ExpectedType);
            }
        }
        Builder->CreateRet(LastValue);
    } else {
        // Return default value
        if (ExpectedType->isIntegerTy()) {
            Builder->CreateRet(llvm::ConstantInt::get(ExpectedType, 0));
        } else if (ExpectedType->isFloatingPointTy()) {
            Builder->CreateRet(llvm::ConstantFP::get(ExpectedType, 0.0));
        } else {
            Builder->CreateRet(llvm::UndefValue::get(ExpectedType));
        }
    }

    // Verify module
    std::string VerifyError;
    llvm::raw_string_ostream VerifyStream(VerifyError);
    if (llvm::verifyModule(*Module, &VerifyStream)) {
        return llvm::make_error<llvm::StringError>(
            "JIT module verification failed: " + VerifyError,
            llvm::inconvertibleErrorCode()
        );
    }

    // Create LLJIT instance
    auto JITOrErr = llvm::orc::LLJITBuilder().create();
    if (!JITOrErr)
        return JITOrErr.takeError();

    auto &JIT = *JITOrErr;

    // Add module to JIT
    auto TSM = llvm::orc::ThreadSafeModule(std::move(Module), std::move(Context));
    if (auto Err = JIT->addIRModule(std::move(TSM)))
        return std::move(Err);

    // Look up the wrapper function
    auto SymOrErr = JIT->lookup("__scaly_jit_main");
    if (!SymOrErr)
        return SymOrErr.takeError();

    // Get function pointer and call it
    auto *FnPtr = SymOrErr->toPtr<uint64_t(*)()>();
    uint64_t Result = FnPtr();

    // Recreate context for future use (since we moved it to JIT)
    Context = std::make_unique<llvm::LLVMContext>();
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
    initIntrinsicTypes();

    return Result;
}

llvm::Expected<int64_t> Emitter::jitExecuteInt(const Plan &P) {
    auto ResultOrErr = jitExecuteRaw(P, llvm::Type::getInt64Ty(*Context));
    if (!ResultOrErr)
        return ResultOrErr.takeError();
    return static_cast<int64_t>(*ResultOrErr);
}

llvm::Expected<double> Emitter::jitExecuteFloat(const Plan &P) {
    // For floats, we need to use a different approach since we're returning bits
    auto ResultOrErr = jitExecuteRaw(P, llvm::Type::getDoubleTy(*Context));
    if (!ResultOrErr)
        return ResultOrErr.takeError();

    // Reinterpret bits as double
    uint64_t Bits = *ResultOrErr;
    double Result;
    std::memcpy(&Result, &Bits, sizeof(double));
    return Result;
}

llvm::Expected<bool> Emitter::jitExecuteBool(const Plan &P) {
    auto ResultOrErr = jitExecuteRaw(P, llvm::Type::getInt1Ty(*Context));
    if (!ResultOrErr)
        return ResultOrErr.takeError();
    return (*ResultOrErr) != 0;
}

} // namespace scaly
