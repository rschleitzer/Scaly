// ModelError.cpp - Error type implementations
#include "ModelError.h"
#include "llvm/Support/MemoryBuffer.h"
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

std::string readFileForError(llvm::StringRef Path) {
    auto BufOrErr = llvm::MemoryBuffer::getFile(Path);
    if (!BufOrErr)
        return "";
    return (*BufOrErr)->getBuffer().str();
}

std::string formatErrorHeader(llvm::StringRef File, llvm::StringRef Text, size_t Offset) {
    Position Pos = calculatePosition(Text, Offset);
    std::ostringstream OS;
    OS << File.str() << ":" << Pos.Line << ":" << Pos.Column << ": error: ";
    return OS.str();
}

std::string buildHintLines(llvm::StringRef Text, size_t Start, size_t End) {
    if (Text.empty() || Start >= Text.size())
        return "";

    // Find the start of the line containing Start
    size_t LineStart = Start;
    while (LineStart > 0 && Text[LineStart - 1] != '\n')
        --LineStart;

    // Find the end of the line containing End (or Start if End is before Start)
    size_t ErrorEnd = (End > Start) ? End : Start + 1;
    size_t LineEnd = Start;
    while (LineEnd < Text.size() && Text[LineEnd] != '\n')
        ++LineEnd;

    // Extract the source line
    llvm::StringRef SourceLine = Text.slice(LineStart, LineEnd);

    // Build the indicator line with carets
    std::string Indicator;
    Indicator.reserve(SourceLine.size());
    for (size_t I = LineStart; I < LineEnd; ++I) {
        if (I >= Start && I < ErrorEnd) {
            Indicator += '^';
        } else if (Text[I] == '\t') {
            Indicator += '\t';
        } else {
            Indicator += ' ';
        }
    }

    std::ostringstream OS;
    OS << "    " << SourceLine.str() << "\n";
    OS << "    " << Indicator;
    return OS.str();
}

std::string FileModelError::toString() const {
    return Path + ": " + Message;
}

std::string ParserModelError::toString() const {
    std::string Source = readFileForError(File);

    if (auto *Inv = std::get_if<InvalidSyntax>(&Error)) {
        std::ostringstream OS;
        OS << formatErrorHeader(File, Source, Inv->Start);
        OS << "Expected " << Inv->Message.str() << "\n";
        if (!Source.empty()) {
            OS << buildHintLines(Source, Inv->Start, Inv->End);
        }
        return OS.str();
    } else {
        // DifferentSyntax - no position info available
        return File + ": error: Unexpected syntax";
    }
}

std::string NotImplementedError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "The " << Feature << " syntax cannot be processed by the modeler yet.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string DuplicateNameError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "This declaration already exists.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string NonFunctionSymbolExistsError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "This declaration already exists, but not as a function.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string FunctionSymbolExistsError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "This declaration already exists, but as a function.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string DeInitializerExistsError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "A deinitializer has already been defined.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string InvalidConstantError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "This is an invalid constant.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string InvalidComponentNameError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "The component does not have an identifier as name.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string ModuleRootMustBeConceptError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << "The root definition of a module must be a concept.\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
}

std::string InvalidInitLifetimeError::toString() const {
    std::string Source = readFileForError(File);
    std::ostringstream OS;
    OS << formatErrorHeader(File, Source, Loc.Start);
    OS << Message << "\n";
    if (!Source.empty())
        OS << buildHintLines(Source, Loc.Start, Loc.End);
    return OS.str();
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
