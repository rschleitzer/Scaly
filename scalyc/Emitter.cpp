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
        auto ValueOrErr = emitAction(*Action);
        if (!ValueOrErr)
            return ValueOrErr.takeError();
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
            auto ValueOrErr = emitAction(S);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            return llvm::Error::success();
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

llvm::Expected<llvm::Value*> Emitter::emitAction(const PlannedAction &Action) {
    // Emit source operands
    auto ValueOrErr = emitOperands(Action.Source);
    if (!ValueOrErr)
        return ValueOrErr.takeError();

    llvm::Value *Value = *ValueOrErr;

    // If there are target operands, this is an assignment
    if (!Action.Target.empty()) {
        // Get the target - should be a variable reference for simple assignment
        const PlannedOperand &TargetOp = Action.Target[0];

        if (auto *Var = std::get_if<PlannedVariable>(&TargetOp.Expr)) {
            // Look up the variable's alloca pointer
            llvm::Value *VarPtr = lookupVariable(Var->Name);
            if (!VarPtr) {
                return llvm::make_error<llvm::StringError>(
                    "Undefined variable in assignment: " + Var->Name,
                    llvm::inconvertibleErrorCode()
                );
            }

            if (!Var->IsMutable) {
                return llvm::make_error<llvm::StringError>(
                    "Cannot assign to immutable variable: " + Var->Name,
                    llvm::inconvertibleErrorCode()
                );
            }

            // Store the value to the variable
            Builder->CreateStore(Value, VarPtr);
        } else {
            return llvm::make_error<llvm::StringError>(
                "Assignment target must be a variable",
                llvm::inconvertibleErrorCode()
            );
        }
    }

    return Value;
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
            return emitTuple(E);
        } else if constexpr (std::is_same_v<T, PlannedMatrix>) {
            return emitMatrix(E);
        } else if constexpr (std::is_same_v<T, PlannedBlock>) {
            return emitBlock(E);
        } else if constexpr (std::is_same_v<T, PlannedIf>) {
            return emitIf(E);
        } else if constexpr (std::is_same_v<T, PlannedChoose>) {
            return emitChoose(E);
        } else if constexpr (std::is_same_v<T, PlannedMatch>) {
            return emitMatch(E);
        } else if constexpr (std::is_same_v<T, PlannedWhile>) {
            return emitWhile(E);
        } else if constexpr (std::is_same_v<T, PlannedFor>) {
            return emitFor(E);
        } else if constexpr (std::is_same_v<T, PlannedTry>) {
            return emitTry(E);
        } else if constexpr (std::is_same_v<T, PlannedSizeOf>) {
            return emitSizeOf(E);
        } else if constexpr (std::is_same_v<T, PlannedIs>) {
            return emitIs(E);
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
    // Emit the condition
    if (If.Condition.empty()) {
        return llvm::make_error<llvm::StringError>(
            "If statement has no condition",
            llvm::inconvertibleErrorCode()
        );
    }

    auto CondValueOrErr = emitOperands(If.Condition);
    if (!CondValueOrErr)
        return CondValueOrErr.takeError();

    llvm::Value *CondValue = *CondValueOrErr;

    // Ensure we have a boolean (i1) for the branch
    if (!CondValue->getType()->isIntegerTy(1)) {
        if (CondValue->getType()->isIntegerTy()) {
            CondValue = Builder->CreateICmpNE(
                CondValue,
                llvm::ConstantInt::get(CondValue->getType(), 0),
                "if.tobool"
            );
        } else {
            return llvm::make_error<llvm::StringError>(
                "If condition must be boolean or integer",
                llvm::inconvertibleErrorCode()
            );
        }
    }

    // Create basic blocks
    llvm::BasicBlock *ThenBlock = createBlock("if.then");
    llvm::BasicBlock *ElseBlock = If.Alternative ? createBlock("if.else") : nullptr;
    llvm::BasicBlock *MergeBlock = createBlock("if.end");

    // Branch based on condition
    if (ElseBlock) {
        Builder->CreateCondBr(CondValue, ThenBlock, ElseBlock);
    } else {
        Builder->CreateCondBr(CondValue, ThenBlock, MergeBlock);
    }

    // Emit then block
    Builder->SetInsertPoint(ThenBlock);
    llvm::Value *ThenValue = nullptr;

    if (If.Consequent) {
        // Emit the consequent statement
        if (auto *Action = std::get_if<PlannedAction>(If.Consequent.get())) {
            // Emit the action and get its value
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            // For non-assignments, use the value
            if (Action->Target.empty()) {
                ThenValue = *ValueOrErr;
            }
        } else if (auto *Binding = std::get_if<PlannedBinding>(If.Consequent.get())) {
            if (auto Err = emitBinding(*Binding))
                return std::move(Err);
        }
        // Other statement types don't produce values
    }

    // Jump to merge block (if not already terminated)
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(MergeBlock);
    }
    llvm::BasicBlock *ThenEndBlock = Builder->GetInsertBlock();

    // Emit else block if present
    llvm::Value *ElseValue = nullptr;
    llvm::BasicBlock *ElseEndBlock = nullptr;

    if (ElseBlock) {
        Builder->SetInsertPoint(ElseBlock);

        if (If.Alternative) {
            if (auto *Action = std::get_if<PlannedAction>(If.Alternative.get())) {
                // Emit the action and get its value
                auto ValueOrErr = emitAction(*Action);
                if (!ValueOrErr)
                    return ValueOrErr.takeError();
                // For non-assignments, use the value
                if (Action->Target.empty()) {
                    ElseValue = *ValueOrErr;
                }
            } else if (auto *Binding = std::get_if<PlannedBinding>(If.Alternative.get())) {
                if (auto Err = emitBinding(*Binding))
                    return std::move(Err);
            }
        }

        if (!Builder->GetInsertBlock()->getTerminator()) {
            Builder->CreateBr(MergeBlock);
        }
        ElseEndBlock = Builder->GetInsertBlock();
    }

    // Continue at merge block
    Builder->SetInsertPoint(MergeBlock);

    // If both branches produce values of the same type, create a PHI node
    if (ThenValue && ElseValue && ThenValue->getType() == ElseValue->getType()) {
        llvm::PHINode *PHI = Builder->CreatePHI(ThenValue->getType(), 2, "if.value");
        PHI->addIncoming(ThenValue, ThenEndBlock);
        PHI->addIncoming(ElseValue, ElseEndBlock);
        return PHI;
    }

    // Return the then value if only then branch has a value (no else)
    if (ThenValue && !If.Alternative) {
        return ThenValue;
    }

    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitChoose(const PlannedChoose &Choose) {
    // Emit the condition (union value)
    if (Choose.Condition.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Choose expression has no condition",
            llvm::inconvertibleErrorCode()
        );
    }

    auto CondValueOrErr = emitOperands(Choose.Condition);
    if (!CondValueOrErr)
        return CondValueOrErr.takeError();

    llvm::Value *UnionValue = *CondValueOrErr;

    // Union layout is { i8 tag, [maxSize x i8] data }
    // We need to extract the tag to switch on it

    // Get pointer to the union (may already be a pointer or need alloca)
    llvm::Value *UnionPtr;
    if (UnionValue->getType()->isPointerTy()) {
        UnionPtr = UnionValue;
    } else {
        // Store the union value to get a pointer
        UnionPtr = Builder->CreateAlloca(UnionValue->getType(), nullptr, "choose.union");
        Builder->CreateStore(UnionValue, UnionPtr);
    }

    // Load the tag (first field, i8)
    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
    llvm::Value *TagPtr = Builder->CreateStructGEP(UnionValue->getType(), UnionPtr, 0, "tag.ptr");
    llvm::Value *Tag = Builder->CreateLoad(I8Ty, TagPtr, "tag");

    // Create blocks
    llvm::BasicBlock *MergeBlock = createBlock("choose.end");
    llvm::BasicBlock *DefaultBlock = Choose.Alternative ? createBlock("choose.else") : MergeBlock;

    // Create switch instruction
    llvm::SwitchInst *Switch = Builder->CreateSwitch(Tag, DefaultBlock, Choose.Cases.size());

    // Track values and source blocks for PHI node
    std::vector<std::pair<llvm::Value*, llvm::BasicBlock*>> IncomingValues;

    // Process each when case
    for (size_t i = 0; i < Choose.Cases.size(); ++i) {
        const auto &When = Choose.Cases[i];

        llvm::BasicBlock *CaseBlock = createBlock("choose.when." + When.Name);
        Switch->addCase(llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(I8Ty, i)), CaseBlock);

        Builder->SetInsertPoint(CaseBlock);

        // If the variant has data and a binding name, extract and bind it
        // TODO: For now we don't bind the variant data, just execute the consequent

        llvm::Value *CaseValue = nullptr;

        if (When.Consequent) {
            if (auto *Action = std::get_if<PlannedAction>(When.Consequent.get())) {
                auto ValueOrErr = emitAction(*Action);
                if (!ValueOrErr)
                    return ValueOrErr.takeError();
                if (Action->Target.empty()) {
                    CaseValue = *ValueOrErr;
                }
            } else if (auto *Binding = std::get_if<PlannedBinding>(When.Consequent.get())) {
                if (auto Err = emitBinding(*Binding))
                    return std::move(Err);
            }
        }

        if (!Builder->GetInsertBlock()->getTerminator()) {
            Builder->CreateBr(MergeBlock);
        }

        if (CaseValue) {
            IncomingValues.push_back({CaseValue, Builder->GetInsertBlock()});
        }
    }

    // Emit the alternative (else) block if present
    if (Choose.Alternative) {
        Builder->SetInsertPoint(DefaultBlock);
        llvm::Value *ElseValue = nullptr;

        if (auto *Action = std::get_if<PlannedAction>(Choose.Alternative.get())) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            if (Action->Target.empty()) {
                ElseValue = *ValueOrErr;
            }
        } else if (auto *Binding = std::get_if<PlannedBinding>(Choose.Alternative.get())) {
            if (auto Err = emitBinding(*Binding))
                return std::move(Err);
        }

        if (!Builder->GetInsertBlock()->getTerminator()) {
            Builder->CreateBr(MergeBlock);
        }

        if (ElseValue) {
            IncomingValues.push_back({ElseValue, Builder->GetInsertBlock()});
        }
    }

    // Continue at merge block
    Builder->SetInsertPoint(MergeBlock);

    // Create PHI node if branches produce values
    if (!IncomingValues.empty()) {
        llvm::Type *ValueType = IncomingValues[0].first->getType();
        bool AllSameType = true;
        for (const auto &[Val, Block] : IncomingValues) {
            if (Val->getType() != ValueType) {
                AllSameType = false;
                break;
            }
        }

        if (AllSameType) {
            llvm::PHINode *PHI = Builder->CreatePHI(ValueType, IncomingValues.size(), "choose.value");
            for (const auto &[Val, Block] : IncomingValues) {
                PHI->addIncoming(Val, Block);
            }
            return PHI;
        }
    }

    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitMatch(const PlannedMatch &Match) {
    // Emit the condition being matched
    if (Match.Condition.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Match expression has no condition",
            llvm::inconvertibleErrorCode()
        );
    }

    auto CondValueOrErr = emitOperands(Match.Condition);
    if (!CondValueOrErr)
        return CondValueOrErr.takeError();

    llvm::Value *CondValue = *CondValueOrErr;

    // Create the merge block where all branches converge
    llvm::BasicBlock *MergeBlock = createBlock("match.end");

    // Track values and their source blocks for PHI node
    std::vector<std::pair<llvm::Value*, llvm::BasicBlock*>> IncomingValues;

    // Process each branch
    for (size_t i = 0; i < Match.Branches.size(); ++i) {
        const auto &Branch = Match.Branches[i];

        // Create blocks for this branch
        llvm::BasicBlock *BranchBlock = createBlock("match.case");
        llvm::BasicBlock *NextBlock = (i + 1 < Match.Branches.size() || Match.Alternative)
            ? createBlock("match.next")
            : MergeBlock;

        // Build the condition: check if CondValue matches any case in this branch
        llvm::Value *BranchCond = nullptr;
        for (const auto &Case : Branch.Cases) {
            auto CaseValueOrErr = emitOperands(Case.Condition);
            if (!CaseValueOrErr)
                return CaseValueOrErr.takeError();

            llvm::Value *CaseValue = *CaseValueOrErr;

            // Compare condition with this case
            llvm::Value *CaseCond;
            if (CondValue->getType()->isIntegerTy()) {
                CaseCond = Builder->CreateICmpEQ(CondValue, CaseValue, "match.cmp");
            } else if (CondValue->getType()->isFloatingPointTy()) {
                CaseCond = Builder->CreateFCmpOEQ(CondValue, CaseValue, "match.cmp");
            } else {
                return llvm::make_error<llvm::StringError>(
                    "Match expression requires integer or float type",
                    llvm::inconvertibleErrorCode()
                );
            }

            // OR together multiple cases in the same branch
            if (BranchCond) {
                BranchCond = Builder->CreateOr(BranchCond, CaseCond, "match.or");
            } else {
                BranchCond = CaseCond;
            }
        }

        // Branch to case block or next check
        if (BranchCond) {
            Builder->CreateCondBr(BranchCond, BranchBlock, NextBlock);
        } else {
            // No cases - unconditional branch (shouldn't happen)
            Builder->CreateBr(BranchBlock);
        }

        // Emit the branch body
        Builder->SetInsertPoint(BranchBlock);
        llvm::Value *BranchValue = nullptr;

        if (Branch.Consequent) {
            if (auto *Action = std::get_if<PlannedAction>(Branch.Consequent.get())) {
                auto ValueOrErr = emitAction(*Action);
                if (!ValueOrErr)
                    return ValueOrErr.takeError();
                if (Action->Target.empty()) {
                    BranchValue = *ValueOrErr;
                }
            } else if (auto *Binding = std::get_if<PlannedBinding>(Branch.Consequent.get())) {
                if (auto Err = emitBinding(*Binding))
                    return std::move(Err);
            }
        }

        // Jump to merge block
        if (!Builder->GetInsertBlock()->getTerminator()) {
            Builder->CreateBr(MergeBlock);
        }

        if (BranchValue) {
            IncomingValues.push_back({BranchValue, Builder->GetInsertBlock()});
        }

        // Continue at next block for subsequent checks
        Builder->SetInsertPoint(NextBlock);
    }

    // Emit the alternative (else) block if present
    if (Match.Alternative) {
        llvm::Value *ElseValue = nullptr;

        if (auto *Action = std::get_if<PlannedAction>(Match.Alternative.get())) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            if (Action->Target.empty()) {
                ElseValue = *ValueOrErr;
            }
        } else if (auto *Binding = std::get_if<PlannedBinding>(Match.Alternative.get())) {
            if (auto Err = emitBinding(*Binding))
                return std::move(Err);
        }

        if (!Builder->GetInsertBlock()->getTerminator()) {
            Builder->CreateBr(MergeBlock);
        }

        if (ElseValue) {
            IncomingValues.push_back({ElseValue, Builder->GetInsertBlock()});
        }
    }

    // Continue at merge block
    Builder->SetInsertPoint(MergeBlock);

    // Create PHI node if branches produce values
    if (!IncomingValues.empty()) {
        // Check that all values have the same type
        llvm::Type *ValueType = IncomingValues[0].first->getType();
        bool AllSameType = true;
        for (const auto &[Val, Block] : IncomingValues) {
            if (Val->getType() != ValueType) {
                AllSameType = false;
                break;
            }
        }

        if (AllSameType) {
            llvm::PHINode *PHI = Builder->CreatePHI(ValueType, IncomingValues.size(), "match.value");
            for (const auto &[Val, Block] : IncomingValues) {
                PHI->addIncoming(Val, Block);
            }
            return PHI;
        }
    }

    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitFor(const PlannedFor &For) {
    // For now, implement a simple integer range loop: for i in N : body
    // This iterates i from 0 to N-1

    if (For.Expr.empty()) {
        return llvm::make_error<llvm::StringError>(
            "For loop has no range expression",
            llvm::inconvertibleErrorCode()
        );
    }

    // Evaluate the range expression to get the end value
    auto EndValueOrErr = emitOperands(For.Expr);
    if (!EndValueOrErr)
        return EndValueOrErr.takeError();

    llvm::Value *EndValue = *EndValueOrErr;
    if (!EndValue->getType()->isIntegerTy()) {
        return llvm::make_error<llvm::StringError>(
            "For loop range must be an integer",
            llvm::inconvertibleErrorCode()
        );
    }

    // Create the loop counter (allocate on stack for incrementing)
    llvm::Type *IndexType = EndValue->getType();
    llvm::AllocaInst *LoopCounter = Builder->CreateAlloca(IndexType, nullptr, For.Identifier + ".counter");

    // Initialize loop counter to 0
    Builder->CreateStore(llvm::ConstantInt::get(IndexType, 0), LoopCounter);

    // Create basic blocks
    llvm::BasicBlock *CondBlock = createBlock("for.cond");
    llvm::BasicBlock *BodyBlock = createBlock("for.body");
    llvm::BasicBlock *IncrBlock = createBlock("for.incr");
    llvm::BasicBlock *ExitBlock = createBlock("for.exit");

    // Jump to condition block
    Builder->CreateBr(CondBlock);

    // Emit condition block: i < end
    Builder->SetInsertPoint(CondBlock);
    llvm::Value *CurrentIndex = Builder->CreateLoad(IndexType, LoopCounter, "i");
    llvm::Value *Cond = Builder->CreateICmpSLT(CurrentIndex, EndValue, "for.cmp");
    Builder->CreateCondBr(Cond, BodyBlock, ExitBlock);

    // Emit body block
    Builder->SetInsertPoint(BodyBlock);

    // Load current index and store as immutable value for body to access
    llvm::Value *BodyIndex = Builder->CreateLoad(IndexType, LoopCounter, For.Identifier);
    LocalVariables[For.Identifier] = BodyIndex;

    auto BodyValueOrErr = emitAction(For.Body);
    if (!BodyValueOrErr)
        return BodyValueOrErr.takeError();

    // Jump to increment block
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(IncrBlock);
    }

    // Emit increment block: i++
    Builder->SetInsertPoint(IncrBlock);
    llvm::Value *IncrIndex = Builder->CreateLoad(IndexType, LoopCounter, "i.load");
    llvm::Value *NextIndex = Builder->CreateAdd(IncrIndex, llvm::ConstantInt::get(IndexType, 1), "i.next");
    Builder->CreateStore(NextIndex, LoopCounter);
    Builder->CreateBr(CondBlock);

    // Continue at exit block
    Builder->SetInsertPoint(ExitBlock);

    // Clean up the loop variable from LocalVariables
    LocalVariables.erase(For.Identifier);

    // For loops don't produce a value
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitWhile(const PlannedWhile &While) {
    // Create basic blocks for the loop structure
    llvm::BasicBlock *CondBlock = createBlock("while.cond");
    llvm::BasicBlock *BodyBlock = createBlock("while.body");
    llvm::BasicBlock *ExitBlock = createBlock("while.exit");

    // Jump to condition block
    Builder->CreateBr(CondBlock);

    // Emit condition block
    Builder->SetInsertPoint(CondBlock);

    // Emit the condition binding
    // The condition's Operation contains the boolean expression
    if (While.Cond.Operation.empty()) {
        return llvm::make_error<llvm::StringError>(
            "While condition has no expression",
            llvm::inconvertibleErrorCode()
        );
    }

    auto CondValueOrErr = emitOperands(While.Cond.Operation);
    if (!CondValueOrErr)
        return CondValueOrErr.takeError();

    llvm::Value *CondValue = *CondValueOrErr;

    // Ensure we have a boolean (i1) for the branch
    if (!CondValue->getType()->isIntegerTy(1)) {
        // Convert to boolean by comparing with zero
        if (CondValue->getType()->isIntegerTy()) {
            CondValue = Builder->CreateICmpNE(
                CondValue,
                llvm::ConstantInt::get(CondValue->getType(), 0),
                "while.tobool"
            );
        } else {
            return llvm::make_error<llvm::StringError>(
                "While condition must be boolean or integer",
                llvm::inconvertibleErrorCode()
            );
        }
    }

    // Branch: if condition is true, go to body; otherwise exit
    Builder->CreateCondBr(CondValue, BodyBlock, ExitBlock);

    // Emit body block
    Builder->SetInsertPoint(BodyBlock);

    // Emit the loop body action
    auto BodyValueOrErr = emitAction(While.Body);
    if (!BodyValueOrErr)
        return BodyValueOrErr.takeError();

    // Jump back to condition (loop back)
    Builder->CreateBr(CondBlock);

    // Continue at exit block
    Builder->SetInsertPoint(ExitBlock);

    // While loops don't produce a value
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitTry(const PlannedTry &Try) {
    // Try expressions work with Result-like types (unions with Ok/Err variants)
    // 1. Evaluate the binding's operation
    // 2. Check if result is Ok (tag 0) or Err (tag 1+)
    // 3. If Ok, bind the value and return it
    // 4. If Err, match against catch clauses

    // First, emit the binding's operation
    if (Try.Cond.Operation.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Try binding has no operation",
            llvm::inconvertibleErrorCode()
        );
    }

    auto CondValueOrErr = emitOperands(Try.Cond.Operation);
    if (!CondValueOrErr)
        return CondValueOrErr.takeError();

    llvm::Value *ResultValue = *CondValueOrErr;

    // Get binding name if present
    std::string BindingName;
    if (Try.Cond.BindingItem.Name) {
        BindingName = *Try.Cond.BindingItem.Name;
    }
    bool IsMutable = (Try.Cond.BindingType == "var");

    // If result is not a struct (union), just return it (no error possible)
    if (!ResultValue->getType()->isStructTy() && !ResultValue->getType()->isPointerTy()) {
        // Simple value, not a Result type - just bind and return
        if (!BindingName.empty()) {
            LocalVariables[BindingName] = ResultValue;
        }
        return ResultValue;
    }

    // Get pointer to the result value
    llvm::Value *ResultPtr;
    llvm::Type *ResultType = ResultValue->getType();
    if (ResultValue->getType()->isPointerTy()) {
        ResultPtr = ResultValue;
        // Need to determine the actual struct type - for now assume it's stored
    } else {
        // Store the result to get a pointer
        ResultPtr = Builder->CreateAlloca(ResultType, nullptr, "try.result");
        Builder->CreateStore(ResultValue, ResultPtr);
    }

    // Load the tag (first field, i8) - Result layout: { i8 tag, [size x i8] data }
    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
    llvm::Value *TagPtr = Builder->CreateStructGEP(ResultType, ResultPtr, 0, "tag.ptr");
    llvm::Value *Tag = Builder->CreateLoad(I8Ty, TagPtr, "tag");

    // Create blocks
    llvm::BasicBlock *OkBlock = createBlock("try.ok");
    llvm::BasicBlock *ErrBlock = (Try.Catches.empty() && !Try.Alternative)
        ? nullptr : createBlock("try.err");
    llvm::BasicBlock *MergeBlock = createBlock("try.end");

    // Check if tag is 0 (Ok)
    llvm::Value *IsOk = Builder->CreateICmpEQ(Tag, llvm::ConstantInt::get(I8Ty, 0), "is.ok");
    Builder->CreateCondBr(IsOk, OkBlock, ErrBlock ? ErrBlock : MergeBlock);

    // Track values and source blocks for PHI node
    std::vector<std::pair<llvm::Value*, llvm::BasicBlock*>> IncomingValues;

    // Emit Ok block - bind the success value and continue
    Builder->SetInsertPoint(OkBlock);

    // For now, the Ok value is the whole result (since we can't easily extract
    // the data portion without knowing the exact type)
    // In a full implementation, we'd extract the data from offset 1
    llvm::Value *OkValue = ResultValue;

    // Bind the value if there's a name
    if (!BindingName.empty()) {
        if (IsMutable) {
            llvm::Type *Ty = OkValue->getType();
            llvm::AllocaInst *Alloca = Builder->CreateAlloca(Ty, nullptr, BindingName);
            Builder->CreateStore(OkValue, Alloca);
            LocalVariables[BindingName] = Alloca;
        } else {
            LocalVariables[BindingName] = OkValue;
        }
    }

    IncomingValues.push_back({OkValue, OkBlock});
    Builder->CreateBr(MergeBlock);

    // Emit error handling blocks
    if (ErrBlock) {
        Builder->SetInsertPoint(ErrBlock);

        if (!Try.Catches.empty()) {
            // Create switch for different error types
            llvm::BasicBlock *DefaultBlock = Try.Alternative
                ? createBlock("try.else") : MergeBlock;

            llvm::SwitchInst *Switch = Builder->CreateSwitch(Tag, DefaultBlock, Try.Catches.size());

            // Process each catch clause
            for (size_t i = 0; i < Try.Catches.size(); ++i) {
                const auto &Catch = Try.Catches[i];

                llvm::BasicBlock *CatchBlock = createBlock("try.catch." + Catch.Name);
                // Error tags start at 1 (0 is Ok)
                Switch->addCase(
                    llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(I8Ty, i + 1)),
                    CatchBlock
                );

                Builder->SetInsertPoint(CatchBlock);

                // Bind the error value if there's a name
                // TODO: Extract actual error data from the union

                llvm::Value *CatchValue = nullptr;

                if (Catch.Consequent) {
                    if (auto *Action = std::get_if<PlannedAction>(Catch.Consequent.get())) {
                        auto ValueOrErr = emitAction(*Action);
                        if (!ValueOrErr)
                            return ValueOrErr.takeError();
                        if (Action->Target.empty()) {
                            CatchValue = *ValueOrErr;
                        }
                    } else if (auto *Binding = std::get_if<PlannedBinding>(Catch.Consequent.get())) {
                        if (auto Err = emitBinding(*Binding))
                            return std::move(Err);
                    }
                }

                if (!Builder->GetInsertBlock()->getTerminator()) {
                    Builder->CreateBr(MergeBlock);
                }

                if (CatchValue) {
                    IncomingValues.push_back({CatchValue, Builder->GetInsertBlock()});
                }
            }

            // Emit else block if present
            if (Try.Alternative) {
                Builder->SetInsertPoint(DefaultBlock);
                llvm::Value *ElseValue = nullptr;

                if (auto *Action = std::get_if<PlannedAction>(Try.Alternative.get())) {
                    auto ValueOrErr = emitAction(*Action);
                    if (!ValueOrErr)
                        return ValueOrErr.takeError();
                    if (Action->Target.empty()) {
                        ElseValue = *ValueOrErr;
                    }
                } else if (auto *Binding = std::get_if<PlannedBinding>(Try.Alternative.get())) {
                    if (auto Err = emitBinding(*Binding))
                        return std::move(Err);
                }

                if (!Builder->GetInsertBlock()->getTerminator()) {
                    Builder->CreateBr(MergeBlock);
                }

                if (ElseValue) {
                    IncomingValues.push_back({ElseValue, Builder->GetInsertBlock()});
                }
            }
        } else if (Try.Alternative) {
            // No catches, just else
            llvm::Value *ElseValue = nullptr;

            if (auto *Action = std::get_if<PlannedAction>(Try.Alternative.get())) {
                auto ValueOrErr = emitAction(*Action);
                if (!ValueOrErr)
                    return ValueOrErr.takeError();
                if (Action->Target.empty()) {
                    ElseValue = *ValueOrErr;
                }
            } else if (auto *Binding = std::get_if<PlannedBinding>(Try.Alternative.get())) {
                if (auto Err = emitBinding(*Binding))
                    return std::move(Err);
            }

            if (!Builder->GetInsertBlock()->getTerminator()) {
                Builder->CreateBr(MergeBlock);
            }

            if (ElseValue) {
                IncomingValues.push_back({ElseValue, Builder->GetInsertBlock()});
            }
        }
    }

    // Continue at merge block
    Builder->SetInsertPoint(MergeBlock);

    // Clean up binding from scope
    if (!BindingName.empty()) {
        LocalVariables.erase(BindingName);
    }

    // Create PHI node if branches produce values
    if (!IncomingValues.empty()) {
        llvm::Type *ValueType = IncomingValues[0].first->getType();
        bool AllSameType = true;
        for (const auto &[Val, Block] : IncomingValues) {
            if (Val->getType() != ValueType) {
                AllSameType = false;
                break;
            }
        }

        if (AllSameType) {
            llvm::PHINode *PHI = Builder->CreatePHI(ValueType, IncomingValues.size(), "try.value");
            for (const auto &[Val, Block] : IncomingValues) {
                PHI->addIncoming(Val, Block);
            }
            return PHI;
        }
    }

    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitSizeOf(const PlannedSizeOf &SizeOf) {
    // The size is already computed by the Planner - just emit it as a constant
    // sizeof returns a size_t (u64)
    llvm::Type *SizeTy = llvm::Type::getInt64Ty(*Context);
    return llvm::ConstantInt::get(SizeTy, SizeOf.Size);
}

