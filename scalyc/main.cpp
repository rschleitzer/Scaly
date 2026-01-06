#include "Lexer.h"
#include "LexerTests.h"
#include "Parser.h"
#include "ParserTests.h"
#include "Modeler.h"
#include "Planner.h"
#include "Emitter.h"
#include "EmitterTests.h"
#include "ModuleTests.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
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

static cl::opt<std::string> RunFunction(
    "run",
    cl::desc("JIT-execute a function by name"),
    cl::value_desc("function"),
    cl::cat(ScalyCategory));

static cl::list<std::string> IncludePaths(
    "I",
    cl::desc("Add package search path"),
    cl::value_desc("path"),
    cl::Prefix,
    cl::cat(ScalyCategory));

static cl::list<std::string> LibraryPaths(
    "L",
    cl::desc("Add library search path"),
    cl::value_desc("path"),
    cl::Prefix,
    cl::cat(ScalyCategory));

static cl::list<std::string> Libraries(
    "l",
    cl::desc("Link library"),
    cl::value_desc("library"),
    cl::Prefix,
    cl::cat(ScalyCategory));

static cl::opt<bool> EnableLTO(
    "flto",
    cl::desc("Enable Link Time Optimization"),
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
    for (const auto &P : IncludePaths) {
        Modeler.addPackageSearchPath(P);
    }
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
    for (const auto &P : IncludePaths) {
        Modeler.addPackageSearchPath(P);
    }
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Planner Planner(Filename);
    for (const auto &P : IncludePaths) {
        Planner.addPackageSearchPath(P);
    }
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

        // Debug: show Vector.char methods if present
        for (const auto &[name, Struct] : PlanResult->Structures) {
            if (name.find("Vector") != std::string::npos) {
                outs() << "  Structure: " << name << " has " << Struct.Methods.size() << " methods\n";
                for (const auto &Method : Struct.Methods) {
                    outs() << "    Method: " << Method.MangledName << "\n";
                }
            }
        }
    }
    return 0;
}

