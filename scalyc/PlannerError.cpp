// PlannerError.cpp - Error implementations for the Planner
#include "PlannerError.h"
#include "llvm/Support/raw_ostream.h"
#include <sstream>

namespace scaly {

char PlannerError::ID = 0;

std::string UndefinedTypeError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: undefined type '" << Name << "'";
    return OS.str();
}

std::string UndefinedSymbolError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: undefined symbol '" << Name << "'";
    return OS.str();
}

std::string TypeMismatchError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: type mismatch: expected '" << Expected
       << "', got '" << Actual << "'";
    return OS.str();
}

std::string GenericArityError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: wrong number of type arguments for '" << TypeName
       << "': expected " << Expected << ", got " << Actual;
    return OS.str();
}

std::string ArgumentArityError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: wrong number of arguments for '" << FunctionName
       << "': expected " << Expected << ", got " << Actual;
    return OS.str();
}

std::string DuplicateDefinitionError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: duplicate definition of '" << Name << "'";
    return OS.str();
}

std::string InfiniteTypeError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: infinite type detected for '" << TypeName
       << "' (occurs check failed)";
    return OS.str();
}

std::string PlannerNotImplementedError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: feature not yet implemented: " << Feature;
    return OS.str();
}

std::string AmbiguousOverloadError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: ambiguous call to '" << FunctionName << "'\n";
    OS << "  candidates are:\n";
    for (const auto &C : Candidates) {
        OS << "    " << C << "\n";
    }
    return OS.str();
}

std::string NoMatchingOverloadError::toString() const {
    std::ostringstream OS;
    OS << File << ":" << Loc.Start
       << ": error: no matching overload for '" << FunctionName << "'";
    if (!ArgumentTypes.empty()) {
        OS << " with argument types (";
        for (size_t I = 0; I < ArgumentTypes.size(); ++I) {
            if (I > 0) OS << ", ";
            OS << ArgumentTypes[I];
        }
        OS << ")";
    }
    return OS.str();
}

void PlannerError::log(llvm::raw_ostream &OS) const {
    std::visit([&OS](const auto &E) { OS << E.toString(); }, Err);
}

std::error_code PlannerError::convertToErrorCode() const {
    return std::error_code(1, std::generic_category());
}

} // namespace scaly