llvm::Expected<llvm::Value*> Emitter::emitIs(const PlannedIs &Is) {
    // 'is' expression tests if a value is of a specific union variant type
    // This requires the previous operand in the expression to be a union value
    // For now, return a placeholder - full implementation needs context from operand
    //
    // In a full implementation:
    // 1. Get the union value from the expression context
    // 2. Load the tag from the union
    // 3. Compare against the expected tag for TestType
    // 4. Return i1 (bool) result
    //
    // Since 'is' is typically used as a postfix operator on an operand,
    // the actual implementation would happen in emitOperand where we have
    // access to the preceding operand value.

    // For standalone use, return false (shouldn't happen in well-formed code)
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*Context), 0);
}

llvm::Expected<llvm::Value*> Emitter::emitTuple(const PlannedTuple &Tuple) {
    // Handle grouped expressions (single anonymous component)
    // vs actual tuples (multiple or named components)

    if (Tuple.Components.size() == 1 && !Tuple.Components[0].Name) {
        // Grouped expression like (2 + 3) - evaluate the inner value
        if (!Tuple.Components[0].Value.empty()) {
            return emitOperand(Tuple.Components[0].Value.back());
        }
        return nullptr;
    }

    // Actual tuple - create a struct with the component values
    // First, determine the component types and emit values
    std::vector<llvm::Type*> ComponentTypes;
    std::vector<llvm::Value*> ComponentValues;

    for (const auto &Comp : Tuple.Components) {
        if (Comp.Value.empty()) {
            return llvm::make_error<llvm::StringError>(
                "Tuple component has no value",
                llvm::inconvertibleErrorCode()
            );
        }

        // Emit the component value
        auto ValueOrErr = emitOperands(Comp.Value);
        if (!ValueOrErr)
            return ValueOrErr.takeError();

        llvm::Value *Val = *ValueOrErr;
        ComponentTypes.push_back(Val->getType());
        ComponentValues.push_back(Val);
    }

    // Create an anonymous struct type for the tuple
    llvm::StructType *TupleTy = llvm::StructType::get(*Context, ComponentTypes);

    // Allocate space for the tuple and store component values
    llvm::Value *TuplePtr = Builder->CreateAlloca(TupleTy, nullptr, "tuple");

    for (size_t i = 0; i < ComponentValues.size(); ++i) {
        llvm::Value *FieldPtr = Builder->CreateStructGEP(TupleTy, TuplePtr, i, "tuple.field");
        Builder->CreateStore(ComponentValues[i], FieldPtr);
    }

    // Load and return the tuple value
    return Builder->CreateLoad(TupleTy, TuplePtr, "tuple.val");
}

