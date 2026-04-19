// ModuleTests.cpp - Tests for complete Scaly modules
//
// This test infrastructure compiles the scaly package and runs test functions.
// Test functions return int: 0 for success, negative for error codes.
// This allows proper error handling without exit() calls.
//
// The package entry point (scaly.scaly) is compiled, which loads all modules
// (memory, containers, io). Test functions from any module are then available.

#include "ModuleTests.h"
#include "Emitter.h"
#include "Plan.h"
#include "Parser.h"
#include "Modeler.h"
#include "Planner.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include <fstream>
#include <sstream>
#include <regex>

namespace scaly {

static int TestsPassed = 0;
static int TestsFailed = 0;

static void pass(const char* Name) {
    ++TestsPassed;
    llvm::outs() << "  PASS: " << Name << "\n";
    llvm::outs().flush();
}

static void fail(const char* Name, const char* Message) {
    ++TestsFailed;
    llvm::outs() << "  FAIL: " << Name << " - " << Message << "\n";
    llvm::outs().flush();
}

// Compile a package and run a test function by name
static llvm::Expected<int64_t> runPackageTestFunction(
    llvm::StringRef PackagePath,
    llvm::StringRef FunctionName
) {
    auto BufOrErr = llvm::MemoryBuffer::getFile(PackagePath);
    if (!BufOrErr) {
        return llvm::make_error<llvm::StringError>(
            "Cannot open file: " + PackagePath.str(),
            llvm::inconvertibleErrorCode()
        );
    }

    Parser P((*BufOrErr)->getBuffer());
    auto ParseResult = P.parseProgram();
    if (!ParseResult)
        return ParseResult.takeError();

    Modeler M(PackagePath);
    auto ModelResult = M.buildProgram(*ParseResult);
    if (!ModelResult)
        return ModelResult.takeError();

    Planner Pl(PackagePath);
    auto PlanResult = Pl.plan(*ModelResult);
    if (!PlanResult)
        return PlanResult.takeError();

    // Find the function by name
    std::string MangledName;
    for (const auto &[name, Func] : PlanResult->Functions) {
        if (Func.Name == FunctionName) {
            MangledName = Func.MangledName;
            break;
        }
    }

    if (MangledName.empty()) {
        return llvm::make_error<llvm::StringError>(
            "Function not found: " + FunctionName.str(),
            llvm::inconvertibleErrorCode()
        );
    }

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto Result = E.jitExecuteIntFunction(*PlanResult, MangledName);
    return Result;
}

// ============================================================================
// Module Test Cases
// ============================================================================

// Paths to package entry points (relative to project root)
static const char* ScalyPackagePath = "packages/scaly/0.1.0/scaly.scaly";
static const char* ScalycPackagePath = "packages/scalyc/0.1.0/scalyc.scaly";

// ============================================================================
// Optimized Test Runner - compiles package once, runs multiple tests
// ============================================================================

// Compile a package once and run multiple test functions
static bool runPackageTests(
    llvm::StringRef PackagePath,
    const std::vector<std::pair<const char*, const char*>>& Tests  // {Name, FunctionName}
) {
    auto BufOrErr = llvm::MemoryBuffer::getFile(PackagePath);
    if (!BufOrErr) {
        for (const auto& Test : Tests) {
            fail(Test.first, ("Cannot open file: " + PackagePath.str()).c_str());
        }
        return false;
    }

    Parser P((*BufOrErr)->getBuffer());
    auto ParseResult = P.parseProgram();
    if (!ParseResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ParseResult.takeError();
        for (const auto& Test : Tests) {
            fail(Test.first, ErrMsg.c_str());
        }
        return false;
    }

    Modeler M(PackagePath);
    auto ModelResult = M.buildProgram(*ParseResult);
    if (!ModelResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ModelResult.takeError();
        for (const auto& Test : Tests) {
            fail(Test.first, ErrMsg.c_str());
        }
        return false;
    }

    Planner Pl(PackagePath);
    auto PlanResult = Pl.plan(*ModelResult);
    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        for (const auto& Test : Tests) {
            fail(Test.first, ErrMsg.c_str());
        }
        return false;
    }

