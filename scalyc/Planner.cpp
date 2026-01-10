// Planner.cpp - Generic resolution, type inference, and name mangling
// Transforms Model (with generics) → Plan (concrete types, mangled names)

#include "Planner.h"
#include "Parser.h"
#include "Modeler.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include <algorithm>
#include <set>
#include <sstream>

namespace scaly {

// ============================================================================
// Helper: Check if lifetime indicates page allocation
// ============================================================================

static bool isOnPageLifetime(const Lifetime &Life) {
    // Unspecified = stack allocation, everything else = page allocation
    return !std::holds_alternative<UnspecifiedLifetime>(Life);
}

// ============================================================================
// Helper: Decode UTF-8 string to single Unicode code point
// Returns the code point, or -1 if invalid (empty, multiple code points, or malformed)
// ============================================================================

static int32_t decodeUtf8ToCodePoint(const std::string &Str) {
    if (Str.empty()) return -1;

    const unsigned char *s = reinterpret_cast<const unsigned char*>(Str.data());
    size_t len = Str.size();
    int32_t codePoint;
    size_t charLen;

    // Decode first UTF-8 character
    if ((s[0] & 0x80) == 0) {
        // ASCII: 0xxxxxxx
        codePoint = s[0];
        charLen = 1;
    } else if ((s[0] & 0xE0) == 0xC0 && len >= 2) {
        // 2-byte: 110xxxxx 10xxxxxx
        if ((s[1] & 0xC0) != 0x80) return -1;
        codePoint = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        if (codePoint < 0x80) return -1;  // Overlong encoding
        charLen = 2;
    } else if ((s[0] & 0xF0) == 0xE0 && len >= 3) {
        // 3-byte: 1110xxxx 10xxxxxx 10xxxxxx
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return -1;
        codePoint = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        if (codePoint < 0x800) return -1;  // Overlong encoding
        charLen = 3;
    } else if ((s[0] & 0xF8) == 0xF0 && len >= 4) {
        // 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return -1;
        codePoint = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
                    ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        if (codePoint < 0x10000 || codePoint > 0x10FFFF) return -1;  // Invalid range
        charLen = 4;
    } else {
        return -1;  // Invalid UTF-8 start byte
    }

    // Ensure exactly one character (no trailing bytes)
    if (charLen != len) return -1;

    return codePoint;
}

// ============================================================================
// Constructor
// ============================================================================

Planner::Planner(llvm::StringRef FileName) : File(FileName.str()) {}

void Planner::addSiblingProgram(std::shared_ptr<Program> Prog) {
    SiblingPrograms.push_back(Prog);
}

void Planner::addPackageSearchPath(llvm::StringRef Path) {
    PackageSearchPaths.push_back(Path.str());
}

const Module* Planner::loadPackageOnDemand(llvm::StringRef PackageName) {
    // Check if already loaded
    auto It = LoadedPackages.find(PackageName.str());
    if (It != LoadedPackages.end()) {
        return It->second;
    }

    // Build search paths list
    std::vector<std::string> SearchPaths;

    // Current working directory's packages/
    SearchPaths.push_back("packages");

    // Source file directory's packages/
    llvm::SmallString<256> LocalPath(File);
    llvm::sys::path::remove_filename(LocalPath);
    llvm::SmallString<256> SourcePackages(LocalPath);
    llvm::sys::path::append(SourcePackages, "packages");
    SearchPaths.push_back(SourcePackages.str().str());

    // Walk up directory tree from source file looking for packages/
    llvm::SmallString<256> WalkUp(LocalPath);
    for (int i = 0; i < 10 && !WalkUp.empty(); ++i) {
        llvm::sys::path::remove_filename(WalkUp);
        if (WalkUp.empty()) break;
        llvm::SmallString<256> ParentPackages(WalkUp);
        llvm::sys::path::append(ParentPackages, "packages");
        if (llvm::sys::fs::is_directory(ParentPackages)) {
            SearchPaths.push_back(ParentPackages.str().str());
            break;
        }
    }

    // User-provided -I paths
    for (const auto &P : PackageSearchPaths) {
        SearchPaths.push_back(P);
    }

    // Try each search path with default version 0.1.0
    // (In a full implementation, we'd parse version from Use statements)
    std::string VersionStr = "0.1.0";

    for (const auto &Base : SearchPaths) {
        llvm::SmallString<256> PkgPath(Base);
        llvm::sys::path::append(PkgPath, PackageName, VersionStr,
                                 PackageName.str() + ".scaly");

        if (llvm::sys::fs::exists(PkgPath)) {
            // Read and parse the package file
            auto BufOrErr = llvm::MemoryBuffer::getFile(PkgPath);
            if (!BufOrErr) {
                continue;  // Try next path
            }

            Parser PkgParser((*BufOrErr)->getBuffer());
            auto ParseResult = PkgParser.parseProgram();
            if (!ParseResult) {
                llvm::consumeError(ParseResult.takeError());
                continue;  // Try next path
            }

            // Use Modeler to build the program model
            Modeler PkgModeler(PkgPath.str());
            for (const auto &P : PackageSearchPaths) {
                PkgModeler.addPackageSearchPath(P);
            }
            auto ModelResult = PkgModeler.buildProgram(*ParseResult);
            if (!ModelResult) {
                llvm::consumeError(ModelResult.takeError());
                continue;  // Try next path
            }

            // Store the Program and register the package
            auto Prog = std::make_unique<Program>(std::move(*ModelResult));
            OnDemandPackages.push_back(std::move(Prog));

            // Get the module pointer from the stored Program (after the move)
            const Module* RootMod = &OnDemandPackages.back()->MainModule;
            LoadedPackages[PackageName.str()] = RootMod;

            return RootMod;
        }
    }

    return nullptr;  // Package not found
}

const Module* Planner::loadIntraPackageModule(llvm::StringRef BasePath, llvm::StringRef ModuleName) {
    // Build the module file path: BasePath/ModuleName.scaly
    llvm::SmallString<256> ModulePath(BasePath);
    llvm::sys::path::append(ModulePath, ModuleName.str() + ".scaly");

    // Check if the file exists
    if (!llvm::sys::fs::exists(ModulePath)) {
        return nullptr;
    }

    // Read and parse the module file
    auto BufOrErr = llvm::MemoryBuffer::getFile(ModulePath);
    if (!BufOrErr) {
        return nullptr;
    }

    Parser ModParser((*BufOrErr)->getBuffer());
    auto ParseResult = ModParser.parseProgram();
    if (!ParseResult) {
        llvm::consumeError(ParseResult.takeError());
        return nullptr;
    }

    // Use Modeler to build the program model
    Modeler ModModeler(ModulePath.str());
    for (const auto &P : PackageSearchPaths) {
        ModModeler.addPackageSearchPath(P);
    }
    auto ModelResult = ModModeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        llvm::consumeError(ModelResult.takeError());
        return nullptr;
    }

    // Store the Program and return the module
    auto Prog = std::make_unique<Program>(std::move(*ModelResult));
    OnDemandPackages.push_back(std::move(Prog));

    // Get the module pointer from the stored Program
    const Module* LoadedMod = &OnDemandPackages.back()->MainModule;
    return LoadedMod;
}

// Register built-in runtime functions (aligned_alloc, free, exit) for RBMM support
void Planner::registerRuntimeFunctions() {
    if (RuntimeFunctionsRegistered) return;
    RuntimeFunctionsRegistered = true;

    // Create synthetic Function objects for C runtime functions
    // These are owned by the planner via BuiltinFunctions vector

    // aligned_alloc(alignment: size_t, size: size_t) returns pointer[void] extern
    auto AlignedAlloc = std::make_unique<Function>();
    AlignedAlloc->Name = "aligned_alloc";
    AlignedAlloc->Impl = ExternImpl{Span{0, 0}};
    {
        Item Param1;
        Param1.Name = std::make_unique<std::string>("alignment");
        Type T1; T1.Name = {"size_t"}; T1.Loc = Span{0, 0};
        Param1.ItemType = std::make_unique<Type>(std::move(T1));
        AlignedAlloc->Input.push_back(std::move(Param1));

        Item Param2;
        Param2.Name = std::make_unique<std::string>("size");
        Type T2; T2.Name = {"size_t"}; T2.Loc = Span{0, 0};
        Param2.ItemType = std::make_unique<Type>(std::move(T2));
        AlignedAlloc->Input.push_back(std::move(Param2));

        Type RetType;
        RetType.Name = {"pointer"};
        RetType.Loc = Span{0, 0};
        Type VoidType; VoidType.Name = {"void"}; VoidType.Loc = Span{0, 0};
        RetType.Generics = std::make_unique<std::vector<Type>>();
        RetType.Generics->push_back(std::move(VoidType));
        AlignedAlloc->Returns = std::make_unique<Type>(std::move(RetType));
    }
    Functions["aligned_alloc"].push_back(AlignedAlloc.get());
    BuiltinFunctions.push_back(std::move(AlignedAlloc));

    // free(ptr: pointer[void]) extern
    auto Free = std::make_unique<Function>();
    Free->Name = "free";
    Free->Impl = ExternImpl{Span{0, 0}};
    {
        Item Param;
        Param.Name = std::make_unique<std::string>("ptr");
        Type PtrType;
        PtrType.Name = {"pointer"};
        PtrType.Loc = Span{0, 0};
        Type VoidType; VoidType.Name = {"void"}; VoidType.Loc = Span{0, 0};
        PtrType.Generics = std::make_unique<std::vector<Type>>();
        PtrType.Generics->push_back(std::move(VoidType));
        Param.ItemType = std::make_unique<Type>(std::move(PtrType));
        Free->Input.push_back(std::move(Param));
    }
    Functions["free"].push_back(Free.get());
    BuiltinFunctions.push_back(std::move(Free));

    // exit(code: int) extern
    auto Exit = std::make_unique<Function>();
    Exit->Name = "exit";
    Exit->Impl = ExternImpl{Span{0, 0}};
    {
        Item Param;
        Param.Name = std::make_unique<std::string>("code");
        Type IntType; IntType.Name = {"int"}; IntType.Loc = Span{0, 0};
        Param.ItemType = std::make_unique<Type>(std::move(IntType));
        Exit->Input.push_back(std::move(Param));
    }
    Functions["exit"].push_back(Exit.get());
    BuiltinFunctions.push_back(std::move(Exit));
}

// ============================================================================
// Type Inference Helpers
// ============================================================================

PlannedType Planner::freshTypeVar(const std::string &DebugName) {
    if (!CurrentPlan) {
        // Fallback if no plan context - shouldn't happen in normal use
        PlannedType Result;
        Result.Name = "?";
        return Result;
    }

    TypeVariable Var = CurrentPlan->freshTypeVar(DebugName);
    PlannedType Result;
    Result.Variable = std::make_shared<TypeVariable>(Var);
    Result.Name = DebugName.empty() ? ("?" + std::to_string(Var.Id)) : DebugName;
    return Result;
}

void Planner::addConstraint(const PlannedType &Left, const PlannedType &Right, Span Loc) {
    if (!CurrentPlan) {
        return;
    }

    EqualityConstraint Eq;
    Eq.Left = Left;
    Eq.Right = Right;
    Eq.Loc = Loc;
    Eq.File = File;  // Capture current file for error messages
    CurrentPlan->addConstraint(Eq);
}

llvm::Expected<Substitution> Planner::solveAndApply() {
    if (!CurrentPlan || CurrentPlan->Constraints.empty()) {
        return Substitution{};
    }

    auto Solution = solveConstraints(CurrentPlan->Constraints, File);
    if (!Solution) {
        return Solution.takeError();
    }

    CurrentPlan->Solution = *Solution;
    CurrentPlan->Constraints.clear();  // Constraints are consumed

    return *Solution;
}

PlannedType Planner::applyCurrentSubstitution(const PlannedType &Type) {
    if (!CurrentPlan || CurrentPlan->Solution.empty()) {
        return Type;
    }
    return applySubstitution(Type, CurrentPlan->Solution);
}

// ============================================================================
// Helper: Clone Model (deep copy, since Model contains unique_ptr)
// ============================================================================

// Forward declarations for cloning
static Tuple cloneTuple(const Tuple &T);
static Matrix cloneMatrix(const Matrix &M);
static Component cloneComponent(const Component &C);
static std::vector<Operand> cloneOperands(const std::vector<Operand> &Ops);
static Model cloneModel(const Model &M);
static Attribute cloneAttribute(const Attribute &A);

static Attribute cloneAttribute(const Attribute &A) {
    Attribute Result;
    Result.Loc = A.Loc;
    Result.Name = A.Name;
    Result.Value = cloneModel(A.Value);
    return Result;
}

static Component cloneComponent(const Component &C) {
    Component Result;
    Result.Loc = C.Loc;
    if (C.Name) {
        Result.Name = std::make_unique<std::string>(*C.Name);
    }
    Result.Value = cloneOperands(C.Value);
    for (const auto &Attr : C.Attributes) {
        Result.Attributes.push_back(cloneAttribute(Attr));
    }
    return Result;
}

static Tuple cloneTuple(const Tuple &T) {
    Tuple Result;
    Result.Loc = T.Loc;
    for (const auto &Comp : T.Components) {
        Result.Components.push_back(cloneComponent(Comp));
    }
    return Result;
}

static Matrix cloneMatrix(const Matrix &M) {
    Matrix Result;
    Result.Loc = M.Loc;
    for (const auto &Row : M.Operations) {
        Result.Operations.push_back(cloneOperands(Row));
    }
    return Result;
}

static Operand cloneOperand(const Operand &Op);
static Expression cloneExpression(const Expression &E);

static Type cloneType(const Type &T);

static Type cloneType(const Type &T) {
    Type Result;
    Result.Loc = T.Loc;
    Result.Name = T.Name;
    Result.Life = T.Life;
    if (T.Generics) {
        Result.Generics = std::make_unique<std::vector<Type>>();
        for (const auto &G : *T.Generics) {
            Result.Generics->push_back(cloneType(G));
        }
    }
    return Result;
}

static Expression cloneExpression(const Expression &E) {
    return std::visit([](const auto &V) -> Expression {
        using T = std::decay_t<decltype(V)>;
        if constexpr (std::is_same_v<T, Constant>) {
            return V;  // Constant is copyable
        } else if constexpr (std::is_same_v<T, Type>) {
            return cloneType(V);
        } else if constexpr (std::is_same_v<T, Tuple>) {
            return cloneTuple(V);
        } else if constexpr (std::is_same_v<T, Matrix>) {
            return cloneMatrix(V);
        } else {
            // Block and control flow expressions - return default for now
            // These are complex and rarely used in attribute values
            return Constant{BooleanConstant{{0, 0}, false}};
        }
    }, E);
}

static Operand cloneOperand(const Operand &Op) {
    Operand Result;
    Result.Loc = Op.Loc;
    Result.Expr = cloneExpression(Op.Expr);
    if (Op.MemberAccess) {
        Result.MemberAccess = std::make_shared<std::vector<Type>>(*Op.MemberAccess);
    }
    return Result;
}

static std::vector<Operand> cloneOperands(const std::vector<Operand> &Ops) {
    std::vector<Operand> Result;
    Result.reserve(Ops.size());
    for (const auto &Op : Ops) {
        Result.push_back(cloneOperand(Op));
    }
    return Result;
}

static Model cloneModel(const Model &M) {
    return std::visit([](const auto &V) -> Model {
        using T = std::decay_t<decltype(V)>;
        if constexpr (std::is_same_v<T, Constant>) {
            return V;  // Constant is copyable
        } else if constexpr (std::is_same_v<T, ModelVariable>) {
            return V;  // ModelVariable is copyable
        } else if constexpr (std::is_same_v<T, std::shared_ptr<Tuple>>) {
            if (V) {
                return std::make_shared<Tuple>(cloneTuple(*V));
            }
            return std::shared_ptr<Tuple>(nullptr);
        } else if constexpr (std::is_same_v<T, std::shared_ptr<Matrix>>) {
            if (V) {
                return std::make_shared<Matrix>(cloneMatrix(*V));
            }
            return std::shared_ptr<Matrix>(nullptr);
        } else {
            return Model{};
        }
    }, M);
}

// ============================================================================
// Name Mangling (Itanium ABI inspired)
// ============================================================================

// Encode a name with length prefix: "foo" → "3foo"
std::string Planner::encodeName(llvm::StringRef Name) {
    return std::to_string(Name.size()) + Name.str();
}

// Encode a type for mangling
std::string Planner::encodeType(const PlannedType &Type) {
    // If MangledName is already set and encodes generics, use it
    // This handles cases where we have the mangled name but not the Generics vector
    if (!Type.MangledName.empty() && Type.Generics.empty()) {
        // MangledName starts with "_Z" (from mangleStructure/mangleType)
        // Strip the prefix and use the encoded name directly
        if (Type.MangledName.size() > 2 && Type.MangledName.substr(0, 2) == "_Z") {
            return Type.MangledName.substr(2);
        }
        // If it's just an encoded name without _Z prefix, use it directly
        if (!Type.MangledName.empty() && std::isdigit(Type.MangledName[0])) {
            return Type.MangledName;
        }
    }

    // Built-in types use single-letter codes (Itanium ABI)
    if (Type.Name == "void") return "v";
    if (Type.Name == "bool") return "b";
    if (Type.Name == "char") return "c";
    if (Type.Name == "int") return "i";
    if (Type.Name == "uint") return "j";
    if (Type.Name == "long") return "l";
    if (Type.Name == "ulong") return "m";
    if (Type.Name == "float") return "f";
    if (Type.Name == "double") return "d";
    if (Type.Name == "size" || Type.Name == "size_t") return "m";  // size_t → unsigned long

    // Pointer types
    if (Type.Name.size() > 1 && Type.Name.back() == '*') {
        PlannedType Inner = Type;
        Inner.Name = Type.Name.substr(0, Type.Name.size() - 1);
        return "P" + encodeType(Inner);
    }

    // Reference types
    if (Type.Name.size() > 1 && Type.Name.back() == '&') {
        PlannedType Inner = Type;
        Inner.Name = Type.Name.substr(0, Type.Name.size() - 1);
        return "R" + encodeType(Inner);
    }

    // Scaly pointer[T] types → P + encoded(T)
    if (Type.Name == "pointer" && Type.Generics.size() == 1) {
        return "P" + encodeType(Type.Generics[0]);
    }

    // Scaly ref[T] types → R + encoded(T) (non-owning reference)
    if (Type.Name == "ref" && Type.Generics.size() == 1) {
        return "R" + encodeType(Type.Generics[0]);
    }

    // Generic types: Vector[int] → 6VectorIiE (Itanium ABI: name + I + args + E)
    if (!Type.Generics.empty()) {
        // Extract base name if Name contains instantiation suffix (e.g., "Vector.char" -> "Vector")
        std::string BaseName = Type.Name;
        size_t DotPos = BaseName.find('.');
        if (DotPos != std::string::npos) {
            BaseName = BaseName.substr(0, DotPos);
        }
        std::string Result = encodeName(BaseName) + "I";
        for (const auto &Arg : Type.Generics) {
            Result += encodeType(Arg);
        }
        Result += "E";
        return Result;
    }

    // User-defined types: use length-prefixed name
    return encodeName(Type.Name);
}

// Mangle a type name
std::string Planner::mangleType(const PlannedType &Type) {
    return "_Z" + encodeType(Type);
}

// Mangle a structure/union name with generic args
std::string Planner::mangleStructure(llvm::StringRef Name,
                                      const std::vector<PlannedType> &GenericArgs) {
    if (GenericArgs.empty()) {
        return "_Z" + encodeName(Name);
    }

    // Itanium ABI: Name + I + args + E  (e.g., 6VectorIiE for Vector[int])
    std::string Result = "_Z" + encodeName(Name) + "I";
    for (const auto &Arg : GenericArgs) {
        Result += encodeType(Arg);
    }
    Result += "E";
    return Result;
}

// Mangle a function name
std::string Planner::mangleFunction(llvm::StringRef Name,
                                     const std::vector<PlannedItem> &Params,
                                     const PlannedType *Parent) {
    std::string Result = "_Z";

    // Nested name if inside a type
    if (Parent) {
        Result += "N";
        Result += encodeType(*Parent);
        Result += encodeName(Name);
        Result += "E";
    } else {
        Result += encodeName(Name);
    }

    // Parameter types
    // Skip 'this' parameter for methods - Itanium ABI treats it as implicit
    bool HasExplicitParams = false;
    for (const auto &Param : Params) {
        if (Param.Name && *Param.Name == "this") {
            continue;  // Skip 'this' in mangled name
        }
        if (Param.ItemType) {
            Result += encodeType(*Param.ItemType);
            HasExplicitParams = true;
        }
    }
    if (!HasExplicitParams) {
        Result += "v";  // void parameters
    }

    return Result;
}

// Mangle an operator name
std::string Planner::mangleOperator(llvm::StringRef Name,
                                     const std::vector<PlannedItem> &Params,
                                     const PlannedType *Parent) {
    // Map operator names to Itanium ABI operator codes
    std::string OpCode;
    if (Name == "+") OpCode = "pl";
    else if (Name == "-") OpCode = "mi";
    else if (Name == "*") OpCode = "ml";
    else if (Name == "/") OpCode = "dv";
    else if (Name == "%") OpCode = "rm";
    else if (Name == "==") OpCode = "eq";
    else if (Name == "!=") OpCode = "ne";
    else if (Name == "<") OpCode = "lt";
    else if (Name == ">") OpCode = "gt";
    else if (Name == "<=") OpCode = "le";
    else if (Name == ">=") OpCode = "ge";
    else if (Name == "&&") OpCode = "aa";
    else if (Name == "||") OpCode = "oo";
    else if (Name == "!") OpCode = "nt";
    else if (Name == "&") OpCode = "an";
    else if (Name == "|") OpCode = "or";
    else if (Name == "^") OpCode = "eo";
    else if (Name == "~") OpCode = "co";
    else if (Name == "<<") OpCode = "ls";
    else if (Name == ">>") OpCode = "rs";
    else if (Name == "[]") OpCode = "ix";
    else if (Name == "()") OpCode = "cl";
    else {
        // Unknown operator - use vendor extension
        OpCode = "v0" + encodeName(Name);
    }

    std::string Result = "_Z";

    if (Parent) {
        Result += "N";
        Result += encodeType(*Parent);
        Result += OpCode;
        Result += "E";
    } else {
        Result += OpCode;
    }

    // Parameter types (skip 'this' for member operators)
    size_t StartIdx = 0;
    if (Parent && !Params.empty() && Params[0].Name && *Params[0].Name == "this") {
        StartIdx = 1;  // Skip 'this' parameter for member operators
    }

    if (StartIdx >= Params.size()) {
        Result += "v";  // No parameters after 'this'
    } else {
        for (size_t I = StartIdx; I < Params.size(); ++I) {
            if (Params[I].ItemType) {
                Result += encodeType(*Params[I].ItemType);
            }
        }
    }

    return Result;
}

// ============================================================================
// Scope Management
// ============================================================================

void Planner::pushScope() {
    Scopes.emplace_back();
    ScopeInfoStack.emplace_back();
}

void Planner::popScope() {
    if (!Scopes.empty()) {
        Scopes.pop_back();
    }
    if (!ScopeInfoStack.empty()) {
        ScopeInfoStack.pop_back();
    }
}

void Planner::defineLocal(llvm::StringRef Name, const PlannedType &Type, bool IsMutable, bool IsOnPage) {
    if (!Scopes.empty()) {
        Scopes.back()[Name.str()] = LocalBinding{Type, IsMutable, IsOnPage};
    }
}

std::optional<PlannedType> Planner::lookupLocal(llvm::StringRef Name) {
    // Search from innermost to outermost scope
    for (auto It = Scopes.rbegin(); It != Scopes.rend(); ++It) {
        auto Found = It->find(Name.str());
        if (Found != It->end()) {
            return Found->second.Type;
        }
    }
    return std::nullopt;
}

std::optional<Planner::LocalBinding> Planner::lookupLocalBinding(llvm::StringRef Name) {
    // Search from innermost to outermost scope
    for (auto It = Scopes.rbegin(); It != Scopes.rend(); ++It) {
        auto Found = It->find(Name.str());
        if (Found != It->end()) {
            return Found->second;
        }
    }
    return std::nullopt;
}

// ============================================================================
// Type Utilities
// ============================================================================

std::string Planner::getReadableName(const PlannedType &Type) {
    if (Type.Generics.empty()) {
        return Type.Name;
    }

    std::string Result = Type.Name + "[";
    for (size_t I = 0; I < Type.Generics.size(); ++I) {
        if (I > 0) Result += ", ";
        Result += getReadableName(Type.Generics[I]);
    }
    Result += "]";
    return Result;
}

// Normalize type name to canonical form
static std::string normalizeTypeName(llvm::StringRef Name) {
    // Extract the last component of qualified namespace paths (e.g., "scaly.containers.String" -> "String")
    // but NOT generic instantiations (e.g., "Vector.int" stays as "Vector.int")
    // Heuristic: if the name starts with a known package prefix, extract last component
    if (Name.starts_with("scaly.")) {
        size_t LastDot = Name.rfind('.');
        if (LastDot != std::string::npos) {
            Name = Name.substr(LastDot + 1);
        }
    }

    // Integer type aliases
    if (Name == "int" || Name == "i64") return "i64";
    if (Name == "i32") return "i32";
    if (Name == "i16") return "i16";
    if (Name == "i8") return "i8";
    if (Name == "size_t" || Name == "size" || Name == "u64" || Name == "usize") return "u64";
    if (Name == "u32") return "u32";
    if (Name == "u16") return "u16";
    if (Name == "u8") return "u8";
    // Floating point aliases
    if (Name == "double" || Name == "f64") return "f64";
    if (Name == "float" || Name == "f32") return "f32";
    // Others
    return Name.str();
}

bool Planner::typesEqual(const PlannedType &A, const PlannedType &B) {
    // Normalize both names before comparing
    std::string NormA = normalizeTypeName(A.Name);
    std::string NormB = normalizeTypeName(B.Name);
    if (NormA != NormB) return false;
    if (A.Generics.size() != B.Generics.size()) return false;

    for (size_t I = 0; I < A.Generics.size(); ++I) {
        if (!typesEqual(A.Generics[I], B.Generics[I])) {
            return false;
        }
    }
    return true;
}

// Check if ArgType can be implicitly converted to ParamType
static bool isIntegerType(llvm::StringRef Normalized) {
    return Normalized == "i64" || Normalized == "i32" || Normalized == "i16" || Normalized == "i8" ||
           Normalized == "u64" || Normalized == "u32" || Normalized == "u16" || Normalized == "u8" ||
           Normalized == "size_t" || Normalized == "int" || Normalized == "uint";
}

bool Planner::typesCompatible(const PlannedType &ParamType, const PlannedType &ArgType) {
    // First check for exact equality
    if (typesEqual(ParamType, ArgType)) return true;

    // Check for integer type compatibility (allow int <-> size_t, etc.)
    std::string NormParam = normalizeTypeName(ParamType.Name);
    std::string NormArg = normalizeTypeName(ArgType.Name);

    // Allow any integer type to be passed where another integer type is expected
    // (the Emitter will handle any necessary casts)
    if (isIntegerType(NormParam) && isIntegerType(NormArg)) {
        return true;
    }

    // Allow pointer[T] to be passed where T is expected (implicit dereference)
    // This handles 'this' being passed to functions expecting the value type
    if (ArgType.Name == "pointer" && !ArgType.Generics.empty()) {
        if (typesEqual(ParamType, ArgType.Generics[0])) {
            return true;
        }
    }

    // Allow ref[T] to be passed where T is expected (implicit dereference)
    // ref[T] is a non-owning reference that can be implicitly dereferenced
    if (ArgType.Name == "ref" && !ArgType.Generics.empty()) {
        if (typesEqual(ParamType, ArgType.Generics[0])) {
            return true;
        }
    }

    // Allow string literals (pointer[i8]) to be passed where pointer[const_char] is expected
    // This handles C-string function parameters like strlen, String(rp, c_string), etc.
    if (ParamType.Name == "pointer" && ArgType.Name == "pointer" &&
        !ParamType.Generics.empty() && !ArgType.Generics.empty()) {
        if ((ParamType.Generics[0].Name == "const_char" && ArgType.Generics[0].Name == "i8") ||
            (ParamType.Generics[0].Name == "i8" && ArgType.Generics[0].Name == "const_char")) {
            return true;
        }
    }

    // Allow string literals (pointer[i8]) to be passed where String is expected
    // But only as a FALLBACK if no pointer[const_char] initializer is found
    // The Emitter will wrap the C-string in a String constructor
    // NOTE: This should have lower priority than the pointer[const_char] match above
    // For now, we disable this to force the correct initializer selection
    // if (ParamType.Name == "String" && ArgType.Name == "pointer" && !ArgType.Generics.empty()) {
    //     if (ArgType.Generics[0].Name == "i8") {
    //         return true;
    //     }
    // }

    return false;
}

void Planner::computeStructLayout(PlannedStructure &Struct) {
    size_t Offset = 0;
    size_t MaxAlign = 1;

    for (auto &Prop : Struct.Properties) {
        // Get alignment for this property type
        size_t Align = 8;  // Default alignment
        if (Prop.PropType.Name == "bool" || Prop.PropType.Name == "char") {
            Align = 1;
        } else if (Prop.PropType.Name == "int" || Prop.PropType.Name == "uint" ||
                   Prop.PropType.Name == "float") {
            Align = 4;
        }

        // Align offset
        Offset = (Offset + Align - 1) & ~(Align - 1);
        Prop.Offset = Offset;

        // Advance by size
        size_t Size = Align;  // Simplified: size == alignment for primitives
        Offset += Size;

        if (Align > MaxAlign) {
            MaxAlign = Align;
        }
    }

    // Final struct alignment
    Struct.Size = (Offset + MaxAlign - 1) & ~(MaxAlign - 1);
    Struct.Alignment = MaxAlign;
}

void Planner::computeUnionLayout(PlannedUnion &Union) {
    size_t MaxSize = 0;
    size_t MaxAlign = 1;

    for (const auto &Var : Union.Variants) {
        if (Var.VarType) {
            // Get size/alignment for variant type
            size_t Size = 8;   // Default
            size_t Align = 8;

            if (Var.VarType->Name == "bool" || Var.VarType->Name == "char") {
                Size = 1; Align = 1;
            } else if (Var.VarType->Name == "int" || Var.VarType->Name == "uint" ||
                       Var.VarType->Name == "float") {
                Size = 4; Align = 4;
            }

            if (Size > MaxSize) MaxSize = Size;
            if (Align > MaxAlign) MaxAlign = Align;
        }
    }

    // Add space for tag (typically 4 bytes, but could be 1 for small unions)
    size_t TagSize = Union.Variants.size() <= 256 ? 1 : 4;

    // Union size = max variant + tag, aligned
    Union.Size = ((MaxSize + TagSize) + MaxAlign - 1) & ~(MaxAlign - 1);
    Union.Alignment = MaxAlign;
}

// ============================================================================
// Symbol Lookup
// ============================================================================

// Look up a function by qualified package path (e.g., ["scaly", "containers", "test"])
const Function* Planner::lookupPackageFunction(const std::vector<std::string>& Path) {
    if (Path.size() < 2) return nullptr;

    // Check if first element is a loaded package
    auto PkgIt = LoadedPackages.find(Path[0]);
    if (PkgIt == LoadedPackages.end()) return nullptr;

    // Navigate through the package module hierarchy
    const Module* CurrentMod = PkgIt->second;
    const Namespace* CurrentNS = nullptr;

    // The package typically contains a `define <package_name> {...}` concept
    // Look for the namespace with the same name as the package
    for (const auto& Member : CurrentMod->Members) {
        if (auto* Conc = std::get_if<Concept>(&Member)) {
            if (Conc->Name == Path[0]) {
                if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                    CurrentNS = NS;
                    break;
                }
            }
        }
    }

    // Walk through intermediate path elements (start from index 1 since 0 is the package name)
    for (size_t i = 1; i < Path.size() - 1; ++i) {
        const std::string& PathElem = Path[i];
        bool Found = false;

        // If we're in a namespace, check its modules and members
        if (CurrentNS) {
            for (const auto& SubMod : CurrentNS->Modules) {
                if (SubMod.Name == PathElem) {
                    CurrentMod = &SubMod;
                    // Check if the module has a namespace with the same name (the define statement)
                    CurrentNS = nullptr;
                    for (const auto& Member : SubMod.Members) {
                        if (auto* Conc = std::get_if<Concept>(&Member)) {
                            if (Conc->Name == PathElem) {
                                if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                                    CurrentNS = NS;
                                    break;
                                }
                            }
                        }
                    }
                    Found = true;
                    break;
                }
            }
            if (!Found) {
                for (const auto& Member : CurrentNS->Members) {
                    if (auto* Conc = std::get_if<Concept>(&Member)) {
                        if (Conc->Name == PathElem) {
                            if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                                CurrentNS = NS;
                                Found = true;
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            // Check Module's submodules
            for (const auto& SubMod : CurrentMod->Modules) {
                if (SubMod.Name == PathElem) {
                    CurrentMod = &SubMod;
                    Found = true;
                    break;
                }
            }
            // Check Module's members for Concepts with Namespace definitions
            if (!Found) {
                for (const auto& Member : CurrentMod->Members) {
                    if (auto* Conc = std::get_if<Concept>(&Member)) {
                        if (Conc->Name == PathElem) {
                            if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                                CurrentNS = NS;
                                Found = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (!Found) {
            return nullptr;
        }
    }

    // Look for the function in the final module/namespace
    const std::string& FuncName = Path.back();
    if (CurrentNS) {
        for (const auto& Member : CurrentNS->Members) {
            if (auto* Func = std::get_if<Function>(&Member)) {
                if (Func->Name == FuncName) {
                    return Func;
                }
            }
        }
    } else {
        for (const auto& Member : CurrentMod->Members) {
            if (auto* Func = std::get_if<Function>(&Member)) {
                if (Func->Name == FuncName) {
                    return Func;
                }
            }
        }
    }

    return nullptr;
}

std::vector<const Function*> Planner::lookupFunction(llvm::StringRef Name) {
    // Build cache key that includes current structure context
    // Functions visible depend on what structure we're inside
    std::string CacheKey = Name.str();
    if (!CurrentStructureName.empty()) {
        // Extract base struct name for cache key
        std::string BaseName = CurrentStructureName;
        size_t DotPos = BaseName.find('.');
        if (DotPos != std::string::npos) {
            BaseName = BaseName.substr(0, DotPos);
        }
        CacheKey += "@" + BaseName;
    }

    // Check function cache with structure-qualified key
    auto It = Functions.find(CacheKey);
    if (It != Functions.end()) {
        return It->second;
    }

    // Also try unqualified key (for functions registered from sibling modules)
    if (!CurrentStructureName.empty()) {
        auto UnqualifiedIt = Functions.find(Name.str());
        if (UnqualifiedIt != Functions.end()) {
            return UnqualifiedIt->second;
        }
    }

    // Search module stack for functions
    std::vector<const Function*> Result;
    for (auto ModIt = ModuleStack.rbegin(); ModIt != ModuleStack.rend(); ++ModIt) {
        const Module* Mod = *ModIt;
        for (const auto& Member : Mod->Members) {
            if (auto* Func = std::get_if<Function>(&Member)) {
                if (Func->Name == Name) {
                    Result.push_back(Func);
                }
            }
        }
    }

    // If we're inside a structure, also search its methods
    // This allows calling sibling methods like allocate_page() from allocate()
    // Always search the current structure's methods (not just when Result is empty)
    // to ensure we find methods that might shadow module-level functions
    if (!CurrentStructureName.empty()) {
        // Extract base struct name (remove generic suffix like ".T")
        std::string BaseName = CurrentStructureName;
        size_t DotPos = BaseName.find('.');
        if (DotPos != std::string::npos) {
            BaseName = BaseName.substr(0, DotPos);
        }

        const Concept* Conc = lookupConcept(BaseName);
        if (Conc) {
            if (auto* Struct = std::get_if<Structure>(&Conc->Def)) {
                for (const auto& Member : Struct->Members) {
                    if (auto* Func = std::get_if<Function>(&Member)) {
                        if (Func->Name == Name) {
                            Result.push_back(Func);
                        }
                    }
                }
            }
        }
    }

    // Search sibling programs (for multi-file compilation)
    if (Result.empty()) {
        for (const auto& Sibling : SiblingPrograms) {
            // Search sibling's main module
            for (const auto& Member : Sibling->MainModule.Members) {
                if (auto* Func = std::get_if<Function>(&Member)) {
                    if (Func->Name == Name) {
                        Result.push_back(Func);
                    }
                }
            }

            // Also search sibling's sub-modules
            for (const auto& SubMod : Sibling->MainModule.Modules) {
                for (const auto& Member : SubMod.Members) {
                    if (auto* Func = std::get_if<Function>(&Member)) {
                        if (Func->Name == Name) {
                            Result.push_back(Func);
                        }
                    }
                }
            }
        }
    }

    // Cache the result
    if (!Result.empty()) {
        Functions[CacheKey] = Result;
    }

    return Result;
}

std::vector<const Operator*> Planner::lookupOperator(llvm::StringRef Name) {
    // Check operator cache first
    auto It = Operators.find(Name.str());
    if (It != Operators.end()) {
        return It->second;
    }

    // Collect all operators with this name from module stack
    std::vector<const Operator*> Result;

    for (auto ModIt = ModuleStack.rbegin(); ModIt != ModuleStack.rend(); ++ModIt) {
        const Module* Mod = *ModIt;
        for (const auto& Member : Mod->Members) {
            if (auto* Op = std::get_if<Operator>(&Member)) {
                if (Op->Name == Name) {
                    Result.push_back(Op);
                }
            }
        }

        // Also check sub-modules
        for (const auto& SubMod : Mod->Modules) {
            for (const auto& Member : SubMod.Members) {
                if (auto* Op = std::get_if<Operator>(&Member)) {
                    if (Op->Name == Name) {
                        Result.push_back(Op);
                    }
                }
            }
        }
    }

    // Cache the result
    Operators[Name.str()] = Result;
    return Result;
}

bool Planner::isFunction(llvm::StringRef Name) {
    return !lookupFunction(Name).empty();
}

bool Planner::isOperatorName(llvm::StringRef Name) {
    if (Name.empty()) return false;

    // Known name-shaped prefix functions (act like unary operators)
    if (Name == "abs") return true;

    // Operator names start with operator characters
    char First = Name[0];
    return First == '+' || First == '-' || First == '*' || First == '/' ||
           First == '%' || First == '=' || First == '<' || First == '>' ||
           First == '!' || First == '&' || First == '|' || First == '^' ||
           First == '~' || First == '[' || First == '(';
}

int Planner::getOperatorPrecedence(llvm::StringRef Op) {
    // Higher number = binds tighter
    if (Op == "||") return 1;
    if (Op == "&&") return 2;
    if (Op == "|") return 3;
    if (Op == "^") return 4;
    if (Op == "&") return 5;
    if (Op == "=" || Op == "<>") return 6;
    if (Op == "<" || Op == ">" || Op == "<=" || Op == ">=") return 7;
    if (Op == "<<" || Op == ">>") return 8;
    if (Op == "+" || Op == "-") return 9;
    if (Op == "*" || Op == "/" || Op == "%" || Op == "div" || Op == "mod") return 10;
    return 0;  // Unknown
}

std::optional<Planner::MethodMatch> Planner::lookupMethod(
    const PlannedType &StructType,
    llvm::StringRef MethodName,
    Span Loc) {

    const PlannedStructure *Struct = nullptr;

    // Look up the struct in the instantiation cache
    auto StructIt = InstantiatedStructures.find(StructType.Name);
    if (StructIt != InstantiatedStructures.end()) {
        Struct = &StructIt->second;
    } else {
        // Also try the mangled name
        StructIt = InstantiatedStructures.find(StructType.MangledName);
        if (StructIt != InstantiatedStructures.end()) {
            Struct = &StructIt->second;
        }
    }

    // If not found in cache, check if we're currently planning this structure
    // This handles methods calling other methods during structure planning
    if (!Struct && CurrentStructure &&
        (CurrentStructure->Name == StructType.Name ||
         CurrentStructure->MangledName == StructType.MangledName)) {
        Struct = CurrentStructure;
    }

    // If we have an instantiated struct, search in planned methods first
    if (Struct) {
        for (const auto &Method : Struct->Methods) {
            if (Method.Name == MethodName) {
                MethodMatch Match;
                Match.Method = nullptr;  // We have the planned version, not the Model version
                Match.MangledName = Method.MangledName;
                Match.RequiresPageParam = Method.PageParameter.has_value();
                if (Method.Returns) {
                    Match.ReturnType = *Method.Returns;
                } else {
                    // Void return type
                    Match.ReturnType.Name = "void";
                    Match.ReturnType.Loc = Loc;
                }
                return Match;
            }
        }
    }

    // Search in Model's Concept
    // This handles forward references to methods not yet planned,
    // and methods on sibling types that aren't instantiated yet
    // Extract base name from instantiated type (e.g., "Array.char" -> "Array")
    std::string BaseName = StructType.Name;
    size_t DotPos = BaseName.find('.');
    if (DotPos != std::string::npos) {
        BaseName = BaseName.substr(0, DotPos);
    }
    const Concept *Conc = lookupConcept(BaseName);
    if (Conc) {
        if (auto *ModelStruct = std::get_if<Structure>(&Conc->Def)) {
            // Ensure non-generic struct is planned so its methods are registered
            // This is needed for on-demand loaded packages where the struct hasn't been planned yet
            if (Conc->Parameters.empty() && !Struct) {
                // Plan the struct if not already planned
                if (InstantiatedStructures.find(BaseName) == InstantiatedStructures.end()) {
                    auto PlannedConc = planConcept(*Conc);
                    if (PlannedConc) {
                        // Now get the planned struct
                        auto StructIt = InstantiatedStructures.find(BaseName);
                        if (StructIt != InstantiatedStructures.end()) {
                            Struct = &StructIt->second;
                            // Try finding the method in the now-planned struct
                            for (const auto &Method : Struct->Methods) {
                                if (Method.Name == MethodName) {
                                    MethodMatch Match;
                                    Match.Method = nullptr;
                                    Match.MangledName = Method.MangledName;
                                    Match.RequiresPageParam = Method.PageParameter.has_value();
                                    if (Method.Returns) {
                                        Match.ReturnType = *Method.Returns;
                                    } else {
                                        Match.ReturnType.Name = "void";
                                        Match.ReturnType.Loc = Loc;
                                    }
                                    return Match;
                                }
                            }
                        }
                    } else {
                        llvm::consumeError(PlannedConc.takeError());
                    }
                }
            }

            for (const auto &Member : ModelStruct->Members) {
                if (auto *Func = std::get_if<Function>(&Member)) {
                    if (Func->Name == MethodName) {
                        MethodMatch Match;
                        Match.Method = Func;
                        Match.RequiresPageParam = Func->PageParameter.has_value();

                        // Set up type substitutions for the struct's type parameters
                        // This is needed when resolving parameter/return types that use T
                        std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
                        if (!Conc->Parameters.empty() && !StructType.Generics.empty()) {
                            for (size_t I = 0; I < Conc->Parameters.size() && I < StructType.Generics.size(); ++I) {
                                TypeSubstitutions[Conc->Parameters[I].Name] = StructType.Generics[I];
                            }
                        }

                        // Compute mangled name
                        PlannedType ParentType;
                        ParentType.Name = StructType.Name;
                        ParentType.Generics = StructType.Generics;  // Preserve generics for correct mangling
                        // Use Struct's mangled name if available, otherwise compute it
                        if (Struct) {
                            ParentType.MangledName = Struct->MangledName;
                        } else if (!StructType.MangledName.empty()) {
                            ParentType.MangledName = StructType.MangledName;
                        } else if (!StructType.Generics.empty()) {
                            // Compute mangled name with generics
                            ParentType.MangledName = mangleStructure(BaseName, StructType.Generics);
                        } else {
                            ParentType.MangledName = encodeName(StructType.Name);
                        }
                        std::vector<PlannedItem> Params;
                        for (const auto &Inp : Func->Input) {
                            PlannedItem Item;
                            Item.Loc = Inp.Loc;
                            Item.Name = Inp.Name;
                            // Resolve type if present
                            if (Inp.ItemType) {
                                auto Resolved = resolveType(*Inp.ItemType, Inp.Loc);
                                if (Resolved) {
                                    Item.ItemType = std::make_shared<PlannedType>(*Resolved);
                                }
                            }
                            Params.push_back(Item);
                        }
                        Match.MangledName = mangleFunction(Func->Name, Params, &ParentType);

                        // Resolve return type
                        if (Func->Returns) {
                            auto Resolved = resolveType(*Func->Returns, Loc);
                            if (Resolved) {
                                Match.ReturnType = *Resolved;
                            } else {
                                llvm::consumeError(Resolved.takeError());
                                Match.ReturnType.Name = "void";
                                Match.ReturnType.Loc = Loc;
                            }
                        } else {
                            Match.ReturnType.Name = "void";
                            Match.ReturnType.Loc = Loc;
                        }

                        // Restore type substitutions
                        TypeSubstitutions = OldSubst;
                        return Match;
                    }
                }
            }
        }
    }

    return std::nullopt;
}

std::optional<Planner::MethodMatch> Planner::lookupMethod(
    const PlannedType &StructType,
    llvm::StringRef MethodName,
    const std::vector<PlannedType> &ArgTypes,
    Span Loc) {

    // Debug: uncomment to trace method lookups
    // llvm::errs() << "DEBUG lookupMethod(with args): StructType.Name='" << StructType.Name
    //              << "' MethodName='" << MethodName << "'\n";

    // Collect all method candidates
    std::vector<MethodMatch> Candidates;

    const PlannedStructure *Struct = nullptr;

    // Look up the struct in the instantiation cache
    // Handle fully qualified type names like "scaly.containers.String"
    // But preserve generic instantiation names like "List.char"
    std::string LookupName = StructType.Name;
    if (LookupName.rfind("scaly.", 0) == 0) {
        // Strip package prefix to get just the type name
        size_t LastDotForLookup = LookupName.rfind('.');
        if (LastDotForLookup != std::string::npos) {
            LookupName = LookupName.substr(LastDotForLookup + 1);
        }
    }
    auto StructIt = InstantiatedStructures.find(LookupName);
    if (StructIt != InstantiatedStructures.end()) {
        Struct = &StructIt->second;
    } else {
        StructIt = InstantiatedStructures.find(StructType.MangledName);
        if (StructIt != InstantiatedStructures.end()) {
            Struct = &StructIt->second;
        }
    }

    // Check currently planning structure
    if (!Struct && CurrentStructure &&
        (CurrentStructure->Name == StructType.Name ||
         CurrentStructure->MangledName == StructType.MangledName)) {
        Struct = CurrentStructure;
    }

    // Collect from planned methods
    if (Struct) {
        for (const auto &Method : Struct->Methods) {
            if (Method.Name == MethodName) {
                MethodMatch Match;
                Match.Method = nullptr;
                Match.MangledName = Method.MangledName;
                Match.RequiresPageParam = Method.PageParameter.has_value();
                if (Method.Returns) {
                    Match.ReturnType = *Method.Returns;
                } else {
                    Match.ReturnType.Name = "void";
                    Match.ReturnType.Loc = Loc;
                }
                // Collect parameter types (skip 'this')
                for (size_t i = 1; i < Method.Input.size(); ++i) {
                    if (Method.Input[i].ItemType) {
                        Match.ParameterTypes.push_back(*Method.Input[i].ItemType);
                    }
                }
                Candidates.push_back(std::move(Match));
            }
        }
    }

    // Also collect from Model's Concept
    // Extract the actual type name from fully qualified paths like "scaly.containers.String"
    // But for generic instantiation names like "List.char", extract just the base type name
    std::string BaseName = StructType.Name;
    if (BaseName.rfind("scaly.", 0) == 0) {
        // Strip package prefix to get just the type name
        size_t LastDot = BaseName.rfind('.');
        if (LastDot != std::string::npos) {
            BaseName = BaseName.substr(LastDot + 1);
        }
    } else {
        // For generic instantiation names like "List.char", extract the base type
        size_t FirstDot = BaseName.find('.');
        if (FirstDot != std::string::npos) {
            BaseName = BaseName.substr(0, FirstDot);
        }
    }
    const Concept *Conc = lookupConcept(BaseName);
    if (Conc) {
        if (auto *ModelStruct = std::get_if<Structure>(&Conc->Def)) {
            // Ensure non-generic struct is planned so its methods are registered
            // This is needed for on-demand loaded packages where the struct hasn't been planned yet
            if (Conc->Parameters.empty() && !Struct) {
                // Plan the struct if not already planned
                if (InstantiatedStructures.find(BaseName) == InstantiatedStructures.end()) {
                    auto PlannedConc = planConcept(*Conc);
                    if (PlannedConc) {
                        // Now get the planned struct and collect from its methods
                        auto StructIt = InstantiatedStructures.find(BaseName);
                        if (StructIt != InstantiatedStructures.end()) {
                            Struct = &StructIt->second;
                            // Collect candidates from the now-planned struct
                            for (const auto &Method : Struct->Methods) {
                                if (Method.Name == MethodName) {
                                    MethodMatch Match;
                                    Match.Method = nullptr;
                                    Match.MangledName = Method.MangledName;
                                    Match.RequiresPageParam = Method.PageParameter.has_value();
                                    if (Method.Returns) {
                                        Match.ReturnType = *Method.Returns;
                                    } else {
                                        Match.ReturnType.Name = "void";
                                        Match.ReturnType.Loc = Loc;
                                    }
                                    // Collect parameter types (skip 'this')
                                    for (size_t i = 1; i < Method.Input.size(); ++i) {
                                        if (Method.Input[i].ItemType) {
                                            Match.ParameterTypes.push_back(*Method.Input[i].ItemType);
                                        }
                                    }
                                    Candidates.push_back(std::move(Match));
                                }
                            }
                        }
                    } else {
                        llvm::consumeError(PlannedConc.takeError());
                    }
                }
            }

            for (const auto &Member : ModelStruct->Members) {
                if (auto *Func = std::get_if<Function>(&Member)) {
                    if (Func->Name == MethodName) {
                        MethodMatch Match;
                        Match.Method = Func;
                        Match.RequiresPageParam = Func->PageParameter.has_value();

                        // Set up type substitutions for the struct's type parameters
                        // This is needed when resolving parameter/return types that use T
                        std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
                        if (!Conc->Parameters.empty() && !StructType.Generics.empty()) {
                            for (size_t I = 0; I < Conc->Parameters.size() && I < StructType.Generics.size(); ++I) {
                                TypeSubstitutions[Conc->Parameters[I].Name] = StructType.Generics[I];
                            }
                        }

                        // Compute mangled name
                        PlannedType ParentType;
                        ParentType.Name = StructType.Name;
                        ParentType.Generics = StructType.Generics;
                        ParentType.MangledName = Struct ? Struct->MangledName :
                            (StructType.MangledName.empty() ? StructType.Name : StructType.MangledName);

                        std::vector<PlannedItem> Params;
                        for (const auto &Inp : Func->Input) {
                            PlannedItem Item;
                            Item.Loc = Inp.Loc;
                            Item.Name = Inp.Name;
                            if (Inp.ItemType) {
                                auto Resolved = resolveType(*Inp.ItemType, Inp.Loc);
                                if (Resolved) {
                                    Item.ItemType = std::make_shared<PlannedType>(*Resolved);
                                    // Collect parameter types (skip 'this')
                                    if (!Inp.Name || *Inp.Name != "this") {
                                        Match.ParameterTypes.push_back(*Resolved);
                                    }
                                }
                            }
                            Params.push_back(Item);
                        }
                        Match.MangledName = mangleFunction(Func->Name, Params, &ParentType);

                        // Resolve return type
                        if (Func->Returns) {
                            auto Resolved = resolveType(*Func->Returns, Loc);
                            if (Resolved) {
                                Match.ReturnType = *Resolved;
                            } else {
                                llvm::consumeError(Resolved.takeError());
                                Match.ReturnType.Name = "void";
                                Match.ReturnType.Loc = Loc;
                            }
                        } else {
                            Match.ReturnType.Name = "void";
                            Match.ReturnType.Loc = Loc;
                        }
                        TypeSubstitutions = OldSubst;  // Restore substitutions
                        Candidates.push_back(std::move(Match));
                    }
                }
            }
        }
    }

    // Now find the best match based on argument types
    for (const auto &Candidate : Candidates) {
        if (Candidate.ParameterTypes.size() != ArgTypes.size()) {
            continue;
        }

        bool AllMatch = true;
        for (size_t i = 0; i < ArgTypes.size(); ++i) {
            if (!typesEqual(Candidate.ParameterTypes[i], ArgTypes[i])) {
                AllMatch = false;
                break;
            }
        }

        if (AllMatch) {
            return Candidate;
        }
    }

    // No exact match found, fall back to first candidate (for backwards compatibility)
    if (!Candidates.empty()) {
        return Candidates[0];
    }

    return std::nullopt;
}

// ============================================================================
// Method Call Helpers
// ============================================================================

std::vector<PlannedType> Planner::extractArgTypes(const PlannedOperand &ArgsOp) {
    std::vector<PlannedType> ArgTypes;
    if (auto *TupleExpr = std::get_if<PlannedTuple>(&ArgsOp.Expr)) {
        for (const auto &Comp : TupleExpr->Components) {
            for (const auto &ValOp : Comp.Value) {
                ArgTypes.push_back(ValOp.ResultType);
            }
        }
    }
    return ArgTypes;
}

PlannedCall Planner::createMethodCall(
    const MethodMatch &Match,
    PlannedOperand InstanceOp,
    const PlannedOperand &ArgsOp,
    Span Loc) {

    PlannedCall Call;
    Call.Loc = Loc;
    Call.Name = Match.Method ? Match.Method->Name : "";
    Call.MangledName = Match.MangledName;
    Call.IsIntrinsic = false;
    Call.IsOperator = false;
    Call.ResultType = Match.ReturnType;

    Call.Args = std::make_shared<std::vector<PlannedOperand>>();

    // First arg is the instance
    Call.Args->push_back(std::move(InstanceOp));

    // Add tuple elements as additional arguments
    if (auto *TupleExpr = std::get_if<PlannedTuple>(&ArgsOp.Expr)) {
        for (const auto &Comp : TupleExpr->Components) {
            for (const auto &ValOp : Comp.Value) {
                // Make a copy since we're iterating over const ref
                Call.Args->push_back(ValOp);
            }
        }
    }

    return Call;
}

llvm::Expected<PlannedOperand> Planner::generatePageGetThis(Span Loc) {
    // Look up 'this' to get its type
    auto ThisType = lookupLocal("this");
    if (!ThisType) {
        return makePlannerNotImplementedError(File, Loc,
            "^this can only be used in methods (no 'this' in scope)");
    }

    // Look up Page concept to get the Page.get function
    const Concept* PageConcept = lookupConcept("Page");
    if (!PageConcept) {
        return makePlannerNotImplementedError(File, Loc,
            "^this requires Page type (not found in scope)");
    }

    // Find the static 'get' function in Page
    const Function* GetFunc = nullptr;
    if (auto* PageStruct = std::get_if<Structure>(&PageConcept->Def)) {
        for (const auto& Member : PageStruct->Members) {
            if (auto* Func = std::get_if<Function>(&Member)) {
                if (Func->Name == "get") {
                    // Verify it's the static version (no 'this' parameter, takes pointer[void])
                    bool IsStatic = Func->Input.empty() ||
                        !Func->Input[0].Name ||
                        *Func->Input[0].Name != "this";
                    if (IsStatic && Func->Input.size() == 1) {
                        GetFunc = Func;
                        break;
                    }
                }
            }
        }
    }

    if (!GetFunc) {
        return makePlannerNotImplementedError(File, Loc,
            "^this requires Page.get function (not found)");
    }

    // Plan the Page.get function
    auto PlannedFunc = planFunction(*GetFunc, nullptr);
    if (!PlannedFunc) {
        return PlannedFunc.takeError();
    }

    // Add to InstantiatedFunctions if not already present
    if (InstantiatedFunctions.find(PlannedFunc->MangledName) == InstantiatedFunctions.end()) {
        InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;
    }

    // Create the argument: 'this' cast to pointer[void]
    PlannedOperand ThisArg;
    ThisArg.Loc = Loc;
    ThisArg.ResultType = *ThisType;
    ThisArg.Expr = PlannedVariable{Loc, "this", *ThisType, false};

    // Wrap in cast to pointer[void]
    PlannedType VoidPtrType;
    VoidPtrType.Loc = Loc;
    VoidPtrType.Name = "pointer";
    VoidPtrType.MangledName = "Pv";
    PlannedType VoidType;
    VoidType.Name = "void";
    VoidType.MangledName = "v";
    VoidPtrType.Generics.push_back(VoidType);

    PlannedOperand CastArg;
    CastArg.Loc = Loc;
    CastArg.ResultType = VoidPtrType;
    CastArg.Expr = PlannedAs{Loc, VoidPtrType, std::make_shared<PlannedOperand>(std::move(ThisArg))};

    // Create the PlannedCall for Page.get
    PlannedCall Call;
    Call.Loc = Loc;
    Call.Name = "get";
    Call.MangledName = PlannedFunc->MangledName;
    Call.IsIntrinsic = false;
    Call.IsOperator = false;
    Call.Args = std::make_shared<std::vector<PlannedOperand>>();
    Call.Args->push_back(std::move(CastArg));

    // Result type is pointer[Page]
    PlannedType PageType;
    PageType.Loc = Loc;
    PageType.Name = "scaly.memory.Page";
    PageType.MangledName = "N5scaly6memory4PageE";

    PlannedType PagePtrType;
    PagePtrType.Loc = Loc;
    PagePtrType.Name = "pointer";
    PagePtrType.MangledName = "PN5scaly6memory4PageE";
    PagePtrType.Generics.push_back(PageType);
    Call.ResultType = PagePtrType;

    PlannedOperand Result;
    Result.Loc = Loc;
    Result.ResultType = PagePtrType;
    Result.Expr = std::move(Call);

    return Result;
}

// ============================================================================
// Pattern Detection Helpers
// ============================================================================

llvm::Expected<Planner::MethodCallResult> Planner::processChainedMethodCalls(
    PlannedOperand InitialOp,
    const std::vector<Operand> &Ops,
    size_t StartIndex) {

    MethodCallResult Result;
    Result.Matched = true;
    Result.CallOp = std::move(InitialOp);
    Result.ConsumedOperands = 0;

    size_t ChainIdx = StartIndex;
    const Operand* CurrentArgsOp = &Ops[ChainIdx];

    while (CurrentArgsOp->MemberAccess && !CurrentArgsOp->MemberAccess->empty()) {
        auto MemberChain = resolveMemberAccessChain(Result.CallOp.ResultType,
                                                     *CurrentArgsOp->MemberAccess, CurrentArgsOp->Loc);
        if (!MemberChain) {
            return MemberChain.takeError();
        }

        if (!MemberChain->empty() && MemberChain->back().IsMethod) {
            // Check if there's another tuple following for the chained method
            if (ChainIdx + 1 < Ops.size() &&
                std::holds_alternative<Tuple>(Ops[ChainIdx + 1].Expr)) {

                const auto& ChainedArgsOp = Ops[ChainIdx + 1];
                std::string ChainedMethodName = MemberChain->back().Name;
                PlannedType ChainedInstanceType = MemberChain->back().ParentType;

                // Plan the chained method arguments
                Operand ArgsOpCopy = ChainedArgsOp;
                ArgsOpCopy.MemberAccess = nullptr;
                auto PlannedChainedArgs = planOperand(ArgsOpCopy);
                if (!PlannedChainedArgs) {
                    return PlannedChainedArgs.takeError();
                }

                // Look up the chained method
                auto ChainedArgTypes = extractArgTypes(*PlannedChainedArgs);
                auto ChainedMethodMatch = lookupMethod(ChainedInstanceType, ChainedMethodName,
                                                       ChainedArgTypes, ChainedArgsOp.Loc);
                if (ChainedMethodMatch) {
                    // Create chained method call
                    PlannedCall ChainedCall;
                    ChainedCall.Loc = ChainedArgsOp.Loc;
                    ChainedCall.Name = ChainedMethodName;
                    ChainedCall.MangledName = ChainedMethodMatch->MangledName;
                    ChainedCall.IsIntrinsic = false;
                    ChainedCall.IsOperator = false;
                    ChainedCall.ResultType = ChainedMethodMatch->ReturnType;

                    ChainedCall.Args = std::make_shared<std::vector<PlannedOperand>>();

                    // First arg is the instance (current CallOp)
                    ChainedCall.Args->push_back(std::move(Result.CallOp));

                    // Add remaining arguments
                    if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedChainedArgs->Expr)) {
                        for (auto& Comp : TupleExpr->Components) {
                            for (auto& ValOp : Comp.Value) {
                                ChainedCall.Args->push_back(std::move(ValOp));
                            }
                        }
                    }

                    // Replace CallOp with the chained call
                    Result.CallOp = PlannedOperand();
                    Result.CallOp.Loc = ChainedArgsOp.Loc;
                    Result.CallOp.ResultType = ChainedMethodMatch->ReturnType;
                    Result.CallOp.Expr = std::move(ChainedCall);

                    ChainIdx++;
                    Result.ConsumedOperands++;
                    CurrentArgsOp = &Ops[ChainIdx];
                    continue;  // Check for more chained calls
                }
            }
        }

        // Not a chained method call - just apply member access
        Result.CallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
            std::move(*MemberChain));
        if (!Result.CallOp.MemberAccess->empty()) {
            Result.CallOp.ResultType = Result.CallOp.MemberAccess->back().ResultType;
        }
        break;
    }

    return Result;
}

Planner::BindingInfo Planner::checkLocalOrProperty(llvm::StringRef Name) {
    BindingInfo Result;

    // First check if it's a local variable
    auto LocalBind = lookupLocalBinding(Name);
    if (LocalBind) {
        Result.IsLocal = true;
        Result.Binding = LocalBind;
        return Result;
    }

    // Check if it's a property of the current struct
    if (CurrentStructureCtx.isActive() && CurrentStructureCtx.Properties) {
        auto ThisType = lookupLocal("this");
        if (ThisType) {
            // Extract base name for generic types (e.g., "Box.int" -> "Box")
            std::string BaseStructName = CurrentStructureCtx.Name;
            size_t DotPos = BaseStructName.find('.');
            if (DotPos != std::string::npos) {
                BaseStructName = BaseStructName.substr(0, DotPos);
            }

            // 'this' may be pointer[T], so extract the inner type
            std::string ThisTypeName = ThisType->getInnerTypeIfPointerLike().Name;

            if (ThisTypeName == CurrentStructureCtx.Name || ThisTypeName == BaseStructName) {
                for (const auto &Prop : *CurrentStructureCtx.Properties) {
                    if (Prop.Name == Name) {
                        Result.IsProperty = true;
                        return Result;
                    }
                }
            }
        }
    }

    return Result;
}

std::optional<Planner::InitializerMatch> Planner::findInitializer(
    const PlannedType &StructType,
    const std::vector<PlannedType> &ArgTypes) {

    // Look up the struct in the instantiation cache
    // Handle fully qualified type names like "scaly.containers.String"
    std::string LookupName = StructType.Name;
    if (LookupName.rfind("scaly.", 0) == 0) {
        size_t LastDot = LookupName.rfind('.');
        if (LastDot != std::string::npos) {
            LookupName = LookupName.substr(LastDot + 1);
        }
    }
    auto StructIt = InstantiatedStructures.find(LookupName);
    if (StructIt == InstantiatedStructures.end()) {
        // Also try the mangled name
        StructIt = InstantiatedStructures.find(StructType.MangledName);
    }

    // If struct is not in cache OR has no initializers, look up the original Concept
    if (StructIt == InstantiatedStructures.end() ||
        StructIt->second.Initializers.empty()) {
        // Extract base name - handle both package prefixes and generic suffixes
        // "scaly.containers.String" -> "String"
        // "Vector.int" -> "Vector"
        std::string BaseName = StructType.Name;
        if (BaseName.rfind("scaly.", 0) == 0) {
            // Package-qualified name - extract the type name (last component)
            size_t LastDot = BaseName.rfind('.');
            if (LastDot != std::string::npos) {
                BaseName = BaseName.substr(LastDot + 1);
            }
        } else {
            // Generic instantiation name - extract the base type (first component)
            size_t FirstDot = BaseName.find('.');
            if (FirstDot != std::string::npos) {
                BaseName = BaseName.substr(0, FirstDot);
            }
        }

        // Look up the original Concept
        const Concept* Conc = lookupConcept(BaseName);
        if (Conc && std::holds_alternative<Structure>(Conc->Def)) {
            const Structure& OrigStruct = std::get<Structure>(Conc->Def);

            // Set up type substitutions for generic args
            std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
            for (size_t I = 0; I < Conc->Parameters.size() && I < StructType.Generics.size(); ++I) {
                TypeSubstitutions[Conc->Parameters[I].Name] = StructType.Generics[I];
            }

            // Check each original initializer
            for (const auto &Init : OrigStruct.Initializers) {
                if (Init.Input.size() != ArgTypes.size()) {
                    continue;
                }

                bool AllMatch = true;
                for (size_t i = 0; i < ArgTypes.size(); ++i) {
                    if (!Init.Input[i].ItemType) {
                        continue; // No type specified = match
                    }

                    // Resolve the parameter type with current substitutions
                    auto ParamTypeResult = resolveType(*Init.Input[i].ItemType, Init.Loc);
                    if (!ParamTypeResult) {
                        llvm::consumeError(ParamTypeResult.takeError());
                        AllMatch = false;
                        break;
                    }

                    if (!typesCompatible(*ParamTypeResult, ArgTypes[i])) {
                        AllMatch = false;
                        break;
                    }
                }

                if (AllMatch) {
                    // Found a match - generate the mangled name
                    // Note: TypeSubstitutions still has the generic substitutions set up

                    // Mangle constructor name: _ZN<qualified-name>C1E<params>
                    // StructType.MangledName is like "_Z6VectorIiE" - strip the _Z prefix
                    std::string StructName = StructType.MangledName;
                    if (StructName.substr(0, 2) == "_Z") {
                        StructName = StructName.substr(2);
                    }
                    std::string MangledName = "_ZN" + StructName + "C1E";

                    // If init# was used, include page parameter first
                    if (Init.PageParameter) {
                        MangledName += "PN4scaly6memory4PageE";  // pointer[scaly.memory.Page]
                    }

                    // Mangle parameters - need to use parameter types, not argument types
                    // Use encodeType to get the mangling without _Z prefix
                    for (size_t i = 0; i < ArgTypes.size(); ++i) {
                        if (Init.Input[i].ItemType) {
                            auto ParamTypeResult = resolveType(*Init.Input[i].ItemType, Init.Loc);
                            if (ParamTypeResult) {
                                MangledName += encodeType(*ParamTypeResult);
                            } else {
                                llvm::consumeError(ParamTypeResult.takeError());
                                MangledName += encodeType(ArgTypes[i]);
                            }
                        } else {
                            MangledName += encodeType(ArgTypes[i]);
                        }
                    }

                    TypeSubstitutions = OldSubst;

                    InitializerMatch Match;
                    Match.Init = nullptr;  // We don't have a PlannedInitializer yet
                    Match.StructType = StructType;
                    Match.MangledName = MangledName;
                    Match.RequiresPageParam = Init.PageParameter.has_value();
                    return Match;
                }
            }

            TypeSubstitutions = OldSubst;
        }
        return std::nullopt;
    }

    // At this point, StructIt is valid and has initializers
    const PlannedStructure &Struct = StructIt->second;

    // Search for matching initializer in the cached PlannedStructure
    for (const auto &Init : Struct.Initializers) {
        // Check if parameter count matches
        if (Init.Input.size() != ArgTypes.size()) {
            continue;
        }

        // Check if all parameter types match
        bool AllMatch = true;
        for (size_t i = 0; i < ArgTypes.size(); ++i) {
            if (!Init.Input[i].ItemType) {
                // Parameter has no type - treat as match
                continue;
            }
            if (!typesCompatible(*Init.Input[i].ItemType, ArgTypes[i])) {
                AllMatch = false;
                break;
            }
        }

        if (AllMatch) {
            InitializerMatch Match;
            Match.Init = &Init;
            Match.MangledName = Init.MangledName;
            Match.StructType = StructType;
            Match.RequiresPageParam = Init.PageParameter.has_value();
            return Match;
        }
    }

    // llvm::errs() << "DEBUG findInitializer: no match found\n";
    return std::nullopt;
}

std::optional<Planner::OperatorMatch> Planner::findSubscriptOperator(
    const PlannedType &ContainerType, const PlannedType &IndexType, Span Loc) {

    // First, ensure the container type is instantiated if it's a generic type
    // This triggers planStructure which will create the operators
    auto StructIt = InstantiatedStructures.find(ContainerType.Name);
    if (StructIt == InstantiatedStructures.end()) {
        // Try to find and instantiate the structure
        std::string BaseName = ContainerType.Name;
        size_t DotPos = BaseName.find('.');
        if (DotPos != std::string::npos) {
            BaseName = BaseName.substr(0, DotPos);
        }
        const Concept* Conc = lookupConcept(BaseName);
        if (Conc && std::holds_alternative<Structure>(Conc->Def)) {
            // Trigger instantiation by resolving the type
            // Build a Type from ContainerType
            Type TypeExpr;
            TypeExpr.Loc = Loc;
            TypeExpr.Name = {BaseName};
            if (!ContainerType.Generics.empty()) {
                TypeExpr.Generics = std::make_shared<std::vector<Type>>();
                for (const auto &G : ContainerType.Generics) {
                    Type GenArg;
                    GenArg.Loc = Loc;
                    GenArg.Name = {G.Name};
                    GenArg.Life = UnspecifiedLifetime{};
                    TypeExpr.Generics->push_back(GenArg);
                }
            }
            TypeExpr.Life = UnspecifiedLifetime{};
            // This will instantiate the structure if needed
            auto Resolved = resolveType(TypeExpr, Loc);
            if (!Resolved) {
                llvm::consumeError(Resolved.takeError());
            }
        }
    }

    // Look for operator[] defined on the container type
    // Extract base name (e.g., "Vector" from "Vector.int")
    std::string BaseName = ContainerType.Name;
    size_t DotPos = BaseName.find('.');
    if (DotPos != std::string::npos) {
        BaseName = BaseName.substr(0, DotPos);
    }

    // Look up the concept for this type
    const Concept* Conc = lookupConcept(BaseName);
    if (!Conc) {
        return std::nullopt;
    }

    // Check if it's a structure with an operator[] member
    if (auto* Struct = std::get_if<Structure>(&Conc->Def)) {
        for (const auto& Member : Struct->Members) {
            if (auto* Op = std::get_if<Operator>(&Member)) {
                if (Op->Name == "[]") {
                    // Found subscript operator
                    OperatorMatch Match;
                    Match.Op = Op;
                    Match.IsIntrinsic = std::holds_alternative<IntrinsicImpl>(Op->Impl);

                    // Resolve return type with generic substitution
                    if (Op->Returns) {
                        // Set up type substitutions for generics
                        std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
                        if (!Conc->Parameters.empty() && !ContainerType.Generics.empty()) {
                            for (size_t I = 0; I < Conc->Parameters.size() &&
                                              I < ContainerType.Generics.size(); ++I) {
                                TypeSubstitutions[Conc->Parameters[I].Name] =
                                    ContainerType.Generics[I];
                            }
                        }

                        auto RetResult = resolveType(*Op->Returns, Loc);
                        TypeSubstitutions = OldSubst;

                        if (!RetResult) {
                            llvm::consumeError(RetResult.takeError());
                            continue;
                        }
                        Match.ResultType = std::move(*RetResult);
                    }

                    // Generate mangled name using operator's parameter type (not argument type)
                    // This allows implicit conversions (e.g., int -> size_t)
                    // Note: Operator Input doesn't include 'this' - first param is the index
                    std::vector<PlannedItem> ParamItems;
                    if (Op->Input.size() >= 1 && Op->Input[0].ItemType) {
                        // Use the operator's declared parameter type for mangling
                        std::map<std::string, PlannedType> OldSubst2 = TypeSubstitutions;
                        if (!Conc->Parameters.empty() && !ContainerType.Generics.empty()) {
                            for (size_t I = 0; I < Conc->Parameters.size() &&
                                              I < ContainerType.Generics.size(); ++I) {
                                TypeSubstitutions[Conc->Parameters[I].Name] =
                                    ContainerType.Generics[I];
                            }
                        }
                        auto ParamTypeResult = resolveType(*Op->Input[0].ItemType, Loc);
                        TypeSubstitutions = OldSubst2;
                        if (ParamTypeResult) {
                            PlannedItem IdxItem;
                            IdxItem.ItemType = std::make_shared<PlannedType>(*ParamTypeResult);
                            ParamItems.push_back(IdxItem);
                        }
                    }
                    if (ParamItems.empty()) {
                        // Fallback to argument type if operator param not found
                        PlannedItem IdxItem;
                        IdxItem.ItemType = std::make_shared<PlannedType>(IndexType);
                        ParamItems.push_back(IdxItem);
                    }
                    Match.MangledName = mangleOperator("[]", ParamItems, &ContainerType);

                    return Match;
                }
            }
        }
    }

    return std::nullopt;
}

std::optional<Planner::OperatorMatch> Planner::findOperator(
    llvm::StringRef Name, const PlannedType &Left, const PlannedType &Right) {

    std::vector<const Operator*> OpCandidates = lookupOperator(Name);
    for (const Operator* Op : OpCandidates) {
        // Check parameter type matching
        if (Op->Input.size() >= 2 && Op->Input[0].ItemType && Op->Input[1].ItemType) {
            // Resolve the operator's parameter types
            auto LeftParamResult = resolveType(*Op->Input[0].ItemType, Span{});
            if (!LeftParamResult) {
                llvm::consumeError(LeftParamResult.takeError());
                continue;
            }
            auto RightParamResult = resolveType(*Op->Input[1].ItemType, Span{});
            if (!RightParamResult) {
                llvm::consumeError(RightParamResult.takeError());
                continue;
            }

            // Check if parameter types match
            if (typesEqual(*LeftParamResult, Left) &&
                typesEqual(*RightParamResult, Right)) {
                // Found a match
                OperatorMatch Match;
                Match.Op = Op;
                Match.IsIntrinsic = std::holds_alternative<IntrinsicImpl>(Op->Impl);

                // Generate mangled name for the operator
                // Convert PlannedType to PlannedItem for mangling
                std::vector<PlannedItem> ParamItems;
                PlannedItem LeftItem;
                LeftItem.ItemType = std::make_shared<PlannedType>(Left);
                ParamItems.push_back(LeftItem);
                PlannedItem RightItem;
                RightItem.ItemType = std::make_shared<PlannedType>(Right);
                ParamItems.push_back(RightItem);
                Match.MangledName = mangleOperator(Name, ParamItems);

                return Match;
            }
        }
    }

    // Fallback: check for intrinsic operators on primitive types
    // These are built-in and don't require stdlib to be loaded
    static const std::set<std::string> ArithmeticOps = {"+", "-", "*", "/", "%"};
    static const std::set<std::string> ComparisonOps = {"=", "<>", "<", ">", "<=", ">="};
    static const std::set<std::string> BitwiseOps = {"&", "|", "^", "<<", ">>"};

    auto isIntegerType = [](const std::string& Name) {
        return Name == "i64" || Name == "i32" || Name == "i16" || Name == "i8" ||
               Name == "u64" || Name == "u32" || Name == "u16" || Name == "u8" ||
               Name == "int" || Name == "size_t" || Name == "size" || Name == "char";
    };
    auto isFloatType = [](const std::string& Name) {
        return Name == "f64" || Name == "f32" || Name == "float" || Name == "double";
    };
    auto isNumericType = [&](const std::string& Name) {
        return isIntegerType(Name) || isFloatType(Name);
    };

    bool LeftIsPrimitive = isNumericType(Left.Name) || Left.Name == "bool";
    bool RightIsPrimitive = isNumericType(Right.Name) || Right.Name == "bool";

    // Allow operations between compatible primitive types
    // - Same types: always allowed
    // - Different integer types: allowed for comparisons and arithmetic
    // - Integer + float: allowed (implicit conversion)
    bool TypesCompatible = typesEqual(Left, Right) ||
        (isIntegerType(Left.Name) && isIntegerType(Right.Name)) ||
        (isNumericType(Left.Name) && isNumericType(Right.Name));

    if (LeftIsPrimitive && RightIsPrimitive && TypesCompatible) {
        std::string OpName = Name.str();

        // Determine result type and if operator is valid
        PlannedType ResultType = Left;  // Arithmetic ops return same type
        bool IsValidOp = false;

        if (ArithmeticOps.count(OpName)) {
            IsValidOp = true;
        } else if (ComparisonOps.count(OpName)) {
            IsValidOp = true;
            ResultType.Name = "bool";
            ResultType.MangledName = "bool";
        } else if (BitwiseOps.count(OpName) && Left.Name != "f64" && Left.Name != "f32") {
            // Bitwise ops only on integers
            IsValidOp = true;
        }

        if (IsValidOp) {
            OperatorMatch Match;
            Match.Op = nullptr;  // No Operator* for intrinsics
            Match.IsIntrinsic = true;

            // Generate mangled name
            std::vector<PlannedItem> ParamItems;
            PlannedItem LeftItem;
            LeftItem.ItemType = std::make_shared<PlannedType>(Left);
            ParamItems.push_back(LeftItem);
            PlannedItem RightItem;
            RightItem.ItemType = std::make_shared<PlannedType>(Right);
            ParamItems.push_back(RightItem);
            Match.MangledName = mangleOperator(Name, ParamItems);

            return Match;
        }
    }

    // Pointer arithmetic: pointer[T] +/- int, pointer[T] - pointer[T]
    // Also handle struct types used in pointer arithmetic (like 'this + 1')
    // Struct types passed by pointer can participate in pointer arithmetic
    bool IsPointerType = (Left.Name == "pointer" && !Left.Generics.empty());
    bool IsStructType = (!Left.Name.empty() && Left.Name != "pointer" &&
                         Left.Name != "i64" && Left.Name != "i32" &&
                         Left.Name != "i16" && Left.Name != "i8" &&
                         Left.Name != "u64" && Left.Name != "u32" &&
                         Left.Name != "u16" && Left.Name != "u8" &&
                         Left.Name != "f64" && Left.Name != "f32" &&
                         Left.Name != "bool" && Left.Name != "int" &&
                         Left.Name != "float" && Left.Name != "double" &&
                         Left.Name != "size_t" && Left.Name != "char" &&
                         Left.Name != "void");
    bool IsIntegerType = (Right.Name == "i64" || Right.Name == "i32" ||
                          Right.Name == "i16" || Right.Name == "i8" ||
                          Right.Name == "u64" || Right.Name == "u32" ||
                          Right.Name == "u16" || Right.Name == "u8" ||
                          Right.Name == "int" || Right.Name == "size_t");

    if (IsPointerType && IsIntegerType) {
        std::string OpName = Name.str();
        if (OpName == "+" || OpName == "-") {
            OperatorMatch Match;
            Match.Op = nullptr;
            Match.IsIntrinsic = true;
            Match.ResultType = Left;  // Result is same pointer type

            // Generate mangled name
            std::vector<PlannedItem> ParamItems;
            PlannedItem LeftItem;
            LeftItem.ItemType = std::make_shared<PlannedType>(Left);
            ParamItems.push_back(LeftItem);
            PlannedItem RightItem;
            RightItem.ItemType = std::make_shared<PlannedType>(Right);
            ParamItems.push_back(RightItem);
            Match.MangledName = mangleOperator(Name, ParamItems);

            return Match;
        }
    }

    // Pointer difference: pointer[T] - pointer[T] = size_t
    if (IsPointerType && Right.Name == "pointer" && !Right.Generics.empty()) {
        std::string OpName = Name.str();
        if (OpName == "-" && typesEqual(Left, Right)) {
            OperatorMatch Match;
            Match.Op = nullptr;
            Match.IsIntrinsic = true;
            Match.ResultType.Name = "size_t";
            Match.ResultType.MangledName = "y";  // size_t mangling

            std::vector<PlannedItem> ParamItems;
            PlannedItem LeftItem;
            LeftItem.ItemType = std::make_shared<PlannedType>(Left);
            ParamItems.push_back(LeftItem);
            PlannedItem RightItem;
            RightItem.ItemType = std::make_shared<PlannedType>(Right);
            ParamItems.push_back(RightItem);
            Match.MangledName = mangleOperator(Name, ParamItems);

            return Match;
        }
    }

    // Pointer comparison: pointer[T] == pointer[T] returns bool
    if (IsPointerType && Right.Name == "pointer") {
        std::string OpName = Name.str();
        if (OpName == "=" || OpName == "==" || OpName == "<>" || OpName == "!=") {
            OperatorMatch Match;
            Match.Op = nullptr;
            Match.IsIntrinsic = true;
            Match.ResultType.Name = "bool";
            Match.ResultType.MangledName = "b";

            std::vector<PlannedItem> ParamItems;
            PlannedItem LeftItem;
            LeftItem.ItemType = std::make_shared<PlannedType>(Left);
            ParamItems.push_back(LeftItem);
            PlannedItem RightItem;
            RightItem.ItemType = std::make_shared<PlannedType>(Right);
            ParamItems.push_back(RightItem);
            Match.MangledName = mangleOperator(Name, ParamItems);

            return Match;
        }
    }

    // Struct type + int: allows 'this + 1' pattern where 'this' is passed by pointer
    // The result is pointer[StructType]
    if (IsStructType && IsIntegerType) {
        std::string OpName = Name.str();
        if (OpName == "+" || OpName == "-") {
            OperatorMatch Match;
            Match.Op = nullptr;
            Match.IsIntrinsic = true;

            // Result type is pointer to the struct
            Match.ResultType.Loc = Left.Loc;
            Match.ResultType.Name = "pointer";
            Match.ResultType.Generics.push_back(Left);
            Match.ResultType.MangledName = "P" + Left.MangledName;

            std::vector<PlannedItem> ParamItems;
            PlannedItem LeftItem;
            LeftItem.ItemType = std::make_shared<PlannedType>(Left);
            ParamItems.push_back(LeftItem);
            PlannedItem RightItem;
            RightItem.ItemType = std::make_shared<PlannedType>(Right);
            ParamItems.push_back(RightItem);
            Match.MangledName = mangleOperator(Name, ParamItems);

            return Match;
        }
    }

    return std::nullopt;
}

// ============================================================================
// Operation Resolution
// ============================================================================

llvm::Expected<PlannedType> Planner::resolveFunctionCall(
    llvm::StringRef Name, Span Loc,
    const std::vector<PlannedType> &ArgTypes) {

    // Look up function candidates
    auto Candidates = lookupFunction(Name);
    if (Candidates.empty()) {
        // Not a known function - might be a type constructor
        const Concept* Conc = lookupConcept(Name);
        if (Conc) {
            // Type constructor - return the type
            PlannedType Result;
            Result.Loc = Loc;
            Result.Name = Name.str();
            Result.MangledName = mangleType(Result);
            return Result;
        }

        // Unknown function
        return makePlannerNotImplementedError(File, Loc,
            ("function not found: " + Name).str());
    }

    // Overload resolution: find function with matching arity and parameter types
    // First, try non-generic functions
    const Function* BestMatch = nullptr;
    for (const Function* Func : Candidates) {
        // Skip generic functions in first pass
        if (!Func->Parameters.empty()) {
            continue;
        }

        if (Func->Input.size() != ArgTypes.size()) {
            continue;
        }

        // Check parameter types match
        bool AllMatch = true;
        for (size_t I = 0; I < ArgTypes.size(); ++I) {
            if (!Func->Input[I].ItemType) {
                // Parameter has no type annotation - match any type
                continue;
            }

            auto ParamTypeResult = resolveType(*Func->Input[I].ItemType, Loc);
            if (!ParamTypeResult) {
                // Skip if we can't resolve the parameter type
                llvm::consumeError(ParamTypeResult.takeError());
                AllMatch = false;
                break;
            }

            if (!typesCompatible(*ParamTypeResult, ArgTypes[I])) {
                AllMatch = false;
                break;
            }
        }

        if (AllMatch) {
            BestMatch = Func;
            break;  // Take first matching overload
        }
    }

    if (BestMatch) {
        // Plan the function if it's not already in InstantiatedFunctions
        // This handles functions from sibling files
        auto Planned = planFunction(*BestMatch, nullptr);
        if (Planned) {
            // Add to InstantiatedFunctions if not already present
            if (InstantiatedFunctions.find(Planned->MangledName) == InstantiatedFunctions.end()) {
                InstantiatedFunctions[Planned->MangledName] = *Planned;
            }
        } else {
            // If planning fails, return the error
            return Planned.takeError();
        }

        if (BestMatch->Returns) {
            return resolveType(*BestMatch->Returns, Loc);
        }
        // No return type = void
        PlannedType VoidType;
        VoidType.Loc = Loc;
        VoidType.Name = "void";
        VoidType.MangledName = "v";
        return VoidType;
    }

    // Second pass: try generic functions with type inference
    for (const Function* Func : Candidates) {
        // Only consider generic functions
        if (Func->Parameters.empty()) {
            continue;
        }

        if (Func->Input.size() != ArgTypes.size()) {
            continue;
        }

        // Try to infer type arguments from the call arguments
        auto InferredArgs = inferTypeArguments(*Func, ArgTypes, Loc);
        if (!InferredArgs) {
            // Inference failed - try next candidate
            llvm::consumeError(InferredArgs.takeError());
            continue;
        }

        // Instantiate the generic function
        auto Instantiated = instantiateGenericFunction(*Func, *InferredArgs, Loc);
        if (!Instantiated) {
            // Instantiation failed - try next candidate
            llvm::consumeError(Instantiated.takeError());
            continue;
        }

        // Return the instantiated function's return type
        if (Instantiated->Returns) {
            return *Instantiated->Returns;
        }
        // No return type = void
        PlannedType VoidType;
        VoidType.Loc = Loc;
        VoidType.Name = "void";
        VoidType.MangledName = "v";
        return VoidType;
    }

    // Third pass: try methods with implicit 'this'
    // When calling allocate_oversized(size) from within Page, treat as this.allocate_oversized(size)
    if (!CurrentStructureName.empty()) {
        for (const Function* Func : Candidates) {
            // Skip generic functions
            if (!Func->Parameters.empty()) {
                continue;
            }

            // Check if this is a method (first param is "this")
            if (Func->Input.empty() || !Func->Input[0].Name ||
                *Func->Input[0].Name != "this") {
                continue;
            }

            // Check if arity matches with implicit this
            if (Func->Input.size() != ArgTypes.size() + 1) {
                continue;
            }

            // Check parameter types match (starting from second param)
            bool AllMatch = true;
            for (size_t I = 0; I < ArgTypes.size(); ++I) {
                if (!Func->Input[I + 1].ItemType) {
                    // Parameter has no type annotation - match any type
                    continue;
                }

                auto ParamTypeResult = resolveType(*Func->Input[I + 1].ItemType, Loc);
                if (!ParamTypeResult) {
                    llvm::consumeError(ParamTypeResult.takeError());
                    AllMatch = false;
                    break;
                }

                if (!typesCompatible(*ParamTypeResult, ArgTypes[I])) {
                    AllMatch = false;
                    break;
                }
            }

            if (AllMatch) {
                BestMatch = Func;
                break;
            }
        }

        if (BestMatch) {
            if (BestMatch->Returns) {
                return resolveType(*BestMatch->Returns, Loc);
            }
            PlannedType VoidType;
            VoidType.Loc = Loc;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            return VoidType;
        }
    }

    // No matching overload - build helpful error message
    std::string ArgTypesStr;
    for (size_t I = 0; I < ArgTypes.size(); ++I) {
        if (I > 0) ArgTypesStr += ", ";
        ArgTypesStr += ArgTypes[I].Name;
    }
    return makePlannerNotImplementedError(File, Loc,
        ("no matching overload for function: " + Name + "(" + ArgTypesStr + ")").str());
}

llvm::Expected<PlannedType> Planner::resolveOperatorCall(
    llvm::StringRef Name, Span Loc,
    const PlannedType &Left, const PlannedType &Right) {

    // First, check if the left type is a structure with this operator
    auto StructIt = InstantiatedStructures.find(Left.Name);
    if (StructIt != InstantiatedStructures.end()) {
        for (const auto &Op : StructIt->second.Operators) {
            if (Op.Name == Name) {
                // Check parameter type matching
                // Instance operators have: first param is receiver (this), second is the operand
                // For binary operators, we check if the second parameter matches Right
                if (Op.Input.size() >= 2 && Op.Input[1].ItemType) {
                    if (!typesEqual(*Op.Input[1].ItemType, Right)) {
                        continue;  // Try next operator overload
                    }
                }
                if (Op.Returns) {
                    return *Op.Returns;
                }
                PlannedType VoidType;
                VoidType.Loc = Loc;
                VoidType.Name = "void";
                VoidType.MangledName = "v";
                return VoidType;
            }
        }
    }

    // Check unions as well
    auto UnionIt = InstantiatedUnions.find(Left.Name);
    if (UnionIt != InstantiatedUnions.end()) {
        for (const auto &Op : UnionIt->second.Operators) {
            if (Op.Name == Name) {
                // Check parameter type matching for union operators
                if (Op.Input.size() >= 2 && Op.Input[1].ItemType) {
                    if (!typesEqual(*Op.Input[1].ItemType, Right)) {
                        continue;  // Try next operator overload
                    }
                }
                if (Op.Returns) {
                    return *Op.Returns;
                }
                PlannedType VoidType;
                VoidType.Loc = Loc;
                VoidType.Name = "void";
                VoidType.MangledName = "v";
                return VoidType;
            }
        }
    }

    // Look up module-level operators (all overloads)
    std::vector<const Operator*> OpCandidates = lookupOperator(Name);
    for (const Operator* Op : OpCandidates) {
        // Check parameter type matching
        if (Op->Input.size() >= 2 && Op->Input[0].ItemType && Op->Input[1].ItemType) {
            // Resolve the operator's parameter types
            auto LeftParamResult = resolveType(*Op->Input[0].ItemType, Loc);
            if (!LeftParamResult) {
                // Skip if we can't resolve the type
                llvm::consumeError(LeftParamResult.takeError());
                continue;
            }
            auto RightParamResult = resolveType(*Op->Input[1].ItemType, Loc);
            if (!RightParamResult) {
                llvm::consumeError(RightParamResult.takeError());
                continue;
            }

            // Check if parameter types match
            if (typesEqual(*LeftParamResult, Left) &&
                typesEqual(*RightParamResult, Right)) {
                if (Op->Returns) {
                    return resolveType(*Op->Returns, Loc);
                }
                // No return type = void (unusual for operators)
                PlannedType VoidType;
                VoidType.Loc = Loc;
                VoidType.Name = "void";
                VoidType.MangledName = "v";
                return VoidType;
            }
        }
    }

    // Built-in operators for primitive types
    // Note: The full Scaly stdlib uses wrapper types (Sum, Product, etc.)
    // for operator precedence. This is a simplified built-in fallback.

    // Arithmetic operators on numeric types
    static const std::set<std::string> ArithmeticOps = {"+", "-", "*", "/", "%", "div", "mod"};
    static const std::set<std::string> IntTypes = {"i64", "i32", "i16", "i8", "u64", "u32", "u16", "u8"};
    static const std::set<std::string> FloatTypes = {"f64", "f32"};

    if (ArithmeticOps.count(Name.str())) {
        // Same type arithmetic: T op T -> T
        if (typesEqual(Left, Right)) {
            if (IntTypes.count(Left.Name) || FloatTypes.count(Left.Name)) {
                return Left;
            }
        }
        // Mixed int/float -> larger float type
        if ((IntTypes.count(Left.Name) && FloatTypes.count(Right.Name)) ||
            (FloatTypes.count(Left.Name) && IntTypes.count(Right.Name))) {
            PlannedType Result;
            Result.Loc = Loc;
            Result.Name = "f64";
            Result.MangledName = "f64";
            return Result;
        }
    }

    // Comparison operators return bool
    static const std::set<std::string> ComparisonOps = {"=", "==", "<>", "!=", "<", ">", "<=", ">="};
    if (ComparisonOps.count(Name.str())) {
        // Allow comparison of same numeric types
        if ((IntTypes.count(Left.Name) || FloatTypes.count(Left.Name) ||
             Left.Name == "bool" || Left.Name == "char") &&
            (IntTypes.count(Right.Name) || FloatTypes.count(Right.Name) ||
             Right.Name == "bool" || Right.Name == "char")) {
            PlannedType Result;
            Result.Loc = Loc;
            Result.Name = "bool";
            Result.MangledName = "b";
            return Result;
        }

        // Allow pointer comparison (equality/inequality only)
        if ((Left.Name == "pointer" && Right.Name == "pointer") &&
            (Name == "=" || Name == "==" || Name == "<>" || Name == "!=")) {
            PlannedType Result;
            Result.Loc = Loc;
            Result.Name = "bool";
            Result.MangledName = "b";
            return Result;
        }
    }

    // Boolean operators
    bool IsBoolean = (Name == "and" || Name == "or" || Name == "&&" || Name == "||");
    if (IsBoolean && Left.Name == "bool" && Right.Name == "bool") {
        PlannedType Result;
        Result.Loc = Loc;
        Result.Name = "bool";
        Result.MangledName = "b";
        return Result;
    }

    // Bitwise operators on integers
    bool IsBitwise = (Name == "&" || Name == "|" || Name == "^" ||
                      Name == "<<" || Name == ">>");
    if (IsBitwise && Left.Name == "int" && Right.Name == "int") {
        PlannedType Result;
        Result.Loc = Loc;
        Result.Name = "int";
        Result.MangledName = "i";
        return Result;
    }

    // String concatenation
    if (Name == "+" && Left.Name == "String" && Right.Name == "String") {
        PlannedType Result;
        Result.Loc = Loc;
        Result.Name = "String";
        Result.MangledName = "6String";
        return Result;
    }

    // Unknown operator
    return makePlannerNotImplementedError(File, Loc,
        ("operator not found: " + Name + " for types " +
         Left.Name + " and " + Right.Name).str());
}

std::optional<PlannedType> Planner::getPointerElementType(const PlannedType &PtrType) {
    // Check if this is a pointer type with a generic argument
    if (PtrType.Name == "pointer" && !PtrType.Generics.empty()) {
        return PtrType.Generics[0];
    }

    // Check if this is a ref type with a generic argument (non-owning reference)
    if (PtrType.Name == "ref" && !PtrType.Generics.empty()) {
        return PtrType.Generics[0];
    }

    // Check for pointer[T] in name form (e.g., from unresolved types)
    if (PtrType.Name.substr(0, 8) == "pointer[") {
        // This shouldn't happen after resolution, but handle it gracefully
        return std::nullopt;
    }

    // Check for ref[T] in name form
    if (PtrType.Name.substr(0, 4) == "ref[") {
        return std::nullopt;
    }

    return std::nullopt;
}

llvm::Expected<PlannedType> Planner::resolvePrefixOperatorCall(
    llvm::StringRef Name, Span Loc,
    const PlannedType &Operand) {

    // Dereference operator: *ptr -> element type
    if (Name == "*") {
        auto ElementType = getPointerElementType(Operand);
        if (ElementType) {
            return *ElementType;
        }
        // If not a pointer, this is an error
        return makePlannerNotImplementedError(File, Loc,
            "cannot dereference non-pointer type: " + Operand.Name);
    }

    // Address-of operator: &val -> pointer[typeof(val)]
    if (Name == "&") {
        PlannedType PtrType;
        PtrType.Loc = Loc;
        PtrType.Name = "pointer";
        PtrType.Generics.push_back(Operand);
        PtrType.MangledName = "P" + Operand.MangledName;
        return PtrType;
    }

    // Negation: -x for numeric types
    if (Name == "-") {
        if (Operand.Name == "i64" || Operand.Name == "i32" ||
            Operand.Name == "i16" || Operand.Name == "i8" ||
            Operand.Name == "f64" || Operand.Name == "f32") {
            return Operand;  // Same type
        }
    }

    // Bitwise/logical not: ~x for integers and bools
    if (Name == "~") {
        if (Operand.Name == "i64" || Operand.Name == "i32" ||
            Operand.Name == "i16" || Operand.Name == "i8" ||
            Operand.Name == "u64" || Operand.Name == "u32" ||
            Operand.Name == "u16" || Operand.Name == "u8" ||
            Operand.Name == "bool") {
            return Operand;
        }
    }

    // Unary plus: +x
    if (Name == "+") {
        if (Operand.Name == "i64" || Operand.Name == "i32" ||
            Operand.Name == "i16" || Operand.Name == "i8" ||
            Operand.Name == "f64" || Operand.Name == "f32") {
            return Operand;
        }
    }

    // Absolute value: abs x
    if (Name == "abs") {
        if (Operand.Name == "i64" || Operand.Name == "i32" ||
            Operand.Name == "i16" || Operand.Name == "i8" ||
            Operand.Name == "f64" || Operand.Name == "f32") {
            return Operand;
        }
    }

    // Unknown prefix operator
    return makePlannerNotImplementedError(File, Loc,
        ("unknown prefix operator: " + Name + " for type " + Operand.Name).str());
}

llvm::Expected<PlannedType> Planner::resolveOperationSequence(
    const std::vector<PlannedOperand> &Ops) {

    if (Ops.empty()) {
        PlannedType VoidType;
        VoidType.Name = "void";
        VoidType.MangledName = "v";
        return VoidType;
    }

    if (Ops.size() == 1) {
        return Ops[0].ResultType;
    }

    // Check if first operand is a prefix operator (context is empty)
    size_t I = 0;
    PlannedType Context;
    bool HasContext = false;

    // Check for prefix operator at the start
    if (auto* TypeExpr = std::get_if<PlannedType>(&Ops[0].Expr)) {
        if (isOperatorName(TypeExpr->Name) && Ops.size() > 1) {
            // First operand is a prefix operator
            // Resolve the rest of the sequence first, then apply prefix operator
            std::vector<PlannedOperand> RestOps(Ops.begin() + 1, Ops.end());
            auto RestResult = resolveOperationSequence(RestOps);
            if (!RestResult) {
                return RestResult.takeError();
            }
            auto Result = resolvePrefixOperatorCall(TypeExpr->Name,
                                                     Ops[0].Loc,
                                                     *RestResult);
            if (!Result) {
                return Result.takeError();
            }
            return *Result;  // Entire sequence is resolved
        }
    }

    // Check for cast expression: (Type)value
    // A cast is when the first operand is a tuple containing a single type
    if (!HasContext) {
        if (auto* TupleExpr = std::get_if<PlannedTuple>(&Ops[0].Expr)) {
            // Single-component tuple with a type expression is a cast
            if (TupleExpr->Components.size() == 1 &&
                !TupleExpr->Components[0].Name &&
                TupleExpr->Components[0].Value.size() == 1) {
                const auto& Inner = TupleExpr->Components[0].Value[0];
                if (std::holds_alternative<PlannedType>(Inner.Expr)) {
                    // This is a cast: (Type)value - result type is Type
                    Context = TupleExpr->TupleType;
                    HasContext = true;
                    I = 1;  // Skip the cast type, rest is the value being cast

                    // The cast result IS the type, regardless of what follows
                    // So just return the cast type
                    return Context;
                }
            }
        }
    }

    // If no prefix operator, first operand becomes context
    if (!HasContext) {
        Context = Ops[0].ResultType;
        I = 1;
    }

    // Process remaining operation sequence left-to-right
    while (I < Ops.size()) {
        const PlannedOperand& Current = Ops[I];

        // Check if current operand is an operator/function
        if (auto* TypeExpr = std::get_if<PlannedType>(&Current.Expr)) {
            if (isOperatorName(TypeExpr->Name)) {
                // Operator - needs right operand
                if (I + 1 < Ops.size()) {
                    const PlannedType& Right = Ops[I + 1].ResultType;
                    auto Result = resolveOperatorCall(TypeExpr->Name,
                                                       Current.Loc, Context, Right);
                    if (!Result) {
                        return Result.takeError();
                    }
                    Context = std::move(*Result);
                    I += 2;  // Skip operator and right operand
                    continue;
                } else {
                    // Trailing operator with no right operand - treat as unary postfix
                    // (rare case, but handle gracefully)
                    I++;
                    continue;
                }
            } else if (isFunction(TypeExpr->Name)) {
                // Function call - collect arguments
                // For now, just use the next operand as the argument
                if (I + 1 < Ops.size()) {
                    std::vector<PlannedType> ArgTypes = {Ops[I + 1].ResultType};
                    auto Result = resolveFunctionCall(TypeExpr->Name,
                                                       Current.Loc, ArgTypes);
                    if (!Result) {
                        return Result.takeError();
                    }
                    Context = std::move(*Result);
                    I += 2;
                    continue;
                }
            }
        }

        // Check for type constructor: Type(args)
        // Context is a struct type, Current is a Tuple (constructor args)
        if (auto* TupleExpr = std::get_if<PlannedTuple>(&Current.Expr)) {
            // Check if Context is a known struct type
            const Concept* Conc = lookupConcept(Context.Name);
            if (Conc && std::holds_alternative<Structure>(Conc->Def)) {
                // This is a constructor call: Type(args)
                // Context remains as the struct type - the tuple provides init values
                I++;
                continue;
            }
        }

        // Default: use the operand's type directly
        Context = Current.ResultType;
        I++;
    }

    return Context;
}

llvm::Expected<PlannedOperand> Planner::collapseOperandSequence(
    std::vector<PlannedOperand> Ops) {

    if (Ops.empty()) {
        return makePlannerNotImplementedError(File, Span{},
            "empty operand sequence");
    }

    // Single operand: return as-is
    if (Ops.size() == 1) {
        return std::move(Ops[0]);
    }

    // Handle cast expression: (Type)value
    if (auto* TupleExpr = std::get_if<PlannedTuple>(&Ops[0].Expr)) {
        // Single-component tuple with a type expression is a cast
        if (TupleExpr->Components.size() == 1 &&
            !TupleExpr->Components[0].Name &&
            TupleExpr->Components[0].Value.size() == 1) {
            const auto& Inner = TupleExpr->Components[0].Value[0];
            if (std::holds_alternative<PlannedType>(Inner.Expr)) {
                // This is a cast - collapse the rest and apply cast type
                std::vector<PlannedOperand> RestOps(Ops.begin() + 1, Ops.end());
                if (RestOps.empty()) {
                    // Just (Type) with no value - return the tuple as-is
                    return std::move(Ops[0]);
                }
                auto ValueResult = collapseOperandSequence(std::move(RestOps));
                if (!ValueResult) {
                    return ValueResult.takeError();
                }
                // Override the result type with the cast type
                ValueResult->ResultType = TupleExpr->TupleType;
                return ValueResult;
            }
        }
    }

    // Handle prefix operator (e.g., -x, !x, *ptr)
    // Prefix operators only consume ONE operand (the immediately following one)
    // For `*p >= 5`, `*` applies to `p`, then the result is compared with `5`
    if (auto* TypeExpr = std::get_if<PlannedType>(&Ops[0].Expr)) {
        if (isOperatorName(TypeExpr->Name) && Ops.size() >= 2) {
            // Find the operand for this prefix operator
            // It's either:
            // 1. Another prefix operator followed by its operand (e.g., *-x)
            // 2. A non-operator value (e.g., *p)
            PlannedOperand PrefixOperand;
            size_t ConsumedOps = 1; // Start after the prefix operator itself

            // Check if next element is also a prefix operator
            if (auto* NextType = std::get_if<PlannedType>(&Ops[1].Expr)) {
                if (isOperatorName(NextType->Name)) {
                    // Recursively handle chained prefix operators (e.g., *-x)
                    // Take everything from the second prefix operator to the end
                    // then extract just the prefix chain result
                    std::vector<PlannedOperand> ChainOps(Ops.begin() + 1, Ops.end());
                    auto ChainResult = collapseOperandSequence(std::move(ChainOps));
                    if (!ChainResult) {
                        return ChainResult.takeError();
                    }
                    // This consumes the entire chain, but we only want the prefix part
                    // Actually, we need to figure out how many ops the inner prefix consumed
                    // For simplicity, just take Ops[1] as the inner prefix operand directly
                    // This handles cases like *-p but may not be perfect for all cases
                    PrefixOperand = std::move(*ChainResult);
                    // Return immediately since the entire sequence was consumed

                    // Create PlannedCall for THIS prefix operator (the outer one)
                    PlannedCall Call;
                    Call.Loc = Ops[0].Loc;
                    Call.Name = TypeExpr->Name;
                    Call.IsOperator = true;
                    Call.IsIntrinsic = true;

                    Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                    Call.Args->push_back(std::move(PrefixOperand));

                    PlannedType OperandType = Call.Args->front().ResultType;
                    if (TypeExpr->Name == "!" || TypeExpr->Name == "not") {
                        Call.ResultType.Name = "bool";
                        Call.ResultType.MangledName = "bool";
                    } else if (TypeExpr->Name == "*") {
                        auto ElementType = getPointerElementType(OperandType);
                        if (ElementType) {
                            Call.ResultType = *ElementType;
                        } else {
                            // Not a pointer type - raise an error
                            return makePlannerNotImplementedError(File, Ops[0].Loc,
                                "cannot dereference non-pointer type: " + OperandType.Name);
                        }
                    } else if (TypeExpr->Name == "&") {
                        Call.ResultType.Loc = Ops[0].Loc;
                        Call.ResultType.Name = "pointer";
                        Call.ResultType.Generics.push_back(OperandType);
                        Call.ResultType.MangledName = "P" + OperandType.MangledName;
                    } else {
                        Call.ResultType = OperandType;
                    }

                    std::vector<PlannedItem> ParamItems;
                    PlannedItem Item;
                    Item.ItemType = std::make_shared<PlannedType>(OperandType);
                    ParamItems.push_back(Item);
                    Call.MangledName = mangleOperator(TypeExpr->Name, ParamItems);

                    PlannedOperand Result;
                    Result.Loc = Call.Loc;
                    Result.ResultType = Call.ResultType;
                    Result.Expr = std::move(Call);
                    return Result;
                }
            }

            // Next element is a regular operand (not a prefix operator)
            PrefixOperand = std::move(Ops[1]);
            ConsumedOps = 2; // prefix operator + its operand

            // Create PlannedCall for prefix operator
            PlannedCall Call;
            Call.Loc = Ops[0].Loc;
            Call.Name = TypeExpr->Name;
            Call.IsOperator = true;
            Call.IsIntrinsic = true;  // Prefix operators on primitives are intrinsic

            Call.Args = std::make_shared<std::vector<PlannedOperand>>();
            Call.Args->push_back(std::move(PrefixOperand));

            // Result type: depends on the operator
            PlannedType OperandType = Call.Args->front().ResultType;
            if (TypeExpr->Name == "!" || TypeExpr->Name == "not") {
                Call.ResultType.Name = "bool";
                Call.ResultType.MangledName = "bool";
            } else if (TypeExpr->Name == "*") {
                // Dereference operator: result type is the element type
                auto ElementType = getPointerElementType(OperandType);
                if (ElementType) {
                    Call.ResultType = *ElementType;
                } else {
                    // Not a pointer type - raise an error
                    return makePlannerNotImplementedError(File, Ops[0].Loc,
                        "cannot dereference non-pointer type: " + OperandType.Name);
                }
            } else if (TypeExpr->Name == "&") {
                // Address-of operator: result type is pointer[operand type]
                Call.ResultType.Loc = Ops[0].Loc;
                Call.ResultType.Name = "pointer";
                Call.ResultType.Generics.push_back(OperandType);
                Call.ResultType.MangledName = "P" + OperandType.MangledName;
            } else {
                // Negation, bitwise not: result type = operand type
                Call.ResultType = OperandType;
            }

            // Generate mangled name for unary operator
            std::vector<PlannedItem> ParamItems;
            PlannedItem Item;
            Item.ItemType = std::make_shared<PlannedType>(OperandType);
            ParamItems.push_back(Item);
            Call.MangledName = mangleOperator(TypeExpr->Name, ParamItems);

            PlannedOperand PrefixResult;
            PrefixResult.Loc = Call.Loc;
            PrefixResult.ResultType = Call.ResultType;
            PrefixResult.Expr = std::move(Call);

            // If there are remaining operands after the prefix operator and its operand,
            // continue processing with the result as the new left operand
            // For `*p >= 5`: Ops = [*, p, >=, 5], after prefix: [*p_result, >=, 5]
            if (ConsumedOps < Ops.size()) {
                std::vector<PlannedOperand> RemainingOps;
                RemainingOps.push_back(std::move(PrefixResult));
                for (size_t j = ConsumedOps; j < Ops.size(); ++j) {
                    RemainingOps.push_back(std::move(Ops[j]));
                }
                return collapseOperandSequence(std::move(RemainingOps));
            }
            return PrefixResult;
        }
    }

    // Simple binary case: value op value (3 operands)
    // Left-to-right processing for longer sequences
    PlannedOperand Left = std::move(Ops[0]);
    size_t I = 1;

    while (I < Ops.size()) {
        // Check if current operand is an operator
        if (auto* TypeExpr = std::get_if<PlannedType>(&Ops[I].Expr)) {
            if (isOperatorName(TypeExpr->Name) && I + 1 < Ops.size()) {
                // Binary operator: left op right
                size_t RightConsumed = 1;

                // Check for higher-precedence operators on the right
                // E.g., for "3 + 4 * 2": when at +, see that * has higher precedence
                int CurrentPrec = getOperatorPrecedence(TypeExpr->Name);
                if (CurrentPrec > 0 && I + 2 < Ops.size()) {
                    if (auto* NextOp = std::get_if<PlannedType>(&Ops[I + 2].Expr)) {
                        if (isOperatorName(NextOp->Name)) {
                            int NextPrec = getOperatorPrecedence(NextOp->Name);
                            if (NextPrec > CurrentPrec) {
                                // Higher precedence on right - collapse right side first
                                std::vector<PlannedOperand> RightOps;
                                for (size_t j = I + 1; j < Ops.size(); ++j) {
                                    RightOps.push_back(std::move(Ops[j]));
                                }
                                auto CollapsedRight = collapseOperandSequence(std::move(RightOps));
                                if (!CollapsedRight) {
                                    return CollapsedRight.takeError();
                                }
                                // Replace Ops[I + 1] with the collapsed result
                                Ops[I + 1] = std::move(*CollapsedRight);
                                // Truncate Ops to remove the consumed elements
                                Ops.resize(I + 2);
                            }
                        }
                    }
                }

                PlannedOperand& Right = Ops[I + 1];

                // Find the operator
                auto OpMatch = findOperator(TypeExpr->Name, Left.ResultType, Right.ResultType);

                // If the direct match fails and the right operand looks like a prefix operator,
                // try collapsing it first (e.g., in "*p0 + *p1", treat the second "*" as prefix)
                if (!OpMatch) {
                    if (auto* RightType = std::get_if<PlannedType>(&Right.Expr)) {
                        if (isOperatorName(RightType->Name) && I + 2 < Ops.size()) {
                            // Right is an operator symbol - try to collapse it with next operand
                            std::vector<PlannedOperand> PrefixOps;
                            PrefixOps.push_back(std::move(Right));
                            PrefixOps.push_back(std::move(Ops[I + 2]));

                            auto CollapsedRight = collapseOperandSequence(std::move(PrefixOps));
                            if (CollapsedRight) {
                                // Prefix collapse succeeded - retry findOperator
                                Right = std::move(*CollapsedRight);
                                RightConsumed = 2;
                                OpMatch = findOperator(TypeExpr->Name, Left.ResultType, Right.ResultType);
                            } else {
                                // Prefix collapse failed - propagate error
                                return CollapsedRight.takeError();
                            }
                        }
                    }
                }

                if (OpMatch) {
                    // Create a PlannedCall
                    PlannedCall Call;
                    Call.Loc = Ops[I].Loc;
                    Call.Name = TypeExpr->Name;
                    Call.MangledName = OpMatch->MangledName;
                    Call.IsIntrinsic = OpMatch->IsIntrinsic;
                    Call.IsOperator = true;

                    // Store arguments (make copies for result type calculation)
                    PlannedType LeftType = Left.ResultType;
                    PlannedType RightType = Right.ResultType;
                    Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                    Call.Args->push_back(std::move(Left));
                    Call.Args->push_back(std::move(Right));

                    // Get result type from operator
                    if (OpMatch->Op && OpMatch->Op->Returns) {
                        // Explicit operator with return type
                        auto RetType = resolveType(*OpMatch->Op->Returns, Call.Loc);
                        if (RetType) {
                            Call.ResultType = std::move(*RetType);
                        }
                    } else if (!OpMatch->ResultType.Name.empty()) {
                        // Intrinsic operator with explicit result type (e.g., pointer arithmetic)
                        Call.ResultType = OpMatch->ResultType;
                    } else if (OpMatch->IsIntrinsic) {
                        // Intrinsic operator: determine result type from operation
                        static const std::set<std::string> ComparisonOps = {"=", "<>", "<", ">", "<=", ">="};
                        if (ComparisonOps.count(TypeExpr->Name)) {
                            Call.ResultType.Name = "bool";
                            Call.ResultType.MangledName = "bool";
                        } else {
                            // Arithmetic/bitwise: result is same type as operands
                            Call.ResultType = LeftType;
                        }
                    }

                    // Create new left operand with the call
                    PlannedOperand NewLeft;
                    NewLeft.Loc = Call.Loc;
                    NewLeft.ResultType = Call.ResultType;
                    NewLeft.Expr = std::move(Call);

                    Left = std::move(NewLeft);
                    I += 1 + RightConsumed;  // Skip operator and right operand(s)
                    continue;
                } else {
                    // Operator not found, skip
                    I++;
                    continue;
                }
            }
        }

        // Handle 'is' expression: value is VariantType
        if (auto* IsExpr = std::get_if<PlannedIs>(&Ops[I].Expr)) {
            // The 'is' expression tests if the left operand (union value) is a specific variant
            PlannedIs NewIs = *IsExpr;
            NewIs.Value = std::make_shared<PlannedOperand>(std::move(Left));
            NewIs.UnionName = NewIs.Value->ResultType.Name;

            // Look up the union to get the variant tag
            auto UnionIt = InstantiatedUnions.find(NewIs.UnionName);
            if (UnionIt != InstantiatedUnions.end()) {
                const auto &Union = UnionIt->second;
                // Find the variant matching the TestType name
                std::string VariantName = NewIs.TestType.Name;
                size_t dotPos = VariantName.rfind('.');
                if (dotPos != std::string::npos) {
                    VariantName = VariantName.substr(dotPos + 1);
                }

                for (const auto &Var : Union.Variants) {
                    if (Var.Name == VariantName) {
                        NewIs.VariantTag = Var.Tag;
                        break;
                    }
                }
            }

            // Create new left operand with the is expression
            PlannedOperand NewLeft;
            NewLeft.Loc = Ops[I].Loc;
            NewLeft.ResultType.Name = "bool";
            NewLeft.ResultType.MangledName = "bool";
            NewLeft.Expr = std::move(NewIs);

            Left = std::move(NewLeft);
            I++;
            continue;
        }

        // Handle 'as' expression: value as TargetType
        if (auto* AsExpr = std::get_if<PlannedAs>(&Ops[I].Expr)) {
            // The 'as' expression casts the left operand to the target type
            PlannedAs NewAs = *AsExpr;
            NewAs.Value = std::make_shared<PlannedOperand>(std::move(Left));

            // Create new left operand with the as expression
            PlannedOperand NewLeft;
            NewLeft.Loc = Ops[I].Loc;
            NewLeft.ResultType = NewAs.TargetType;  // Result type is the cast target
            NewLeft.Expr = std::move(NewAs);

            Left = std::move(NewLeft);
            I++;
            continue;
        }

        // Non-operator: use as new left
        Left = std::move(Ops[I]);
        I++;
    }

    return Left;
}

// ============================================================================
// Member Access Resolution
// ============================================================================

llvm::Expected<PlannedType> Planner::resolveMemberAccess(
    const PlannedType &BaseType,
    const std::vector<std::string> &Members,
    Span Loc) {

    if (Members.empty()) {
        return BaseType;
    }

    PlannedType Current = BaseType;

    for (const auto& MemberName : Members) {
        // Handle pointer types by dereferencing to inner type (auto-deref)
        PlannedType LookupType = Current.getInnerTypeIfPointerLike();

        // Extract base name from type
        // For generic instantiations: "Box.int" -> "Box" (first part before generic args)
        // For qualified names: "scaly.containers.String" -> "String" (last component)
        std::string BaseName = LookupType.Name;
        const Concept* Conc = nullptr;

        // If we have generic arguments populated, this is a generic instantiation
        // So the base type is the first component (e.g., "Box.int" -> "Box")
        if (!LookupType.Generics.empty()) {
            size_t DotPos = BaseName.find('.');
            if (DotPos != std::string::npos) {
                BaseName = BaseName.substr(0, DotPos);
            }
            Conc = lookupConcept(BaseName);
        }

        // If not found or no generics, try the full name first
        if (!Conc) {
            Conc = lookupConcept(LookupType.Name);
        }

        // If not found, try the last component (for qualified names like scaly.containers.String)
        if (!Conc) {
            size_t LastDotPos = LookupType.Name.rfind('.');
            if (LastDotPos != std::string::npos) {
                std::string LastComponent = LookupType.Name.substr(LastDotPos + 1);
                Conc = lookupConcept(LastComponent);
            }
        }

        // If still not found and we have a qualified name, try loading the package on demand
        // This handles cases like "scaly.memory.Page" during generic instantiation
        if (!Conc && LookupType.Name.find('.') != std::string::npos) {
            // Parse the qualified name: "scaly.memory.Page" -> package "scaly", path ["memory"], concept "Page"
            std::vector<std::string> Parts;
            std::string Remaining = LookupType.Name;
            size_t Pos;
            while ((Pos = Remaining.find('.')) != std::string::npos) {
                Parts.push_back(Remaining.substr(0, Pos));
                Remaining = Remaining.substr(Pos + 1);
            }
            Parts.push_back(Remaining);  // Last component (concept name)

            if (Parts.size() >= 2) {
                // Try to load the package
                const Module* PkgMod = loadPackageOnDemand(Parts[0]);
                if (PkgMod) {
                    // Find the namespace concept in the package
                    const Namespace* CurrentNS = nullptr;
                    for (const auto& Member : PkgMod->Members) {
                        if (auto* PkgConc = std::get_if<Concept>(&Member)) {
                            if (PkgConc->Name == Parts[0]) {
                                if (auto* NS = std::get_if<Namespace>(&PkgConc->Def)) {
                                    CurrentNS = NS;
                                    break;
                                }
                            }
                        }
                    }

                    // Navigate through the path to find the concept
                    const Module* CurrentMod = nullptr;
                    bool Found = CurrentNS != nullptr;
                    for (size_t I = 1; I < Parts.size() - 1 && Found; ++I) {
                        Found = false;
                        if (CurrentNS) {
                            for (const auto& SubMod : CurrentNS->Modules) {
                                if (SubMod.Name == Parts[I]) {
                                    CurrentMod = &SubMod;
                                    CurrentNS = nullptr;
                                    Found = true;
                                    break;
                                }
                            }
                        }
                        if (!Found && CurrentMod) {
                            for (const auto& SubMod : CurrentMod->Modules) {
                                if (SubMod.Name == Parts[I]) {
                                    CurrentMod = &SubMod;
                                    Found = true;
                                    break;
                                }
                            }
                        }
                        // After finding a module, check if it contains a namespace with the same name
                        // This handles: scaly (namespace) → memory (module) → memory (namespace inside module) → Page (module)
                        if (Found && CurrentMod && !CurrentNS) {
                            for (const auto& ModMember : CurrentMod->Members) {
                                if (auto* SubConc = std::get_if<Concept>(&ModMember)) {
                                    if (SubConc->Name == Parts[I]) {
                                        if (auto* SubNS = std::get_if<Namespace>(&SubConc->Def)) {
                                            CurrentNS = SubNS;
                                            CurrentMod = nullptr;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // After navigation, if we ended on a namespace, look for the concept's module
                    if (Found && CurrentNS && !CurrentMod) {
                        std::string ConceptName = Parts.back();
                        for (const auto& SubMod : CurrentNS->Modules) {
                            if (SubMod.Name == ConceptName) {
                                CurrentMod = &SubMod;
                                break;
                            }
                        }
                    }

                    // Look for the concept in the final module
                    if (Found && CurrentMod) {
                        std::string ConceptName = Parts.back();
                        for (const auto& ModMember : CurrentMod->Members) {
                            if (auto* SubConc = std::get_if<Concept>(&ModMember)) {
                                if (SubConc->Name == ConceptName) {
                                    Conc = SubConc;
                                    Concepts[ConceptName] = Conc;
                                    // Add module to accessed list for sibling lookups
                                    if (std::find(AccessedPackageModules.begin(),
                                                  AccessedPackageModules.end(),
                                                  CurrentMod) == AccessedPackageModules.end()) {
                                        AccessedPackageModules.push_back(CurrentMod);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!Conc) {
            return makePlannerNotImplementedError(File, Loc,
                "cannot access member '" + MemberName + "' on type " +
                 LookupType.Name);
        }

        // Check if it's a structure with properties
        if (auto* Struct = std::get_if<Structure>(&Conc->Def)) {
            bool Found = false;
            for (const auto& Prop : Struct->Properties) {
                if (Prop.Name == MemberName) {
                    if (!Prop.PropType) {
                        return makePlannerNotImplementedError(File, Loc,
                            "property without type");
                    }
                    auto Resolved = resolveType(*Prop.PropType, Loc);
                    if (!Resolved) {
                        return Resolved.takeError();
                    }
                    Current = std::move(*Resolved);
                    Found = true;
                    break;
                }
            }
            if (Found) continue;

            // Check methods
            for (const auto& Member : Struct->Members) {
                if (auto* Func = std::get_if<Function>(&Member)) {
                    if (Func->Name == MemberName) {
                        // Method reference - for now, return void
                        // (proper handling would return a function type)
                        PlannedType MethodType;
                        MethodType.Loc = Loc;
                        MethodType.Name = "function";
                        MethodType.MangledName = "Pv";  // pointer to void
                        Current = MethodType;
                        Found = true;
                        break;
                    }
                }
            }
            if (Found) continue;
        }

        // Check if it's a union with variants
        if (auto* Un = std::get_if<Union>(&Conc->Def)) {
            for (const auto& Var : Un->Variants) {
                if (Var.Name == MemberName) {
                    if (Var.VarType) {
                        auto Resolved = resolveType(*Var.VarType, Loc);
                        if (!Resolved) {
                            return Resolved.takeError();
                        }
                        Current = std::move(*Resolved);
                    } else {
                        // Unit variant - return the union type itself
                        Current = BaseType;
                    }
                    break;
                }
            }
            continue;
        }

        // Member not found
        return makePlannerNotImplementedError(File, Loc,
            "member '" + MemberName + "' not found in type " +
             Current.Name);
    }

    return Current;
}

llvm::Expected<std::vector<PlannedMemberAccess>> Planner::resolveMemberAccessChain(
    const PlannedType &BaseType,
    const std::vector<std::string> &Members,
    Span Loc) {

    std::vector<PlannedMemberAccess> Result;
    if (Members.empty()) {
        return Result;
    }

    PlannedType Current = BaseType;

    for (const auto& MemberName : Members) {
        // Handle pointer types by dereferencing to inner type (auto-deref)
        PlannedType LookupType = Current.getInnerTypeIfPointerLike();

        // Extract base name from type
        // For generic instantiations: "Box.int" -> "Box" (first part before generic args)
        // For qualified names: "scaly.containers.String" -> "String" (last component)
        std::string BaseName = LookupType.Name;
        const Concept* Conc = nullptr;

        // If we have generic arguments populated, this is a generic instantiation
        // So the base type is the first component (e.g., "Box.int" -> "Box")
        if (!LookupType.Generics.empty()) {
            size_t DotPos = BaseName.find('.');
            if (DotPos != std::string::npos) {
                BaseName = BaseName.substr(0, DotPos);
            }
            Conc = lookupConcept(BaseName);
        }

        // If not found or no generics, try the full name first
        if (!Conc) {
            Conc = lookupConcept(LookupType.Name);
        }

        // If not found, try the last component (for qualified names like scaly.containers.String)
        if (!Conc) {
            size_t LastDotPos = LookupType.Name.rfind('.');
            if (LastDotPos != std::string::npos) {
                std::string LastComponent = LookupType.Name.substr(LastDotPos + 1);
                Conc = lookupConcept(LastComponent);
            }
        }

        // If still not found and we have a qualified name, try loading the package on demand
        // This handles cases like "scaly.memory.Page" during generic instantiation
        if (!Conc && LookupType.Name.find('.') != std::string::npos) {
            // Parse the qualified name: "scaly.memory.Page" -> package "scaly", path ["memory"], concept "Page"
            std::vector<std::string> Parts;
            std::string Remaining = LookupType.Name;
            size_t Pos;
            while ((Pos = Remaining.find('.')) != std::string::npos) {
                Parts.push_back(Remaining.substr(0, Pos));
                Remaining = Remaining.substr(Pos + 1);
            }
            Parts.push_back(Remaining);  // Last component (concept name)

            if (Parts.size() >= 2) {
                // Try to load the package
                const Module* PkgMod = loadPackageOnDemand(Parts[0]);
                if (PkgMod) {
                    // Find the namespace concept in the package
                    const Namespace* CurrentNS = nullptr;
                    for (const auto& Member : PkgMod->Members) {
                        if (auto* PkgConc = std::get_if<Concept>(&Member)) {
                            if (PkgConc->Name == Parts[0]) {
                                if (auto* NS = std::get_if<Namespace>(&PkgConc->Def)) {
                                    CurrentNS = NS;
                                    break;
                                }
                            }
                        }
                    }

                    // Navigate through the path to find the concept
                    const Module* CurrentMod = nullptr;
                    bool Found = CurrentNS != nullptr;
                    for (size_t I = 1; I < Parts.size() - 1 && Found; ++I) {
                        Found = false;
                        if (CurrentNS) {
                            for (const auto& SubMod : CurrentNS->Modules) {
                                if (SubMod.Name == Parts[I]) {
                                    CurrentMod = &SubMod;
                                    CurrentNS = nullptr;
                                    Found = true;
                                    break;
                                }
                            }
                        }
                        if (!Found && CurrentMod) {
                            for (const auto& SubMod : CurrentMod->Modules) {
                                if (SubMod.Name == Parts[I]) {
                                    CurrentMod = &SubMod;
                                    Found = true;
                                    break;
                                }
                            }
                        }
                        // After finding a module, check if it contains a namespace with the same name
                        // This handles: scaly (namespace) → memory (module) → memory (namespace inside module) → Page (module)
                        if (Found && CurrentMod && !CurrentNS) {
                            for (const auto& ModMember : CurrentMod->Members) {
                                if (auto* SubConc = std::get_if<Concept>(&ModMember)) {
                                    if (SubConc->Name == Parts[I]) {
                                        if (auto* SubNS = std::get_if<Namespace>(&SubConc->Def)) {
                                            CurrentNS = SubNS;
                                            CurrentMod = nullptr;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // After navigation, if we ended on a namespace, look for the concept's module
                    if (Found && CurrentNS && !CurrentMod) {
                        std::string ConceptName = Parts.back();
                        for (const auto& SubMod : CurrentNS->Modules) {
                            if (SubMod.Name == ConceptName) {
                                CurrentMod = &SubMod;
                                break;
                            }
                        }
                    }

                    // Look for the concept in the final module
                    if (Found && CurrentMod) {
                        std::string ConceptName = Parts.back();
                        for (const auto& ModMember : CurrentMod->Members) {
                            if (auto* SubConc = std::get_if<Concept>(&ModMember)) {
                                if (SubConc->Name == ConceptName) {
                                    Conc = SubConc;
                                    Concepts[ConceptName] = Conc;
                                    // Add module to accessed list for sibling lookups
                                    if (std::find(AccessedPackageModules.begin(),
                                                  AccessedPackageModules.end(),
                                                  CurrentMod) == AccessedPackageModules.end()) {
                                        AccessedPackageModules.push_back(CurrentMod);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!Conc) {
            return makePlannerNotImplementedError(File, Loc,
                "cannot access member '" + MemberName + "' on type " +
                 LookupType.Name);
        }

        // Check if it's a structure with properties
        if (auto* Struct = std::get_if<Structure>(&Conc->Def)) {
            // Set up type substitutions if this is a generic type
            // e.g., for Box.int, we need T -> int when resolving property types
            std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
            if (!Conc->Parameters.empty() && !LookupType.Generics.empty()) {
                for (size_t I = 0; I < Conc->Parameters.size() && I < LookupType.Generics.size(); ++I) {
                    TypeSubstitutions[Conc->Parameters[I].Name] = LookupType.Generics[I];
                }
            }

            bool Found = false;
            size_t FieldIndex = 0;
            for (const auto& Prop : Struct->Properties) {
                if (Prop.Name == MemberName) {
                    if (!Prop.PropType) {
                        TypeSubstitutions = OldSubst;
                        return makePlannerNotImplementedError(File, Loc,
                            "property without type");
                    }
                    auto Resolved = resolveType(*Prop.PropType, Loc);
                    if (!Resolved) {
                        TypeSubstitutions = OldSubst;
                        return Resolved.takeError();
                    }

                    PlannedMemberAccess Access;
                    Access.Name = MemberName;
                    Access.FieldIndex = FieldIndex;
                    Access.ParentType = LookupType;  // Use dereferenced type for auto-deref
                    Access.ResultType = std::move(*Resolved);
                    Result.push_back(std::move(Access));

                    Current = Result.back().ResultType;
                    Found = true;
                    break;
                }
                FieldIndex++;
            }
            TypeSubstitutions = OldSubst;
            if (Found) continue;

            // Check methods
            for (const auto& Member : Struct->Members) {
                if (auto* Func = std::get_if<Function>(&Member)) {
                    if (Func->Name == MemberName) {
                        // Method reference - return a "method" type
                        // The actual call will be resolved later
                        PlannedMemberAccess Access;
                        Access.Name = MemberName;
                        Access.FieldIndex = SIZE_MAX;  // Special marker for methods
                        Access.IsMethod = true;
                        Access.ParentType = LookupType;  // Use dereferenced type for auto-deref
                        Access.ResultType.Name = "function";
                        Access.ResultType.MangledName = "Pv";
                        Result.push_back(std::move(Access));
                        Current = Result.back().ResultType;
                        Found = true;
                        break;
                    }
                }
            }
            if (Found) continue;
        }

        // Check if it's a union - handle .value access
        if (auto* Un = std::get_if<Union>(&Conc->Def)) {
            if (MemberName == "value") {
                // .value on a union extracts the data from the current variant
                // We need to know the variant type - for now, use the first variant with a type
                // In practice, this is typically used after constructing a specific variant
                // Set up type substitutions if this is a generic type
                std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
                if (!Conc->Parameters.empty() && !LookupType.Generics.empty()) {
                    for (size_t I = 0; I < Conc->Parameters.size() && I < LookupType.Generics.size(); ++I) {
                        TypeSubstitutions[Conc->Parameters[I].Name] = LookupType.Generics[I];
                    }
                }

                // Find the first variant with a type (non-unit variant)
                PlannedType ValueType;
                for (const auto& Var : Un->Variants) {
                    if (Var.VarType) {
                        auto Resolved = resolveType(*Var.VarType, Loc);
                        if (Resolved) {
                            ValueType = std::move(*Resolved);
                            break;
                        }
                    }
                }
                TypeSubstitutions = OldSubst;

                if (ValueType.Name.empty()) {
                    return makePlannerNotImplementedError(File, Loc,
                        "union has no variant with a value type");
                }

                PlannedMemberAccess Access;
                Access.Name = "value";
                Access.FieldIndex = 1;  // Data is at field index 1 in union layout
                Access.IsUnionValue = true;
                Access.ParentType = LookupType;
                Access.ResultType = std::move(ValueType);
                Result.push_back(std::move(Access));

                Current = Result.back().ResultType;
                continue;
            }
        }

        // Member not found
        return makePlannerNotImplementedError(File, Loc,
            "member '" + MemberName + "' not found in type " +
             Current.Name);
    }

    return Result;
}

// Overload for Type-based member access - extracts names from Types
llvm::Expected<std::vector<PlannedMemberAccess>> Planner::resolveMemberAccessChain(
    const PlannedType &BaseType,
    const std::vector<Type> &Members,
    Span Loc) {
    // Convert vector<Type> to vector<string> by extracting names
    std::vector<std::string> Names;
    for (const auto& M : Members) {
        // Flatten the Type's name path
        for (const auto& N : M.Name) {
            Names.push_back(N);
        }
    }
    return resolveMemberAccessChain(BaseType, Names, Loc);
}

// ============================================================================
// Type Inference
// ============================================================================

PlannedType Planner::inferConstantType(const Constant &C) {
    PlannedType Result;
    Result.Life = UnspecifiedLifetime{};

    std::visit([&Result](const auto &Val) {
        using T = std::decay_t<decltype(Val)>;

        if constexpr (std::is_same_v<T, BooleanConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "bool";
            Result.MangledName = "b";
        }
        else if constexpr (std::is_same_v<T, IntegerConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "int";
            Result.MangledName = "i";
        }
        else if constexpr (std::is_same_v<T, HexConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "u64";
            Result.MangledName = "u64";
        }
        else if constexpr (std::is_same_v<T, FloatingPointConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "f64";
            Result.MangledName = "f64";
        }
        else if constexpr (std::is_same_v<T, StringConstant>) {
            // String literals are typed as pointer[i8] (C-style null-terminated strings)
            Result.Loc = Val.Loc;
            Result.Name = "pointer";
            PlannedType I8Type;
            I8Type.Name = "i8";
            I8Type.MangledName = "a";  // i8 in Itanium ABI is 'a' (signed char)
            Result.Generics.push_back(I8Type);
            Result.MangledName = "Pa";  // pointer[i8]
        }
        else if constexpr (std::is_same_v<T, CharacterConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "char";
            Result.MangledName = "c";
        }
        else if constexpr (std::is_same_v<T, FragmentConstant>) {
            // Fragments are also C-style strings
            Result.Loc = Val.Loc;
            Result.Name = "pointer";
            PlannedType I8Type;
            I8Type.Name = "i8";
            I8Type.MangledName = "a";
            Result.Generics.push_back(I8Type);
            Result.MangledName = "Pa";
        }
        else if constexpr (std::is_same_v<T, NullConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "pointer";  // Generic null pointer
            // null can be any pointer type - use void* representation
            // The constraint solver will unify this with the expected pointer type
            PlannedType VoidType;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            Result.Generics.push_back(VoidType);
            Result.MangledName = "Pv";  // pointer to void
        }
    }, C);

    return Result;
}

llvm::Expected<PlannedType> Planner::resolveNameOrVariable(const Type &T) {
    // If it's a simple name (single element, no generics), check if it's a variable
    if (T.Name.size() == 1 && (!T.Generics || T.Generics->empty())) {
        const std::string &Name = T.Name[0];

        // First check local variables
        auto LocalType = lookupLocal(Name);
        if (LocalType) {
            // It's a local variable - return its type
            PlannedType Result = *LocalType;
            Result.Loc = T.Loc;  // Use the reference location
            // Note: 'this' is stored as pointer[T] in scope - keep it that way
            // Type compatibility will handle pointer[T] -> T conversion when needed
            return Result;
        }

        // Check if it's a structure property (when inside a method via 'this')
        if (CurrentStructureProperties) {
            // Check if 'this' is in scope
            auto ThisType = lookupLocal("this");
            // Extract base name from CurrentStructureName for generic types
            std::string BaseStructName = CurrentStructureName;
            size_t DotPos = BaseStructName.find('.');
            if (DotPos != std::string::npos) {
                BaseStructName = BaseStructName.substr(0, DotPos);
            }
            // 'this' is now pointer[T], so check the inner type
            std::string ThisTypeName = ThisType ? ThisType->Name : "";
            if (ThisType && ThisType->Name == "pointer" && !ThisType->Generics.empty()) {
                ThisTypeName = ThisType->Generics[0].Name;
            }
            if (ThisType && (ThisTypeName == CurrentStructureName || ThisTypeName == BaseStructName)) {
                // Look for property with this name
                for (const auto &Prop : *CurrentStructureProperties) {
                    if (Prop.Name == Name) {
                        PlannedType Result = Prop.PropType;
                        Result.Loc = T.Loc;
                        return Result;
                    }
                }
            }
        }
    }

    // Not a variable or property, resolve as a type
    return resolveType(T, T.Loc);
}

llvm::Expected<PlannedType> Planner::inferExpressionType(const PlannedExpression &Expr) {
    return std::visit([this](const auto &E) -> llvm::Expected<PlannedType> {
        using T = std::decay_t<decltype(E)>;

        if constexpr (std::is_same_v<T, PlannedConstant>) {
            return inferConstantType(E);
        }
        else if constexpr (std::is_same_v<T, PlannedType>) {
            // Expression is a type - the "value" is the type itself
            // This happens for type references or constructor calls
            return E;
        }
        else if constexpr (std::is_same_v<T, PlannedVariable>) {
            // Variable reference - return the variable's type
            return E.VariableType;
        }
        else if constexpr (std::is_same_v<T, PlannedTuple>) {
            // Return the pre-computed tuple type
            // For single anonymous components (grouping parens), this is the operation type
            // For multi-component tuples, this is a Tuple<T1, T2, ...> type
            return E.TupleType;
        }
        else if constexpr (std::is_same_v<T, PlannedMatrix>) {
            // TODO: Infer element type from matrix contents
            PlannedType ArrayType;
            ArrayType.Loc = E.Loc;
            ArrayType.Name = "Array";
            ArrayType.MangledName = "5Array";
            return ArrayType;
        }
        else if constexpr (std::is_same_v<T, PlannedBlock>) {
            // Block type is the type of its last statement
            if (E.Statements.empty()) {
                PlannedType VoidType;
                VoidType.Loc = E.Loc;
                VoidType.Name = "void";
                VoidType.MangledName = "v";
                return VoidType;
            }

            // Get the last statement's type
            const auto& Last = E.Statements.back();
            return std::visit([this, &E](const auto& Stmt) -> llvm::Expected<PlannedType> {
                using S = std::decay_t<decltype(Stmt)>;

                if constexpr (std::is_same_v<S, PlannedAction>) {
                    // Action type is the computed result type of the operation
                    return Stmt.ResultType;
                }
                else if constexpr (std::is_same_v<S, PlannedReturn>) {
                    if (!Stmt.Result.empty()) {
                        return Stmt.Result.back().ResultType;
                    }
                    PlannedType VoidType;
                    VoidType.Loc = Stmt.Loc;
                    VoidType.Name = "void";
                    VoidType.MangledName = "v";
                    return VoidType;
                }
                else if constexpr (std::is_same_v<S, PlannedBreak>) {
                    if (!Stmt.Result.empty()) {
                        return Stmt.Result.back().ResultType;
                    }
                    PlannedType VoidType;
                    VoidType.Loc = Stmt.Loc;
                    VoidType.Name = "void";
                    VoidType.MangledName = "v";
                    return VoidType;
                }
                else {
                    // Binding, Continue, Throw - block evaluates to void
                    PlannedType VoidType;
                    VoidType.Loc = E.Loc;
                    VoidType.Name = "void";
                    VoidType.MangledName = "v";
                    return VoidType;
                }
            }, Last);
        }
        else if constexpr (std::is_same_v<T, PlannedIf>) {
            // If type is the type of consequent (assumes both branches same type)
            if (E.Consequent) {
                return std::visit([&E](const auto& Stmt) -> llvm::Expected<PlannedType> {
                    using S = std::decay_t<decltype(Stmt)>;

                    if constexpr (std::is_same_v<S, PlannedAction>) {
                        return Stmt.ResultType;
                    }
                    PlannedType VoidType;
                    VoidType.Loc = E.Loc;
                    VoidType.Name = "void";
                    VoidType.MangledName = "v";
                    return VoidType;
                }, *E.Consequent);
            }
            PlannedType VoidType;
            VoidType.Loc = E.Loc;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            return VoidType;
        }
        else if constexpr (std::is_same_v<T, PlannedMatch>) {
            // Match type from first branch
            if (!E.Branches.empty() && E.Branches[0].Consequent) {
                return std::visit([&E](const auto& Stmt) -> llvm::Expected<PlannedType> {
                    using S = std::decay_t<decltype(Stmt)>;

                    if constexpr (std::is_same_v<S, PlannedAction>) {
                        return Stmt.ResultType;
                    }
                    PlannedType VoidType;
                    VoidType.Loc = E.Loc;
                    VoidType.Name = "void";
                    VoidType.MangledName = "v";
                    return VoidType;
                }, *E.Branches[0].Consequent);
            }
            PlannedType VoidType;
            VoidType.Loc = E.Loc;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            return VoidType;
        }
        else if constexpr (std::is_same_v<T, PlannedChoose>) {
            // Choose type from first case
            if (!E.Cases.empty() && E.Cases[0].Consequent) {
                return std::visit([&E](const auto& Stmt) -> llvm::Expected<PlannedType> {
                    using S = std::decay_t<decltype(Stmt)>;

                    if constexpr (std::is_same_v<S, PlannedAction>) {
                        return Stmt.ResultType;
                    }
                    PlannedType VoidType;
                    VoidType.Loc = E.Loc;
                    VoidType.Name = "void";
                    VoidType.MangledName = "v";
                    return VoidType;
                }, *E.Cases[0].Consequent);
            }
            PlannedType VoidType;
            VoidType.Loc = E.Loc;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            return VoidType;
        }
        else if constexpr (std::is_same_v<T, PlannedFor> ||
                          std::is_same_v<T, PlannedWhile>) {
            // Loops are void
            PlannedType VoidType;
            VoidType.Loc = E.Loc;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            return VoidType;
        }
        else if constexpr (std::is_same_v<T, PlannedTry>) {
            // Try type is the type of the binding value
            if (E.Cond.BindingItem.ItemType) {
                return *E.Cond.BindingItem.ItemType;
            }
            PlannedType VoidType;
            VoidType.Loc = E.Loc;
            VoidType.Name = "void";
            VoidType.MangledName = "v";
            return VoidType;
        }
        else if constexpr (std::is_same_v<T, PlannedSizeOf>) {
            // sizeof always returns size (usize/uint)
            PlannedType SizeType;
            SizeType.Loc = E.Loc;
            SizeType.Name = "size";
            SizeType.MangledName = "m";  // size_t
            return SizeType;
        }
        else if constexpr (std::is_same_v<T, PlannedAlignOf>) {
            // alignof always returns size (usize/uint)
            PlannedType SizeType;
            SizeType.Loc = E.Loc;
            SizeType.Name = "size";
            SizeType.MangledName = "m";  // size_t
            return SizeType;
        }
        else if constexpr (std::is_same_v<T, PlannedIs>) {
            // 'is' always returns bool
            PlannedType BoolType;
            BoolType.Loc = E.Loc;
            BoolType.Name = "bool";
            BoolType.MangledName = "b";
            return BoolType;
        }
        else if constexpr (std::is_same_v<T, PlannedAs>) {
            // 'as' returns the target type
            return E.TargetType;
        }
        else {
            // Fallback
            PlannedType UnknownType;
            UnknownType.Name = "void";
            UnknownType.MangledName = "v";
            return UnknownType;
        }
    }, Expr);
}

// ============================================================================
// Cross-Module Concept Lookup
// ============================================================================

const Concept* Planner::lookupConcept(llvm::StringRef Name) {
    // 1. Check flat cache first (most efficient)
    auto CacheIt = Concepts.find(Name.str());
    if (CacheIt != Concepts.end()) {
        return CacheIt->second;
    }

    // 1a. If we're inside a namespace (define block), search sibling modules
    // These are stored in CurrentNamespaceModules during planNamespace
    // This allows scaly.test() to find io when calling io.test()
    if (!CurrentNamespaceModules.empty()) {
        for (const Module* Mod : CurrentNamespaceModules) {
            if (Mod->Name == Name) {
                // Found a module with matching name - look for a concept with same name
                for (const auto& Member : Mod->Members) {
                    if (auto* Conc = std::get_if<Concept>(&Member)) {
                        if (Conc->Name == Name) {
                            Concepts[Name.str()] = Conc;
                            return Conc;
                        }
                    }
                }
            }
        }
    }

    // 1b. Search modules we've previously accessed from on-demand packages
    // This allows finding sibling concepts like PAGE_SIZE when Page is used
    for (const Module* AccessedMod : AccessedPackageModules) {
        for (const auto& Member : AccessedMod->Members) {
            if (auto* Conc = std::get_if<Concept>(&Member)) {
                if (Conc->Name == Name) {
                    Concepts[Name.str()] = Conc;
                    return Conc;
                }
                // Also search inside namespace definitions (e.g., define memory { module PageList })
                if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                    for (const auto& NsMod : NS->Modules) {
                        if (NsMod.Name == Name) {
                            for (const auto& NsMember : NsMod.Members) {
                                if (auto* NsConc = std::get_if<Concept>(&NsMember)) {
                                    if (NsConc->Name == Name) {
                                        Concepts[Name.str()] = NsConc;
                                        return NsConc;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // Also search sub-modules (e.g., memory has "module PageList" as a sub-module)
        for (const auto& SubMod : AccessedMod->Modules) {
            if (SubMod.Name == Name) {
                // Found a sub-module with matching name - look for concept with same name inside
                for (const auto& SubMember : SubMod.Members) {
                    if (auto* SubConc = std::get_if<Concept>(&SubMember)) {
                        if (SubConc->Name == Name) {
                            Concepts[Name.str()] = SubConc;
                            return SubConc;
                        }
                    }
                }
            }
        }
    }

    // 2. Search current module stack (innermost to outermost)
    for (auto It = ModuleStack.rbegin(); It != ModuleStack.rend(); ++It) {
        const Module* Mod = *It;

        // Check this module's members for a matching concept
        for (const auto& Member : Mod->Members) {
            if (auto* Conc = std::get_if<Concept>(&Member)) {
                if (Conc->Name == Name) {
                    Concepts[Name.str()] = Conc;
                    return Conc;
                }

                // Also check if this concept's definition contains sub-modules
                // with the concept we're looking for (e.g., define containers { module Vector })
                if (auto* Struct = std::get_if<Structure>(&Conc->Def)) {
                    for (const auto& SubMod : Struct->Modules) {
                        for (const auto& SubMember : SubMod.Members) {
                            if (auto* SubConc = std::get_if<Concept>(&SubMember)) {
                                if (SubConc->Name == Name) {
                                    Concepts[Name.str()] = SubConc;
                                    return SubConc;
                                }
                            }
                        }
                    }
                } else if (auto* Ns = std::get_if<Namespace>(&Conc->Def)) {
                    for (const auto& SubMod : Ns->Modules) {
                        for (const auto& SubMember : SubMod.Members) {
                            if (auto* SubConc = std::get_if<Concept>(&SubMember)) {
                                if (SubConc->Name == Name) {
                                    Concepts[Name.str()] = SubConc;
                                    return SubConc;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Check sub-modules for exported concepts
        for (const auto& SubMod : Mod->Modules) {
            // Sub-module name matches the concept name we're looking for
            if (SubMod.Name == Name) {
                // Look for a concept with same name as module (common pattern)
                for (const auto& Member : SubMod.Members) {
                    if (auto* Conc = std::get_if<Concept>(&Member)) {
                        if (Conc->Name == Name) {
                            Concepts[Name.str()] = Conc;
                            return Conc;
                        }
                    }
                }
            }

            // Also check if the sub-module exports a concept we're looking for
            if (!SubMod.Private) {
                for (const auto& Member : SubMod.Members) {
                    if (auto* Conc = std::get_if<Concept>(&Member)) {
                        if (Conc->Name == Name) {
                            Concepts[Name.str()] = Conc;
                            return Conc;
                        }
                    }
                }
            }
        }
    }

    // 2b. Check Use statements in module stack
    // If there's a "use foo.bar.Baz" and we're looking for "Baz", resolve the full path
    for (auto It = ModuleStack.rbegin(); It != ModuleStack.rend(); ++It) {
        const Module* Mod = *It;
        for (const auto& Use : Mod->Uses) {
            // Check if the Use path ends with the name we're looking for
            if (!Use.Path.empty() && Use.Path.back() == Name) {
                // First, check if the path starts with a namespace defined in our module stack
                // This handles intra-package references like "use scaly.memory.Page" from within scaly
                if (Use.Path.size() >= 2) {
                    for (auto StackIt = ModuleStack.rbegin(); StackIt != ModuleStack.rend(); ++StackIt) {
                        const Module* ParentMod = *StackIt;
                        for (const auto& Member : ParentMod->Members) {
                            if (auto* Conc = std::get_if<Concept>(&Member)) {
                                if (Conc->Name == Use.Path[0]) {
                                    // Found a namespace matching the first path element
                                    // Navigate through the namespace's modules
                                    const Namespace* CurrentNS = nullptr;
                                    if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                                        CurrentNS = NS;
                                    }
                                    const Module* CurrentMod = nullptr;
                                    bool Found = CurrentNS != nullptr;

                                    for (size_t I = 1; I < Use.Path.size() - 1 && Found; ++I) {
                                        Found = false;
                                        if (CurrentNS) {
                                            for (const auto& SubMod : CurrentNS->Modules) {
                                                if (SubMod.Name == Use.Path[I]) {
                                                    // Check if this is a stub (no members) and needs loading
                                                    if (SubMod.Members.empty()) {
                                                        // Get the base path from the parent module's file
                                                        llvm::SmallString<256> BasePath(ParentMod->File);
                                                        llvm::sys::path::remove_filename(BasePath);
                                                        llvm::sys::path::append(BasePath, Use.Path[0]);

                                                        // Try to load the module on demand
                                                        if (const Module* Loaded = loadIntraPackageModule(BasePath.str(), SubMod.Name)) {
                                                            CurrentMod = Loaded;
                                                        } else {
                                                            CurrentMod = &SubMod;  // Fall back to stub
                                                        }
                                                    } else {
                                                        CurrentMod = &SubMod;
                                                    }
                                                    CurrentNS = nullptr;
                                                    Found = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (!Found && CurrentMod) {
                                            for (const auto& SubMod : CurrentMod->Modules) {
                                                if (SubMod.Name == Use.Path[I]) {
                                                    // Check if this is a stub that needs loading
                                                    if (SubMod.Members.empty() && !CurrentMod->File.empty()) {
                                                        llvm::SmallString<256> BasePath(CurrentMod->File);
                                                        llvm::sys::path::remove_filename(BasePath);
                                                        llvm::sys::path::append(BasePath, CurrentMod->Name);

                                                        if (const Module* Loaded = loadIntraPackageModule(BasePath.str(), SubMod.Name)) {
                                                            CurrentMod = Loaded;
                                                        } else {
                                                            CurrentMod = &SubMod;
                                                        }
                                                    } else {
                                                        CurrentMod = &SubMod;
                                                    }
                                                    Found = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }

                                    if (Found && CurrentMod) {
                                        // Look for the concept in the final module
                                        for (const auto& ModMember : CurrentMod->Members) {
                                            if (auto* SubConc = std::get_if<Concept>(&ModMember)) {
                                                if (SubConc->Name == Name) {
                                                    Concepts[Name.str()] = SubConc;
                                                    return SubConc;
                                                }
                                            }
                                        }
                                        // Also check sub-modules
                                        for (const auto& SubMod : CurrentMod->Modules) {
                                            if (SubMod.Name == Name) {
                                                for (const auto& SubMember : SubMod.Members) {
                                                    if (auto* SubConc = std::get_if<Concept>(&SubMember)) {
                                                        if (SubConc->Name == Name) {
                                                            Concepts[Name.str()] = SubConc;
                                                            return SubConc;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Try to resolve the full path by searching sibling programs
                // Start from the first element of the path
                for (const auto& Sibling : SiblingPrograms) {
                    // Check if first path element matches sibling's main module name
                    if (Use.Path.size() >= 2 && Sibling->MainModule.Name == Use.Path[0]) {
                        // Navigate through the path
                        const Module* CurrentMod = &Sibling->MainModule;
                        bool Found = true;
                        for (size_t I = 1; I < Use.Path.size() - 1 && Found; ++I) {
                            Found = false;
                            for (const auto& SubMod : CurrentMod->Modules) {
                                if (SubMod.Name == Use.Path[I]) {
                                    CurrentMod = &SubMod;
                                    Found = true;
                                    break;
                                }
                            }
                        }
                        if (Found) {
                            // Look for the concept in the final module
                            for (const auto& Member : CurrentMod->Members) {
                                if (auto* Conc = std::get_if<Concept>(&Member)) {
                                    if (Conc->Name == Name) {
                                        Concepts[Name.str()] = Conc;
                                        return Conc;
                                    }
                                }
                            }
                            // Also check sub-modules
                            for (const auto& SubMod : CurrentMod->Modules) {
                                if (SubMod.Name == Name) {
                                    for (const auto& Member : SubMod.Members) {
                                        if (auto* Conc = std::get_if<Concept>(&Member)) {
                                            if (Conc->Name == Name) {
                                                Concepts[Name.str()] = Conc;
                                                return Conc;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Also check LoadedPackages for packages loaded via "package" statements
                // or try to load the package on demand if not found
                if (Use.Path.size() >= 2) {
                    auto PkgIt = LoadedPackages.find(Use.Path[0]);

                    // Try to load the package on demand if not already loaded
                    if (PkgIt == LoadedPackages.end()) {
                        if (const Module* LoadedMod = loadPackageOnDemand(Use.Path[0])) {
                            PkgIt = LoadedPackages.find(Use.Path[0]);
                        }
                    }

                    if (PkgIt != LoadedPackages.end()) {
                        const Module* CurrentMod = PkgIt->second;
                        const Namespace* CurrentNS = nullptr;

                        // The package typically contains a `define <package_name> {...}` concept
                        for (const auto& Member : CurrentMod->Members) {
                            if (auto* Conc = std::get_if<Concept>(&Member)) {
                                if (Conc->Name == Use.Path[0]) {
                                    if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                                        CurrentNS = NS;
                                        break;
                                    }
                                }
                            }
                        }

                        // Navigate through the path (start from index 1 since 0 is the package name)
                        bool Found = true;
                        for (size_t I = 1; I < Use.Path.size() - 1 && Found; ++I) {
                            Found = false;
                            if (CurrentNS) {
                                for (const auto& SubMod : CurrentNS->Modules) {
                                    if (SubMod.Name == Use.Path[I]) {
                                        CurrentMod = &SubMod;
                                        CurrentNS = nullptr;
                                        Found = true;
                                        break;
                                    }
                                }
                            }
                            if (!Found && CurrentMod) {
                                for (const auto& SubMod : CurrentMod->Modules) {
                                    if (SubMod.Name == Use.Path[I]) {
                                        CurrentMod = &SubMod;
                                        Found = true;
                                        break;
                                    }
                                }
                            }
                        }

                        if (Found && CurrentMod) {
                            // Cache this module for sibling concept lookup
                            if (std::find(AccessedPackageModules.begin(),
                                          AccessedPackageModules.end(),
                                          CurrentMod) == AccessedPackageModules.end()) {
                                AccessedPackageModules.push_back(CurrentMod);
                            }

                            // First look directly in the module's members
                            for (const auto& Member : CurrentMod->Members) {
                                if (auto* Conc = std::get_if<Concept>(&Member)) {
                                    if (Conc->Name == Name) {
                                        Concepts[Name.str()] = Conc;
                                        return Conc;
                                    }
                                }
                            }

                            // If the module has a concept with a namespace definition
                            // (e.g., define containers { ... }), look inside that namespace
                            for (const auto& Member : CurrentMod->Members) {
                                if (auto* Conc = std::get_if<Concept>(&Member)) {
                                    if (auto* NS = std::get_if<Namespace>(&Conc->Def)) {
                                        // Look in namespace's modules for the target concept
                                        for (const auto& SubMod : NS->Modules) {
                                            if (SubMod.Name == Name) {
                                                // Cache the sub-module for sibling lookup
                                                if (std::find(AccessedPackageModules.begin(),
                                                              AccessedPackageModules.end(),
                                                              &SubMod) == AccessedPackageModules.end()) {
                                                    AccessedPackageModules.push_back(&SubMod);
                                                }
                                                for (const auto& SubMember : SubMod.Members) {
                                                    if (auto* SubConc = std::get_if<Concept>(&SubMember)) {
                                                        if (SubConc->Name == Name) {
                                                            Concepts[Name.str()] = SubConc;
                                                            return SubConc;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            // Also check sub-modules
                            for (const auto& SubMod : CurrentMod->Modules) {
                                if (SubMod.Name == Name) {
                                    // Cache the sub-module for sibling lookup
                                    if (std::find(AccessedPackageModules.begin(),
                                                  AccessedPackageModules.end(),
                                                  &SubMod) == AccessedPackageModules.end()) {
                                        AccessedPackageModules.push_back(&SubMod);
                                    }
                                    for (const auto& Member : SubMod.Members) {
                                        if (auto* Conc = std::get_if<Concept>(&Member)) {
                                            if (Conc->Name == Name) {
                                                Concepts[Name.str()] = Conc;
                                                return Conc;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 3. Search sibling programs (for multi-file compilation)
    for (const auto& Sibling : SiblingPrograms) {
        // Search sibling's main module
        for (const auto& Member : Sibling->MainModule.Members) {
            if (auto* Conc = std::get_if<Concept>(&Member)) {
                if (Conc->Name == Name) {
                    Concepts[Name.str()] = Conc;
                    return Conc;
                }
            }
        }

        // Also search sibling's sub-modules
        for (const auto& SubMod : Sibling->MainModule.Modules) {
            for (const auto& Member : SubMod.Members) {
                if (auto* Conc = std::get_if<Concept>(&Member)) {
                    if (Conc->Name == Name) {
                        Concepts[Name.str()] = Conc;
                        return Conc;
                    }
                }
            }
        }
    }

    return nullptr;
}

// ============================================================================
// Type Resolution
// ============================================================================

llvm::Expected<PlannedType> Planner::resolveType(const Type &T, Span InstantiationLoc) {
    PlannedType Result;
    Result.Loc = T.Loc;
    Result.Life = T.Life;

    // Build readable name from path
    std::string Name;
    for (size_t I = 0; I < T.Name.size(); ++I) {
        if (I > 0) Name += ".";
        Name += T.Name[I];
    }

    // Check if this is a type parameter substitution
    auto SubstIt = TypeSubstitutions.find(Name);
    if (SubstIt != TypeSubstitutions.end()) {
        Result = SubstIt->second;
        Result.Loc = T.Loc;  // Keep original location
        return Result;
    }

    Result.Name = Name;

    // Resolve generic arguments
    if (T.Generics) {
        for (const auto &Arg : *T.Generics) {
            auto ResolvedArg = resolveType(Arg, InstantiationLoc);
            if (!ResolvedArg) {
                return ResolvedArg.takeError();
            }
            Result.Generics.push_back(std::move(*ResolvedArg));
        }
    }

    // Handle intrinsic generic types
    if (Name == "pointer" && Result.Generics.size() == 1) {
        // pointer[T] - Itanium ABI uses P prefix
        Result.MangledName = "P" + Result.Generics[0].MangledName;
        return Result;
    }

    // Handle pointer without generics - this is an error
    if (Name == "pointer" && Result.Generics.empty()) {
        return makeGenericArityError(File, T.Loc, Name, 1, 0);
    }

    // Handle fixed-size array types: char[]N, int[]N, etc.
    // When a primitive type has a "generic argument", it's actually an array size
    // Convert char[]9 to pointer[char] for stack array access
    bool IsPrimitive = (Name == "char" || Name == "int" || Name == "bool" ||
                        Name == "size_t" || Name == "float" || Name == "double" ||
                        Name == "i8" || Name == "i16" || Name == "i32" || Name == "i64" ||
                        Name == "u8" || Name == "u16" || Name == "u32" || Name == "u64");
    if (IsPrimitive && !Result.Generics.empty()) {
        // This is a fixed-size array type like char[]PACKED_SIZE
        // Store the array size in ArraySize field
        // The generic argument is the array size (a constant like PACKED_SIZE)
        // For stack arrays, we treat them as pointer[T] for pointer arithmetic
        PlannedType PtrType;
        PtrType.Loc = T.Loc;
        PtrType.Name = "pointer";
        PlannedType ElemType;
        ElemType.Loc = T.Loc;
        ElemType.Name = Name;
        ElemType.MangledName = mangleType(ElemType);
        PtrType.Generics.push_back(std::move(ElemType));
        PtrType.MangledName = "P" + PtrType.Generics[0].MangledName;
        PtrType.Life = T.Life;
        // Store array size for alloca sizing - the generic argument should be a constant
        // Evaluate the constant to get the numeric value
        if (!Result.Generics.empty()) {
            std::string SizeName = Result.Generics[0].Name;
            // Try to parse as a number first
            try {
                std::stoull(SizeName);
                // Already numeric
                PtrType.ArraySize = SizeName;
            } catch (const std::exception&) {
                // Not a number - look up as global constant
                auto GlobalIt = PlannedGlobals.find(SizeName);
                if (GlobalIt != PlannedGlobals.end()) {
                    // Extract the value from the global constant
                    const auto& Global = GlobalIt->second;
                    if (!Global.Value.empty()) {
                        // The value should be a constant expression
                        const auto& ValueOp = Global.Value[0];
                        if (auto* ConstExpr = std::get_if<PlannedConstant>(&ValueOp.Expr)) {
                            // PlannedConstant is a Constant variant - extract integer
                            if (auto* IntConst = std::get_if<IntegerConstant>(ConstExpr)) {
                                PtrType.ArraySize = std::to_string(IntConst->Value);
                            } else {
                                // Not an integer constant - use name as fallback
                                PtrType.ArraySize = SizeName;
                            }
                        } else {
                            // Fallback - use the name as-is
                            PtrType.ArraySize = SizeName;
                        }
                    } else {
                        PtrType.ArraySize = SizeName;
                    }
                } else {
                    // Unknown constant - use name as-is (may cause emitter error)
                    PtrType.ArraySize = SizeName;
                }
            }
        }
        return PtrType;
    }

    // Check if this type refers to a generic Concept that needs instantiation
    // Use lookupConcept for cross-module resolution
    const Concept *Conc = lookupConcept(Name);
    if (Conc) {
        // If concept has generic parameters, we need to instantiate
        if (!Conc->Parameters.empty()) {
            if (Result.Generics.empty()) {
                // Generic type used without type arguments
                return makeGenericArityError(File, T.Loc, Name,
                    Conc->Parameters.size(), 0);
            }

            // Instantiate the generic
            return instantiateGeneric(*Conc, Result.Generics, InstantiationLoc);
        }

        // Non-generic concept - plan it if not already planned
        if (std::get_if<Structure>(&Conc->Def)) {
            // Plan the structure if it's a sibling concept that hasn't been planned yet
            if (InstantiatedStructures.find(Name) == InstantiatedStructures.end()) {
                auto PlannedConc = planConcept(*Conc);
                if (!PlannedConc) {
                    llvm::consumeError(PlannedConc.takeError());
                }
            }
            Result.MangledName = encodeName(Name);
            return Result;
        }
    }

    // Generate mangled name for non-generic types
    if (Result.Generics.empty()) {
        Result.MangledName = mangleType(Result);
    } else {
        Result.MangledName = mangleStructure(Name, Result.Generics);
    }

    return Result;
}

llvm::Expected<PlannedType> Planner::resolveTypePath(
    const std::vector<std::string> &Path, Span Loc) {

    if (Path.empty()) {
        return makeUndefinedTypeError(File, Loc, "<empty>");
    }

    // For now, just join the path
    std::string Name;
    for (size_t I = 0; I < Path.size(); ++I) {
        if (I > 0) Name += ".";
        Name += Path[I];
    }

    PlannedType Result;
    Result.Loc = Loc;
    Result.Name = Name;
    Result.MangledName = mangleType(Result);

    return Result;
}

llvm::Expected<PlannedType> Planner::instantiateGeneric(
    const Concept &Generic,
    const std::vector<PlannedType> &Args,
    Span InstantiationLoc) {

    // 1. Check arity matches
    if (Args.size() != Generic.Parameters.size()) {
        return makeGenericArityError(File, InstantiationLoc, Generic.Name,
                                      Generic.Parameters.size(), Args.size());
    }

    // 2. Generate cache key (e.g., "List.int" or "Map.string.int")
    std::string CacheKey = Generic.Name;
    for (const auto &Arg : Args) {
        CacheKey += ".";
        CacheKey += Arg.Name;
    }

    // 3. Check cache for structures
    auto StructCacheIt = InstantiatedStructures.find(CacheKey);
    if (StructCacheIt != InstantiatedStructures.end()) {
        PlannedType Result;
        Result.Loc = InstantiationLoc;
        Result.Name = CacheKey;
        Result.MangledName = StructCacheIt->second.MangledName;
        Result.Generics = Args;
        return Result;
    }

    // Check cache for unions
    auto UnionCacheIt = InstantiatedUnions.find(CacheKey);
    if (UnionCacheIt != InstantiatedUnions.end()) {
        PlannedType Result;
        Result.Loc = InstantiationLoc;
        Result.Name = CacheKey;
        Result.MangledName = UnionCacheIt->second.MangledName;
        Result.Generics = Args;
        return Result;
    }

    // 4. Set up TypeSubstitutions
    std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
    for (size_t I = 0; I < Args.size(); ++I) {
        TypeSubstitutions[Generic.Parameters[I].Name] = Args[I];
    }

    // Save and clear scopes to prevent caller's local variables from leaking
    // into the generic instantiation context
    std::vector<std::map<std::string, LocalBinding>> OldScopes;
    std::swap(Scopes, OldScopes);

    // 5. Plan the specialized version based on definition kind
    PlannedType Result;
    Result.Loc = InstantiationLoc;
    Result.Name = CacheKey;
    Result.Generics = Args;

    // Create provenance info for debug tracking
    auto Origin = std::make_shared<InstantiationInfo>();
    Origin->DefinitionLoc = Generic.Loc;
    Origin->InstantiationLoc = InstantiationLoc;
    for (const auto &Arg : Args) {
        Origin->TypeArgs.push_back(Arg.Name);
    }
    Result.Origin = Origin;

    // Plan based on definition type
    bool Success = false;
    if (auto *Struct = std::get_if<Structure>(&Generic.Def)) {
        // Insert placeholder BEFORE planning to handle self-referencing types
        // like Node[T] containing pointer[Node[T]]
        PlannedStructure Placeholder;
        Placeholder.Name = CacheKey;
        Placeholder.MangledName = mangleStructure(Generic.Name, Args);
        Result.MangledName = Placeholder.MangledName;
        InstantiatedStructures[CacheKey] = std::move(Placeholder);

        auto Planned = planStructure(*Struct, Generic.Name, Args);
        if (!Planned) {
            InstantiatedStructures.erase(CacheKey);  // Remove placeholder on error
            TypeSubstitutions = OldSubst;
            std::swap(Scopes, OldScopes);  // Restore scopes on error
            return Planned.takeError();
        }
        Planned->Origin = Origin;
        InstantiatedStructures[CacheKey] = std::move(*Planned);
        Success = true;
    }
    else if (auto *Un = std::get_if<Union>(&Generic.Def)) {
        // Insert placeholder BEFORE planning for self-referencing unions
        PlannedUnion Placeholder;
        Placeholder.Name = CacheKey;
        Placeholder.MangledName = mangleStructure(Generic.Name, Args);
        Result.MangledName = Placeholder.MangledName;
        InstantiatedUnions[CacheKey] = std::move(Placeholder);

        auto Planned = planUnion(*Un, Generic.Name, Args);
        if (!Planned) {
            InstantiatedUnions.erase(CacheKey);  // Remove placeholder on error
            TypeSubstitutions = OldSubst;
            std::swap(Scopes, OldScopes);  // Restore scopes on error
            return Planned.takeError();
        }
        Planned->Origin = Origin;
        InstantiatedUnions[CacheKey] = std::move(*Planned);
        Success = true;
    }
    else if (auto *T = std::get_if<Type>(&Generic.Def)) {
        // Type alias - resolve with substitutions active
        auto Resolved = resolveType(*T, InstantiationLoc);
        TypeSubstitutions = OldSubst;
        std::swap(Scopes, OldScopes);  // Restore scopes
        if (!Resolved) {
            return Resolved.takeError();
        }
        return *Resolved;
    }

    // Restore old substitutions and scopes
    TypeSubstitutions = OldSubst;
    std::swap(Scopes, OldScopes);

    if (!Success) {
        return makePlannerNotImplementedError(File, InstantiationLoc,
            "generic instantiation for this definition kind");
    }

    return Result;
}

// ============================================================================
// Generic Function Instantiation
// ============================================================================

llvm::Expected<PlannedFunction> Planner::instantiateGenericFunction(
    const Function &Func,
    const std::vector<PlannedType> &TypeArgs,
    Span InstantiationLoc) {

    // 1. Check arity matches
    if (TypeArgs.size() != Func.Parameters.size()) {
        return makeGenericArityError(File, InstantiationLoc, Func.Name,
                                      Func.Parameters.size(), TypeArgs.size());
    }

    // 2. Generate cache key (e.g., "identity.int" or "map.string.int")
    std::string CacheKey = Func.Name;
    for (const auto &Arg : TypeArgs) {
        CacheKey += ".";
        CacheKey += Arg.Name;
    }

    // 3. Check cache
    auto CacheIt = InstantiatedFunctions.find(CacheKey);
    if (CacheIt != InstantiatedFunctions.end()) {
        return CacheIt->second;
    }

    // 4. Set up TypeSubstitutions
    std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;
    for (size_t I = 0; I < TypeArgs.size(); ++I) {
        TypeSubstitutions[Func.Parameters[I].Name] = TypeArgs[I];
    }

    // 5. Plan the specialized version
    auto Planned = planFunction(Func, nullptr);
    if (!Planned) {
        TypeSubstitutions = OldSubst;
        return Planned.takeError();
    }

    // 6. Update name and mangled name with type arguments
    Planned->Name = CacheKey;
    // Regenerate mangled name with the specialized types
    Planned->MangledName = mangleFunction(CacheKey, Planned->Input, nullptr);

    // 7. Create provenance info
    auto Origin = std::make_shared<InstantiationInfo>();
    Origin->DefinitionLoc = Func.Loc;
    Origin->InstantiationLoc = InstantiationLoc;
    for (const auto &Arg : TypeArgs) {
        Origin->TypeArgs.push_back(Arg.Name);
    }
    Planned->Origin = Origin;

    // 8. Restore old substitutions and cache the result
    TypeSubstitutions = OldSubst;
    InstantiatedFunctions[CacheKey] = *Planned;

    return *Planned;
}

llvm::Expected<std::vector<PlannedType>> Planner::inferTypeArguments(
    const Function &Func,
    const std::vector<PlannedType> &ArgTypes,
    Span Loc) {

    // Simple inference: match parameter types to argument types
    // For each type parameter, find where it appears in parameter types
    // and extract the corresponding type from arguments

    std::map<std::string, PlannedType> Inferred;

    // For each parameter position
    for (size_t I = 0; I < Func.Input.size() && I < ArgTypes.size(); ++I) {
        if (!Func.Input[I].ItemType) {
            continue;
        }

        const Type &ParamType = *Func.Input[I].ItemType;
        const PlannedType &ArgType = ArgTypes[I];

        // Check if the parameter type is a simple type variable
        if (ParamType.Name.size() == 1) {
            const std::string &TypeName = ParamType.Name[0];

            // Check if this is one of the generic parameters
            for (const auto &GP : Func.Parameters) {
                if (GP.Name == TypeName) {
                    // Found a match - infer this type parameter
                    auto It = Inferred.find(TypeName);
                    if (It == Inferred.end()) {
                        Inferred[TypeName] = ArgType;
                    } else if (!typesEqual(It->second, ArgType)) {
                        // Conflicting inference
                        return makePlannerNotImplementedError(File, Loc,
                            ("conflicting type inference for " + TypeName +
                             ": " + It->second.Name + " vs " + ArgType.Name).c_str());
                    }
                    break;
                }
            }
        }
        // TODO: Handle more complex cases like List[T] matching List[int]
    }

    // Build result vector in parameter order
    std::vector<PlannedType> Result;
    for (const auto &GP : Func.Parameters) {
        auto It = Inferred.find(GP.Name);
        if (It == Inferred.end()) {
            return makePlannerNotImplementedError(File, Loc,
                ("could not infer type for parameter " + GP.Name).c_str());
        }
        Result.push_back(It->second);
    }

    return Result;
}

// ============================================================================
// Item Planning
// ============================================================================

llvm::Expected<PlannedItem> Planner::planItem(const Item &It) {
    PlannedItem Result;
    Result.Loc = It.Loc;
    Result.Private = It.Private;

    if (It.Name) {
        Result.Name = std::make_unique<std::string>(*It.Name);
    }

    if (It.ItemType) {
        auto ResolvedType = resolveType(*It.ItemType, It.Loc);
        if (!ResolvedType) {
            return ResolvedType.takeError();
        }
        Result.ItemType = std::make_unique<PlannedType>(std::move(*ResolvedType));
    }

    // Plan attributes
    for (const auto &Attr : It.Attributes) {
        PlannedAttribute PA;
        PA.Loc = Attr.Loc;
        PA.Name = Attr.Name;
        PA.Value = cloneModel(Attr.Value);
        Result.Attributes.push_back(std::move(PA));
    }

    return Result;
}

// ============================================================================
// Property Planning
// ============================================================================

llvm::Expected<PlannedProperty> Planner::planProperty(const Property &Prop) {
    PlannedProperty Result;
    Result.Loc = Prop.Loc;
    Result.Private = Prop.Private;
    Result.Name = Prop.Name;

    // Resolve type (PropType is a unique_ptr in Model::Property)
    if (!Prop.PropType) {
        return makePlannerNotImplementedError(File, Prop.Loc, "property without type");
    }
    auto ResolvedType = resolveType(*Prop.PropType, Prop.Loc);
    if (!ResolvedType) {
        return ResolvedType.takeError();
    }
    Result.PropType = std::move(*ResolvedType);

    // Generate mangled name
    Result.MangledName = encodeName(Prop.Name);

    // Plan initializer if present
    if (Prop.Initializer) {
        auto PlannedInit = planOperands(*Prop.Initializer);
        if (!PlannedInit) {
            return PlannedInit.takeError();
        }
        Result.Initializer = std::make_unique<std::vector<PlannedOperand>>(
            std::move(*PlannedInit));
    }

    // Plan attributes
    for (const auto &Attr : Prop.Attributes) {
        PlannedAttribute PA;
        PA.Loc = Attr.Loc;
        PA.Name = Attr.Name;
        PA.Value = cloneModel(Attr.Value);
        Result.Attributes.push_back(std::move(PA));
    }

    return Result;
}

// ============================================================================
// Variant Planning
// ============================================================================

llvm::Expected<PlannedVariant> Planner::planVariant(const Variant &Var, int Tag) {
    PlannedVariant Result;
    Result.Loc = Var.Loc;
    Result.Name = Var.Name;
    Result.Tag = Tag;

    // Generate mangled name
    Result.MangledName = encodeName(Var.Name);

    // Resolve type if present
    if (Var.VarType) {
        auto ResolvedType = resolveType(*Var.VarType, Var.Loc);
        if (!ResolvedType) {
            return ResolvedType.takeError();
        }
        Result.VarType = std::make_unique<PlannedType>(std::move(*ResolvedType));
    }

    // Plan attributes
    for (const auto &Attr : Var.Attributes) {
        PlannedAttribute PA;
        PA.Loc = Attr.Loc;
        PA.Name = Attr.Name;
        PA.Value = cloneModel(Attr.Value);
        Result.Attributes.push_back(std::move(PA));
    }

    return Result;
}

// ============================================================================
// Expression Planning
// ============================================================================

llvm::Expected<PlannedOperand> Planner::planOperand(const Operand &Op) {
    PlannedOperand Result;
    Result.Loc = Op.Loc;

    // Check for variable member access encoded as Type path (e.g., p.x parsed as Type(["p", "x"]))
    // This happens when the parser treats dotted names as type paths
    std::vector<Type> ImplicitMemberAccess;
    Operand ModifiedOp = Op;

    if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
        if (TypeExpr->Name.size() > 1 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
            // Check if the first element is a local variable or property
            auto BindInfo = checkLocalOrProperty(TypeExpr->Name[0]);
            if (BindInfo.IsLocal) {
                // The first element is a variable, rest are member accesses
                // Convert to single-element Type + implicit member access
                Type SingleType;
                SingleType.Loc = TypeExpr->Loc;
                SingleType.Name = {TypeExpr->Name[0]};
                SingleType.Generics = nullptr;
                SingleType.Life = TypeExpr->Life;
                ModifiedOp.Expr = SingleType;

                // Collect remaining path elements as implicit member access
                for (size_t i = 1; i < TypeExpr->Name.size(); ++i) {
                    Type MemberType;
                    MemberType.Loc = TypeExpr->Loc;
                    MemberType.Name = {TypeExpr->Name[i]};
                    MemberType.Generics = nullptr;
                    MemberType.Life = UnspecifiedLifetime{};
                    ImplicitMemberAccess.push_back(MemberType);
                }
            } else if (BindInfo.IsProperty) {
                // First element is a property - convert to this.property.rest...
                Type ThisTypeExpr;
                ThisTypeExpr.Loc = TypeExpr->Loc;
                ThisTypeExpr.Name = {"this"};
                ThisTypeExpr.Generics = nullptr;
                ThisTypeExpr.Life = TypeExpr->Life;
                ModifiedOp.Expr = ThisTypeExpr;

                // Add all path elements as implicit member access
                for (size_t i = 0; i < TypeExpr->Name.size(); ++i) {
                    Type MemberType;
                    MemberType.Loc = TypeExpr->Loc;
                    MemberType.Name = {TypeExpr->Name[i]};
                    MemberType.Generics = nullptr;
                    MemberType.Life = UnspecifiedLifetime{};
                    ImplicitMemberAccess.push_back(MemberType);
                }
            }

            // Check for qualified global constant access (e.g., hashing.HASH_PRIMES)
            if (ImplicitMemberAccess.empty() && TypeExpr->Name.size() == 2) {
                const std::string& NamespaceName = TypeExpr->Name[0];
                const std::string& GlobalName = TypeExpr->Name[1];

                // Check if first element is a namespace concept
                const Concept* Conc = lookupConcept(NamespaceName);
                if (Conc && std::holds_alternative<Namespace>(Conc->Def)) {
                    // Look for a global constant in this namespace
                    const Namespace& NS = std::get<Namespace>(Conc->Def);
                    for (const auto& Member : NS.Members) {
                        if (auto* NestedConc = std::get_if<Concept>(&Member)) {
                            if (NestedConc->Name == GlobalName &&
                                std::holds_alternative<Global>(NestedConc->Def)) {
                                // Found the global - plan it if needed
                                auto GlobalIt = PlannedGlobals.find(GlobalName);
                                if (GlobalIt == PlannedGlobals.end()) {
                                    auto PlannedConc = planConcept(*NestedConc);
                                    if (PlannedConc) {
                                        GlobalIt = PlannedGlobals.find(GlobalName);
                                    }
                                }
                                if (GlobalIt != PlannedGlobals.end()) {
                                    // Return the global reference
                                    const auto& GVal = GlobalIt->second.Value;
                                    bool IsArray = !GVal.empty() &&
                                        std::holds_alternative<PlannedMatrix>(GVal[0].Expr);

                                    if (!IsArray && GVal.size() == 1) {
                                        // Scalar constant
                                        PlannedOperand ResultOp;
                                        ResultOp.Loc = Op.Loc;
                                        ResultOp.Expr = GVal[0].Expr;
                                        ResultOp.ResultType = GlobalIt->second.GlobalType;
                                        return ResultOp;
                                    }

                                    // Array constant - return a pointer to the array
                                    PlannedGlobalRef GRef;
                                    GRef.Loc = Op.Loc;
                                    GRef.Name = GlobalIt->second.Name;
                                    GRef.MangledName = GlobalIt->second.MangledName;
                                    GRef.GlobalType = GlobalIt->second.GlobalType;

                                    PlannedOperand ResultOp;
                                    ResultOp.Loc = Op.Loc;
                                    ResultOp.Expr = std::move(GRef);
                                    PlannedType PtrType;
                                    PtrType.Loc = GlobalIt->second.GlobalType.Loc;
                                    PtrType.Name = "pointer";
                                    if (GlobalIt->second.GlobalType.Generics.empty()) {
                                        PtrType.Generics.push_back(GlobalIt->second.GlobalType);
                                        PtrType.MangledName = "P" + GlobalIt->second.GlobalType.MangledName;
                                    } else {
                                        PtrType.Generics = GlobalIt->second.GlobalType.Generics;
                                        PtrType.MangledName = "P" + GlobalIt->second.GlobalType.MangledName;
                                    }
                                    ResultOp.ResultType = PtrType;
                                    return ResultOp;
                                }
                            }
                        }
                    }
                }
            }
        }
        // Check for implicit property access (e.g., just "x" in a method body)
        // This should become "this.x"
        else if (TypeExpr->Name.size() == 1 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
            const std::string& Name = TypeExpr->Name[0];

            // Check for built-in constants (available without loading stdlib)
            if (Name == "SIZE_MAX") {
                PlannedOperand ResultOp;
                ResultOp.Loc = Op.Loc;
                ResultOp.Expr = PlannedConstant{IntegerConstant{0xFFFFFFFFFFFFFFFFULL}};
                PlannedType SizeType;
                SizeType.Loc = Op.Loc;
                SizeType.Name = "size_t";
                SizeType.MangledName = "m";
                ResultOp.ResultType = SizeType;
                return ResultOp;
            }

            // Check if this is a global constant
            auto GlobalIt = PlannedGlobals.find(Name);
            if (GlobalIt == PlannedGlobals.end()) {
                // Try to find and plan the global if it's a sibling concept
                const Concept *GlobalConcept = lookupConcept(Name);
                if (GlobalConcept && std::holds_alternative<Global>(GlobalConcept->Def)) {
                    auto PlannedConc = planConcept(*GlobalConcept);
                    if (PlannedConc) {
                        GlobalIt = PlannedGlobals.find(Name);
                    }
                }
            }
            if (GlobalIt != PlannedGlobals.end()) {
                // Check if this is a scalar constant or an array constant
                // Scalar constants (like PAGE_SIZE int 0x1000) have a single constant value
                // Array constants (like PRIMES: int[] [2, 3, 5, 7, 11]) have a Matrix value
                const auto& GVal = GlobalIt->second.Value;
                bool IsArray = false;
                if (!GVal.empty()) {
                    if (std::holds_alternative<PlannedMatrix>(GVal[0].Expr)) {
                        IsArray = true;
                    }
                }

                if (!IsArray && GVal.size() == 1) {
                    // Scalar constant - return the constant value directly (constant folding)
                    PlannedOperand ResultOp;
                    ResultOp.Loc = Op.Loc;
                    ResultOp.Expr = GVal[0].Expr;
                    ResultOp.ResultType = GlobalIt->second.GlobalType;
                    return ResultOp;
                }

                // Array constant - return a pointer to the array
                PlannedGlobalRef GRef;
                GRef.Loc = Op.Loc;
                GRef.Name = GlobalIt->second.Name;
                GRef.MangledName = GlobalIt->second.MangledName;
                GRef.GlobalType = GlobalIt->second.GlobalType;

                PlannedOperand ResultOp;
                ResultOp.Loc = Op.Loc;
                ResultOp.Expr = std::move(GRef);
                // For array types, the result type is a pointer to the element type
                // If GlobalType has no generics, it IS the element type (e.g., int[] -> pointer[int])
                PlannedType PtrType;
                PtrType.Loc = GlobalIt->second.GlobalType.Loc;
                PtrType.Name = "pointer";
                if (GlobalIt->second.GlobalType.Generics.empty()) {
                    // Array type like int[] - use GlobalType as element type
                    PtrType.Generics.push_back(GlobalIt->second.GlobalType);
                    PtrType.MangledName = "P" + GlobalIt->second.GlobalType.MangledName;
                } else {
                    // Already has generics
                    PtrType.Generics = GlobalIt->second.GlobalType.Generics;
                    PtrType.MangledName = "P" + GlobalIt->second.GlobalType.MangledName;
                }
                ResultOp.ResultType = PtrType;
                return ResultOp;
            }
            // Check for implicit property access: when 'data' is used in a method body,
            // it should become 'this.data'. But ONLY if 'this' is actually in scope.
            // This guards against context mismatches during recursive planning.
            // IMPORTANT: Check if the name is a local variable first - local variables
            // (including parameters) should shadow properties with the same name.
            auto LocalBind = lookupLocalBinding(Name);
            if (!LocalBind) {
                auto ThisType = lookupLocal("this");
                if (CurrentStructureProperties && ThisType) {
                    // Extract base name from CurrentStructureName for generic types
                    // e.g., "VectorIterator.char" -> "VectorIterator"
                    std::string BaseStructName = CurrentStructureName;
                    size_t DotPos = BaseStructName.find('.');
                    if (DotPos != std::string::npos) {
                        BaseStructName = BaseStructName.substr(0, DotPos);
                    }

                    // 'this' is now pointer[T], so check the inner type
                    std::string ThisTypeName = ThisType->Name;
                    if (ThisType->Name == "pointer" && !ThisType->Generics.empty()) {
                        ThisTypeName = ThisType->Generics[0].Name;
                    }

                    if (ThisTypeName == CurrentStructureName || ThisTypeName == BaseStructName) {
                        for (const auto &Prop : *CurrentStructureProperties) {
                            if (Prop.Name == Name) {
                                // Convert to "this" with member access to the property
                                Type ThisTypeExpr;
                                ThisTypeExpr.Loc = TypeExpr->Loc;
                                ThisTypeExpr.Name = {"this"};
                                ThisTypeExpr.Generics = nullptr;
                                ThisTypeExpr.Life = TypeExpr->Life;
                                ModifiedOp.Expr = ThisTypeExpr;

                                // Add the property as implicit member access
                                Type PropType;
                                PropType.Loc = TypeExpr->Loc;
                                PropType.Name = {Name};
                                PropType.Generics = nullptr;
                                PropType.Life = UnspecifiedLifetime{};
                                ImplicitMemberAccess.push_back(PropType);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Plan the expression
    auto PlannedExpr = planExpression(ModifiedOp.Expr);
    if (!PlannedExpr) {
        return PlannedExpr.takeError();
    }
    Result.Expr = std::move(*PlannedExpr);

    // Compute ResultType via type inference
    auto InferredType = inferExpressionType(Result.Expr);
    if (!InferredType) {
        return InferredType.takeError();
    }
    Result.ResultType = std::move(*InferredType);

    // Combine implicit member access (from Type path) with explicit member access
    std::vector<Type> CombinedMemberAccess = ImplicitMemberAccess;
    if (Op.MemberAccess && !Op.MemberAccess->empty()) {
        for (const auto& M : *Op.MemberAccess) {
            CombinedMemberAccess.push_back(M);
        }
    }

    // Apply member access to refine the result type and compute field indices
    if (!CombinedMemberAccess.empty()) {
        auto MemberChain = resolveMemberAccessChain(Result.ResultType,
                                                     CombinedMemberAccess, Op.Loc);
        if (!MemberChain) {
            return MemberChain.takeError();
        }
        Result.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
            std::move(*MemberChain));

        // Update result type to the final type after all member accesses
        if (!Result.MemberAccess->empty()) {
            Result.ResultType = Result.MemberAccess->back().ResultType;
        }
    }

    return Result;
}

llvm::Expected<std::vector<PlannedOperand>> Planner::planOperands(
    const std::vector<Operand> &Ops) {

    // Pre-process: Combine Type + Matrix patterns into Type with generics
    // This handles the new grammar where generic arguments are parsed as Vector
    // e.g., HashMap[String, int]$() becomes: Name + Vector + Lifetime + Tuple
    // We combine Name + Vector into Type with generics, then attach Lifetime
    std::vector<Operand> ProcessedOps;
    ProcessedOps.reserve(Ops.size());

    for (size_t i = 0; i < Ops.size(); ++i) {
        const Operand& Op = Ops[i];

        // Check for Type (name only, no generics) followed by Matrix (generic arguments)
        if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
            if (!TypeExpr->Name.empty() && (!TypeExpr->Generics || TypeExpr->Generics->empty()) &&
                i + 1 < Ops.size()) {
                const Operand& NextOp = Ops[i + 1];
                if (auto* MatrixExpr = std::get_if<Matrix>(&NextOp.Expr)) {
                    // Extract types from Matrix elements (each element is a vector of operands)
                    // Handle nested generics: [Box[int]] is Matrix containing [Name("Box"), Matrix([int])]
                    // Use a recursive lambda to handle arbitrary nesting depth
                    std::function<std::optional<Type>(const std::vector<Operand>&)> extractTypeFromRow;
                    extractTypeFromRow = [&](const std::vector<Operand>& Row) -> std::optional<Type> {
                        if (Row.empty()) return std::nullopt;

                        // Check for nested generic pattern: Type + Matrix
                        if (Row.size() >= 2) {
                            if (auto* NameType = std::get_if<Type>(&Row[0].Expr)) {
                                if (!NameType->Name.empty() &&
                                    (!NameType->Generics || NameType->Generics->empty())) {
                                    if (auto* NestedMatrix = std::get_if<Matrix>(&Row[1].Expr)) {
                                        // Recursively extract generics from nested Matrix
                                        std::vector<Type> NestedGenerics;
                                        for (const auto& NestedRow : NestedMatrix->Operations) {
                                            auto NestedType = extractTypeFromRow(NestedRow);
                                            if (!NestedType) return std::nullopt;
                                            NestedGenerics.push_back(std::move(*NestedType));
                                        }
                                        if (!NestedGenerics.empty()) {
                                            Type Result = *NameType;
                                            Result.Generics = std::make_shared<std::vector<Type>>(
                                                std::move(NestedGenerics));
                                            return Result;
                                        }
                                    }
                                }
                            }
                        }

                        // Single-element row: just extract the type
                        if (Row.size() == 1) {
                            if (auto* ElemType = std::get_if<Type>(&Row[0].Expr)) {
                                return *ElemType;
                            }
                        }

                        return std::nullopt;
                    };

                    std::vector<Type> ExtractedGenerics;
                    bool ValidGenerics = true;
                    for (const auto& Row : MatrixExpr->Operations) {
                        auto Extracted = extractTypeFromRow(Row);
                        if (Extracted) {
                            ExtractedGenerics.push_back(std::move(*Extracted));
                        } else {
                            ValidGenerics = false;
                            break;
                        }
                    }

                    if (ValidGenerics && !ExtractedGenerics.empty()) {
                        // Create combined Type with generics
                        Type CombinedType = *TypeExpr;
                        CombinedType.Generics = std::make_shared<std::vector<Type>>(std::move(ExtractedGenerics));

                        // Use the lifetime from the Matrix (e.g., [T]^lifetime)
                        if (!std::holds_alternative<UnspecifiedLifetime>(MatrixExpr->Life)) {
                            CombinedType.Life = MatrixExpr->Life;
                        }

                        // Create new operand with combined type
                        Operand CombinedOp;
                        CombinedOp.Loc = Op.Loc;
                        CombinedOp.Expr = CombinedType;
                        CombinedOp.MemberAccess = NextOp.MemberAccess;  // Preserve member access from Matrix
                        ProcessedOps.push_back(std::move(CombinedOp));
                        i++;  // Skip the Matrix operand (already processed)
                        continue;
                    }

                    // Check for stack array pattern: PrimitiveType[Constant]
                    // e.g., char[64] where Matrix contains a single integer constant
                    // This creates a fixed-size stack array
                    if (!ValidGenerics && MatrixExpr->Operations.size() == 1 &&
                        MatrixExpr->Operations[0].size() == 1) {
                        const auto& SizeOp = MatrixExpr->Operations[0][0];
                        if (auto* SizeConst = std::get_if<Constant>(&SizeOp.Expr)) {
                            // Check if this is an integer constant
                            if (auto* IntConst = std::get_if<IntegerConstant>(SizeConst)) {
                                // Check if base type is a primitive
                                std::string BaseName = TypeExpr->Name.empty() ? "" : TypeExpr->Name[0];
                                bool IsPrimitive = (BaseName == "char" || BaseName == "int" ||
                                                    BaseName == "bool" || BaseName == "size_t" ||
                                                    BaseName == "float" || BaseName == "double" ||
                                                    BaseName == "i8" || BaseName == "i16" ||
                                                    BaseName == "i32" || BaseName == "i64" ||
                                                    BaseName == "u8" || BaseName == "u16" ||
                                                    BaseName == "u32" || BaseName == "u64");
                                if (IsPrimitive) {
                                    // Create a Type with the size as a "generic" argument
                                    // resolveType will then handle this as a fixed-size array
                                    Type SizeType;
                                    SizeType.Loc = SizeOp.Loc;
                                    SizeType.Name = {std::to_string(IntConst->Value)};
                                    SizeType.Life = UnspecifiedLifetime{};

                                    Type CombinedType = *TypeExpr;
                                    CombinedType.Generics = std::make_shared<std::vector<Type>>();
                                    CombinedType.Generics->push_back(std::move(SizeType));

                                    Operand CombinedOp;
                                    CombinedOp.Loc = Op.Loc;
                                    CombinedOp.Expr = CombinedType;
                                    CombinedOp.MemberAccess = NextOp.MemberAccess;
                                    ProcessedOps.push_back(std::move(CombinedOp));
                                    i++;  // Skip the Matrix operand
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Check for Type (no generics) + Lifetime pattern (e.g., Point$(1,2))
        // This handles non-generic constructor calls with lifetime
        if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
            if (!TypeExpr->Name.empty() && (!TypeExpr->Generics || TypeExpr->Generics->empty()) &&
                std::holds_alternative<UnspecifiedLifetime>(TypeExpr->Life) &&
                i + 1 < Ops.size()) {
                const Operand& NextOp = Ops[i + 1];
                if (auto* LifeType = std::get_if<Type>(&NextOp.Expr)) {
                    if (LifeType->Name.empty() &&
                        !std::holds_alternative<UnspecifiedLifetime>(LifeType->Life)) {
                        // Combine Type + Lifetime
                        Type CombinedType = *TypeExpr;
                        CombinedType.Life = LifeType->Life;

                        Operand CombinedOp;
                        CombinedOp.Loc = Op.Loc;
                        CombinedOp.Expr = CombinedType;
                        CombinedOp.MemberAccess = Op.MemberAccess;
                        ProcessedOps.push_back(std::move(CombinedOp));
                        i++;  // Skip the Lifetime operand
                        continue;
                    }
                }
            }
        }

        ProcessedOps.push_back(Op);
    }

    std::vector<PlannedOperand> Result;
    Result.reserve(ProcessedOps.size());

    for (size_t i = 0; i < ProcessedOps.size(); ++i) {
        const auto &Op = ProcessedOps[i];

        // Check for method call pattern: variable.method followed by Tuple
        // e.g., p.distance() where p is a variable of struct type
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];

            // Check if current op is a Type path like ["p", "method"] where "p" is a variable
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                if (TypeExpr->Name.size() >= 2 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
                    // Check if first element is a local variable or a property
                    auto BindInfo = checkLocalOrProperty(TypeExpr->Name[0]);
                    if ((BindInfo.IsLocal || BindInfo.IsProperty) && std::holds_alternative<Tuple>(NextOp.Expr)) {
                        // This is variable.member...method(args)
                        // Last element of path is the method name
                        std::string MethodName = TypeExpr->Name.back();

                        // Build the variable access (all but the last element)
                        Type VarType;
                        VarType.Loc = TypeExpr->Loc;
                        for (size_t j = 0; j < TypeExpr->Name.size() - 1; ++j) {
                            VarType.Name.push_back(TypeExpr->Name[j]);
                        }
                        VarType.Generics = nullptr;
                        VarType.Life = TypeExpr->Life;

                        Operand VarOp;
                        VarOp.Loc = Op.Loc;
                        VarOp.Expr = VarType;
                        // No member access - it's all in the Type path

                        auto PlannedVar = planOperand(VarOp);
                        if (!PlannedVar) {
                            return PlannedVar.takeError();
                        }

                        // Plan the arguments first to get their types for overload resolution
                        Operand ArgsOp = NextOp;
                        ArgsOp.MemberAccess = nullptr;
                        auto PlannedArgs = planOperand(ArgsOp);
                        if (!PlannedArgs) {
                            return PlannedArgs.takeError();
                        }

                        // Look up the method on the variable's type with overload resolution
                        // Auto-deref pointer types for method lookup
                        PlannedType LookupType = PlannedVar->ResultType.getInnerTypeIfPointerLike();
                        auto ArgTypes = extractArgTypes(*PlannedArgs);
                        auto MethodMatch = lookupMethod(LookupType, MethodName, ArgTypes, Op.Loc);
                        if (MethodMatch) {
                            // Create method call with instance as first argument
                            PlannedCall Call;
                            Call.Loc = Op.Loc;
                            Call.Name = MethodName;
                            Call.MangledName = MethodMatch->MangledName;
                            Call.IsIntrinsic = false;  // Methods are not intrinsic
                            Call.IsOperator = false;
                            Call.ResultType = MethodMatch->ReturnType;

                            // Build args: [region (if any), instance, ...tuple_args]
                            Call.Args = std::make_shared<std::vector<PlannedOperand>>();

                            // Handle function# page parameter for method calls
                            if (MethodMatch->RequiresPageParam) {
                                Call.RequiresPageParam = true;
                                Call.Life = TypeExpr->Life;

                                if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                    // ^name - pass explicit page as first argument
                                    // Special case: ^this generates Page.get(this)
                                    if (RefLife->Location == "this") {
                                        auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                        if (!PageGetResult) {
                                            return PageGetResult.takeError();
                                        }
                                        Call.Args->push_back(std::move(*PageGetResult));
                                    } else {
                                        auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                        if (!RegionBinding) {
                                            return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        // Validate that the referenced variable is a pointer[Page]
                                        auto isPageType = [](const std::string &Name) {
                                            return Name == "Page" || Name == "scaly.memory.Page";
                                        };
                                        bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                             !RegionBinding->Type.Generics.empty() &&
                                                             isPageType(RegionBinding->Type.Generics[0].Name);
                                        if (!isPagePointer) {
                                            return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                        }

                                        PlannedOperand RegionArg;
                                        RegionArg.Loc = RefLife->Loc;
                                        RegionArg.ResultType = RegionBinding->Type;
                                        RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                        Call.Args->push_back(std::move(RegionArg));
                                    }
                                }
                                // For # lifetime, Emitter uses ReturnPage (rp)
                                // For $ lifetime, Emitter uses LocalPage
                                else if (std::holds_alternative<LocalLifetime>(TypeExpr->Life)) {
                                    // Track that we need local page allocation
                                    CurrentFunctionUsesLocalLifetime = true;
                                    if (!ScopeInfoStack.empty()) {
                                        ScopeInfoStack.back().HasLocalAllocations = true;
                                    }
                                }
                                else if (!std::holds_alternative<CallLifetime>(TypeExpr->Life)) {
                                    return makePlannerNotImplementedError(File, Op.Loc,
                                        "method# '" + MethodName + "' must be called with #, $, or ^page syntax");
                                }
                            }
                            // If the Type has a ReferenceLifetime but method doesn't have PageParameter,
                            // still pass the region (for legacy/init calls)
                            else if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                // Special case: ^this generates Page.get(this)
                                if (RefLife->Location == "this") {
                                    auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                    if (!PageGetResult) {
                                        return PageGetResult.takeError();
                                    }
                                    Call.Args->push_back(std::move(*PageGetResult));
                                } else {
                                    auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                    if (!RegionBinding) {
                                        return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                    }
                                    // Validate that the referenced variable is a pointer[Page]
                                    auto isPageType = [](const std::string &Name) {
                                        return Name == "Page" || Name == "scaly.memory.Page";
                                    };
                                    bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                         !RegionBinding->Type.Generics.empty() &&
                                                         isPageType(RegionBinding->Type.Generics[0].Name);
                                    if (!isPagePointer) {
                                        return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                    }

                                    PlannedOperand RegionArg;
                                    RegionArg.Loc = RefLife->Loc;
                                    RegionArg.ResultType = RegionBinding->Type;
                                    RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                    Call.Args->push_back(std::move(RegionArg));
                                }
                            }

                            Call.Args->push_back(std::move(*PlannedVar));

                            // Add tuple elements as additional arguments
                            if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                for (auto& Comp : TupleExpr->Components) {
                                    // Each component's value is a vector of operands
                                    for (auto& ValOp : Comp.Value) {
                                        Call.Args->push_back(std::move(ValOp));
                                    }
                                }
                            }

                            // Create operand with the call
                            PlannedOperand CallOp;
                            CallOp.Loc = Op.Loc;
                            CallOp.Expr = std::move(Call);
                            CallOp.ResultType = MethodMatch->ReturnType;

                            // Handle chained method calls: expr.method1().method2().method3()
                            auto ChainResult = processChainedMethodCalls(std::move(CallOp), ProcessedOps, i + 1);
                            if (!ChainResult) {
                                return ChainResult.takeError();
                            }

                            Result.push_back(std::move(ChainResult->CallOp));
                            i += 1 + ChainResult->ConsumedOperands;  // Skip consumed tuples
                            continue;
                        }
                        // If method not found, fall through to normal processing
                    }

                    // Check for sibling module function call pattern (e.g., lexer.test())
                    // First element is sibling module name, second is function name
                    if (TypeExpr->Name.size() == 2 && std::holds_alternative<Tuple>(NextOp.Expr)) {
                        const std::string& ModuleName = TypeExpr->Name[0];
                        const std::string& FuncName = TypeExpr->Name[1];

                        // Search CurrentNamespaceModules for the module
                        const Module* TargetModule = nullptr;
                        for (const Module* Mod : CurrentNamespaceModules) {
                            if (Mod->Name == ModuleName) {
                                TargetModule = Mod;
                                break;
                            }
                        }

                        if (TargetModule) {
                            // Look for the function in the module
                            const Function* TargetFunc = nullptr;
                            for (const auto& Member : TargetModule->Members) {
                                if (auto* Func = std::get_if<Function>(&Member)) {
                                    if (Func->Name == FuncName) {
                                        TargetFunc = Func;
                                        break;
                                    }
                                }
                                // Skip non-function members
                            }

                            if (TargetFunc) {
                                // Plan the arguments
                                Operand ArgsOp = NextOp;
                                ArgsOp.MemberAccess = nullptr;
                                auto PlannedArgs = planOperand(ArgsOp);
                                if (!PlannedArgs) {
                                    return PlannedArgs.takeError();
                                }

                                // Plan the function
                                auto PlannedFunc = planFunction(*TargetFunc, nullptr);
                                if (!PlannedFunc) {
                                    return PlannedFunc.takeError();
                                }

                                // Create the function call
                                PlannedCall Call;
                                Call.Loc = Op.Loc;
                                Call.Name = TargetFunc->Name;
                                Call.MangledName = PlannedFunc->MangledName;
                                Call.IsIntrinsic = false;
                                Call.IsOperator = false;
                                if (PlannedFunc->Returns) {
                                    Call.ResultType = *PlannedFunc->Returns;
                                }

                                // Build args from tuple
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (auto& Comp : TupleExpr->Components) {
                                        for (auto& ValOp : Comp.Value) {
                                            Call.Args->push_back(std::move(ValOp));
                                        }
                                    }
                                }

                                // Create operand with the call
                                PlannedOperand CallOp;
                                CallOp.Loc = Op.Loc;
                                CallOp.Expr = std::move(Call);
                                if (PlannedFunc->Returns) {
                                    CallOp.ResultType = *PlannedFunc->Returns;
                                }

                                // Apply any member access on the result
                                if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(CallOp.ResultType,
                                                                                 *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    CallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!CallOp.MemberAccess->empty()) {
                                        CallOp.ResultType = CallOp.MemberAccess->back().ResultType;
                                    }
                                }

                                Result.push_back(std::move(CallOp));
                                i++;  // Skip the tuple operand
                                continue;
                            }
                        }
                    }

                    // Check for package function call pattern (e.g., scaly.containers.test())
                    // First element is package name, path leads to function
                    if (TypeExpr->Name.size() >= 2 && std::holds_alternative<Tuple>(NextOp.Expr)) {
                        auto PkgIt = LoadedPackages.find(TypeExpr->Name[0]);
                        if (PkgIt != LoadedPackages.end()) {
                            // Look up the function in the package
                            const Function* PkgFunc = lookupPackageFunction(TypeExpr->Name);
                            if (PkgFunc) {
                                // Plan the arguments
                                Operand ArgsOp = NextOp;
                                ArgsOp.MemberAccess = nullptr;
                                auto PlannedArgs = planOperand(ArgsOp);
                                if (!PlannedArgs) {
                                    return PlannedArgs.takeError();
                                }

                                // Plan the function
                                auto PlannedFunc = planFunction(*PkgFunc, nullptr);
                                if (!PlannedFunc) {
                                    return PlannedFunc.takeError();
                                }

                                // Create the function call
                                PlannedCall Call;
                                Call.Loc = Op.Loc;
                                Call.Name = PkgFunc->Name;
                                Call.MangledName = PlannedFunc->MangledName;
                                Call.IsIntrinsic = false;
                                Call.IsOperator = false;
                                if (PlannedFunc->Returns) {
                                    Call.ResultType = *PlannedFunc->Returns;
                                }

                                // Build args from tuple
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (auto& Comp : TupleExpr->Components) {
                                        for (auto& ValOp : Comp.Value) {
                                            Call.Args->push_back(std::move(ValOp));
                                        }
                                    }
                                }

                                // Create operand with the call
                                PlannedOperand CallOp;
                                CallOp.Loc = Op.Loc;
                                CallOp.Expr = std::move(Call);
                                if (PlannedFunc->Returns) {
                                    CallOp.ResultType = *PlannedFunc->Returns;
                                }

                                // Apply any member access on the result
                                if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(CallOp.ResultType,
                                                                                 *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    CallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!CallOp.MemberAccess->empty()) {
                                        CallOp.ResultType = CallOp.MemberAccess->back().ResultType;
                                    }
                                }

                                Result.push_back(std::move(CallOp));
                                i++;  // Skip the tuple operand
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // Check for method call pattern on any operand with member access
        // e.g., (*page).reset() where (*page) is a grouped dereference expression
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];
            // Check if current op has member access and next is a tuple (args)
            if (Op.MemberAccess && !Op.MemberAccess->empty() &&
                std::holds_alternative<Tuple>(NextOp.Expr)) {
                // The last member in the access chain might be a method name
                const Type& MethodType = Op.MemberAccess->back();
                std::string MethodName = MethodType.Name.empty() ? "" : MethodType.Name[0];
                // Include any extensions in the method name path
                for (size_t j = 1; j < MethodType.Name.size(); ++j) {
                    MethodName = MethodType.Name[j];  // Use last segment as method name
                }

                // Plan the base operand without the last member access
                Operand BaseOp = Op;
                if (Op.MemberAccess->size() == 1) {
                    BaseOp.MemberAccess = nullptr;  // No more member access
                } else {
                    // Keep all but the last member access
                    BaseOp.MemberAccess = std::make_shared<std::vector<Type>>(
                        Op.MemberAccess->begin(), Op.MemberAccess->end() - 1);
                }

                auto PlannedBase = planOperand(BaseOp);
                if (!PlannedBase) {
                    return PlannedBase.takeError();
                }

                // Plan the arguments first to get their types for overload resolution
                Operand ArgsOp = NextOp;
                ArgsOp.MemberAccess = nullptr;
                auto PlannedArgs = planOperand(ArgsOp);
                if (!PlannedArgs) {
                    return PlannedArgs.takeError();
                }

                // Look up the method on the base type with overload resolution
                auto ArgTypes = extractArgTypes(*PlannedArgs);
                auto MethodMatch = lookupMethod(PlannedBase->ResultType, MethodName, ArgTypes, Op.Loc);
                if (MethodMatch) {
                    // Create method call with instance as first argument
                    PlannedCall Call;
                    Call.Loc = Op.Loc;
                    Call.Name = MethodName;
                    Call.MangledName = MethodMatch->MangledName;
                    Call.IsIntrinsic = false;
                    Call.IsOperator = false;
                    Call.ResultType = MethodMatch->ReturnType;

                    // Build args: [region (if any), instance, ...tuple_args]
                    Call.Args = std::make_shared<std::vector<PlannedOperand>>();

                    // Handle function# page parameter for method calls
                    if (MethodMatch->RequiresPageParam) {
                        Call.RequiresPageParam = true;
                        Call.Life = MethodType.Life;

                        if (auto* RefLife = std::get_if<ReferenceLifetime>(&MethodType.Life)) {
                            // ^name - pass explicit page as first argument
                            // Special case: ^this generates Page.get(this)
                            if (RefLife->Location == "this") {
                                auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                if (!PageGetResult) {
                                    return PageGetResult.takeError();
                                }
                                Call.Args->push_back(std::move(*PageGetResult));
                            } else {
                                auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                if (!RegionBinding) {
                                    return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                }
                                auto isPageType = [](const std::string &Name) {
                                    return Name == "Page" || Name == "scaly.memory.Page";
                                };
                                bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                     !RegionBinding->Type.Generics.empty() &&
                                                     isPageType(RegionBinding->Type.Generics[0].Name);
                                if (!isPagePointer) {
                                    return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                }

                                PlannedOperand RegionArg;
                                RegionArg.Loc = RefLife->Loc;
                                RegionArg.ResultType = RegionBinding->Type;
                                RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                Call.Args->push_back(std::move(RegionArg));
                            }
                        }
                        // For # lifetime, Emitter uses ReturnPage (rp)
                        // For $ lifetime, Emitter uses LocalPage
                        else if (std::holds_alternative<LocalLifetime>(MethodType.Life)) {
                            // Track that we need local page allocation
                            CurrentFunctionUsesLocalLifetime = true;
                            if (!ScopeInfoStack.empty()) {
                                ScopeInfoStack.back().HasLocalAllocations = true;
                            }
                        }
                        else if (!std::holds_alternative<CallLifetime>(MethodType.Life)) {
                            return makePlannerNotImplementedError(File, Op.Loc,
                                "method# '" + MethodName + "' must be called with #, $, or ^page syntax");
                        }
                    }
                    // If the method Type has a ReferenceLifetime but method doesn't have PageParameter,
                    // still pass the region (for legacy/init calls)
                    else if (auto* RefLife = std::get_if<ReferenceLifetime>(&MethodType.Life)) {
                        // Special case: ^this generates Page.get(this)
                        if (RefLife->Location == "this") {
                            auto PageGetResult = generatePageGetThis(RefLife->Loc);
                            if (!PageGetResult) {
                                return PageGetResult.takeError();
                            }
                            Call.Args->push_back(std::move(*PageGetResult));
                        } else {
                            auto RegionBinding = lookupLocalBinding(RefLife->Location);
                            if (!RegionBinding) {
                                return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                            }
                            // Validate that the referenced variable is a pointer[Page]
                            auto isPageType = [](const std::string &Name) {
                                return Name == "Page" || Name == "scaly.memory.Page";
                            };
                            bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                 !RegionBinding->Type.Generics.empty() &&
                                                 isPageType(RegionBinding->Type.Generics[0].Name);
                            if (!isPagePointer) {
                                return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                            }

                            PlannedOperand RegionArg;
                            RegionArg.Loc = RefLife->Loc;
                            RegionArg.ResultType = RegionBinding->Type;
                            RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                            Call.Args->push_back(std::move(RegionArg));
                        }
                    }

                    Call.Args->push_back(std::move(*PlannedBase));

                    // Add tuple elements as additional arguments
                    if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                        for (auto& Comp : TupleExpr->Components) {
                            for (auto& ValOp : Comp.Value) {
                                Call.Args->push_back(std::move(ValOp));
                            }
                        }
                    }

                    // Create operand with the call
                    PlannedOperand CallOp;
                    CallOp.Loc = Op.Loc;
                    CallOp.Expr = std::move(Call);
                    CallOp.ResultType = MethodMatch->ReturnType;

                    // Handle chained method calls: expr.method1().method2().method3()
                    auto ChainResult = processChainedMethodCalls(std::move(CallOp), ProcessedOps, i + 1);
                    if (!ChainResult) {
                        return ChainResult.takeError();
                    }

                    Result.push_back(std::move(ChainResult->CallOp));
                    i += 1 + ChainResult->ConsumedOperands;  // Skip consumed tuples
                    continue;
                }
                // If method not found, fall through to normal processing
            }
        }

        // Check for union variant constructor pattern: Union.Variant(value)
        // e.g., Result.Ok(42) or Option.Some(value)
        // Also handles generic unions: Option[int].Some(42)
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                // Case 1: Non-generic union - Name has both union and variant (e.g., Result.Ok)
                if (TypeExpr->Name.size() >= 2 && std::holds_alternative<Tuple>(NextOp.Expr)) {
                    // Look up the first name as a union
                    const std::string& UnionName = TypeExpr->Name[0];
                    const std::string& VariantName = TypeExpr->Name[1];

                    auto UnionIt = InstantiatedUnions.find(UnionName);
                    if (UnionIt != InstantiatedUnions.end()) {
                        const PlannedUnion& Union = UnionIt->second;

                        // Find the variant
                        const PlannedVariant* FoundVariant = nullptr;
                        for (const auto& Var : Union.Variants) {
                            if (Var.Name == VariantName) {
                                FoundVariant = &Var;
                                break;
                            }
                        }

                        if (FoundVariant) {
                            // Plan the tuple argument
                            Operand ArgsOp = NextOp;
                            ArgsOp.MemberAccess = nullptr;
                            auto PlannedArgs = planOperand(ArgsOp);
                            if (!PlannedArgs) {
                                return PlannedArgs.takeError();
                            }

                            // Extract the first argument value (for single-value variants)
                            std::shared_ptr<PlannedOperand> ArgValue;
                            if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                if (!TupleExpr->Components.empty() &&
                                    !TupleExpr->Components[0].Value.empty()) {
                                    ArgValue = std::make_shared<PlannedOperand>(
                                        std::move(TupleExpr->Components[0].Value.back()));
                                }
                            }

                            // Build the union type
                            PlannedType UnionType;
                            UnionType.Loc = Op.Loc;
                            UnionType.Name = UnionName;
                            UnionType.MangledName = Union.MangledName;

                            // Create the variant construction
                            PlannedVariantConstruction VarConstruct;
                            VarConstruct.Loc = Op.Loc;
                            VarConstruct.UnionType = UnionType;
                            VarConstruct.VariantName = VariantName;
                            VarConstruct.VariantTag = FoundVariant->Tag;
                            VarConstruct.Value = ArgValue;

                            PlannedOperand ResultOp;
                            ResultOp.Loc = Op.Loc;
                            ResultOp.ResultType = UnionType;
                            ResultOp.Expr = std::move(VarConstruct);

                            // Apply member access from the tuple operand (e.g., Some(5).value)
                            if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                auto MemberChain = resolveMemberAccessChain(
                                    UnionType, *NextOp.MemberAccess, NextOp.Loc);
                                if (!MemberChain) {
                                    return MemberChain.takeError();
                                }
                                ResultOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                    std::move(*MemberChain));
                                if (!ResultOp.MemberAccess->empty()) {
                                    ResultOp.ResultType = ResultOp.MemberAccess->back().ResultType;
                                }
                            }

                            Result.push_back(std::move(ResultOp));
                            i++;  // Skip the tuple operand
                            continue;
                        }
                    }
                }

                // Case 2: Generic union with member access - Option[int].Some(42)
                // TypeExpr has Name=["Option"], Generics=[int], MemberAccess=["Some"]
                if (TypeExpr->Name.size() == 1 && TypeExpr->Generics && !TypeExpr->Generics->empty() &&
                    Op.MemberAccess && !Op.MemberAccess->empty()) {
                    const std::string& UnionName = TypeExpr->Name[0];
                    const std::string& VariantName = Op.MemberAccess->at(0).Name[0];

                    // Look up the concept to check if it's a union
                    const Concept* Conc = lookupConcept(UnionName);
                    if (Conc && std::holds_alternative<Union>(Conc->Def)) {
                        // Resolve the generic union type (instantiate it)
                        auto ResolvedUnion = resolveType(*TypeExpr, Op.Loc);
                        if (!ResolvedUnion) {
                            return ResolvedUnion.takeError();
                        }
                        PlannedType UnionType = std::move(*ResolvedUnion);

                        // Look up the instantiated union
                        auto UnionIt = InstantiatedUnions.find(UnionType.Name);
                        if (UnionIt != InstantiatedUnions.end()) {
                            const PlannedUnion& Union = UnionIt->second;

                            // Find the variant
                            const PlannedVariant* FoundVariant = nullptr;
                            for (const auto& Var : Union.Variants) {
                                if (Var.Name == VariantName) {
                                    FoundVariant = &Var;
                                    break;
                                }
                            }

                            if (FoundVariant) {
                                // Check if next op is a tuple (value for the variant)
                                std::shared_ptr<PlannedOperand> ArgValue;
                                bool ConsumedTuple = false;

                                if (std::holds_alternative<Tuple>(NextOp.Expr)) {
                                    // Plan the tuple argument
                                    Operand ArgsOp = NextOp;
                                    ArgsOp.MemberAccess = nullptr;
                                    auto PlannedArgs = planOperand(ArgsOp);
                                    if (!PlannedArgs) {
                                        return PlannedArgs.takeError();
                                    }

                                    // Extract the first argument value
                                    if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                        if (!TupleExpr->Components.empty() &&
                                            !TupleExpr->Components[0].Value.empty()) {
                                            ArgValue = std::make_shared<PlannedOperand>(
                                                std::move(TupleExpr->Components[0].Value.back()));
                                        }
                                    }
                                    ConsumedTuple = true;
                                }
                                // Else: variant with no value (like None)

                                // Create the variant construction
                                PlannedVariantConstruction VarConstruct;
                                VarConstruct.Loc = Op.Loc;
                                VarConstruct.UnionType = UnionType;
                                VarConstruct.VariantName = VariantName;
                                VarConstruct.VariantTag = FoundVariant->Tag;
                                VarConstruct.Value = ArgValue;

                                PlannedOperand ResultOp;
                                ResultOp.Loc = Op.Loc;
                                ResultOp.ResultType = UnionType;
                                ResultOp.Expr = std::move(VarConstruct);

                                // Apply member access from the tuple operand (e.g., Some(42).value)
                                if (ConsumedTuple && NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(
                                        UnionType, *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    ResultOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!ResultOp.MemberAccess->empty()) {
                                        ResultOp.ResultType = ResultOp.MemberAccess->back().ResultType;
                                    }
                                }

                                Result.push_back(std::move(ResultOp));
                                if (ConsumedTuple) {
                                    i++;  // Skip the tuple operand
                                }
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // Case 3: Unit variant without tuple - Option.None
        // Name has both union and variant but no tuple follows
        if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
            if (TypeExpr->Name.size() >= 2) {
                const std::string& UnionName = TypeExpr->Name[0];
                const std::string& VariantName = TypeExpr->Name[1];

                auto UnionIt = InstantiatedUnions.find(UnionName);
                if (UnionIt != InstantiatedUnions.end()) {
                    const PlannedUnion& Union = UnionIt->second;

                    // Find the variant
                    const PlannedVariant* FoundVariant = nullptr;
                    for (const auto& Var : Union.Variants) {
                        if (Var.Name == VariantName) {
                            FoundVariant = &Var;
                            break;
                        }
                    }

                    if (FoundVariant && !FoundVariant->VarType) {
                        // Unit variant (no value type)
                        PlannedType UnionType;
                        UnionType.Loc = Op.Loc;
                        UnionType.Name = UnionName;
                        UnionType.MangledName = Union.MangledName;

                        PlannedVariantConstruction VarConstruct;
                        VarConstruct.Loc = Op.Loc;
                        VarConstruct.UnionType = UnionType;
                        VarConstruct.VariantName = VariantName;
                        VarConstruct.VariantTag = FoundVariant->Tag;
                        VarConstruct.Value = nullptr;  // No value for unit variant

                        PlannedOperand ResultOp;
                        ResultOp.Loc = Op.Loc;
                        ResultOp.ResultType = UnionType;
                        ResultOp.Expr = std::move(VarConstruct);

                        Result.push_back(std::move(ResultOp));
                        continue;
                    }
                }
            }
        }

        // Check for static method call pattern: Type.method() followed by Tuple
        // e.g., Page.allocate_page() or Foo.create(args)
        // Also handles fully qualified names: scaly.memory.Page.get()
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                // Type path with at least 2 segments: Type.method or module.Type.method
                if (TypeExpr->Name.size() >= 2 && std::holds_alternative<Tuple>(NextOp.Expr)) {
                    const std::string& MethodName = TypeExpr->Name.back();
                    // For qualified names like scaly.memory.Page.get, the type is the second-to-last segment
                    // For simple names like Page.get, the type is the first segment
                    const std::string& TypeName = TypeExpr->Name.size() > 2
                        ? TypeExpr->Name[TypeExpr->Name.size() - 2]
                        : TypeExpr->Name[0];

                    // Look up the type as a Concept
                    const Concept* Conc = lookupConcept(TypeName);
                    if (Conc) {
                        if (auto* ModelStruct = std::get_if<Structure>(&Conc->Def)) {
                            // Search for a static method (function without 'this' parameter)
                            const Function* StaticMethod = nullptr;
                            for (const auto& Member : ModelStruct->Members) {
                                if (auto* Func = std::get_if<Function>(&Member)) {
                                    if (Func->Name == MethodName) {
                                        // Check if it's a static method (no 'this' parameter)
                                        bool IsStatic = Func->Input.empty() ||
                                            !Func->Input[0].Name ||
                                            *Func->Input[0].Name != "this";
                                        if (IsStatic) {
                                            StaticMethod = Func;
                                            break;
                                        }
                                    }
                                }
                            }

                            if (StaticMethod) {
                                // Plan the arguments tuple
                                Operand ArgsOp = NextOp;
                                ArgsOp.MemberAccess = nullptr;
                                auto PlannedArgs = planOperand(ArgsOp);
                                if (!PlannedArgs) {
                                    return PlannedArgs.takeError();
                                }

                                // Extract argument types from the planned tuple
                                std::vector<PlannedType> ArgTypes;
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (const auto &Comp : TupleExpr->Components) {
                                        if (!Comp.Value.empty()) {
                                            ArgTypes.push_back(Comp.Value.back().ResultType);
                                        }
                                    }
                                }

                                // Plan the static method
                                auto PlannedFunc = planFunction(*StaticMethod, nullptr);
                                if (!PlannedFunc) {
                                    return PlannedFunc.takeError();
                                }

                                // Add to InstantiatedFunctions if not already present
                                if (InstantiatedFunctions.find(PlannedFunc->MangledName) == InstantiatedFunctions.end()) {
                                    InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;
                                }

                                // Build PlannedItems for mangling
                                std::vector<PlannedItem> ParamItems;
                                for (const auto& Inp : StaticMethod->Input) {
                                    PlannedItem Item;
                                    Item.Loc = Inp.Loc;
                                    Item.Name = Inp.Name;
                                    if (Inp.ItemType) {
                                        auto Resolved = resolveType(*Inp.ItemType, Inp.Loc);
                                        if (Resolved) {
                                            Item.ItemType = std::make_shared<PlannedType>(*Resolved);
                                        }
                                    }
                                    ParamItems.push_back(Item);
                                }

                                // Create PlannedCall for the static method
                                PlannedCall Call;
                                Call.Loc = Op.Loc;
                                Call.Name = MethodName;
                                Call.MangledName = PlannedFunc->MangledName;
                                Call.IsIntrinsic = false;
                                Call.IsOperator = false;

                                // Convert tuple components to call arguments
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (auto &Comp : TupleExpr->Components) {
                                        if (!Comp.Value.empty()) {
                                            Call.Args->push_back(std::move(Comp.Value.back()));
                                        }
                                    }
                                }

                                // Set return type
                                if (PlannedFunc->Returns) {
                                    Call.ResultType = *PlannedFunc->Returns;
                                } else {
                                    Call.ResultType.Name = "void";
                                    Call.ResultType.MangledName = "v";
                                }

                                PlannedOperand CallOp;
                                CallOp.Loc = Op.Loc;
                                CallOp.ResultType = Call.ResultType;
                                CallOp.Expr = std::move(Call);

                                // Handle member access on the result (e.g., Type.method().field)
                                if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(
                                        CallOp.ResultType, *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    CallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!CallOp.MemberAccess->empty()) {
                                        CallOp.ResultType = CallOp.MemberAccess->back().ResultType;
                                    }
                                }

                                Result.push_back(std::move(CallOp));
                                i++;  // Skip the tuple operand
                                continue;
                            }
                        }
                        // Also handle Namespace (define blocks) for functions like File.write_from_string
                        else if (auto* ModelNS = std::get_if<Namespace>(&Conc->Def)) {
                            // Search for a function in the namespace
                            const Function* NSFunction = nullptr;
                            for (const auto& Member : ModelNS->Members) {
                                if (auto* Func = std::get_if<Function>(&Member)) {
                                    if (Func->Name == MethodName) {
                                        NSFunction = Func;
                                        break;
                                    }
                                }
                            }

                            if (NSFunction) {
                                // Plan the arguments tuple
                                Operand ArgsOp = NextOp;
                                ArgsOp.MemberAccess = nullptr;
                                auto PlannedArgs = planOperand(ArgsOp);
                                if (!PlannedArgs) {
                                    return PlannedArgs.takeError();
                                }

                                // Extract argument types from the planned tuple
                                std::vector<PlannedType> ArgTypes;
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (const auto &Comp : TupleExpr->Components) {
                                        if (!Comp.Value.empty()) {
                                            ArgTypes.push_back(Comp.Value.back().ResultType);
                                        }
                                    }
                                }

                                // Plan the namespace function
                                auto PlannedFunc = planFunction(*NSFunction, nullptr);
                                if (!PlannedFunc) {
                                    return PlannedFunc.takeError();
                                }

                                // Add to InstantiatedFunctions if not already present
                                if (InstantiatedFunctions.find(PlannedFunc->MangledName) == InstantiatedFunctions.end()) {
                                    InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;
                                }

                                // Create PlannedCall for the namespace function
                                PlannedCall Call;
                                Call.Loc = Op.Loc;
                                Call.Name = MethodName;
                                Call.MangledName = PlannedFunc->MangledName;
                                Call.IsIntrinsic = false;
                                Call.IsOperator = false;

                                // Convert tuple components to call arguments
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (auto &Comp : TupleExpr->Components) {
                                        if (!Comp.Value.empty()) {
                                            Call.Args->push_back(std::move(Comp.Value.back()));
                                        }
                                    }
                                }

                                // Set return type
                                if (PlannedFunc->Returns) {
                                    Call.ResultType = *PlannedFunc->Returns;
                                } else {
                                    Call.ResultType.Name = "void";
                                    Call.ResultType.MangledName = "v";
                                }

                                PlannedOperand CallOp;
                                CallOp.Loc = Op.Loc;
                                CallOp.ResultType = Call.ResultType;
                                CallOp.Expr = std::move(Call);

                                // Handle member access on the result
                                if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(
                                        CallOp.ResultType, *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    CallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!CallOp.MemberAccess->empty()) {
                                        CallOp.ResultType = CallOp.MemberAccess->back().ResultType;
                                    }
                                }

                                Result.push_back(std::move(CallOp));
                                i++;  // Skip the tuple operand
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // Check for type constructor pattern: Type followed by Tuple
        // e.g., Point(3, 4) or Point(3, 4).x where Point is a struct type
        // Also handles generic types: Box[int](42) where Box[T] is a generic struct
        // Also handles qualified type names: scaly.containers.String(rp, "hello")
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];
            // Check if current op is a type that's a struct (with or without generics)
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                // Build full type name for lookup (handles both simple and qualified names)
                std::string FullTypeName;
                for (size_t j = 0; j < TypeExpr->Name.size(); ++j) {
                    if (j > 0) FullTypeName += ".";
                    FullTypeName += TypeExpr->Name[j];
                }
                const Concept* Conc = lookupConcept(FullTypeName);
                // If qualified name lookup fails, try just the last component (for use statements)
                if (!Conc && TypeExpr->Name.size() > 1) {
                    Conc = lookupConcept(TypeExpr->Name.back());
                }
                if (Conc && std::holds_alternative<Structure>(Conc->Def)) {
                        // Check if next op is a tuple (constructor args)
                        if (std::holds_alternative<Tuple>(NextOp.Expr)) {
                            // This is Type(args) or Type[T](args) - plan the tuple as a struct
                            // Plan the tuple (args) without its member access first
                            Operand ArgsOp = NextOp;
                            ArgsOp.MemberAccess = nullptr;  // Remove member access from args
                            auto PlannedArgs = planOperand(ArgsOp);
                            if (!PlannedArgs) {
                                return PlannedArgs.takeError();
                            }

                            // Resolve the type (handles both generic and non-generic)
                            // For generics, this triggers instantiation (e.g., Box[int] -> Box.int)
                            auto ResolvedType = resolveType(*TypeExpr, Op.Loc);
                            if (!ResolvedType) {
                                return ResolvedType.takeError();
                            }
                            PlannedType StructType = std::move(*ResolvedType);

                            // Extract argument types from the planned tuple
                            std::vector<PlannedType> ArgTypes;
                            if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                for (const auto &Comp : TupleExpr->Components) {
                                    if (!Comp.Value.empty()) {
                                        ArgTypes.push_back(Comp.Value.back().ResultType);
                                    }
                                }
                            }

                            // Check if there's a matching initializer
                            auto InitMatch = findInitializer(StructType, ArgTypes);
                            if (InitMatch) {
                                // Generate a PlannedCall to the initializer
                                PlannedCall Call;
                                Call.Loc = Op.Loc;
                                Call.Name = StructType.Name;
                                Call.MangledName = InitMatch->MangledName;
                                Call.IsIntrinsic = false;
                                Call.IsOperator = false;
                                Call.RequiresPageParam = InitMatch->RequiresPageParam;
                                Call.Life = TypeExpr->Life;  // Preserve lifetime for Emitter
                                // Convert tuple components to call arguments
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();

                                // Check if this is a page allocation (any lifetime suffix)
                                // $ = local page, # = caller page, ^name = named region
                                bool IsRegionAlloc = !std::holds_alternative<UnspecifiedLifetime>(TypeExpr->Life);

                                // Track if this function/scope uses local lifetime allocations
                                if (std::holds_alternative<LocalLifetime>(TypeExpr->Life)) {
                                    CurrentFunctionUsesLocalLifetime = true;
                                    if (!ScopeInfoStack.empty()) {
                                        ScopeInfoStack.back().HasLocalAllocations = true;
                                    }
                                }

                                // For ReferenceLifetime (^name), pass the region variable as first arg
                                if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                    // Special case: ^this generates Page.get(this)
                                    if (RefLife->Location == "this") {
                                        auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                        if (!PageGetResult) {
                                            return PageGetResult.takeError();
                                        }
                                        Call.Args->push_back(std::move(*PageGetResult));
                                    } else {
                                        auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                        if (!RegionBinding) {
                                            return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        // Validate that the referenced variable is a pointer[Page]
                                        auto isPageType = [](const std::string &Name) {
                                            return Name == "Page" || Name == "scaly.memory.Page";
                                        };
                                        bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                             !RegionBinding->Type.Generics.empty() &&
                                                             isPageType(RegionBinding->Type.Generics[0].Name);
                                        if (!isPagePointer) {
                                            return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                        }

                                        PlannedOperand RegionArg;
                                        RegionArg.Loc = RefLife->Loc;
                                        RegionArg.ResultType = RegionBinding->Type;
                                        RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                        Call.Args->push_back(std::move(RegionArg));
                                    }
                                }
                                // For $ and # lifetimes, Emitter uses CurrentRegion.LocalPage or ReturnPage

                                // Set result type: pointer[StructType] for page alloc, StructType for stack
                                PlannedType ResultType = StructType;
                                if (IsRegionAlloc) {
                                    ResultType.Name = "pointer";
                                    ResultType.MangledName = "P" + StructType.MangledName;
                                    ResultType.Generics = {StructType};
                                }
                                Call.ResultType = ResultType;

                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (auto &Comp : TupleExpr->Components) {
                                        if (!Comp.Value.empty()) {
                                            Call.Args->push_back(std::move(Comp.Value.back()));
                                        }
                                    }
                                }

                                PlannedOperand CallOperand;
                                CallOperand.Loc = Op.Loc;
                                CallOperand.ResultType = ResultType;
                                CallOperand.Expr = std::move(Call);

                                // Apply member access if present
                                if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(StructType,
                                                                                 *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    CallOperand.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!CallOperand.MemberAccess->empty()) {
                                        CallOperand.ResultType = CallOperand.MemberAccess->back().ResultType;
                                    }
                                }

                                Result.push_back(std::move(CallOperand));
                            } else {
                                // No explicit initializer - check if we can use property defaults
                                // Get the Structure to access property default values
                                const Structure& Struct = std::get<Structure>(Conc->Def);

                                // Get tuple component count
                                size_t ProvidedArgs = 0;
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    ProvidedArgs = TupleExpr->Components.size();
                                }

                                // Check if we need to fill in defaults
                                if (ProvidedArgs < Struct.Properties.size()) {
                                    // Verify ALL missing properties have defaults
                                    for (size_t pi = ProvidedArgs; pi < Struct.Properties.size(); ++pi) {
                                        const Property& Prop = Struct.Properties[pi];
                                        if (!Prop.Initializer) {
                                            // Property has no default - error
                                            return makePlannerNotImplementedError(File, Op.Loc,
                                                "no matching constructor for " + StructType.Name +
                                                ": property '" + Prop.Name + "' has no default value");
                                        }
                                    }

                                    // Fill in defaults for missing properties
                                    if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                        for (size_t pi = ProvidedArgs; pi < Struct.Properties.size(); ++pi) {
                                            const Property& Prop = Struct.Properties[pi];
                                            // Plan the default value expression
                                            auto PlannedDefault = planOperands(*Prop.Initializer);
                                            if (!PlannedDefault) {
                                                return PlannedDefault.takeError();
                                            }
                                            // Collapse to single operand
                                            if (!PlannedDefault->empty()) {
                                                auto Collapsed = collapseOperandSequence(std::move(*PlannedDefault));
                                                if (!Collapsed) {
                                                    return Collapsed.takeError();
                                                }
                                                PlannedComponent PC;
                                                PC.Loc = Prop.Loc;
                                                PC.Value.push_back(std::move(*Collapsed));
                                                TupleExpr->Components.push_back(std::move(PC));
                                            }
                                        }
                                    }
                                }

                                // Check if this is a page allocation (any lifetime suffix)
                                // $ = local page, # = caller page, ^name = named region
                                bool IsRegionAlloc = !std::holds_alternative<UnspecifiedLifetime>(TypeExpr->Life);

                                // Track if this function/scope uses local lifetime allocations
                                if (std::holds_alternative<LocalLifetime>(TypeExpr->Life)) {
                                    CurrentFunctionUsesLocalLifetime = true;
                                    if (!ScopeInfoStack.empty()) {
                                        ScopeInfoStack.back().HasLocalAllocations = true;
                                    }
                                }

                                // For ReferenceLifetime (^name), look up the region variable
                                PlannedOperand RegionArg;
                                if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                    // Special case: ^this generates Page.get(this)
                                    if (RefLife->Location == "this") {
                                        auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                        if (!PageGetResult) {
                                            return PageGetResult.takeError();
                                        }
                                        RegionArg = std::move(*PageGetResult);
                                    } else {
                                        auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                        if (!RegionBinding) {
                                            return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        // Validate that the referenced variable is a pointer[Page]
                                        auto isPageType = [](const std::string &Name) {
                                            return Name == "Page" || Name == "scaly.memory.Page";
                                        };
                                        bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                             !RegionBinding->Type.Generics.empty() &&
                                                             isPageType(RegionBinding->Type.Generics[0].Name);
                                        if (!isPagePointer) {
                                            return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        RegionArg.Loc = RefLife->Loc;
                                        RegionArg.ResultType = RegionBinding->Type;
                                        RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                    }
                                }
                                // For $ and # lifetimes, Emitter uses CurrentRegion.LocalPage or ReturnPage

                                // Set result type: pointer[StructType] for page alloc, StructType for stack
                                PlannedType ResultType = StructType;
                                if (IsRegionAlloc) {
                                    ResultType.Name = "pointer";
                                    ResultType.MangledName = "P" + StructType.MangledName;
                                    ResultType.Generics = {StructType};
                                }

                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    TupleExpr->TupleType = StructType;
                                    TupleExpr->IsRegionAlloc = IsRegionAlloc;
                                    TupleExpr->Life = TypeExpr->Life;  // Preserve lifetime for Emitter
                                    if (std::holds_alternative<ReferenceLifetime>(TypeExpr->Life)) {
                                        TupleExpr->RegionArg = std::make_shared<PlannedOperand>(std::move(RegionArg));
                                    }
                                }
                                PlannedArgs->ResultType = ResultType;

                                // Apply member access to the struct type if present
                                if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                    auto MemberChain = resolveMemberAccessChain(StructType,
                                                                                 *NextOp.MemberAccess, NextOp.Loc);
                                    if (!MemberChain) {
                                        return MemberChain.takeError();
                                    }
                                    PlannedArgs->MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                        std::move(*MemberChain));
                                    if (!PlannedArgs->MemberAccess->empty()) {
                                        PlannedArgs->ResultType = PlannedArgs->MemberAccess->back().ResultType;
                                    }
                                }

                                // Push the constructed struct (skip the type operand, use only the tuple)
                                Result.push_back(std::move(*PlannedArgs));
                            }

                            // Skip the next operand (we consumed it)
                            i++;
                            continue;
                        }
                }
            }
        }

        // Check for zero-argument struct constructor: Type$ or Type# without parentheses
        // e.g., Empty$ or Point$ where the struct has no required fields
        if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
            if (TypeExpr->Name.size() == 1 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
                // Only handle if lifetime is specified (not UnspecifiedLifetime)
                if (!std::holds_alternative<UnspecifiedLifetime>(TypeExpr->Life)) {
                    const Concept* Conc = lookupConcept(TypeExpr->Name[0]);
                    if (Conc && std::holds_alternative<Structure>(Conc->Def)) {
                        // Check that next operand is NOT a tuple (otherwise the above block handles it)
                        bool NextIsTuple = (i + 1 < ProcessedOps.size()) &&
                                           std::holds_alternative<Tuple>(ProcessedOps[i + 1].Expr);
                        if (!NextIsTuple) {
                            // This is Type$ or Type# without args - zero-argument constructor
                            PlannedType StructType;
                            StructType.Loc = Op.Loc;
                            StructType.Name = TypeExpr->Name[0];
                            StructType.MangledName = mangleType(StructType);

                            // Check if there's a matching zero-argument initializer
                            std::vector<PlannedType> ArgTypes;  // Empty
                            auto InitMatch = findInitializer(StructType, ArgTypes);

                            if (InitMatch) {
                                // Generate a PlannedCall to the initializer
                                PlannedCall Call;
                                Call.Loc = Op.Loc;
                                Call.Name = StructType.Name;
                                Call.MangledName = InitMatch->MangledName;
                                Call.IsIntrinsic = false;
                                Call.IsOperator = false;
                                Call.RequiresPageParam = InitMatch->RequiresPageParam;
                                Call.Life = TypeExpr->Life;
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();

                                // Page allocation based on lifetime
                                bool IsRegionAlloc = true;  // We already checked lifetime is not Unspecified

                                // Track if this function/scope uses local lifetime allocations
                                if (std::holds_alternative<LocalLifetime>(TypeExpr->Life)) {
                                    CurrentFunctionUsesLocalLifetime = true;
                                    if (!ScopeInfoStack.empty()) {
                                        ScopeInfoStack.back().HasLocalAllocations = true;
                                    }
                                }

                                // For ReferenceLifetime (^name), pass the region variable as first arg
                                if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                    // Special case: ^this generates Page.get(this)
                                    if (RefLife->Location == "this") {
                                        auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                        if (!PageGetResult) {
                                            return PageGetResult.takeError();
                                        }
                                        Call.Args->push_back(std::move(*PageGetResult));
                                    } else {
                                        auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                        if (!RegionBinding) {
                                            return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        // Validate that the referenced variable is a pointer[Page]
                                        auto isPageType = [](const std::string &Name) {
                                            return Name == "Page" || Name == "scaly.memory.Page";
                                        };
                                        bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                             !RegionBinding->Type.Generics.empty() &&
                                                             isPageType(RegionBinding->Type.Generics[0].Name);
                                        if (!isPagePointer) {
                                            return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        PlannedOperand RegionArg;
                                        RegionArg.Loc = RefLife->Loc;
                                        RegionArg.ResultType = RegionBinding->Type;
                                        RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                        Call.Args->push_back(std::move(RegionArg));
                                    }
                                }

                                // Set result type: pointer[StructType] for page alloc
                                PlannedType ResultType;
                                ResultType.Loc = StructType.Loc;
                                ResultType.Name = "pointer";
                                ResultType.MangledName = "P" + StructType.MangledName;
                                ResultType.Generics = {StructType};
                                Call.ResultType = ResultType;

                                PlannedOperand CallOperand;
                                CallOperand.Loc = Op.Loc;
                                CallOperand.ResultType = ResultType;
                                CallOperand.Expr = std::move(Call);

                                Result.push_back(std::move(CallOperand));
                                continue;
                            } else {
                                // No initializer - use direct tuple construction with empty components
                                PlannedTuple EmptyTuple;
                                EmptyTuple.Loc = Op.Loc;
                                EmptyTuple.TupleType = StructType;
                                EmptyTuple.IsRegionAlloc = true;
                                EmptyTuple.Life = TypeExpr->Life;

                                // Track if this function/scope uses local lifetime allocations
                                if (std::holds_alternative<LocalLifetime>(TypeExpr->Life)) {
                                    CurrentFunctionUsesLocalLifetime = true;
                                    if (!ScopeInfoStack.empty()) {
                                        ScopeInfoStack.back().HasLocalAllocations = true;
                                    }
                                }

                                if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                    // Special case: ^this generates Page.get(this)
                                    if (RefLife->Location == "this") {
                                        auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                        if (!PageGetResult) {
                                            return PageGetResult.takeError();
                                        }
                                        EmptyTuple.RegionArg = std::make_shared<PlannedOperand>(std::move(*PageGetResult));
                                    } else {
                                        auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                        if (!RegionBinding) {
                                            return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        // Validate that the referenced variable is a pointer[Page]
                                        auto isPageType = [](const std::string &Name) {
                                            return Name == "Page" || Name == "scaly.memory.Page";
                                        };
                                        bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                             !RegionBinding->Type.Generics.empty() &&
                                                             isPageType(RegionBinding->Type.Generics[0].Name);
                                        if (!isPagePointer) {
                                            return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                        }
                                        PlannedOperand RegionArg;
                                        RegionArg.Loc = RefLife->Loc;
                                        RegionArg.ResultType = RegionBinding->Type;
                                        RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                        EmptyTuple.RegionArg = std::make_shared<PlannedOperand>(std::move(RegionArg));
                                    }
                                }

                                PlannedType ResultType;
                                ResultType.Loc = StructType.Loc;
                                ResultType.Name = "pointer";
                                ResultType.MangledName = "P" + StructType.MangledName;
                                ResultType.Generics = {StructType};

                                PlannedOperand TupleOperand;
                                TupleOperand.Loc = Op.Loc;
                                TupleOperand.ResultType = ResultType;
                                TupleOperand.Expr = std::move(EmptyTuple);

                                Result.push_back(std::move(TupleOperand));
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // Check for generic union unit variant: Option[int].None (no tuple)
        // This is separate from Case 2 above because it doesn't require a next operand
        if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
            if (TypeExpr->Name.size() == 1 && TypeExpr->Generics && !TypeExpr->Generics->empty() &&
                Op.MemberAccess && !Op.MemberAccess->empty()) {
                const std::string& UnionName = TypeExpr->Name[0];
                const std::string& VariantName = Op.MemberAccess->at(0).Name[0];

                // Only handle if next operand is NOT a tuple (otherwise Case 2 above handles it)
                bool NextIsTuple = (i + 1 < ProcessedOps.size()) &&
                                   std::holds_alternative<Tuple>(ProcessedOps[i + 1].Expr);
                if (!NextIsTuple) {
                    // Look up the concept to check if it's a union
                    const Concept* Conc = lookupConcept(UnionName);
                    if (Conc && std::holds_alternative<Union>(Conc->Def)) {
                        // Resolve the generic union type (instantiate it)
                        auto ResolvedUnion = resolveType(*TypeExpr, Op.Loc);
                        if (!ResolvedUnion) {
                            return ResolvedUnion.takeError();
                        }
                        PlannedType UnionType = std::move(*ResolvedUnion);

                        // Look up the instantiated union
                        auto UnionIt = InstantiatedUnions.find(UnionType.Name);
                        if (UnionIt != InstantiatedUnions.end()) {
                            const PlannedUnion& Union = UnionIt->second;

                            // Find the variant
                            const PlannedVariant* FoundVariant = nullptr;
                            for (const auto& Var : Union.Variants) {
                                if (Var.Name == VariantName) {
                                    FoundVariant = &Var;
                                    break;
                                }
                            }

                            if (FoundVariant) {
                                // Create unit variant construction (no value)
                                PlannedVariantConstruction VarConstruct;
                                VarConstruct.Loc = Op.Loc;
                                VarConstruct.UnionType = UnionType;
                                VarConstruct.VariantName = VariantName;
                                VarConstruct.VariantTag = FoundVariant->Tag;
                                VarConstruct.Value = nullptr;  // Unit variant

                                PlannedOperand ResultOp;
                                ResultOp.Loc = Op.Loc;
                                ResultOp.ResultType = UnionType;
                                ResultOp.Expr = std::move(VarConstruct);

                                Result.push_back(std::move(ResultOp));
                                continue;
                            }
                        }
                    }
                }
            }
        }

        // Check for function call pattern: function_name followed by Tuple
        // e.g., seven() or add(3, 4)
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                if (TypeExpr->Name.size() == 1 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
                    const std::string& FuncName = TypeExpr->Name[0];
                    // Check if it's a known function (not a struct/concept)
                    if (isFunction(FuncName) && std::holds_alternative<Tuple>(NextOp.Expr)) {
                        // This is function(args) - create a function call

                        // Plan the arguments tuple
                        Operand ArgsOp = NextOp;
                        ArgsOp.MemberAccess = nullptr;
                        auto PlannedArgs = planOperand(ArgsOp);
                        if (!PlannedArgs) {
                            return PlannedArgs.takeError();
                        }

                        // Extract argument types from the planned tuple
                        std::vector<PlannedType> ArgTypes;
                        if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                            for (const auto &Comp : TupleExpr->Components) {
                                if (!Comp.Value.empty()) {
                                    ArgTypes.push_back(Comp.Value.back().ResultType);
                                }
                            }
                        }

                        // Resolve the function call to get return type and match function
                        auto FuncResult = resolveFunctionCall(FuncName, Op.Loc, ArgTypes);
                        if (!FuncResult) {
                            return FuncResult.takeError();
                        }

                        // Check if this is a method call (with or without implicit 'this')
                        // This happens when we call a sibling method from within the same struct
                        bool NeedsImplicitThis = false;
                        bool IsSiblingMethod = false;
                        bool IsNamespaceSibling = false;
                        const Function* MatchedFunc = nullptr;
                        if (!CurrentStructureName.empty()) {
                            // Get the current structure to verify membership
                            std::string BaseName = CurrentStructureName;
                            size_t DotPos = BaseName.find('.');
                            if (DotPos != std::string::npos) {
                                BaseName = BaseName.substr(0, DotPos);
                            }
                            const Concept* StructConc = lookupConcept(BaseName);
                            const Structure* CurrentStruct = nullptr;
                            if (StructConc && std::holds_alternative<Structure>(StructConc->Def)) {
                                CurrentStruct = &std::get<Structure>(StructConc->Def);
                            }

                            auto Candidates = lookupFunction(FuncName);
                            for (const Function* Func : Candidates) {
                                // Verify this function is actually a member of the current structure
                                bool IsStructMember = false;
                                if (CurrentStruct) {
                                    for (const auto& Member : CurrentStruct->Members) {
                                        if (auto* StructFunc = std::get_if<Function>(&Member)) {
                                            if (StructFunc == Func) {
                                                IsStructMember = true;
                                                break;
                                            }
                                        }
                                    }
                                }

                                if (!IsStructMember) {
                                    // Not a struct member, just track as potential match
                                    if (!MatchedFunc && Func->Parameters.empty() &&
                                        Func->Input.size() == ArgTypes.size()) {
                                        MatchedFunc = Func;
                                    }
                                    continue;
                                }

                                // Check if this function takes 'this' as first param
                                if (Func->Parameters.empty() &&
                                    !Func->Input.empty() &&
                                    Func->Input[0].Name &&
                                    *Func->Input[0].Name == "this" &&
                                    Func->Input.size() == ArgTypes.size() + 1) {
                                    NeedsImplicitThis = true;
                                    IsSiblingMethod = true;
                                    MatchedFunc = Func;
                                    break;
                                }
                                // Check if this is a static method (no 'this', but still a sibling)
                                else if (Func->Parameters.empty() &&
                                         Func->Input.size() == ArgTypes.size()) {
                                    IsSiblingMethod = true;
                                    MatchedFunc = Func;
                                    // Don't break - prefer 'this' variant if it exists
                                }
                            }
                        }
                        // Check if this is a sibling function in the same namespace (define block)
                        if (!IsSiblingMethod && !CurrentNamespaceName.empty() && CurrentNamespace) {
                            auto Candidates = lookupFunction(FuncName);
                            for (const Function* Func : Candidates) {
                                // Check if this function has matching parameter count (no 'this')
                                if (Func->Parameters.empty() &&
                                    Func->Input.size() == ArgTypes.size()) {
                                    // Verify the function is actually in the current namespace
                                    // by checking if it's in CurrentNamespace->Members
                                    bool InNamespace = false;
                                    for (const auto& Member : CurrentNamespace->Members) {
                                        if (auto* NSFunc = std::get_if<Function>(&Member)) {
                                            if (NSFunc == Func) {
                                                InNamespace = true;
                                                break;
                                            }
                                        }
                                    }
                                    if (InNamespace) {
                                        IsNamespaceSibling = true;
                                        MatchedFunc = Func;
                                        break;
                                    }
                                    // If not in namespace, still use as MatchedFunc but not as namespace sibling
                                    if (!MatchedFunc) {
                                        MatchedFunc = Func;
                                    }
                                }
                            }
                        }

                        // Build PlannedItems for mangling
                        // Use the matched function's declared parameter types, not argument types
                        // This ensures correct mangling when integer literals are passed
                        // (e.g., passing '0' where size_t is expected)
                        std::vector<PlannedItem> ParamItems;
                        if (MatchedFunc) {
                            // Use declared parameter types from the matched function
                            for (const auto& Inp : MatchedFunc->Input) {
                                PlannedItem Item;
                                Item.Loc = Inp.Loc;
                                Item.Name = Inp.Name;
                                if (Inp.ItemType) {
                                    auto Resolved = resolveType(*Inp.ItemType, Inp.Loc);
                                    if (Resolved) {
                                        Item.ItemType = std::make_shared<PlannedType>(*Resolved);
                                    }
                                }
                                ParamItems.push_back(Item);
                            }
                        } else {
                            // Fallback: build from argument types (for unmatched functions)
                            if (NeedsImplicitThis) {
                                // Add 'this' type for mangling
                                PlannedItem ThisItem;
                                ThisItem.Name = std::make_shared<std::string>("this");
                                std::string BaseName = CurrentStructureName;
                                size_t DotPos = BaseName.find('.');
                                if (DotPos != std::string::npos) {
                                    BaseName = BaseName.substr(0, DotPos);
                                }
                                PlannedType ThisType;
                                ThisType.Name = BaseName;
                                ThisType.MangledName = mangleType(ThisType);
                                ThisItem.ItemType = std::make_shared<PlannedType>(ThisType);
                                ParamItems.push_back(ThisItem);
                            }
                            for (const auto& ArgType : ArgTypes) {
                                PlannedItem Item;
                                Item.ItemType = std::make_shared<PlannedType>(ArgType);
                                ParamItems.push_back(Item);
                            }
                        }

                        // Generate mangled name - if it's a sibling method, use parent type
                        // For extern functions, use the unmangled C name
                        std::string MangledName;

                        // Check if this is an extern function (should use unmangled name)
                        bool IsExtern = false;
                        if (MatchedFunc && std::holds_alternative<ExternImpl>(MatchedFunc->Impl)) {
                            IsExtern = true;
                        } else if (!IsSiblingMethod) {
                            // Check top-level functions for extern
                            auto Candidates = lookupFunction(FuncName);
                            for (const Function* Func : Candidates) {
                                if (Func->Input.size() == ArgTypes.size() &&
                                    std::holds_alternative<ExternImpl>(Func->Impl)) {
                                    IsExtern = true;
                                    break;
                                }
                            }
                        }

                        if (IsExtern) {
                            // Extern C functions use unmangled name
                            MangledName = FuncName;
                        } else if (IsSiblingMethod) {
                            // Look up the current structure to get its mangled name
                            auto StructIt = InstantiatedStructures.find(CurrentStructureName);
                            if (StructIt != InstantiatedStructures.end()) {
                                // Use the instantiated structure's mangled name directly
                                // The MangledName already encodes generic type arguments correctly
                                PlannedType ParentType;
                                ParentType.Name = StructIt->second.Name;
                                ParentType.MangledName = StructIt->second.MangledName;
                                // Don't set Generics here - we use MangledName directly in mangleFunction
                                MangledName = mangleFunction(FuncName, ParamItems, &ParentType);
                            } else {
                                // Fallback for non-generic structures
                                std::string BaseName = CurrentStructureName;
                                size_t DotPos = BaseName.find('.');
                                if (DotPos != std::string::npos) {
                                    BaseName = BaseName.substr(0, DotPos);
                                }
                                PlannedType ParentType;
                                ParentType.Name = BaseName;
                                ParentType.MangledName = encodeName(BaseName);
                                MangledName = mangleFunction(FuncName, ParamItems, &ParentType);
                            }
                        } else if (IsNamespaceSibling) {
                            // Sibling function in the same namespace - use namespace prefix
                            PlannedType ParentType;
                            ParentType.Name = CurrentNamespaceName;
                            ParentType.MangledName = encodeName(CurrentNamespaceName);
                            MangledName = mangleFunction(FuncName, ParamItems, &ParentType);
                        } else {
                            MangledName = mangleFunction(FuncName, ParamItems, nullptr);
                        }

                        // Create the function call
                        PlannedCall Call;
                        Call.Loc = Op.Loc;
                        Call.Name = FuncName;
                        Call.MangledName = MangledName;
                        Call.IsIntrinsic = false;
                        Call.IsOperator = false;
                        Call.ResultType = std::move(*FuncResult);

                        // Convert tuple components to call arguments
                        Call.Args = std::make_shared<std::vector<PlannedOperand>>();

                        // Handle function# page parameter
                        if (MatchedFunc && MatchedFunc->PageParameter) {
                            // Function requires a page parameter - check call site lifetime
                            Call.RequiresPageParam = true;
                            Call.Life = TypeExpr->Life;

                            if (auto* RefLife = std::get_if<ReferenceLifetime>(&TypeExpr->Life)) {
                                // ^name - pass explicit page as first argument
                                // Special case: ^this generates Page.get(this)
                                if (RefLife->Location == "this") {
                                    auto PageGetResult = generatePageGetThis(RefLife->Loc);
                                    if (!PageGetResult) {
                                        return PageGetResult.takeError();
                                    }
                                    Call.Args->push_back(std::move(*PageGetResult));
                                } else {
                                    auto RegionBinding = lookupLocalBinding(RefLife->Location);
                                    if (!RegionBinding) {
                                        return makeUndefinedSymbolError(File, RefLife->Loc, RefLife->Location);
                                    }
                                    // Validate that the referenced variable is a pointer[Page]
                                    auto isPageType = [](const std::string &Name) {
                                        return Name == "Page" || Name == "scaly.memory.Page";
                                    };
                                    bool isPagePointer = RegionBinding->Type.Name == "pointer" &&
                                                         !RegionBinding->Type.Generics.empty() &&
                                                         isPageType(RegionBinding->Type.Generics[0].Name);
                                    if (!isPagePointer) {
                                        return makeStackLifetimeError(File, RefLife->Loc, RefLife->Location);
                                    }

                                    PlannedOperand RegionArg;
                                    RegionArg.Loc = RefLife->Loc;
                                    RegionArg.ResultType = RegionBinding->Type;
                                    RegionArg.Expr = PlannedVariable{RefLife->Loc, RefLife->Location, RegionBinding->Type, RegionBinding->IsMutable};
                                    Call.Args->push_back(std::move(RegionArg));
                                }
                            }
                            // For # lifetime, Emitter uses ReturnPage (rp)
                            // For $ lifetime, Emitter uses LocalPage
                            // For unspecified, error - function# must be called with #, $, or ^name
                            else if (std::holds_alternative<LocalLifetime>(TypeExpr->Life)) {
                                // Track that we need local page allocation
                                CurrentFunctionUsesLocalLifetime = true;
                                if (!ScopeInfoStack.empty()) {
                                    ScopeInfoStack.back().HasLocalAllocations = true;
                                }
                            }
                            else if (!std::holds_alternative<CallLifetime>(TypeExpr->Life)) {
                                return makePlannerNotImplementedError(File, Op.Loc,
                                    "function# '" + FuncName + "' must be called with #, $, or ^page syntax");
                            }
                        }

                        // Add implicit 'this' as first argument if needed
                        if (NeedsImplicitThis) {
                            PlannedOperand ThisOp;
                            ThisOp.Loc = Op.Loc;
                            PlannedVariable ThisVar;
                            ThisVar.Loc = Op.Loc;
                            ThisVar.Name = "this";
                            // Look up the current structure to get its type info
                            auto StructIt = InstantiatedStructures.find(CurrentStructureName);
                            if (StructIt != InstantiatedStructures.end()) {
                                ThisVar.VariableType.Name = StructIt->second.Name;
                                ThisVar.VariableType.MangledName = StructIt->second.MangledName;
                                // Generics are encoded in MangledName, don't need to set them
                            } else {
                                std::string BaseName = CurrentStructureName;
                                size_t DotPos = BaseName.find('.');
                                if (DotPos != std::string::npos) {
                                    BaseName = BaseName.substr(0, DotPos);
                                }
                                ThisVar.VariableType.Name = BaseName;
                                ThisVar.VariableType.MangledName = encodeName(BaseName);
                            }
                            ThisOp.ResultType = ThisVar.VariableType;
                            ThisOp.Expr = std::move(ThisVar);
                            Call.Args->push_back(std::move(ThisOp));
                        }

                        if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                            for (auto &Comp : TupleExpr->Components) {
                                if (!Comp.Value.empty()) {
                                    Call.Args->push_back(std::move(Comp.Value.back()));
                                }
                            }
                        }

                        PlannedOperand CallOperand;
                        CallOperand.Loc = Op.Loc;
                        CallOperand.ResultType = Call.ResultType;
                        CallOperand.Expr = std::move(Call);

                        // Apply member access if present on the result
                        if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                            auto MemberChain = resolveMemberAccessChain(CallOperand.ResultType,
                                                                         *NextOp.MemberAccess, NextOp.Loc);
                            if (!MemberChain) {
                                return MemberChain.takeError();
                            }
                            CallOperand.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                std::move(*MemberChain));
                            if (!CallOperand.MemberAccess->empty()) {
                                const auto& LastMember = CallOperand.MemberAccess->back();

                                // Check if the last member is a method and there's another tuple following
                                // This handles chained method calls: func(args).method(more_args)
                                if (LastMember.IsMethod && i + 2 < ProcessedOps.size()) {
                                    const auto& MethodArgsOp = ProcessedOps[i + 2];
                                    if (std::holds_alternative<Tuple>(MethodArgsOp.Expr)) {
                                        // Chained method call detected
                                        std::string MethodName = LastMember.Name;
                                        PlannedType InstanceType = LastMember.ParentType;

                                        // Remove the method from member access chain
                                        CallOperand.MemberAccess->pop_back();
                                        if (CallOperand.MemberAccess->empty()) {
                                            CallOperand.MemberAccess = nullptr;
                                        } else {
                                            CallOperand.ResultType = CallOperand.MemberAccess->back().ResultType;
                                        }
                                        // Reset ResultType to the instance type (what the method is called on)
                                        CallOperand.ResultType = InstanceType;

                                        // Plan the method arguments
                                        Operand ArgsOp = MethodArgsOp;
                                        ArgsOp.MemberAccess = nullptr;
                                        auto PlannedMethodArgs = planOperand(ArgsOp);
                                        if (!PlannedMethodArgs) {
                                            return PlannedMethodArgs.takeError();
                                        }

                                        // Look up the method on the instance type
                                        auto MethodArgTypes = extractArgTypes(*PlannedMethodArgs);
                                        auto MethodMatch = lookupMethod(InstanceType, MethodName, MethodArgTypes, MethodArgsOp.Loc);
                                        if (MethodMatch) {
                                            // Create method call
                                            PlannedCall MethodCall;
                                            MethodCall.Loc = MethodArgsOp.Loc;
                                            MethodCall.Name = MethodName;
                                            MethodCall.MangledName = MethodMatch->MangledName;
                                            MethodCall.IsIntrinsic = false;
                                            MethodCall.IsOperator = false;
                                            MethodCall.ResultType = MethodMatch->ReturnType;

                                            MethodCall.Args = std::make_shared<std::vector<PlannedOperand>>();

                                            // First argument is the instance (the CallOperand)
                                            MethodCall.Args->push_back(std::move(CallOperand));

                                            // Add remaining arguments from the tuple
                                            if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedMethodArgs->Expr)) {
                                                for (auto& Comp : TupleExpr->Components) {
                                                    for (auto& ValOp : Comp.Value) {
                                                        MethodCall.Args->push_back(std::move(ValOp));
                                                    }
                                                }
                                            }

                                            // Apply any member access on the method result (from MethodArgsOp)
                                            PlannedOperand MethodCallOp;
                                            MethodCallOp.Loc = MethodArgsOp.Loc;
                                            MethodCallOp.ResultType = MethodMatch->ReturnType;
                                            MethodCallOp.Expr = std::move(MethodCall);

                                            if (MethodArgsOp.MemberAccess && !MethodArgsOp.MemberAccess->empty()) {
                                                auto ChainedMemberChain = resolveMemberAccessChain(MethodCallOp.ResultType,
                                                                                                   *MethodArgsOp.MemberAccess, MethodArgsOp.Loc);
                                                if (!ChainedMemberChain) {
                                                    return ChainedMemberChain.takeError();
                                                }
                                                MethodCallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                                    std::move(*ChainedMemberChain));
                                                if (!MethodCallOp.MemberAccess->empty()) {
                                                    MethodCallOp.ResultType = MethodCallOp.MemberAccess->back().ResultType;
                                                }
                                            }

                                            Result.push_back(std::move(MethodCallOp));
                                            i += 2;  // Skip both tuples
                                            continue;
                                        }
                                    }
                                }

                                CallOperand.ResultType = CallOperand.MemberAccess->back().ResultType;
                            }
                        }

                        Result.push_back(std::move(CallOperand));
                        i++;  // Skip the tuple operand
                        continue;
                    }
                }
            }
        }

        auto Planned = planOperand(Op);
        if (!Planned) {
            return Planned.takeError();
        }

        // Check for subscript operator pattern: operand followed by Matrix
        // e.g., (*vector)[1] where (*vector) is the operand and [1] is the Matrix
        // This is NOT generic instantiation (which was handled in pre-processing)
        if (i + 1 < ProcessedOps.size()) {
            const auto &NextOp = ProcessedOps[i + 1];
            if (auto* MatrixExpr = std::get_if<Matrix>(&NextOp.Expr)) {
                // Check if this looks like a subscript (Matrix contains non-type expressions)
                // Generic instantiation was already handled in pre-processing
                bool IsSubscript = false;
                for (const auto& Row : MatrixExpr->Operations) {
                    for (const auto& Op : Row) {
                        // If the expression is not a simple type name, it's likely a subscript index
                        if (auto* T = std::get_if<Type>(&Op.Expr)) {
                            // Check if it's a literal or variable (subscript) vs type name
                            if (T->Name.size() == 1) {
                                auto Bind = lookupLocalBinding(T->Name[0]);
                                if (Bind) {
                                    IsSubscript = true;
                                    break;
                                }
                            }
                        } else if (std::holds_alternative<Constant>(Op.Expr)) {
                            IsSubscript = true;
                            break;
                        } else if (std::holds_alternative<Tuple>(Op.Expr)) {
                            // Expression like String(rp, "key") - definitely subscript
                            IsSubscript = true;
                            break;
                        }
                    }
                    if (IsSubscript) break;
                }

                if (IsSubscript) {
                    // Look up operator[] on the result type
                    PlannedType& LeftType = Planned->ResultType;

                    // Plan the index expression (from Matrix)
                    // The row may contain multiple operands forming a pattern like String(rp, "using")
                    // We need to plan the entire row to recognize constructor calls
                    if (!MatrixExpr->Operations.empty() && !MatrixExpr->Operations[0].empty()) {
                        const auto& IndexRow = MatrixExpr->Operations[0];

                        // Plan the entire row as a sequence of operands
                        auto PlannedRow = planOperands(IndexRow);
                        if (!PlannedRow) {
                            return PlannedRow.takeError();
                        }

                        // The result should be a single operand after pattern recognition
                        if (PlannedRow->empty()) {
                            return llvm::make_error<llvm::StringError>(
                                "Empty subscript index",
                                llvm::inconvertibleErrorCode()
                            );
                        }

                        // Take the last operand (result of pattern recognition)
                        PlannedOperand PlannedIndex = std::move(PlannedRow->back());

                        // Look for operator[] on the left type
                        auto OpMatch = findSubscriptOperator(LeftType, PlannedIndex.ResultType, NextOp.Loc);
                        if (OpMatch) {
                            // Create the operator call
                            PlannedCall Call;
                            Call.Loc = NextOp.Loc;
                            Call.Name = "[]";
                            Call.MangledName = OpMatch->MangledName;
                            Call.IsIntrinsic = false;
                            Call.IsOperator = true;
                            Call.ResultType = OpMatch->ResultType;

                            // Build args: [left, index]
                            Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                            Call.Args->push_back(std::move(*Planned));
                            Call.Args->push_back(std::move(PlannedIndex));

                            PlannedOperand CallOp;
                            CallOp.Loc = NextOp.Loc;
                            CallOp.Expr = std::move(Call);
                            CallOp.ResultType = OpMatch->ResultType;

                            // Apply any member access from the Matrix operand
                            if (NextOp.MemberAccess && !NextOp.MemberAccess->empty()) {
                                auto MemberChain = resolveMemberAccessChain(CallOp.ResultType,
                                                                             *NextOp.MemberAccess, NextOp.Loc);
                                if (!MemberChain) {
                                    return MemberChain.takeError();
                                }
                                CallOp.MemberAccess = std::make_shared<std::vector<PlannedMemberAccess>>(
                                    std::move(*MemberChain));
                                if (!CallOp.MemberAccess->empty()) {
                                    CallOp.ResultType = CallOp.MemberAccess->back().ResultType;
                                }
                            }

                            Result.push_back(std::move(CallOp));
                            i++;  // Skip the Matrix operand
                            continue;
                        }
                    }
                }
            }
        }

        Result.push_back(std::move(*Planned));
    }

    return Result;
}

llvm::Expected<PlannedExpression> Planner::planExpression(const Expression &Expr) {
    return std::visit([this](const auto &E) -> llvm::Expected<PlannedExpression> {
        using T = std::decay_t<decltype(E)>;

        if constexpr (std::is_same_v<T, Constant>) {
            return PlannedConstant(E);
        }
        else if constexpr (std::is_same_v<T, Type>) {
            // Check for boolean literals first (true/false are parsed as Type names)
            if (E.Name.size() == 1 && (!E.Generics || E.Generics->empty())) {
                const std::string &Name = E.Name[0];
                if (Name == "true") {
                    return PlannedConstant(BooleanConstant{E.Loc, true});
                }
                if (Name == "false") {
                    return PlannedConstant(BooleanConstant{E.Loc, false});
                }
                if (Name == "null") {
                    return PlannedConstant(NullConstant{E.Loc});
                }

                // Check if it's a local variable reference
                auto LocalBind = lookupLocalBinding(Name);
                if (LocalBind) {
                    PlannedVariable Var;
                    Var.Loc = E.Loc;
                    Var.Name = Name;
                    Var.VariableType = LocalBind->Type;
                    Var.IsMutable = LocalBind->IsMutable;
                    // Note: 'this' is stored as pointer[T] in scope - keep it that way
                    // Type compatibility will handle pointer[T] -> T conversion when needed
                    return PlannedExpression(Var);
                }

                // Check if it's a structure property access (when inside a method via 'this')
                if (CurrentStructureProperties) {
                    auto ThisType = lookupLocal("this");
                    std::string BaseStructName = CurrentStructureName;
                    size_t DotPos = BaseStructName.find('.');
                    if (DotPos != std::string::npos) {
                        BaseStructName = BaseStructName.substr(0, DotPos);
                    }
                    std::string ThisTypeName = ThisType ? ThisType->Name : "";
                    if (ThisType && ThisType->Name == "pointer" && !ThisType->Generics.empty()) {
                        ThisTypeName = ThisType->Generics[0].Name;
                    }
                    if (ThisType && (ThisTypeName == CurrentStructureName || ThisTypeName == BaseStructName)) {
                        for (const auto &Prop : *CurrentStructureProperties) {
                            if (Prop.Name == Name) {
                                // Property found - create an Operand that represents this.property
                                // and plan it through planOperand which handles member access properly
                                Operand PropOp;
                                PropOp.Loc = E.Loc;
                                Type ThisTypeExpr;
                                ThisTypeExpr.Loc = E.Loc;
                                ThisTypeExpr.Name = {"this"};
                                ThisTypeExpr.Generics = nullptr;
                                ThisTypeExpr.Life = UnspecifiedLifetime{};
                                PropOp.Expr = ThisTypeExpr;

                                // Add property as member access
                                auto MemberList = std::make_shared<std::vector<Type>>();
                                Type MemberType;
                                MemberType.Loc = E.Loc;
                                MemberType.Name = {Name};
                                MemberType.Generics = nullptr;
                                MemberType.Life = UnspecifiedLifetime{};
                                MemberList->push_back(MemberType);
                                PropOp.MemberAccess = MemberList;

                                auto PlannedOp = planOperand(PropOp);
                                if (!PlannedOp) {
                                    return PlannedOp.takeError();
                                }
                                return PlannedOp->Expr;
                            }
                        }
                    }
                }
            }

            // This could be a type reference (not a variable)
            auto Resolved = resolveNameOrVariable(E);
            if (!Resolved) {
                return Resolved.takeError();
            }
            return PlannedExpression(*Resolved);
        }
        else if constexpr (std::is_same_v<T, Tuple>) {
            auto Planned = planTuple(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Matrix>) {
            auto Planned = planMatrix(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Block>) {
            auto Planned = planBlock(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, If>) {
            auto Planned = planIf(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Match>) {
            auto Planned = planMatch(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Choose>) {
            auto Planned = planChoose(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, For>) {
            auto Planned = planFor(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, While>) {
            auto Planned = planWhile(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Try>) {
            auto Planned = planTry(E);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedExpression(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, SizeOf>) {
            PlannedSizeOf PSO;
            PSO.Loc = E.Loc;
            auto Resolved = resolveType(E.SizedType, E.Loc);
            if (!Resolved) {
                return Resolved.takeError();
            }
            PSO.SizedType = std::move(*Resolved);

            // Compute size based on type
            const std::string &TypeName = PSO.SizedType.Name;
            if (TypeName == "bool" || TypeName == "i1") {
                PSO.Size = 1;
            } else if (TypeName == "i8" || TypeName == "u8" || TypeName == "char") {
                PSO.Size = 1;
            } else if (TypeName == "i16" || TypeName == "u16") {
                PSO.Size = 2;
            } else if (TypeName == "i32" || TypeName == "u32" || TypeName == "float" || TypeName == "f32") {
                PSO.Size = 4;
            } else if (TypeName == "i64" || TypeName == "u64" || TypeName == "int" || TypeName == "uint" ||
                       TypeName == "double" || TypeName == "f64" || TypeName == "size_t" || TypeName == "ptr") {
                PSO.Size = 8;
            } else {
                // For structures and other types, look up in caches
                auto StructIt = InstantiatedStructures.find(PSO.SizedType.MangledName);
                if (StructIt != InstantiatedStructures.end()) {
                    PSO.Size = StructIt->second.Size;
                } else {
                    auto UnionIt = InstantiatedUnions.find(PSO.SizedType.MangledName);
                    if (UnionIt != InstantiatedUnions.end()) {
                        PSO.Size = UnionIt->second.Size;
                    } else {
                        PSO.Size = 0;  // Unknown type size
                    }
                }
            }
            return PlannedExpression(std::move(PSO));
        }
        else if constexpr (std::is_same_v<T, AlignOf>) {
            PlannedAlignOf PAO;
            PAO.Loc = E.Loc;
            auto Resolved = resolveType(E.AlignedType, E.Loc);
            if (!Resolved) {
                return Resolved.takeError();
            }
            PAO.AlignedType = std::move(*Resolved);

            // Compute alignment based on type
            const std::string &TypeName = PAO.AlignedType.Name;
            if (TypeName == "bool" || TypeName == "i1") {
                PAO.Alignment = 1;
            } else if (TypeName == "i8" || TypeName == "u8" || TypeName == "char") {
                PAO.Alignment = 1;
            } else if (TypeName == "i16" || TypeName == "u16") {
                PAO.Alignment = 2;
            } else if (TypeName == "i32" || TypeName == "u32" || TypeName == "float" || TypeName == "f32") {
                PAO.Alignment = 4;
            } else if (TypeName == "i64" || TypeName == "u64" || TypeName == "int" || TypeName == "uint" ||
                       TypeName == "double" || TypeName == "f64" || TypeName == "size_t" || TypeName == "ptr") {
                PAO.Alignment = 8;
            } else {
                // For structures and other types, look up in caches
                auto StructIt = InstantiatedStructures.find(PAO.AlignedType.MangledName);
                if (StructIt != InstantiatedStructures.end()) {
                    PAO.Alignment = StructIt->second.Alignment;
                } else {
                    auto UnionIt = InstantiatedUnions.find(PAO.AlignedType.MangledName);
                    if (UnionIt != InstantiatedUnions.end()) {
                        PAO.Alignment = UnionIt->second.Alignment;
                    } else {
                        PAO.Alignment = 1;  // Unknown type alignment defaults to 1
                    }
                }
            }
            return PlannedExpression(std::move(PAO));
        }
        else if constexpr (std::is_same_v<T, Is>) {
            PlannedIs PI;
            PI.Loc = E.Loc;
            // Check for "is null" - null check on optional types
            if (E.Name.size() == 1 && E.Name[0] == "null") {
                PI.IsNullCheck = true;
                // TestType not used for null checks
                return PlannedExpression(std::move(PI));
            }
            // Is has Name (vector<string>), not TestType
            auto Resolved = resolveTypePath(E.Name, E.Loc);
            if (!Resolved) {
                return Resolved.takeError();
            }
            PI.TestType = std::move(*Resolved);
            return PlannedExpression(std::move(PI));
        }
        else if constexpr (std::is_same_v<T, As>) {
            PlannedAs PA;
            PA.Loc = E.Loc;
            // Resolve the target type
            auto Resolved = resolveType(E.TargetType, E.Loc);
            if (!Resolved) {
                return Resolved.takeError();
            }
            PA.TargetType = std::move(*Resolved);
            // Note: Value will be populated later when combining with context
            return PlannedExpression(std::move(PA));
        }
        else {
            // Shouldn't reach here
            return PlannedExpression(PlannedConstant{});
        }
    }, Expr);
}

// ============================================================================
// Tuple and Matrix Planning
// ============================================================================

llvm::Expected<PlannedTuple> Planner::planTuple(const Tuple &Tup) {
    PlannedTuple Result;
    Result.Loc = Tup.Loc;

    for (const auto &Comp : Tup.Components) {
        PlannedComponent PC;
        PC.Loc = Comp.Loc;

        if (Comp.Name) {
            PC.Name = std::make_unique<std::string>(*Comp.Name);
        }

        auto PlannedValue = planOperands(Comp.Value);
        if (!PlannedValue) {
            return PlannedValue.takeError();
        }

        // Collapse the operand sequence to a single value
        // This handles operators like *ptr within tuple arguments
        if (!PlannedValue->empty()) {
            auto Collapsed = collapseOperandSequence(std::move(*PlannedValue));
            if (!Collapsed) {
                return Collapsed.takeError();
            }
            PC.Value.push_back(std::move(*Collapsed));
        }

        for (const auto &Attr : Comp.Attributes) {
            PlannedAttribute PA;
            PA.Loc = Attr.Loc;
            PA.Name = Attr.Name;
            PA.Value = cloneModel(Attr.Value);
            PC.Attributes.push_back(std::move(PA));
        }

        Result.Components.push_back(std::move(PC));
    }

    // Compute TupleType
    // Single anonymous component = grouping parentheses, use the value's result type
    if (Result.Components.size() == 1 && !Result.Components[0].Name) {
        // For grouping parentheses, use the collapsed value's type
        if (!Result.Components[0].Value.empty()) {
            Result.TupleType = Result.Components[0].Value[0].ResultType;
        }
    } else {
        // Multi-component or named component tuple
        // Build a proper tuple type with generics for each component
        if (Result.Components.size() == 0) {
            // Empty tuple - this is valid for () constructor calls
            Result.TupleType.Loc = Tup.Loc;
            Result.TupleType.Name = "void";
            Result.TupleType.MangledName = "v";
            return Result;
        }
        Result.TupleType.Loc = Tup.Loc;
        Result.TupleType.Name = "Tuple";

        std::string MangledName = std::to_string(Result.Components.size()) + "Tuple";
        if (!Result.Components.empty()) {
            MangledName += "I";  // Start template args
            for (const auto &Comp : Result.Components) {
                if (!Comp.Value.empty()) {
                    auto CompType = resolveOperationSequence(Comp.Value);
                    if (CompType) {
                        Result.TupleType.Generics.push_back(*CompType);
                        MangledName += CompType->MangledName;
                    }
                }
            }
            MangledName += "E";  // End template args
        }
        Result.TupleType.MangledName = MangledName;
    }

    return Result;
}

llvm::Expected<PlannedMatrix> Planner::planMatrix(const Matrix &Mat) {
    PlannedMatrix Result;
    Result.Loc = Mat.Loc;

    for (const auto &Row : Mat.Operations) {
        auto PlannedRow = planOperands(Row);
        if (!PlannedRow) {
            return PlannedRow.takeError();
        }
        Result.Operations.push_back(std::move(*PlannedRow));
    }

    // TODO: Compute ElementType

    return Result;
}

// ============================================================================
// Statement Planning
// ============================================================================

llvm::Expected<PlannedStatement> Planner::planStatement(const Statement &Stmt) {
    return std::visit([this](const auto &S) -> llvm::Expected<PlannedStatement> {
        using T = std::decay_t<decltype(S)>;

        if constexpr (std::is_same_v<T, Action>) {
            auto Planned = planAction(S);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedStatement(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Binding>) {
            auto Planned = planBinding(S);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedStatement(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, Break>) {
            PlannedBreak PB;
            PB.Loc = S.Loc;
            if (!S.Result.empty()) {
                auto PlannedResult = planOperands(S.Result);
                if (!PlannedResult) {
                    return PlannedResult.takeError();
                }
                PB.Result = std::move(*PlannedResult);
            }
            return PlannedStatement(std::move(PB));
        }
        else if constexpr (std::is_same_v<T, Continue>) {
            return PlannedStatement(PlannedContinue{S.Loc});
        }
        else if constexpr (std::is_same_v<T, Return>) {
            PlannedReturn PR;
            PR.Loc = S.Loc;
            if (!S.Result.empty()) {
                auto PlannedResult = planOperands(S.Result);
                if (!PlannedResult) {
                    return PlannedResult.takeError();
                }
                // Collapse operand sequence to create PlannedCall for operators
                auto CollapsedResult = collapseOperandSequence(std::move(*PlannedResult));
                if (!CollapsedResult) {
                    return CollapsedResult.takeError();
                }
                PR.Result.push_back(std::move(*CollapsedResult));
            }
            return PlannedStatement(std::move(PR));
        }
        else if constexpr (std::is_same_v<T, Throw>) {
            PlannedThrow PT;
            PT.Loc = S.Loc;
            auto PlannedResult = planOperands(S.Result);
            if (!PlannedResult) {
                return PlannedResult.takeError();
            }
            PT.Result = std::move(*PlannedResult);
            return PlannedStatement(std::move(PT));
        }
        else {
            return PlannedStatement(PlannedAction{});
        }
    }, Stmt);
}

llvm::Expected<std::vector<PlannedStatement>> Planner::planStatements(
    const std::vector<Statement> &Stmts) {

    std::vector<PlannedStatement> Result;
    Result.reserve(Stmts.size());

    for (const auto &Stmt : Stmts) {
        auto Planned = planStatement(Stmt);
        if (!Planned) {
            return Planned.takeError();
        }
        Result.push_back(std::move(*Planned));
    }

    return Result;
}

llvm::Expected<PlannedAction> Planner::planAction(const Action &Act) {
    PlannedAction Result;

    auto PlannedSource = planOperands(Act.Source);
    if (!PlannedSource) {
        return PlannedSource.takeError();
    }

    // Collapse operand sequence to create PlannedCall structures for operators
    auto CollapsedSource = collapseOperandSequence(std::move(*PlannedSource));
    if (!CollapsedSource) {
        return CollapsedSource.takeError();
    }
    Result.Source.push_back(std::move(*CollapsedSource));

    auto PlannedTarget = planOperands(Act.Target);
    if (!PlannedTarget) {
        return PlannedTarget.takeError();
    }

    // Collapse target operand sequence to create PlannedCall structures for operators
    // This is needed for pointer dereference targets like *(ptr + offset)
    // Only collapse if there's a target (some actions like function calls have no target)
    if (!PlannedTarget->empty()) {
        auto CollapsedTarget = collapseOperandSequence(std::move(*PlannedTarget));
        if (!CollapsedTarget) {
            return CollapsedTarget.takeError();
        }
        Result.Target.push_back(std::move(*CollapsedTarget));
    }

    // Compute the result type of the source operation sequence
    auto SeqType = resolveOperationSequence(Result.Source);
    if (!SeqType) {
        return SeqType.takeError();
    }
    Result.ResultType = std::move(*SeqType);

    return Result;
}

llvm::Expected<PlannedBinding> Planner::planBinding(const Binding &Bind) {
    PlannedBinding Result;
    Result.Loc = Bind.Loc;
    Result.BindingType = Bind.BindingType;

    // Plan the initializer operands first (needed for type inference)
    auto PlannedOp = planOperands(Bind.Operation);
    if (!PlannedOp) {
        return PlannedOp.takeError();
    }

    // Skip the leading `=` operator if present (binding syntax: name = value)
    std::vector<PlannedOperand> ValueOps;
    bool SkippedEquals = false;
    for (auto &Op : *PlannedOp) {
        if (!SkippedEquals) {
            if (auto *TypeExpr = std::get_if<PlannedType>(&Op.Expr)) {
                if (TypeExpr->Name == "=") {
                    SkippedEquals = true;
                    continue;  // Skip the = operator
                }
            }
        }
        ValueOps.push_back(std::move(Op));
    }

    // Collapse operand sequence to create PlannedCall structures for operators
    PlannedType StackArrayType;  // For stack array declarations like char[64]
    bool IsStackArrayDecl = false;
    if (!ValueOps.empty()) {
        auto CollapsedOp = collapseOperandSequence(std::move(ValueOps));
        if (!CollapsedOp) {
            return CollapsedOp.takeError();
        }

        // Check if this is a stack array type declaration (e.g., char[64])
        // In this case, the collapsed result is a PlannedType with ArraySize set
        if (auto* TypeExpr = std::get_if<PlannedType>(&CollapsedOp->Expr)) {
            if (!TypeExpr->ArraySize.empty() && TypeExpr->Name == "pointer") {
                // This is a stack array declaration - use as type, not value
                IsStackArrayDecl = true;
                StackArrayType = std::move(*TypeExpr);
            } else {
                Result.Operation.push_back(std::move(*CollapsedOp));
            }
        } else {
            Result.Operation.push_back(std::move(*CollapsedOp));
        }
    }

    // Plan the binding item
    auto PlannedItemResult = planItem(Bind.BindingItem);
    if (!PlannedItemResult) {
        return PlannedItemResult.takeError();
    }
    Result.BindingItem = std::move(*PlannedItemResult);

    // Handle stack array declaration (e.g., var buffer char[64])
    if (IsStackArrayDecl) {
        Result.BindingItem.ItemType = std::make_shared<PlannedType>(std::move(StackArrayType));
    }
    // Type inference for bindings without explicit type annotation
    else if (!Result.BindingItem.ItemType && !Result.Operation.empty()) {
        // Try to infer the type from the initializer expression
        auto SeqType = resolveOperationSequence(Result.Operation);
        if (SeqType) {
            // Successful inference - use the inferred type
            Result.BindingItem.ItemType = std::make_shared<PlannedType>(
                std::move(*SeqType));
        } else {
            // Inference failed - create a type variable for H-M inference
            llvm::consumeError(SeqType.takeError());

            std::string VarName = Result.BindingItem.Name
                ? ("T_" + *Result.BindingItem.Name)
                : "";
            PlannedType TypeVar = freshTypeVar(VarName);
            Result.BindingItem.ItemType = std::make_shared<PlannedType>(TypeVar);

            // If we have operation result types, add constraint
            if (!Result.Operation.empty()) {
                const PlannedType &ExprType = Result.Operation.back().ResultType;
                if (!ExprType.Name.empty()) {
                    addConstraint(TypeVar, ExprType, Bind.Loc);
                }
            }
        }
    } else if (Result.BindingItem.ItemType && !Result.Operation.empty()) {
        // Explicit type annotation
        const PlannedType &DeclaredType = *Result.BindingItem.ItemType;

        // String-to-char coercion: convert string literal to Unicode code point
        // Must happen BEFORE constraint check so the types match
        bool coerced = false;
        if (DeclaredType.Name == "char") {
            auto& Op = Result.Operation.back();
            if (auto* Const = std::get_if<PlannedConstant>(&Op.Expr)) {
                if (auto* StrConst = std::get_if<StringConstant>(Const)) {
                    int32_t codePoint = decodeUtf8ToCodePoint(StrConst->Value);
                    if (codePoint < 0) {
                        return llvm::make_error<llvm::StringError>(
                            "Cannot convert string to char: must be a single Unicode code point",
                            llvm::inconvertibleErrorCode());
                    }
                    // Replace StringConstant with IntegerConstant
                    IntegerConstant IntConst;
                    IntConst.Loc = StrConst->Loc;
                    IntConst.Value = codePoint;
                    Op.Expr = IntConst;
                    Op.ResultType.Name = "char";
                    Op.ResultType.MangledName = "i";  // i32 mangling
                    coerced = true;
                }
            }
        }

        // Add constraint for type checking (skip if we just coerced)
        if (!coerced) {
            const PlannedType &ExprType = Result.Operation.back().ResultType;
            if (!DeclaredType.Name.empty() && !ExprType.Name.empty()) {
                addConstraint(DeclaredType, ExprType, Bind.Loc);
            }
        }
    }

    // Add to scope
    if (Result.BindingItem.Name && Result.BindingItem.ItemType) {
        bool IsMutable = (Result.BindingType == "var" || Result.BindingType == "mutable");
        bool IsOnPage = isOnPageLifetime(Result.BindingItem.ItemType->Life);
        defineLocal(*Result.BindingItem.Name, *Result.BindingItem.ItemType, IsMutable, IsOnPage);
    }

    return Result;
}

llvm::Expected<PlannedBlock> Planner::planBlock(const Block &Blk) {
    PlannedBlock Result;
    Result.Loc = Blk.Loc;

    pushScope();

    auto PlannedStmts = planStatements(Blk.Statements);
    if (!PlannedStmts) {
        popScope();
        return PlannedStmts.takeError();
    }
    Result.Statements = std::move(*PlannedStmts);

    // Capture scope info before popping - does this block need cleanup?
    if (!ScopeInfoStack.empty()) {
        Result.NeedsLocalPageCleanup = ScopeInfoStack.back().HasLocalAllocations;
    }

    popScope();

    return Result;
}

// ============================================================================
// Control Flow Planning
// ============================================================================

llvm::Expected<PlannedIf> Planner::planIf(const If &IfExpr) {
    PlannedIf Result;
    Result.Loc = IfExpr.Loc;

    auto PlannedCond = planOperands(IfExpr.Condition);
    if (!PlannedCond) {
        return PlannedCond.takeError();
    }
    // Collapse the operand sequence (handles operator chains like "i = 3")
    auto CollapsedCond = collapseOperandSequence(std::move(*PlannedCond));
    if (!CollapsedCond) {
        return CollapsedCond.takeError();
    }
    Result.Condition.push_back(std::move(*CollapsedCond));

    if (IfExpr.Prop) {
        auto PlannedProp = planProperty(*IfExpr.Prop);
        if (!PlannedProp) {
            return PlannedProp.takeError();
        }
        Result.Prop = std::make_unique<PlannedProperty>(std::move(*PlannedProp));
    }

    if (IfExpr.Consequent) {
        auto PlannedCons = planStatement(*IfExpr.Consequent);
        if (!PlannedCons) {
            return PlannedCons.takeError();
        }
        Result.Consequent = std::make_unique<PlannedStatement>(std::move(*PlannedCons));
    }

    if (IfExpr.Alternative) {
        auto PlannedAlt = planStatement(*IfExpr.Alternative);
        if (!PlannedAlt) {
            return PlannedAlt.takeError();
        }
        Result.Alternative = std::make_unique<PlannedStatement>(std::move(*PlannedAlt));
    }

    return Result;
}

llvm::Expected<PlannedMatch> Planner::planMatch(const Match &MatchExpr) {
    PlannedMatch Result;
    Result.Loc = MatchExpr.Loc;

    auto PlannedCond = planOperands(MatchExpr.Condition);
    if (!PlannedCond) {
        return PlannedCond.takeError();
    }
    Result.Condition = std::move(*PlannedCond);

    for (const auto &Branch : MatchExpr.Branches) {
        PlannedBranch PB;
        PB.Loc = Branch.Loc;

        for (const auto &Case : Branch.Cases) {
            PlannedCase PC;
            PC.Loc = Case.Loc;

            auto PlannedCaseCond = planOperands(Case.Condition);
            if (!PlannedCaseCond) {
                return PlannedCaseCond.takeError();
            }
            PC.Condition = std::move(*PlannedCaseCond);

            PB.Cases.push_back(std::move(PC));
        }

        if (Branch.Consequent) {
            auto PlannedCons = planStatement(*Branch.Consequent);
            if (!PlannedCons) {
                return PlannedCons.takeError();
            }
            PB.Consequent = std::make_unique<PlannedStatement>(std::move(*PlannedCons));
        }

        Result.Branches.push_back(std::move(PB));
    }

    if (MatchExpr.Alternative) {
        auto PlannedAlt = planStatement(*MatchExpr.Alternative);
        if (!PlannedAlt) {
            return PlannedAlt.takeError();
        }
        Result.Alternative = std::make_unique<PlannedStatement>(std::move(*PlannedAlt));
    }

    return Result;
}

llvm::Expected<PlannedChoose> Planner::planChoose(const Choose &ChooseExpr) {
    PlannedChoose Result;
    Result.Loc = ChooseExpr.Loc;

    auto PlannedCond = planOperands(ChooseExpr.Condition);
    if (!PlannedCond) {
        return PlannedCond.takeError();
    }
    Result.Condition = std::move(*PlannedCond);

    // Get the result type from the condition to look up the union
    const PlannedUnion *CondUnion = nullptr;
    if (!Result.Condition.empty()) {
        const auto &ResultType = Result.Condition[0].ResultType;
        auto UnionIt = InstantiatedUnions.find(ResultType.Name);
        if (UnionIt != InstantiatedUnions.end()) {
            CondUnion = &UnionIt->second;
        }
    }

    pushScope();

    for (const auto &Case : ChooseExpr.Cases) {
        PlannedWhen PW;
        PW.Loc = Case.Loc;
        PW.Name = Case.Name;
        PW.VariantIndex = 0;  // Default

        // Get the variant name from the path
        std::string VariantName;
        if (!Case.VariantPath.empty()) {
            VariantName = Case.VariantPath.back();
        }

        // Look up the variant in the union to get its tag
        if (CondUnion && !VariantName.empty()) {
            for (size_t i = 0; i < CondUnion->Variants.size(); ++i) {
                if (CondUnion->Variants[i].Name == VariantName) {
                    PW.VariantIndex = CondUnion->Variants[i].Tag;
                    // Also get the variant's type for the binding variable
                    if (CondUnion->Variants[i].VarType) {
                        PW.VariantType = *CondUnion->Variants[i].VarType;
                    }
                    break;
                }
            }
        }

        // Fallback: resolve the variant path if we didn't find it in the union
        if (PW.VariantType.Name.empty()) {
            auto ResolvedType = resolveTypePath(Case.VariantPath, Case.Loc);
            if (!ResolvedType) {
                popScope();
                return ResolvedType.takeError();
            }
            PW.VariantType = std::move(*ResolvedType);
        }

        // Add the binding variable to the scope
        if (!Case.Name.empty()) {
            defineLocal(Case.Name, PW.VariantType, false);
        }

        if (Case.Consequent) {
            auto PlannedCons = planStatement(*Case.Consequent);
            if (!PlannedCons) {
                popScope();
                return PlannedCons.takeError();
            }
            PW.Consequent = std::make_unique<PlannedStatement>(std::move(*PlannedCons));
        }

        Result.Cases.push_back(std::move(PW));
    }

    if (ChooseExpr.Alternative) {
        auto PlannedAlt = planStatement(*ChooseExpr.Alternative);
        if (!PlannedAlt) {
            popScope();
            return PlannedAlt.takeError();
        }
        Result.Alternative = std::make_unique<PlannedStatement>(std::move(*PlannedAlt));
    }

    popScope();

    return Result;
}

llvm::Expected<PlannedFor> Planner::planFor(const For &ForExpr) {
    PlannedFor Result;
    Result.Loc = ForExpr.Loc;
    Result.Identifier = ForExpr.Identifier;

    auto PlannedExpr = planOperands(ForExpr.Expr);
    if (!PlannedExpr) {
        return PlannedExpr.takeError();
    }
    Result.Expr = std::move(*PlannedExpr);

    pushScope();

    // Define loop variable in scope with inferred element type
    // Scaly for-loops can be:
    // 1. Integer range: for i in N (iterates 0 to N-1)
    // 2. Iterator-based: for item in collection (uses get_iterator/next protocol)
    if (!Result.Expr.empty()) {
        const auto &CollType = Result.Expr.back().ResultType;
        Result.CollectionType = CollType;

        // Check if this is an integer range loop
        if (CollType.Name == "int" || CollType.Name == "int8" ||
            CollType.Name == "int16" || CollType.Name == "int32" ||
            CollType.Name == "int64" || CollType.Name == "size_t" ||
            CollType.Name == "uint8" || CollType.Name == "uint16" ||
            CollType.Name == "uint32" || CollType.Name == "uint64") {
            // Integer range loop: for i in N
            Result.IsIteratorLoop = false;
            Result.ElementType = CollType;
        } else {
            // Try iterator protocol
            auto IterMethod = lookupMethod(CollType, "get_iterator", ForExpr.Loc);
            if (IterMethod) {
                Result.IsIteratorLoop = true;
                Result.IteratorType = IterMethod->ReturnType;
                Result.GetIteratorMethod = IterMethod->MangledName;

                // Look for next() on the iterator type
                auto NextMethod = lookupMethod(Result.IteratorType, "next", ForExpr.Loc);
                if (NextMethod) {
                    Result.NextMethod = NextMethod->MangledName;
                    // Element type is exactly what next() returns
                    // (typically pointer[T] where null means end of iteration)
                    Result.ElementType = NextMethod->ReturnType;
                } else {
                    // No next() method found - error
                    popScope();
                    return llvm::make_error<PlannerError>(UndefinedSymbolError{
                        File, ForExpr.Loc,
                        Result.IteratorType.Name + ".next"
                    });
                }
            } else {
                // No get_iterator - fallback to first generic arg (for pointer[T] etc.)
                Result.IsIteratorLoop = false;
                if (!CollType.Generics.empty()) {
                    Result.ElementType = CollType.Generics[0];
                } else {
                    Result.ElementType = CollType;
                }
            }
        }

        defineLocal(ForExpr.Identifier, Result.ElementType);
    }

    auto PlannedBody = planAction(ForExpr.Body);
    if (!PlannedBody) {
        popScope();
        return PlannedBody.takeError();
    }
    Result.Body = std::move(*PlannedBody);

    popScope();

    return Result;
}

llvm::Expected<PlannedWhile> Planner::planWhile(const While &WhileExpr) {
    PlannedWhile Result;
    Result.Loc = WhileExpr.Loc;

    pushScope();

    auto PlannedCond = planBinding(WhileExpr.Cond);
    if (!PlannedCond) {
        popScope();
        return PlannedCond.takeError();
    }
    Result.Cond = std::move(*PlannedCond);

    auto PlannedBody = planAction(WhileExpr.Body);
    if (!PlannedBody) {
        popScope();
        return PlannedBody.takeError();
    }
    Result.Body = std::move(*PlannedBody);

    popScope();

    return Result;
}

llvm::Expected<PlannedTry> Planner::planTry(const Try &TryExpr) {
    PlannedTry Result;
    Result.Loc = TryExpr.Loc;

    pushScope();

    auto PlannedCond = planBinding(TryExpr.Cond);
    if (!PlannedCond) {
        popScope();
        return PlannedCond.takeError();
    }
    Result.Cond = std::move(*PlannedCond);

    // Get the result type from the binding's operation to look up the union
    const PlannedUnion *CondUnion = nullptr;
    if (!Result.Cond.Operation.empty()) {
        const auto &ResultType = Result.Cond.Operation[0].ResultType;
        auto UnionIt = InstantiatedUnions.find(ResultType.Name);
        if (UnionIt != InstantiatedUnions.end()) {
            CondUnion = &UnionIt->second;
        }
    }

    for (const auto &Catch : TryExpr.Catches) {
        PlannedWhen PW;
        PW.Loc = Catch.Loc;
        PW.Name = Catch.Name;
        PW.VariantIndex = 0;  // Default

        // Get the variant name from the path
        std::string VariantName;
        if (!Catch.VariantPath.empty()) {
            VariantName = Catch.VariantPath.back();
        }

        // Look up the variant in the union to get its tag
        if (CondUnion && !VariantName.empty()) {
            for (size_t i = 0; i < CondUnion->Variants.size(); ++i) {
                if (CondUnion->Variants[i].Name == VariantName) {
                    PW.VariantIndex = CondUnion->Variants[i].Tag;
                    // Also get the variant's type for the catch variable
                    if (CondUnion->Variants[i].VarType) {
                        PW.VariantType = *CondUnion->Variants[i].VarType;
                    }
                    break;
                }
            }
        }

        // Fallback: resolve the variant path if we didn't find it in the union
        if (PW.VariantType.Name.empty()) {
            auto ResolvedType = resolveTypePath(Catch.VariantPath, Catch.Loc);
            if (!ResolvedType) {
                popScope();
                return ResolvedType.takeError();
            }
            PW.VariantType = std::move(*ResolvedType);
        }

        // Add the catch variable to the scope so it can be referenced in the consequent
        if (!Catch.Name.empty()) {
            defineLocal(Catch.Name, PW.VariantType, false);  // false = not mutable
        }

        if (Catch.Consequent) {
            auto PlannedCons = planStatement(*Catch.Consequent);
            if (!PlannedCons) {
                popScope();
                return PlannedCons.takeError();
            }
            PW.Consequent = std::make_unique<PlannedStatement>(std::move(*PlannedCons));
        }

        Result.Catches.push_back(std::move(PW));
    }

    if (TryExpr.Alternative) {
        auto PlannedAlt = planStatement(*TryExpr.Alternative);
        if (!PlannedAlt) {
            popScope();
            return PlannedAlt.takeError();
        }
        Result.Alternative = std::make_unique<PlannedStatement>(std::move(*PlannedAlt));
    }

    popScope();

    return Result;
}

// ============================================================================
// Implementation Planning
// ============================================================================

llvm::Expected<PlannedImplementation> Planner::planImplementation(
    const Implementation &Impl) {
    return std::visit([this](const auto &I) -> llvm::Expected<PlannedImplementation> {
        using T = std::decay_t<decltype(I)>;

        if constexpr (std::is_same_v<T, Action>) {
            auto Planned = planAction(I);
            if (!Planned) {
                return Planned.takeError();
            }
            return PlannedImplementation(std::move(*Planned));
        }
        else if constexpr (std::is_same_v<T, ExternImpl>) {
            return PlannedImplementation(PlannedExternImpl{I.Loc});
        }
        else if constexpr (std::is_same_v<T, InstructionImpl>) {
            return PlannedImplementation(PlannedInstructionImpl{I.Loc});
        }
        else if constexpr (std::is_same_v<T, IntrinsicImpl>) {
            return PlannedImplementation(PlannedIntrinsicImpl{I.Loc});
        }
        else {
            // Should never reach here
            return PlannedImplementation(PlannedIntrinsicImpl{Span{0, 0}});
        }
    }, Impl);
}

// ============================================================================
// Function Planning
// ============================================================================

llvm::Expected<PlannedFunction> Planner::planFunction(const Function &Func,
                                                       const PlannedType *Parent) {
    PlannedFunction Result;
    Result.Loc = Func.Loc;
    Result.Private = Func.Private;
    Result.Pure = Func.Pure;
    Result.Name = Func.Name;
    Result.Life = Func.Life;
    Result.PageParameter = Func.PageParameter;

    // Save and reset tracking for $ allocations in function body
    // This allows nested planFunction calls (e.g., when planning constructors)
    // without corrupting the outer function's NeedsLocalPage flag
    bool SavedUsesLocalLifetime = CurrentFunctionUsesLocalLifetime;
    CurrentFunctionUsesLocalLifetime = false;

    pushScope();

    // If function# was used, define the page parameter in scope
    if (Func.PageParameter) {
        PlannedType PagePtrType;
        PagePtrType.Loc = Func.Loc;
        PagePtrType.Name = "pointer";
        PagePtrType.MangledName = "PN4scaly6memory4PageE";  // pointer[scaly.memory.Page]

        PlannedType PageType;
        PageType.Loc = Func.Loc;
        PageType.Name = "scaly.memory.Page";
        PageType.MangledName = "N4scaly6memory4PageE";
        PagePtrType.Generics.push_back(PageType);

        defineLocal(*Func.PageParameter, PagePtrType, false);
    }

    // If function has a ReferenceLifetime, add implicit region parameter
    if (auto* RefLife = std::get_if<ReferenceLifetime>(&Func.Life)) {
        // Create implicit _<name>: pointer[Page] parameter
        std::string RegionParamName = "_" + RefLife->Location;

        // Create the pointer[Page] type
        PlannedType PageType;
        PageType.Loc = RefLife->Loc;
        PageType.Name = "Page";
        PageType.MangledName = "4Page";

        PlannedType PtrPageType;
        PtrPageType.Loc = RefLife->Loc;
        PtrPageType.Name = "pointer";
        PtrPageType.MangledName = "P4Page";
        PtrPageType.Generics.push_back(PageType);

        PlannedItem RegionParam;
        RegionParam.Loc = RefLife->Loc;
        RegionParam.Private = false;
        RegionParam.Name = std::make_shared<std::string>(RegionParamName);
        RegionParam.ItemType = std::make_shared<PlannedType>(PtrPageType);

        Result.Input.push_back(std::move(RegionParam));

        // Add to scope so code inside the function can reference it
        // Define both _rp (the actual parameter name) and rp (the lifetime name)
        // so that code can reference the region using either form
        defineLocal(RegionParamName, PtrPageType);
        defineLocal(RefLife->Location, PtrPageType);
    }

    // Plan input parameters
    for (const auto &Param : Func.Input) {
        auto PlannedParam = planItem(Param);
        if (!PlannedParam) {
            popScope();
            return PlannedParam.takeError();
        }

        // Handle 'this' parameter: if no explicit type, use Parent type
        if (PlannedParam->Name && *PlannedParam->Name == "this" && !PlannedParam->ItemType) {
            if (Parent) {
                PlannedParam->ItemType = std::make_shared<PlannedType>(*Parent);
            } else {
                popScope();
                return llvm::make_error<llvm::StringError>(
                    "'this' parameter without enclosing type",
                    llvm::inconvertibleErrorCode()
                );
            }
        }

        // For 'this' parameter, the type in scope should be pointer[T] since
        // 'this' is passed by reference at runtime (like C++ 'this' pointer)
        PlannedType ScopeType;
        bool IsThisParam = PlannedParam->Name && *PlannedParam->Name == "this";
        if (IsThisParam && PlannedParam->ItemType) {
            // Wrap the declared type in pointer[T]
            PlannedType InnerType = *PlannedParam->ItemType;
            ScopeType.Loc = InnerType.Loc;
            ScopeType.Name = "pointer";
            ScopeType.MangledName = "P" + InnerType.MangledName;
            ScopeType.Generics.push_back(InnerType);
        }

        Result.Input.push_back(std::move(*PlannedParam));

        // Add parameter to scope
        if (Result.Input.back().Name && Result.Input.back().ItemType) {
            if (IsThisParam) {
                // Use pointer[T] type for 'this' in scope
                defineLocal(*Result.Input.back().Name, ScopeType);
            } else {
                defineLocal(*Result.Input.back().Name, *Result.Input.back().ItemType);
            }
        }
    }

    // Plan return type
    if (Func.Returns) {
        auto ResolvedReturn = resolveType(*Func.Returns, Func.Loc);
        if (!ResolvedReturn) {
            popScope();
            return ResolvedReturn.takeError();
        }
        Result.Returns = std::make_shared<PlannedType>(std::move(*ResolvedReturn));

        // Local lifetime ($) is forbidden on return types
        // Use no lifetime for by-value return, or # for caller's page allocation
        if (std::holds_alternative<LocalLifetime>(Result.Returns->Life)) {
            popScope();
            return makeLocalReturnError(File, Func.Returns->Loc);
        }

        // If return type has CallLifetime (#), ensure Page is instantiated for RBMM support
        if (std::holds_alternative<CallLifetime>(Result.Returns->Life) ||
            std::holds_alternative<ReferenceLifetime>(Result.Returns->Life)) {
            if (InstantiatedStructures.find("Page") == InstantiatedStructures.end()) {
                // Register runtime functions (aligned_alloc, free, exit) before planning Page
                registerRuntimeFunctions();

                const Concept *PageConcept = lookupConcept("Page");
                if (PageConcept) {
                    auto PlannedConc = planConcept(*PageConcept);
                    if (!PlannedConc) {
                        llvm::consumeError(PlannedConc.takeError());
                    }
                }
            }
        }
    }

    // Plan throws type
    if (Func.Throws) {
        auto ResolvedThrows = resolveType(*Func.Throws, Func.Loc);
        if (!ResolvedThrows) {
            popScope();
            return ResolvedThrows.takeError();
        }
        Result.Throws = std::make_shared<PlannedType>(std::move(*ResolvedThrows));
        Result.CanThrow = true;
        // Note: The exception page parameter is added by the Emitter, not here.
        // The Emitter checks Func.CanThrow/Func.Throws and adds the parameter.
    }

    // Plan implementation (function body)
    auto PlannedImpl = planImplementation(Func.Impl);
    if (!PlannedImpl) {
        popScope();
        return PlannedImpl.takeError();
    }
    Result.Impl = std::move(*PlannedImpl);

    popScope();

    // Copy the flag to indicate if function needs a local page
    Result.NeedsLocalPage = CurrentFunctionUsesLocalLifetime;

    // Restore saved flag (allows outer function to continue tracking its own $ allocations)
    CurrentFunctionUsesLocalLifetime = SavedUsesLocalLifetime;

    // Generate mangled name
    // For extern functions, use the unmangled C name for linking
    if (std::holds_alternative<PlannedExternImpl>(Result.Impl)) {
        Result.MangledName = Func.Name;
    } else {
        Result.MangledName = mangleFunction(Func.Name, Result.Input, Parent);
    }

    return Result;
}

// ============================================================================
// Operator Planning
// ============================================================================

llvm::Expected<PlannedOperator> Planner::planOperator(const Operator &Op,
                                                       const PlannedType *Parent) {
    PlannedOperator Result;
    Result.Loc = Op.Loc;
    Result.Private = Op.Private;
    Result.Name = Op.Name;

    pushScope();

    // For operators inside a struct, add implicit 'this' parameter if not present
    // This allows operators to access struct properties directly
    bool HasThisParam = false;
    for (const auto &Param : Op.Input) {
        if (Param.Name && *Param.Name == "this") {
            HasThisParam = true;
            break;
        }
    }

    if (Parent && !HasThisParam) {
        // Add implicit 'this' parameter at the beginning
        PlannedItem ThisParam;
        ThisParam.Loc = Op.Loc;
        ThisParam.Private = false;
        ThisParam.Name = std::make_shared<std::string>("this");
        ThisParam.ItemType = std::make_shared<PlannedType>(*Parent);
        Result.Input.push_back(std::move(ThisParam));

        // Define 'this' in scope as pointer[T] since it's passed by reference
        PlannedType ThisPtrType;
        ThisPtrType.Loc = Parent->Loc;
        ThisPtrType.Name = "pointer";
        ThisPtrType.MangledName = "P" + Parent->MangledName;
        ThisPtrType.Generics.push_back(*Parent);
        defineLocal("this", ThisPtrType);
    }

    // Plan input parameters
    for (const auto &Param : Op.Input) {
        auto PlannedParam = planItem(Param);
        if (!PlannedParam) {
            popScope();
            return PlannedParam.takeError();
        }

        // Handle explicit 'this' parameter: if no explicit type, use Parent type
        if (PlannedParam->Name && *PlannedParam->Name == "this" && !PlannedParam->ItemType) {
            if (Parent) {
                PlannedParam->ItemType = std::make_shared<PlannedType>(*Parent);
            } else {
                popScope();
                return llvm::make_error<llvm::StringError>(
                    "'this' parameter without enclosing type",
                    llvm::inconvertibleErrorCode()
                );
            }
        }

        // For 'this' parameter, the type in scope should be pointer[T]
        PlannedType ScopeType;
        bool IsThisParam = PlannedParam->Name && *PlannedParam->Name == "this";
        if (IsThisParam && PlannedParam->ItemType) {
            PlannedType InnerType = *PlannedParam->ItemType;
            ScopeType.Loc = InnerType.Loc;
            ScopeType.Name = "pointer";
            ScopeType.MangledName = "P" + InnerType.MangledName;
            ScopeType.Generics.push_back(InnerType);
        }

        Result.Input.push_back(std::move(*PlannedParam));

        // Add parameter to scope
        if (Result.Input.back().Name && Result.Input.back().ItemType) {
            if (IsThisParam) {
                defineLocal(*Result.Input.back().Name, ScopeType);
            } else {
                defineLocal(*Result.Input.back().Name, *Result.Input.back().ItemType);
            }
        }
    }

    // Plan return type
    if (Op.Returns) {
        auto ResolvedReturn = resolveType(*Op.Returns, Op.Loc);
        if (!ResolvedReturn) {
            popScope();
            return ResolvedReturn.takeError();
        }
        Result.Returns = std::make_shared<PlannedType>(std::move(*ResolvedReturn));

        // Local lifetime ($) is forbidden on return types
        if (std::holds_alternative<LocalLifetime>(Result.Returns->Life)) {
            popScope();
            return makeLocalReturnError(File, Op.Returns->Loc);
        }
    }

    // Plan throws type
    if (Op.Throws) {
        auto ResolvedThrows = resolveType(*Op.Throws, Op.Loc);
        if (!ResolvedThrows) {
            popScope();
            return ResolvedThrows.takeError();
        }
        Result.Throws = std::make_shared<PlannedType>(std::move(*ResolvedThrows));
    }

    // Plan implementation (operator body)
    auto PlannedImpl = planImplementation(Op.Impl);
    if (!PlannedImpl) {
        popScope();
        return PlannedImpl.takeError();
    }
    Result.Impl = std::move(*PlannedImpl);

    popScope();

    // Generate mangled name
    Result.MangledName = mangleOperator(Op.Name, Result.Input, Parent);

    return Result;
}

// ============================================================================
// Initializer Planning
// ============================================================================

llvm::Expected<PlannedInitializer> Planner::planInitializer(const Initializer &Init,
                                                             const PlannedType &Parent) {
    PlannedInitializer Result;
    Result.Loc = Init.Loc;
    Result.Private = Init.Private;
    Result.PageParameter = Init.PageParameter;

    pushScope();

    // If init# was used, define the page parameter in scope
    if (Init.PageParameter) {
        PlannedType PagePtrType;
        PagePtrType.Loc = Init.Loc;
        PagePtrType.Name = "pointer";
        PagePtrType.MangledName = "PN4scaly6memory4PageE";  // pointer[scaly.memory.Page]

        PlannedType PageType;
        PageType.Loc = Init.Loc;
        PageType.Name = "scaly.memory.Page";
        PageType.MangledName = "N4scaly6memory4PageE";
        PagePtrType.Generics.push_back(PageType);

        defineLocal(*Init.PageParameter, PagePtrType, false);
    }

    // Plan input parameters
    std::vector<PlannedItem> Params;
    for (const auto &Param : Init.Input) {
        auto PlannedParam = planItem(Param);
        if (!PlannedParam) {
            popScope();
            return PlannedParam.takeError();
        }
        Params.push_back(std::move(*PlannedParam));

        if (Params.back().Name && Params.back().ItemType) {
            defineLocal(*Params.back().Name, *Params.back().ItemType);
        }
    }
    Result.Input = std::move(Params);

    // Define 'this' in scope for the initializer body
    // IsMutable=false because 'this' is passed as a pointer parameter, not an alloca
    // Field mutations are handled via GEP on the pointer
    // Wrap Parent in pointer[T] since 'this' is actually a pointer at runtime
    PlannedType ThisPtrType;
    ThisPtrType.Loc = Parent.Loc;
    ThisPtrType.Name = "pointer";
    ThisPtrType.MangledName = "P" + Parent.MangledName;
    ThisPtrType.Generics.push_back(Parent);
    defineLocal("this", ThisPtrType, false);

    // Plan implementation (initializer body)
    auto PlannedImpl = planImplementation(Init.Impl);
    if (!PlannedImpl) {
        popScope();
        return PlannedImpl.takeError();
    }
    Result.Impl = std::move(*PlannedImpl);

    popScope();

    // Mangle as constructor: _ZN<type>C1E<params>
    std::string Mangled = "_ZN";
    Mangled += encodeType(Parent);
    Mangled += "C1E";  // C1 = complete object constructor

    bool HasParams = false;

    // If init# was used, include page parameter first
    if (Result.PageParameter) {
        Mangled += "PN4scaly6memory4PageE";  // pointer[scaly.memory.Page]
        HasParams = true;
    }

    if (!Result.Input.empty()) {
        for (const auto &P : Result.Input) {
            if (P.ItemType) {
                Mangled += encodeType(*P.ItemType);
            }
        }
        HasParams = true;
    }

    if (!HasParams) {
        Mangled += "v";
    }

    Result.MangledName = Mangled;

    return Result;
}

llvm::Expected<PlannedDeInitializer> Planner::planDeInitializer(
    const DeInitializer &DeInit, const PlannedType &Parent) {

    PlannedDeInitializer Result;
    Result.Loc = DeInit.Loc;

    // Push scope for deinitializer body
    pushScope();

    // Define 'this' in scope for the deinitializer body
    // Wrap Parent in pointer[T] since 'this' is actually a pointer at runtime
    PlannedType ThisPtrType;
    ThisPtrType.Loc = Parent.Loc;
    ThisPtrType.Name = "pointer";
    ThisPtrType.MangledName = "P" + Parent.MangledName;
    ThisPtrType.Generics.push_back(Parent);
    defineLocal("this", ThisPtrType, false);  // 'this' is immutable in deinitializers

    // Plan implementation (deinitializer body)
    auto PlannedImpl = planImplementation(DeInit.Impl);
    if (!PlannedImpl) {
        popScope();
        return PlannedImpl.takeError();
    }
    Result.Impl = std::move(*PlannedImpl);

    popScope();

    // Mangle as destructor: _ZN<type>D1Ev
    std::string Mangled = "_ZN";
    Mangled += encodeType(Parent);
    Mangled += "D1Ev";  // D1 = complete object destructor
    Result.MangledName = Mangled;

    return Result;
}

// ============================================================================
// Structure Planning
// ============================================================================

llvm::Expected<PlannedStructure> Planner::planStructure(
    const Structure &Struct, llvm::StringRef Name,
    const std::vector<PlannedType> &GenericArgs) {
    PlannedStructure Result;
    Result.Loc = Struct.Loc;
    Result.Private = Struct.Private;
    Result.Name = Name.str();
    Result.MangledName = mangleStructure(Name, GenericArgs);

    // Note: Generics come from the enclosing Concept, passed as GenericArgs
    // Set up type substitutions for generics (GenericArgs already resolved)
    std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;

    // Plan properties
    for (const auto &Prop : Struct.Properties) {
        auto PlannedProp = planProperty(Prop);
        if (!PlannedProp) {
            TypeSubstitutions = OldSubst;
            return PlannedProp.takeError();
        }
        Result.Properties.push_back(std::move(*PlannedProp));
    }

    // Compute struct layout
    computeStructLayout(Result);

    // Create parent type for method mangling
    PlannedType ParentType;
    ParentType.Name = Name.str();
    ParentType.Generics = GenericArgs;
    ParentType.MangledName = Result.MangledName;

    // Build full structure name including generic args for property lookup
    std::string FullStructureName = Name.str();
    if (!GenericArgs.empty()) {
        FullStructureName += ".";
        for (size_t i = 0; i < GenericArgs.size(); ++i) {
            if (i > 0) FullStructureName += ".";
            FullStructureName += GenericArgs[i].Name;
        }
    }

    // Set current structure context for property access in methods
    std::string OldStructureName = CurrentStructureName;
    std::vector<PlannedProperty>* OldStructureProperties = CurrentStructureProperties;
    PlannedStructure* OldStructure = CurrentStructure;
    CurrentStructureName = FullStructureName;
    CurrentStructureProperties = &Result.Properties;
    CurrentStructure = &Result;

    // Plan initializers
    for (const auto &Init : Struct.Initializers) {
        auto PlannedInit = planInitializer(Init, ParentType);
        if (!PlannedInit) {
            TypeSubstitutions = OldSubst;
            CurrentStructureName = OldStructureName;
            CurrentStructureProperties = OldStructureProperties;
            CurrentStructure = OldStructure;
            return PlannedInit.takeError();
        }
        Result.Initializers.push_back(std::move(*PlannedInit));
    }

    // Plan deinitializer
    if (Struct.Deinitializer) {
        auto PlannedDeInit = planDeInitializer(*Struct.Deinitializer, ParentType);
        if (!PlannedDeInit) {
            TypeSubstitutions = OldSubst;
            CurrentStructureName = OldStructureName;
            CurrentStructureProperties = OldStructureProperties;
            CurrentStructure = OldStructure;
            return PlannedDeInit.takeError();
        }
        Result.Deinitializer = std::make_unique<PlannedDeInitializer>(
            std::move(*PlannedDeInit));
    }

    // Plan members (Methods and Operators come from Members variant)
    for (const auto &Member : Struct.Members) {
        if (auto *Func = std::get_if<Function>(&Member)) {
            auto PlannedMethod = planFunction(*Func, &ParentType);
            if (!PlannedMethod) {
                TypeSubstitutions = OldSubst;
                CurrentStructureName = OldStructureName;
                CurrentStructureProperties = OldStructureProperties;
                CurrentStructure = OldStructure;
                return PlannedMethod.takeError();
            }
            // Register method in InstantiatedFunctions for the Emitter to find
            InstantiatedFunctions[PlannedMethod->MangledName] = *PlannedMethod;
            Result.Methods.push_back(std::move(*PlannedMethod));
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, &ParentType);
            if (!PlannedOp) {
                TypeSubstitutions = OldSubst;
                CurrentStructureName = OldStructureName;
                CurrentStructureProperties = OldStructureProperties;
                CurrentStructure = OldStructure;
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));
        }
        // Skip Concept members for now - would need recursive planning
    }

    // Restore substitutions and structure context
    TypeSubstitutions = OldSubst;
    CurrentStructureName = OldStructureName;
    CurrentStructureProperties = OldStructureProperties;
    CurrentStructure = OldStructure;

    // Add provenance if this is an instantiation
    if (!GenericArgs.empty()) {
        Result.Origin = std::make_shared<InstantiationInfo>();
        Result.Origin->DefinitionLoc = Struct.Loc;
        for (const auto &Arg : GenericArgs) {
            Result.Origin->TypeArgs.push_back(getReadableName(Arg));
        }
    }

    return Result;
}

// ============================================================================
// Union Planning
// ============================================================================

llvm::Expected<PlannedUnion> Planner::planUnion(
    const Union &Un, llvm::StringRef Name,
    const std::vector<PlannedType> &GenericArgs) {

    PlannedUnion Result;
    Result.Loc = Un.Loc;
    Result.Private = Un.Private;
    Result.Name = Name.str();
    Result.MangledName = mangleStructure(Name, GenericArgs);

    // Note: Generics come from enclosing Concept, passed as GenericArgs
    std::map<std::string, PlannedType> OldSubst = TypeSubstitutions;

    // Plan variants with tags
    int Tag = 0;
    for (const auto &Var : Un.Variants) {
        auto PlannedVar = planVariant(Var, Tag++);
        if (!PlannedVar) {
            TypeSubstitutions = OldSubst;
            return PlannedVar.takeError();
        }
        Result.Variants.push_back(std::move(*PlannedVar));
    }

    // Compute union layout
    computeUnionLayout(Result);

    // Create parent type for method mangling
    PlannedType ParentType;
    ParentType.Name = Name.str();
    ParentType.Generics = GenericArgs;
    ParentType.MangledName = Result.MangledName;

    // Plan members (Methods and Operators come from Members variant)
    for (const auto &Member : Un.Members) {
        if (auto *Func = std::get_if<Function>(&Member)) {
            auto PlannedMethod = planFunction(*Func, &ParentType);
            if (!PlannedMethod) {
                TypeSubstitutions = OldSubst;
                return PlannedMethod.takeError();
            }
            // Register method in InstantiatedFunctions for the Emitter to find
            InstantiatedFunctions[PlannedMethod->MangledName] = *PlannedMethod;
            Result.Methods.push_back(std::move(*PlannedMethod));
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, &ParentType);
            if (!PlannedOp) {
                TypeSubstitutions = OldSubst;
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));
        }
        // Skip Concept members for now
    }

    // Restore substitutions
    TypeSubstitutions = OldSubst;

    // Add provenance
    if (!GenericArgs.empty()) {
        Result.Origin = std::make_shared<InstantiationInfo>();
        Result.Origin->DefinitionLoc = Un.Loc;
        for (const auto &Arg : GenericArgs) {
            Result.Origin->TypeArgs.push_back(getReadableName(Arg));
        }
    }

    return Result;
}

// ============================================================================
// Namespace Planning
// ============================================================================

llvm::Expected<PlannedNamespace> Planner::planNamespace(const Namespace &NS,
                                                         llvm::StringRef Name) {
    PlannedNamespace Result;
    Result.Loc = NS.Loc;
    Result.Name = Name.str();
    Result.MangledName = encodeName(Name);

    // Create parent type for function/operator mangling (namespace prefix)
    PlannedType ParentType;
    ParentType.Name = Name.str();
    ParentType.MangledName = Result.MangledName;

    // Set current namespace context for sibling function calls
    std::string OldNamespaceName = CurrentNamespaceName;
    const Namespace* OldNamespace = CurrentNamespace;
    CurrentNamespaceName = Name.str();
    CurrentNamespace = &NS;

    // Create a pseudo-module wrapper for the namespace to enable cross-module lookup
    // This allows functions in the namespace to find concepts from sub-modules
    Module PseudoModule;
    PseudoModule.Name = Name.str();
    // Note: Don't copy NS.Modules - sub-modules push themselves onto stack during planning
    // Copy member concepts into a format compatible with Module.Members
    for (const auto &Member : NS.Members) {
        if (auto *Conc = std::get_if<Concept>(&Member)) {
            PseudoModule.Members.push_back(*Conc);
        }
    }

    // Push pseudo-module for cross-module resolution
    ModuleStack.push_back(&PseudoModule);

    // Plan sub-modules first (so their concepts are available)
    // Store pointers to loaded modules for sibling concept lookup
    std::vector<const Module*> OldNamespaceModules = CurrentNamespaceModules;
    CurrentNamespaceModules.clear();
    for (const auto &SubMod : NS.Modules) {
        // planModule pushes the module onto ModuleStack, we capture it from there
        size_t stackSizeBefore = ModuleStack.size();
        auto PlannedSubMod = planModule(SubMod);
        if (!PlannedSubMod) {
            ModuleStack.pop_back();
            CurrentNamespaceName = OldNamespaceName;
            CurrentNamespace = OldNamespace;
            CurrentNamespaceModules = OldNamespaceModules;
            return PlannedSubMod.takeError();
        }
        // planModule leaves the loaded module on stack temporarily - capture it
        // Actually planModule pops it before returning, but we can get it from &SubMod
        // The SubMod reference points to the original module which is populated by Modeler
        CurrentNamespaceModules.push_back(&SubMod);
        Result.Modules.push_back(std::move(*PlannedSubMod));
    }

    // Plan members (Functions and Operators come from Members variant)
    for (const auto &Member : NS.Members) {
        if (auto *Func = std::get_if<Function>(&Member)) {
            auto PlannedFunc = planFunction(*Func, &ParentType);
            if (!PlannedFunc) {
                ModuleStack.pop_back();
                CurrentNamespaceName = OldNamespaceName;
                CurrentNamespace = OldNamespace;
                CurrentNamespaceModules = OldNamespaceModules;
                return PlannedFunc.takeError();
            }

            // Set fully qualified name for namespace functions
            PlannedFunc->Name = Name.str() + "." + PlannedFunc->Name;

            // Also add to InstantiatedFunctions for the emitter's lookup
            InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;

            Result.Functions.push_back(std::move(*PlannedFunc));
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, &ParentType);
            if (!PlannedOp) {
                ModuleStack.pop_back();
                CurrentNamespaceName = OldNamespaceName;
                CurrentNamespace = OldNamespace;
                CurrentNamespaceModules = OldNamespaceModules;
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));
        }
        // Skip Concept members for now
    }

    ModuleStack.pop_back();
    CurrentNamespaceName = OldNamespaceName;
    CurrentNamespace = OldNamespace;
    CurrentNamespaceModules = OldNamespaceModules;
    return Result;
}

// ============================================================================
// Concept Planning
// ============================================================================

llvm::Expected<PlannedConcept> Planner::planConcept(const Concept &Conc) {
    PlannedConcept Result;
    Result.Loc = Conc.Loc;
    Result.Name = Conc.Name;

    // Register in symbol table first (needed for recursive types)
    Concepts[Conc.Name] = &Conc;

    // For generic concepts, create a placeholder - don't fully plan
    // Generic concepts will be planned when instantiated with concrete types
    if (!Conc.Parameters.empty()) {
        // Create a minimal placeholder structure
        PlannedStructure Placeholder;
        Placeholder.Loc = Conc.Loc;
        Placeholder.Name = Conc.Name;
        Placeholder.MangledName = encodeName(Conc.Name);
        Placeholder.IsGenericPlaceholder = true;
        Result.Definition = std::move(Placeholder);
        Result.MangledName = Placeholder.MangledName;
        return Result;
    }

    // Plan attributes
    for (const auto &Attr : Conc.Attributes) {
        PlannedAttribute PA;
        PA.Loc = Attr.Loc;
        PA.Name = Attr.Name;
        PA.Value = cloneModel(Attr.Value);
        Result.Attributes.push_back(std::move(PA));
    }

    // Plan the definition based on its kind
    auto DefinitionResult = std::visit([this, &Conc](const auto &Def)
        -> llvm::Expected<std::variant<PlannedStructure, PlannedUnion,
                                        PlannedNamespace, PlannedGlobal, PlannedType>> {
        using T = std::decay_t<decltype(Def)>;

        if constexpr (std::is_same_v<T, Structure>) {
            // Check if structure is already being planned or fully planned
            auto ExistingIt = InstantiatedStructures.find(Conc.Name);
            if (ExistingIt != InstantiatedStructures.end()) {
                // Structure is already in cache - return it to avoid re-planning
                // This can happen when a method call triggers planConcept for the
                // same structure that's currently being planned
                return ExistingIt->second;
            }

            // Insert placeholder BEFORE planning to handle recursive planning
            // (e.g., when a method has LocalLifetime return type and triggers planConcept again)
            PlannedStructure Placeholder;
            Placeholder.Name = Conc.Name;
            Placeholder.MangledName = encodeName(Conc.Name);
            InstantiatedStructures[Conc.Name] = std::move(Placeholder);

            auto Planned = planStructure(Def, Conc.Name, {});
            if (!Planned) {
                InstantiatedStructures.erase(Conc.Name);  // Remove placeholder on error
                return Planned.takeError();
            }
            // Replace placeholder with actual planned structure
            InstantiatedStructures[Conc.Name] = *Planned;
            return *Planned;
        }
        else if constexpr (std::is_same_v<T, Union>) {
            // Check if union is already being planned or fully planned
            auto ExistingIt = InstantiatedUnions.find(Conc.Name);
            if (ExistingIt != InstantiatedUnions.end()) {
                return ExistingIt->second;
            }

            // Insert placeholder BEFORE planning to handle recursive planning
            PlannedUnion Placeholder;
            Placeholder.Name = Conc.Name;
            Placeholder.MangledName = encodeName(Conc.Name);
            InstantiatedUnions[Conc.Name] = std::move(Placeholder);

            auto Planned = planUnion(Def, Conc.Name, {});
            if (!Planned) {
                InstantiatedUnions.erase(Conc.Name);  // Remove placeholder on error
                return Planned.takeError();
            }
            // Replace placeholder with actual planned union
            InstantiatedUnions[Conc.Name] = *Planned;
            return *Planned;
        }
        else if constexpr (std::is_same_v<T, Namespace>) {
            auto Planned = planNamespace(Def, Conc.Name);
            if (!Planned) {
                return Planned.takeError();
            }
            return *Planned;
        }
        else if constexpr (std::is_same_v<T, Type>) {
            // Type alias
            auto Resolved = resolveType(Def, Conc.Loc);
            if (!Resolved) {
                return Resolved.takeError();
            }
            return *Resolved;
        }
        else if constexpr (std::is_same_v<T, Global>) {
            // Global constant - plan the type and value
            PlannedGlobal PG;
            PG.Loc = Conc.Loc;
            PG.Name = Conc.Name;
            PG.MangledName = encodeName(Conc.Name);

            // Resolve the global's type
            auto ResolvedType = resolveType(Def.GlobalType, Def.Loc);
            if (!ResolvedType) {
                return ResolvedType.takeError();
            }
            PG.GlobalType = std::move(*ResolvedType);

            // Plan the value operands
            for (const auto &Op : Def.Value) {
                auto PlannedOp = planOperand(Op);
                if (!PlannedOp) {
                    return PlannedOp.takeError();
                }
                PG.Value.push_back(std::move(*PlannedOp));
            }

            return PG;
        }
        else if constexpr (std::is_same_v<T, IntrinsicImpl>) {
            // Intrinsic implementation - not directly planned
            PlannedGlobal PG;
            PG.Loc = Conc.Loc;
            PG.Name = Conc.Name;
            PG.MangledName = encodeName(Conc.Name);
            return PG;
        }
        else {
            // Should not happen - all Definition variants handled
            PlannedGlobal PG;
            PG.Loc = Conc.Loc;
            PG.Name = Conc.Name;
            PG.MangledName = encodeName(Conc.Name);
            return PG;
        }
    }, Conc.Def);

    if (!DefinitionResult) {
        return DefinitionResult.takeError();
    }

    // Cache globals for lookup during expression planning
    if (std::holds_alternative<PlannedGlobal>(*DefinitionResult)) {
        PlannedGlobals[Conc.Name] = std::get<PlannedGlobal>(*DefinitionResult);
    }

    Result.Definition = std::move(*DefinitionResult);
    Result.MangledName = mangleStructure(Conc.Name, {});

    return Result;
}

// ============================================================================
// Module Planning
// ============================================================================

llvm::Expected<PlannedModule> Planner::planModule(const Module &Mod) {
    PlannedModule Result;
    Result.File = Mod.File;
    Result.Name = Mod.Name;

    // Save and update current file for error messages
    std::string OldFile = File;
    if (!Mod.File.empty()) {
        File = Mod.File;
    }

    // Push module onto stack for cross-module resolution
    ModuleStack.push_back(&Mod);

    // Plan sub-modules first (so their concepts are available)
    for (const auto &SubMod : Mod.Modules) {
        auto PlannedSubMod = planModule(SubMod);
        if (!PlannedSubMod) {
            File = OldFile;
            ModuleStack.pop_back();
            return PlannedSubMod.takeError();
        }
        Result.Modules.push_back(std::move(*PlannedSubMod));
    }

    // Plan members (Module.Members is vector<Member> where Member is variant)
    for (const auto &Member : Mod.Members) {
        if (auto *Pkg = std::get_if<Package>(&Member)) {
            // Package - plan the package's root module if loaded
            if (Pkg->Root) {
                auto PlannedPkgMod = planModule(*Pkg->Root);
                if (!PlannedPkgMod) {
                    File = OldFile;
                    ModuleStack.pop_back();
                    return PlannedPkgMod.takeError();
                }
                Result.Modules.push_back(std::move(*PlannedPkgMod));
            }
        } else if (auto *Conc = std::get_if<Concept>(&Member)) {
            // Register in symbol table first
            Concepts[Conc->Name] = Conc;

            // Plan concept
            auto PlannedConc = planConcept(*Conc);
            if (!PlannedConc) {
                File = OldFile;
                ModuleStack.pop_back();
                return PlannedConc.takeError();
            }
            Result.Concepts.push_back(std::move(*PlannedConc));
        } else if (auto *Func = std::get_if<Function>(&Member)) {
            auto PlannedFunc = planFunction(*Func, nullptr);
            if (!PlannedFunc) {
                File = OldFile;
                ModuleStack.pop_back();
                return PlannedFunc.takeError();
            }

            // Also add to InstantiatedFunctions for the emitter's lookup
            InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;

            Result.Functions.push_back(std::move(*PlannedFunc));

            // Register in symbol table
            Functions[Func->Name].push_back(Func);
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, nullptr);
            if (!PlannedOp) {
                File = OldFile;
                ModuleStack.pop_back();
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));

            // Register in symbol table (supports overloading)
            Operators[Op->Name].push_back(Op);
        }
    }

    File = OldFile;
    ModuleStack.pop_back();
    return Result;
}

// ============================================================================
// Main Entry Point
// ============================================================================

llvm::Expected<Plan> Planner::plan(const Program &Prog) {
    Plan Result;

    // Set up type inference context
    CurrentPlan = &Result;

    // Register C runtime functions (aligned_alloc, free, exit) for RBMM support
    registerRuntimeFunctions();

    // Load packages into the lookup map and plan them
    for (const auto &Pkg : Prog.Packages) {
        if (Pkg.Root) {
            LoadedPackages[Pkg.Name] = Pkg.Root.get();
            // Plan package module so its functions get added to InstantiatedFunctions
            auto PlannedPkgMod = planModule(*Pkg.Root);
            if (!PlannedPkgMod) {
                CurrentPlan = nullptr;
                return PlannedPkgMod.takeError();
            }
            // Functions are added to InstantiatedFunctions as a side effect of planning
        }
    }

    // Plan the main module
    auto PlannedMod = planModule(Prog.MainModule);
    if (!PlannedMod) {
        CurrentPlan = nullptr;
        return PlannedMod.takeError();
    }
    Result.MainModule = std::move(*PlannedMod);

    // Plan top-level statements (push a scope for local bindings)
    pushScope();
    auto PlannedStmts = planStatements(Prog.Statements);
    if (!PlannedStmts) {
        popScope();
        CurrentPlan = nullptr;
        return PlannedStmts.takeError();
    }
    Result.Statements = std::move(*PlannedStmts);
    popScope();

    // Solve any accumulated type constraints
    auto SolveResult = solveAndApply();
    if (!SolveResult) {
        CurrentPlan = nullptr;
        return SolveResult.takeError();
    }

    // Copy instantiated types to Plan's lookup maps
    for (auto &Pair : InstantiatedStructures) {
        Result.Structures[Pair.first] = Pair.second;
    }
    for (auto &Pair : InstantiatedUnions) {
        Result.Unions[Pair.first] = Pair.second;
    }
    for (auto &Pair : InstantiatedFunctions) {
        Result.Functions[Pair.first] = Pair.second;
    }
    for (auto &Pair : PlannedGlobals) {
        Result.Globals[Pair.first] = Pair.second;
    }

    CurrentPlan = nullptr;
    return Result;
}

// ============================================================================
// Type Inference Utilities (free functions declared in Plan.h)
// ============================================================================

PlannedType applySubstitution(const PlannedType &Type, const Substitution &Subst) {
    // If this is a type variable, look it up
    if (Type.Variable) {
        auto It = Subst.find(Type.Variable->Id);
        if (It != Subst.end()) {
            // Recursively apply to handle chains
            return applySubstitution(It->second, Subst);
        }
        // Unresolved variable - return as-is
        return Type;
    }

    // Apply to generic arguments
    PlannedType Result = Type;
    Result.Generics.clear();
    for (const auto &Arg : Type.Generics) {
        Result.Generics.push_back(applySubstitution(Arg, Subst));
    }

    return Result;
}

std::set<uint64_t> freeTypeVars(const PlannedType &Type) {
    std::set<uint64_t> Result;

    if (Type.Variable) {
        Result.insert(Type.Variable->Id);
    }

    for (const auto &Arg : Type.Generics) {
        auto ArgVars = freeTypeVars(Arg);
        Result.insert(ArgVars.begin(), ArgVars.end());
    }

    return Result;
}

bool occursIn(uint64_t VarId, const PlannedType &Type) {
    if (Type.Variable && Type.Variable->Id == VarId) {
        return true;
    }

    for (const auto &Arg : Type.Generics) {
        if (occursIn(VarId, Arg)) {
            return true;
        }
    }

    return false;
}

// Compose two substitutions: apply S2 after S1
Substitution composeSubstitutions(const Substitution &S1, const Substitution &S2) {
    Substitution Result = S2;

    for (const auto &[Id, Type] : S1) {
        Result[Id] = applySubstitution(Type, S2);
    }

    return Result;
}

// Check if a type name is an integer type
bool isIntegerTypeName(const std::string &Name) {
    static const std::set<std::string> IntTypes = {
        "int", "i8", "i16", "i32", "i64",
        "uint", "u8", "u16", "u32", "u64",
        "size_t", "usize", "isize", "size",
        "char"  // char is an integer type in C/C++ semantics
    };
    return IntTypes.count(Name) > 0;
}

// Check if two types are compatible integer types (for implicit coercion)
// This allows integer literals (typed as 'int') to be used where size_t, u64, etc. are expected
bool areCompatibleIntegerTypes(const std::string &Left, const std::string &Right) {
    return isIntegerTypeName(Left) && isIntegerTypeName(Right);
}

llvm::Expected<Substitution> unify(const TypeOrVariable &Left,
                                    const TypeOrVariable &Right,
                                    Span Loc,
                                    llvm::StringRef File) {
    // Get types from variants
    const PlannedType *LeftType = std::get_if<PlannedType>(&Left);
    const PlannedType *RightType = std::get_if<PlannedType>(&Right);
    const TypeVariable *LeftVar = std::get_if<TypeVariable>(&Left);
    const TypeVariable *RightVar = std::get_if<TypeVariable>(&Right);

    // Check if PlannedTypes contain type variables (embedded variable)
    // This handles the case where a type variable is wrapped in PlannedType
    const TypeVariable *LeftEmbeddedVar = nullptr;
    const TypeVariable *RightEmbeddedVar = nullptr;
    if (LeftType && LeftType->Variable) {
        LeftEmbeddedVar = LeftType->Variable.get();
    }
    if (RightType && RightType->Variable) {
        RightEmbeddedVar = RightType->Variable.get();
    }

    // Use embedded variables if present
    if (LeftEmbeddedVar) LeftVar = LeftEmbeddedVar;
    if (RightEmbeddedVar) RightVar = RightEmbeddedVar;

    // Case 1: Both are the same variable
    if (LeftVar && RightVar && LeftVar->Id == RightVar->Id) {
        return Substitution{};
    }

    // Case 2: Left is a variable
    if (LeftVar) {
        // For occurs check, use RightType only if it's not just a variable wrapper
        if (RightType && !RightEmbeddedVar && occursIn(LeftVar->Id, *RightType)) {
            return makeInfiniteTypeError(File, Loc, LeftVar->DebugName);
        }
        Substitution S;
        if (RightType && !RightEmbeddedVar) {
            S[LeftVar->Id] = *RightType;
        } else if (RightVar) {
            PlannedType VarType;
            VarType.Variable = std::make_shared<TypeVariable>(*RightVar);
            S[LeftVar->Id] = VarType;
        }
        return S;
    }

    // Case 3: Right is a variable
    if (RightVar) {
        if (LeftType && !LeftEmbeddedVar && occursIn(RightVar->Id, *LeftType)) {
            return makeInfiniteTypeError(File, Loc, RightVar->DebugName);
        }
        Substitution S;
        if (LeftType && !LeftEmbeddedVar) {
            S[RightVar->Id] = *LeftType;
        }
        return S;
    }

    // Case 4: Both are concrete types
    if (LeftType && RightType) {
        // Special case: void unifies with anything (for null pointer compatibility)
        // This allows pointer[void] (null) to unify with pointer[T] for any T
        if (LeftType->Name == "void" || RightType->Name == "void") {
            return Substitution{};
        }

        // Special case: compatible integer types unify with each other
        // This allows integer literals (typed as 'int') to be used where size_t, u64, etc. are expected
        if (areCompatibleIntegerTypes(LeftType->Name, RightType->Name)) {
            return Substitution{};
        }

        // Names must match
        if (LeftType->Name != RightType->Name) {
            return makeTypeMismatchError(File, Loc, LeftType->Name, RightType->Name);
        }

        // Arity must match
        if (LeftType->Generics.size() != RightType->Generics.size()) {
            return makeGenericArityError(File, Loc, LeftType->Name,
                                          LeftType->Generics.size(),
                                          RightType->Generics.size());
        }

        // Unify all generic arguments
        Substitution Result;
        for (size_t I = 0; I < LeftType->Generics.size(); ++I) {
            auto SubResult = unify(LeftType->Generics[I], RightType->Generics[I],
                                   Loc, File);
            if (!SubResult) {
                return SubResult.takeError();
            }
            Result = composeSubstitutions(Result, *SubResult);
        }

        return Result;
    }

    // Shouldn't reach here
    return Substitution{};
}

llvm::Expected<Substitution> solveConstraints(
    const std::vector<TypeConstraint> &Constraints, llvm::StringRef File) {

    Substitution Solution;

    for (const auto &Constraint : Constraints) {
        if (auto *Eq = std::get_if<EqualityConstraint>(&Constraint)) {
            // Apply current solution to both sides
            TypeOrVariable Left = Eq->Left;
            TypeOrVariable Right = Eq->Right;

            if (auto *LT = std::get_if<PlannedType>(&Left)) {
                Left = applySubstitution(*LT, Solution);
            }
            if (auto *RT = std::get_if<PlannedType>(&Right)) {
                Right = applySubstitution(*RT, Solution);
            }

            auto UnifyResult = unify(Left, Right, Eq->Loc, Eq->File);
            if (!UnifyResult) {
                return UnifyResult.takeError();
            }

            Solution = composeSubstitutions(Solution, *UnifyResult);
        }
        else if (auto *Inst = std::get_if<InstanceConstraint>(&Constraint)) {
            // TODO: Handle trait constraints
            // For now, just skip them
            (void)Inst;
        }
    }

    return Solution;
}

TypeScheme generalize(const PlannedType &Type,
                      const std::set<uint64_t> &FreeInEnvironment) {
    TypeScheme Result;
    Result.Type = std::make_shared<PlannedType>(Type);

    // Find all type variables in the type that are NOT in the environment
    auto TypeVars = freeTypeVars(Type);
    for (uint64_t Id : TypeVars) {
        if (FreeInEnvironment.find(Id) == FreeInEnvironment.end()) {
            Result.Quantified.push_back(TypeVariable{Id, ""});
        }
    }

    return Result;
}

PlannedType instantiate(const TypeScheme &Scheme, Plan &P) {
    if (!Scheme.Type) {
        return PlannedType{};
    }

    // Create fresh type variables for each quantified variable
    Substitution Fresh;
    for (const auto &QVar : Scheme.Quantified) {
        TypeVariable NewVar = P.freshTypeVar(QVar.DebugName);
        PlannedType VarType;
        VarType.Variable = std::make_shared<TypeVariable>(NewVar);
        Fresh[QVar.Id] = VarType;
    }

    return applySubstitution(*Scheme.Type, Fresh);
}

} // namespace scaly
