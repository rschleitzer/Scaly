// Emitter.cpp - LLVM IR generation from Plan

#include "Emitter.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include <cstring>
#include <set>

// Flag to track if LLVM library has been loaded for JIT symbol resolution
static bool LLVMLibraryLoaded = false;

namespace scaly {

// Helper to decode first UTF-8 character to Unicode code point
static int32_t decodeUtf8ToCodePoint(const std::string &Str) {
    if (Str.empty()) return 0;

    const unsigned char *s = reinterpret_cast<const unsigned char*>(Str.data());
    size_t len = Str.size();
    int32_t codePoint;

    if ((s[0] & 0x80) == 0) {
        codePoint = s[0];
    } else if ((s[0] & 0xE0) == 0xC0 && len >= 2) {
        codePoint = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
    } else if ((s[0] & 0xF0) == 0xE0 && len >= 3) {
        codePoint = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    } else if ((s[0] & 0xF8) == 0xF0 && len >= 4) {
        codePoint = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
                    ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
    } else {
        codePoint = s[0];  // Fallback to raw byte
    }
    return codePoint;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================

Emitter::Emitter(const EmitterConfig &Cfg) : Config(Cfg) {
    // Initialize LLVM targets for JIT and AOT (only once)
    static bool LLVMInitialized = false;
    if (!LLVMInitialized) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        LLVMInitialized = true;
    }

    // Load LLVM shared library to make C API symbols available for JIT
    // This is needed when JIT-compiling Scaly code that uses LLVM bindings
    if (!LLVMLibraryLoaded) {
        // Try LLVM library paths - CMake-detected path first, then fallbacks
        const char* LLVMLibPaths[] = {
#ifdef LLVM_SHARED_LIB_PATH
            // Path detected by CMake at build time (most reliable)
            LLVM_SHARED_LIB_PATH,
#endif
            // macOS Homebrew fallbacks
            "/opt/homebrew/opt/llvm@18/lib/libLLVM.dylib",
            "/opt/homebrew/opt/llvm/lib/libLLVM.dylib",
            "/opt/homebrew/lib/libLLVM.dylib",
            "/usr/local/opt/llvm/lib/libLLVM.dylib",
            "/usr/local/lib/libLLVM.dylib",
            // Linux fallbacks
            "/usr/lib/llvm-18/lib/libLLVM.so",
            "/usr/lib/llvm-18/lib/libLLVM-18.so",
            "/usr/lib/x86_64-linux-gnu/libLLVM-18.so",
            "/usr/lib/libLLVM-18.so",
            "/usr/lib/libLLVM.so",
            nullptr
        };

        for (const char** Path = LLVMLibPaths; *Path; ++Path) {
            std::string ErrMsg;
            if (!llvm::sys::DynamicLibrary::LoadLibraryPermanently(*Path, &ErrMsg)) {
                LLVMLibraryLoaded = true;
                break;
            }
        }
    }

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
    IntrinsicTypes["uint"] = IntrinsicTypes["u64"];
    IntrinsicTypes["size_t"] = IntrinsicTypes["u64"];
    IntrinsicTypes["float"] = IntrinsicTypes["f32"];
    IntrinsicTypes["double"] = IntrinsicTypes["f64"];
    IntrinsicTypes["char"] = IntrinsicTypes["i32"];  // Unicode code point
}

// ============================================================================
// Debug Info Initialization
// ============================================================================

void Emitter::initDebugInfo(llvm::StringRef FileName) {
    if (!Config.EmitDebugInfo)
        return;

    // Set DWARF version (4 is widely supported)
    Module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 4);
    Module->addModuleFlag(llvm::Module::Warning, "Debug Info Version",
                          llvm::DEBUG_METADATA_VERSION);

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
// RBMM (Region-Based Memory Management) Initialization
// ============================================================================

void Emitter::initRBMM() {
    auto *VoidTy = llvm::Type::getVoidTy(*Context);
    auto *PtrTy = llvm::PointerType::get(*Context, 0);
    auto *I64Ty = llvm::Type::getInt64Ty(*Context);

    // Create Page struct type
    // Page { next_object: ptr, current_page: ptr, next_page: ptr, exclusive_pages: PageList }
    // For now, we treat PageList as an opaque struct (pointer + pointer for simplicity)
    // The actual layout is: { ptr, ptr, ptr, { ptr, ptr } }
    PageType = llvm::StructType::create(*Context, "Page");
    auto *PageListType = llvm::StructType::create(*Context, {PtrTy, PtrTy}, "PageList");
    PageType->setBody({PtrTy, PtrTy, PtrTy, PageListType});

    // Note: aligned_alloc and free are now declared as 'extern' in Scaly source code
    // They will be emitted when the extern function declarations are processed

    // Declare Page_allocate_page() -> ptr
    // This is the static function that allocates a new page
    auto *AllocatePageTy = llvm::FunctionType::get(PtrTy, {}, false);
    PageAllocatePage = llvm::Function::Create(
        AllocatePageTy, llvm::GlobalValue::ExternalLinkage,
        "_ZN4Page13allocate_pageEv", *Module);
    FunctionCache["_ZN4Page13allocate_pageEv"] = PageAllocatePage;

    // Declare Page_allocate(page: ptr, size: i64, align: i64) -> ptr
    // This is the method that allocates from a page
    auto *AllocateTy = llvm::FunctionType::get(PtrTy, {PtrTy, I64Ty, I64Ty}, false);
    PageAllocate = llvm::Function::Create(
        AllocateTy, llvm::GlobalValue::ExternalLinkage,
        "_ZN4Page8allocateEmm", *Module);  // size_t = unsigned long (m)
    FunctionCache["_ZN4Page8allocateEmm"] = PageAllocate;

    // Declare Page_deallocate_extensions(page: ptr) -> void
    // Called to clean up a page's extensions when leaving a scope
    auto *DeallocateTy = llvm::FunctionType::get(VoidTy, {PtrTy}, false);
    PageDeallocateExtensions = llvm::Function::Create(
        DeallocateTy, llvm::GlobalValue::ExternalLinkage,
        "_ZN4Page21deallocate_extensionsEv", *Module);
    FunctionCache["_ZN4Page21deallocate_extensionsEv"] = PageDeallocateExtensions;

    // Create BlockWatermark struct type for block-scoped cleanup
    // BlockWatermark { page: ptr, next_object: ptr, extension_tail: ptr, exclusive_head: ptr }
    BlockWatermarkType = llvm::StructType::create(*Context, {PtrTy, PtrTy, PtrTy, PtrTy}, "BlockWatermark");

    // Declare Page_save_watermark(page: ptr) -> BlockWatermark (returned by sret)
    // Note: Uses sret calling convention - first param is pointer to return struct
    auto *SaveWatermarkTy = llvm::FunctionType::get(VoidTy, {PtrTy, PtrTy}, false);
    PageSaveWatermark = llvm::Function::Create(
        SaveWatermarkTy, llvm::GlobalValue::ExternalLinkage,
        "_ZN4Page14save_watermarkEv", *Module);
    // Set sret attribute on first param so emitReturn knows to store-and-return-void
    PageSaveWatermark->addParamAttr(0, llvm::Attribute::getWithStructRetType(*Context, BlockWatermarkType));
    FunctionCache["_ZN4Page14save_watermarkEv"] = PageSaveWatermark;

    // Declare Page_restore_watermark(page: ptr, watermark: ptr) -> void
    // watermark is passed as pointer to BlockWatermark struct
    auto *RestoreWatermarkTy = llvm::FunctionType::get(VoidTy, {PtrTy, PtrTy}, false);
    PageRestoreWatermark = llvm::Function::Create(
        RestoreWatermarkTy, llvm::GlobalValue::ExternalLinkage,
        "_ZN4Page17restore_watermarkE14BlockWatermark", *Module);
    FunctionCache["_ZN4Page17restore_watermarkE14BlockWatermark"] = PageRestoreWatermark;
}

// Declare C runtime functions (aligned_alloc, free, exit)
// These are needed by Page.scaly for RBMM
void Emitter::declareRuntimeFunctions() {
    auto *PtrTy = llvm::PointerType::getUnqual(*Context);
    auto *I64Ty = llvm::Type::getInt64Ty(*Context);
    auto *I32Ty = llvm::Type::getInt32Ty(*Context);
    auto *VoidTy = llvm::Type::getVoidTy(*Context);

    // aligned_alloc(alignment: i64, size: i64) returns pointer[void]
    // First check if it was already declared (e.g., from Plan's extern declarations)
    if (auto It = FunctionCache.find("aligned_alloc"); It != FunctionCache.end()) {
        AlignedAlloc = It->second;
    } else if (!AlignedAlloc) {
        auto *AlignedAllocTy = llvm::FunctionType::get(PtrTy, {I64Ty, I64Ty}, false);
        AlignedAlloc = llvm::Function::Create(
            AlignedAllocTy, llvm::GlobalValue::ExternalLinkage,
            "aligned_alloc", *Module);
        FunctionCache["aligned_alloc"] = AlignedAlloc;
    }

    // free(ptr: pointer[void])
    if (auto It = FunctionCache.find("free"); It != FunctionCache.end()) {
        Free = It->second;
    } else if (!Free) {
        auto *FreeTy = llvm::FunctionType::get(VoidTy, {PtrTy}, false);
        Free = llvm::Function::Create(
            FreeTy, llvm::GlobalValue::ExternalLinkage,
            "free", *Module);
        FunctionCache["free"] = Free;
    }

    // exit(code: int)
    if (auto It = FunctionCache.find("exit"); It != FunctionCache.end()) {
        ExitFunc = It->second;
    } else if (!ExitFunc) {
        auto *ExitTy = llvm::FunctionType::get(VoidTy, {I32Ty}, false);
        ExitFunc = llvm::Function::Create(
            ExitTy, llvm::GlobalValue::ExternalLinkage,
            "exit", *Module);
        FunctionCache["exit"] = ExitFunc;
    }
}

// ============================================================================
// Main Emission Entry Point
// ============================================================================

llvm::Expected<std::unique_ptr<llvm::Module>> Emitter::emit(const Plan &P,
                                                             llvm::StringRef ModuleName) {
    // Store current plan for type lookups
    CurrentPlan = &P;

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

    // Initialize RBMM runtime declarations (after cache clear so they persist)
    initRBMM();

    // Phase 1: Emit all type declarations (forward declarations for structs)
    for (const auto &[name, Struct] : P.Structures) {
        emitStructType(Struct);
    }
    for (const auto &[name, Union] : P.Unions) {
        emitUnionType(Union);
    }

    // Phase 1.5: Emit global constants
    for (const auto &[name, Global] : P.Globals) {
        emitGlobal(Global);
    }

    // Phase 2: Emit all function declarations
    for (const auto &[name, Func] : P.Functions) {
        emitFunctionDecl(Func);
    }
    // Also emit method, operator, initializer, and deinitializer declarations from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            emitFunctionDecl(Method);
        }
        for (const auto &Op : Struct.Operators) {
            // Convert PlannedOperator to PlannedFunction for emission
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;  // Operators are always pure
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
        for (const auto &Init : Struct.Initializers) {
            emitInitializerDecl(Struct, Init);
        }
        if (Struct.Deinitializer) {
            emitDeInitializerDecl(Struct, *Struct.Deinitializer);
        }
    }

    // Phase 3: Emit function bodies
    for (const auto &[name, Func] : P.Functions) {
        if (auto *LLVMFunc = FunctionCache[Func.MangledName]) {
            if (auto Err = emitFunctionBody(Func, LLVMFunc))
                return std::move(Err);
        }
    }
    // Also emit method, operator, initializer, and deinitializer bodies from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return std::move(Err);
            }
        }
        for (const auto &Op : Struct.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                // Convert PlannedOperator to PlannedFunction for emission
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return std::move(Err);
            }
        }
        for (const auto &Init : Struct.Initializers) {
            if (auto *LLVMFunc = FunctionCache[Init.MangledName]) {
                if (auto Err = emitInitializerBody(Struct, Init, LLVMFunc))
                    return std::move(Err);
            }
        }
        if (Struct.Deinitializer) {
            if (auto *LLVMFunc = FunctionCache[Struct.Deinitializer->MangledName]) {
                if (auto Err = emitDeInitializerBody(Struct, *Struct.Deinitializer, LLVMFunc))
                    return std::move(Err);
            }
        }
    }
    // Also emit method and operator bodies from unions (e.g., Option.unwrap)
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return std::move(Err);
            }
        }
        for (const auto &Op : Union.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return std::move(Err);
            }
        }
    }

    // Phase 4: Emit top-level statements (program entry)
    if (!P.Statements.empty()) {
        if (auto Err = emitMainWrapper(P)) {
            return std::move(Err);
        }
    }

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

llvm::Error Emitter::emitObjectFile(const Plan &P,
                                     llvm::StringRef ModuleName,
                                     llvm::StringRef OutputPath) {
    // First, emit the LLVM IR module
    auto ModuleOrErr = emit(P, ModuleName);
    if (!ModuleOrErr) {
        return ModuleOrErr.takeError();
    }
    auto TheModule = std::move(*ModuleOrErr);

    // Get target triple
    std::string TargetTriple = Config.TargetTriple.empty()
        ? llvm::sys::getDefaultTargetTriple()
        : Config.TargetTriple;
    TheModule->setTargetTriple(TargetTriple);

    // Look up the target
    std::string Error;
    const llvm::Target *Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target) {
        return llvm::make_error<llvm::StringError>(
            "Failed to lookup target: " + Error,
            llvm::inconvertibleErrorCode()
        );
    }

    // Create target machine
    llvm::TargetOptions Options;
    auto RM = std::optional<llvm::Reloc::Model>(llvm::Reloc::PIC_);
    auto TheTargetMachine = std::unique_ptr<llvm::TargetMachine>(
        Target->createTargetMachine(
            TargetTriple,
            "generic",  // CPU
            "",         // Features
            Options,
            RM
        )
    );

    if (!TheTargetMachine) {
        return llvm::make_error<llvm::StringError>(
            "Failed to create target machine",
            llvm::inconvertibleErrorCode()
        );
    }

    // Set data layout
    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    // Open output file
    std::error_code EC;
    llvm::raw_fd_ostream Dest(OutputPath, EC, llvm::sys::fs::OF_None);
    if (EC) {
        return llvm::make_error<llvm::StringError>(
            "Could not open output file: " + EC.message(),
            llvm::inconvertibleErrorCode()
        );
    }

    // Create pass manager and emit object code
    llvm::legacy::PassManager PM;
    if (TheTargetMachine->addPassesToEmitFile(PM, Dest, nullptr,
                                               llvm::CodeGenFileType::ObjectFile)) {
        return llvm::make_error<llvm::StringError>(
            "Target machine cannot emit object file",
            llvm::inconvertibleErrorCode()
        );
    }

    PM.run(*TheModule);
    Dest.flush();

    return llvm::Error::success();
}

llvm::Error Emitter::emitBitcodeFile(const Plan &P,
                                     llvm::StringRef ModuleName,
                                     llvm::StringRef OutputPath) {
    // First, emit the LLVM IR module
    auto ModuleOrErr = emit(P, ModuleName);
    if (!ModuleOrErr) {
        return ModuleOrErr.takeError();
    }
    auto TheModule = std::move(*ModuleOrErr);

    // Open output file
    std::error_code EC;
    llvm::raw_fd_ostream Dest(OutputPath, EC, llvm::sys::fs::OF_None);
    if (EC) {
        return llvm::make_error<llvm::StringError>(
            "Could not open output file: " + EC.message(),
            llvm::inconvertibleErrorCode()
        );
    }

    // Write bitcode
    llvm::WriteBitcodeToFile(*TheModule, Dest);
    Dest.flush();

    return llvm::Error::success();
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
    if (auto *StructTy = lookupStructType(Type)) {
        return StructTy;
    }

    // For pointer types: pointer[T]
    if (Type.Name == "pointer" && !Type.Generics.empty()) {
        // In LLVM 15+, all pointers are opaque (ptr)
        auto *PtrTy = llvm::PointerType::get(*Context, 0);
        TypeCache[Type.MangledName] = PtrTy;
        return PtrTy;
    }

    // For ref types: ref[T] (non-owning reference, same as pointer at LLVM level)
    if (Type.Name == "ref" && !Type.Generics.empty()) {
        auto *PtrTy = llvm::PointerType::get(*Context, 0);
        TypeCache[Type.MangledName] = PtrTy;
        return PtrTy;
    }

    // For Option[T]: null pointer optimization (null = None, non-null = Some)
    // NPO only applies when the inner type is pointer or ref
    if (Type.Name == "Option" && !Type.Generics.empty()) {
        const PlannedType &InnerType = Type.Generics[0];
        bool IsPointerLike = InnerType.Name == "pointer" || InnerType.Name == "ref" ||
                             (InnerType.Name.size() >= 8 && InnerType.Name.substr(0, 8) == "pointer.") ||
                             (InnerType.Name.size() >= 4 && InnerType.Name.substr(0, 4) == "ref.");
        if (IsPointerLike) {
            auto *PtrTy = llvm::PointerType::get(*Context, 0);
            TypeCache[Type.MangledName] = PtrTy;
            return PtrTy;
        }
    }
    // Handle instantiated Option types where the inner type is baked into the name
    // e.g., "Option.ref" with Generics=[int] or "Option.ref.int" with Generics=[]
    if (Type.Name.size() > 7 && Type.Name.substr(0, 7) == "Option.") {
        // Check if this is Option over a pointer-like type (ref or pointer)
        // Name format: "Option.ref" or "Option.pointer" or "Option.ref.int"
        std::string_view NameView = Type.Name;
        std::string_view Inner = NameView.substr(7); // After "Option."
        if (Inner.substr(0, 3) == "ref" || Inner.substr(0, 7) == "pointer") {
            // Option[ref[T]] or Option[pointer[T]] - use NPO (single pointer)
            auto *PtrTy = llvm::PointerType::get(*Context, 0);
            TypeCache[Type.MangledName] = PtrTy;
            return PtrTy;
        }
    }

    // Check if this type is a struct in the Plan that hasn't been emitted yet
    // This handles nested generics like Box[Box[int]] where the inner Box[int]
    // is referenced as a property type before being explicitly emitted
    if (CurrentPlan) {
        // Try to find by Name (e.g., "Box.int")
        auto StructIt = CurrentPlan->Structures.find(Type.Name);
        if (StructIt != CurrentPlan->Structures.end()) {
            auto *StructTy = emitStructType(StructIt->second);
            return StructTy;
        }

        // Try to find by MangledName
        for (const auto &[Key, Struct] : CurrentPlan->Structures) {
            if (Struct.MangledName == Type.MangledName) {
                auto *StructTy = emitStructType(Struct);
                return StructTy;
            }
        }

        // Check unions too
        auto UnionIt = CurrentPlan->Unions.find(Type.Name);
        if (UnionIt != CurrentPlan->Unions.end()) {
            auto *UnionTy = emitUnionType(UnionIt->second);
            return UnionTy;
        }
    }

    // Unknown type - should have been planned (programming error)
    // Return opaque pointer as fallback
    return llvm::PointerType::get(*Context, 0);
}

llvm::Type *Emitter::mapIntrinsicType(llvm::StringRef Name) {
    if (auto It = IntrinsicTypes.find(Name.str()); It != IntrinsicTypes.end()) {
        return It->second.LLVMType;
    }
    return nullptr;
}

llvm::StructType *Emitter::lookupStructType(const PlannedType &Type) {
    return lookupStructType(Type.Name, Type.MangledName);
}