    // DEBUG: Print plan statistics
    size_t totalMethods = 0;
    size_t totalOps = 0;
    size_t totalInits = 0;
    for (const auto &[name, Struct] : PlanResult->Structures) {
        totalMethods += Struct.Methods.size();
        totalOps += Struct.Operators.size();
        totalInits += Struct.Initializers.size();
    }
    llvm::outs() << "    Plan stats for " << PackagePath << ":\n";
    llvm::outs() << "      Structures: " << PlanResult->Structures.size() << "\n";
    llvm::outs() << "      Unions: " << PlanResult->Unions.size() << "\n";
    llvm::outs() << "      Functions: " << PlanResult->Functions.size() << "\n";
    llvm::outs() << "      Total struct methods: " << totalMethods << "\n";
    llvm::outs() << "      Total struct operators: " << totalOps << "\n";
    llvm::outs() << "      Total struct initializers: " << totalInits << "\n";
    llvm::outs().flush();

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    bool AllPassed = true;
    for (const auto& Test : Tests) {
        // Look up the mangled name from the plan
        std::string MangledName;
        for (const auto &[name, Func] : PlanResult->Functions) {
            if (Func.Name == Test.second) {
                MangledName = Func.MangledName;
                break;
            }
        }

        if (MangledName.empty()) {
            fail(Test.first, ("Function not found: " + std::string(Test.second)).c_str());
            AllPassed = false;
            continue;
        }

        auto Result = E.jitExecuteIntFunction(*PlanResult, MangledName);
        if (!Result) {
            std::string ErrMsg;
            llvm::raw_string_ostream OS(ErrMsg);
            OS << Result.takeError();
            fail(Test.first, ErrMsg.c_str());
            AllPassed = false;
        } else if (*Result != 0) {
            std::string Msg = "test returned error code " + std::to_string(*Result);
            fail(Test.first, Msg.c_str());
            AllPassed = false;
        } else {
            pass(Test.first);
        }
    }
    return AllPassed;
}

// Compile multiple packages together and run test functions
// Dependencies are compiled first so their types/functions are available
static bool runMultiPackageTests(
    const std::vector<llvm::StringRef>& PackagePaths,  // First = dependencies, Last = main
    const std::vector<std::pair<const char*, const char*>>& Tests  // {Name, FunctionName}
) {
    if (PackagePaths.empty()) {
        for (const auto& Test : Tests) {
            fail(Test.first, "No packages to compile");
        }
        return false;
    }

    // Create merged plan
    Plan MergedPlan;

    // Compile each package and merge into the combined plan
    for (const auto& PackagePath : PackagePaths) {
        auto BufOrErr = llvm::MemoryBuffer::getFile(PackagePath);
        if (!BufOrErr) {
            for (const auto& Test : Tests) {
                fail(Test.first, ("Cannot open file: " + PackagePath.str()).c_str());
            }
            return false;
        }

        Parser P((*BufOrErr)->getBuffer());
        auto ParseResult = P.parseProgram();
        if (!ParseResult) {
            std::string ErrMsg;
            llvm::raw_string_ostream OS(ErrMsg);
            OS << ParseResult.takeError();
            for (const auto& Test : Tests) {
                fail(Test.first, ErrMsg.c_str());
            }
            return false;
        }

        Modeler M(PackagePath);
        auto ModelResult = M.buildProgram(*ParseResult);
        if (!ModelResult) {
            std::string ErrMsg;
            llvm::raw_string_ostream OS(ErrMsg);
            OS << ModelResult.takeError();
            for (const auto& Test : Tests) {
                fail(Test.first, ErrMsg.c_str());
            }
            return false;
        }

        Planner Pl(PackagePath);
        auto PlanResult = Pl.plan(*ModelResult);
        if (!PlanResult) {
            std::string ErrMsg;
            llvm::raw_string_ostream OS(ErrMsg);
            OS << PlanResult.takeError();
            for (const auto& Test : Tests) {
                fail(Test.first, ErrMsg.c_str());
            }
            return false;
        }

        // Merge this package's plan into the combined plan
        mergePlans(MergedPlan, *PlanResult);
    }

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    bool AllPassed = true;
    for (const auto& Test : Tests) {
        // Look up the mangled name from the merged plan
        std::string MangledName;
        for (const auto &[name, Func] : MergedPlan.Functions) {
            if (Func.Name == Test.second) {
                MangledName = Func.MangledName;
                break;
            }
        }

        if (MangledName.empty()) {
            fail(Test.first, ("Function not found: " + std::string(Test.second)).c_str());
            AllPassed = false;
            continue;
        }

        auto Result = E.jitExecuteIntFunction(MergedPlan, MangledName);
        if (!Result) {
            std::string ErrMsg;
            llvm::raw_string_ostream OS(ErrMsg);
            OS << Result.takeError();
            fail(Test.first, ErrMsg.c_str());
            AllPassed = false;
        } else if (*Result != 0) {
            std::string Msg = "test returned error code " + std::to_string(*Result);
            fail(Test.first, Msg.c_str());
            AllPassed = false;
        } else {
            pass(Test.first);
        }
    }
    return AllPassed;
}

