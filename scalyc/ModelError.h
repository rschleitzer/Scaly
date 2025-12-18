// ModelError.h - Error types for semantic analysis
#pragma once

#include "Model.h"
#include "Syntax.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <string>
#include <variant>

namespace scaly {

// Position in source file
struct Position {
    size_t Line;
    size_t Column;
};

// Calculate position from offset in source text
Position calculatePosition(llvm::StringRef Text, size_t Offset);

// IO-related errors
struct FileModelError {
    std::string Path;
    std::string Message;

    std::string toString() const;
};

// Parser error wrapper
struct ParserModelError {
    std::string File;
    ParserError Error;

    std::string toString() const;
};

// Model builder errors
struct NotImplementedError {
    std::string File;
    std::string Feature;
    Span Loc;

    std::string toString() const;
};

struct DuplicateNameError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

struct NonFunctionSymbolExistsError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

struct FunctionSymbolExistsError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

struct DeInitializerExistsError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

struct InvalidConstantError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

struct InvalidComponentNameError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

struct ModuleRootMustBeConceptError {
    std::string File;
    Span Loc;

    std::string toString() const;
};

// Aggregate builder error type
using ModelBuilderError = std::variant<
    NotImplementedError,
    DuplicateNameError,
    NonFunctionSymbolExistsError,
    FunctionSymbolExistsError,
    DeInitializerExistsError,
    InvalidConstantError,
    InvalidComponentNameError,
    ModuleRootMustBeConceptError
>;

std::string modelBuilderErrorToString(const ModelBuilderError &Err);

// Top-level model error type
using ModelErrorVariant = std::variant<
    FileModelError,
    ParserModelError,
    ModelBuilderError
>;

// LLVM-style error class
class ModelError : public llvm::ErrorInfo<ModelError> {
public:
    static char ID;

    ModelError(ModelErrorVariant Err) : Err(std::move(Err)) {}

    void log(llvm::raw_ostream &OS) const override;
    std::error_code convertToErrorCode() const override;

    const ModelErrorVariant &getError() const { return Err; }

private:
    ModelErrorVariant Err;
};

// Helper to create model errors
inline llvm::Error makeModelError(ModelErrorVariant Err) {
    return llvm::make_error<ModelError>(std::move(Err));
}

// Convenience constructors
inline llvm::Error makeNotImplementedError(llvm::StringRef File,
                                            llvm::StringRef Feature,
                                            Span Loc) {
    return makeModelError(ModelBuilderError{
        NotImplementedError{File.str(), Feature.str(), Loc}});
}

inline llvm::Error makeDuplicateNameError(llvm::StringRef File, Span Loc) {
    return makeModelError(ModelBuilderError{
        DuplicateNameError{File.str(), Loc}});
}

inline llvm::Error makeNonFunctionSymbolExistsError(llvm::StringRef File,
                                                     Span Loc) {
    return makeModelError(ModelBuilderError{
        NonFunctionSymbolExistsError{File.str(), Loc}});
}

inline llvm::Error makeFunctionSymbolExistsError(llvm::StringRef File,
                                                  Span Loc) {
    return makeModelError(ModelBuilderError{
        FunctionSymbolExistsError{File.str(), Loc}});
}

inline llvm::Error makeDeInitializerExistsError(llvm::StringRef File,
                                                 Span Loc) {
    return makeModelError(ModelBuilderError{
        DeInitializerExistsError{File.str(), Loc}});
}

inline llvm::Error makeInvalidConstantError(llvm::StringRef File, Span Loc) {
    return makeModelError(ModelBuilderError{
        InvalidConstantError{File.str(), Loc}});
}

inline llvm::Error makeInvalidComponentNameError(llvm::StringRef File,
                                                  Span Loc) {
    return makeModelError(ModelBuilderError{
        InvalidComponentNameError{File.str(), Loc}});
}

inline llvm::Error makeFileError(llvm::StringRef Path, llvm::StringRef Message) {
    return makeModelError(FileModelError{Path.str(), Message.str()});
}

inline llvm::Error makeParserError(llvm::StringRef File, ParserError Err) {
    return makeModelError(ParserModelError{File.str(), std::move(Err)});
}

} // namespace scaly
