// Planner.cpp - Generic resolution, type inference, and name mangling
// Transforms Model (with generics) → Plan (concrete types, mangled names)

#include "Planner.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <sstream>

namespace scaly {

// ============================================================================
// Constructor
// ============================================================================

Planner::Planner(llvm::StringRef FileName) : File(FileName.str()) {}

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
        Result.MemberAccess = std::make_shared<std::vector<std::string>>(*Op.MemberAccess);
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
    if (Type.Name == "size") return "m";  // size_t → unsigned long

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

    // Generic types: List[int] → I4ListIiEE
    if (!Type.Generics.empty()) {
        std::string Result = "I" + encodeName(Type.Name);
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

    std::string Result = "_ZI" + encodeName(Name);
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
    if (Params.empty()) {
        Result += "v";  // void parameters
    } else {
        for (const auto &Param : Params) {
            if (Param.ItemType) {
                Result += encodeType(*Param.ItemType);
            }
        }
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

    // Parameter types
    if (Params.empty()) {
        Result += "v";
    } else {
        for (const auto &Param : Params) {
            if (Param.ItemType) {
                Result += encodeType(*Param.ItemType);
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
}

void Planner::popScope() {
    if (!Scopes.empty()) {
        Scopes.pop_back();
    }
}

void Planner::defineLocal(llvm::StringRef Name, const PlannedType &Type) {
    if (!Scopes.empty()) {
        Scopes.back()[Name.str()] = Type;
    }
}

std::optional<PlannedType> Planner::lookupLocal(llvm::StringRef Name) {
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

bool Planner::typesEqual(const PlannedType &A, const PlannedType &B) {
    if (A.Name != B.Name) return false;
    if (A.Generics.size() != B.Generics.size()) return false;

    for (size_t I = 0; I < A.Generics.size(); ++I) {
        if (!typesEqual(A.Generics[I], B.Generics[I])) {
            return false;
        }
    }
    return true;
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
// Cross-Module Concept Lookup
// ============================================================================

const Concept* Planner::lookupConcept(llvm::StringRef Name) {
    // 1. Check flat cache first (most efficient)
    auto CacheIt = Concepts.find(Name.str());
    if (CacheIt != Concepts.end()) {
        return CacheIt->second;
    }

    // 2. Search current module stack (innermost to outermost)
    for (auto It = ModuleStack.rbegin(); It != ModuleStack.rend(); ++It) {
        const Module* Mod = *It;

        // Check this module's members for a matching concept
        for (const auto& Member : Mod->Members) {
            if (auto* Conc = std::get_if<Concept>(&Member)) {
                if (Conc->Name == Name) {
                    // Cache it for future lookups
                    Concepts[Name.str()] = Conc;
                    return Conc;
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
        if (!Resolved) {
            return Resolved.takeError();
        }
        return *Resolved;
    }

    // Restore old substitutions
    TypeSubstitutions = OldSubst;

    if (!Success) {
        return makePlannerNotImplementedError(File, InstantiationLoc,
            "generic instantiation for this definition kind");
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

    // Plan the expression
    auto PlannedExpr = planExpression(Op.Expr);
    if (!PlannedExpr) {
        return PlannedExpr.takeError();
    }
    Result.Expr = std::move(*PlannedExpr);

    // Copy member access path
    if (Op.MemberAccess) {
        Result.MemberAccess = std::make_shared<std::vector<std::string>>(
            *Op.MemberAccess);
    }

    // TODO: Compute ResultType via type inference

    return Result;
}

llvm::Expected<std::vector<PlannedOperand>> Planner::planOperands(
    const std::vector<Operand> &Ops) {

    std::vector<PlannedOperand> Result;
    Result.reserve(Ops.size());

    for (const auto &Op : Ops) {
        auto Planned = planOperand(Op);
        if (!Planned) {
            return Planned.takeError();
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
            auto Resolved = resolveType(E, E.Loc);
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
            PSO.Size = 0;  // TODO: Compute actual size
            return PlannedExpression(std::move(PSO));
        }
        else if constexpr (std::is_same_v<T, Is>) {
            PlannedIs PI;
            PI.Loc = E.Loc;
            // Is has Name (vector<string>), not TestType
            auto Resolved = resolveTypePath(E.Name, E.Loc);
            if (!Resolved) {
                return Resolved.takeError();
            }
            PI.TestType = std::move(*Resolved);
            return PlannedExpression(std::move(PI));
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
        PC.Value = std::move(*PlannedValue);

        for (const auto &Attr : Comp.Attributes) {
            PlannedAttribute PA;
            PA.Loc = Attr.Loc;
            PA.Name = Attr.Name;
            PA.Value = cloneModel(Attr.Value);
            PC.Attributes.push_back(std::move(PA));
        }

        Result.Components.push_back(std::move(PC));
    }

    // TODO: Compute TupleType

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
                PR.Result = std::move(*PlannedResult);
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
    Result.Source = std::move(*PlannedSource);

    auto PlannedTarget = planOperands(Act.Target);
    if (!PlannedTarget) {
        return PlannedTarget.takeError();
    }
    Result.Target = std::move(*PlannedTarget);

    return Result;
}

llvm::Expected<PlannedBinding> Planner::planBinding(const Binding &Bind) {
    PlannedBinding Result;
    Result.Loc = Bind.Loc;
    Result.BindingType = Bind.BindingType;

    auto PlannedItem = planItem(Bind.BindingItem);
    if (!PlannedItem) {
        return PlannedItem.takeError();
    }
    Result.BindingItem = std::move(*PlannedItem);

    auto PlannedOp = planOperands(Bind.Operation);
    if (!PlannedOp) {
        return PlannedOp.takeError();
    }
    Result.Operation = std::move(*PlannedOp);

    // Add to scope
    if (Result.BindingItem.Name && Result.BindingItem.ItemType) {
        defineLocal(*Result.BindingItem.Name, *Result.BindingItem.ItemType);
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
    Result.Condition = std::move(*PlannedCond);

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

    for (const auto &Case : ChooseExpr.Cases) {
        PlannedWhen PW;
        PW.Loc = Case.Loc;
        PW.Name = Case.Name;

        // When has VariantPath (vector<string>), not VariantType
        auto ResolvedType = resolveTypePath(Case.VariantPath, Case.Loc);
        if (!ResolvedType) {
            return ResolvedType.takeError();
        }
        PW.VariantType = std::move(*ResolvedType);

        if (Case.Consequent) {
            auto PlannedCons = planStatement(*Case.Consequent);
            if (!PlannedCons) {
                return PlannedCons.takeError();
            }
            PW.Consequent = std::make_unique<PlannedStatement>(std::move(*PlannedCons));
        }

        Result.Cases.push_back(std::move(PW));
    }

    if (ChooseExpr.Alternative) {
        auto PlannedAlt = planStatement(*ChooseExpr.Alternative);
        if (!PlannedAlt) {
            return PlannedAlt.takeError();
        }
        Result.Alternative = std::make_unique<PlannedStatement>(std::move(*PlannedAlt));
    }

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
    // TODO: Define loop variable in scope with inferred type

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

    for (const auto &Catch : TryExpr.Catches) {
        PlannedWhen PW;
        PW.Loc = Catch.Loc;
        PW.Name = Catch.Name;

        // When has VariantPath (vector<string>), not VariantType
        auto ResolvedType = resolveTypePath(Catch.VariantPath, Catch.Loc);
        if (!ResolvedType) {
            popScope();
            return ResolvedType.takeError();
        }
        PW.VariantType = std::move(*ResolvedType);

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

    pushScope();

    // Plan input parameters
    for (const auto &Param : Func.Input) {
        auto PlannedParam = planItem(Param);
        if (!PlannedParam) {
            popScope();
            return PlannedParam.takeError();
        }
        Result.Input.push_back(std::move(*PlannedParam));

        // Add parameter to scope
        if (Result.Input.back().Name && Result.Input.back().ItemType) {
            defineLocal(*Result.Input.back().Name, *Result.Input.back().ItemType);
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
    }

    // Plan throws type
    if (Func.Throws) {
        auto ResolvedThrows = resolveType(*Func.Throws, Func.Loc);
        if (!ResolvedThrows) {
            popScope();
            return ResolvedThrows.takeError();
        }
        Result.Throws = std::make_shared<PlannedType>(std::move(*ResolvedThrows));
    }

    // Plan implementation (function body)
    auto PlannedImpl = planImplementation(Func.Impl);
    if (!PlannedImpl) {
        popScope();
        return PlannedImpl.takeError();
    }
    Result.Impl = std::move(*PlannedImpl);

    popScope();

    // Generate mangled name
    Result.MangledName = mangleFunction(Func.Name, Result.Input, Parent);

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

    // Plan input parameters
    for (const auto &Param : Op.Input) {
        auto PlannedParam = planItem(Param);
        if (!PlannedParam) {
            popScope();
            return PlannedParam.takeError();
        }
        Result.Input.push_back(std::move(*PlannedParam));

        // Add parameter to scope
        if (Result.Input.back().Name && Result.Input.back().ItemType) {
            defineLocal(*Result.Input.back().Name, *Result.Input.back().ItemType);
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

    pushScope();

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
    if (Result.Input.empty()) {
        Mangled += "v";
    } else {
        for (const auto &P : Result.Input) {
            if (P.ItemType) {
                Mangled += encodeType(*P.ItemType);
            }
        }
    }
    Result.MangledName = Mangled;

    return Result;
}

llvm::Expected<PlannedDeInitializer> Planner::planDeInitializer(
    const DeInitializer &DeInit, const PlannedType &Parent) {

    PlannedDeInitializer Result;
    Result.Loc = DeInit.Loc;

    // Plan implementation (deinitializer body)
    auto PlannedImpl = planImplementation(DeInit.Impl);
    if (!PlannedImpl) {
        return PlannedImpl.takeError();
    }
    Result.Impl = std::move(*PlannedImpl);

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

    // Plan initializers
    for (const auto &Init : Struct.Initializers) {
        auto PlannedInit = planInitializer(Init, ParentType);
        if (!PlannedInit) {
            TypeSubstitutions = OldSubst;
            return PlannedInit.takeError();
        }
        Result.Initializers.push_back(std::move(*PlannedInit));
    }

    // Plan deinitializer
    if (Struct.Deinitializer) {
        auto PlannedDeInit = planDeInitializer(*Struct.Deinitializer, ParentType);
        if (!PlannedDeInit) {
            TypeSubstitutions = OldSubst;
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
                return PlannedMethod.takeError();
            }
            Result.Methods.push_back(std::move(*PlannedMethod));
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, &ParentType);
            if (!PlannedOp) {
                TypeSubstitutions = OldSubst;
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));
        }
        // Skip Concept members for now - would need recursive planning
    }

    // Restore substitutions
    TypeSubstitutions = OldSubst;

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

    // Create a pseudo-module wrapper for the namespace to enable cross-module lookup
    // This allows functions in the namespace to find concepts from sub-modules
    Module PseudoModule;
    PseudoModule.Name = Name.str();
    PseudoModule.Modules = NS.Modules;
    // Copy member concepts into a format compatible with Module.Members
    for (const auto &Member : NS.Members) {
        if (auto *Conc = std::get_if<Concept>(&Member)) {
            PseudoModule.Members.push_back(*Conc);
        }
    }

    // Push pseudo-module for cross-module resolution
    ModuleStack.push_back(&PseudoModule);

    // Plan sub-modules first (so their concepts are available)
    for (const auto &SubMod : NS.Modules) {
        auto PlannedSubMod = planModule(SubMod);
        if (!PlannedSubMod) {
            ModuleStack.pop_back();
            return PlannedSubMod.takeError();
        }
        Result.Modules.push_back(std::move(*PlannedSubMod));
    }

    // Plan members (Functions and Operators come from Members variant)
    for (const auto &Member : NS.Members) {
        if (auto *Func = std::get_if<Function>(&Member)) {
            auto PlannedFunc = planFunction(*Func, nullptr);
            if (!PlannedFunc) {
                ModuleStack.pop_back();
                return PlannedFunc.takeError();
            }
            Result.Functions.push_back(std::move(*PlannedFunc));
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, nullptr);
            if (!PlannedOp) {
                ModuleStack.pop_back();
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));
        }
        // Skip Concept members for now
    }

    ModuleStack.pop_back();
    return Result;
}

// ============================================================================
// Concept Planning
// ============================================================================

llvm::Expected<PlannedConcept> Planner::planConcept(const Concept &Conc) {
    PlannedConcept Result;
    Result.Loc = Conc.Loc;
    Result.Name = Conc.Name;

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
            auto Planned = planStructure(Def, Conc.Name, {});
            if (!Planned) {
                return Planned.takeError();
            }
            return *Planned;
        }
        else if constexpr (std::is_same_v<T, Union>) {
            auto Planned = planUnion(Def, Conc.Name, {});
            if (!Planned) {
                return Planned.takeError();
            }
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
        else {
            // Global or other
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

    Result.Definition = std::move(*DefinitionResult);
    Result.MangledName = mangleStructure(Conc.Name, {});

    // Register in symbol table
    Concepts[Conc.Name] = &Conc;

    return Result;
}

// ============================================================================
// Module Planning
// ============================================================================

llvm::Expected<PlannedModule> Planner::planModule(const Module &Mod) {
    PlannedModule Result;
    Result.File = Mod.File;
    Result.Name = Mod.Name;

    // Push module onto stack for cross-module resolution
    ModuleStack.push_back(&Mod);

    // Plan sub-modules first (so their concepts are available)
    for (const auto &SubMod : Mod.Modules) {
        auto PlannedSubMod = planModule(SubMod);
        if (!PlannedSubMod) {
            ModuleStack.pop_back();
            return PlannedSubMod.takeError();
        }
        Result.Modules.push_back(std::move(*PlannedSubMod));
    }

    // Plan members (Module.Members is vector<Member> where Member is variant)
    for (const auto &Member : Mod.Members) {
        if (auto *Packages = std::get_if<std::vector<Module>>(&Member)) {
            // Package - plan sub-modules
            for (const auto &SubMod : *Packages) {
                auto PlannedSubMod = planModule(SubMod);
                if (!PlannedSubMod) {
                    ModuleStack.pop_back();
                    return PlannedSubMod.takeError();
                }
                Result.Modules.push_back(std::move(*PlannedSubMod));
            }
        } else if (auto *Conc = std::get_if<Concept>(&Member)) {
            // Register in symbol table first
            Concepts[Conc->Name] = Conc;

            // Plan concept
            auto PlannedConc = planConcept(*Conc);
            if (!PlannedConc) {
                ModuleStack.pop_back();
                return PlannedConc.takeError();
            }
            Result.Concepts.push_back(std::move(*PlannedConc));
        } else if (auto *Func = std::get_if<Function>(&Member)) {
            auto PlannedFunc = planFunction(*Func, nullptr);
            if (!PlannedFunc) {
                ModuleStack.pop_back();
                return PlannedFunc.takeError();
            }
            Result.Functions.push_back(std::move(*PlannedFunc));

            // Register in symbol table
            Functions[Func->Name].push_back(Func);
        } else if (auto *Op = std::get_if<Operator>(&Member)) {
            auto PlannedOp = planOperator(*Op, nullptr);
            if (!PlannedOp) {
                ModuleStack.pop_back();
                return PlannedOp.takeError();
            }
            Result.Operators.push_back(std::move(*PlannedOp));

            // Register in symbol table
            Operators[Op->Name] = Op;
        }
    }

    ModuleStack.pop_back();
    return Result;
}

// ============================================================================
// Main Entry Point
// ============================================================================

llvm::Expected<Plan> Planner::plan(const Program &Prog) {
    Plan Result;

    // Plan the main module
    auto PlannedMod = planModule(Prog.MainModule);
    if (!PlannedMod) {
        return PlannedMod.takeError();
    }
    Result.MainModule = std::move(*PlannedMod);

    // Plan top-level statements
    auto PlannedStmts = planStatements(Prog.Statements);
    if (!PlannedStmts) {
        return PlannedStmts.takeError();
    }
    Result.Statements = std::move(*PlannedStmts);

    // Copy instantiated types to Plan's lookup maps
    for (auto &Pair : InstantiatedStructures) {
        Result.Structures[Pair.first] = &Pair.second;
    }
    for (auto &Pair : InstantiatedUnions) {
        Result.Unions[Pair.first] = &Pair.second;
    }
    for (auto &Pair : InstantiatedFunctions) {
        Result.Functions[Pair.first] = &Pair.second;
    }

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

llvm::Expected<Substitution> unify(const TypeOrVariable &Left,
                                    const TypeOrVariable &Right,
                                    Span Loc,
                                    llvm::StringRef File) {
    // Get types from variants
    const PlannedType *LeftType = std::get_if<PlannedType>(&Left);
    const PlannedType *RightType = std::get_if<PlannedType>(&Right);
    const TypeVariable *LeftVar = std::get_if<TypeVariable>(&Left);
    const TypeVariable *RightVar = std::get_if<TypeVariable>(&Right);

    // Case 1: Both are the same variable
    if (LeftVar && RightVar && LeftVar->Id == RightVar->Id) {
        return Substitution{};
    }

    // Case 2: Left is a variable
    if (LeftVar) {
        if (RightType && occursIn(LeftVar->Id, *RightType)) {
            return makeInfiniteTypeError(File, Loc, LeftVar->DebugName);
        }
        Substitution S;
        if (RightType) {
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
        if (LeftType && occursIn(RightVar->Id, *LeftType)) {
            return makeInfiniteTypeError(File, Loc, RightVar->DebugName);
        }
        Substitution S;
        if (LeftType) {
            S[RightVar->Id] = *LeftType;
        }
        return S;
    }

    // Case 4: Both are concrete types
    if (LeftType && RightType) {
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

            auto UnifyResult = unify(Left, Right, Eq->Loc, File);
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
