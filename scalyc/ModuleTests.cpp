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

    return E.jitExecuteIntFunction(*PlanResult, MangledName);
}

// ============================================================================
// Module Test Cases
// ============================================================================

// Path to the scaly package entry point (relative to scalyc/build/)
static const char* ScalyPackagePath = "../../packages/scaly/0.1.0/scaly.scaly";

static bool testMemoryModule() {
    const char* Name = "Module: test_memory()";

    auto ResultOrErr = runPackageTestFunction(ScalyPackagePath, "test_memory");

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    int64_t Result = *ResultOrErr;
    if (Result != 0) {
        std::string Msg = "test returned error code " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testContainersModule() {
    const char* Name = "Module: test_containers()";

    auto ResultOrErr = runPackageTestFunction(ScalyPackagePath, "test_containers");

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    int64_t Result = *ResultOrErr;
    if (Result != 0) {
        std::string Msg = "test returned error code " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Main Test Runner
// ============================================================================

bool runModuleTests() {
    llvm::outs() << "Running Module tests...\n";
    llvm::outs().flush();

    TestsPassed = 0;
    TestsFailed = 0;

    // Memory module tests
    llvm::outs() << "  Memory module tests:\n";
    testMemoryModule();

    // Container module tests
    llvm::outs() << "  Container module tests:\n";
    testContainersModule();

    llvm::outs() << "\nModule tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