llvm::StructType *Emitter::lookupStructType(llvm::StringRef Name, llvm::StringRef MangledName) {
    // Try mangled name first (most specific)
    if (auto It = StructCache.find(MangledName.str()); It != StructCache.end()) {
        return It->second;
    }
    // Try readable name
    if (auto It = StructCache.find(Name.str()); It != StructCache.end()) {
        return It->second;
    }
    // Try with _Z prefix (full mangled name)
    if (auto It = StructCache.find("_Z" + MangledName.str()); It != StructCache.end()) {
        return It->second;
    }
    // Try without _Z prefix if MangledName starts with _Z
    if (MangledName.starts_with("_Z")) {
        std::string WithoutPrefix = MangledName.substr(2).str();
        if (auto It = StructCache.find(WithoutPrefix); It != StructCache.end()) {
            return It->second;
        }
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
        auto *PropTy = mapType(Prop.PropType);
        Elements.push_back(PropTy);
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

    // NPO (Null Pointer Optimization) for Option types:
    // Option[ref[T]] and Option[pointer[T]] are represented as a single pointer
    // (null = None, non-null = Some). Don't create a struct for these.
    // Instead, return a "fake" struct that will be bypassed by mapType's NPO check.
    // Actually, we need to handle this in mapType - here we just skip creating the struct.
    if (Union.Name == "Option" || Union.Name.substr(0, 7) == "Option.") {
        // Check if this is Option over a pointer-like type
        bool HasPointerVariant = false;
        for (const auto &Var : Union.Variants) {
            if (Var.VarType) {
                const std::string &VarTypeName = Var.VarType->Name;
                if (VarTypeName == "ref" || VarTypeName == "pointer" ||
                    VarTypeName.substr(0, 4) == "ref." || VarTypeName.substr(0, 8) == "pointer.") {
                    HasPointerVariant = true;
                    break;
                }
            }
        }
        if (HasPointerVariant) {
            // Don't create a struct - mapType will return pointer type for this
            // Create a minimal placeholder that won't be used
            auto *PtrTy = llvm::StructType::create(*Context, {llvm::PointerType::get(*Context, 0)}, Union.MangledName + "_NPO");
            // Don't cache this - we want mapType's NPO check to handle it
            return PtrTy;
        }
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
// Global Constant Emission
// ============================================================================

llvm::GlobalVariable *Emitter::emitGlobal(const PlannedGlobal &Global) {
    // Get the element type from the global's type
    // For array types like size_t[], we need to look at the generic argument
    llvm::Type *ElemTy = nullptr;
    size_t ArraySize = 0;

    // Check if this is an array type (has empty generics or [] suffix)
    if (Global.GlobalType.Name == "pointer" && !Global.GlobalType.Generics.empty()) {
        // pointer[T] - get element type from generic
        ElemTy = mapType(Global.GlobalType.Generics[0]);
    } else {
        // Scalar type
        ElemTy = mapType(Global.GlobalType);
    }

    // For array constants, the value is a Matrix (vector literal)
    if (Global.Value.size() == 1) {
        if (auto *Matrix = std::get_if<PlannedMatrix>(&Global.Value[0].Expr)) {
            // Matrix structure: [2, 3, 5, 7, 11] creates 5 rows of 1 column each
            // OR: [[2, 3, 5, 7, 11]] creates 1 row of 5 columns
            if (!Matrix->Operations.empty()) {
                if (Matrix->Operations.size() == 1) {
                    // Single row with multiple columns: [[2, 3, 5, 7, 11]]
                    ArraySize = Matrix->Operations[0].size();
                } else if (!Matrix->Operations.empty() && Matrix->Operations[0].size() == 1) {
                    // Multiple rows with 1 column each: [2, 3, 5, 7, 11]
                    ArraySize = Matrix->Operations.size();
                }
            }
        }
    }

    if (ArraySize == 0) {
        // Scalar constants are inlined by the Planner, so this path is not reached
        return nullptr;
    }

    // Create array type
    llvm::ArrayType *ArrTy = llvm::ArrayType::get(ElemTy, ArraySize);

    // Build constant initializer
    std::vector<llvm::Constant*> Elements;
    if (Global.Value.size() == 1) {
        if (auto *Matrix = std::get_if<PlannedMatrix>(&Global.Value[0].Expr)) {
            if (Matrix->Operations.size() == 1) {
                // Single row with multiple columns
                for (const auto &Elem : Matrix->Operations[0]) {
                    if (auto *Const = std::get_if<PlannedConstant>(&Elem.Expr)) {
                        if (auto *IConst = std::get_if<IntegerConstant>(Const)) {
                            Elements.push_back(llvm::ConstantInt::get(ElemTy, IConst->Value));
                        } else if (auto *HConst = std::get_if<HexConstant>(Const)) {
                            Elements.push_back(llvm::ConstantInt::get(ElemTy, HConst->Value));
                        }
                    }
                }
            } else {
                // Multiple rows, 1 column each
                for (const auto &Row : Matrix->Operations) {
                    if (!Row.empty()) {
                        if (auto *Const = std::get_if<PlannedConstant>(&Row[0].Expr)) {
                            if (auto *IConst = std::get_if<IntegerConstant>(Const)) {
                                Elements.push_back(llvm::ConstantInt::get(ElemTy, IConst->Value));
                            } else if (auto *HConst = std::get_if<HexConstant>(Const)) {
                                Elements.push_back(llvm::ConstantInt::get(ElemTy, HConst->Value));
                            }
                        }
                    }
                }
            }
        }
    }

    if (Elements.size() != ArraySize) {
        // Not all elements could be converted to constants
        return nullptr;
    }

    // Create constant array
    llvm::Constant *Init = llvm::ConstantArray::get(ArrTy, Elements);

    // Create global variable
    auto *GV = new llvm::GlobalVariable(
        *Module,
        ArrTy,
        true,  // isConstant
        llvm::GlobalValue::PrivateLinkage,
        Init,
        Global.MangledName
    );

    return GV;
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
    }

    // If function throws, wrap return type in a Result union: { i8 tag, data }
    // tag=0 means success (value), tag=1 means error
    if (Func.Throws) {
        llvm::Type *ValueTy = ReturnLLVMType ? ReturnLLVMType : llvm::Type::getVoidTy(*Context);
        llvm::Type *ErrorTy = mapType(*Func.Throws);

        // Data is a byte array large enough for either value or error
        // (union semantics - same memory can hold either type)
        uint64_t ValueSize = ValueTy->isVoidTy() ? 0 : Module->getDataLayout().getTypeAllocSize(ValueTy);
        uint64_t ErrorSize = Module->getDataLayout().getTypeAllocSize(ErrorTy);
        uint64_t DataSize = std::max(ValueSize, ErrorSize);
        if (DataSize == 0) DataSize = 1;  // Minimum size for empty unions

        llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
        llvm::ArrayType *DataTy = llvm::ArrayType::get(I8Ty, DataSize);

        // Create the Result struct type: { i8 tag, [N x i8] data }
        std::string ResultName = "_ZResult_" + Func.MangledName;
        llvm::StructType *ResultTy = llvm::StructType::create(
            *Context, {I8Ty, DataTy}, ResultName, false);

        ReturnLLVMType = ResultTy;
    }

    // Use sret for non-primitive types (structs, unions)
    if (ReturnLLVMType && (ReturnLLVMType->isStructTy() || ReturnLLVMType->isArrayTy())) {
        UseSret = true;
        ParamTypes.push_back(llvm::PointerType::get(*Context, 0));
    }

    // If function# was used, add page parameter
    if (Func.PageParameter) {
        ParamTypes.push_back(llvm::PointerType::get(*Context, 0));
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

    // Use unmangled "main" for the entry point so the linker can find it
    std::string FuncName = (Func.Name == "main") ? "main" : Func.MangledName;

    // Create function
    auto *LLVMFunc = llvm::Function::Create(
        FuncTy,
        llvm::GlobalValue::ExternalLinkage,
        FuncName,
        *Module
    );

    // Set parameter attributes
    unsigned ParamIdx = 0;
    if (UseSret) {
        // StructRet is a type attribute - must include the struct type
        LLVMFunc->addParamAttr(ParamIdx,
            llvm::Attribute::getWithStructRetType(*Context, ReturnLLVMType));
        LLVMFunc->addParamAttr(ParamIdx, llvm::Attribute::NoAlias);
        ParamIdx++;
    }
    if (Func.PageParameter) {
        // Page parameter - skip it for attribute setting
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

    // Track throwing functions for call site exception page allocation
    if (Func.CanThrow || Func.Throws) {
        ThrowingFunctions.insert(Func.MangledName);
    }

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

    // Skip if function body already emitted
    if (!LLVMFunc->isDeclaration()) {
        return llvm::Error::success();
    }

    // Create entry block
    auto *EntryBB = llvm::BasicBlock::Create(*Context, "entry", LLVMFunc);
    Builder->SetInsertPoint(EntryBB);

    // Create a dummy instruction as an alloca insertion point marker
    // All allocas will be inserted after this to ensure they're in the entry block
    AllocaInsertPt = new llvm::BitCastInst(
        llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*Context)),
        llvm::Type::getInt32Ty(*Context), "alloca.point", EntryBB);

    // Set up current function state
    CurrentFunction = LLVMFunc;
    CurrentBlock = EntryBB;
    LocalVariables.clear();
    CurrentFunctionCanThrow = Func.CanThrow || (Func.Throws != nullptr);

    // Reset region state for this function
    CurrentRegion = RegionInfo{};

    // Bind parameters to names
    unsigned ArgIdx = 0;
    auto ArgIt = LLVMFunc->arg_begin();
    llvm::Value *SretPtr = nullptr;

    // Skip sret if present (this is the return struct pointer, not a page)
    if (Func.Returns) {
        llvm::Type *RetTy = mapType(*Func.Returns);
        if (RetTy->isStructTy() || RetTy->isArrayTy()) {
            SretPtr = &*ArgIt;
            ArgIt++;
            ArgIdx++;
        }
    }

    // Handle function# page parameter
    if (Func.PageParameter && ArgIt != LLVMFunc->arg_end()) {
        ArgIt->setName(*Func.PageParameter);
        LocalVariables[*Func.PageParameter] = &*ArgIt;
        // Use this as the return page for # allocations within this function
        CurrentRegion.ReturnPage = &*ArgIt;
        ArgIt++;
        ArgIdx++;
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

            // If this parameter has a reference lifetime, track its region
            if (Item.ItemType) {
                if (std::holds_alternative<ReferenceLifetime>(Item.ItemType->Life)) {
                    const auto &RefLife = std::get<ReferenceLifetime>(Item.ItemType->Life);
                    // The parameter carries a region with it
                    CurrentRegion.NamedRegions[RefLife.Location] = &*ArgIt;
                }
            }
        }
        ArgIt++;
        ArgIdx++;
    }

    // Allocate local page for this function's $ allocations
    // Use the NeedsLocalPage flag set by the Planner based on function body analysis
    bool NeedsLocalPage = Func.NeedsLocalPage;

    // Also check if any return type or parameter has LocalLifetime
    if (Func.Returns && std::holds_alternative<LocalLifetime>(Func.Returns->Life)) {
        NeedsLocalPage = true;
    }
    for (const auto &Item : Func.Input) {
        if (Item.ItemType && std::holds_alternative<LocalLifetime>(Item.ItemType->Life)) {
            NeedsLocalPage = true;
            break;
        }
    }
    // Also check the function's own lifetime annotation
    if (std::holds_alternative<LocalLifetime>(Func.Life)) {
        NeedsLocalPage = true;
    }

    if (NeedsLocalPage) {
        CurrentRegion.LocalPage = Builder->CreateCall(PageAllocatePage, {}, "local_page");
    } else {
        CurrentRegion.LocalPage = nullptr;  // Will use stack allocation as fallback
    }

    // Emit function body
    llvm::Value *ReturnValue = nullptr;
    if (auto *Action = std::get_if<PlannedAction>(&Func.Impl)) {
        auto ValueOrErr = emitAction(*Action);
        if (!ValueOrErr) {
            return ValueOrErr.takeError();
        }
        ReturnValue = *ValueOrErr;
    }

    // Clean up local page before returning
    // We need to do this before every return point
    // Only call deallocate_extensions if the page was actually used for allocations
    auto CleanupLocalPage = [this]() {
        if (CurrentRegion.LocalPage && PageType) {
            // Check if the page was used for allocations by comparing next_object to page + 1
            // A fresh page has next_object = page + 1 (after reset), so if unchanged, no allocations were made
            // This avoids infinite recursion: deallocate_extensions -> get_iterator -> deallocate_extensions
            llvm::Value *NextObjectPtr = Builder->CreateStructGEP(PageType, CurrentRegion.LocalPage, 0, "next_object_ptr");
            llvm::Value *NextObject = Builder->CreateLoad(Builder->getPtrTy(), NextObjectPtr, "next_object");
            llvm::Value *PagePlus1 = Builder->CreateGEP(PageType, CurrentRegion.LocalPage,
                                                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 1),
                                                        "page_plus_1");
            llvm::Value *WasUsed = Builder->CreateICmpNE(NextObject, PagePlus1, "page_was_used");

            llvm::BasicBlock *CleanupBlock = llvm::BasicBlock::Create(*Context, "cleanup", CurrentFunction);
            llvm::BasicBlock *SkipCleanupBlock = llvm::BasicBlock::Create(*Context, "skip_cleanup", CurrentFunction);
            Builder->CreateCondBr(WasUsed, CleanupBlock, SkipCleanupBlock);

            Builder->SetInsertPoint(CleanupBlock);
            Builder->CreateCall(PageDeallocateExtensions, {CurrentRegion.LocalPage});
            Builder->CreateBr(SkipCleanupBlock);

            Builder->SetInsertPoint(SkipCleanupBlock);
            // Always free the local page (even if no extensions)
            if (auto *FreeFn = Module->getFunction("free")) {
                Builder->CreateCall(FreeFn, {CurrentRegion.LocalPage});
            }
        }
    };

    // Add return if block doesn't end with terminator
    // Use Builder->GetInsertBlock() to get the current block after control flow
    llvm::BasicBlock *FinalBlock = Builder->GetInsertBlock();
    if (!FinalBlock->getTerminator()) {
        CleanupLocalPage();
        if (LLVMFunc->getReturnType()->isVoidTy()) {
            // Check if this is an sret function - need to store return value
            if (SretPtr && ReturnValue) {
                if (ReturnValue->getType()->isStructTy()) {
                    Builder->CreateStore(ReturnValue, SretPtr);
                } else {
                    // Non-struct value being returned via sret (single field)
                    llvm::Type *SRetTy = LLVMFunc->getParamStructRetType(0);
                    if (SRetTy && SRetTy->isStructTy()) {
                        llvm::Value *FieldPtr = Builder->CreateStructGEP(
                            SRetTy, SretPtr, 0, "sret.field");
                        Builder->CreateStore(ReturnValue, FieldPtr);
                    } else {
                        Builder->CreateStore(ReturnValue, SretPtr);
                    }
                }
            }
            Builder->CreateRetVoid();
        } else if (ReturnValue) {
            // Return the computed value, with type conversion if needed
            llvm::Type *ExpectedRetTy = LLVMFunc->getReturnType();
            llvm::Type *ActualTy = ReturnValue->getType();

            if (ActualTy != ExpectedRetTy) {
                if (ExpectedRetTy->isStructTy() && ActualTy->isPointerTy()) {
                    // Returning pointer but function expects struct by value
                    ReturnValue = Builder->CreateLoad(ExpectedRetTy, ReturnValue, "ret.load");
                } else if (ExpectedRetTy->isPointerTy() && ActualTy->isStructTy()) {
                    // Returning struct but function expects pointer
                    // Allocate the struct on the return page (if available) or stack
                    llvm::Value *StructPtr;
                    if (CurrentRegion.ReturnPage) {
                        // Allocate on return page
                        auto &DL = Module->getDataLayout();
                        uint64_t Size = DL.getTypeAllocSize(ActualTy);
                        uint64_t Align = DL.getABITypeAlign(ActualTy).value();
                        StructPtr = Builder->CreateCall(PageAllocate,
                            {CurrentRegion.ReturnPage,
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Size),
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Align)},
                            "ret.alloc");
                    } else {
                        // Fall back to stack allocation
                        StructPtr = createEntryBlockAlloca(ActualTy, "ret.alloca");
                    }
                    Builder->CreateStore(ReturnValue, StructPtr);
                    ReturnValue = StructPtr;
                } else if (ExpectedRetTy->isPointerTy() && ActualTy->isArrayTy()) {
                    // Returning array but function expects pointer
                    // Allocate the array on the return page (if available) or stack
                    llvm::Value *ArrayPtr;
                    if (CurrentRegion.ReturnPage) {
                        auto &DL = Module->getDataLayout();
                        uint64_t Size = DL.getTypeAllocSize(ActualTy);
                        uint64_t Align = DL.getABITypeAlign(ActualTy).value();
                        ArrayPtr = Builder->CreateCall(PageAllocate,
                            {CurrentRegion.ReturnPage,
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Size),
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Align)},
                            "ret.alloc");
                    } else {
                        ArrayPtr = createEntryBlockAlloca(ActualTy, "ret.alloca");
                    }
                    Builder->CreateStore(ReturnValue, ArrayPtr);
                    ReturnValue = ArrayPtr;
                } else if ((ExpectedRetTy->isIntegerTy() || ExpectedRetTy->isFloatTy() ||
                            ExpectedRetTy->isDoubleTy()) && ActualTy->isPointerTy()) {
                    // Returning pointer but function expects primitive type
                    ReturnValue = Builder->CreateLoad(ExpectedRetTy, ReturnValue, "ret.load");
                } else if (ExpectedRetTy->isIntegerTy() && ActualTy->isIntegerTy()) {
                    // Integer type mismatch - cast
                    ReturnValue = Builder->CreateIntCast(ReturnValue, ExpectedRetTy, true, "ret.cast");
                }
            }
            Builder->CreateRet(ReturnValue);
        } else {
            // No value computed but function expects return - this shouldn't happen
            Builder->CreateUnreachable();
        }
    }

    return llvm::Error::success();
}

// ============================================================================
// Initializer Emission
// ============================================================================

llvm::Function *Emitter::emitInitializerDecl(const PlannedStructure &Struct,
                                              const PlannedInitializer &Init) {
    // Check cache
    if (auto It = FunctionCache.find(Init.MangledName); It != FunctionCache.end()) {
        return It->second;
    }

    // Build parameter types:
    // - First param is pointer to the struct being initialized (this)
    // - If init#, second param is pointer to Page
    // - Followed by any additional parameters from Init.Input
    std::vector<llvm::Type*> ParamTypes;

    // Get the struct type
    llvm::Type *StructTy = StructCache[Struct.MangledName];
    if (!StructTy) {
        return nullptr;
    }

    // First param: pointer to struct (this)
    ParamTypes.push_back(llvm::PointerType::get(*Context, 0));

    // If init# was used, add page parameter
    if (Init.PageParameter) {
        ParamTypes.push_back(llvm::PointerType::get(*Context, 0));  // pointer[Page]
    }

    // Add additional parameters
    for (const auto &Item : Init.Input) {
        if (Item.ItemType) {
            llvm::Type *ItemTy = mapType(*Item.ItemType);
            if (ItemTy->isStructTy() || ItemTy->isArrayTy()) {
                ParamTypes.push_back(llvm::PointerType::get(*Context, 0));
            } else {
                ParamTypes.push_back(ItemTy);
            }
        }
    }

    // Initializers return void (the struct is initialized in-place)
    llvm::Type *ReturnTy = llvm::Type::getVoidTy(*Context);

    // Create function type
    auto *FuncTy = llvm::FunctionType::get(ReturnTy, ParamTypes, false);

    // Create function
    auto *Func = llvm::Function::Create(
        FuncTy,
        llvm::Function::ExternalLinkage,
        Init.MangledName,
        Module.get()
    );

    // Set calling convention and attributes
    Func->setCallingConv(llvm::CallingConv::C);

    // Name parameters
    auto ArgIt = Func->arg_begin();
    ArgIt->setName("this");
    ++ArgIt;

    // If init# was used, name the page parameter
    if (Init.PageParameter && ArgIt != Func->arg_end()) {
        ArgIt->setName(*Init.PageParameter);
        ++ArgIt;
    }

    for (size_t I = 0; I < Init.Input.size() && ArgIt != Func->arg_end(); ++I, ++ArgIt) {
        if (Init.Input[I].Name) {
            ArgIt->setName(*Init.Input[I].Name);
        }
    }

    // Cache the function
    FunctionCache[Init.MangledName] = Func;

    return Func;
}

llvm::Error Emitter::emitInitializerBody(const PlannedStructure &Struct,
                                          const PlannedInitializer &Init,
                                          llvm::Function *LLVMFunc) {
    // Skip extern/intrinsic initializers
    if (std::holds_alternative<PlannedExternImpl>(Init.Impl) ||
        std::holds_alternative<PlannedIntrinsicImpl>(Init.Impl)) {
        return llvm::Error::success();
    }

    // Skip if initializer body already emitted
    if (!LLVMFunc->isDeclaration()) {
        return llvm::Error::success();
    }

    // Create entry block
    auto *EntryBB = llvm::BasicBlock::Create(*Context, "entry", LLVMFunc);
    Builder->SetInsertPoint(EntryBB);

    // Create a dummy instruction as an alloca insertion point marker
    AllocaInsertPt = new llvm::BitCastInst(
        llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*Context)),
        llvm::Type::getInt32Ty(*Context), "alloca.point", EntryBB);

    // Set up current function state
    CurrentFunction = LLVMFunc;
    CurrentBlock = EntryBB;
    LocalVariables.clear();

    // Reset region state for this initializer
    CurrentRegion = RegionInfo{};

    // Bind parameters to names
    auto ArgIt = LLVMFunc->arg_begin();

    // First arg is 'this' - the pointer to the struct
    llvm::Value *ThisPtr = &*ArgIt;
    LocalVariables["this"] = ThisPtr;
    ++ArgIt;

    // If init# was used, second arg is the page pointer
    if (Init.PageParameter) {
        LocalVariables[*Init.PageParameter] = &*ArgIt;
        // Use this as the return page for # allocations within this initializer
        CurrentRegion.ReturnPage = &*ArgIt;
        ++ArgIt;
    }

    // Bind additional parameters
    for (const auto &Item : Init.Input) {
        if (Item.Name) {
            LocalVariables[*Item.Name] = &*ArgIt;
        }
        ++ArgIt;
    }

    // Emit initializer body
    if (auto *Action = std::get_if<PlannedAction>(&Init.Impl)) {
        auto ValueOrErr = emitAction(*Action);
        if (!ValueOrErr) {
            return ValueOrErr.takeError();
        }
    }

    // Add return void if block doesn't end with terminator
    // Use Builder->GetInsertBlock() to get the current block after control flow
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateRetVoid();
    }

    return llvm::Error::success();
}

llvm::Function *Emitter::emitDeInitializerDecl(const PlannedStructure &Struct,
                                                const PlannedDeInitializer &DeInit) {
    // Check cache
    if (auto It = FunctionCache.find(DeInit.MangledName); It != FunctionCache.end()) {
        return It->second;
    }

    // Deinitializer takes only 'this' pointer
    std::vector<llvm::Type*> ParamTypes;

    // Get the struct type
    llvm::Type *StructTy = StructCache[Struct.MangledName];
    if (!StructTy) {
        return nullptr;
    }

    // Only param: pointer to struct (this)
    ParamTypes.push_back(llvm::PointerType::get(*Context, 0));

    // Deinitializer returns void
    auto *FuncType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*Context),
        ParamTypes,
        false);

    auto *LLVMFunc = llvm::Function::Create(
        FuncType,
        llvm::Function::ExternalLinkage,
        DeInit.MangledName,
        Module.get());

    // Set parameter name
    LLVMFunc->arg_begin()->setName("this");

    FunctionCache[DeInit.MangledName] = LLVMFunc;
    return LLVMFunc;
}

llvm::Error Emitter::emitDeInitializerBody(const PlannedStructure &Struct,
                                            const PlannedDeInitializer &DeInit,
                                            llvm::Function *LLVMFunc) {
    // Skip extern/intrinsic deinitializers
    if (std::holds_alternative<PlannedExternImpl>(DeInit.Impl) ||
        std::holds_alternative<PlannedIntrinsicImpl>(DeInit.Impl)) {
        return llvm::Error::success();
    }

    // Skip if deinitializer body already emitted
    if (!LLVMFunc->isDeclaration()) {
        return llvm::Error::success();
    }

    // Create entry block
    auto *EntryBB = llvm::BasicBlock::Create(*Context, "entry", LLVMFunc);
    Builder->SetInsertPoint(EntryBB);

    // Create a dummy instruction as an alloca insertion point marker
    AllocaInsertPt = new llvm::BitCastInst(
        llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*Context)),
        llvm::Type::getInt32Ty(*Context), "alloca.point", EntryBB);

    // Set up current function state
    CurrentFunction = LLVMFunc;
    CurrentBlock = EntryBB;
    LocalVariables.clear();

    // First arg is 'this' - the pointer to the struct
    llvm::Value *ThisPtr = &*LLVMFunc->arg_begin();
    LocalVariables["this"] = ThisPtr;

    // Emit deinitializer body
    if (auto *Action = std::get_if<PlannedAction>(&DeInit.Impl)) {
        auto ValueOrErr = emitAction(*Action);
        if (!ValueOrErr) {
            return ValueOrErr.takeError();
        }
    }

    // Add return void if block doesn't end with terminator
    // Use Builder->GetInsertBlock() to get the current block after control flow
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateRetVoid();
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
            // Look up the variable's pointer
            llvm::Value *VarPtr = lookupVariable(Var->Name);
            if (!VarPtr) {
                return llvm::make_error<llvm::StringError>(
                    "Undefined variable in assignment: " + Var->Name,
                    llvm::inconvertibleErrorCode()
                );
            }

            // Check for member access (e.g., this.x = value)
            if (TargetOp.MemberAccess && !TargetOp.MemberAccess->empty()) {
                // Get the struct type
                llvm::Type *CurrentType = nullptr;
                llvm::Value *CurrentPtr = VarPtr;

                // If VarPtr is an alloca, get its allocated type
                if (auto *Alloca = llvm::dyn_cast<llvm::AllocaInst>(VarPtr)) {
                    CurrentType = Alloca->getAllocatedType();

                    // Handle auto-deref for pointer types:
                    // If the variable is pointer[T], we need to load the pointer value first
                    if (Var->VariableType.isPointer()) {
                        // Load the pointer value from the alloca
                        llvm::Type *PtrType = mapType(Var->VariableType);
                        CurrentPtr = Builder->CreateLoad(PtrType, VarPtr, "deref.ptr");

                        // Get the struct type from the inner type
                        PlannedType InnerType = Var->VariableType.getInnerTypeIfPointer();
                        CurrentType = lookupStructType(InnerType);
                    }
                } else {
                    // For pointers passed as arguments (like 'this'), load to get the struct
                    // Actually, 'this' is already a pointer to the struct
                    // We need to check if it's a pointer type
                    if (VarPtr->getType()->isPointerTy()) {
                        // Get the struct type from cache using the variable type
                        // For 'this' which is pointer[T], use the inner type T
                        PlannedType LookupType = Var->VariableType;
                        if (Var->VariableType.isPointer()) {
                            LookupType = Var->VariableType.getInnerTypeIfPointer();
                        }
                        // Handle NPO Option types: Option[ref[T]] or Option[pointer[T]]
                        // With NPO, these are represented as a single pointer (null = None, non-null = Some)
                        else if (Var->VariableType.Name == "Option" && !Var->VariableType.Generics.empty()) {
                            const PlannedType &Inner = Var->VariableType.Generics[0];
                            if (Inner.isPointerLike()) {
                                // NPO: load the pointer (it's stored directly as a pointer)
                                llvm::Type *PtrType = llvm::PointerType::get(*Context, 0);
                                CurrentPtr = Builder->CreateLoad(PtrType, VarPtr, "option.load");
                                // Get the inner-inner type (T from ref[T] or pointer[T])
                                LookupType = Inner.getInnerTypeIfPointerLike();
                            }
                        }
                        CurrentType = lookupStructType(LookupType);
                        if (!CurrentPtr) CurrentPtr = VarPtr;  // 'this' is already a pointer
                    }
                }

                if (!CurrentType) {
                    return llvm::make_error<llvm::StringError>(
                        "Cannot determine struct type for member access in assignment",
                        llvm::inconvertibleErrorCode()
                    );
                }

                // Navigate through member access chain
                const auto &MemberChain = *TargetOp.MemberAccess;
                for (size_t i = 0; i < MemberChain.size(); ++i) {
                    const auto &Member = MemberChain[i];
                    bool isLastMember = (i == MemberChain.size() - 1);

                    if (!CurrentType->isStructTy()) {
                        return llvm::make_error<llvm::StringError>(
                            "Cannot access member on non-struct type in assignment",
                            llvm::inconvertibleErrorCode()
                        );
                    }
                    CurrentPtr = Builder->CreateStructGEP(CurrentType, CurrentPtr,
                                                           Member.FieldIndex, Member.Name);
                    // Get the field type for the next iteration
                    CurrentType = llvm::cast<llvm::StructType>(CurrentType)
                                      ->getElementType(Member.FieldIndex);

                    // If the LLVM type is a pointer and there are MORE accesses after this,
                    // we need to load through it and get the pointed-to struct type
                    // This handles pointer[T], ref[T], and Option[ref[T]] cases
                    if (!isLastMember && CurrentType->isPointerTy()) {
                        // Use getInnerTypeForMethodLookup which unwraps Option, ref, and pointer
                        PlannedType InnerType = Member.ResultType.getInnerTypeForMethodLookup();
                        llvm::Type *InnerStructType = lookupStructType(InnerType);
                        if (InnerStructType) {
                            // Load the pointer value
                            CurrentPtr = Builder->CreateLoad(CurrentType, CurrentPtr, Member.Name + ".deref");
                            CurrentType = InnerStructType;
                        }
                        // If we can't find the struct type, just continue - the next iteration
                        // will catch the error if there's a real problem
                    }
                }

                // Store the value to the field
                Builder->CreateStore(Value, CurrentPtr);
            } else {
                // Simple variable assignment
                if (!Var->IsMutable) {
                    return llvm::make_error<llvm::StringError>(
                        "Cannot assign to immutable variable: " + Var->Name,
                        llvm::inconvertibleErrorCode()
                    );
                }

                // Store the value to the variable
                Builder->CreateStore(Value, VarPtr);
            }
        } else if (auto *Tuple = std::get_if<PlannedTuple>(&TargetOp.Expr)) {
            // Handle dereferenced pointer assignment like (*ptr).field = value
            // The tuple contains the dereference expression
            if (Tuple->Components.size() == 1 && !Tuple->Components[0].Name) {
                // This is a grouped expression like (*ptr)
                if (!Tuple->Components[0].Value.empty()) {
                    const auto& InnerOp = Tuple->Components[0].Value.back();

                    // Check if this is a dereference call (*ptr)
                    // In that case, we need to get the pointer, not the dereferenced value
                    llvm::Value *Ptr = nullptr;
                    PlannedType PointedType;

                    if (auto *Call = std::get_if<PlannedCall>(&InnerOp.Expr)) {
                        if (Call->Name == "*" && Call->Args && Call->Args->size() == 1) {
                            // This is a dereference - emit the argument to get the pointer
                            auto PtrOrErr = emitOperand((*Call->Args)[0]);
                            if (!PtrOrErr)
                                return PtrOrErr.takeError();
                            Ptr = *PtrOrErr;
                            // The result type of the dereference is the pointed-to type
                            PointedType = InnerOp.ResultType;
                        }
                    }

                    if (!Ptr) {
                        // Not a dereference call, emit normally
                        auto InnerOrErr = emitOperand(InnerOp);
                        if (!InnerOrErr)
                            return InnerOrErr.takeError();
                        Ptr = *InnerOrErr;
                        PointedType = InnerOp.ResultType;
                    }

                    // The inner result should be a pointer to the struct
                    if (!Ptr->getType()->isPointerTy()) {
                        std::string TypeStr;
                        llvm::raw_string_ostream TypeStream(TypeStr);
                        Ptr->getType()->print(TypeStream);
                        return llvm::make_error<llvm::StringError>(
                            "Dereference target is not a pointer (got " + TypeStr +
                            ", PointedType=" + PointedType.Name + ")",
                            llvm::inconvertibleErrorCode()
                        );
                    }

                    // Get the struct type from the pointed-to type
                    // Try multiple key formats due to mangling variations
                    llvm::Type *StructType = nullptr;
                    auto TypeIt = StructCache.find(PointedType.MangledName);
                    if (TypeIt == StructCache.end()) {
                        TypeIt = StructCache.find(PointedType.Name);
                    }
                    if (TypeIt == StructCache.end()) {
                        TypeIt = StructCache.find("_Z" + PointedType.MangledName);
                    }
                    if (TypeIt != StructCache.end()) {
                        StructType = TypeIt->second;
                    }

                    if (!StructType) {
                        return llvm::make_error<llvm::StringError>(
                            "Cannot determine struct type for dereference assignment",
                            llvm::inconvertibleErrorCode()
                        );
                    }

                    // Navigate through member access chain if present
                    llvm::Value *CurrentPtr = Ptr;
                    llvm::Type *CurrentType = StructType;

                    if (TargetOp.MemberAccess && !TargetOp.MemberAccess->empty()) {
                        const auto &MemberChain = *TargetOp.MemberAccess;
                        for (size_t i = 0; i < MemberChain.size(); ++i) {
                            const auto &Member = MemberChain[i];
                            bool isLastMember = (i == MemberChain.size() - 1);

                            if (!CurrentType->isStructTy()) {
                                return llvm::make_error<llvm::StringError>(
                                    "Cannot access member on non-struct type in dereference assignment",
                                    llvm::inconvertibleErrorCode()
                                );
                            }
                            CurrentPtr = Builder->CreateStructGEP(CurrentType, CurrentPtr,
                                                                   Member.FieldIndex, Member.Name);
                            CurrentType = llvm::cast<llvm::StructType>(CurrentType)
                                              ->getElementType(Member.FieldIndex);

                            // If the LLVM type is a pointer and there are MORE accesses after this,
                            // we need to load through it and get the pointed-to struct type
                            // This handles pointer[T], ref[T], and Option[ref[T]] cases
                            if (!isLastMember && CurrentType->isPointerTy()) {
                                // Use getInnerTypeForMethodLookup which unwraps Option, ref, and pointer
                                PlannedType InnerType = Member.ResultType.getInnerTypeForMethodLookup();
                                llvm::Type *InnerStructType = lookupStructType(InnerType);
                                if (InnerStructType) {
                                    // Load the pointer value
                                    CurrentPtr = Builder->CreateLoad(CurrentType, CurrentPtr, Member.Name + ".deref");
                                    CurrentType = InnerStructType;
                                }
                                // If we can't find the struct type, just continue - the next iteration
                                // will catch the error if there's a real problem
                            }
                        }
                    }

                    // Store the value
                    Builder->CreateStore(Value, CurrentPtr);
                }
            }
        } else if (auto *Call = std::get_if<PlannedCall>(&TargetOp.Expr)) {
            // Handle direct dereference assignment like *ptr = value (no parentheses)
            if (Call->Name == "*" && Call->Args && Call->Args->size() == 1) {
                // Emit the argument to get the pointer
                auto PtrOrErr = emitOperand((*Call->Args)[0]);
                if (!PtrOrErr)
                    return PtrOrErr.takeError();
                llvm::Value *Ptr = *PtrOrErr;

                if (!Ptr->getType()->isPointerTy()) {
                    std::string TypeStr, ArgTypeName;
                    llvm::raw_string_ostream TypeStream(TypeStr);
                    Ptr->getType()->print(TypeStream);
                    if ((*Call->Args)[0].ResultType.Name.empty()) {
                        ArgTypeName = "(empty)";
                    } else {
                        ArgTypeName = (*Call->Args)[0].ResultType.Name;
                    }
                    std::string FuncName = CurrentFunction ? CurrentFunction->getName().str() : "(unknown)";
                    return llvm::make_error<llvm::StringError>(
                        "Dereference target is not a pointer (got " + TypeStr +
                        ", ArgResultType=" + ArgTypeName + ", in function " + FuncName + ")",
                        llvm::inconvertibleErrorCode()
                    );
                }

                llvm::Value *CurrentPtr = Ptr;

                // Navigate through member access chain if present
                if (TargetOp.MemberAccess && !TargetOp.MemberAccess->empty()) {
                    PlannedType PointedType = TargetOp.ResultType;
                    // Get the initial struct type (before member access)
                    // Since we're dereferencing a pointer[Struct], look up Struct from first element's type
                    PlannedType InitialPointedType = (*Call->Args)[0].ResultType;
                    if (!InitialPointedType.Generics.empty()) {
                        InitialPointedType = InitialPointedType.Generics[0];  // pointer[T] -> T
                    }

                    llvm::Type *CurrentType = nullptr;
                    auto TypeIt = StructCache.find(InitialPointedType.MangledName);
                    if (TypeIt == StructCache.end()) {
                        TypeIt = StructCache.find(InitialPointedType.Name);
                    }
                    if (TypeIt != StructCache.end()) {
                        CurrentType = TypeIt->second;
                    }

                    if (!CurrentType || !CurrentType->isStructTy()) {
                        return llvm::make_error<llvm::StringError>(
                            "Cannot determine struct type for dereference assignment",
                            llvm::inconvertibleErrorCode()
                        );
                    }

                    for (const auto &Member : *TargetOp.MemberAccess) {
                        CurrentPtr = Builder->CreateStructGEP(CurrentType, CurrentPtr,
                                                               Member.FieldIndex, Member.Name);
                        CurrentType = llvm::cast<llvm::StructType>(CurrentType)
                                          ->getElementType(Member.FieldIndex);
                    }
                }

                // Store the value to the pointer target
                Builder->CreateStore(Value, CurrentPtr);
            } else {
                return llvm::make_error<llvm::StringError>(
                    "Assignment target must be a variable or dereference",
                    llvm::inconvertibleErrorCode()
                );
            }
        } else {
            // Debug: what type is the target?
            std::string TypeName = std::visit([](auto &&arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, PlannedConstant>) return "PlannedConstant";
                else if constexpr (std::is_same_v<T, PlannedType>) return "PlannedType";
                else if constexpr (std::is_same_v<T, PlannedVariable>) return "PlannedVariable";
                else if constexpr (std::is_same_v<T, PlannedTuple>) return "PlannedTuple";
                else if constexpr (std::is_same_v<T, PlannedCall>) {
                    return "PlannedCall(" + arg.Name + ")";
                }
                else if constexpr (std::is_same_v<T, PlannedBlock>) return "PlannedBlock";
                else if constexpr (std::is_same_v<T, PlannedMatrix>) {
                    return "PlannedMatrix(" + std::to_string(arg.Operations.size()) + " rows)";
                }
                else return "Unknown";
            }, TargetOp.Expr);
            return llvm::make_error<llvm::StringError>(
                "Assignment target must be a variable (got " + TypeName +
                    " at offset " + std::to_string(TargetOp.Loc.Start) + ")",
                llvm::inconvertibleErrorCode()
            );
        }
    }

    return Value;
}

