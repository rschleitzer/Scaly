#include "Lexer.h"
#include "LexerTests.h"
#include "Parser.h"
#include "ParserTests.h"
#include "Modeler.h"
#include "Planner.h"
#include "EmitterTests.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace llvm;

// Command line options
static cl::OptionCategory ScalyCategory("Scaly Compiler Options");

static cl::list<std::string> InputFiles(
    cl::Positional,
    cl::desc("<input files>"),
    cl::cat(ScalyCategory));

static cl::opt<std::string> OutputFile(
    "o",
    cl::desc("Output file"),
    cl::value_desc("file"),
    cl::cat(ScalyCategory));

static cl::opt<bool> CompileOnly(
    "c",
    cl::desc("Compile only (no link)"),
    cl::cat(ScalyCategory));

static cl::opt<bool> EmitLLVM(
    "S",
    cl::desc("Emit LLVM IR"),
    cl::cat(ScalyCategory));

static cl::opt<bool> RunTests(
    "test",
    cl::desc("Run test suite"),
    cl::cat(ScalyCategory));

static cl::opt<std::string> TestName(
    "test-name",
    cl::desc("Run specific test"),
    cl::value_desc("name"),
    cl::cat(ScalyCategory));

static cl::opt<bool> Verbose(
    "v",
    cl::desc("Verbose output"),
    cl::cat(ScalyCategory));

static cl::opt<bool> LexOnly(
    "lex",
    cl::desc("Lex input and print tokens"),
    cl::cat(ScalyCategory));

static cl::opt<bool> ParseOnly(
    "parse",
    cl::desc("Parse input and validate syntax"),
    cl::cat(ScalyCategory));

static cl::opt<bool> ModelOnly(
    "model",
    cl::desc("Build semantic model"),
    cl::cat(ScalyCategory));

static cl::opt<bool> PlanOnly(
    "plan",
    cl::desc("Run planner (type resolution, name mangling)"),
    cl::cat(ScalyCategory));

// Print a token in readable format
static void printToken(const scaly::Token &Tok) {
    std::visit([](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, scaly::EmptyToken>) {
            outs() << "Empty\n";
        } else if constexpr (std::is_same_v<T, scaly::InvalidToken>) {
            outs() << "Invalid\n";
        } else if constexpr (std::is_same_v<T, scaly::ColonToken>) {
            outs() << "Colon\n";
        } else if constexpr (std::is_same_v<T, scaly::IdentifierToken>) {
            outs() << "Identifier(" << arg.Name << ")\n";
        } else if constexpr (std::is_same_v<T, scaly::AttributeToken>) {
            outs() << "Attribute(" << arg.Name << ")\n";
        } else if constexpr (std::is_same_v<T, scaly::PunctuationToken>) {
            outs() << "Punctuation(" << arg.Sign << ")\n";
        } else if constexpr (std::is_same_v<T, scaly::LiteralToken>) {
            std::visit([](auto &&lit) {
                using L = std::decay_t<decltype(lit)>;
                if constexpr (std::is_same_v<L, scaly::StringLiteral>) {
                    outs() << "String(\"" << lit.Value << "\")\n";
                } else if constexpr (std::is_same_v<L, scaly::CharacterLiteral>) {
                    outs() << "Character('" << lit.Value << "')\n";
                } else if constexpr (std::is_same_v<L, scaly::FragmentLiteral>) {
                    outs() << "Fragment(`" << lit.Value << "`)\n";
                } else if constexpr (std::is_same_v<L, scaly::IntegerLiteral>) {
                    outs() << "Integer(" << lit.Value << ")\n";
                } else if constexpr (std::is_same_v<L, scaly::BooleanLiteral>) {
                    outs() << "Boolean(" << (lit.Value ? "true" : "false") << ")\n";
                } else if constexpr (std::is_same_v<L, scaly::FloatingPointLiteral>) {
                    outs() << "Float(" << lit.Value << ")\n";
                } else if constexpr (std::is_same_v<L, scaly::HexLiteral>) {
                    outs() << "Hex(" << lit.Value << ")\n";
                }
            }, arg.Value);
        }
    }, Tok);
}

// Lex a file and print all tokens
static int lexFile(StringRef Filename) {
    auto BufOrErr = MemoryBuffer::getFileOrSTDIN(Filename);
    if (!BufOrErr) {
        errs() << "scalyc: error: " << BufOrErr.getError().message() << ": " << Filename << "\n";
        return 1;
    }

    scaly::Lexer Lex((*BufOrErr)->getBuffer());

    while (!Lex.isAtEnd()) {
        Lex.advance();
        printToken(Lex.token());
        if (scaly::is<scaly::InvalidToken>(Lex.token())) {
            errs() << "scalyc: error: invalid token at position " << Lex.position() << "\n";
            return 1;
        }
    }

    return 0;
}