// ============================================================================
// AOT Test Runner
// ============================================================================

// Parse expected output from test file comments
// Format: "; Expected: <expected output>"
static std::string parseExpectedOutput(llvm::StringRef Source) {
    std::string Expected;
    std::regex ExpectedRegex("; Expected: (.*)");
    std::smatch Match;
    std::string SourceStr = Source.str();
    if (std::regex_search(SourceStr, Match, ExpectedRegex)) {
        Expected = Match[1].str();
    }
    return Expected;
}

// Compile a .scaly file to executable and run it
static bool runAotTest(llvm::StringRef TestPath) {
    std::string TestName = llvm::sys::path::stem(TestPath).str();

    // Read source file
    auto BufOrErr = llvm::MemoryBuffer::getFile(TestPath);
    if (!BufOrErr) {
        fail(("AOT: " + TestName).c_str(), "Cannot read test file");
        return false;
    }

    std::string Source = (*BufOrErr)->getBuffer().str();
    std::string Expected = parseExpectedOutput(Source);

    // Parse
    Parser P(Source);
    auto ParseResult = P.parseProgram();
    if (!ParseResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ParseResult.takeError();
        fail(("AOT: " + TestName).c_str(), ErrMsg.c_str());
        return false;
    }

    // Model
    Modeler M(TestPath);
    auto ModelResult = M.buildProgram(*ParseResult);
    if (!ModelResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ModelResult.takeError();
        fail(("AOT: " + TestName).c_str(), ErrMsg.c_str());
        return false;
    }

    // Plan
    Planner Pl(TestPath);
    auto PlanResult = Pl.plan(*ModelResult);
    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(("AOT: " + TestName).c_str(), ErrMsg.c_str());
        return false;
    }

    // Create temp directory for output
    llvm::SmallString<128> TempDir;
    (void)llvm::sys::fs::current_path(TempDir);
    llvm::sys::path::append(TempDir, "tests", "aot", "build");
    (void)llvm::sys::fs::create_directories(TempDir);

    // Emit object file
    llvm::SmallString<128> ObjPath(TempDir);
    llvm::sys::path::append(ObjPath, TestName + ".o");

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto EmitErr = E.emitObjectFile(*PlanResult, TestName, ObjPath.str().str());
    if (EmitErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << std::move(EmitErr);
        fail(("AOT: " + TestName).c_str(), ErrMsg.c_str());
        return false;
    }

    // Link executable
    llvm::SmallString<128> ExePath(TempDir);
    llvm::sys::path::append(ExePath, TestName);

    auto ClangPath = llvm::sys::findProgramByName("clang");
    if (!ClangPath) {
        fail(("AOT: " + TestName).c_str(), "clang not found");
        return false;
    }

    std::vector<llvm::StringRef> LinkArgs = {
        *ClangPath,
        ObjPath,
        "-o",
        ExePath
    };

    std::string LinkErrMsg;
    int LinkResult = llvm::sys::ExecuteAndWait(
        *ClangPath, LinkArgs, std::nullopt, {}, 0, 0, &LinkErrMsg);

    if (LinkResult != 0) {
        fail(("AOT: " + TestName).c_str(), ("Link failed: " + LinkErrMsg).c_str());
        return false;
    }

    // Run executable and capture output
    llvm::SmallString<128> OutputPath(TempDir);
    llvm::sys::path::append(OutputPath, TestName + ".out");

    std::optional<llvm::StringRef> Redirects[] = {
        std::nullopt,  // stdin
        llvm::StringRef(OutputPath),  // stdout
        std::nullopt   // stderr
    };

    std::string RunErrMsg;
    int RunResult = llvm::sys::ExecuteAndWait(
        ExePath, {ExePath}, std::nullopt, Redirects, 0, 0, &RunErrMsg);

    if (RunResult != 0) {
        fail(("AOT: " + TestName).c_str(), ("Run failed with exit " + std::to_string(RunResult)).c_str());
        return false;
    }

    // Read output
    auto OutBufOrErr = llvm::MemoryBuffer::getFile(OutputPath);
    if (!OutBufOrErr) {
        fail(("AOT: " + TestName).c_str(), "Cannot read output");
        return false;
    }

    std::string Actual = (*OutBufOrErr)->getBuffer().str();
    // Trim trailing newline
    while (!Actual.empty() && (Actual.back() == '\n' || Actual.back() == '\r')) {
        Actual.pop_back();
    }

    // Compare output
    if (Actual != Expected) {
        std::string Msg = "Expected '" + Expected + "' but got '" + Actual + "'";
        fail(("AOT: " + TestName).c_str(), Msg.c_str());
        return false;
    }

    pass(("AOT: " + TestName).c_str());
    return true;
}