llvm::Error Emitter::emitBinding(const PlannedBinding &Binding) {
    llvm::Value *Value = nullptr;

    std::string BindingName = Binding.BindingItem.Name ? *Binding.BindingItem.Name : "<unnamed>";
    // Emit initialization expression if present
    if (!Binding.Operation.empty()) {
        auto ValueOrErr = emitOperands(Binding.Operation);
        if (!ValueOrErr) {
            return ValueOrErr.takeError();
        }

        Value = *ValueOrErr;
        if (!Value) {
            return llvm::make_error<llvm::StringError>(
                "Binding initialization expression produced null value",
                llvm::inconvertibleErrorCode()
            );
        }
    }

    // Create alloca for the binding
    if (Binding.BindingItem.Name) {
        if (Value) {
            llvm::Type *Ty = Value->getType();
            if (Binding.BindingType == "var" || Binding.BindingType == "mutable") {
                // Mutable binding: allocate on stack
                auto *Alloca = createEntryBlockAlloca(Ty, *Binding.BindingItem.Name);
                Builder->CreateStore(Value, Alloca);
                LocalVariables[*Binding.BindingItem.Name] = Alloca;
            } else {
                // Immutable binding: just use the value directly
                LocalVariables[*Binding.BindingItem.Name] = Value;
            }
        } else if (Binding.BindingItem.ItemType) {
            // No initialization but has type annotation - allocate uninitialized
            const PlannedType& ItemType = *Binding.BindingItem.ItemType;

            // Check for stack array: pointer[T] with ArraySize set (must be numeric)
            bool IsStackArray = false;
            uint64_t ArraySizeVal = 0;
            if (!ItemType.ArraySize.empty() && ItemType.Name == "pointer" &&
                !ItemType.Generics.empty()) {
                // Try to parse ArraySize as a number
                try {
                    ArraySizeVal = std::stoull(ItemType.ArraySize);
                    IsStackArray = true;
                } catch (const std::exception&) {
                    // Not a numeric size - treat as regular pointer type
                }
            }

            if (IsStackArray) {
                // Stack array declaration: var buffer char[64]
                // Allocate [ArraySize x ElementType] on the stack
                llvm::Type *ElemTy = mapType(ItemType.Generics[0]);
                llvm::ArrayType *ArrTy = llvm::ArrayType::get(ElemTy, ArraySizeVal);
                auto *ArrayAlloca = createEntryBlockAlloca(ArrTy, *Binding.BindingItem.Name + ".arr");
                // Get pointer to the first element (array decay semantics)
                llvm::Value *Indices[] = {
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 0),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 0)
                };
                llvm::Value *FirstElemPtr = Builder->CreateGEP(ArrTy, ArrayAlloca, Indices,
                                                                *Binding.BindingItem.Name + ".ptr");
                // Store the pointer in an alloca so mutable variable access works correctly
                // (mutable variables are accessed via load from their alloca)
                auto *PtrAlloca = createEntryBlockAlloca(Builder->getPtrTy(), *Binding.BindingItem.Name);
                Builder->CreateStore(FirstElemPtr, PtrAlloca);
                LocalVariables[*Binding.BindingItem.Name] = PtrAlloca;
            } else {
                // Regular type - map and allocate
                llvm::Type *Ty = mapType(ItemType);
                auto *Alloca = createEntryBlockAlloca(Ty, *Binding.BindingItem.Name);
                LocalVariables[*Binding.BindingItem.Name] = Alloca;
            }
        } else {
            return llvm::make_error<llvm::StringError>(
                "Binding has no initialization expression and no type annotation",
                llvm::inconvertibleErrorCode()
            );
        }
    }

    return llvm::Error::success();
}

llvm::Error Emitter::emitReturn(const PlannedReturn &Return) {
    // Evaluate return value first (before cleanup)
    llvm::Value *RetVal = nullptr;
    if (!Return.Result.empty()) {
        auto ValueOrErr = emitOperands(Return.Result);
        if (!ValueOrErr)
            return ValueOrErr.takeError();
        RetVal = *ValueOrErr;
    }

    // Emit block-scoped cleanups for all pending blocks
    emitBlockCleanups(0);

    // Clean up local page before returning
    // Only call deallocate_extensions if the page was actually used for allocations
    if (CurrentRegion.LocalPage) {
        // Check if the page was used for allocations by comparing next_object to page + 1
        // A fresh page has next_object = page + 1 (after reset), so if unchanged, no allocations were made
        // This avoids infinite recursion: deallocate_extensions -> get_iterator -> deallocate_extensions
        llvm::Value *NextObjectPtr = Builder->CreateStructGEP(PageType, CurrentRegion.LocalPage, 0, "next_object_ptr");
        llvm::Value *NextObject = Builder->CreateLoad(Builder->getPtrTy(), NextObjectPtr, "next_object");
        llvm::Value *PagePlus1 = Builder->CreateGEP(PageType, CurrentRegion.LocalPage,
                                                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 1),
                                                    "page_plus_1");
        llvm::Value *WasUsed = Builder->CreateICmpNE(NextObject, PagePlus1, "page_was_used");

        llvm::BasicBlock *CleanupBlock = llvm::BasicBlock::Create(*Context, "cleanup", CurrentFunction);
        llvm::BasicBlock *ContinueBlock = llvm::BasicBlock::Create(*Context, "continue", CurrentFunction);
        Builder->CreateCondBr(WasUsed, CleanupBlock, ContinueBlock);

        Builder->SetInsertPoint(CleanupBlock);
        Builder->CreateCall(PageDeallocateExtensions, {CurrentRegion.LocalPage});
        Builder->CreateBr(ContinueBlock);

        Builder->SetInsertPoint(ContinueBlock);
        // Always free the local page (even if no extensions)
        if (auto *FreeFn = Module->getFunction("free")) {
            Builder->CreateCall(FreeFn, {CurrentRegion.LocalPage});
        }
    }

    // Check if function uses sret (struct return via first parameter)
    // If so, store to the sret param and return void
    bool HasSret = CurrentFunction && CurrentFunction->arg_size() > 0 &&
                   CurrentFunction->hasParamAttribute(0, llvm::Attribute::StructRet);
    if (HasSret) {
        llvm::Argument *SRetArg = CurrentFunction->getArg(0);
        if (RetVal) {
            // For struct returns, store the value to the sret pointer
            if (RetVal->getType()->isStructTy()) {
                Builder->CreateStore(RetVal, SRetArg);
            } else {
                // If returning a pointer or scalar that should populate a struct,
                // we need to handle it properly. For PageListIterator(head), the
                // value is a pointer that should be stored as the first field.
                llvm::Type *SRetTy = CurrentFunction->getParamStructRetType(0);
                if (SRetTy && SRetTy->isStructTy()) {
                    llvm::Value *FieldPtr = Builder->CreateStructGEP(
                        SRetTy, SRetArg, 0, "sret.field");
                    Builder->CreateStore(RetVal, FieldPtr);
                } else {
                    Builder->CreateStore(RetVal, SRetArg);
                }
            }
        }
        Builder->CreateRetVoid();
    } else {
        // Regular return
        if (RetVal) {
            llvm::Type *ExpectedRetTy = CurrentFunction->getReturnType();
            llvm::Type *ActualTy = RetVal->getType();

            // Handle type mismatch between return value and expected return type
            if (ActualTy != ExpectedRetTy) {
                if (ExpectedRetTy->isIntegerTy() && ActualTy->isIntegerTy()) {
                    // Integer type mismatch - truncate or extend
                    unsigned ExpectedBits = ExpectedRetTy->getIntegerBitWidth();
                    unsigned ActualBits = ActualTy->getIntegerBitWidth();
                    if (ExpectedBits < ActualBits) {
                        RetVal = Builder->CreateTrunc(RetVal, ExpectedRetTy, "ret.trunc");
                    } else {
                        RetVal = Builder->CreateZExt(RetVal, ExpectedRetTy, "ret.zext");
                    }
                } else if (ExpectedRetTy->isIntegerTy() && ActualTy->isStructTy()) {
                    // Returning struct but function expects integer
                    // Find the field with matching integer type
                    llvm::StructType *StructTy = llvm::cast<llvm::StructType>(ActualTy);
                    bool Found = false;
                    for (unsigned i = 0; i < StructTy->getNumElements(); ++i) {
                        llvm::Type *FieldTy = StructTy->getElementType(i);
                        if (FieldTy == ExpectedRetTy) {
                            RetVal = Builder->CreateExtractValue(RetVal, {i}, "ret.extract");
                            Found = true;
                            break;
                        }
                    }
                    if (!Found) {
                        // Try to find any integer field and cast it
                        for (unsigned i = 0; i < StructTy->getNumElements(); ++i) {
                            llvm::Type *FieldTy = StructTy->getElementType(i);
                            if (FieldTy->isIntegerTy()) {
                                RetVal = Builder->CreateExtractValue(RetVal, {i}, "ret.extract");
                                unsigned ExpectedBits = ExpectedRetTy->getIntegerBitWidth();
                                unsigned ActualBits = FieldTy->getIntegerBitWidth();
                                if (ExpectedBits < ActualBits) {
                                    RetVal = Builder->CreateTrunc(RetVal, ExpectedRetTy, "ret.trunc");
                                } else if (ExpectedBits > ActualBits) {
                                    RetVal = Builder->CreateZExt(RetVal, ExpectedRetTy, "ret.zext");
                                }
                                Found = true;
                                break;
                            }
                        }
                    }
                } else if (ExpectedRetTy->isPointerTy() && ActualTy->isStructTy()) {
                    // Returning struct but function expects pointer
                    // Allocate the struct on the return page (if available) or stack
                    llvm::Value *StructPtr;
                    if (CurrentRegion.ReturnPage) {
                        auto &DL = Module->getDataLayout();
                        uint64_t Size = DL.getTypeAllocSize(ActualTy);
                        uint64_t Align = DL.getABITypeAlign(ActualTy).value();
                        StructPtr = Builder->CreateCall(PageAllocate,
                            {CurrentRegion.ReturnPage,
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Size),
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Align)},
                            "ret.alloc");
                    } else {
                        StructPtr = createEntryBlockAlloca(ActualTy, "ret.alloca");
                    }
                    Builder->CreateStore(RetVal, StructPtr);
                    RetVal = StructPtr;
                } else if (ExpectedRetTy->isPointerTy() && ActualTy->isArrayTy()) {
                    // Returning array but function expects pointer
                    llvm::Value *ArrayPtr;
                    if (CurrentRegion.ReturnPage) {
                        auto &DL = Module->getDataLayout();
                        uint64_t Size = DL.getTypeAllocSize(ActualTy);
                        uint64_t Align = DL.getABITypeAlign(ActualTy).value();
                        ArrayPtr = Builder->CreateCall(PageAllocate,
                            {CurrentRegion.ReturnPage,
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Size),
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), Align)},
                            "ret.alloc");
                    } else {
                        ArrayPtr = createEntryBlockAlloca(ActualTy, "ret.alloca");
                    }
                    Builder->CreateStore(RetVal, ArrayPtr);
                    RetVal = ArrayPtr;
                } else if (ExpectedRetTy->isStructTy() && ActualTy->isPointerTy()) {
                    // Returning pointer but function expects struct by value
                    // Load the struct from the pointer
                    RetVal = Builder->CreateLoad(ExpectedRetTy, RetVal, "ret.load");
                } else if ((ExpectedRetTy->isIntegerTy() || ExpectedRetTy->isFloatTy() ||
                            ExpectedRetTy->isDoubleTy()) && ActualTy->isPointerTy()) {
                    // Returning pointer but function expects primitive type
                    // Load the value from the pointer
                    RetVal = Builder->CreateLoad(ExpectedRetTy, RetVal, "ret.load");
                }
            }
            Builder->CreateRet(RetVal);
        } else {
            Builder->CreateRetVoid();
        }
    }
    return llvm::Error::success();
}

llvm::Error Emitter::emitThrow(const PlannedThrow &Throw) {
    // Throw creates an error Result and returns it
    // Result layout: { i8 tag, [size x i8] data }
    // Tag 0 = Ok, Tag 1+ = error variants

    if (!CurrentFunction) {
        return llvm::make_error<llvm::StringError>(
            "throw statement outside of function",
            llvm::inconvertibleErrorCode()
        );
    }

    // Check if function uses sret (returns struct via first parameter)
    // or returns directly
    llvm::Value *ResultPtr = nullptr;
    llvm::StructType *ResultTy = nullptr;

    // Check for sret parameter (first param with sret attribute)
    if (CurrentFunction->arg_size() > 0 &&
        CurrentFunction->hasParamAttribute(0, llvm::Attribute::StructRet)) {
        // sret: result is written to first parameter
        ResultPtr = CurrentFunction->getArg(0);
        // Get the struct type from the sret parameter's pointee type
        llvm::Type *ParamTy = CurrentFunction->getParamStructRetType(0);
        if (!ParamTy || !ParamTy->isStructTy()) {
            return llvm::make_error<llvm::StringError>(
                "throw: sret parameter is not a struct type",
                llvm::inconvertibleErrorCode()
            );
        }
        ResultTy = llvm::cast<llvm::StructType>(ParamTy);
    } else {
        // Direct return: check return type
        llvm::Type *RetTy = CurrentFunction->getReturnType();
        if (!RetTy->isStructTy()) {
            std::string TyStr;
            llvm::raw_string_ostream OS(TyStr);
            RetTy->print(OS);
            return llvm::make_error<llvm::StringError>(
                "throw in function that doesn't return a Result type",
                llvm::inconvertibleErrorCode()
            );
        }
        ResultTy = llvm::cast<llvm::StructType>(RetTy);
        // Allocate space for the Result
        ResultPtr = createEntryBlockAlloca(ResultTy, "throw.result");
    }

    // Set the tag to 1 (first error variant)
    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
    llvm::Value *TagPtr = Builder->CreateStructGEP(ResultTy, ResultPtr, 0, "throw.tag.ptr");
    Builder->CreateStore(llvm::ConstantInt::get(I8Ty, 1), TagPtr);

    // Evaluate the thrown value and store in the data portion
    if (!Throw.Result.empty()) {
        auto ValueOrErr = emitOperands(Throw.Result);
        if (!ValueOrErr)
            return ValueOrErr.takeError();

        llvm::Value *ThrownValue = *ValueOrErr;

        // Get pointer to the data field (field 1)
        llvm::Value *DataPtr = Builder->CreateStructGEP(ResultTy, ResultPtr, 1, "throw.data.ptr");

        // Check if thrown value is a pointer to a struct (from ! lifetime allocation)
        // If so, copy the struct into the union data, not the pointer
        llvm::Type *ThrownTy = ThrownValue->getType();
        if (ThrownTy->isPointerTy()) {
            // Load the struct from the pointer and store it
            // We need to determine the struct type - check the Result union's data size
            // For now, load as the struct type that fits in the data portion
            llvm::Type *DataFieldTy = ResultTy->getStructElementType(1);
            if (DataFieldTy->isArrayTy()) {
                // Data is [N x i8] - we have N bytes available
                // Copy from the pointer source to the data destination
                auto *I64Ty = llvm::Type::getInt64Ty(*Context);
                size_t DataSize = DataFieldTy->getArrayNumElements();
                Builder->CreateMemCpy(DataPtr, llvm::MaybeAlign(1),
                                      ThrownValue, llvm::MaybeAlign(1),
                                      llvm::ConstantInt::get(I64Ty, DataSize));
            } else {
                // Fallback: store the pointer directly
                llvm::Value *DataCast = Builder->CreateBitCast(
                    DataPtr,
                    llvm::PointerType::getUnqual(ThrownTy),
                    "throw.data.cast"
                );
                Builder->CreateStore(ThrownValue, DataCast);
            }
        } else {
            // Non-pointer value: store directly
            llvm::Value *DataCast = Builder->CreateBitCast(
                DataPtr,
                llvm::PointerType::getUnqual(ThrownTy),
                "throw.data.cast"
            );
            Builder->CreateStore(ThrownValue, DataCast);
        }
    }

    // Emit block-scoped cleanups for all pending blocks
    emitBlockCleanups(0);

    // Return (void for sret, value for direct return)
    if (CurrentFunction->hasParamAttribute(0, llvm::Attribute::StructRet)) {
        Builder->CreateRetVoid();
    } else {
        llvm::Value *Result = Builder->CreateLoad(ResultTy, ResultPtr, "throw.val");
        Builder->CreateRet(Result);
    }

    return llvm::Error::success();
}

llvm::Error Emitter::emitBreak(const PlannedBreak &Break) {
    if (LoopStack.empty()) {
        return llvm::make_error<llvm::StringError>(
            "break statement outside of loop",
            llvm::inconvertibleErrorCode()
        );
    }

    // Emit block-scoped cleanups back to the loop's entry depth
    emitBlockCleanups(LoopStack.back().BlockCleanupDepth);

    // Jump to the exit block of the innermost loop
    Builder->CreateBr(LoopStack.back().ExitBlock);
    return llvm::Error::success();
}

llvm::Error Emitter::emitContinue(const PlannedContinue &Continue) {
    if (LoopStack.empty()) {
        return llvm::make_error<llvm::StringError>(
            "continue statement outside of loop",
            llvm::inconvertibleErrorCode()
        );
    }

    // Emit block-scoped cleanups back to the loop's entry depth
    emitBlockCleanups(LoopStack.back().BlockCleanupDepth);

    // Jump to the continue block of the innermost loop
    // For 'for' loops, this is the increment block
    // For 'while' loops, this is the condition block
    Builder->CreateBr(LoopStack.back().ContinueBlock);
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

    // After collapseOperandSequence, we typically have a single collapsed operand
    // that contains the full expression tree (calls, operators, is expressions, etc.)
    return emitOperand(Ops[0]);
}