llvm::Expected<llvm::Value*> Emitter::emitMatrix(const PlannedMatrix &Matrix) {
    // Matrix is a 2D array of operands
    // For a 1D array (vector), Operations has one row
    // For a 2D matrix, Operations has multiple rows

    if (Matrix.Operations.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Matrix has no elements",
            llvm::inconvertibleErrorCode()
        );
    }

    // Check if this is a 1D array (vector) or 2D matrix
    bool Is1D = Matrix.Operations.size() == 1;

    if (Is1D) {
        // 1D array (vector): [e1, e2, e3, ...]
        const auto &Elements = Matrix.Operations[0];

        if (Elements.empty()) {
            // Empty array
            llvm::Type *ElemTy = mapType(Matrix.ElementType);
            llvm::ArrayType *ArrTy = llvm::ArrayType::get(ElemTy, 0);
            return llvm::UndefValue::get(ArrTy);
        }

        // Emit all element values
        std::vector<llvm::Value*> ElementValues;
        llvm::Type *ElemTy = nullptr;

        for (const auto &Elem : Elements) {
            auto ValueOrErr = emitOperand(Elem);
            if (!ValueOrErr)
                return ValueOrErr.takeError();

            llvm::Value *Val = *ValueOrErr;
            ElementValues.push_back(Val);

            if (!ElemTy) {
                ElemTy = Val->getType();
            }
        }

        // Create array type
        llvm::ArrayType *ArrTy = llvm::ArrayType::get(ElemTy, ElementValues.size());

        // Allocate and populate the array
        llvm::Value *ArrPtr = Builder->CreateAlloca(ArrTy, nullptr, "array");

        for (size_t i = 0; i < ElementValues.size(); ++i) {
            llvm::Value *Indices[] = {
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 0),
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), i)
            };
            llvm::Value *ElemPtr = Builder->CreateGEP(ArrTy, ArrPtr, Indices, "elem.ptr");
            Builder->CreateStore(ElementValues[i], ElemPtr);
        }

        // Load and return the array value
        return Builder->CreateLoad(ArrTy, ArrPtr, "array.val");
    } else {
        // 2D matrix: [[e11, e12], [e21, e22], ...]
        size_t NumRows = Matrix.Operations.size();
        size_t NumCols = Matrix.Operations[0].size();

        // Verify all rows have the same length
        for (const auto &Row : Matrix.Operations) {
            if (Row.size() != NumCols) {
                return llvm::make_error<llvm::StringError>(
                    "Matrix rows have different lengths",
                    llvm::inconvertibleErrorCode()
                );
            }
        }

        // Emit all element values
        std::vector<std::vector<llvm::Value*>> RowValues;
        llvm::Type *ElemTy = nullptr;

        for (const auto &Row : Matrix.Operations) {
            std::vector<llvm::Value*> ColValues;
            for (const auto &Elem : Row) {
                auto ValueOrErr = emitOperand(Elem);
                if (!ValueOrErr)
                    return ValueOrErr.takeError();

                llvm::Value *Val = *ValueOrErr;
                ColValues.push_back(Val);

                if (!ElemTy) {
                    ElemTy = Val->getType();
                }
            }
            RowValues.push_back(ColValues);
        }

        // Create 2D array type: [NumRows x [NumCols x ElemTy]]
        llvm::ArrayType *RowTy = llvm::ArrayType::get(ElemTy, NumCols);
        llvm::ArrayType *MatrixTy = llvm::ArrayType::get(RowTy, NumRows);

        // Allocate and populate the matrix
        llvm::Value *MatrixPtr = Builder->CreateAlloca(MatrixTy, nullptr, "matrix");

        for (size_t i = 0; i < NumRows; ++i) {
            for (size_t j = 0; j < NumCols; ++j) {
                llvm::Value *Indices[] = {
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 0),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), i),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), j)
                };
                llvm::Value *ElemPtr = Builder->CreateGEP(MatrixTy, MatrixPtr, Indices, "elem.ptr");
                Builder->CreateStore(RowValues[i][j], ElemPtr);
            }
        }

        // Load and return the matrix value
        return Builder->CreateLoad(MatrixTy, MatrixPtr, "matrix.val");
    }
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
            // Handle the action (may be expression or assignment)
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();

            // If not an assignment, save value as potential result
            if (Action->Target.empty()) {
                LastValue = *ValueOrErr;
            }
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
