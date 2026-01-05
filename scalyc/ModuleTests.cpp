// ModuleTests.cpp - Tests for complete Scaly modules
//
// This test infrastructure compiles and runs test functions from module files.
// Test functions return int: 0 for success, negative for error codes.
// This allows proper error handling without exit() calls.
//
// NOTE: loadSiblingPrograms is a workaround because the package/module
// resolution system isn't fully implemented yet. The 'use' statement brings
// names into scope but doesn't trigger file loading. Once proper package
// resolution is implemented, this can be simplified.

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
#include <set>
#include <functional>

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

// Workaround: Load sibling .scaly files until proper package resolution is implemented
static void loadSiblingPrograms(Planner &Planner, llvm::StringRef Filename) {
    llvm::SmallString<256> Dir = llvm::sys::path::parent_path(Filename);
    if (Dir.empty()) return;

    std::set<std::string> ProcessedDirs;

    auto loadFilesFromDir = [&](llvm::StringRef DirPath) {
        std::error_code EC;
        for (llvm::sys::fs::directory_iterator DI(DirPath, EC), DE; DI != DE && !EC; DI.increment(EC)) {
            llvm::StringRef SibPath = DI->path();
            if (SibPath == Filename || !SibPath.ends_with(".scaly"))
                continue;

            auto SibBuf = llvm::MemoryBuffer::getFile(SibPath);
            if (!SibBuf) continue;

            Parser SibParser((*SibBuf)->getBuffer());
            auto SibParse = SibParser.parseProgram();
            if (!SibParse) {
                llvm::consumeError(SibParse.takeError());
                continue;
            }

            Modeler SibModeler(SibPath);
            auto SibModel = SibModeler.buildProgram(*SibParse);
            if (!SibModel) {
                llvm::consumeError(SibModel.takeError());
                continue;
            }

            Planner.addSiblingProgram(std::make_shared<Program>(std::move(*SibModel)));
        }
    };

    std::function<void(llvm::StringRef)> loadFromDirRecursive = [&](llvm::StringRef DirPath) {
        if (ProcessedDirs.count(DirPath.str())) return;
        ProcessedDirs.insert(DirPath.str());
        loadFilesFromDir(DirPath);

        std::error_code EC;
        for (llvm::sys::fs::directory_iterator DI(DirPath, EC), DE; DI != DE && !EC; DI.increment(EC)) {
            llvm::StringRef SubPath = DI->path();
            bool IsDir = false;
            if (!llvm::sys::fs::is_directory(SubPath, IsDir) && IsDir)
                loadFromDirRecursive(SubPath);
        }
    };

    loadFromDirRecursive(Dir);

    llvm::SmallString<256> ParentDir = llvm::sys::path::parent_path(Dir);
    if (!ParentDir.empty() && ParentDir != "/" && ParentDir.size() > 4) {
        std::error_code EC;
        for (llvm::sys::fs::directory_iterator DI(ParentDir, EC), DE; DI != DE && !EC; DI.increment(EC)) {
            llvm::StringRef SubPath = DI->path();
            if (SubPath == Dir) continue;
            bool IsDir = false;
            if (!llvm::sys::fs::is_directory(SubPath, IsDir) && IsDir)
                loadFromDirRecursive(SubPath);
        }
    }
}

// Compile a module file and run a test function, returning the int result
static llvm::Expected<int64_t> runModuleTestFunction(
    llvm::StringRef ModulePath,
    llvm::StringRef FunctionName
) {
    auto BufOrErr = llvm::MemoryBuffer::getFile(ModulePath);
    if (!BufOrErr) {
        return llvm::make_error<llvm::StringError>(
            "Cannot open file: " + ModulePath.str(),
            llvm::inconvertibleErrorCode()
        );
    }

    Parser P((*BufOrErr)->getBuffer());
    auto ParseResult = P.parseProgram();
    if (!ParseResult)
        return ParseResult.takeError();

    Modeler M(ModulePath);
    auto ModelResult = M.buildProgram(*ParseResult);
    if (!ModelResult)
        return ModelResult.takeError();

    Planner Pl(ModulePath);
    loadSiblingPrograms(Pl, ModulePath);

    auto PlanResult = Pl.plan(*ModelResult);
    if (!PlanResult)
        return PlanResult.takeError();

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

static bool testMemoryModule() {
    const char* Name = "Module: memory.test()";

    auto ResultOrErr = runModuleTestFunction(
        "../../packages/scaly/0.1.0/scaly/memory.scaly",
        "test"
    );

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
    const char* Name = "Module: containers.test()";

    auto ResultOrErr = runModuleTestFunction(
        "../../packages/scaly/0.1.0/scaly/containers.scaly",
        "test"
    );

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