llvm::Expected<llvm::Value*> Emitter::emitOperand(const PlannedOperand &Op) {
    llvm::Value *Value = nullptr;

    // Handle constants specially since they need type info
    if (auto *Const = std::get_if<PlannedConstant>(&Op.Expr)) {
        Value = emitConstant(*Const, Op.ResultType);
    } else {
        auto ExprResult = emitExpression(Op.Expr);
        if (!ExprResult)
            return ExprResult.takeError();
        Value = *ExprResult;
    }

    // Apply member access chain if present
    if (Op.MemberAccess && !Op.MemberAccess->empty() && Value) {
        // If Value is a pointer to a struct, we need to load it first for extractvalue
        if (Value->getType()->isPointerTy()) {
            // Get the type to load based on the expression type
            PlannedType BaseType;
            if (auto *Var = std::get_if<PlannedVariable>(&Op.Expr)) {
                BaseType = Var->VariableType;
            } else if (auto *Call = std::get_if<PlannedCall>(&Op.Expr)) {
                BaseType = Call->ResultType;
            } else if (auto *GlobalRef = std::get_if<PlannedGlobalRef>(&Op.Expr)) {
                BaseType = GlobalRef->GlobalType;
            }

            // If we have a base type and it's a pointer, get the element type
            if (!BaseType.Name.empty()) {
                if (BaseType.Name == "pointer" && !BaseType.Generics.empty()) {
                    // Load the pointed-to struct
                    auto &InnerType = BaseType.Generics[0];
                    llvm::Type *StructTy = mapType(InnerType);
                    Value = Builder->CreateLoad(StructTy, Value, "load.struct");
                } else {
                    // Load the struct directly
                    llvm::Type *StructTy = mapType(BaseType);
                    Value = Builder->CreateLoad(StructTy, Value, "load.struct");
                }
            }
        }

        // Now apply member accesses - but handle pointer returns from extractvalue
        for (const auto &Access : *Op.MemberAccess) {
            // Handle zero-arg method calls (e.g., string.length, list.get_iterator)
            if (Access.IsZeroArgMethodCall) {
                // Look up the method and call it with current value as 'this'
                std::string MethodName = Access.ParentType.Name + "." + Access.Name;
                std::string MangledMethodName = "_Z" + std::to_string(Access.ParentType.Name.size()) +
                                                Access.ParentType.Name + std::to_string(Access.Name.size()) +
                                                Access.Name + "E";

                llvm::Function *MethodFunc = nullptr;
                // Try to find in current module first
                if (Module) {
                    MethodFunc = Module->getFunction(MangledMethodName);
                }
                // Try function cache if not in module
                if (!MethodFunc) {
                    if (auto It = FunctionCache.find(MangledMethodName); It != FunctionCache.end()) {
                        MethodFunc = It->second;
                    }
                }
                // Try to find by readable name
                if (!MethodFunc && Module) {
                    for (auto &F : *Module) {
                        if (F.getName().contains(Access.Name)) {
                            // Check if this function takes the right type as first arg
                            if (F.arg_size() >= 1) {
                                MethodFunc = &F;
                                break;
                            }
                        }
                    }
                }

                if (MethodFunc) {
                    // Call the method with Value as 'this'
                    // For by-value 'this', the struct is passed directly
                    // For by-ref 'this', we need to pass a pointer
                    llvm::Value *ThisArg = Value;

                    // Check if function expects a pointer for this
                    if (MethodFunc->arg_size() > 0) {
                        llvm::Type *ExpectedThisType = MethodFunc->getArg(0)->getType();
                        if (ExpectedThisType->isPointerTy() && !Value->getType()->isPointerTy()) {
                            // Need to store to temp alloca and pass pointer
                            llvm::AllocaInst *TempAlloca = createEntryBlockAlloca(Value->getType(), "this.tmp");
                            Builder->CreateStore(Value, TempAlloca);
                            ThisArg = TempAlloca;
                        }
                    }

                    Value = Builder->CreateCall(MethodFunc, {ThisArg}, Access.Name + ".result");
                    continue;
                } else {
                    return llvm::make_error<llvm::StringError>(
                        "Cannot find method '" + Access.Name + "' on type '" + Access.ParentType.Name + "'",
                        llvm::inconvertibleErrorCode()
                    );
                }
            }

            // Check if Value is still a pointer (e.g., when extracting a pointer field)
            if (Value->getType()->isPointerTy()) {
                // Need to load the struct first - use ParentType (the struct being accessed)
                llvm::Type *StructTy = mapType(Access.ParentType);
                if (!StructTy) {
                    return llvm::make_error<llvm::StringError>(
                        "Cannot map parent type for member access '" + Access.Name + "'",
                        llvm::inconvertibleErrorCode()
                    );
                }
                Value = Builder->CreateLoad(StructTy, Value, "deref");
            }

            // Handle union .value extraction specially
            if (Access.IsUnionValue) {
                // Union layout is { i8 tag, [size x i8] data }
                // We need to store to alloca, GEP to field 1, bitcast, and load
                llvm::Type *UnionTy = mapType(Access.ParentType);
                llvm::Type *ResultTy = mapType(Access.ResultType);

                // Store the union value to an alloca
                llvm::AllocaInst *UnionPtr = createEntryBlockAlloca(UnionTy, "union.tmp");
                Builder->CreateStore(Value, UnionPtr);

                // Get pointer to field 1 (the data field)
                llvm::Value *DataPtr = Builder->CreateStructGEP(UnionTy, UnionPtr, 1, "union.data.ptr");

                // Bitcast to the result type pointer
                llvm::Value *DataCast = Builder->CreateBitCast(
                    DataPtr,
                    llvm::PointerType::getUnqual(ResultTy),
                    "union.data.cast"
                );

                // Load the value
                Value = Builder->CreateLoad(ResultTy, DataCast, "union.value");
                continue;
            }

            // Ensure Value is an aggregate type before extracting
            llvm::Type *ValTy = Value->getType();
            if (!ValTy->isAggregateType()) {
                std::string TypeStr;
                llvm::raw_string_ostream OS(TypeStr);
                ValTy->print(OS);
                return llvm::make_error<llvm::StringError>(
                    "Cannot apply member access '" + Access.Name + "' to non-aggregate type",
                    llvm::inconvertibleErrorCode()
                );
            }

            // Additional validation for struct types
            if (auto *STy = llvm::dyn_cast<llvm::StructType>(ValTy)) {
                if (Access.FieldIndex >= STy->getNumElements()) {
                    return llvm::make_error<llvm::StringError>(
                        "Field index " + std::to_string(Access.FieldIndex) +
                        " out of bounds for member access '" + Access.Name +
                        "' (struct has " + std::to_string(STy->getNumElements()) + " elements)",
                        llvm::inconvertibleErrorCode()
                    );
                }
            }

            // Use extractvalue for struct field access
            // extractvalue takes a value (not pointer) and field index
            Value = Builder->CreateExtractValue(Value, Access.FieldIndex,
                                                 Access.Name);
        }
    }

    return Value;
}

llvm::Expected<llvm::Value*> Emitter::emitExpression(const PlannedExpression &Expr) {
    return std::visit([this](const auto &E) -> llvm::Expected<llvm::Value*> {
        using T = std::decay_t<decltype(E)>;
        if constexpr (std::is_same_v<T, PlannedConstant>) {
            // Fallback: type info not available, use defaults
            return emitConstant(E, PlannedType{});
        } else if constexpr (std::is_same_v<T, PlannedType>) {
            // Type used as expression value - this shouldn't normally occur
            // sizeof is handled via PlannedSizeOf, constructors via PlannedCall
            return llvm::make_error<llvm::StringError>(
                "Type '" + E.Name + "' cannot be used as a value (offset " +
                    std::to_string(E.Loc.Start) + ")",
                llvm::inconvertibleErrorCode()
            );
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
        } else if constexpr (std::is_same_v<T, PlannedGlobalRef>) {
            // Global constant reference - look up in module and return pointer to first element
            // Use getNamedGlobal instead of getGlobalVariable (the latter doesn't
            // work for PrivateLinkage globals in LLVM 18)
            auto *GV = Module->getNamedGlobal(E.MangledName);
            if (!GV) {
                return llvm::make_error<llvm::StringError>(
                    "Undefined global: " + E.Name,
                    llvm::inconvertibleErrorCode()
                );
            }
            // Get pointer to first element of the array
            llvm::Value *Indices[] = {
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 0),
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(*Context), 0)
            };
            return Builder->CreateGEP(GV->getValueType(), GV, Indices, "global.elem");
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
        } else if constexpr (std::is_same_v<T, PlannedAlignOf>) {
            return emitAlignOf(E);
        } else if constexpr (std::is_same_v<T, PlannedIs>) {
            return emitIs(E);
        } else if constexpr (std::is_same_v<T, PlannedAs>) {
            return emitAs(E);
        } else if constexpr (std::is_same_v<T, PlannedVariantConstruction>) {
            return emitVariantConstruction(E);
        } else {
            return llvm::make_error<llvm::StringError>(
                "Unimplemented expression type",
                llvm::inconvertibleErrorCode()
            );
        }
    }, Expr);
}

llvm::Expected<llvm::Value*> Emitter::emitCall(const PlannedCall &Call) {
    // Look up the function first to know parameter types
    auto *Func = lookupFunction(Call.MangledName);

    // Check if this is an initializer call (constructor)
    // Initializers have mangled names like _ZN<type>C1E<params>
    bool IsInitializerCall = Call.MangledName.find("C1E") != std::string::npos &&
                             Call.MangledName.rfind("_ZN", 0) == 0;

    // Emit arguments, optimizing dereference-to-pointer conversions
    std::vector<llvm::Value*> Args;
    if (Call.Args) {
        auto *FuncTy = Func ? Func->getFunctionType() : nullptr;
        size_t ParamOffset = 0;

        // Check if function uses sret (return via first pointer param)
        if (Func && Func->arg_size() > 0 &&
            Func->hasParamAttribute(0, llvm::Attribute::StructRet)) {
            ParamOffset = 1;  // Skip sret param when matching args to params
        }

        // For initializer calls, account for 'this' and possibly 'page' params
        // when matching Call.Args indices to function parameter indices
        if (IsInitializerCall && Func) {
            ParamOffset += 1;  // Skip 'this' param
            if (Call.RequiresPageParam) {
                ParamOffset += 1;  // Skip 'page' param for init#
            }
        }

        for (size_t i = 0; i < Call.Args->size(); ++i) {
            const auto &Arg = (*Call.Args)[i];

            // Check if this argument is a dereference call (*ptr) and the
            // function expects a pointer - if so, pass the pointer directly
            // instead of loading the struct and creating a copy
            bool PassedPointerDirectly = false;
            if (FuncTy && i + ParamOffset < FuncTy->getNumParams()) {
                llvm::Type *ParamTy = FuncTy->getParamType(i + ParamOffset);
                if (ParamTy->isPointerTy()) {
                    // Check if argument is a dereference call directly
                    if (auto *DerefCall = std::get_if<PlannedCall>(&Arg.Expr)) {
                        if (DerefCall->Name == "*" && DerefCall->Args &&
                            DerefCall->Args->size() == 1) {
                            // This is *ptr and function expects pointer
                            // Emit the pointer argument directly
                            auto PtrVal = emitOperand((*DerefCall->Args)[0]);
                            if (!PtrVal)
                                return PtrVal.takeError();
                            Args.push_back(*PtrVal);
                            PassedPointerDirectly = true;
                        }
                    }
                    // Check if argument is a tuple wrapping a dereference: (*ptr)
                    else if (auto *Tuple = std::get_if<PlannedTuple>(&Arg.Expr)) {
                        if (Tuple->Components.size() == 1 && !Tuple->Components[0].Name &&
                            !Tuple->Components[0].Value.empty()) {
                            const auto& InnerOp = Tuple->Components[0].Value.back();
                            if (auto *DerefCall = std::get_if<PlannedCall>(&InnerOp.Expr)) {
                                if (DerefCall->Name == "*" && DerefCall->Args &&
                                    DerefCall->Args->size() == 1) {
                                    // This is (*ptr) and function expects pointer
                                    // Emit the pointer argument directly
                                    auto PtrVal = emitOperand((*DerefCall->Args)[0]);
                                    if (!PtrVal)
                                        return PtrVal.takeError();
                                    Args.push_back(*PtrVal);
                                    PassedPointerDirectly = true;
                                }
                            }
                        }
                    }
                }
            }

            if (!PassedPointerDirectly) {
                auto ArgVal = emitOperand(Arg);
                if (!ArgVal)
                    return ArgVal.takeError();
                llvm::Value *FinalArg = *ArgVal;

                // Coerce argument type to match parameter type if needed
                if (FuncTy && i + ParamOffset < FuncTy->getNumParams()) {
                    llvm::Type *ParamTy = FuncTy->getParamType(i + ParamOffset);
                    llvm::Type *ArgTy = FinalArg->getType();

                    if (ParamTy != ArgTy) {
                        // Integer type widening (e.g., i32 -> i64)
                        if (ParamTy->isIntegerTy() && ArgTy->isIntegerTy()) {
                            unsigned ParamBits = ParamTy->getIntegerBitWidth();
                            unsigned ArgBits = ArgTy->getIntegerBitWidth();
                            if (ParamBits > ArgBits) {
                                // Widen: use sext for signed, zext for unsigned
                                // Default to sext for safety (int literals are signed)
                                FinalArg = Builder->CreateSExt(FinalArg, ParamTy);
                            } else if (ParamBits < ArgBits) {
                                // Narrow: truncate
                                FinalArg = Builder->CreateTrunc(FinalArg, ParamTy);
                            }
                        }
                    }
                }

                Args.push_back(FinalArg);
            }
        }
    }

    // Handle intrinsic operators
    if (Call.IsIntrinsic && Call.IsOperator) {
        if (Args.size() == 2) {
            // Binary operator
            return emitIntrinsicOp(Call.Name, Args[0], Args[1], Call.ResultType);
        } else if (Args.size() == 1) {
            // Unary operator - but handle address-of specially
            if (Call.Name == "&") {
                // Address-of operator: we need to get a pointer, not a loaded value
                // The argument was already emitted, so we need to check its type
                llvm::Value *Operand = Args[0];

                if (Operand->getType()->isPointerTy()) {
                    // Already a pointer (from variable or GEP) - return it
                    return Operand;
                } else {
                    // The value was loaded - we need to store it and return the address
                    // This happens when address-of is applied to an expression result
                    auto *Alloca = createEntryBlockAlloca(Operand->getType(), "addr.tmp");
                    Builder->CreateStore(Operand, Alloca);
                    return Alloca;
                }
            }
            return emitIntrinsicUnaryOp(Call.Name, Args[0], Call.ResultType);
        }
    }

    // Handle intrinsic methods (non-operators)
    if (Call.IsIntrinsic && !Call.IsOperator) {
        // Debug log

        // Handle Option::unwrap - extract payload from union
        if (Call.Name == "unwrap" && Args.size() == 1) {
            llvm::Value *OptionValue = Args[0];

            // Get the LLVM type for the result
            llvm::Type *ResultTy = mapType(Call.ResultType);
            if (!ResultTy) {
                return llvm::make_error<llvm::StringError>(
                    "Cannot map result type for unwrap: " + Call.ResultType.Name,
                    llvm::inconvertibleErrorCode()
                );
            }

            // Extract the union type name from the mangled method name
            // Format: _ZN<len>Option<generics>E<len>unwrap...
            std::string MangledName = Call.MangledName;
            size_t startPos = MangledName.find("_ZN");
            if (startPos == std::string::npos) {
                return llvm::make_error<llvm::StringError>(
                    "Invalid mangled name for unwrap: " + MangledName,
                    llvm::inconvertibleErrorCode()
                );
            }

            // Find the union name by looking for "E6unwrap" or "E9unwrap_or"
            size_t unwrapPos = MangledName.find("E6unwrap");
            if (unwrapPos == std::string::npos) {
                unwrapPos = MangledName.find("E9unwrap_or");
            }
            if (unwrapPos == std::string::npos) {
                return llvm::make_error<llvm::StringError>(
                    "Cannot find unwrap in mangled name: " + MangledName,
                    llvm::inconvertibleErrorCode()
                );
            }

            // Extract the union type's mangled name (everything up to and including E)
            std::string UnionMangledName = "_Z" + MangledName.substr(3, unwrapPos - 2);


            // Look up the union type in StructCache (union types are stored there)
            // If not found, it means this is an NPO (Null Pointer Optimization) Option
            // where the Option is represented as a raw pointer (null = None, non-null = Some)
            auto StructIt = StructCache.find(UnionMangledName);
            if (StructIt != StructCache.end()) {
                // Regular union - extract payload from struct
                llvm::StructType *UnionTy = StructIt->second;

                // Union has structure: { i8 tag, [N x i8] payload }
                // GEP to payload area (index 1)
                auto *PayloadPtr = Builder->CreateStructGEP(
                    UnionTy, OptionValue, 1, "unwrap.payload.ptr");

                // For reference types (pointer result), just load the pointer from payload
                if (ResultTy->isPointerTy()) {
                    return Builder->CreateLoad(ResultTy, PayloadPtr, "unwrap.ptr");
                }

                // For value types, load from the payload area
                return Builder->CreateLoad(ResultTy, PayloadPtr, "unwrap.value");
            } else {
                // NPO Option - Args[0] is a pointer to the Option storage location
                // For Option[ref[T]], the storage contains a pointer (T*), so Args[0] is T**
                // We need to load from it to get T*
                return Builder->CreateLoad(ResultTy, OptionValue, "unwrap.npo");
            }
        }

        return llvm::make_error<llvm::StringError>(
            "Unknown intrinsic method: " + Call.Name,
            llvm::inconvertibleErrorCode()
        );
    }

    if (IsInitializerCall) {
        // Look up the initializer function
        auto *InitFunc = lookupFunction(Call.MangledName);
        if (!InitFunc) {
            return llvm::make_error<llvm::StringError>(
                "Initializer not found: " + Call.MangledName,
                llvm::inconvertibleErrorCode()
            );
        }

        // Determine allocation strategy based on Call.Life
        // UnspecifiedLifetime = stack, LocalLifetime ($) = local page,
        // CallLifetime (#) = caller's page, ReferenceLifetime (^name) = named region
        bool IsRegionAlloc = !std::holds_alternative<UnspecifiedLifetime>(Call.Life);

        // Get the struct type
        llvm::Type *StructTy;
        if (IsRegionAlloc) {
            // For region allocation, ResultType is pointer[StructType]
            if (!Call.ResultType.Generics.empty()) {
                StructTy = mapType(Call.ResultType.Generics[0]);
            } else {
                StructTy = mapType(Call.ResultType);
            }
        } else {
            StructTy = mapType(Call.ResultType);
        }
        if (!StructTy) {
            return llvm::make_error<llvm::StringError>(
                "Cannot map struct type for initializer: " + Call.ResultType.Name,
                llvm::inconvertibleErrorCode()
            );
        }

        llvm::Value *StructPtr;
        size_t ArgsOffset = 0;

        if (IsRegionAlloc) {
            // Determine which page to use based on lifetime
            llvm::Value *Page = nullptr;

            if (std::holds_alternative<LocalLifetime>(Call.Life)) {
                // $ = local page
                Page = CurrentRegion.LocalPage;
            } else if (std::holds_alternative<CallLifetime>(Call.Life)) {
                // # = caller's return page
                Page = CurrentRegion.ReturnPage;
            } else if (std::holds_alternative<ReferenceLifetime>(Call.Life)) {
                // ^name = first argument is the page pointer
                if (!Args.empty()) {
                    Page = Args[0];
                    ArgsOffset = 1;  // Skip the region arg
                }
            }

            if (Page) {
                auto *I64Ty = llvm::Type::getInt64Ty(*Context);
                size_t TypeSize = getTypeSize(StructTy);
                size_t TypeAlign = getTypeAlignment(StructTy);
                auto *Size = llvm::ConstantInt::get(I64Ty, TypeSize);
                auto *Align = llvm::ConstantInt::get(I64Ty, TypeAlign);
                StructPtr = Builder->CreateCall(PageAllocate, {Page, Size, Align}, "struct.region");
            } else {
                // Fallback to stack if page not available
                StructPtr = createEntryBlockAlloca(StructTy, "struct.init");
                IsRegionAlloc = false;  // Adjust for return logic
            }
        } else {
            // Stack allocation (no lifetime suffix)
            StructPtr = createEntryBlockAlloca(StructTy, "struct.init");
        }

        // Prepare arguments: struct pointer first, then page if init#, then other args
        std::vector<llvm::Value*> InitArgs;
        InitArgs.push_back(StructPtr);

        // If init# was used, pass the page as second argument
        if (Call.RequiresPageParam) {
            llvm::Value *PageArg = nullptr;
            if (std::holds_alternative<LocalLifetime>(Call.Life)) {
                PageArg = CurrentRegion.LocalPage;
            } else if (std::holds_alternative<CallLifetime>(Call.Life)) {
                PageArg = CurrentRegion.ReturnPage;
                // Fallback: use local page if ReturnPage not available
                if (!PageArg && CurrentRegion.LocalPage) {
                    PageArg = CurrentRegion.LocalPage;
                }
            } else if (std::holds_alternative<ReferenceLifetime>(Call.Life)) {
                // For ^name, the page is already in Args[0]
                if (!Args.empty()) {
                    PageArg = Args[0];
                }
            }

            // If we still don't have a page, allocate one on demand
            // This handles cases like String#() inside a non-function# function
            if (!PageArg) {
                if (PageAllocatePage) {
                    CurrentRegion.LocalPage = Builder->CreateCall(PageAllocatePage, {}, "local_page.ondemand");
                    PageArg = CurrentRegion.LocalPage;
                } else {
                }
            }
            if (PageArg) {
                InitArgs.push_back(PageArg);
            }
        }

        // Adjust remaining arguments for the initializer
        auto *FuncTy = InitFunc->getFunctionType();
        size_t ParamOffset = 1;  // Skip 'this' param
        if (Call.RequiresPageParam) {
            ParamOffset = 2;  // Skip 'this' and 'page' params
        }
        for (size_t i = ArgsOffset; i < Args.size() && i - ArgsOffset + ParamOffset < FuncTy->getNumParams(); ++i) {
            llvm::Value *ArgVal = Args[i];
            llvm::Type *ParamTy = FuncTy->getParamType(i - ArgsOffset + ParamOffset);

            if (ParamTy->isPointerTy() && ArgVal->getType()->isStructTy()) {
                // Create alloca and store for struct arguments
                auto *Alloca = createEntryBlockAlloca(ArgVal->getType(), "init.arg.tmp");
                Builder->CreateStore(ArgVal, Alloca);
                InitArgs.push_back(Alloca);
            } else {
                InitArgs.push_back(ArgVal);
            }
        }

        // Call the initializer (returns void)
        Builder->CreateCall(InitFunc, InitArgs);

        // Return: pointer for region alloc, loaded struct value for stack alloc
        if (IsRegionAlloc) {
            return StructPtr;
        } else {
            return Builder->CreateLoad(StructTy, StructPtr, "struct.val");
        }
    }

    // Non-intrinsic call: use already looked-up function
    if (!Func) {
        return llvm::make_error<llvm::StringError>(
            "Function not found: " + Call.MangledName,
            llvm::inconvertibleErrorCode()
        );
    }

    // Handle function# page parameter (similar to init#)
    // For CallLifetime (#), pass ReturnPage. For ReferenceLifetime (^name), Args[0] has it.
    size_t FuncArgsOffset = 0;  // Skip Args[0] if it's an explicit page
    llvm::Value *FuncPageArg = nullptr;
    if (Call.MangledName.find("read_to_string") != std::string::npos) {
    }
    if (Call.RequiresPageParam) {
        if (std::holds_alternative<LocalLifetime>(Call.Life)) {
            FuncPageArg = CurrentRegion.LocalPage;
        } else if (std::holds_alternative<CallLifetime>(Call.Life)) {
            // Use ReturnPage (caller's rp) if available, otherwise fall back to LocalPage
            FuncPageArg = CurrentRegion.ReturnPage;
            if (!FuncPageArg) {
                FuncPageArg = CurrentRegion.LocalPage;
            }
        } else if (std::holds_alternative<ReferenceLifetime>(Call.Life)) {
            // For ^name, the page is already in Args[0]
            if (!Args.empty()) {
                FuncPageArg = Args[0];
                FuncArgsOffset = 1;  // Skip the page in the regular args
            }
        }

        // If we still don't have a page, allocate one on demand
        // This handles cases like func#() inside a non-function# function
        if (!FuncPageArg && PageAllocatePage) {
            CurrentRegion.LocalPage = Builder->CreateCall(PageAllocatePage, {}, "local_page.ondemand");
            FuncPageArg = CurrentRegion.LocalPage;
        }
    }

    // Check if function uses sret (returns struct via first parameter)
    bool UsesSret = Func->arg_size() > 0 &&
                    Func->hasParamAttribute(0, llvm::Attribute::StructRet);

    if (UsesSret) {
        // Get the struct type from the sret attribute
        llvm::Type *RetTy = Func->getParamStructRetType(0);
        if (!RetTy) {
            return llvm::make_error<llvm::StringError>(
                "sret function has no return type: " + Call.MangledName,
                llvm::inconvertibleErrorCode()
            );
        }

        // Allocate space for the return value
        llvm::Value *RetPtr = createEntryBlockAlloca(RetTy, "sret.result");

        // Build argument list: sret pointer first
        std::vector<llvm::Value*> CallArgs;
        CallArgs.push_back(RetPtr);

        // If function# was used, pass the page as second argument (after sret)
        auto *FuncTy = Func->getFunctionType();
        size_t FirstUserArg = 1;  // Start after sret
        if (FuncPageArg) {
            CallArgs.push_back(FuncPageArg);
            FirstUserArg = 2;
        }

        // Check if this is a throwing function - allocate exception page if needed
        bool IsThrowingFunction = ThrowingFunctions.count(Call.MangledName) > 0;
        llvm::Value *CalleeExceptionPage = nullptr;
        if (IsThrowingFunction) {
            // Allocate an exception page for the called function to use for error allocation
            // Only allocate if Page.allocate_page is available; otherwise pass null
            if (PageAllocatePage && !PageAllocatePage->isDeclaration()) {
                CalleeExceptionPage = Builder->CreateCall(PageAllocatePage, {}, "exception_page");
            } else {
                CalleeExceptionPage = llvm::ConstantPointerNull::get(llvm::PointerType::get(*Context, 0));
            }
            CallArgs.push_back(CalleeExceptionPage);
            FirstUserArg++;
        }

        // Adjust remaining arguments for the function
        for (size_t i = FuncArgsOffset; i < Args.size() && i - FuncArgsOffset + FirstUserArg < FuncTy->getNumParams(); ++i) {
            llvm::Value *ArgVal = Args[i];
            llvm::Type *ParamTy = FuncTy->getParamType(i - FuncArgsOffset + FirstUserArg);

            if (ParamTy->isPointerTy() && ArgVal->getType()->isStructTy()) {
                auto *Alloca = createEntryBlockAlloca(ArgVal->getType(), "sret.arg.tmp");
                Builder->CreateStore(ArgVal, Alloca);
                CallArgs.push_back(Alloca);
            } else {
                CallArgs.push_back(ArgVal);
            }
        }

        // Call the function (returns void)
        Builder->CreateCall(Func, CallArgs);

        // Note: For throwing functions, the result is a Result union.
        // Error checking is done by try/choose expressions, not automatically here.
        // If a throwing function is called outside try/choose, the caller
        // is responsible for handling the Result.

        // Load and return the result
        return Builder->CreateLoad(RetTy, RetPtr, "sret.val");
    }

    // Check if this is a throwing function (non-sret case)
    bool IsThrowingFunction = ThrowingFunctions.count(Call.MangledName) > 0;

    // If no user arguments (after accounting for page offset), handle special cases
    if (Args.size() <= FuncArgsOffset) {
        std::vector<llvm::Value*> CallArgs;
        // Add function# page parameter if needed
        if (FuncPageArg) {
            CallArgs.push_back(FuncPageArg);
        }
        if (IsThrowingFunction) {
            // Allocate exception page and pass as argument
            // Only allocate if Page.allocate_page is available; otherwise pass null
            llvm::Value *CalleeExceptionPage;
            if (PageAllocatePage && !PageAllocatePage->isDeclaration()) {
                CalleeExceptionPage = Builder->CreateCall(PageAllocatePage, {}, "exception_page");
            } else {
                CalleeExceptionPage = llvm::ConstantPointerNull::get(llvm::PointerType::get(*Context, 0));
            }
            CallArgs.push_back(CalleeExceptionPage);
        }
        if (CallArgs.empty()) {
            return Builder->CreateCall(Func, Args);
        }
        return Builder->CreateCall(Func, CallArgs);
    }

    // Check if we need to adjust any arguments
    auto *FuncTy = Func->getFunctionType();
    size_t ParamOffset = (FuncPageArg ? 1 : 0) + (IsThrowingFunction ? 1 : 0);  // Skip page and/or exception page param
    bool NeedsAdjustment = FuncPageArg || IsThrowingFunction;  // Always need to prepend page or exception page
    for (size_t i = FuncArgsOffset; i < Args.size() && i - FuncArgsOffset + ParamOffset < FuncTy->getNumParams(); ++i) {
        llvm::Type *ParamTy = FuncTy->getParamType(i - FuncArgsOffset + ParamOffset);
        llvm::Type *ArgTy = Args[i]->getType();
        if (ParamTy->isPointerTy() && ArgTy->isStructTy()) {
            NeedsAdjustment = true;
            break;
        }
        // Integer type mismatch (e.g., i64 arg to i32 param)
        if (ParamTy->isIntegerTy() && ArgTy->isIntegerTy() && ParamTy != ArgTy) {
            NeedsAdjustment = true;
            break;
        }
        // Pointer to integer mismatch (e.g., ptr arg to i8 param) - indicates a bug
        if (ParamTy->isIntegerTy() && ArgTy->isPointerTy()) {
            NeedsAdjustment = true;
            break;
        }
    }

    if (!NeedsAdjustment) {
        return Builder->CreateCall(Func, Args);
    }

    // Adjust arguments: convert struct values to pointers, and integer types
    std::vector<llvm::Value*> AdjustedArgs;

    // Prepend function# page parameter if needed
    if (FuncPageArg) {
        AdjustedArgs.push_back(FuncPageArg);
    }

    // Prepend exception page if this is a throwing function
    llvm::Value *CalleeExceptionPage = nullptr;
    if (IsThrowingFunction) {
        // Only allocate if Page.allocate_page is available; otherwise pass null
        if (PageAllocatePage && !PageAllocatePage->isDeclaration()) {
            CalleeExceptionPage = Builder->CreateCall(PageAllocatePage, {}, "exception_page");
        } else {
            CalleeExceptionPage = llvm::ConstantPointerNull::get(llvm::PointerType::get(*Context, 0));
        }
        AdjustedArgs.push_back(CalleeExceptionPage);
    }

    for (size_t i = FuncArgsOffset; i < Args.size() && i - FuncArgsOffset + ParamOffset < FuncTy->getNumParams(); ++i) {
        llvm::Value *ArgVal = Args[i];
        llvm::Type *ParamTy = FuncTy->getParamType(i - FuncArgsOffset + ParamOffset);
        llvm::Type *ArgTy = ArgVal->getType();

        if (ParamTy->isPointerTy() && ArgTy->isStructTy()) {
            // Create alloca at entry block and store the value
            auto *Alloca = createEntryBlockAlloca(ArgTy, "arg.tmp");
            Builder->CreateStore(ArgVal, Alloca);
            AdjustedArgs.push_back(Alloca);
        } else if (ParamTy->isIntegerTy() && ArgTy->isIntegerTy() && ParamTy != ArgTy) {
            // Integer type conversion (truncate or extend)
            AdjustedArgs.push_back(Builder->CreateIntCast(ArgVal, ParamTy, true, "arg.cast"));
        } else if (ParamTy->isIntegerTy() && ArgTy->isPointerTy()) {
            // Pointer to integer - load the value from pointer
            // This happens when a global string constant is passed where a char is expected
            AdjustedArgs.push_back(Builder->CreateLoad(ParamTy, ArgVal, "arg.load"));
        } else {
            AdjustedArgs.push_back(ArgVal);
        }
    }

    // Return the call result - try/choose will check for errors if needed
    return Builder->CreateCall(Func, AdjustedArgs);
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

    // Pointer arithmetic: pointer + int, pointer - int
    bool LeftIsPtr = Left->getType()->isPointerTy();
    bool RightIsInt = Right->getType()->isIntegerTy();

    if (LeftIsPtr && RightIsInt) {
        if (OpName == "+") {
            // pointer + int: use GEP to offset by N elements
            // Get the element type from the result type
            llvm::Type *ElemTy = llvm::Type::getInt8Ty(*Context);  // Default to byte offset
            if (ResultType.Name == "pointer" && !ResultType.Generics.empty()) {
                ElemTy = mapType(ResultType.Generics[0]);
            }
            return Builder->CreateGEP(ElemTy, Left, Right, "ptr.add");
        }
        if (OpName == "-") {
            // pointer - int: negate and use GEP
            llvm::Type *ElemTy = llvm::Type::getInt8Ty(*Context);
            if (ResultType.Name == "pointer" && !ResultType.Generics.empty()) {
                ElemTy = mapType(ResultType.Generics[0]);
            }
            llvm::Value *NegOffset = Builder->CreateNeg(Right, "neg");
            return Builder->CreateGEP(ElemTy, Left, NegOffset, "ptr.sub");
        }
    }

    // Pointer difference: pointer - pointer
    if (LeftIsPtr && Right->getType()->isPointerTy() && OpName == "-") {
        return Builder->CreatePtrDiff(llvm::Type::getInt8Ty(*Context), Left, Right, "ptr.diff");
    }

    // Integer type promotion: if both are integers but different widths, extend the narrower one
    if (Left->getType()->isIntegerTy() && Right->getType()->isIntegerTy()) {
        unsigned LeftBits = Left->getType()->getIntegerBitWidth();
        unsigned RightBits = Right->getType()->getIntegerBitWidth();
        if (LeftBits != RightBits) {
            if (LeftBits > RightBits) {
                // Extend Right to match Left's width
                Right = Builder->CreateZExt(Right, Left->getType(), "zext");
            } else {
                // Extend Left to match Right's width
                Left = Builder->CreateZExt(Left, Right->getType(), "zext");
            }
        }
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

    // Bit shift operators
    if (OpName == "<<") {
        return Builder->CreateShl(Left, Right, "shl");
    }
    if (OpName == ">>") {
        // Arithmetic shift for signed (preserves sign), logical for unsigned
        if (IsSigned)
            return Builder->CreateAShr(Left, Right, "ashr");
        return Builder->CreateLShr(Left, Right, "lshr");
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
    if (OpName == "+") {
        // Unary plus: just return the operand unchanged
        return Operand;
    }
    if (OpName == "abs") {
        if (IsFloat) {
            // Use llvm.fabs intrinsic for floating point
            llvm::Function *FabsFn = llvm::Intrinsic::getDeclaration(
                Module.get(), llvm::Intrinsic::fabs, {Ty});
            return Builder->CreateCall(FabsFn, {Operand}, "fabs");
        }
        // Use llvm.abs intrinsic for integers
        // Second argument is "is_int_min_poison" - set to false for safety
        llvm::Function *AbsFn = llvm::Intrinsic::getDeclaration(
            Module.get(), llvm::Intrinsic::abs, {Ty});
        llvm::Value *IsIntMinPoison = llvm::ConstantInt::getFalse(*Context);
        return Builder->CreateCall(AbsFn, {Operand, IsIntMinPoison}, "abs");
    }
    if (OpName == "!") {
        return Builder->CreateNot(Operand, "not");
    }
    if (OpName == "~") {
        return Builder->CreateNot(Operand, "bitnot");
    }
    if (OpName == "*") {
        // Dereference: load value from pointer
        // Operand is the pointer, ResultType is the element type
        if (!Operand->getType()->isPointerTy()) {
            return llvm::make_error<llvm::StringError>(
                "Dereference operator '*' requires a pointer operand, got: " +
                    std::to_string(Operand->getType()->getTypeID()),
                llvm::inconvertibleErrorCode()
            );
        }
        auto *Result = Builder->CreateLoad(Ty, Operand, "deref");
        return Result;
    }
    if (OpName == "&") {
        // Address-of: the operand should already be a pointer (from alloca)
        // Just return the pointer value itself
        return Operand;
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
        } else if constexpr (std::is_same_v<T, HexConstant>) {
            // Hex constants are unsigned integers
            llvm::Type *Ty = Type.isResolved() ? mapType(Type) : llvm::Type::getInt64Ty(*Context);
            return llvm::ConstantInt::get(Ty, C.Value, /*isSigned=*/false);
        } else if constexpr (std::is_same_v<T, FloatingPointConstant>) {
            llvm::Type *Ty = Type.isResolved() ? mapType(Type) : llvm::Type::getDoubleTy(*Context);
            return llvm::ConstantFP::get(Ty, C.Value);
        } else if constexpr (std::is_same_v<T, BooleanConstant>) {
            return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*Context), C.Value ? 1 : 0);
        } else if constexpr (std::is_same_v<T, StringConstant>) {
            return Builder->CreateGlobalStringPtr(C.Value);
        } else if constexpr (std::is_same_v<T, CharacterConstant>) {
            // Character constant - emit as i32 (Unicode code point)
            int32_t CharVal = decodeUtf8ToCodePoint(C.Value);
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), CharVal);
        } else if constexpr (std::is_same_v<T, NullConstant>) {
            // Null pointer - use the target type or default to i8*
            llvm::Type *Ty = Type.isResolved() ? mapType(Type) : llvm::PointerType::getUnqual(*Context);
            if (!Ty->isPointerTy()) {
                Ty = llvm::PointerType::getUnqual(*Context);
            }
            return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(Ty));
        } else {
            return llvm::UndefValue::get(llvm::Type::getInt64Ty(*Context));
        }
    }, Const);
}

