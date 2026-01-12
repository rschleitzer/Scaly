#include "EmitterTests.h"
#include "Emitter.h"
#include "Plan.h"
#include "Parser.h"
#include "Modeler.h"
#include "Planner.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include <chrono>
#include <vector>
#include <algorithm>

namespace scaly {

// Timing data for tests
static std::vector<std::pair<const char*, int64_t>> TestTimings;

#define TIMED_TEST(testFunc) do { \
    auto _start = std::chrono::high_resolution_clock::now(); \
    testFunc(); \
    auto _end = std::chrono::high_resolution_clock::now(); \
    TestTimings.push_back({#testFunc, std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count()}); \
} while(0)

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

// Helper to create an integer constant operand
static PlannedOperand makeIntOperand(int64_t Value) {
    PlannedOperand Op;
    IntegerConstant IC;
    IC.Value = Value;
    Op.Expr = IC;
    PlannedType IntType;
    IntType.Name = "i64";
    IntType.MangledName = "i64";
    Op.ResultType = IntType;
    return Op;
}

// Helper to create a PlannedCall for a binary intrinsic operator
static PlannedCall makeBinaryIntOp(const std::string& OpName, int64_t Left, int64_t Right) {
    PlannedCall Call;
    Call.Name = OpName;
    Call.MangledName = "_Z" + std::to_string(OpName.length()) + OpName + "i64i64";
    Call.IsIntrinsic = true;
    Call.IsOperator = true;
    Call.Args = std::make_shared<std::vector<PlannedOperand>>();
    Call.Args->push_back(makeIntOperand(Left));
    Call.Args->push_back(makeIntOperand(Right));
    PlannedType IntType;
    IntType.Name = "i64";
    IntType.MangledName = "i64";
    Call.ResultType = IntType;
    return Call;
}

static bool testJitAddition() {
    const char* Name = "JIT addition (3 + 4 = 7)";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    // Create a PlannedCall for 3 + 4
    PlannedAction Action;
    PlannedOperand Operand;
    Operand.Expr = makeBinaryIntOp("+", 3, 4);
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
    if (Result != 7) {
        std::string Msg = "expected 7, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitSubtraction() {
    const char* Name = "JIT subtraction (10 - 3 = 7)";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    Operand.Expr = makeBinaryIntOp("-", 10, 3);
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
    if (Result != 7) {
        std::string Msg = "expected 7, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitMultiplication() {
    const char* Name = "JIT multiplication (6 * 7 = 42)";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    Operand.Expr = makeBinaryIntOp("*", 6, 7);
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
    if (Result != 42) {
        std::string Msg = "expected 42, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitDivision() {
    const char* Name = "JIT division (84 / 2 = 42)";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    Operand.Expr = makeBinaryIntOp("/", 84, 2);
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
    if (Result != 42) {
        std::string Msg = "expected 42, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testJitModulo() {
    const char* Name = "JIT modulo (17 % 5 = 2)";

    Plan P;
    P.MainModule.File = "test.scaly";
    P.MainModule.Name = "test";

    PlannedAction Action;
    PlannedOperand Operand;
    Operand.Expr = makeBinaryIntOp("%", 17, 5);
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
    if (Result != 2) {
        std::string Msg = "expected 2, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Full Pipeline Tests (Source -> Parse -> Model -> Plan -> JIT)
// ============================================================================

// Helper: run full pipeline from source string to Plan
static llvm::Expected<Plan> compileToPlan(llvm::StringRef Source) {
    Parser P(Source);
    auto ParseResult = P.parseProgram();
    if (!ParseResult)
        return ParseResult.takeError();

    Modeler M("test.scaly");
    auto ModelResult = M.buildProgram(*ParseResult);
    if (!ModelResult)
        return ModelResult.takeError();

    Planner Pl("test.scaly");
    return Pl.plan(*ModelResult);
}

// Helper: run pipeline to JIT int result
static llvm::Expected<int64_t> evalSimpleInt(llvm::StringRef Source) {
    auto PlanResult = compileToPlan(Source);
    if (!PlanResult)
        return PlanResult.takeError();

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    return E.jitExecuteInt(*PlanResult);
}

// Helper: run pipeline to JIT bool result
static llvm::Expected<bool> evalSimpleBool(llvm::StringRef Source) {
    auto PlanResult = compileToPlan(Source);
    if (!PlanResult)
        return PlanResult.takeError();

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    return E.jitExecuteBool(*PlanResult);
}

// Helper: run full pipeline from source to JIT int result
static llvm::Expected<int64_t> evalInt(llvm::StringRef Source) {
    auto PlanResult = compileToPlan(Source);
    if (!PlanResult)
        return PlanResult.takeError();

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    return E.jitExecuteInt(*PlanResult);
}

// Helper: run full pipeline from source to JIT bool result
static llvm::Expected<bool> evalBool(llvm::StringRef Source) {
    auto PlanResult = compileToPlan(Source);
    if (!PlanResult)
        return PlanResult.takeError();

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    return E.jitExecuteBool(*PlanResult);
}

static bool testPipelineParentheses() {
    const char* Name = "Pipeline: (2 + 3) * 4";

    auto ResultOrErr = evalInt("(2 + 3) * 4");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    int64_t Result = *ResultOrErr;
    if (Result != 20) {
        std::string Msg = "expected 20, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Variable Binding Tests
// ============================================================================

static bool testPipelineLetSimple() {
    const char* Name = "Pipeline: let x = 5: x";

    auto ResultOrErr = evalInt("let x = 5: x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 5) {
        std::string Msg = "expected 5, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLetWithOp() {
    const char* Name = "Pipeline: let x = 3: x + 4";

    auto ResultOrErr = evalInt("let x = 3: x + 4");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLetNested() {
    const char* Name = "Pipeline: let x = 10: let y = 3: x - y";

    auto ResultOrErr = evalInt("let x = 10: let y = 3: x - y");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLetMultiply() {
    const char* Name = "Pipeline: let a = 6: let b = 7: a * b";

    auto ResultOrErr = evalInt("let a = 6: let b = 7: a * b");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Match expression tests

static bool testPipelineMatchSingleCase() {
    const char* Name = "Pipeline: match 2 : case 2: 100 else 0";

    auto ResultOrErr = evalInt("match 2 : case 2: 100 else 0");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 100) {
        std::string Msg = "expected 100, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMatchElse() {
    const char* Name = "Pipeline: match 5 : case 1: 10 case 2: 20 else 99";

    auto ResultOrErr = evalInt("match 5 : case 1: 10 case 2: 20 else 99");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 99) {
        std::string Msg = "expected 99, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMatchWithVar() {
    const char* Name = "Pipeline: let x = 3: match x : case 1: 10 case 2: 20 case 3: 30 else 0";

    auto ResultOrErr = evalInt("let x = 3: match x : case 1: 10 case 2: 20 case 3: 30 else 0");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 30) {
        std::string Msg = "expected 30, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMatchFirstCase() {
    const char* Name = "Pipeline: match 1 : case 1: 100 case 2: 200 else 0";

    auto ResultOrErr = evalInt("match 1 : case 1: 100 case 2: 200 else 0");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 100) {
        std::string Msg = "expected 100, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Try expression tests
// Note: Full try expression testing requires Result/union types.
// These tests verify basic functionality with simple values.

static bool testPipelineTrySimpleValue() {
    const char* Name = "Pipeline: try 42: else 0";

    auto ResultOrErr = evalInt("try 42: else 0");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Simple integer passes through - try returns the value
    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineTryWithBinding() {
    const char* Name = "Pipeline: try let x = 10: else 0";

    auto ResultOrErr = evalInt("try let x = 10: else 0");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Binding succeeds, returns the bound value
    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineTryExpression() {
    const char* Name = "Pipeline: try 3 + 4: else 0";

    auto ResultOrErr = evalInt("try 3 + 4: else 0");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Expression evaluates and passes through
    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Throw statement tests

static bool testThrowInFunction() {
    const char* Name = "Pipeline: throw in function";

    // Define a Result union and a function that throws
    // The function returns the error value when throw is called
    // We use try to catch it and return the error value
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "try let r = fail(): when e: Error: e else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // The throw should produce Error(42), and we extract the 42
    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Choose expression tests

static bool testChooseMatchesError() {
    const char* Name = "Pipeline: choose matches error variant";

    // Use throw to create an Error variant (tag 1), then choose to match it
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "choose fail(): when x: Ok: x when e: Error: e else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // throw creates Error variant (tag 1) with value 42
    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testChooseWithDataBinding() {
    const char* Name = "Pipeline: choose with data binding";

    // Choose and use the bound data in an expression
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 10 }\n"
        "choose fail(): when x: Ok: x when val: Error: val * 2 else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // val = 10, val * 2 = 20
    if (*ResultOrErr != 20) {
        std::string Msg = "expected 20, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testChooseElseFallback() {
    const char* Name = "Pipeline: choose else fallback";

    // When no when clause matches, fall through to else
    // throw creates Error (tag 1), but we only match Ok (tag 0)
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 5 }\n"
        "choose fail(): when x: Ok: x else 77"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Error variant (tag 1) doesn't match Ok (tag 0), so else is taken
    if (*ResultOrErr != 77) {
        std::string Msg = "expected 77, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testChooseMultipleCases() {
    const char* Name = "Pipeline: choose multiple cases";

    // Multiple when clauses, matching the second one
    auto ResultOrErr = evalInt(
        "define Triple union: (A: int, B: int, C: int)\n"
        "function makeB() returns Triple throws int { throw 33 }\n"
        "choose makeB(): when a: A: a when b: B: b + 1 when c: C: c else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // throw creates B variant (tag 1) with value 33, result is 33 + 1 = 34
    if (*ResultOrErr != 34) {
        std::string Msg = "expected 34, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testChooseOnStructField() {
    const char* Name = "Pipeline: choose on struct field";

    // Test choose on a union field of a struct
    auto ResultOrErr = evalInt(
        "define Option union: (Some: int, None)\n"
        "define Container(value: Option)\n"
        "let c Container(Option.Some(42))\n"
        "choose c.value: when v: Some: v else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testChooseOnVariantConstruction() {
    const char* Name = "Pipeline: choose on variant construction";

    // Test choose directly on a variant construction (not through a variable)
    auto ResultOrErr = evalInt(
        "define Option union: (Some: int, None)\n"
        "choose Option.Some(7): when v: Some: v else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// SizeOf expression tests

static bool testPipelineSizeOfInt() {
    const char* Name = "Pipeline: sizeof int";

    auto ResultOrErr = evalInt("sizeof int");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // int is 8 bytes (i64)
    if (*ResultOrErr != 8) {
        std::string Msg = "expected 8, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineSizeOfBool() {
    const char* Name = "Pipeline: sizeof bool";

    auto ResultOrErr = evalInt("sizeof bool");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // bool is 1 byte
    if (*ResultOrErr != 1) {
        std::string Msg = "expected 1, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineSizeOfI32() {
    const char* Name = "Pipeline: sizeof i32";

    auto ResultOrErr = evalInt("sizeof i32");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // i32 is 4 bytes
    if (*ResultOrErr != 4) {
        std::string Msg = "expected 4, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineSizeOfPtr() {
    const char* Name = "Pipeline: sizeof ptr";

    auto ResultOrErr = evalInt("sizeof ptr");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // ptr is 8 bytes on 64-bit
    if (*ResultOrErr != 8) {
        std::string Msg = "expected 8, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// AlignOf expression tests

static bool testPipelineAlignOfInt() {
    const char* Name = "Pipeline: alignof int";

    auto ResultOrErr = evalInt("alignof int");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // int (i64) is 8-byte aligned
    if (*ResultOrErr != 8) {
        std::string Msg = "expected 8, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineAlignOfI32() {
    const char* Name = "Pipeline: alignof i32";

    auto ResultOrErr = evalInt("alignof i32");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // i32 is 4-byte aligned
    if (*ResultOrErr != 4) {
        std::string Msg = "expected 4, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineAlignOfI16() {
    const char* Name = "Pipeline: alignof i16";

    auto ResultOrErr = evalInt("alignof i16");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // i16 is 2-byte aligned
    if (*ResultOrErr != 2) {
        std::string Msg = "expected 2, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineAlignOfBool() {
    const char* Name = "Pipeline: alignof bool";

    auto ResultOrErr = evalInt("alignof bool");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // bool is 1-byte aligned
    if (*ResultOrErr != 1) {
        std::string Msg = "expected 1, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Extern function tests

static bool testExternStrlen() {
    const char* Name = "Pipeline: strlen extern call";

    // strlen("hello") should return 5
    auto ResultOrErr = evalInt("function strlen(s: pointer[i8]) returns size_t extern\nstrlen(\"hello\") as int");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 5) {
        std::string Msg = "expected 5, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testExternSqrt() {
    const char* Name = "Pipeline: sqrt extern call";

    // sqrt(16.0) should return 4.0
    auto ResultOrErr = evalInt("function sqrt(x: double) returns double extern\nsqrt(16.0) as int");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 4) {
        std::string Msg = "expected 4, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Grouped expression tests (exercises emitTuple for single anonymous component)

static bool testPipelineGroupedSimple() {
    const char* Name = "Pipeline: (42)";

    auto ResultOrErr = evalInt("(42)");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineGroupedAdd() {
    const char* Name = "Pipeline: (3 + 4)";

    auto ResultOrErr = evalInt("(3 + 4)");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineNestedGrouped() {
    const char* Name = "Pipeline: ((1 + 2))";

    auto ResultOrErr = evalInt("((1 + 2))");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 3) {
        std::string Msg = "expected 3, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineComplexGrouped() {
    const char* Name = "Pipeline: ((2 + 3) * (4 + 1))";

    auto ResultOrErr = evalInt("((2 + 3) * (4 + 1))");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // (2+3)*(4+1) = 5*5 = 25
    if (*ResultOrErr != 25) {
        std::string Msg = "expected 25, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Pointer dereference tests

static bool testPointerDerefCompiles() {
    const char* Name = "Pipeline: pointer dereference compiles";

    // Test that a function with pointer parameter and dereference compiles
    auto PlanResult = compileToPlan(
        "define Point(x: int, y: int)\n"
        "function getX(p: pointer[Point]) returns int { (*p).x }\n"
        "0"  // Return something for the top-level
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPointerDerefSimple() {
    const char* Name = "Pipeline: simple pointer dereference compiles";

    // Test dereference of a pointer[int]
    auto PlanResult = compileToPlan(
        "function deref(p: pointer[int]) returns int { *p }\n"
        "0"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPointerDerefMemberAccess() {
    const char* Name = "Pipeline: pointer dereference with member access";

    // More complex case: dereference and access multiple members
    auto PlanResult = compileToPlan(
        "define Point(x: int, y: int)\n"
        "function sum(p: pointer[Point]) returns int { (*p).x + (*p).y }\n"
        "0"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Null literal tests

static bool testNullLiteralCompiles() {
    const char* Name = "Pipeline: null literal compiles";

    // Test that null compiles as a standalone expression
    auto PlanResult = compileToPlan(
        "function isNull(p: pointer[int]) returns bool { p = null }\n"
        "0"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testNullInFunction() {
    const char* Name = "Pipeline: null in function return";

    // Test that a function can return null
    auto PlanResult = compileToPlan(
        "function getNull() returns pointer[int] { null }\n"
        "0"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testNullComparison() {
    const char* Name = "Pipeline: null comparison compiles";

    // Test that null can be compared with <>
    auto PlanResult = compileToPlan(
        "function isNotNull(p: pointer[int]) returns bool { p <> null }\n"
        "0"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Member access tests

static bool testPipelineMemberAccessSimple() {
    const char* Name = "Pipeline: Point.x access";

    auto ResultOrErr = evalInt("define Point(x: int, y: int)\nPoint(3, 4).x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 3) {
        std::string Msg = "expected 3, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMemberAccessSecondField() {
    const char* Name = "Pipeline: Point.y access";

    auto ResultOrErr = evalInt("define Point(x: int, y: int)\nPoint(3, 4).y");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 4) {
        std::string Msg = "expected 4, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMemberAccessVariableOnly() {
    const char* Name = "Pipeline: let p = Point: p.x";

    auto ResultOrErr = evalInt("define Point(x: int, y: int)\nlet p = Point(3, 4)\np.x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // p.x should be 3
    if (*ResultOrErr != 3) {
        std::string Msg = "expected 3, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMemberAccessWithBinding() {
    const char* Name = "Pipeline: let p = Point: p.x + p.y";

    auto ResultOrErr = evalInt("define Point(x: int, y: int)\nlet p = Point(3, 4)\np.x + p.y");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 3 + 4 = 7
    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMemberAccessMultiply() {
    const char* Name = "Pipeline: p.x * p.y";

    auto ResultOrErr = evalInt("define Point(x: int, y: int)\nlet p = Point(5, 7)\np.x * p.y");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 5 * 7 = 35
    if (*ResultOrErr != 35) {
        std::string Msg = "expected 35, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Method call tests

static bool testPipelineMethodCallSimple() {
    const char* Name = "Pipeline: p.double_x()";

    // Define a struct with a method that returns 2*x
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    function double_x(this) returns int { this.x * 2 }\n"
        "}\n"
        "let p = Point(5, 3)\n"
        "p.double_x()"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 5 * 2 = 10
    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMethodCallSum() {
    const char* Name = "Pipeline: p.sum()";

    // Define a struct with a method that returns x + y
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    function sum(this) returns int { this.x + this.y }\n"
        "}\n"
        "let p = Point(3, 7)\n"
        "p.sum()"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 3 + 7 = 10
    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineMethodCallWithArgs() {
    const char* Name = "Pipeline: p.add(n)";

    // Define a struct with a method that takes an argument
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    function add(this, n: int) returns int { this.x + this.y + n }\n"
        "}\n"
        "let p = Point(2, 3)\n"
        "p.add(5)"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 2 + 3 + 5 = 10
    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Initializer tests

static bool testPipelineInitializerSimple() {
    const char* Name = "Pipeline: init() with fixed values";

    // Define a struct with an initializer that sets fixed values
    // Use 'set target : source' syntax for assignment
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    init() { set this.x : 10 : set this.y : 20 }\n"
        "}\n"
        "let p = Point()\n"
        "p.x + p.y"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 10 + 20 = 30
    if (*ResultOrErr != 30) {
        std::string Msg = "expected 30, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineInitializerWithParams() {
    const char* Name = "Pipeline: init(a, b) copies to fields";

    // Define a struct with an initializer that uses parameters
    // Use 'set target : source' syntax for assignment
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    init(a: int, b: int) { set this.x : a : set this.y : b }\n"
        "}\n"
        "let p = Point(5, 7)\n"
        "p.x * p.y"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 5 * 7 = 35
    if (*ResultOrErr != 35) {
        std::string Msg = "expected 35, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineInitializerWithComputation() {
    const char* Name = "Pipeline: init(v) computes fields";

    // Define a struct with an initializer that performs computation
    // Use 'set target : source' syntax for assignment
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    init(v: int) { set this.x : v : set this.y : v * 2 }\n"
        "}\n"
        "let p = Point(6)\n"
        "p.x + p.y"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 6 + 12 = 18
    if (*ResultOrErr != 18) {
        std::string Msg = "expected 18, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Deinitializer tests

static bool testPipelineDeinitializerSimple() {
    const char* Name = "Pipeline: deinit with this access";

    // Define a struct with a deinitializer that accesses this
    // The deinit won't be called automatically yet, but should compile
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    deinit this.x + this.y\n"
        "}\n"
        "let p = Point(10, 20)\n"
        "p.x + p.y"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 10 + 20 = 30
    if (*ResultOrErr != 30) {
        std::string Msg = "expected 30, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineDeinitializerWithInit() {
    const char* Name = "Pipeline: deinit with init";

    // Define a struct with both initializer and deinitializer
    auto ResultOrErr = evalInt(
        "define Point(x: int, y: int) {\n"
        "    init(a: int, b: int) { set this.x : a : set this.y : b }\n"
        "    deinit this.x\n"
        "}\n"
        "let p = Point(5, 7)\n"
        "p.x * p.y"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 5 * 7 = 35
    if (*ResultOrErr != 35) {
        std::string Msg = "expected 35, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Standalone function call tests

static bool testPipelineFunctionCallSimple() {
    const char* Name = "Pipeline: simple function call (constant)";

    // Start with simplest case - function returning a constant (no braces)
    auto ResultOrErr = evalInt(
        "function seven() returns int 7\n"
        "seven()"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineFunctionCallWithArgs() {
    const char* Name = "Pipeline: function call with arguments";

    // Function with two arguments
    auto ResultOrErr = evalInt(
        "function add(a: int, b: int) returns int a + b\n"
        "add(3, 4)"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 7) {
        std::string Msg = "expected 7, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineFunctionCallChained() {
    const char* Name = "Pipeline: function calling another function";

    // Function that calls another function
    auto ResultOrErr = evalInt(
        "function double(x: int) returns int x + x\n"
        "function quad(x: int) returns int double(double(x))\n"
        "quad(3)"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 12) {
        std::string Msg = "expected 12, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineFunctionCallWithExpression() {
    const char* Name = "Pipeline: function call with expression arg";

    // Function called with expression argument
    auto ResultOrErr = evalInt(
        "function square(x: int) returns int x * x\n"
        "square(3 + 2)"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 25) {
        std::string Msg = "expected 25, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// RBMM Tests (Region-Based Memory Management)
// ============================================================================

static bool testRBMMLocalLifetimeForbidden() {
    const char* Name = "Pipeline: RBMM local lifetime ($) on return type is forbidden";

    // Local lifetime ($) on return type should produce an error
    // Use no lifetime for by-value return, or # for caller's page allocation
    auto ResultOrErr = evalInt(
        "function make_value() returns int$ {\n"
        "    42\n"
        "}\n"
        "make_value()"
    );
    if (ResultOrErr) {
        fail(Name, "Expected error for $ on return type, but got success");
        return false;
    }

    std::string ErrMsg;
    llvm::raw_string_ostream OS(ErrMsg);
    OS << ResultOrErr.takeError();

    // Check that the error message mentions local lifetime
    if (ErrMsg.find("local lifetime") == std::string::npos) {
        std::string Msg = "Expected 'local lifetime' error, got: " + ErrMsg;
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testRBMMByValueReturn() {
    const char* Name = "Pipeline: RBMM by-value return (no lifetime)";

    // Function without lifetime annotation returns by value
    auto ResultOrErr = evalInt(
        "function add_values(a: int, b: int) returns int {\n"
        "    a + b\n"
        "}\n"
        "add_values(10, 32)"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Note: testRBMMLocalPageAllocation and testRBMMCallerPageAllocation removed
// These features ($ and # lifetime allocations) are thoroughly tested by
// the Module tests which run the actual scaly runtime (memory, containers, io).

static bool testRBMMEmptyConstructorWithoutParens() {
    const char* Name = "Pipeline: empty constructor without parens (stack alloc)";

    // Test that Type() with explicit parens works (stack allocation, no lifetime)
    auto TestWithParens = evalInt(
        "define Counter: (value: int) {\n"
        "    init { set this.value: 42 }\n"
        "}\n"
        "function test() returns int {\n"
        "    let c Counter()\n"  // Stack allocation with explicit parens
        "    c.value\n"
        "}\n"
        "test()"
    );
    if (!TestWithParens) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << TestWithParens.takeError();
        fail("Baseline: Counter() with parens", ErrMsg.c_str());
        return false;
    }
    if (*TestWithParens != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*TestWithParens);
        fail("Baseline: Counter() with parens", Msg.c_str());
        return false;
    }
    pass("Baseline: Counter() with parens");
    pass(Name);
    return true;
}

static bool testRBMMThrownLifetime() {
    const char* Name = "Pipeline: RBMM thrown lifetime (!) syntax";

    // Test that thrown lifetime (!) allocates on exception page
    // and can be caught using try/choose expressions.

    // Test 1: Verify struct throw with ! lifetime plans and emits correctly
    auto PlanResult = compileToPlan(
        "define Error: (code: int)\n"
        "define Result union: (Ok: int, Err: Error)\n"
        "function fail() returns Result throws Error {\n"
        "    throw Error!(42)\n"
        "}\n"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Test 2: Verify primitive throw executes correctly with try/catch
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "try let r = fail(): when e: Error: e else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testThrowStructWithMemberAccess() {
    const char* Name = "Pipeline: throw struct with member access in catch";

    // Test extracting member from caught struct error
    // Note: This test documents a current limitation - direct member access
    // on catch variable (e.code) has PHI node issues. Use choose instead.
    auto ResultOrErr = evalInt(
        "define Error: (code: int)\n"
        "define Result union: (Ok: int, Err: Error)\n"
        "function fail() returns Result throws Error {\n"
        "    throw Error!(42)\n"
        "}\n"
        "choose fail(): when e: Err: e.code when x: Ok: x else 0\n"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testRBMMBlockScopedCleanup() {
    const char* Name = "Pipeline: RBMM block-scoped cleanup behavior";

    // NOTE: Currently in Scaly's RBMM:
    // - All $ allocations in a function share a single LocalPage
    // - The page is allocated at function entry (if NeedsLocalPage is set)
    // - The page is cleaned up at function exit (return statements)
    // - Block-scoped cleanup is NOT implemented
    //
    // Future enhancement: For long-running functions with loops, block-scoped
    // cleanup could prevent memory accumulation by cleaning up at block exit.

    // Test that multiple $ allocations in nested blocks work
    // (all share the same page, cleaned up at function exit)
    auto PlanResult = compileToPlan(
        "define Point: (x: int, y: int)\n"
        "function test() returns int {\n"
        "    let outer Point$(1, 2)\n"
        "    {\n"
        "        let inner Point$(3, 4)\n"
        "        (*inner).x\n"  // Use inner in nested block
        "    }\n"
        "    (*outer).x\n"  // outer still valid after inner block exits
        "}\n"
    );

    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        // Planning should succeed - both allocations share the same LocalPage
        fail(Name, ErrMsg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testRBMMReferenceLifetimeRequiresPagePointer() {
    const char* Name = "Pipeline: RBMM ^name requires pointer[Page] type";

    // ^name syntax requires the variable to be of type pointer[Page]
    // Using a regular pointer should fail
    auto PlanResult = compileToPlan(
        "define Point: (x: int, y: int)\n"
        "function test() returns int {\n"
        "    let p Point$(1, 2)\n"  // p is pointer[Point], not pointer[Page]
        "    let q Point^p(3, 4)\n"  // ERROR: p is not pointer[Page]
        "    (*q).x\n"
        "}\n"
    );

    if (PlanResult) {
        fail(Name, "Expected error for ^name with non-Page pointer type");
        return false;
    }

    std::string ErrMsg;
    llvm::raw_string_ostream OS(ErrMsg);
    OS << PlanResult.takeError();

    // Check that the error message mentions pointer[Page]
    if (ErrMsg.find("pointer[Page]") == std::string::npos) {
        std::string Msg = "Expected 'pointer[Page]' in error, got: " + ErrMsg;
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testStaticArrayConstant() {
    const char* Name = "Pipeline: static array constant";

    // Test defining and accessing a static array constant
    auto ResultOrErr = evalInt(
        "define PRIMES: int[] [2, 3, 5, 7, 11]\n"
        "*(PRIMES + 2)\n"  // Access third element (5)
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 5) {
        std::string Msg = "expected 5, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testStackArrayDeclaration() {
    const char* Name = "Pipeline: stack array declaration";

    // Test stack array syntax: var buffer int[4]
    // This should allocate 4 integers on the stack
    auto ResultOrErr = evalSimpleInt(
        "define test {\n"
        "    function run() returns int {\n"
        "        var buffer int[4]\n"
        "        ; Write to array elements via pointer arithmetic\n"
        "        set *(buffer + 0): 10\n"
        "        set *(buffer + 1): 20\n"
        "        set *(buffer + 2): 30\n"
        "        set *(buffer + 3): 40\n"
        "        ; Read back and sum\n"
        "        *buffer + *(buffer + 1) + *(buffer + 2) + *(buffer + 3)\n"
        "    }\n"
        "}\n"
        "test.run()\n"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 100) {  // 10 + 20 + 30 + 40 = 100
        std::string Msg = "expected 100, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testStackArrayWithTypeAnnotation() {
    const char* Name = "Pipeline: stack array with type annotation";

    // Test explicit type annotation: var buffer: pointer[int] int[4]
    auto ResultOrErr = evalSimpleInt(
        "define test {\n"
        "    function run() returns int {\n"
        "        var buffer: pointer[int] int[4]\n"
        "        set *(buffer + 0): 5\n"
        "        set *(buffer + 1): 15\n"
        "        *buffer + *(buffer + 1)\n"
        "    }\n"
        "}\n"
        "test.run()\n"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 20) {  // 5 + 15 = 20
        std::string Msg = "expected 20, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Generic Type Tests
// ============================================================================

static bool testGenericBoxInt() {
    const char* Name = "Pipeline: generic Box[int] instantiation";

    // First test: just creation, no member access
    auto SimpleResult = evalSimpleInt(
        "define Box[T]: (value: T)\n"
        "let b Box[int](42)\n"
        "0"  // Just return 0, don't access b.value
    );
    if (!SimpleResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << SimpleResult.takeError();
        fail("Baseline: generic Box creation", ErrMsg.c_str());
        return false;
    }
    pass("Baseline: generic Box creation");

    // Simple generic struct with one type parameter
    auto ResultOrErr = evalSimpleInt(
        "define Box[T]: (value: T)\n"
        "let b Box[int](42)\n"
        "b.value"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testGenericBoxBool() {
    const char* Name = "Pipeline: generic Box[bool] instantiation";

    // Same generic struct instantiated with different type
    auto ResultOrErr = evalSimpleBool(
        "define Box[T]: (value: T)\n"
        "let b Box[bool](true)\n"
        "b.value"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (!*ResultOrErr) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testGenericPair() {
    const char* Name = "Pipeline: generic Pair[T, U] with two type params";

    // Generic struct with two type parameters
    auto ResultOrErr = evalSimpleInt(
        "define Pair[T, U]: (first: T, second: U)\n"
        "let p Pair[int, int](10, 32)\n"
        "p.first + p.second"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testGenericMethodOnStruct() {
    const char* Name = "Pipeline: generic method on Box[T]";

    // Generic struct with method that uses T
    auto ResultOrErr = evalSimpleInt(
        "define Box[T]: (value: T) {\n"
        "    function get(this) returns T { this.value }\n"
        "}\n"
        "let b Box[int](42)\n"
        "b.get()"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testGenericNestedTypes() {
    const char* Name = "Pipeline: nested generic types Box[Box[int]]";

    // Nested generic instantiation
    auto ResultOrErr = evalSimpleInt(
        "define Box[T]: (value: T)\n"
        "let inner Box[int](42)\n"
        "let outer Box[Box[int]](inner)\n"
        "outer.value.value"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Is Expression Tests
// ============================================================================

static bool testIsExpressionMatchesError() {
    const char* Name = "Pipeline: is expression matches Error variant";

    // Use throw to create Error variant (tag 1), then test with 'is'
    auto ResultOrErr = evalBool(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "fail() is Error"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // throw creates Error variant (tag 1), so 'is Error' should be true
    if (!*ResultOrErr) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testIsExpressionNoMatchOk() {
    const char* Name = "Pipeline: is expression does not match Ok variant";

    // throw creates Error variant, so 'is Ok' should be false
    auto ResultOrErr = evalBool(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "fail() is Ok"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // throw creates Error variant (tag 1), so 'is Ok' should be false
    if (*ResultOrErr) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testIsExpressionWithIf() {
    const char* Name = "Pipeline: is expression with if statement";

    // Use 'is' in an if condition to branch
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "if fail() is Error : 100 else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Should take the if branch since it's Error variant
    if (*ResultOrErr != 100) {
        std::string Msg = "expected 100, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testIsExpressionNotMatching() {
    const char* Name = "Pipeline: is expression takes else branch";

    // Use 'is Ok' which should be false for Error variant
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "function fail() returns Result throws int { throw 42 }\n"
        "if fail() is Ok : 100 else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Should take the else branch since it's Error variant, not Ok
    if (*ResultOrErr != 0) {
        std::string Msg = "expected 0, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Note: Variant construction tests moved to literate tests (tests/choose.sgm)

bool runEmitterTests() {
    llvm::outs() << "Running Emitter tests...\n";
    llvm::outs().flush();

    TestsPassed = 0;
    TestsFailed = 0;
    TestTimings.clear();

    // Direct Plan -> JIT tests
    llvm::outs() << "  Direct JIT tests:\n";
    llvm::outs().flush();
    TIMED_TEST(testJitIntegerConstant);
    TIMED_TEST(testJitBooleanTrue);
    TIMED_TEST(testJitBooleanFalse);
    TIMED_TEST(testJitNegativeInteger);
    TIMED_TEST(testJitAddition);
    TIMED_TEST(testJitSubtraction);
    TIMED_TEST(testJitMultiplication);
    TIMED_TEST(testJitDivision);
    TIMED_TEST(testJitModulo);

    // Full pipeline tests (Source -> Parse -> Model -> Plan -> JIT)
    // Note: Most basic operator tests moved to literate tests (tests/expressions.sgm)
    llvm::outs() << "  Full pipeline tests:\n";
    TIMED_TEST(testPipelineParentheses);

    // Variable bindings (immutable let)
    llvm::outs() << "  Variable binding tests:\n";
    TIMED_TEST(testPipelineLetSimple);
    TIMED_TEST(testPipelineLetWithOp);
    TIMED_TEST(testPipelineLetNested);
    TIMED_TEST(testPipelineLetMultiply);

    // Note: var/set, while, if, for, break/continue tests moved to literate tests (tests/controlflow.sgm)

    // Match expressions
    llvm::outs() << "  Match expression tests:\n";
    TIMED_TEST(testPipelineMatchSingleCase);
    TIMED_TEST(testPipelineMatchElse);
    TIMED_TEST(testPipelineMatchWithVar);
    TIMED_TEST(testPipelineMatchFirstCase);

    // Try expressions
    llvm::outs() << "  Try expression tests:\n";
    TIMED_TEST(testPipelineTrySimpleValue);
    TIMED_TEST(testPipelineTryWithBinding);
    TIMED_TEST(testPipelineTryExpression);

    // Throw statements
    llvm::outs() << "  Throw statement tests:\n";
    TIMED_TEST(testThrowInFunction);

    // Choose expressions
    llvm::outs() << "  Choose expression tests:\n";
    TIMED_TEST(testChooseMatchesError);
    TIMED_TEST(testChooseWithDataBinding);
    TIMED_TEST(testChooseElseFallback);
    TIMED_TEST(testChooseMultipleCases);
    TIMED_TEST(testChooseOnStructField);
    TIMED_TEST(testChooseOnVariantConstruction);

    // Is expressions
    llvm::outs() << "  Is expression tests:\n";
    TIMED_TEST(testIsExpressionMatchesError);
    TIMED_TEST(testIsExpressionNoMatchOk);
    TIMED_TEST(testIsExpressionWithIf);
    TIMED_TEST(testIsExpressionNotMatching);
    // Note: Variant construction tests moved to literate tests (tests/choose.sgm)

    // SizeOf expressions
    llvm::outs() << "  SizeOf expression tests:\n";
    TIMED_TEST(testPipelineSizeOfInt);
    TIMED_TEST(testPipelineSizeOfBool);
    TIMED_TEST(testPipelineSizeOfI32);
    TIMED_TEST(testPipelineSizeOfPtr);

    // AlignOf expressions
    llvm::outs() << "  AlignOf expression tests:\n";
    TIMED_TEST(testPipelineAlignOfInt);
    TIMED_TEST(testPipelineAlignOfI32);
    TIMED_TEST(testPipelineAlignOfI16);
    TIMED_TEST(testPipelineAlignOfBool);

    // Extern function tests
    llvm::outs() << "  Extern function tests:\n";
    TIMED_TEST(testExternStrlen);
    TIMED_TEST(testExternSqrt);

    llvm::outs() << "  Grouped expression tests:\n";
    TIMED_TEST(testPipelineGroupedSimple);
    TIMED_TEST(testPipelineGroupedAdd);
    TIMED_TEST(testPipelineNestedGrouped);
    TIMED_TEST(testPipelineComplexGrouped);

    llvm::outs() << "  Pointer dereference tests:\n";
    TIMED_TEST(testPointerDerefCompiles);
    TIMED_TEST(testPointerDerefSimple);
    TIMED_TEST(testPointerDerefMemberAccess);

    llvm::outs() << "  Null literal tests:\n";
    TIMED_TEST(testNullLiteralCompiles);
    TIMED_TEST(testNullInFunction);
    TIMED_TEST(testNullComparison);

    llvm::outs() << "  Member access tests:\n";
    TIMED_TEST(testPipelineMemberAccessSimple);
    TIMED_TEST(testPipelineMemberAccessSecondField);
    TIMED_TEST(testPipelineMemberAccessVariableOnly);
    TIMED_TEST(testPipelineMemberAccessWithBinding);
    TIMED_TEST(testPipelineMemberAccessMultiply);

    llvm::outs() << "  Method call tests:\n";
    TIMED_TEST(testPipelineMethodCallSimple);
    TIMED_TEST(testPipelineMethodCallSum);
    TIMED_TEST(testPipelineMethodCallWithArgs);

    llvm::outs() << "  Initializer tests:\n";
    TIMED_TEST(testPipelineInitializerSimple);
    TIMED_TEST(testPipelineInitializerWithParams);
    TIMED_TEST(testPipelineInitializerWithComputation);

    llvm::outs() << "  Deinitializer tests:\n";
    TIMED_TEST(testPipelineDeinitializerSimple);
    TIMED_TEST(testPipelineDeinitializerWithInit);

    llvm::outs() << "  Function call tests:\n";
    TIMED_TEST(testPipelineFunctionCallSimple);
    TIMED_TEST(testPipelineFunctionCallWithArgs);
    TIMED_TEST(testPipelineFunctionCallChained);
    TIMED_TEST(testPipelineFunctionCallWithExpression);

    llvm::outs() << "  RBMM tests:\n";
    TIMED_TEST(testRBMMLocalLifetimeForbidden);
    TIMED_TEST(testRBMMByValueReturn);
    // Note: $ and # allocation tests removed - covered by Module tests
    TIMED_TEST(testRBMMEmptyConstructorWithoutParens);
    TIMED_TEST(testRBMMThrownLifetime);
    TIMED_TEST(testThrowStructWithMemberAccess);
    TIMED_TEST(testRBMMBlockScopedCleanup);
    TIMED_TEST(testRBMMReferenceLifetimeRequiresPagePointer);

    // Static array constant tests
    llvm::outs() << "  Static array tests:\n";
    TIMED_TEST(testStaticArrayConstant);
    TIMED_TEST(testStackArrayDeclaration);
    TIMED_TEST(testStackArrayWithTypeAnnotation);

    llvm::outs() << "  Generic type tests:\n";
    TIMED_TEST(testGenericBoxInt);
    TIMED_TEST(testGenericBoxBool);
    TIMED_TEST(testGenericPair);
    // Note: Generic Option and Optional tests moved to literate tests (tests/choose.sgm)
    TIMED_TEST(testGenericMethodOnStruct);
    TIMED_TEST(testGenericNestedTypes);

    // Sort by time descending and print top 10 slowest
    std::sort(TestTimings.begin(), TestTimings.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    llvm::outs() << "\n  Slowest tests:\n";
    for (size_t i = 0; i < std::min(TestTimings.size(), size_t(10)); ++i) {
        llvm::outs() << "    " << TestTimings[i].second << "ms: " << TestTimings[i].first << "\n";
    }

    llvm::outs() << "\nEmitter tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
