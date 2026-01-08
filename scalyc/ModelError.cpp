// ModelError.cpp - Error type implementations
#include "ModelError.h"
#include "llvm/Support/raw_ostream.h"
#include <sstream>

namespace scaly {

char ModelError::ID = 0;

Position calculatePosition(llvm::StringRef Text, size_t Offset) {
    size_t Line = 1;
    size_t Column = 1;

    for (size_t I = 0; I < Offset && I < Text.size(); ++I) {
        if (Text[I] == '\n') {
            ++Line;
            Column = 1;
        } else {
            ++Column;
        }
    }

    return Position{Line, Column};
}

std::string FileModelError::toString() const {
    return Path + ": " + Message;
}

std::string ParserModelError::toString() const {
    std::string Result = File + ": ";
    if (auto *Inv = std::get_if<InvalidSyntax>(&Error)) {
        Result += "error: Expected " + std::string(Inv->Message);
    } else {
        Result += "error: Unexpected syntax";
    }
    return Result;
}

std::string NotImplementedError::toString() const {
    std::ostringstream OS;
    OS << File << ": error: The " << Feature
       << " syntax cannot be processed by the modeler yet.";
    return OS.str();
}

std::string DuplicateNameError::toString() const {
    return File + ": error: This declaration already exists.";
}

std::string NonFunctionSymbolExistsError::toString() const {
    return File + ": error: This declaration already exists, but not as a function.";
}

std::string FunctionSymbolExistsError::toString() const {
    return File + ": error: This declaration already exists, but as a function.";
}

std::string DeInitializerExistsError::toString() const {
    return File + ": error: A deinitializer has already been defined.";
}

std::string InvalidConstantError::toString() const {
    return File + ": error: This is an invalid constant.";
}

std::string InvalidComponentNameError::toString() const {
    return File + ": error: The component does not have an identifier as name.";
}

std::string ModuleRootMustBeConceptError::toString() const {
    return File + ": error: The root definition of a module must be a concept.";
}

std::string InvalidInitLifetimeError::toString() const {
    return File + ": error: " + Message;
}

std::string modelBuilderErrorToString(const ModelBuilderError &Err) {
    return std::visit([](const auto &E) { return E.toString(); }, Err);
}

void ModelError::log(llvm::raw_ostream &OS) const {
    std::visit([&OS](const auto &E) {
        using T = std::decay_t<decltype(E)>;
        if constexpr (std::is_same_v<T, FileModelError>) {
            OS << E.toString();
        } else if constexpr (std::is_same_v<T, ParserModelError>) {
            OS << E.toString();
        } else if constexpr (std::is_same_v<T, ModelBuilderError>) {
            OS << modelBuilderErrorToString(E);
        }
    }, Err);
}

std::error_code ModelError::convertToErrorCode() const {
    return llvm::inconvertibleErrorCode();
}

} // namespace scaly