// ============================================================================
// Control Flow (Stubs)
// ============================================================================

llvm::Expected<llvm::Value*> Emitter::emitBlock(const PlannedBlock &Block) {
    // Save watermark at block entry if this block has $ allocations
    // Note: Block-scoped cleanup is only possible when Page.save_watermark is available
    // (i.e., when Page.scaly is compiled into the module, not just declared as external)
    BlockCleanupContext Cleanup;
    Cleanup.NeedsCleanup = Block.NeedsLocalPageCleanup && CurrentRegion.LocalPage != nullptr &&
                          PageSaveWatermark && !PageSaveWatermark->isDeclaration();
    if (Cleanup.NeedsCleanup) {
        // Allocate space for BlockWatermark on stack
        Cleanup.Watermark = createEntryBlockAlloca(BlockWatermarkType, "block.watermark");
        // Call save_watermark(page, &watermark) - stores watermark in the alloca
        Builder->CreateCall(PageSaveWatermark, {CurrentRegion.LocalPage, Cleanup.Watermark});
    }
    BlockCleanupStack.push_back(Cleanup);

    llvm::Value *LastValue = nullptr;
    for (const auto &Stmt : Block.Statements) {
        // For PlannedAction, capture the result value (the block's result is the last value)
        if (auto *Action = std::get_if<PlannedAction>(&Stmt)) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr) {
                BlockCleanupStack.pop_back();
                return ValueOrErr.takeError();
            }
            // Only save value if it's not an assignment (assignments have targets)
            if (Action->Target.empty()) {
                LastValue = *ValueOrErr;
            }
        } else if (auto *Return = std::get_if<PlannedReturn>(&Stmt)) {
            // Emit the full return instruction (including cleanup for local pages)
            // Note: emitReturn handles cleanup for ALL pending blocks
            if (auto Err = emitReturn(*Return)) {
                BlockCleanupStack.pop_back();
                return std::move(Err);
            }
            // After return, block has terminated, so LastValue doesn't matter
            BlockCleanupStack.pop_back();
            return nullptr;
        } else if (auto *Throw = std::get_if<PlannedThrow>(&Stmt)) {
            // Throw emits return - block terminates
            if (auto Err = emitThrow(*Throw)) {
                BlockCleanupStack.pop_back();
                return std::move(Err);
            }
            BlockCleanupStack.pop_back();
            return nullptr;
        } else if (auto *Break = std::get_if<PlannedBreak>(&Stmt)) {
            // Break emits branch - block terminates
            if (auto Err = emitBreak(*Break)) {
                BlockCleanupStack.pop_back();
                return std::move(Err);
            }
            BlockCleanupStack.pop_back();
            return nullptr;
        } else if (auto *Continue = std::get_if<PlannedContinue>(&Stmt)) {
            // Continue emits branch - block terminates
            if (auto Err = emitContinue(*Continue)) {
                BlockCleanupStack.pop_back();
                return std::move(Err);
            }
            BlockCleanupStack.pop_back();
            return nullptr;
        } else {
            if (auto Err = emitStatement(Stmt)) {
                BlockCleanupStack.pop_back();
                return std::move(Err);
            }
        }
    }

    // Restore watermark at block exit
    if (Cleanup.NeedsCleanup) {
        Builder->CreateCall(PageRestoreWatermark, {CurrentRegion.LocalPage, Cleanup.Watermark});
    }
    BlockCleanupStack.pop_back();

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

    // Defensive null check - emitOperands should not return nullptr without error
    if (!CondValue) {
        return llvm::make_error<llvm::StringError>(
            "If condition emitted to null value (possible cross-module call issue)",
            llvm::inconvertibleErrorCode()
        );
    }

    // Ensure we have a boolean (i1) for the branch
    if (!CondValue->getType()->isIntegerTy(1)) {
        if (CondValue->getType()->isIntegerTy()) {
            CondValue = Builder->CreateICmpNE(
                CondValue,
                llvm::ConstantInt::get(CondValue->getType(), 0),
                "if.tobool"
            );
        } else if (CondValue->getType()->isPointerTy()) {
            // Pointer - compare to null
            CondValue = Builder->CreateICmpNE(
                CondValue,
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(CondValue->getType())),
                "if.tobool"
            );
        } else {
            std::string TypeStr;
            llvm::raw_string_ostream OS(TypeStr);
            CondValue->getType()->print(OS);
            return llvm::make_error<llvm::StringError>(
                "If condition must be boolean or integer, got type: " + TypeStr +
                " at offset " + std::to_string(If.Loc.Start),
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
        } else if (auto *Break = std::get_if<PlannedBreak>(If.Consequent.get())) {
            if (auto Err = emitBreak(*Break))
                return std::move(Err);
        } else if (auto *Continue = std::get_if<PlannedContinue>(If.Consequent.get())) {
            if (auto Err = emitContinue(*Continue))
                return std::move(Err);
        } else if (auto *Return = std::get_if<PlannedReturn>(If.Consequent.get())) {
            if (auto Err = emitReturn(*Return))
                return std::move(Err);
        } else if (auto *Throw = std::get_if<PlannedThrow>(If.Consequent.get())) {
            if (auto Err = emitThrow(*Throw))
                return std::move(Err);
        }
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
            } else if (auto *Break = std::get_if<PlannedBreak>(If.Alternative.get())) {
                if (auto Err = emitBreak(*Break))
                    return std::move(Err);
            } else if (auto *Continue = std::get_if<PlannedContinue>(If.Alternative.get())) {
                if (auto Err = emitContinue(*Continue))
                    return std::move(Err);
            } else if (auto *Return = std::get_if<PlannedReturn>(If.Alternative.get())) {
                if (auto Err = emitReturn(*Return))
                    return std::move(Err);
            } else if (auto *Throw = std::get_if<PlannedThrow>(If.Alternative.get())) {
                if (auto Err = emitThrow(*Throw))
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
        llvm::Type *ValueType = ThenValue->getType();

        // Don't create PHI for void type
        if (ValueType->isVoidTy()) {
            return nullptr;
        }

        // Collect incoming values with their actual end blocks
        std::vector<std::pair<llvm::Value*, llvm::BasicBlock*>> IncomingValues;
        IncomingValues.push_back({ThenValue, ThenEndBlock});
        IncomingValues.push_back({ElseValue, ElseEndBlock});

        // Collect all blocks that have entries
        std::set<llvm::BasicBlock*> CoveredBlocks;
        for (const auto &[Val, Block] : IncomingValues) {
            CoveredBlocks.insert(Block);
        }

        // Add undef entries for any predecessor blocks not covered
        for (llvm::BasicBlock *Pred : llvm::predecessors(MergeBlock)) {
            if (CoveredBlocks.find(Pred) == CoveredBlocks.end()) {
                IncomingValues.push_back({llvm::UndefValue::get(ValueType), Pred});
            }
        }

        llvm::PHINode *PHI = Builder->CreatePHI(ValueType, IncomingValues.size(), "if.value");
        for (const auto &[Val, Block] : IncomingValues) {
            PHI->addIncoming(Val, Block);
        }
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

    // Get the planned union type for proper LLVM type lookup
    const PlannedType &CondType = Choose.Condition[0].ResultType;

    // Check if the condition is a throwing function call
    // Throwing calls return a Result[T, E] wrapper, NOT the value type
    // So we should NOT use Option NPO for them even if the value type is Option
    bool IsThrowingCall = false;
    if (auto *Call = std::get_if<PlannedCall>(&Choose.Condition[0].Expr)) {
        IsThrowingCall = Call->CanThrow;
    }

    // Handle Option with NPO (Null Pointer Optimization)
    // NPO only applies when the inner type is pointer or ref - then Option is a single pointer
    // (null = None, non-null = Some). For value types like Option[int], use regular tagged union.
    // Note: For methods, CondType may be pointer[Option[T]] since 'this' is a pointer
    // IMPORTANT: Do NOT use NPO for throwing function calls - they return Result[T, E], not Option
    const PlannedType *OptionType = &CondType;
    bool CondIsPointer = false;
    if ((CondType.Name == "pointer" || CondType.Name == "ref") && !CondType.Generics.empty()) {
        OptionType = &CondType.Generics[0];
        CondIsPointer = true;
    }

    bool IsOption = !IsThrowingCall &&
                    (OptionType->Name == "Option" ||
                     (OptionType->Name.size() > 7 && OptionType->Name.substr(0, 7) == "Option."));
    if (IsOption && !OptionType->Generics.empty()) {
        const PlannedType &InnerType = OptionType->Generics[0];
        bool IsPointerLike = InnerType.Name == "pointer" || InnerType.Name == "ref" ||
                             (InnerType.Name.size() >= 8 && InnerType.Name.substr(0, 8) == "pointer.") ||
                             (InnerType.Name.size() >= 4 && InnerType.Name.substr(0, 4) == "ref.");
        if (IsPointerLike) {
            // If CondType was pointer[Option[...]], we need to load the Option value first
            if (CondIsPointer) {
                UnionValue = Builder->CreateLoad(llvm::PointerType::get(*Context, 0), UnionValue, "option.load");
            }
            return emitChooseNPO(Choose, UnionValue);
        }
    }
    // Also handle instantiated Option names like "Option.ref", "Option.pointer"
    // But NOT for throwing calls - they return Result, not Option
    if (!IsThrowingCall && OptionType->Name.size() > 7 && OptionType->Name.substr(0, 7) == "Option.") {
        std::string_view Inner = std::string_view(OptionType->Name).substr(7);
        if (Inner.substr(0, 3) == "ref" || Inner.substr(0, 7) == "pointer") {
            // If CondType was pointer[Option[...]], we need to load the Option value first
            if (CondIsPointer) {
                UnionValue = Builder->CreateLoad(llvm::PointerType::get(*Context, 0), UnionValue, "option.load");
            }
            return emitChooseNPO(Choose, UnionValue);
        }
    }

    // Union layout is { i8 tag, [maxSize x i8] data }
    // We need to extract the tag to switch on it

    // Get pointer to the union (may already be a pointer or need alloca)
    llvm::Value *UnionPtr;
    llvm::Type *UnionType = UnionValue->getType();
    if (UnionValue->getType()->isPointerTy()) {
        UnionPtr = UnionValue;
        // Look up the pointee type from cache
        // If CondType is pointer[T], we need the inner type T for cache lookup
        const PlannedType &LookupType = CondType.isPointer() && !CondType.Generics.empty()
            ? CondType.Generics[0] : CondType;

        auto CacheIt = StructCache.find(LookupType.MangledName);
        if (CacheIt == StructCache.end()) {
            CacheIt = StructCache.find(LookupType.Name);
        }
        if (CacheIt == StructCache.end()) {
            CacheIt = StructCache.find("_Z" + LookupType.MangledName);
        }
        if (CacheIt != StructCache.end()) {
            UnionType = CacheIt->second;
        } else {
            // Fallback: get the type via mapType
            UnionType = mapType(LookupType);
            // If mapType returned a pointer, we need to get the underlying struct type
            if (UnionType->isPointerTy()) {
                // Try looking up by the condition type's mangled name
                auto InnerCacheIt = StructCache.find(CondType.MangledName);
                if (InnerCacheIt != StructCache.end()) {
                    UnionType = InnerCacheIt->second;
                }
            }
        }
    } else {
        // The union value might have an anonymous struct type (from extractvalue)
        // that is structurally equivalent to the named type but not identical.
        // Use the value's actual type for consistency.
        UnionType = UnionValue->getType();

        // Ensure it's a struct type suitable for GEP
        if (!UnionType->isStructTy()) {
            return llvm::make_error<llvm::StringError>(
                "Choose condition is not a union/struct type",
                llvm::inconvertibleErrorCode()
            );
        }

        // Store the union value to get a pointer
        UnionPtr = createEntryBlockAlloca(UnionType, "choose.union");
        Builder->CreateStore(UnionValue, UnionPtr);
    }

    // Load the tag (first field, i8)
    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
    llvm::Value *TagPtr = Builder->CreateStructGEP(UnionType, UnionPtr, 0, "tag.ptr");
    llvm::Value *Tag = Builder->CreateLoad(I8Ty, TagPtr, "tag");

    // Create blocks
    llvm::BasicBlock *MergeBlock = createBlock("choose.end");
    // Always create a separate default block to avoid PHI node predecessor issues
    llvm::BasicBlock *DefaultBlock = createBlock("choose.else");

    // Create switch instruction
    llvm::SwitchInst *Switch = Builder->CreateSwitch(Tag, DefaultBlock, Choose.Cases.size());

    // Use an alloca for result values to avoid domination issues with nested control flow
    // Values from nested if/choose might be PHIs that don't dominate the merge block edges
    llvm::AllocaInst *ResultAlloca = nullptr;
    llvm::Type *ResultType = nullptr;

    // Process each when case
    for (const auto &When : Choose.Cases) {
        llvm::BasicBlock *CaseBlock = createBlock("choose.when." + When.Name);
        // Use the variant's tag from the planned union
        Switch->addCase(
            llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(I8Ty, When.VariantIndex)),
            CaseBlock
        );

        Builder->SetInsertPoint(CaseBlock);

        // Extract variant data and bind to the when clause's variable name
        if (!When.Name.empty()) {
            llvm::Value *DataPtr = Builder->CreateStructGEP(
                UnionType, UnionPtr, 1, "variant.data.ptr");

            // Cast to the variant's type and load
            llvm::Type *VarTy = mapType(When.VariantType);
            llvm::Value *DataCast = Builder->CreateBitCast(
                DataPtr,
                llvm::PointerType::getUnqual(VarTy),
                "variant.data.cast"
            );
            llvm::Value *VarValue = Builder->CreateLoad(VarTy, DataCast, "variant.val");

            // Bind the value to the variable name
            LocalVariables[When.Name] = VarValue;
        }

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
            } else {
                // Handle other statement types (throw, return, break, continue)
                if (auto Err = emitStatement(*When.Consequent))
                    return std::move(Err);
            }
        }

        // Store case value to alloca to avoid domination issues
        // Check that the value dominates the current block before storing
        llvm::BasicBlock *CurrentBlock = Builder->GetInsertBlock();
        bool CanUseValue = CaseValue && !CaseValue->getType()->isVoidTy() &&
                           !CurrentBlock->getTerminator();
        if (CanUseValue) {
            bool ValueDominates = true;
            if (auto *Inst = llvm::dyn_cast<llvm::Instruction>(CaseValue)) {
                llvm::BasicBlock *ValueBlock = Inst->getParent();
                // Only store if the value is in the current block
                // Values from nested control flow (inner choose) may not dominate
                ValueDominates = (ValueBlock == CurrentBlock);
            }
            if (ValueDominates) {
                if (!ResultAlloca) {
                    // Create alloca at function entry for proper domination
                    ResultAlloca = createEntryBlockAlloca(CaseValue->getType(), "choose.result");
                    ResultType = CaseValue->getType();
                }
                Builder->CreateStore(CaseValue, ResultAlloca);
            }
        }

        if (!Builder->GetInsertBlock()->getTerminator()) {
            Builder->CreateBr(MergeBlock);
        }
    }

    // Emit the alternative (else) block
    Builder->SetInsertPoint(DefaultBlock);
    llvm::Value *ElseValue = nullptr;

    if (Choose.Alternative) {
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
        } else {
            // Handle other statement types (throw, return, break, continue)
            if (auto Err = emitStatement(*Choose.Alternative))
                return std::move(Err);
        }
    }

    // Store else value to alloca if we have one
    // Check that the value dominates the current block before storing
    llvm::BasicBlock *ElseCurrentBlock = Builder->GetInsertBlock();
    bool CanUseElseValue = ElseValue && !ElseValue->getType()->isVoidTy() &&
                           !ElseCurrentBlock->getTerminator();
    if (CanUseElseValue) {
        bool ElseValueDominates = true;
        if (auto *Inst = llvm::dyn_cast<llvm::Instruction>(ElseValue)) {
            llvm::BasicBlock *ValueBlock = Inst->getParent();
            // Only store if the value is in the current block
            ElseValueDominates = (ValueBlock == ElseCurrentBlock);
        }
        if (ElseValueDominates) {
            if (!ResultAlloca) {
                // Create alloca at function entry for proper domination
                ResultAlloca = createEntryBlockAlloca(ElseValue->getType(), "choose.result");
                ResultType = ElseValue->getType();
            }
            Builder->CreateStore(ElseValue, ResultAlloca);
        }
    } else if (ResultAlloca && !ElseValue) {
        // If when clauses produce values but no else was given, store undef for the default path
        Builder->CreateStore(llvm::UndefValue::get(ResultType), ResultAlloca);
    }

    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(MergeBlock);
    }

    // Continue at merge block only if it's reachable
    // If no paths lead to the merge block, all paths terminated (return/throw)
    // In that case, mark the block unreachable and don't continue emitting there
    if (MergeBlock->hasNPredecessorsOrMore(1)) {
        Builder->SetInsertPoint(MergeBlock);

        // Load result from alloca if we have one
        if (ResultAlloca) {
            return Builder->CreateLoad(ResultType, ResultAlloca, "choose.value");
        }
    } else {
        // Block is unreachable - add unreachable terminator
        Builder->SetInsertPoint(MergeBlock);
        Builder->CreateUnreachable();
    }

    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitChooseNPO(
    const PlannedChoose &Choose, llvm::Value *OptionPtr) {
    // Handle Option[T] with NPO (Null Pointer Optimization)
    // OptionPtr is a pointer: null = None, non-null = Some(value)

    // Get the element type from the Option's generic argument
    const PlannedType &CondType = Choose.Condition[0].ResultType;
    llvm::Type *ElemTy = mapType(CondType.Generics[0]);

    // Create blocks
    llvm::BasicBlock *MergeBlock = createBlock("choose.end");
    llvm::BasicBlock *SomeBlock = createBlock("choose.some");
    llvm::BasicBlock *NoneBlock = createBlock("choose.none");

    // Check if null
    llvm::Value *IsNull = Builder->CreateIsNull(OptionPtr, "is.none");
    Builder->CreateCondBr(IsNull, NoneBlock, SomeBlock);

    // Find Some and None cases
    const PlannedWhen *SomeCase = nullptr;
    const PlannedWhen *NoneCase = nullptr;
    for (const auto &When : Choose.Cases) {
        if (When.VariantIndex == 0) {
            // Some is usually tag 0 (first variant)
            SomeCase = &When;
        } else {
            // None is usually tag 1 (second variant)
            NoneCase = &When;
        }
    }

    // Use alloca-based result storage to handle complex control flow in actions
    // This avoids PHI domination issues when actions have nested if statements
    llvm::AllocaInst *ResultAlloca = nullptr;
    llvm::Type *ResultType = nullptr;
    bool HasSomeValue = false;
    bool HasNoneValue = false;

    // Emit Some case
    Builder->SetInsertPoint(SomeBlock);
    if (SomeCase && !SomeCase->Name.empty()) {
        // Dereference to get the value
        llvm::Value *Value = Builder->CreateLoad(ElemTy, OptionPtr, "some.val");
        LocalVariables[SomeCase->Name] = Value;
    }

    if (SomeCase && SomeCase->Consequent) {
        if (auto *Action = std::get_if<PlannedAction>(SomeCase->Consequent.get())) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            llvm::Value *SomeValue = *ValueOrErr;
            llvm::BasicBlock *CurrentBlock = Builder->GetInsertBlock();
            // Only use value if current block is not terminated and value is usable
            // A value is usable if it's in the current block or dominates it
            bool CanUseValue = SomeValue && !SomeValue->getType()->isVoidTy() &&
                               Action->Target.empty() && !CurrentBlock->getTerminator();
            if (CanUseValue) {
                // Check if value dominates current block
                // If value is an instruction, its parent must dominate current block
                bool ValueDominates = true;
                if (auto *Inst = llvm::dyn_cast<llvm::Instruction>(SomeValue)) {
                    llvm::BasicBlock *ValueBlock = Inst->getParent();
                    // Simple check: value block == current block, or we're in the same function
                    // and there's no complex control flow
                    ValueDominates = (ValueBlock == CurrentBlock);
                }
                if (ValueDominates) {
                    ResultType = SomeValue->getType();
                    ResultAlloca = createEntryBlockAlloca(ResultType, "choose.result");
                    Builder->CreateStore(SomeValue, ResultAlloca);
                    HasSomeValue = true;
                }
            }
        }
    }

    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(MergeBlock);
    }

    // Emit None case
    Builder->SetInsertPoint(NoneBlock);

    if (NoneCase && NoneCase->Consequent) {
        if (auto *Action = std::get_if<PlannedAction>(NoneCase->Consequent.get())) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            llvm::Value *NoneValue = *ValueOrErr;
            llvm::BasicBlock *CurrentBlock = Builder->GetInsertBlock();
            bool CanUseValue = NoneValue && !NoneValue->getType()->isVoidTy() &&
                               Action->Target.empty() && !CurrentBlock->getTerminator();
            if (CanUseValue) {
                bool ValueDominates = true;
                if (auto *Inst = llvm::dyn_cast<llvm::Instruction>(NoneValue)) {
                    ValueDominates = (Inst->getParent() == CurrentBlock);
                }
                if (ValueDominates) {
                    if (!ResultAlloca) {
                        ResultType = NoneValue->getType();
                        ResultAlloca = createEntryBlockAlloca(ResultType, "choose.result");
                    }
                    Builder->CreateStore(NoneValue, ResultAlloca);
                    HasNoneValue = true;
                }
            }
        }
    } else if (Choose.Alternative) {
        // Use else clause for None
        if (auto *Action = std::get_if<PlannedAction>(Choose.Alternative.get())) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr)
                return ValueOrErr.takeError();
            llvm::Value *NoneValue = *ValueOrErr;
            llvm::BasicBlock *CurrentBlock = Builder->GetInsertBlock();
            bool CanUseValue = NoneValue && !NoneValue->getType()->isVoidTy() &&
                               Action->Target.empty() && !CurrentBlock->getTerminator();
            if (CanUseValue) {
                bool ValueDominates = true;
                if (auto *Inst = llvm::dyn_cast<llvm::Instruction>(NoneValue)) {
                    ValueDominates = (Inst->getParent() == CurrentBlock);
                }
                if (ValueDominates) {
                    if (!ResultAlloca) {
                        ResultType = NoneValue->getType();
                        ResultAlloca = createEntryBlockAlloca(ResultType, "choose.result");
                    }
                    Builder->CreateStore(NoneValue, ResultAlloca);
                    HasNoneValue = true;
                }
            }
        }
    }

    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(MergeBlock);
    }

    // Continue at merge block only if it's reachable
    if (MergeBlock->hasNPredecessorsOrMore(1)) {
        Builder->SetInsertPoint(MergeBlock);

        // Load result if we stored one
        if (ResultAlloca && (HasSomeValue || HasNoneValue)) {
            return Builder->CreateLoad(ResultType, ResultAlloca, "choose.value");
        }
    } else {
        // Block is unreachable - add unreachable terminator
        Builder->SetInsertPoint(MergeBlock);
        Builder->CreateUnreachable();
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
    if (For.Expr.empty()) {
        return llvm::make_error<llvm::StringError>(
            "For loop has no range expression",
            llvm::inconvertibleErrorCode()
        );
    }

    // Dispatch to iterator or integer range loop
    if (For.IsIteratorLoop) {
        return emitForIterator(For);
    }
    return emitForIntegerRange(For);
}