// Run all AOT tests in tests/aot/
static bool runAotTests() {
    llvm::outs() << "  AOT compilation tests:\n";

    std::error_code EC;
    llvm::SmallString<128> AotDir;
    (void)llvm::sys::fs::current_path(AotDir);
    llvm::sys::path::append(AotDir, "tests", "aot");

    bool AllPassed = true;
    for (llvm::sys::fs::directory_iterator DI(AotDir, EC), DE;
         DI != DE && !EC; DI.increment(EC)) {
        llvm::StringRef Path = DI->path();
        if (llvm::sys::path::extension(Path) == ".scaly") {
            if (!runAotTest(Path)) {
                AllPassed = false;
            }
        }
    }

    return AllPassed;
}

// ============================================================================
// Main Test Runner
// ============================================================================

bool runModuleTests() {
    llvm::outs() << "Running Module tests...\n";
    llvm::outs().flush();

    TestsPassed = 0;
    TestsFailed = 0;

    // Compile scaly package and run its tests
    // This tests the stdlib: memory, containers, io
    llvm::outs() << "  Scaly package tests:\n";
    runPackageTests(ScalyPackagePath, {
        {"Module: scaly.test()", "scaly.test"},
    });

    // Compile scalyc package and run its tests
    // This tests the self-hosting compiler modules: lexer, parser, compiler
    // Note: scalyc declares "package scaly 0.1.0", so the Modeler loads scaly automatically
    llvm::outs() << "  Scalyc package tests:\n";
    runPackageTests(ScalycPackagePath, {
        {"Module: scalyc.test()", "scalyc.test"},
    });

    // Run AOT compilation tests
    runAotTests();

    llvm::outs() << "\nModule tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