// Parse a file and validate syntax
static int parseFile(StringRef Filename) {
    auto BufOrErr = MemoryBuffer::getFileOrSTDIN(Filename);
    if (!BufOrErr) {
        errs() << "scalyc: error: " << BufOrErr.getError().message() << ": " << Filename << "\n";
        return 1;
    }

    scaly::Parser Parser((*BufOrErr)->getBuffer());
    auto Result = Parser.parseProgram();
    if (!Result) {
        handleAllErrors(Result.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    if (Verbose) {
        outs() << "Parse successful: " << Filename << "\n";
    }
    return 0;
}

// Build semantic model from a file
static int modelFile(StringRef Filename) {
    auto BufOrErr = MemoryBuffer::getFileOrSTDIN(Filename);
    if (!BufOrErr) {
        errs() << "scalyc: error: " << BufOrErr.getError().message() << ": " << Filename << "\n";
        return 1;
    }

    scaly::Parser Parser((*BufOrErr)->getBuffer());
    auto ParseResult = Parser.parseProgram();
    if (!ParseResult) {
        handleAllErrors(ParseResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Modeler Modeler(Filename);
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    if (Verbose) {
        outs() << "Model built: " << Filename << "\n";
        outs() << "  Main module: " << ModelResult->MainModule.Name << "\n";
        outs() << "  Statements: " << ModelResult->Statements.size() << "\n";
    }
    return 0;
}

// Run planner on a file
static int planFile(StringRef Filename) {
    auto BufOrErr = MemoryBuffer::getFileOrSTDIN(Filename);
    if (!BufOrErr) {
        errs() << "scalyc: error: " << BufOrErr.getError().message() << ": " << Filename << "\n";
        return 1;
    }

    scaly::Parser Parser((*BufOrErr)->getBuffer());
    auto ParseResult = Parser.parseProgram();
    if (!ParseResult) {
        handleAllErrors(ParseResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Modeler Modeler(Filename);
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Planner Planner(Filename);
    auto PlanResult = Planner.plan(*ModelResult);
    if (!PlanResult) {
        handleAllErrors(PlanResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    if (Verbose) {
        outs() << "Plan created: " << Filename << "\n";
        outs() << "  Main module: " << PlanResult->MainModule.Name << "\n";
        outs() << "  Modules: " << PlanResult->MainModule.Modules.size() << "\n";
        outs() << "  Concepts: " << PlanResult->MainModule.Concepts.size() << "\n";
        outs() << "  Instantiated structures: " << PlanResult->Structures.size() << "\n";
        outs() << "  Instantiated functions: " << PlanResult->Functions.size() << "\n";
    }
    return 0;
}

int main(int argc, char **argv) {
    cl::HideUnrelatedOptions(ScalyCategory);
    cl::ParseCommandLineOptions(argc, argv, "Scaly Compiler\n");

    if (RunTests) {
        bool AllPassed = true;
        AllPassed &= scaly::runParserTests();
        std::cout << std::endl << std::flush;
        llvm::outs().flush();
        AllPassed &= scaly::runLexerTests();
        std::cout << std::endl << std::flush;
        llvm::outs().flush();
        AllPassed &= scaly::runEmitterTests();
        return AllPassed ? 0 : 1;
    }

    if (InputFiles.empty() && !LexOnly && !ParseOnly && !ModelOnly && !PlanOnly) {
        errs() << "scalyc: error: no input files\n";
        return 1;
    }

    // Handle stdin for single-file modes
    if (InputFiles.empty()) {
        if (LexOnly) return lexFile("-");
        if (ParseOnly) return parseFile("-");
        if (ModelOnly) return modelFile("-");
        if (PlanOnly) return planFile("-");
    }

    int Result = 0;
    for (const auto &File : InputFiles) {
        if (Verbose) {
            outs() << "Processing: " << File << "\n";
        }

        if (LexOnly) {
            Result |= lexFile(File);
        } else if (ParseOnly) {
            Result |= parseFile(File);
        } else if (ModelOnly) {
            Result |= modelFile(File);
        } else if (PlanOnly) {
            Result |= planFile(File);
        } else {
            // Full compilation - run planner as final step for now
            Result |= planFile(File);
        }
    }

    return Result;
}