llvm::Expected<llvm::Value*> Emitter::emitForIntegerRange(const PlannedFor &For) {
    // Integer range loop: for i in N (iterates 0 to N-1)

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

    // Create the loop counter in entry block (allocate on stack for incrementing)
    llvm::Type *IndexType = EndValue->getType();
    llvm::AllocaInst *LoopCounter = createEntryBlockAlloca(IndexType, For.Identifier + ".counter");

    // Initialize loop counter to 0
    Builder->CreateStore(llvm::ConstantInt::get(IndexType, 0), LoopCounter);

    // Create basic blocks
    llvm::BasicBlock *CondBlock = createBlock("for.cond");
    llvm::BasicBlock *BodyBlock = createBlock("for.body");
    llvm::BasicBlock *IncrBlock = createBlock("for.incr");
    llvm::BasicBlock *ExitBlock = createBlock("for.exit");

    // Push loop context for break/continue
    LoopStack.push_back({ExitBlock, IncrBlock, BlockCleanupStack.size()});

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
    if (!BodyValueOrErr) {
        LoopStack.pop_back();
        return BodyValueOrErr.takeError();
    }

    // Jump to increment block (if not already terminated by break/continue)
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(IncrBlock);
    }

    // Emit increment block: i++
    Builder->SetInsertPoint(IncrBlock);
    llvm::Value *IncrIndex = Builder->CreateLoad(IndexType, LoopCounter, "i.load");
    llvm::Value *NextIndex = Builder->CreateAdd(IncrIndex, llvm::ConstantInt::get(IndexType, 1), "i.next");
    Builder->CreateStore(NextIndex, LoopCounter);
    Builder->CreateBr(CondBlock);

    // Pop loop context
    LoopStack.pop_back();

    // Continue at exit block
    Builder->SetInsertPoint(ExitBlock);

    // Clean up the loop variable from LocalVariables
    LocalVariables.erase(For.Identifier);

    // For loops don't produce a value
    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitForIterator(const PlannedFor &For) {
    // Iterator-based loop: for item in collection
    // Uses the iterator protocol: get_iterator() and next()

    // 1. Evaluate collection expression
    auto CollectionOrErr = emitOperands(For.Expr);
    if (!CollectionOrErr)
        return CollectionOrErr.takeError();
    llvm::Value *Collection = *CollectionOrErr;

    // 2. Get the get_iterator function
    llvm::Function *GetIterFn = lookupFunction(For.GetIteratorMethod);
    if (!GetIterFn) {
        return llvm::make_error<llvm::StringError>(
            "get_iterator method not found: " + For.GetIteratorMethod,
            llvm::inconvertibleErrorCode()
        );
    }

    // 3. Call get_iterator() on collection
    // Collection is passed by pointer (this parameter)
    llvm::Value *CollectionPtr = Collection;
    if (!Collection->getType()->isPointerTy()) {
        // Collection is a value, store it and get pointer
        auto *CollAlloca = createEntryBlockAlloca(Collection->getType(), "coll.tmp");
        Builder->CreateStore(Collection, CollAlloca);
        CollectionPtr = CollAlloca;
    }

    // Check if get_iterator returns via sret
    llvm::Value *Iterator;
    if (GetIterFn->hasParamAttribute(0, llvm::Attribute::StructRet)) {
        // Function uses sret - allocate space and pass as first arg
        llvm::Type *IterType = mapType(For.IteratorType);
        auto *IterAlloca = createEntryBlockAlloca(IterType, "iter.alloca");
        Builder->CreateCall(GetIterFn, {IterAlloca, CollectionPtr});
        Iterator = IterAlloca;
    } else {
        // Function returns directly
        Iterator = Builder->CreateCall(GetIterFn, {CollectionPtr}, "iter");
        // Store iterator on stack for mutation by next()
        auto *IterAlloca = createEntryBlockAlloca(Iterator->getType(), "iter.alloca");
        Builder->CreateStore(Iterator, IterAlloca);
        Iterator = IterAlloca;
    }

    // 4. Get the next function
    llvm::Function *NextFn = lookupFunction(For.NextMethod);
    if (!NextFn) {
        return llvm::make_error<llvm::StringError>(
            "next method not found: " + For.NextMethod,
            llvm::inconvertibleErrorCode()
        );
    }

    // 5. Create basic blocks
    llvm::BasicBlock *CondBlock = createBlock("for.cond");
    llvm::BasicBlock *BodyBlock = createBlock("for.body");
    llvm::BasicBlock *ExitBlock = createBlock("for.exit");

    // Push loop context for break/continue
    LoopStack.push_back({ExitBlock, CondBlock, BlockCleanupStack.size()});

    // Jump to condition block
    Builder->CreateBr(CondBlock);

    // 6. Condition block: call next(), check if null
    Builder->SetInsertPoint(CondBlock);

    // Call next() with iterator pointer (procedures take this by pointer)
    llvm::Value *NextResult = Builder->CreateCall(NextFn, {Iterator}, "next.result");

    // Check if result is null
    llvm::Value *IsNull = Builder->CreateIsNull(NextResult, "is.null");
    Builder->CreateCondBr(IsNull, ExitBlock, BodyBlock);

    // 7. Body block
    Builder->SetInsertPoint(BodyBlock);

    // Loop variable is exactly what next() returns (no auto-dereference)
    LocalVariables[For.Identifier] = NextResult;

    // Execute loop body
    auto BodyResult = emitAction(For.Body);
    if (!BodyResult) {
        LoopStack.pop_back();
        return BodyResult.takeError();
    }

    // Jump back to condition (if not already terminated by break)
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(CondBlock);
    }

    // 8. Exit block
    Builder->SetInsertPoint(ExitBlock);
    LocalVariables.erase(For.Identifier);
    LoopStack.pop_back();

    return nullptr;
}

