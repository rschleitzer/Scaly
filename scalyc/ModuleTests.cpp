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

// Paths to package entry points (relative to scalyc/build/)
static const char* ScalyPackagePath = "../../packages/scaly/0.1.0/scaly.scaly";
static const char* ScalycPackagePath = "../../packages/scalyc/0.1.0/scalyc.scaly";

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

// ============================================================================
// Main Test Runner
// ============================================================================

bool runModuleTests(bool includeSlowTests) {
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

    // Compile scalyc package once and run its tests (slow: ~20s for 16k lines)
    // This tests the self-hosting compiler modules: lexer, parser, compiler
    if (includeSlowTests) {
        llvm::outs() << "  Scalyc package tests:\n";
        runPackageTests(ScalycPackagePath, {
            {"Module: scalyc.test()", "scalyc.test"},
        });
    } else {
        llvm::outs() << "  (Scalyc package tests skipped - use --slow-tests to include)\n";
    }

    llvm::outs() << "\nModule tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
