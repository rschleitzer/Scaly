#include "EmitterTests.h"
#include "Emitter.h"
#include "Plan.h"
#include "llvm/Support/raw_ostream.h"

namespace scaly {

static int TestsPassed = 0;
static int TestsFailed = 0;

static void pass(const char* Name) {
    ++TestsPassed;
    llvm::outs() << "  PASS: " << Name << "\n";
}

static void fail(const char* Name, const char* Message) {
    ++TestsFailed;
    llvm::outs() << "  FAIL: " << Name << " - " << Message << "\n";
}

static bool testJitIntegerConstant() {
    const char* Name = "JIT integer constant";

    // Create a simple plan with an integer constant
    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    // Create a statement that evaluates to integer 42
    PlannedAction Action;
    PlannedOperand Operand;
    IntegerConstant IC;
    IC.Value = 42;
    Operand.Expr = IC;
    PlannedType IntType;
    IntType.Name = "i64";
    IntType.MangledName = "i64";
    Operand.ResultType = IntType;
    Action.Source.push_back(Operand);
    Action.ResultType = IntType;
    P.Statements.push_back(Action);

    // Create emitter and execute
    EmitterConfig Config;
    Config.EmitDebugInfo = false;  // No debug info for tests
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteInt(P);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    int64_t Result = *ResultOrErr;
    if (Result != 42) {
        std::string Msg = "expected 42, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitBooleanTrue() {
    const char* Name = "JIT boolean true";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    BooleanConstant BC;
    BC.Value = true;
    Operand.Expr = BC;
    PlannedType BoolType;
    BoolType.Name = "bool";
    BoolType.MangledName = "bool";
    Operand.ResultType = BoolType;
    Action.Source.push_back(Operand);
    Action.ResultType = BoolType;
    P.Statements.push_back(Action);

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteBool(P);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    bool Result = *ResultOrErr;
    if (!Result) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitBooleanFalse() {
    const char* Name = "JIT boolean false";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    BooleanConstant BC;
    BC.Value = false;
    Operand.Expr = BC;
    PlannedType BoolType;
    BoolType.Name = "bool";
    BoolType.MangledName = "bool";
    Operand.ResultType = BoolType;
    Action.Source.push_back(Operand);
    Action.ResultType = BoolType;
    P.Statements.push_back(Action);

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteBool(P);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    bool Result = *ResultOrErr;
    if (Result) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitNegativeInteger() {
    const char* Name = "JIT negative integer";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    IntegerConstant IC;
    IC.Value = -123;
    Operand.Expr = IC;
    PlannedType IntType;
    IntType.Name = "i64";
    IntType.MangledName = "i64";
    Operand.ResultType = IntType;
    Action.Source.push_back(Operand);
    Action.ResultType = IntType;
    P.Statements.push_back(Action);

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteInt(P);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    int64_t Result = *ResultOrErr;
    if (Result != -123) {
        std::string Msg = "expected -123, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

bool runEmitterTests() {
    llvm::outs() << "Running Emitter tests...\n";

    TestsPassed = 0;
    TestsFailed = 0;

    testJitIntegerConstant();
    testJitBooleanTrue();
    testJitBooleanFalse();
    testJitNegativeInteger();

    llvm::outs() << "\nEmitter tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