llvm::Expected<llvm::Value*> Emitter::emitWhile(const PlannedWhile &While) {
    // Create basic blocks for the loop structure
    llvm::BasicBlock *CondBlock = createBlock("while.cond");
    llvm::BasicBlock *BodyBlock = createBlock("while.body");
    llvm::BasicBlock *ExitBlock = createBlock("while.exit");

    // Push loop context for break/continue
    // For while loops, continue goes back to condition check
    LoopStack.push_back({ExitBlock, CondBlock, BlockCleanupStack.size()});

    // For "while let" bindings with mutable variables, create alloca before the loop
    llvm::AllocaInst *BindingAlloca = nullptr;
    bool HasBinding = While.Cond.BindingItem.Name != nullptr;
    bool IsMutable = While.Cond.BindingType == "var" || While.Cond.BindingType == "mutable";

    if (HasBinding && IsMutable) {
        // We need to create the alloca in the entry block for proper SSA form
        // First, we need to know the type - evaluate once to get it
        // For now, create a placeholder that will be set in the cond block
    }

    // Jump to condition block
    Builder->CreateBr(CondBlock);

    // Emit condition block
    Builder->SetInsertPoint(CondBlock);

    // Emit the condition binding
    // The condition's Operation contains the boolean expression
    if (While.Cond.Operation.empty()) {
        LoopStack.pop_back();
        return llvm::make_error<llvm::StringError>(
            "While condition has no expression",
            llvm::inconvertibleErrorCode()
        );
    }

    auto CondValueOrErr = emitOperands(While.Cond.Operation);
    if (!CondValueOrErr) {
        LoopStack.pop_back();
        return CondValueOrErr.takeError();
    }

    llvm::Value *CondValue = *CondValueOrErr;

    // Handle "while let" binding - bind the value to the variable
    if (HasBinding) {
        if (IsMutable) {
            // Mutable binding: allocate on stack and store
            if (!BindingAlloca) {
                // Create alloca in entry block for proper LLVM form
                BindingAlloca = createEntryBlockAlloca(CondValue->getType(), *While.Cond.BindingItem.Name);
            }
            Builder->CreateStore(CondValue, BindingAlloca);
            LocalVariables[*While.Cond.BindingItem.Name] = BindingAlloca;
        } else {
            // Immutable binding: just use the value directly
            // Each iteration will update this
            LocalVariables[*While.Cond.BindingItem.Name] = CondValue;
        }
    }

    // Ensure we have a boolean (i1) for the branch
    if (!CondValue->getType()->isIntegerTy(1)) {
        // Convert to boolean by comparing with zero/null
        if (CondValue->getType()->isIntegerTy()) {
            CondValue = Builder->CreateICmpNE(
                CondValue,
                llvm::ConstantInt::get(CondValue->getType(), 0),
                "while.tobool"
            );
        } else if (CondValue->getType()->isPointerTy()) {
            // Pointer-to-boolean: compare with null
            // This handles "while let x = ptr_returning_func()" patterns
            CondValue = Builder->CreateICmpNE(
                CondValue,
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(CondValue->getType())),
                "while.ptrtobool"
            );
        } else {
            LoopStack.pop_back();
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
    if (!BodyValueOrErr) {
        LoopStack.pop_back();
        return BodyValueOrErr.takeError();
    }

    // Jump back to condition (if not already terminated by break/continue)
    if (!Builder->GetInsertBlock()->getTerminator()) {
        Builder->CreateBr(CondBlock);
    }

    // Pop loop context
    LoopStack.pop_back();

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
        ResultPtr = createEntryBlockAlloca(ResultType, "try.result");
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

    // Extract the Ok data from the union (data is at index 1)
    // For now, extract as i64 to match typical catch/else types
    llvm::Value *OkDataPtr = Builder->CreateStructGEP(ResultType, ResultPtr, 1, "ok.data.ptr");
    llvm::Type *I64Ty = llvm::Type::getInt64Ty(*Context);
    llvm::Value *OkValue = Builder->CreateLoad(I64Ty, OkDataPtr, "ok.val");

    // Bind the value if there's a name
    if (!BindingName.empty()) {
        if (IsMutable) {
            llvm::AllocaInst *Alloca = createEntryBlockAlloca(I64Ty, BindingName);
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
                // Use the variant's tag from the planned union
                Switch->addCase(
                    llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(I8Ty, Catch.VariantIndex)),
                    CatchBlock
                );

                Builder->SetInsertPoint(CatchBlock);

                // Extract error data from union and bind to catch variable
                // Union layout: { i8 tag, [N x i8] data }
                if (!Catch.Name.empty()) {
                    llvm::Value *DataPtr = Builder->CreateStructGEP(
                        ResultType, ResultPtr, 1, "err.data.ptr");

                    // Cast to the variant's type and load
                    llvm::Type *VarTy = mapType(Catch.VariantType);
                    llvm::Value *DataCast = Builder->CreateBitCast(
                        DataPtr,
                        llvm::PointerType::getUnqual(VarTy),
                        "err.data.cast"
                    );
                    llvm::Value *ErrValue = Builder->CreateLoad(VarTy, DataCast, "err.val");

                    // Bind the error value to the catch variable name
                    LocalVariables[Catch.Name] = ErrValue;
                }

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

                // Capture the current block BEFORE adding the branch
                llvm::BasicBlock *CatchEndBlock = Builder->GetInsertBlock();

                if (!CatchEndBlock->getTerminator()) {
                    Builder->CreateBr(MergeBlock);
                    // Add to PHI if we have a value
                    if (CatchValue) {
                        IncomingValues.push_back({CatchValue, CatchEndBlock});
                    }
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

                // Capture the current block BEFORE adding the branch
                llvm::BasicBlock *ElseEndBlock = Builder->GetInsertBlock();

                if (!ElseEndBlock->getTerminator()) {
                    Builder->CreateBr(MergeBlock);
                    if (ElseValue) {
                        IncomingValues.push_back({ElseValue, ElseEndBlock});
                    }
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

            // Capture the current block BEFORE adding the branch
            llvm::BasicBlock *ElseEndBlock = Builder->GetInsertBlock();

            if (!ElseEndBlock->getTerminator()) {
                Builder->CreateBr(MergeBlock);
                if (ElseValue) {
                    IncomingValues.push_back({ElseValue, ElseEndBlock});
                }
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

llvm::Expected<llvm::Value*> Emitter::emitAlignOf(const PlannedAlignOf &AlignOf) {
    // The alignment is already computed by the Planner - just emit it as a constant
    // alignof returns a size_t (u64)
    llvm::Type *SizeTy = llvm::Type::getInt64Ty(*Context);
    return llvm::ConstantInt::get(SizeTy, AlignOf.Alignment);
}

llvm::Expected<llvm::Value*> Emitter::emitIs(const PlannedIs &Is) {
    // 'is' expression tests if a union value is of a specific variant type
    if (!Is.Value) {
        // No value to test - this shouldn't happen in well-formed code
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*Context), 0);
    }

    // Emit the union value
    auto UnionValueOrErr = emitOperand(*Is.Value);
    if (!UnionValueOrErr)
        return UnionValueOrErr.takeError();

    llvm::Value *Value = *UnionValueOrErr;

    // Handle "is null" for optional types
    if (Is.IsNullCheck) {
        // Check if this is an Option type and if it uses NPO
        const PlannedType &ValueType = Is.Value->ResultType;
        const PlannedType *OptionType = &ValueType;
        bool ValueIsPointer = false;
        if ((ValueType.Name == "pointer" || ValueType.Name == "ref") && !ValueType.Generics.empty()) {
            OptionType = &ValueType.Generics[0];
            ValueIsPointer = true;
        }

        bool IsOption = OptionType->Name == "Option" ||
                        (OptionType->Name.size() > 7 && OptionType->Name.substr(0, 7) == "Option.");
        bool UsesNPO = false;
        if (IsOption && !OptionType->Generics.empty()) {
            const PlannedType &InnerType = OptionType->Generics[0];
            UsesNPO = InnerType.Name == "pointer" || InnerType.Name == "ref" ||
                      (InnerType.Name.size() >= 8 && InnerType.Name.substr(0, 8) == "pointer.") ||
                      (InnerType.Name.size() >= 4 && InnerType.Name.substr(0, 4) == "ref.");
        }
        // Also check instantiated names like "Option.ref", "Option.pointer"
        if (!UsesNPO && OptionType->Name.size() > 7 && OptionType->Name.substr(0, 7) == "Option.") {
            std::string_view Inner = std::string_view(OptionType->Name).substr(7);
            UsesNPO = Inner.substr(0, 3) == "ref" || Inner.substr(0, 7) == "pointer";
        }

        if (UsesNPO) {
            // For Option[pointer[T]] with NPO, the value is a pointer - null means None
            if (ValueIsPointer) {
                // Load the Option value from the pointer
                Value = Builder->CreateLoad(llvm::PointerType::get(*Context, 0), Value, "option.val");
            }
            llvm::Value *IsNull = Builder->CreateIsNull(Value, "is.null");
            return IsNull;
        } else {
            // For non-NPO Option[T] (e.g., Option[int]), check if tag == 1 (None)
            // None is typically tag 1 (second variant after Some)
            PlannedIs NoneIs = Is;
            NoneIs.IsNullCheck = false;
            NoneIs.VariantTag = 1;  // None's tag
            return emitIsWithValue(Value, *OptionType, NoneIs);
        }
    }

    const PlannedType &UnionType = Is.Value->ResultType;
    return emitIsWithValue(Value, UnionType, Is);
}

llvm::Expected<llvm::Value*> Emitter::emitIsWithValue(
    llvm::Value *UnionValue,
    const PlannedType &UnionType,
    const PlannedIs &Is) {

    // 'is' expression: test if a union value is of a specific variant type
    // Union layout: { i8 tag, [maxSize x i8] data }
    // We compare the tag against the expected variant's tag

    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
    llvm::Type *I1Ty = llvm::Type::getInt1Ty(*Context);

    // Get pointer to the union (may already be a pointer or need alloca)
    llvm::Value *UnionPtr;
    llvm::Type *LLVMUnionType = UnionValue->getType();

    if (UnionValue->getType()->isPointerTy()) {
        UnionPtr = UnionValue;
        // Need to get the actual struct type for GEP
        // Try to find it from the StructCache
        // Use MangledName as that's the key used in emitUnionType
        auto CacheIt = StructCache.find(UnionType.MangledName);
        if (CacheIt == StructCache.end()) {
            // Fallback to Name in case it's stored differently
            CacheIt = StructCache.find(UnionType.Name);
        }
        if (CacheIt == StructCache.end()) {
            // Try with _Z prefix
            CacheIt = StructCache.find("_Z" + UnionType.MangledName);
        }
        if (CacheIt != StructCache.end()) {
            LLVMUnionType = CacheIt->second;
        } else {
            // Fallback: assume a generic struct layout
            // This shouldn't normally happen for well-typed code
            return llvm::make_error<llvm::StringError>(
                "Cannot find union type for 'is' expression: " + UnionType.Name,
                llvm::inconvertibleErrorCode()
            );
        }
    } else {
        // Store the union value to get a pointer
        UnionPtr = createEntryBlockAlloca(LLVMUnionType, "is.union");
        Builder->CreateStore(UnionValue, UnionPtr);
    }

    // Load the tag (first field, i8)
    llvm::Value *TagPtr = Builder->CreateStructGEP(LLVMUnionType, UnionPtr, 0, "is.tag.ptr");
    llvm::Value *Tag = Builder->CreateLoad(I8Ty, TagPtr, "is.tag");

    // Compare with expected variant tag
    llvm::Value *ExpectedTag = llvm::ConstantInt::get(I8Ty, Is.VariantTag);
    llvm::Value *Result = Builder->CreateICmpEQ(Tag, ExpectedTag, "is.result");

    return Result;
}

llvm::Expected<llvm::Value*> Emitter::emitAs(const PlannedAs &As) {
    // 'as' expression: cast a value to a target type
    // value as Type

    // First, emit the value being cast
    if (!As.Value) {
        return llvm::make_error<llvm::StringError>(
            "As expression missing value",
            llvm::inconvertibleErrorCode()
        );
    }

    auto ValueOrErr = emitOperand(*As.Value);
    if (!ValueOrErr)
        return ValueOrErr.takeError();

    llvm::Value *SrcValue = *ValueOrErr;
    llvm::Type *SrcTy = SrcValue->getType();

    // Map the target type to LLVM type
    llvm::Type *DstTy = mapType(As.TargetType);
    if (!DstTy) {
        return llvm::make_error<llvm::StringError>(
            "Failed to map target type: " + As.TargetType.Name,
            llvm::inconvertibleErrorCode()
        );
    }

    // If types are the same, no cast needed
    if (SrcTy == DstTy) {
        return SrcValue;
    }

    // Determine the appropriate cast based on source and target types
    bool SrcIsPtr = SrcTy->isPointerTy();
    bool DstIsPtr = DstTy->isPointerTy();
    bool SrcIsInt = SrcTy->isIntegerTy();
    bool DstIsInt = DstTy->isIntegerTy();
    bool SrcIsFloat = SrcTy->isFloatingPointTy();
    bool DstIsFloat = DstTy->isFloatingPointTy();

    // Pointer to pointer: bitcast
    if (SrcIsPtr && DstIsPtr) {
        return Builder->CreateBitCast(SrcValue, DstTy, "as.ptr");
    }

    // Pointer to integer: ptrtoint
    if (SrcIsPtr && DstIsInt) {
        return Builder->CreatePtrToInt(SrcValue, DstTy, "as.ptrtoint");
    }

    // Integer to pointer: inttoptr
    if (SrcIsInt && DstIsPtr) {
        return Builder->CreateIntToPtr(SrcValue, DstTy, "as.inttoptr");
    }

    // Integer to integer: trunc, zext, or sext based on size
    if (SrcIsInt && DstIsInt) {
        unsigned SrcBits = SrcTy->getIntegerBitWidth();
        unsigned DstBits = DstTy->getIntegerBitWidth();

        if (SrcBits > DstBits) {
            return Builder->CreateTrunc(SrcValue, DstTy, "as.trunc");
        } else if (SrcBits < DstBits) {
            // Check if the source type is signed
            // For now, use zext (zero extend) for unsigned types
            // We'd need type info to know if signed extension is needed
            // Use zext by default as it's safer for most pointer-related casts
            return Builder->CreateZExt(SrcValue, DstTy, "as.zext");
        }
        // Same size, different int types - no-op
        return SrcValue;
    }

    // Float to integer: fptosi or fptoui
    if (SrcIsFloat && DstIsInt) {
        // Use signed conversion by default
        return Builder->CreateFPToSI(SrcValue, DstTy, "as.fptosi");
    }

    // Integer to float: sitofp or uitofp
    if (SrcIsInt && DstIsFloat) {
        // Use signed conversion by default
        return Builder->CreateSIToFP(SrcValue, DstTy, "as.sitofp");
    }

    // Float to float: fpext or fptrunc
    if (SrcIsFloat && DstIsFloat) {
        unsigned SrcBits = SrcTy->getPrimitiveSizeInBits();
        unsigned DstBits = DstTy->getPrimitiveSizeInBits();

        if (SrcBits > DstBits) {
            return Builder->CreateFPTrunc(SrcValue, DstTy, "as.fptrunc");
        } else {
            return Builder->CreateFPExt(SrcValue, DstTy, "as.fpext");
        }
    }

    // Fallback: struct to pointer or other invalid casts
    // Instead of creating invalid bitcast, emit an error
    std::string SrcTypeName, DstTypeName;
    llvm::raw_string_ostream SrcStream(SrcTypeName), DstStream(DstTypeName);
    SrcTy->print(SrcStream);
    DstTy->print(DstStream);
    std::string LocInfo;
    if (As.Value) {
        LocInfo = " at pos " + std::to_string(As.Value->Loc.Start);
    }
    return llvm::make_error<llvm::StringError>(
        "Cannot cast from " + SrcTypeName + " to " + DstTypeName +
        " (as " + As.TargetType.Name + " from value type " +
        (As.Value ? As.Value->ResultType.Name : "unknown") + ")" + LocInfo,
        llvm::inconvertibleErrorCode()
    );
}

llvm::Expected<llvm::Value*> Emitter::emitVariantConstruction(
    const PlannedVariantConstruction &Construct) {
    // Handle Option with NPO (Null Pointer Optimization)
    // NPO only applies when the inner type is pointer or ref
    // UnionType.Name may be "Option" (without generics) or "Option.int" (with generics)
    bool IsOption = Construct.UnionType.Name == "Option" ||
                    Construct.UnionType.Name.substr(0, 7) == "Option.";
    bool UseNPO = false;
    if (IsOption && !Construct.UnionType.Generics.empty()) {
        const PlannedType &InnerType = Construct.UnionType.Generics[0];
        UseNPO = InnerType.Name == "pointer" || InnerType.Name == "ref" ||
                 (InnerType.Name.size() >= 8 && InnerType.Name.substr(0, 8) == "pointer.") ||
                 (InnerType.Name.size() >= 4 && InnerType.Name.substr(0, 4) == "ref.");
    }
    // Also handle instantiated Option names like "Option.ref", "Option.pointer"
    if (!UseNPO && Construct.UnionType.Name.size() > 7 &&
        Construct.UnionType.Name.substr(0, 7) == "Option.") {
        std::string_view Inner = std::string_view(Construct.UnionType.Name).substr(7);
        UseNPO = Inner.substr(0, 3) == "ref" || Inner.substr(0, 7) == "pointer";
    }

    if (UseNPO) {
        auto *PtrTy = llvm::PointerType::get(*Context, 0);

        if (Construct.VariantName == "None") {
            // None = null pointer
            return llvm::ConstantPointerNull::get(PtrTy);
        } else if (Construct.VariantName == "Some" && Construct.Value) {
            // Some(value) = pointer to allocated value
            auto ValueOrErr = emitOperand(*Construct.Value);
            if (!ValueOrErr)
                return ValueOrErr.takeError();

            llvm::Value *Value = *ValueOrErr;
            llvm::Type *ValueTy = Value->getType();

            // Allocate space for the value on the stack
            llvm::Value *ValuePtr = createEntryBlockAlloca(ValueTy, "option.some");
            Builder->CreateStore(Value, ValuePtr);

            return ValuePtr;
        }
    }

    // Construct a union variant: { i8 tag, [size x i8] data }
    // Similar to throw, but with an arbitrary tag value

    // Look up the union type from the cache (should have been emitted already)
    // Use MangledName as that's the key used in emitUnionType
    auto CacheIt = StructCache.find(Construct.UnionType.MangledName);
    if (CacheIt == StructCache.end()) {
        // Fallback to Name in case it's stored differently
        CacheIt = StructCache.find(Construct.UnionType.Name);
    }
    if (CacheIt == StructCache.end()) {
        // Try with _Z prefix
        CacheIt = StructCache.find("_Z" + Construct.UnionType.MangledName);
    }
    if (CacheIt == StructCache.end()) {
        return llvm::make_error<llvm::StringError>(
            "Union type not found: " + Construct.UnionType.Name,
            llvm::inconvertibleErrorCode()
        );
    }
    llvm::StructType *UnionTy = CacheIt->second;

    // Allocate space for the union
    llvm::Value *UnionPtr = createEntryBlockAlloca(UnionTy, "variant.ptr");

    // Store the tag (field 0)
    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);
    llvm::Value *TagPtr = Builder->CreateStructGEP(UnionTy, UnionPtr, 0, "variant.tag.ptr");
    Builder->CreateStore(llvm::ConstantInt::get(I8Ty, Construct.VariantTag), TagPtr);

    // Store the value (field 1) if present
    if (Construct.Value) {
        auto ValueOrErr = emitOperand(*Construct.Value);
        if (!ValueOrErr)
            return ValueOrErr.takeError();

        llvm::Value *Value = *ValueOrErr;
        llvm::Value *DataPtr = Builder->CreateStructGEP(UnionTy, UnionPtr, 1, "variant.data.ptr");

        // Bitcast the data pointer to the value's type and store
        llvm::Type *ValueTy = Value->getType();
        llvm::Value *DataCast = Builder->CreateBitCast(
            DataPtr,
            llvm::PointerType::getUnqual(ValueTy),
            "variant.data.cast"
        );
        Builder->CreateStore(Value, DataCast);
    }

    // Load and return the union value
    return Builder->CreateLoad(UnionTy, UnionPtr, "variant.val");
}

llvm::Expected<llvm::Value*> Emitter::emitTuple(const PlannedTuple &Tuple) {
    // Handle grouped expressions (single anonymous component)
    // vs actual tuples (multiple or named components)
    // But NOT for struct construction - if TupleType is a struct, we must create a struct

    // Check if this is struct construction (TupleType names a user-defined struct/union)
    // We need to distinguish between:
    // - Grouped expressions like (2 + 3) where TupleType might be "int" (result type)
    // - Actual struct construction like Container(value) where TupleType is "Container"
    bool IsStructConstruction = false;
    llvm::StructType *PreEmittedStructTy = nullptr;
    if (!Tuple.TupleType.Name.empty() && Tuple.TupleType.Name != "Tuple") {
        // Check if it's already in the StructCache (from previous emission)
        if (auto It = StructCache.find(Tuple.TupleType.MangledName); It != StructCache.end()) {
            IsStructConstruction = true;
            PreEmittedStructTy = It->second;
        } else if (auto It = StructCache.find(Tuple.TupleType.Name); It != StructCache.end()) {
            IsStructConstruction = true;
            PreEmittedStructTy = It->second;
        }
        // For single-component tuples that aren't in StructCache, check the Plan.
        // This handles inline struct definitions like `define Container(value: Option)`.
        // We only do this for single-component tuples if the struct/union has exactly 1 property,
        // to distinguish from grouped expressions like (*p) where the TupleType might be set
        // to the result type but it's not actually struct construction.
        if (!IsStructConstruction && Tuple.Components.size() == 1 && CurrentPlan) {
            // Check if it's a struct in the plan with exactly 1 property
            auto StructIt = CurrentPlan->Structures.find(Tuple.TupleType.Name);
            if (StructIt != CurrentPlan->Structures.end() &&
                StructIt->second.Properties.size() == 1) {
                IsStructConstruction = true;
                PreEmittedStructTy = emitStructType(StructIt->second);
            }
            if (!IsStructConstruction) {
                // Check by mangled name
                for (const auto &[key, s] : CurrentPlan->Structures) {
                    if (s.MangledName == Tuple.TupleType.MangledName &&
                        s.Properties.size() == 1) {
                        IsStructConstruction = true;
                        PreEmittedStructTy = emitStructType(s);
                        break;
                    }
                }
            }
            // Check unions too - for unions with exactly 1 variant (degenerate case)
            if (!IsStructConstruction) {
                auto UnionIt = CurrentPlan->Unions.find(Tuple.TupleType.Name);
                if (UnionIt != CurrentPlan->Unions.end() &&
                    UnionIt->second.Variants.size() == 1) {
                    IsStructConstruction = true;
                    PreEmittedStructTy = emitUnionType(UnionIt->second);
                }
                if (!IsStructConstruction) {
                    for (const auto &[key, u] : CurrentPlan->Unions) {
                        if (u.MangledName == Tuple.TupleType.MangledName &&
                            u.Variants.size() == 1) {
                            IsStructConstruction = true;
                            PreEmittedStructTy = emitUnionType(u);
                            break;
                        }
                    }
                }
            }
        }
    }

    if (Tuple.Components.size() == 1 && !Tuple.Components[0].Name && !IsStructConstruction) {
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

    // Get the struct type - either from pre-emitted type, TupleType lookup, or create anonymous
    llvm::StructType *TupleTy = PreEmittedStructTy;
    if (!TupleTy && !Tuple.TupleType.Name.empty() && Tuple.TupleType.Name != "Tuple") {
        // Named struct type - look it up in cache
        auto It = StructCache.find(Tuple.TupleType.MangledName);
        if (It == StructCache.end()) {
            It = StructCache.find(Tuple.TupleType.Name);
        }
        if (It != StructCache.end()) {
            TupleTy = It->second;
        }
    }
    if (!TupleTy) {
        // Create an anonymous struct type for the tuple
        TupleTy = llvm::StructType::get(*Context, ComponentTypes);
    }

    llvm::Value *TuplePtr = nullptr;

    if (Tuple.IsRegionAlloc && PageAllocate) {
        // Determine which page to use based on lifetime
        llvm::Value *Page = nullptr;

        if (std::holds_alternative<LocalLifetime>(Tuple.Life)) {
            // $ = local page
            Page = CurrentRegion.LocalPage;
        } else if (std::holds_alternative<CallLifetime>(Tuple.Life)) {
            // # = caller's return page
            Page = CurrentRegion.ReturnPage;
        } else if (std::holds_alternative<ReferenceLifetime>(Tuple.Life) && Tuple.RegionArg) {
            // ^name = emit the region argument
            auto PageOrErr = emitOperand(*Tuple.RegionArg);
            if (!PageOrErr)
                return PageOrErr.takeError();
            Page = *PageOrErr;
        }

        if (Page) {
            auto *I64Ty = llvm::Type::getInt64Ty(*Context);
            size_t TypeSize = getTypeSize(TupleTy);
            size_t TypeAlign = getTypeAlignment(TupleTy);
            auto *Size = llvm::ConstantInt::get(I64Ty, TypeSize);
            auto *Align = llvm::ConstantInt::get(I64Ty, TypeAlign);
            TuplePtr = Builder->CreateCall(PageAllocate, {Page, Size, Align}, "tuple.region");
        } else {
            // Fallback to stack if page not available
            TuplePtr = createEntryBlockAlloca(TupleTy, "tuple");
        }
    } else {
        // Stack allocation (no lifetime suffix)
        TuplePtr = createEntryBlockAlloca(TupleTy, "tuple");
    }

    // Store provided component values
    // Skip if tuple type has no elements (empty struct like Unspecified)
    unsigned NumStructFields = TupleTy->getNumElements();
    for (size_t i = 0; i < ComponentValues.size() && i < NumStructFields; ++i) {
        llvm::Value *FieldPtr = Builder->CreateStructGEP(TupleTy, TuplePtr, i, "tuple.field");
        Builder->CreateStore(ComponentValues[i], FieldPtr);
    }

    // Initialize remaining fields with null/zero values (for struct constructors with defaults)
    for (size_t i = ComponentValues.size(); i < NumStructFields; ++i) {
        llvm::Type *FieldTy = TupleTy->getElementType(i);
        llvm::Value *DefaultVal;
        if (FieldTy->isPointerTy()) {
            DefaultVal = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(FieldTy));
        } else if (FieldTy->isIntegerTy()) {
            DefaultVal = llvm::ConstantInt::get(FieldTy, 0);
        } else if (FieldTy->isFloatingPointTy()) {
            DefaultVal = llvm::ConstantFP::get(FieldTy, 0.0);
        } else if (FieldTy->isStructTy()) {
            // For nested structs, use zeroinitializer
            DefaultVal = llvm::Constant::getNullValue(FieldTy);
        } else {
            DefaultVal = llvm::Constant::getNullValue(FieldTy);
        }
        llvm::Value *FieldPtr = Builder->CreateStructGEP(TupleTy, TuplePtr, i, "tuple.field.default");
        Builder->CreateStore(DefaultVal, FieldPtr);
    }

    if (Tuple.IsRegionAlloc) {
        // Return the pointer for region allocation
        return TuplePtr;
    }

    // Load and return the tuple value for stack allocation
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
        llvm::Value *ArrPtr = createEntryBlockAlloca(ArrTy, "array");

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
        llvm::Value *MatrixPtr = createEntryBlockAlloca(MatrixTy, "matrix");

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

llvm::AllocaInst *Emitter::createEntryBlockAlloca(llvm::Type *Ty, llvm::StringRef Name) {
    // Create an alloca in the entry block of the current function
    // This ensures allocas don't grow the stack inside loops
    llvm::IRBuilder<> TmpBuilder(&CurrentFunction->getEntryBlock(),
                                  CurrentFunction->getEntryBlock().begin());
    if (AllocaInsertPt) {
        // Insert after the alloca insertion point marker
        if (auto *NextNode = AllocaInsertPt->getNextNode()) {
            TmpBuilder.SetInsertPoint(NextNode);
        } else {
            // Marker is at end of block, insert at end of entry block
            TmpBuilder.SetInsertPoint(&CurrentFunction->getEntryBlock());
        }
    }
    return TmpBuilder.CreateAlloca(Ty, nullptr, Name);
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

void Emitter::emitBlockCleanups(size_t TargetDepth) {
    // Emit restore_watermark for each block that needs cleanup,
    // from innermost to TargetDepth (not including TargetDepth)
    // We iterate in reverse order (innermost first)
    // Note: Only emit calls if Page.restore_watermark is actually defined
    if (!PageRestoreWatermark || PageRestoreWatermark->isDeclaration())
        return;

    for (size_t i = BlockCleanupStack.size(); i > TargetDepth; --i) {
        const auto &Cleanup = BlockCleanupStack[i - 1];
        if (Cleanup.NeedsCleanup && Cleanup.Watermark && CurrentRegion.LocalPage) {
            Builder->CreateCall(PageRestoreWatermark,
                               {CurrentRegion.LocalPage, Cleanup.Watermark});
        }
    }
}

llvm::Expected<llvm::Value*> Emitter::checkAndPropagateError(
    llvm::Value *ResultPtr,
    llvm::Type *ResultType,
    llvm::Value *CalleeExceptionPage) {
    // Result is a union: { i8 tag, [N x i8] data }
    // Tag 0 = Ok (success), Tag != 0 = Error

    llvm::Type *I8Ty = llvm::Type::getInt8Ty(*Context);

    // Load the tag
    llvm::Value *TagPtr = Builder->CreateStructGEP(ResultType, ResultPtr, 0, "err.tag.ptr");
    llvm::Value *Tag = Builder->CreateLoad(I8Ty, TagPtr, "err.tag");

    // Check if error (tag != 0)
    llvm::Value *IsError = Builder->CreateICmpNE(Tag, llvm::ConstantInt::get(I8Ty, 0), "is.error");

    // Create blocks
    llvm::BasicBlock *ErrorBlock = createBlock("call.error");
    llvm::BasicBlock *OkBlock = createBlock("call.ok");

    Builder->CreateCondBr(IsError, ErrorBlock, OkBlock);

    // Error block: propagate or crash
    Builder->SetInsertPoint(ErrorBlock);

    if (CurrentFunctionCanThrow && CurrentRegion.ExceptionPage) {
        // Propagate error to caller
        bool HasSret = CurrentFunction && CurrentFunction->arg_size() > 0 &&
                       CurrentFunction->hasParamAttribute(0, llvm::Attribute::StructRet);

        if (HasSret) {
            // Copy result to sret pointer and return void
            llvm::Value *SRetPtr = CurrentFunction->getArg(0);
            llvm::Type *SRetTy = CurrentFunction->getParamStructRetType(0);

            // Copy the error result (tag and data) to sret
            llvm::Value *ResultVal = Builder->CreateLoad(ResultType, ResultPtr, "err.result");
            Builder->CreateStore(ResultVal, SRetPtr);
            Builder->CreateRetVoid();
        } else {
            // Return the error result directly
            llvm::Value *ResultVal = Builder->CreateLoad(ResultType, ResultPtr, "err.result");
            Builder->CreateRet(ResultVal);
        }
    } else {
        // Current function cannot throw - unhandled error, call exit(1)
        if (ExitFunc) {
            Builder->CreateCall(ExitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), 1)});
        }
        Builder->CreateUnreachable();
    }

    // Ok block: extract success value
    Builder->SetInsertPoint(OkBlock);

    // Deallocate the exception page from the callee (it's no longer needed)
    if (CalleeExceptionPage && PageDeallocateExtensions &&
        !PageDeallocateExtensions->isDeclaration()) {
        Builder->CreateCall(PageDeallocateExtensions, {CalleeExceptionPage});
    }

    // Extract success data from index 1
    // For now, return as i64 - caller may need to cast appropriately
    llvm::Value *DataPtr = Builder->CreateStructGEP(ResultType, ResultPtr, 1, "ok.data.ptr");
    llvm::Type *I64Ty = llvm::Type::getInt64Ty(*Context);
    llvm::Value *OkValue = Builder->CreateLoad(I64Ty, DataPtr, "ok.val");

    return OkValue;
}

llvm::Value *Emitter::allocate(llvm::Type *Ty, Lifetime Life, llvm::StringRef Name) {
    // Get the appropriate page for this lifetime
    llvm::Value *Page = getRegionPage(Life);

    // If no page available (UnspecifiedLifetime or no region set up), use stack allocation
    if (!Page) {
        return createEntryBlockAlloca(Ty, Name);
    }

    // Calculate size and alignment
    auto *I64Ty = llvm::Type::getInt64Ty(*Context);
    size_t TypeSize = getTypeSize(Ty);
    size_t TypeAlign = getTypeAlignment(Ty);

    auto *Size = llvm::ConstantInt::get(I64Ty, TypeSize);
    auto *Align = llvm::ConstantInt::get(I64Ty, TypeAlign);

    // Call Page.allocate(page, size, align)
    llvm::Value *RawPtr = Builder->CreateCall(PageAllocate, {Page, Size, Align}, Name);

    // The result is void*, but we need the correct pointer type for LLVM
    // In opaque pointer mode (LLVM 15+), all pointers are ptr, so no cast needed
    return RawPtr;
}

llvm::Value *Emitter::getRegionPage(Lifetime Life) {
    if (std::holds_alternative<LocalLifetime>(Life)) {
        return CurrentRegion.LocalPage;
    } else if (std::holds_alternative<CallLifetime>(Life)) {
        return CurrentRegion.ReturnPage;
    } else if (std::holds_alternative<ThrownLifetime>(Life)) {
        return CurrentRegion.ExceptionPage;
    } else if (std::holds_alternative<ReferenceLifetime>(Life)) {
        // Look up the named region from the current scope
        const auto &RefLife = std::get<ReferenceLifetime>(Life);
        auto It = CurrentRegion.NamedRegions.find(RefLife.Location);
        if (It != CurrentRegion.NamedRegions.end()) {
            return It->second;
        }
        // Named region not found - fall back to local page
        return CurrentRegion.LocalPage;
    }
    // UnspecifiedLifetime - use stack allocation (return nullptr)
    return nullptr;
}

// ============================================================================
// Main Function Generation
// ============================================================================

llvm::Error Emitter::emitMainWrapper(const Plan &P) {
    // Create main function: int main(int argc, char** argv)
    auto *Int32Ty = llvm::Type::getInt32Ty(*Context);
    auto *Int8PtrPtrTy = llvm::PointerType::getUnqual(*Context);

    auto *MainTy = llvm::FunctionType::get(
        Int32Ty,
        {Int32Ty, Int8PtrPtrTy},
        false
    );

    auto *MainFunc = llvm::Function::Create(
        MainTy,
        llvm::GlobalValue::ExternalLinkage,
        "main",
        *Module
    );

    // Name the arguments
    auto ArgIt = MainFunc->arg_begin();
    ArgIt->setName("argc");
    ++ArgIt;
    ArgIt->setName("argv");

    // Create entry block
    auto *EntryBB = llvm::BasicBlock::Create(*Context, "entry", MainFunc);
    Builder->SetInsertPoint(EntryBB);

    // Create a dummy instruction as an alloca insertion point marker
    AllocaInsertPt = new llvm::BitCastInst(
        llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*Context)),
        llvm::Type::getInt32Ty(*Context), "alloca.point", EntryBB);

    // Set up current function state
    CurrentFunction = MainFunc;
    CurrentBlock = EntryBB;
    LocalVariables.clear();

    // Emit top-level statements
    llvm::Value *LastValue = nullptr;
    for (const auto &Stmt : P.Statements) {
        if (auto *Action = std::get_if<PlannedAction>(&Stmt)) {
            auto ValueOrErr = emitAction(*Action);
            if (!ValueOrErr) {
                return ValueOrErr.takeError();
            }
            // If not an assignment, save value as potential result
            if (Action->Target.empty()) {
                LastValue = *ValueOrErr;
            }
        } else if (auto *Binding = std::get_if<PlannedBinding>(&Stmt)) {
            if (auto Err = emitBinding(*Binding)) {
                return Err;
            }
        } else if (auto *Return = std::get_if<PlannedReturn>(&Stmt)) {
            auto ValueOrErr = emitOperands(Return->Result);
            if (!ValueOrErr) {
                return ValueOrErr.takeError();
            }
            LastValue = *ValueOrErr;
        }
    }

    // Programs always return 0 by default (success exit code)
    // Unlike functions, program top-level statements are executed for side effects
    // Use explicit "return N" in top-level code to return a specific exit code
    (void)LastValue;  // Intentionally unused for programs
    Builder->CreateRet(llvm::ConstantInt::get(Int32Ty, 0));

    return llvm::Error::success();
}

// ============================================================================
// JIT Execution
// ============================================================================

llvm::Function *Emitter::createJITWrapper(llvm::Type *ReturnType) {
    // Create a wrapper function: always returns i64 __scaly_jit_main()
    // For floats, we bitcast to i64 before returning (ARM64 uses different registers)
    (void)ReturnType;  // Unused - always return i64
    auto *I64Ty = llvm::Type::getInt64Ty(*Context);
    auto *WrapperTy = llvm::FunctionType::get(I64Ty, {}, false);
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
    // Store current plan for type lookups
    CurrentPlan = &P;

    // Create fresh module for JIT
    Module = std::make_unique<llvm::Module>("jit_module", *Context);
    Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    // Clear caches
    TypeCache.clear();
    StructCache.clear();
    FunctionCache.clear();

    // Initialize RBMM runtime declarations only if Page is not in the Plan
    // (If Page is in the Plan, its methods will be emitted from the Plan)
    bool PageInPlan = P.Structures.find("Page") != P.Structures.end();
    if (!PageInPlan) {
        initRBMM();
    }

    // Emit type declarations
    for (const auto &[name, Struct] : P.Structures) {
        emitStructType(Struct);
    }
    for (const auto &[name, Union] : P.Unions) {
        emitUnionType(Union);
    }

    // Emit global constants
    for (const auto &[name, Global] : P.Globals) {
        emitGlobal(Global);
    }

    // Emit function declarations
    for (const auto &[name, Func] : P.Functions) {
        emitFunctionDecl(Func);
    }
    // Also emit method, operator, initializer, and deinitializer declarations from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            emitFunctionDecl(Method);
        }
        for (const auto &Op : Struct.Operators) {
            // Convert PlannedOperator to PlannedFunction for emission
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;  // Operators are always pure
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
        for (const auto &Init : Struct.Initializers) {
            emitInitializerDecl(Struct, Init);
        }
        if (Struct.Deinitializer) {
            emitDeInitializerDecl(Struct, *Struct.Deinitializer);
        }
    }

    // If Page is in the Plan, set up the RBMM function pointers from the emitted declarations
    if (PageInPlan) {
        // Declare C runtime functions needed by Page.scaly
        declareRuntimeFunctions();

        PageAllocatePage = FunctionCache["_ZN4Page13allocate_pageEv"];
        PageAllocate = FunctionCache["_ZN4Page8allocateEmm"];  // size_t = unsigned long (m)
        PageDeallocateExtensions = FunctionCache["_ZN4Page21deallocate_extensionsEv"];
        // Also set up PageType from StructCache (using mangled name)
        if (auto It = StructCache.find("_Z4Page"); It != StructCache.end()) {
            PageType = It->second;
        }
    }

    // Emit function bodies
    for (const auto &[name, Func] : P.Functions) {
        if (auto *LLVMFunc = FunctionCache[Func.MangledName]) {
            if (auto Err = emitFunctionBody(Func, LLVMFunc))
                return std::move(Err);
        }
    }
    // Also emit method, operator, initializer, and deinitializer bodies from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return std::move(Err);
            }
        }
        for (const auto &Op : Struct.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                // Convert PlannedOperator to PlannedFunction for emission
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return std::move(Err);
            }
        }
        for (const auto &Init : Struct.Initializers) {
            if (auto *LLVMFunc = FunctionCache[Init.MangledName]) {
                if (auto Err = emitInitializerBody(Struct, Init, LLVMFunc))
                    return std::move(Err);
            }
        }
        if (Struct.Deinitializer) {
            if (auto *LLVMFunc = FunctionCache[Struct.Deinitializer->MangledName]) {
                if (auto Err = emitDeInitializerBody(Struct, *Struct.Deinitializer, LLVMFunc))
                    return std::move(Err);
            }
        }
    }
    // Also emit method and operator bodies from unions (e.g., Option.unwrap)
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return std::move(Err);
            }
        }
        for (const auto &Op : Union.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return std::move(Err);
            }
        }
    }

    // Create JIT wrapper function
    auto *Wrapper = createJITWrapper(ExpectedType);
    CurrentFunction = Wrapper;
    LocalVariables.clear();

    // Set up alloca insertion point for the JIT wrapper
    // Insert the marker at the beginning of the entry block
    auto &EntryBlock = Wrapper->getEntryBlock();
    AllocaInsertPt = new llvm::BitCastInst(
        llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*Context)),
        llvm::Type::getInt32Ty(*Context), "alloca.point", &EntryBlock);

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
    // Wrapper always returns i64, so we may need to convert
    auto *I64Ty = llvm::Type::getInt64Ty(*Context);
    if (LastValue) {
        // Cast to expected type first if needed
        if (LastValue->getType() != ExpectedType) {
            if (ExpectedType->isIntegerTy() && LastValue->getType()->isIntegerTy()) {
                LastValue = Builder->CreateIntCast(LastValue, ExpectedType, true);
            } else if (ExpectedType->isDoubleTy() && LastValue->getType()->isFloatTy()) {
                LastValue = Builder->CreateFPExt(LastValue, ExpectedType);
            } else if (ExpectedType->isFloatTy() && LastValue->getType()->isDoubleTy()) {
                LastValue = Builder->CreateFPTrunc(LastValue, ExpectedType);
            }
        }
        // Now convert to i64 for the wrapper return
        if (LastValue->getType()->isFloatingPointTy()) {
            // Bitcast float/double to i64
            if (LastValue->getType()->isFloatTy()) {
                // f32 -> i32 -> i64
                auto *I32Val = Builder->CreateBitCast(LastValue, llvm::Type::getInt32Ty(*Context));
                LastValue = Builder->CreateZExt(I32Val, I64Ty);
            } else {
                // f64 -> i64
                LastValue = Builder->CreateBitCast(LastValue, I64Ty);
            }
        } else if (LastValue->getType()->isIntegerTy() && LastValue->getType() != I64Ty) {
            LastValue = Builder->CreateIntCast(LastValue, I64Ty, true);
        } else if (LastValue->getType()->isPointerTy()) {
            LastValue = Builder->CreatePtrToInt(LastValue, I64Ty);
        }
        Builder->CreateRet(LastValue);
    } else {
        // Return default value (0)
        Builder->CreateRet(llvm::ConstantInt::get(I64Ty, 0));
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

    // Add process symbol generator to resolve external functions like aligned_alloc, free, exit
    auto &MainJD = JIT->getMainJITDylib();
    auto ProcessSymbolsGenerator = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
        JIT->getDataLayout().getGlobalPrefix());
    if (!ProcessSymbolsGenerator) {
        return ProcessSymbolsGenerator.takeError();
    }
    MainJD.addGenerator(std::move(*ProcessSymbolsGenerator));

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

