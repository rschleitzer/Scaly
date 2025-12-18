// PlannerError.h - Error types for the Planner (generic resolution)
#pragma once

#include "Model.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <string>
#include <variant>
#include <vector>

namespace scaly {

// Undefined type reference
struct UndefinedTypeError {
    std::string File;
    Span Loc;
    std::string Name;

    std::string toString() const;
};

// Undefined symbol (variable, function, etc.)
struct UndefinedSymbolError {
    std::string File;
    Span Loc;
    std::string Name;

    std::string toString() const;
};

// Type mismatch
struct TypeMismatchError {
    std::string File;
    Span Loc;
    std::string Expected;
    std::string Actual;

    std::string toString() const;
};

// Wrong number of generic arguments
struct GenericArityError {
    std::string File;
    Span Loc;
    std::string TypeName;
    size_t Expected;
    size_t Actual;

    std::string toString() const;
};

// Wrong number of function arguments
struct ArgumentArityError {
    std::string File;
    Span Loc;
    std::string FunctionName;
    size_t Expected;
    size_t Actual;

    std::string toString() const;
};

// Duplicate definition
struct DuplicateDefinitionError {
    std::string File;
    Span Loc;
    std::string Name;

    std::string toString() const;
};

// Recursive type without indirection
struct InfiniteTypeError {
    std::string File;
    Span Loc;
    std::string TypeName;

    std::string toString() const;
};

// Feature not yet implemented
struct NotImplementedError {
    std::string File;
    Span Loc;
    std::string Feature;

    std::string toString() const;
};

// Ambiguous overload resolution
struct AmbiguousOverloadError {
    std::string File;
    Span Loc;
    std::string FunctionName;
    std::vector<std::string> Candidates;

    std::string toString() const;
};

// No matching overload found
struct NoMatchingOverloadError {
    std::string File;
    Span Loc;
    std::string FunctionName;
    std::vector<std::string> ArgumentTypes;

    std::string toString() const;
};

// Aggregate planner error type
using PlannerErrorVariant = std::variant<
    UndefinedTypeError,
    UndefinedSymbolError,
    TypeMismatchError,
    GenericArityError,
    ArgumentArityError,
    DuplicateDefinitionError,
    InfiniteTypeError,
    NotImplementedError,
    AmbiguousOverloadError,
    NoMatchingOverloadError
>;

// LLVM-style error class
class PlannerError : public llvm::ErrorInfo<PlannerError> {
public:
    static char ID;

    PlannerError(PlannerErrorVariant Err) : Err(std::move(Err)) {}

    void log(llvm::raw_ostream &OS) const override;
    std::error_code convertToErrorCode() const override;

    const PlannerErrorVariant &getError() const { return Err; }

private:
    PlannerErrorVariant Err;
};

// Helper to create planner errors
inline llvm::Error makePlannerError(PlannerErrorVariant Err) {
    return llvm::make_error<PlannerError>(std::move(Err));
}

// Convenience constructors
inline llvm::Error makeUndefinedTypeError(llvm::StringRef File, Span Loc,
                                           llvm::StringRef Name) {
    return makePlannerError(UndefinedTypeError{File.str(), Loc, Name.str()});
}

inline llvm::Error makeUndefinedSymbolError(llvm::StringRef File, Span Loc,
                                             llvm::StringRef Name) {
    return makePlannerError(UndefinedSymbolError{File.str(), Loc, Name.str()});
}

inline llvm::Error makeTypeMismatchError(llvm::StringRef File, Span Loc,
                                          llvm::StringRef Expected,
                                          llvm::StringRef Actual) {
    return makePlannerError(
        TypeMismatchError{File.str(), Loc, Expected.str(), Actual.str()});
}

inline llvm::Error makeGenericArityError(llvm::StringRef File, Span Loc,
                                          llvm::StringRef TypeName,
                                          size_t Expected, size_t Actual) {
    return makePlannerError(
        GenericArityError{File.str(), Loc, TypeName.str(), Expected, Actual});
}

inline llvm::Error makeArgumentArityError(llvm::StringRef File, Span Loc,
                                           llvm::StringRef FunctionName,
                                           size_t Expected, size_t Actual) {
    return makePlannerError(
        ArgumentArityError{File.str(), Loc, FunctionName.str(), Expected, Actual});
}

inline llvm::Error makeDuplicateDefinitionError(llvm::StringRef File, Span Loc,
                                                 llvm::StringRef Name) {
    return makePlannerError(DuplicateDefinitionError{File.str(), Loc, Name.str()});
}

inline llvm::Error makeInfiniteTypeError(llvm::StringRef File, Span Loc,
                                          llvm::StringRef TypeName) {
    return makePlannerError(InfiniteTypeError{File.str(), Loc, TypeName.str()});
}

inline llvm::Error makePlannerNotImplementedError(llvm::StringRef File, Span Loc,
                                                   llvm::StringRef Feature) {
    return makePlannerError(NotImplementedError{File.str(), Loc, Feature.str()});
}

inline llvm::Error makeAmbiguousOverloadError(llvm::StringRef File, Span Loc,
                                               llvm::StringRef FunctionName,
                                               std::vector<std::string> Candidates) {
    return makePlannerError(
        AmbiguousOverloadError{File.str(), Loc, FunctionName.str(), std::move(Candidates)});
}

inline llvm::Error makeNoMatchingOverloadError(llvm::StringRef File, Span Loc,
                                                llvm::StringRef FunctionName,
                                                std::vector<std::string> ArgumentTypes) {
    return makePlannerError(
        NoMatchingOverloadError{File.str(), Loc, FunctionName.str(), std::move(ArgumentTypes)});
}

} // namespace scaly
