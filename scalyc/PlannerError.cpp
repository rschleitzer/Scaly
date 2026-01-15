// PlannerError.cpp - Error implementations for the Planner
#include "PlannerError.h"
#include "ModelError.h"  // For formatErrorHeader, buildHintLines, readFileForError
#include "llvm/Support/raw_ostream.h"
#include <sstream>

namespace scaly {

char PlannerError::ID = 0;

std::string UndefinedTypeError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "undefined type '" << Name << "'\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string UndefinedSymbolError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "undefined symbol '" << Name << "'\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string TypeMismatchError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "type mismatch: expected '" << Expected << "', got '" << Actual << "'\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string GenericArityError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "wrong number of type arguments for '" << TypeName
       << "': expected " << Expected << ", got " << Actual << "\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string ArgumentArityError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "wrong number of arguments for '" << FunctionName
       << "': expected " << Expected << ", got " << Actual << "\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string DuplicateDefinitionError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "duplicate definition of '" << Name << "'\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string InfiniteTypeError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "infinite type detected for '" << TypeName << "' (occurs check failed)\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string PlannerNotImplementedError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "feature not yet implemented: " << Feature << "\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string AmbiguousOverloadError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "ambiguous call to '" << FunctionName << "'\n";
    OS << "  candidates are:\n";
    for (const auto &C : Candidates) {
        OS << "    " << C << "\n";
    }
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string NoMatchingOverloadError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "no matching overload for '" << FunctionName << "'";
    if (!ArgumentTypes.empty()) {
        OS << " with argument types (";
        for (size_t I = 0; I < ArgumentTypes.size(); ++I) {
            if (I > 0) OS << ", ";
            OS << ArgumentTypes[I];
        }
        OS << ")";
    }
    OS << "\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string StackLifetimeError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "cannot use '^" << VariableName << "' - variable must be of type pointer[Page]\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string LocalReturnError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "local lifetime ($) is not allowed on return types\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

void PlannerError::log(llvm::raw_ostream &OS) const {
    std::visit([&OS](const auto &E) { OS << E.toString(); }, Err);
}

std::error_code PlannerError::convertToErrorCode() const {
    return std::error_code(1, std::generic_category());
}

} // namespace scaly