llvm::Expected<const char*> Emitter::jitExecuteString(const Plan &P) {
    auto ResultOrErr = jitExecuteRaw(P, llvm::PointerType::getUnqual(*Context));
    if (!ResultOrErr)
        return ResultOrErr.takeError();
    return reinterpret_cast<const char*>(*ResultOrErr);
}

llvm::Expected<int32_t> Emitter::jitExecuteChar(const Plan &P) {
    auto ResultOrErr = jitExecuteRaw(P, llvm::Type::getInt32Ty(*Context));
    if (!ResultOrErr)
        return ResultOrErr.takeError();
    return static_cast<int32_t>(*ResultOrErr);
}

llvm::Error Emitter::jitExecuteVoid(const Plan &P, llvm::StringRef MangledFunctionName) {

    // Store current plan for type lookups
    CurrentPlan = &P;

    // Create fresh module for JIT
    Module = std::make_unique<llvm::Module>("jit_module", *Context);
    Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    // Clear caches
    TypeCache.clear();
    StructCache.clear();
    FunctionCache.clear();

    // Reset runtime function pointers (they belonged to the old module)
    AlignedAlloc = nullptr;
    Free = nullptr;
    ExitFunc = nullptr;
    PageAllocatePage = nullptr;
    PageAllocate = nullptr;
    PageDeallocateExtensions = nullptr;
    PageType = nullptr;
    BlockWatermarkType = nullptr;
    PageSaveWatermark = nullptr;
    PageRestoreWatermark = nullptr;

    // Check if Page is in the Plan
    bool PageInPlan = P.Structures.find("Page") != P.Structures.end();
    if (!PageInPlan) {
        initRBMM();
    }

    // Emit type declarations
    for (const auto &[name, Struct] : P.Structures) {
        emitStructType(Struct);
    }
    for (const auto &[name, Union] : P.Unions) {
        emitUnionType(Union);
    }

    // Emit global constants
    for (const auto &[name, Global] : P.Globals) {
        emitGlobal(Global);
    }

    // Emit function declarations
    for (const auto &[name, Func] : P.Functions) {
        emitFunctionDecl(Func);
    }
    // Also emit method, operator, initializer, and deinitializer declarations from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            emitFunctionDecl(Method);
        }
        for (const auto &Op : Struct.Operators) {
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
        for (const auto &Init : Struct.Initializers) {
            emitInitializerDecl(Struct, Init);
        }
        if (Struct.Deinitializer) {
            emitDeInitializerDecl(Struct, *Struct.Deinitializer);
        }
    }
    // Also emit method and operator declarations from unions (e.g., Option.unwrap)
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            emitFunctionDecl(Method);
        }
        for (const auto &Op : Union.Operators) {
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
    }


    // Set up RBMM function pointers if Page is in the Plan
    if (PageInPlan) {
        declareRuntimeFunctions();
        PageAllocatePage = FunctionCache["_ZN4Page13allocate_pageEv"];
        PageAllocate = FunctionCache["_ZN4Page8allocateEmm"];  // size_t = unsigned long (m)
        PageDeallocateExtensions = FunctionCache["_ZN4Page21deallocate_extensionsEv"];
        if (auto It = StructCache.find("_Z4Page"); It != StructCache.end()) {
            PageType = It->second;
        }
    }

    // Emit function bodies
    for (const auto &[name, Func] : P.Functions) {
        if (auto *LLVMFunc = FunctionCache[Func.MangledName]) {
            if (auto Err = emitFunctionBody(Func, LLVMFunc))
                return Err;
        }
    }
    // Also emit method, operator, initializer, and deinitializer bodies from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return Err;
            }
        }
        for (const auto &Op : Struct.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return Err;
            }
        }
        for (const auto &Init : Struct.Initializers) {
            if (auto *LLVMFunc = FunctionCache[Init.MangledName]) {
                if (auto Err = emitInitializerBody(Struct, Init, LLVMFunc))
                    return Err;
            }
        }
        if (Struct.Deinitializer) {
            if (auto *LLVMFunc = FunctionCache[Struct.Deinitializer->MangledName]) {
                if (auto Err = emitDeInitializerBody(Struct, *Struct.Deinitializer, LLVMFunc))
                    return Err;
            }
        }
    }
    // Also emit method and operator bodies from unions (e.g., Option.unwrap)
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return Err;
            }
        }
        for (const auto &Op : Union.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return Err;
            }
        }
    }

    // Find the target function
    llvm::Function *TargetFunc = FunctionCache[MangledFunctionName.str()];
    if (!TargetFunc) {
        return llvm::make_error<llvm::StringError>(
            "Function not found: " + MangledFunctionName.str(),
            llvm::inconvertibleErrorCode()
        );
    }

    // Create JIT wrapper that calls the target function
    auto *WrapperTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*Context), {}, false);
    auto *Wrapper = llvm::Function::Create(
        WrapperTy,
        llvm::GlobalValue::ExternalLinkage,
        "__scaly_jit_main",
        *Module
    );

    auto *Entry = llvm::BasicBlock::Create(*Context, "entry", Wrapper);
    Builder->SetInsertPoint(Entry);

    // Call the target function with no arguments
    Builder->CreateCall(TargetFunc);
    Builder->CreateRetVoid();

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

    // Add process symbol generator for external functions
    auto &MainJD = JIT->getMainJITDylib();
    auto ProcessSymbolsGenerator = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
        JIT->getDataLayout().getGlobalPrefix());
    if (!ProcessSymbolsGenerator) {
        return ProcessSymbolsGenerator.takeError();
    }
    MainJD.addGenerator(std::move(*ProcessSymbolsGenerator));

    // Add module to JIT
    auto TSM = llvm::orc::ThreadSafeModule(std::move(Module), std::move(Context));
    if (auto Err = JIT->addIRModule(std::move(TSM)))
        return Err;

    // Look up and execute the wrapper function
    auto SymOrErr = JIT->lookup("__scaly_jit_main");
    if (!SymOrErr)
        return SymOrErr.takeError();

    // Execute the wrapper (which calls the target function)
    auto *MainFn = SymOrErr->toPtr<void(*)()>();
    MainFn();

    // Recreate context for future use
    Context = std::make_unique<llvm::LLVMContext>();
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
    initIntrinsicTypes();

    return llvm::Error::success();
}

llvm::Expected<int64_t> Emitter::jitExecuteIntFunction(const Plan &P, llvm::StringRef MangledFunctionName) {

    // Store current plan for type lookups
    CurrentPlan = &P;

    // Create fresh module for JIT
    Module = std::make_unique<llvm::Module>("jit_module", *Context);
    Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    // Clear caches
    TypeCache.clear();
    StructCache.clear();
    FunctionCache.clear();

    // Reset runtime function pointers (they belonged to the old module)
    AlignedAlloc = nullptr;
    Free = nullptr;
    ExitFunc = nullptr;
    PageAllocatePage = nullptr;
    PageAllocate = nullptr;
    PageDeallocateExtensions = nullptr;
    PageType = nullptr;
    BlockWatermarkType = nullptr;
    PageSaveWatermark = nullptr;
    PageRestoreWatermark = nullptr;

    // Check if Page is in the Plan
    bool PageInPlan = P.Structures.find("Page") != P.Structures.end();
    if (!PageInPlan) {
        initRBMM();
    }

    // Emit type declarations
    llvm::errs().flush();
    for (const auto &[name, Struct] : P.Structures) {
        emitStructType(Struct);
    }
    llvm::errs().flush();
    for (const auto &[name, Union] : P.Unions) {
        emitUnionType(Union);
    }

    // Emit global constants
    llvm::errs().flush();
    for (const auto &[name, Global] : P.Globals) {
        emitGlobal(Global);
    }

    llvm::errs().flush();
    // Emit function declarations with debug tracking
    size_t declCount = 0, skipCount = 0;
    for (const auto &[name, Func] : P.Functions) {
        size_t before = FunctionCache.size();
        emitFunctionDecl(Func);
        if (FunctionCache.size() > before) declCount++;
        else skipCount++;
    }

    // Also emit method, operator, initializer, and deinitializer declarations from structures
    declCount = 0; skipCount = 0;
    size_t initDeclCount = 0;
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            size_t before = FunctionCache.size();
            emitFunctionDecl(Method);
            if (FunctionCache.size() > before) declCount++;
            else skipCount++;
        }
        for (const auto &Op : Struct.Operators) {
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
        for (const auto &Init : Struct.Initializers) {
            emitInitializerDecl(Struct, Init);
            initDeclCount++;
        }
        if (Struct.Deinitializer) {
            emitDeInitializerDecl(Struct, *Struct.Deinitializer);
        }
    }

    // Also emit method and operator declarations from unions (e.g., Option.unwrap)
    declCount = 0; skipCount = 0;
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            size_t before = FunctionCache.size();
            emitFunctionDecl(Method);
            if (FunctionCache.size() > before) declCount++;
            else skipCount++;
        }
        for (const auto &Op : Union.Operators) {
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
    }

    // Set up RBMM function pointers if Page is in the Plan
    if (PageInPlan) {
        declareRuntimeFunctions();
        PageAllocatePage = FunctionCache["_ZN4Page13allocate_pageEv"];
        PageAllocate = FunctionCache["_ZN4Page8allocateEmm"];  // size_t = unsigned long (m)
        PageDeallocateExtensions = FunctionCache["_ZN4Page21deallocate_extensionsEv"];
        if (auto It = StructCache.find("_Z4Page"); It != StructCache.end()) {
            PageType = It->second;
        }
    }

    llvm::errs().flush();
    // Emit function bodies - track how many are emitted vs skipped
    size_t bodyEmitCount = 0, bodySkipExtern = 0, bodySkipIntrinsic = 0, bodySkipAlreadyDone = 0, bodyNotInCache = 0;
    size_t implAction = 0, implExtern = 0, implIntrinsic = 0, implInstruction = 0;
    for (const auto &[name, Func] : P.Functions) {
        // Count impl types
        if (std::holds_alternative<PlannedAction>(Func.Impl)) implAction++;
        else if (std::holds_alternative<PlannedExternImpl>(Func.Impl)) implExtern++;
        else if (std::holds_alternative<PlannedIntrinsicImpl>(Func.Impl)) implIntrinsic++;
        else if (std::holds_alternative<PlannedInstructionImpl>(Func.Impl)) implInstruction++;

        if (auto *LLVMFunc = FunctionCache[Func.MangledName]) {
            bool isExtern = std::holds_alternative<PlannedExternImpl>(Func.Impl);
            bool isIntrinsic = std::holds_alternative<PlannedIntrinsicImpl>(Func.Impl);
            bool alreadyDone = !LLVMFunc->isDeclaration();
            // Debug Option::unwrap skip reasons
            if (Func.MangledName.find("OptionI") != std::string::npos &&
                Func.MangledName.find("unwrap") != std::string::npos) {
            }
            if (isExtern) {
                bodySkipExtern++;
                // Print first few extern names for debugging
                if (bodySkipExtern <= 5) {
                }
            }
            else if (isIntrinsic) bodySkipIntrinsic++;
            else if (alreadyDone) bodySkipAlreadyDone++;
            else {
                bodyEmitCount++;
                // Print the test functions specifically
                if (Func.MangledName.find("4test") != std::string::npos) {
                }
                // Print Option unwrap functions
                if (Func.MangledName.find("OptionI") != std::string::npos &&
                    Func.MangledName.find("unwrap") != std::string::npos) {
                }
            }
            if (auto Err = emitFunctionBody(Func, LLVMFunc))
                return Err;
        } else {
            bodyNotInCache++;
        }
    }

    // Also emit method, operator, initializer, and deinitializer bodies from structures
    size_t initBodyEmit = 0, initBodySkip = 0, initNotInCache = 0;
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return Err;
            }
        }
        for (const auto &Op : Struct.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return Err;
            }
        }
        for (const auto &Init : Struct.Initializers) {
            if (auto *LLVMFunc = FunctionCache[Init.MangledName]) {
                bool isExtern = std::holds_alternative<PlannedExternImpl>(Init.Impl);
                bool isIntrinsic = std::holds_alternative<PlannedIntrinsicImpl>(Init.Impl);
                bool alreadyDone = !LLVMFunc->isDeclaration();
                if (isExtern || isIntrinsic || alreadyDone) {
                    initBodySkip++;
                } else {
                    initBodyEmit++;
                }
                if (auto Err = emitInitializerBody(Struct, Init, LLVMFunc))
                    return Err;
            } else {
                initNotInCache++;
            }
        }
        if (Struct.Deinitializer) {
            if (auto *LLVMFunc = FunctionCache[Struct.Deinitializer->MangledName]) {
                if (auto Err = emitDeInitializerBody(Struct, *Struct.Deinitializer, LLVMFunc))
                    return Err;
            }
        }
    }

    // Also emit method and operator bodies from unions (e.g., Option.unwrap)
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc))
                    return Err;
            }
        }
        for (const auto &Op : Union.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc))
                    return Err;
            }
        }
    }

    // Verify that all non-extern functions have bodies
    size_t stillDeclarations = 0;
    for (const auto &[name, Func] : FunctionCache) {
        if (Func->isDeclaration()) {
            stillDeclarations++;
            if (stillDeclarations <= 10) {
            }
        }
    }

    // Check for functions in Module that aren't in FunctionCache (added during emission)
    size_t extraFuncs = 0, extraDecls = 0;
    for (auto &F : *Module) {
        if (FunctionCache.find(F.getName().str()) == FunctionCache.end()) {
            extraFuncs++;
            if (F.isDeclaration()) extraDecls++;
            if (extraFuncs <= 10) {
            }
        }
    }

    // Check for specific failing symbols (without Darwin prefix)
    const char* checkSymbols[] = {
        "_ZN5ArrayIcEC1Em",           // Array<char>::Array(size_t)
        "_ZN13EmitterConfigC1Ev",     // EmitterConfig::EmitterConfig()
        "_ZN6VectorI11LoopContextEC1Ev",  // Vector<LoopContext>::Vector()
        "_ZN7hashing9get_primeEm",    // hashing::get_prime(size_t)
    };
    for (const char* sym : checkSymbols) {
        auto it = FunctionCache.find(sym);
    }

    // Check for declarations that should have bodies (not extern/libc)
    size_t internalDecls = 0;
    for (auto &F : *Module) {
        if (F.isDeclaration() && !F.isIntrinsic()) {
            llvm::StringRef Name = F.getName();
            // Skip known extern symbols (libc, LLVM C API)
            if (!Name.starts_with("llvm.") && !Name.starts_with("LLVM") &&
                !Name.starts_with("__") &&
                Name != "memcpy" && Name != "memset" && Name != "memmove" &&
                Name != "malloc" && Name != "free" && Name != "realloc" &&
                Name != "strlen" && Name != "strcmp" && Name != "strncmp" &&
                Name != "fopen" && Name != "fclose" && Name != "fread" && Name != "fwrite" &&
                Name != "fseek" && Name != "ftell" && Name != "printf" && Name != "fprintf" &&
                Name != "exit" && Name != "abort" && Name != "sqrt" && Name != "pow") {
                internalDecls++;
                if (internalDecls <= 10) {
                }
            }
        }
    }

    // Check if the missing Option::unwrap functions are in the plan
    for (const auto &[name, Func] : P.Functions) {
        if (Func.MangledName.find("OptionI") != std::string::npos &&
            Func.MangledName.find("unwrap") != std::string::npos) {
            auto it = FunctionCache.find(Func.MangledName);
        }
    }
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            if (Method.MangledName.find("OptionI") != std::string::npos &&
                Method.MangledName.find("unwrap") != std::string::npos) {
                auto it = FunctionCache.find(Method.MangledName);
            }
        }
    }

    // Debug: count declared methods
    size_t totalMethods = 0, totalOps = 0, totalInits = 0, totalDeinits = 0;
    size_t totalUnionMethods = 0, totalUnionOps = 0;
    for (const auto &[name, Struct] : P.Structures) {
        totalMethods += Struct.Methods.size();
        totalOps += Struct.Operators.size();
        totalInits += Struct.Initializers.size();
        if (Struct.Deinitializer) totalDeinits++;
    }
    for (const auto &[name, Union] : P.Unions) {
        totalUnionMethods += Union.Methods.size();
        totalUnionOps += Union.Operators.size();
    }

    // Find the target function
    llvm::Function *TargetFunc = FunctionCache[MangledFunctionName.str()];
    if (!TargetFunc) {
        return llvm::make_error<llvm::StringError>(
            "Function not found: " + MangledFunctionName.str(),
            llvm::inconvertibleErrorCode()
        );
    }

    // Create JIT wrapper that calls the target function and returns the int result
    auto *WrapperTy = llvm::FunctionType::get(llvm::Type::getInt64Ty(*Context), {}, false);
    auto *Wrapper = llvm::Function::Create(
        WrapperTy,
        llvm::GlobalValue::ExternalLinkage,
        "__scaly_jit_main",
        *Module
    );

    auto *Entry = llvm::BasicBlock::Create(*Context, "entry", Wrapper);
    Builder->SetInsertPoint(Entry);

    // Call the target function
    auto *CallResult = Builder->CreateCall(TargetFunc);

    // The function might return i32 or i64, ensure we return i64
    llvm::Value *ReturnValue = CallResult;
    if (CallResult->getType()->isIntegerTy(32)) {
        ReturnValue = Builder->CreateSExt(CallResult, llvm::Type::getInt64Ty(*Context));
    }
    Builder->CreateRet(ReturnValue);

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

    // Add process symbol generator for external functions
    auto &MainJD = JIT->getMainJITDylib();
    auto ProcessSymbolsGenerator = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
        JIT->getDataLayout().getGlobalPrefix());
    if (!ProcessSymbolsGenerator) {
        return ProcessSymbolsGenerator.takeError();
    }
    MainJD.addGenerator(std::move(*ProcessSymbolsGenerator));

    // Add module to JIT
    llvm::errs().flush();
    auto TSM = llvm::orc::ThreadSafeModule(std::move(Module), std::move(Context));
    if (auto Err = JIT->addIRModule(std::move(TSM)))
        return Err;

    // Look up and execute the wrapper function
    llvm::errs().flush();
    auto SymOrErr = JIT->lookup("__scaly_jit_main");
    if (!SymOrErr)
        return SymOrErr.takeError();

    // Execute the wrapper (which calls the target function) and get the result
    llvm::errs().flush();
    auto *MainFn = SymOrErr->toPtr<int64_t(*)()>();
    int64_t Result = MainFn();
    llvm::errs().flush();

    // Recreate context for future use
    Context = std::make_unique<llvm::LLVMContext>();
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);

    // Re-initialize intrinsic types for the new context
    // (IntrinsicTypes contained pointers to types from the old context)
    initIntrinsicTypes();

    return Result;
}

void Emitter::dumpIR(const Plan &P) {
    // Store current plan for type lookups
    CurrentPlan = &P;

    // Create fresh module for IR dump
    Module = std::make_unique<llvm::Module>("ir_dump_module", *Context);
    Module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    // Clear caches
    TypeCache.clear();
    StructCache.clear();
    FunctionCache.clear();

    // Reset runtime function pointers
    AlignedAlloc = nullptr;
    Free = nullptr;
    ExitFunc = nullptr;
    PageAllocatePage = nullptr;
    PageAllocate = nullptr;
    PageDeallocateExtensions = nullptr;
    PageType = nullptr;
    BlockWatermarkType = nullptr;
    PageSaveWatermark = nullptr;
    PageRestoreWatermark = nullptr;

    // Check if Page is in the Plan
    bool PageInPlan = P.Structures.find("Page") != P.Structures.end();
    if (!PageInPlan) {
        initRBMM();
    }

    // Emit type declarations
    for (const auto &[name, Struct] : P.Structures) {
        emitStructType(Struct);
    }
    for (const auto &[name, Union] : P.Unions) {
        emitUnionType(Union);
    }

    // Emit global constants
    for (const auto &[name, Global] : P.Globals) {
        emitGlobal(Global);
    }

    // Emit function declarations
    for (const auto &[name, Func] : P.Functions) {
        emitFunctionDecl(Func);
    }
    // Also emit method, operator, initializer, and deinitializer declarations from structures
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            emitFunctionDecl(Method);
        }
        for (const auto &Op : Struct.Operators) {
            PlannedFunction OpFunc;
            OpFunc.Loc = Op.Loc;
            OpFunc.Private = Op.Private;
            OpFunc.Pure = true;
            OpFunc.Name = Op.Name;
            OpFunc.MangledName = Op.MangledName;
            OpFunc.Input = Op.Input;
            OpFunc.Returns = Op.Returns;
            OpFunc.Throws = Op.Throws;
            OpFunc.Impl = Op.Impl;
            OpFunc.Origin = Op.Origin;
            OpFunc.Scheme = Op.Scheme;
            emitFunctionDecl(OpFunc);
        }
        for (const auto &Init : Struct.Initializers) {
            emitInitializerDecl(Struct, Init);
        }
        if (Struct.Deinitializer) {
            emitDeInitializerDecl(Struct, *Struct.Deinitializer);
        }
    }

    // Set up RBMM function pointers if Page is in the Plan
    if (PageInPlan) {
        declareRuntimeFunctions();
        PageAllocatePage = FunctionCache["_ZN4Page13allocate_pageEv"];
        PageAllocate = FunctionCache["_ZN4Page8allocateEmm"];
        PageDeallocateExtensions = FunctionCache["_ZN4Page21deallocate_extensionsEv"];
        if (auto It = StructCache.find("_Z4Page"); It != StructCache.end()) {
            PageType = It->second;
        }
    }

    // Emit function bodies
    for (const auto &[name, Func] : P.Functions) {
        if (auto *LLVMFunc = FunctionCache[Func.MangledName]) {
            if (auto Err = emitFunctionBody(Func, LLVMFunc)) {
                llvm::consumeError(std::move(Err));
            }
        }
    }
    for (const auto &[name, Struct] : P.Structures) {
        for (const auto &Method : Struct.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc)) {
                    llvm::consumeError(std::move(Err));
                }
            }
        }
        for (const auto &Op : Struct.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc)) {
                    llvm::consumeError(std::move(Err));
                }
            }
        }
        for (const auto &Init : Struct.Initializers) {
            if (auto *LLVMFunc = FunctionCache[Init.MangledName]) {
                if (auto Err = emitInitializerBody(Struct, Init, LLVMFunc)) {
                    llvm::consumeError(std::move(Err));
                }
            }
        }
        if (Struct.Deinitializer) {
            auto &DeInit = *Struct.Deinitializer;
            if (auto *LLVMFunc = FunctionCache[DeInit.MangledName]) {
                if (auto Err = emitDeInitializerBody(Struct, DeInit, LLVMFunc)) {
                    llvm::consumeError(std::move(Err));
                }
            }
        }
    }
    // Also emit method and operator bodies from unions (e.g., Option.unwrap)
    for (const auto &[name, Union] : P.Unions) {
        for (const auto &Method : Union.Methods) {
            if (auto *LLVMFunc = FunctionCache[Method.MangledName]) {
                if (auto Err = emitFunctionBody(Method, LLVMFunc)) {
                    llvm::consumeError(std::move(Err));
                }
            }
        }
        for (const auto &Op : Union.Operators) {
            if (auto *LLVMFunc = FunctionCache[Op.MangledName]) {
                PlannedFunction OpFunc;
                OpFunc.Loc = Op.Loc;
                OpFunc.Private = Op.Private;
                OpFunc.Pure = true;
                OpFunc.Name = Op.Name;
                OpFunc.MangledName = Op.MangledName;
                OpFunc.Input = Op.Input;
                OpFunc.Returns = Op.Returns;
                OpFunc.Throws = Op.Throws;
                OpFunc.Impl = Op.Impl;
                OpFunc.Origin = Op.Origin;
                OpFunc.Scheme = Op.Scheme;
                if (auto Err = emitFunctionBody(OpFunc, LLVMFunc)) {
                    llvm::consumeError(std::move(Err));
                }
            }
        }
    }

    // Print the module IR
    Module->print(llvm::errs(), nullptr);
}

} // namespace scaly