// Compile a file to object code
static int compileFile(StringRef Filename, StringRef OutputPath) {
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
    for (const auto &P : IncludePaths) {
        Modeler.addPackageSearchPath(P);
    }
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Planner Planner(Filename);
    for (const auto &P : IncludePaths) {
        Planner.addPackageSearchPath(P);
    }
    auto PlanResult = Planner.plan(*ModelResult);
    if (!PlanResult) {
        handleAllErrors(PlanResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    // Emit object file
    scaly::Emitter Emitter;
    std::string ModuleName = sys::path::stem(Filename).str();
    auto EmitErr = Emitter.emitObjectFile(*PlanResult, ModuleName, OutputPath);
    if (EmitErr) {
        handleAllErrors(std::move(EmitErr), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    if (Verbose) {
        outs() << "Compiled: " << Filename << " -> " << OutputPath << "\n";
    }
    return 0;
}

// Emit LLVM IR to a file or stdout
static int emitLLVMFile(StringRef Filename, StringRef OutputPath) {
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
    for (const auto &P : IncludePaths) {
        Modeler.addPackageSearchPath(P);
    }
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Planner Planner(Filename);
    for (const auto &P : IncludePaths) {
        Planner.addPackageSearchPath(P);
    }
    auto PlanResult = Planner.plan(*ModelResult);
    if (!PlanResult) {
        handleAllErrors(PlanResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    // Emit LLVM IR
    scaly::Emitter Emitter;
    std::string ModuleName = sys::path::stem(Filename).str();
    auto ModuleOrErr = Emitter.emit(*PlanResult, ModuleName);
    if (!ModuleOrErr) {
        handleAllErrors(ModuleOrErr.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    // Output to file or stdout
    std::error_code EC;
    raw_fd_ostream OS(OutputPath.empty() ? "-" : OutputPath, EC);
    if (EC) {
        errs() << "scalyc: error: " << EC.message() << ": " << OutputPath << "\n";
        return 1;
    }

    (*ModuleOrErr)->print(OS, nullptr);

    if (Verbose && !OutputPath.empty()) {
        outs() << "Emitted LLVM IR: " << Filename << " -> " << OutputPath << "\n";
    }
    return 0;
}

// JIT-execute a function from a file
static int runFile(StringRef Filename, StringRef FunctionName) {
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
    for (const auto &P : IncludePaths) {
        Modeler.addPackageSearchPath(P);
    }
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Planner Planner(Filename);
    for (const auto &P : IncludePaths) {
        Planner.addPackageSearchPath(P);
    }
    auto PlanResult = Planner.plan(*ModelResult);
    if (!PlanResult) {
        handleAllErrors(PlanResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    // Find the mangled name for the function
    std::string MangledName;
    for (const auto &[name, Func] : PlanResult->Functions) {
        if (Func.Name == FunctionName) {
            MangledName = Func.MangledName;
            break;
        }
    }

    if (MangledName.empty()) {
        errs() << "scalyc: error: function '" << FunctionName << "' not found\n";
        return 1;
    }

    if (Verbose) {
        outs() << "Running function: " << FunctionName << " (mangled: " << MangledName << ")\n";
        outs().flush();
    }

    // JIT execute the function
    scaly::Emitter Emitter;
    if (auto Err = Emitter.jitExecuteVoid(*PlanResult, MangledName)) {
        handleAllErrors(std::move(Err), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    if (Verbose) {
        outs() << "Function " << FunctionName << " completed successfully.\n";
    }

    return 0;
}

// Link object/bitcode file(s) to executable using clang
static int linkExecutable(ArrayRef<std::string> InputFiles,
                          StringRef OutputPath,
                          ArrayRef<std::string> LibPaths,
                          ArrayRef<std::string> Libs,
                          bool LTO) {
    auto ClangOrErr = sys::findProgramByName("clang");
    if (!ClangOrErr) {
        errs() << "scalyc: error: cannot find clang in PATH\n";
        return 1;
    }
    std::string Clang = *ClangOrErr;

    // Build argument list - need stable storage for StringRefs
    SmallVector<std::string, 32> ArgStorage;
    SmallVector<StringRef, 32> Args;

    ArgStorage.push_back(Clang);
    Args.push_back(ArgStorage.back());

    // Add input files
    for (const auto &File : InputFiles) {
        ArgStorage.push_back(File);
        Args.push_back(ArgStorage.back());
    }

    // Output file
    ArgStorage.push_back("-o");
    Args.push_back(ArgStorage.back());
    ArgStorage.push_back(OutputPath.str());
    Args.push_back(ArgStorage.back());

    // LTO flag
    if (LTO) {
        ArgStorage.push_back("-flto");
        Args.push_back(ArgStorage.back());
    }

    // Library search paths
    for (const auto &Path : LibPaths) {
        ArgStorage.push_back("-L" + Path);
        Args.push_back(ArgStorage.back());
    }

    // Libraries
    for (const auto &Lib : Libs) {
        ArgStorage.push_back("-l" + Lib);
        Args.push_back(ArgStorage.back());
    }

    if (Verbose) {
        outs() << "Linking:";
        for (const auto &Arg : Args) {
            outs() << " " << Arg;
        }
        outs() << "\n";
    }

    std::string ErrMsg;
    int Result = sys::ExecuteAndWait(Clang, Args, std::nullopt, {}, 0, 0, &ErrMsg);
    if (Result != 0) {
        if (!ErrMsg.empty()) {
            errs() << "scalyc: link error: " << ErrMsg << "\n";
        } else {
            errs() << "scalyc: link failed with exit code " << Result << "\n";
        }
        return 1;
    }

    return 0;
}

// Compile file to bitcode (for LTO)
static int compileToBitcode(StringRef Filename, StringRef OutputPath) {
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
    for (const auto &P : IncludePaths) {
        Modeler.addPackageSearchPath(P);
    }
    auto ModelResult = Modeler.buildProgram(*ParseResult);
    if (!ModelResult) {
        handleAllErrors(ModelResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    scaly::Planner Planner(Filename);
    for (const auto &P : IncludePaths) {
        Planner.addPackageSearchPath(P);
    }
    auto PlanResult = Planner.plan(*ModelResult);
    if (!PlanResult) {
        handleAllErrors(PlanResult.takeError(), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    // Emit bitcode file
    scaly::Emitter Emitter;
    std::string ModuleName = sys::path::stem(Filename).str();
    auto EmitErr = Emitter.emitBitcodeFile(*PlanResult, ModuleName, OutputPath);
    if (EmitErr) {
        handleAllErrors(std::move(EmitErr), [&](const llvm::ErrorInfoBase &E) {
            errs() << "scalyc: " << Filename << ": " << E.message() << "\n";
        });
        return 1;
    }

    if (Verbose) {
        outs() << "Compiled (bitcode): " << Filename << " -> " << OutputPath << "\n";
    }
    return 0;
}

// Compile and link a file to executable
static int buildFile(StringRef Filename, StringRef OutputPath) {
    SmallString<128> TempPath;
    const char *Extension = EnableLTO ? "bc" : "o";

    if (auto EC = sys::fs::createTemporaryFile("scalyc", Extension, TempPath)) {
        errs() << "scalyc: error: cannot create temp file: " << EC.message() << "\n";
        return 1;
    }

    // Compile to object or bitcode
    int CompileResult;
    if (EnableLTO) {
        CompileResult = compileToBitcode(Filename, TempPath);
    } else {
        CompileResult = compileFile(Filename, TempPath);
    }

    if (CompileResult != 0) {
        sys::fs::remove(TempPath);
        return CompileResult;
    }

    // Link to executable
    std::vector<std::string> InputFiles = {std::string(TempPath)};
    std::vector<std::string> LibPaths(LibraryPaths.begin(), LibraryPaths.end());
    std::vector<std::string> Libs(Libraries.begin(), Libraries.end());

    int LinkResult = linkExecutable(InputFiles, OutputPath, LibPaths, Libs, EnableLTO);

    // Clean up temp file
    sys::fs::remove(TempPath);

    if (LinkResult == 0 && Verbose) {
        outs() << "Built: " << Filename << " -> " << OutputPath << "\n";
    }

    return LinkResult;
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
        std::cout << std::endl << std::flush;
        llvm::outs().flush();
        AllPassed &= scaly::runModuleTests();
        return AllPassed ? 0 : 1;
    }

    if (InputFiles.empty() && !LexOnly && !ParseOnly && !ModelOnly && !PlanOnly && RunFunction.empty()) {
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
            outs().flush();
        }

        if (LexOnly) {
            Result |= lexFile(File);
        } else if (ParseOnly) {
            Result |= parseFile(File);
        } else if (ModelOnly) {
            Result |= modelFile(File);
        } else if (PlanOnly) {
            Result |= planFile(File);
        } else if (CompileOnly) {
            // Compile to object file
            std::string OutPath;
            if (!OutputFile.empty()) {
                OutPath = OutputFile;
            } else {
                // Default: replace .scaly with .o
                OutPath = sys::path::stem(File).str() + ".o";
            }
            Result |= compileFile(File, OutPath);
        } else if (EmitLLVM) {
            // Emit LLVM IR
            std::string OutPath;
            if (!OutputFile.empty()) {
                OutPath = OutputFile;
            } else {
                // Default: replace .scaly with .ll
                OutPath = sys::path::stem(File).str() + ".ll";
            }
            Result |= emitLLVMFile(File, OutPath);
        } else if (!RunFunction.empty()) {
            // JIT-execute a function
            Result |= runFile(File, RunFunction);
        } else {
            // Full compilation: compile + link to executable
            std::string OutPath;
            if (!OutputFile.empty()) {
                OutPath = OutputFile;
            } else {
                // Default: remove .scaly extension (or add nothing)
                OutPath = sys::path::stem(File).str();
            }
            Result |= buildFile(File, OutPath);
        }
    }

    return Result;
}
