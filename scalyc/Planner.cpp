// Planner.cpp - Generic resolution, type inference, and name mangling
// Transforms Model (with generics) → Plan (concrete types, mangled names)

#include "Planner.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <set>
#include <sstream>

namespace scaly {

// ============================================================================
// Constructor
// ============================================================================

Planner::Planner(llvm::StringRef FileName) : File(FileName.str()) {}

void Planner::addSiblingProgram(std::shared_ptr<Program> Prog) {
    SiblingPrograms.push_back(Prog);
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

void Planner::defineLocal(llvm::StringRef Name, const PlannedType &Type, bool IsMutable) {
    if (!Scopes.empty()) {
        Scopes.back()[Name.str()] = LocalBinding{Type, IsMutable};
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

std::vector<const Function*> Planner::lookupFunction(llvm::StringRef Name) {
    // Check function cache
    auto It = Functions.find(Name.str());
    if (It != Functions.end()) {
        return It->second;
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
    if (Result.empty() && !CurrentStructureName.empty()) {
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

    // Cache the result
    if (!Result.empty()) {
        Functions[Name.str()] = Result;
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

    // Operator names start with operator characters
    char First = Name[0];
    return First == '+' || First == '-' || First == '*' || First == '/' ||
           First == '%' || First == '=' || First == '<' || First == '>' ||
           First == '!' || First == '&' || First == '|' || First == '^' ||
           First == '~' || First == '[' || First == '(';
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
    const Concept *Conc = lookupConcept(StructType.Name);
    if (Conc) {
        if (auto *ModelStruct = std::get_if<Structure>(&Conc->Def)) {
            for (const auto &Member : ModelStruct->Members) {
                if (auto *Func = std::get_if<Function>(&Member)) {
                    if (Func->Name == MethodName) {
                        MethodMatch Match;
                        Match.Method = Func;

                        // Compute mangled name
                        PlannedType ParentType;
                        ParentType.Name = StructType.Name;
                        // Use Struct's mangled name if available, otherwise use StructType's
                        ParentType.MangledName = Struct ? Struct->MangledName :
                            (StructType.MangledName.empty() ? StructType.Name : StructType.MangledName);
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
                        return Match;
                    }
                }
            }
        }
    }

    return std::nullopt;
}

std::optional<Planner::InitializerMatch> Planner::findInitializer(
    const PlannedType &StructType,
    const std::vector<PlannedType> &ArgTypes) {

    // Look up the struct in the instantiation cache
    auto StructIt = InstantiatedStructures.find(StructType.Name);
    if (StructIt == InstantiatedStructures.end()) {
        // Also try the mangled name
        StructIt = InstantiatedStructures.find(StructType.MangledName);
        if (StructIt == InstantiatedStructures.end()) {
            return std::nullopt;
        }
    }

    const PlannedStructure &Struct = StructIt->second;

    // Search for matching initializer
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
            if (!typesEqual(*Init.Input[i].ItemType, ArgTypes[i])) {
                AllMatch = false;
                break;
            }
        }

        if (AllMatch) {
            InitializerMatch Match;
            Match.Init = &Init;
            Match.MangledName = Init.MangledName;
            Match.StructType = StructType;
            return Match;
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

    bool IsPrimitiveType = (Left.Name == "i64" || Left.Name == "i32" ||
                            Left.Name == "i16" || Left.Name == "i8" ||
                            Left.Name == "u64" || Left.Name == "u32" ||
                            Left.Name == "u16" || Left.Name == "u8" ||
                            Left.Name == "f64" || Left.Name == "f32" ||
                            Left.Name == "bool" ||
                            Left.Name == "int" || Left.Name == "float" ||
                            Left.Name == "double" || Left.Name == "size_t");

    if (IsPrimitiveType && typesEqual(Left, Right)) {
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

            if (!typesEqual(*ParamTypeResult, ArgTypes[I])) {
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

                if (!typesEqual(*ParamTypeResult, ArgTypes[I])) {
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

    // Check for pointer[T] in name form (e.g., from unresolved types)
    if (PtrType.Name.substr(0, 8) == "pointer[") {
        // This shouldn't happen after resolution, but handle it gracefully
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

    // Handle prefix operator (e.g., -x, !x)
    if (auto* TypeExpr = std::get_if<PlannedType>(&Ops[0].Expr)) {
        if (isOperatorName(TypeExpr->Name) && Ops.size() >= 2) {
            // Prefix operator: collapse rest first, then apply prefix
            std::vector<PlannedOperand> RestOps(Ops.begin() + 1, Ops.end());
            auto RestResult = collapseOperandSequence(std::move(RestOps));
            if (!RestResult) {
                return RestResult.takeError();
            }

            // Create PlannedCall for prefix operator
            PlannedCall Call;
            Call.Loc = Ops[0].Loc;
            Call.Name = TypeExpr->Name;
            Call.IsOperator = true;
            Call.IsIntrinsic = true;  // Prefix operators on primitives are intrinsic

            Call.Args = std::make_shared<std::vector<PlannedOperand>>();
            Call.Args->push_back(std::move(*RestResult));

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
                    // Not a pointer type - error will be caught later
                    Call.ResultType = OperandType;
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

            PlannedOperand Result;
            Result.Loc = Call.Loc;
            Result.ResultType = Call.ResultType;
            Result.Expr = std::move(Call);
            return Result;
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
                PlannedOperand& Right = Ops[I + 1];

                // Find the operator
                auto OpMatch = findOperator(TypeExpr->Name, Left.ResultType, Right.ResultType);

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
                    I += 2;  // Skip operator and right operand
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
        // Extract base name from instantiated type (e.g., "Node.int" -> "Node")
        std::string BaseName = Current.Name;
        size_t DotPos = BaseName.find('.');
        if (DotPos != std::string::npos) {
            BaseName = BaseName.substr(0, DotPos);
        }

        // Look up the base type as a concept
        const Concept* Conc = lookupConcept(BaseName);
        if (!Conc) {
            return makePlannerNotImplementedError(File, Loc,
                "cannot access member '" + MemberName + "' on type " +
                 Current.Name);
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
        // Extract base name from instantiated type (e.g., "Node.int" -> "Node")
        std::string BaseName = Current.Name;
        size_t DotPos = BaseName.find('.');
        if (DotPos != std::string::npos) {
            BaseName = BaseName.substr(0, DotPos);
        }

        // Look up the base type as a concept
        const Concept* Conc = lookupConcept(BaseName);
        if (!Conc) {
            return makePlannerNotImplementedError(File, Loc,
                "cannot access member '" + MemberName + "' on type " +
                 Current.Name);
        }

        // Check if it's a structure with properties
        if (auto* Struct = std::get_if<Structure>(&Conc->Def)) {
            bool Found = false;
            size_t FieldIndex = 0;
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

                    PlannedMemberAccess Access;
                    Access.Name = MemberName;
                    Access.FieldIndex = FieldIndex;
                    Access.ResultType = std::move(*Resolved);
                    Result.push_back(std::move(Access));

                    Current = Result.back().ResultType;
                    Found = true;
                    break;
                }
                FieldIndex++;
            }
            if (Found) continue;
        }

        // Member not found
        return makePlannerNotImplementedError(File, Loc,
            "member '" + MemberName + "' not found in type " +
             Current.Name);
    }

    return Result;
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
            Result.Loc = Val.Loc;
            Result.Name = "String";
            Result.MangledName = "6String";
        }
        else if constexpr (std::is_same_v<T, CharacterConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "char";
            Result.MangledName = "c";
        }
        else if constexpr (std::is_same_v<T, FragmentConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "String";  // Fragments are strings
            Result.MangledName = "6String";
        }
        else if constexpr (std::is_same_v<T, NullConstant>) {
            Result.Loc = Val.Loc;
            Result.Name = "pointer";  // Generic null pointer
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
            return Result;
        }

        // Check if it's a structure property (when inside a method via 'this')
        if (CurrentStructureProperties) {
            // Check if 'this' is in scope
            auto ThisType = lookupLocal("this");
            if (ThisType && ThisType->Name == CurrentStructureName) {
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
        else if constexpr (std::is_same_v<T, PlannedIs>) {
            // 'is' always returns bool
            PlannedType BoolType;
            BoolType.Loc = E.Loc;
            BoolType.Name = "bool";
            BoolType.MangledName = "b";
            return BoolType;
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
    std::vector<std::string> ImplicitMemberAccess;
    Operand ModifiedOp = Op;

    if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
        if (TypeExpr->Name.size() > 1 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
            // Check if the first element is a local variable
            auto LocalBind = lookupLocalBinding(TypeExpr->Name[0]);
            if (LocalBind) {
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
                    ImplicitMemberAccess.push_back(TypeExpr->Name[i]);
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
    std::vector<std::string> CombinedMemberAccess = ImplicitMemberAccess;
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

    std::vector<PlannedOperand> Result;
    Result.reserve(Ops.size());

    for (size_t i = 0; i < Ops.size(); ++i) {
        const auto &Op = Ops[i];

        // Check for method call pattern: variable.method followed by Tuple
        // e.g., p.distance() where p is a variable of struct type
        if (i + 1 < Ops.size()) {
            const auto &NextOp = Ops[i + 1];

            // Check if current op is a Type path like ["p", "method"] where "p" is a variable
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                if (TypeExpr->Name.size() >= 2 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
                    // Check if first element is a local variable
                    auto LocalBind = lookupLocalBinding(TypeExpr->Name[0]);
                    if (LocalBind && std::holds_alternative<Tuple>(NextOp.Expr)) {
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

                        // Look up the method on the variable's type
                        auto MethodMatch = lookupMethod(PlannedVar->ResultType, MethodName, Op.Loc);
                        if (MethodMatch) {
                            // Plan the arguments tuple
                            Operand ArgsOp = NextOp;
                            ArgsOp.MemberAccess = nullptr;
                            auto PlannedArgs = planOperand(ArgsOp);
                            if (!PlannedArgs) {
                                return PlannedArgs.takeError();
                            }

                            // Create method call with instance as first argument
                            PlannedCall Call;
                            Call.Loc = Op.Loc;
                            Call.Name = MethodName;
                            Call.MangledName = MethodMatch->MangledName;
                            Call.IsIntrinsic = false;  // Methods are not intrinsic
                            Call.IsOperator = false;
                            Call.ResultType = MethodMatch->ReturnType;

                            // Build args: [instance, ...tuple_args]
                            Call.Args = std::make_shared<std::vector<PlannedOperand>>();
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

                            // Apply any member access on the result (from NextOp)
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
                        // If method not found, fall through to normal processing
                    }
                }
            }
        }

        // Check for method call pattern on any operand with member access
        // e.g., (*page).reset() where (*page) is a grouped dereference expression
        if (i + 1 < Ops.size()) {
            const auto &NextOp = Ops[i + 1];
            // Check if current op has member access and next is a tuple (args)
            if (Op.MemberAccess && !Op.MemberAccess->empty() &&
                std::holds_alternative<Tuple>(NextOp.Expr)) {
                // The last member in the access chain might be a method name
                std::string MethodName = Op.MemberAccess->back();

                // Plan the base operand without the last member access
                Operand BaseOp = Op;
                if (Op.MemberAccess->size() == 1) {
                    BaseOp.MemberAccess = nullptr;  // No more member access
                } else {
                    // Keep all but the last member access
                    BaseOp.MemberAccess = std::make_shared<std::vector<std::string>>(
                        Op.MemberAccess->begin(), Op.MemberAccess->end() - 1);
                }

                auto PlannedBase = planOperand(BaseOp);
                if (!PlannedBase) {
                    return PlannedBase.takeError();
                }

                // Look up the method on the base type
                auto MethodMatch = lookupMethod(PlannedBase->ResultType, MethodName, Op.Loc);
                if (MethodMatch) {
                    // Plan the arguments tuple
                    Operand ArgsOp = NextOp;
                    ArgsOp.MemberAccess = nullptr;
                    auto PlannedArgs = planOperand(ArgsOp);
                    if (!PlannedArgs) {
                        return PlannedArgs.takeError();
                    }

                    // Create method call with instance as first argument
                    PlannedCall Call;
                    Call.Loc = Op.Loc;
                    Call.Name = MethodName;
                    Call.MangledName = MethodMatch->MangledName;
                    Call.IsIntrinsic = false;
                    Call.IsOperator = false;
                    Call.ResultType = MethodMatch->ReturnType;

                    // Build args: [instance, ...tuple_args]
                    Call.Args = std::make_shared<std::vector<PlannedOperand>>();
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

                    // Apply any member access on the result (from NextOp)
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
                // If method not found, fall through to normal processing
            }
        }

        // Check for union variant constructor pattern: Union.Variant(value)
        // e.g., Result.Ok(42) or Option.Some(value)
        if (i + 1 < Ops.size()) {
            const auto &NextOp = Ops[i + 1];
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
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

                            Result.push_back(std::move(ResultOp));
                            i++;  // Skip the tuple operand
                            continue;
                        }
                    }
                }
            }
        }

        // Check for type constructor pattern: Type followed by Tuple
        // e.g., Point(3, 4) or Point(3, 4).x where Point is a struct type
        if (i + 1 < Ops.size()) {
            const auto &NextOp = Ops[i + 1];
            // Check if current op is a type that's a struct
            if (auto* TypeExpr = std::get_if<Type>(&Op.Expr)) {
                if (TypeExpr->Name.size() == 1 && (!TypeExpr->Generics || TypeExpr->Generics->empty())) {
                    const Concept* Conc = lookupConcept(TypeExpr->Name[0]);
                    if (Conc && std::holds_alternative<Structure>(Conc->Def)) {
                        // Check if next op is a tuple (constructor args)
                        if (std::holds_alternative<Tuple>(NextOp.Expr)) {
                            // This is Type(args) or Type(args).member - plan the tuple as a struct
                            // Plan the tuple (args) without its member access first
                            Operand ArgsOp = NextOp;
                            ArgsOp.MemberAccess = nullptr;  // Remove member access from args
                            auto PlannedArgs = planOperand(ArgsOp);
                            if (!PlannedArgs) {
                                return PlannedArgs.takeError();
                            }

                            // Change the tuple's type to the struct type
                            PlannedType StructType;
                            StructType.Loc = Op.Loc;
                            StructType.Name = TypeExpr->Name[0];
                            StructType.MangledName = mangleType(StructType);

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
                                Call.ResultType = StructType;

                                // Convert tuple components to call arguments
                                Call.Args = std::make_shared<std::vector<PlannedOperand>>();
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    for (auto &Comp : TupleExpr->Components) {
                                        if (!Comp.Value.empty()) {
                                            Call.Args->push_back(std::move(Comp.Value.back()));
                                        }
                                    }
                                }

                                PlannedOperand CallOperand;
                                CallOperand.Loc = Op.Loc;
                                CallOperand.ResultType = StructType;
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
                                // No initializer - use direct tuple construction
                                // Update the tuple expression to have the struct type
                                if (auto* TupleExpr = std::get_if<PlannedTuple>(&PlannedArgs->Expr)) {
                                    TupleExpr->TupleType = StructType;
                                }
                                PlannedArgs->ResultType = StructType;

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
        }

        // Check for function call pattern: function_name followed by Tuple
        // e.g., seven() or add(3, 4)
        if (i + 1 < Ops.size()) {
            const auto &NextOp = Ops[i + 1];
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

                        // Check if this is a method call with implicit 'this'
                        // This happens when we call a sibling method like allocate_oversized(size)
                        // from within another method of the same struct
                        bool NeedsImplicitThis = false;
                        const Function* MatchedFunc = nullptr;
                        if (!CurrentStructureName.empty()) {
                            auto Candidates = lookupFunction(FuncName);
                            for (const Function* Func : Candidates) {
                                if (Func->Parameters.empty() &&
                                    !Func->Input.empty() &&
                                    Func->Input[0].Name &&
                                    *Func->Input[0].Name == "this" &&
                                    Func->Input.size() == ArgTypes.size() + 1) {
                                    NeedsImplicitThis = true;
                                    MatchedFunc = Func;
                                    break;
                                }
                            }
                        }

                        // Build PlannedItems for mangling
                        std::vector<PlannedItem> ParamItems;
                        if (NeedsImplicitThis) {
                            // Add 'this' type for mangling
                            PlannedItem ThisItem;
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

                        // Generate mangled name - if it's a method, use parent type
                        std::string MangledName;
                        if (NeedsImplicitThis) {
                            std::string BaseName = CurrentStructureName;
                            size_t DotPos = BaseName.find('.');
                            if (DotPos != std::string::npos) {
                                BaseName = BaseName.substr(0, DotPos);
                            }
                            PlannedType ParentType;
                            ParentType.Name = BaseName;
                            ParentType.MangledName = mangleType(ParentType);
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

                        // Add implicit 'this' as first argument if needed
                        if (NeedsImplicitThis) {
                            PlannedOperand ThisOp;
                            ThisOp.Loc = Op.Loc;
                            PlannedVariable ThisVar;
                            ThisVar.Loc = Op.Loc;
                            ThisVar.Name = "this";
                            std::string BaseName = CurrentStructureName;
                            size_t DotPos = BaseName.find('.');
                            if (DotPos != std::string::npos) {
                                BaseName = BaseName.substr(0, DotPos);
                            }
                            ThisVar.VariableType.Name = BaseName;
                            ThisVar.VariableType.MangledName = mangleType(ThisVar.VariableType);
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
                    return PlannedExpression(Var);
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

    // Compute TupleType
    // Single anonymous component = grouping parentheses, use the operation sequence type
    if (Result.Components.size() == 1 && !Result.Components[0].Name) {
        // For grouping parentheses, collapse the operand sequence to a single value
        auto Collapsed = collapseOperandSequence(std::move(Result.Components[0].Value));
        if (!Collapsed) {
            return Collapsed.takeError();
        }
        // Replace the raw operand sequence with the collapsed result
        Result.Components[0].Value.clear();
        Result.Components[0].Value.push_back(std::move(*Collapsed));
        Result.TupleType = Result.Components[0].Value[0].ResultType;
    } else {
        // Multi-component or named component tuple
        // Build a proper tuple type with generics for each component
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
    Result.Target = std::move(*PlannedTarget);

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
    if (!ValueOps.empty()) {
        auto CollapsedOp = collapseOperandSequence(std::move(ValueOps));
        if (!CollapsedOp) {
            return CollapsedOp.takeError();
        }
        Result.Operation.push_back(std::move(*CollapsedOp));
    }

    // Plan the binding item
    auto PlannedItemResult = planItem(Bind.BindingItem);
    if (!PlannedItemResult) {
        return PlannedItemResult.takeError();
    }
    Result.BindingItem = std::move(*PlannedItemResult);

    // Type inference for bindings without explicit type annotation
    if (!Result.BindingItem.ItemType && !Result.Operation.empty()) {
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
        // Explicit type annotation - add constraint for checking
        const PlannedType &DeclaredType = *Result.BindingItem.ItemType;
        const PlannedType &ExprType = Result.Operation.back().ResultType;

        // Only add constraint if both types are present
        if (!DeclaredType.Name.empty() && !ExprType.Name.empty()) {
            addConstraint(DeclaredType, ExprType, Bind.Loc);
        }
    }

    // Add to scope
    if (Result.BindingItem.Name && Result.BindingItem.ItemType) {
        bool IsMutable = (Result.BindingType == "var" || Result.BindingType == "mutable");
        defineLocal(*Result.BindingItem.Name, *Result.BindingItem.ItemType, IsMutable);
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
    // Scaly for-loops use the Iterator pattern:
    // 1. Call get_iterator() on the collection to get an iterator
    // 2. Call next() on the iterator to get each element
    // The element type is what next() returns (possibly with pointer dereferencing)
    if (!Result.Expr.empty()) {
        const auto &CollType = Result.Expr[0].ResultType;
        PlannedType ElementType = CollType;  // Default to collection type

        // Look for get_iterator method on the collection type
        auto IterMethod = lookupMethod(CollType, "get_iterator", ForExpr.Loc);
        if (IterMethod) {
            // Found get_iterator, now look for next() on the iterator type
            auto NextMethod = lookupMethod(IterMethod->ReturnType, "next", ForExpr.Loc);
            if (NextMethod) {
                // The element type is what next() returns
                // If it returns pointer[T], dereference to get T
                ElementType = NextMethod->ReturnType;
                if (ElementType.Name == "pointer" && !ElementType.Generics.empty()) {
                    ElementType = ElementType.Generics[0];
                }
            }
        } else {
            // No get_iterator - try treating as a generic type like pointer[T]
            if (!CollType.Generics.empty()) {
                ElementType = CollType.Generics[0];
            }
        }

        defineLocal(ForExpr.Identifier, ElementType);
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

    pushScope();

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

    // Define 'this' in scope for the initializer body
    defineLocal("this", Parent, true);  // 'this' is mutable in initializers

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

    // Push scope for deinitializer body
    pushScope();

    // Define 'this' in scope for the deinitializer body
    defineLocal("this", Parent, false);  // 'this' is immutable in deinitializers

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

            // Also add to InstantiatedFunctions for the emitter's lookup
            InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;

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
            auto Planned = planStructure(Def, Conc.Name, {});
            if (!Planned) {
                return Planned.takeError();
            }
            // Add non-generic structures to cache for method lookup
            InstantiatedStructures[Conc.Name] = *Planned;
            return *Planned;
        }
        else if constexpr (std::is_same_v<T, Union>) {
            auto Planned = planUnion(Def, Conc.Name, {});
            if (!Planned) {
                return Planned.takeError();
            }
            // Add non-generic unions to cache
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
        if (auto *Pkg = std::get_if<Package>(&Member)) {
            // Package - plan the package's root module if loaded
            if (Pkg->Root) {
                auto PlannedPkgMod = planModule(*Pkg->Root);
                if (!PlannedPkgMod) {
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

            // Also add to InstantiatedFunctions for the emitter's lookup
            InstantiatedFunctions[PlannedFunc->MangledName] = *PlannedFunc;

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

            // Register in symbol table (supports overloading)
            Operators[Op->Name].push_back(Op);
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

    // Set up type inference context
    CurrentPlan = &Result;

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
