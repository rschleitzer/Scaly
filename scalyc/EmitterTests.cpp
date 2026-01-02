#include "EmitterTests.h"
#include "Emitter.h"
#include "Plan.h"
#include "Parser.h"
#include "Modeler.h"
#include "Planner.h"
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

static bool testPipelineIntegerLiteral() {
    const char* Name = "Pipeline: integer literal (42)";

    auto ResultOrErr = evalInt("42");
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

static bool testPipelineNegativeLiteral() {
    const char* Name = "Pipeline: negative literal (-7)";

    auto ResultOrErr = evalInt("-7");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    int64_t Result = *ResultOrErr;
    if (Result != -7) {
        std::string Msg = "expected -7, got " + std::to_string(Result);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineAddition() {
    const char* Name = "Pipeline: 3 + 4";

    auto ResultOrErr = evalInt("3 + 4");
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

static bool testPipelineSubtraction() {
    const char* Name = "Pipeline: 10 - 3";

    auto ResultOrErr = evalInt("10 - 3");
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

static bool testPipelineMultiplication() {
    const char* Name = "Pipeline: 6 * 7";

    auto ResultOrErr = evalInt("6 * 7");
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

static bool testPipelineDivision() {
    const char* Name = "Pipeline: 84 / 2";

    auto ResultOrErr = evalInt("84 / 2");
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

static bool testPipelineModulo() {
    const char* Name = "Pipeline: 17 % 5";

    auto ResultOrErr = evalInt("17 % 5");
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

static bool testPipelineBoolLiteral() {
    const char* Name = "Pipeline: true";

    auto PlanResult = compileToPlan("true");
    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteBool(*PlanResult);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    bool Result = *ResultOrErr;
    if (Result != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBoolNegateTrue() {
    const char* Name = "Pipeline: ~true";

    auto PlanResult = compileToPlan("~true");
    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteBool(*PlanResult);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    bool Result = *ResultOrErr;
    if (Result != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBoolNegateFalse() {
    const char* Name = "Pipeline: ~false";

    auto PlanResult = compileToPlan("~false");
    if (!PlanResult) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << PlanResult.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    EmitterConfig Config;
    Config.EmitDebugInfo = false;
    Emitter E(Config);

    auto ResultOrErr = E.jitExecuteBool(*PlanResult);
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    bool Result = *ResultOrErr;
    if (Result != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Comparison Operator Tests
// ============================================================================

static bool testPipelineEqualTrue() {
    const char* Name = "Pipeline: 5 = 5";

    auto ResultOrErr = evalBool("5 = 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineEqualFalse() {
    const char* Name = "Pipeline: 5 = 3";

    auto ResultOrErr = evalBool("5 = 3");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineNotEqualTrue() {
    const char* Name = "Pipeline: 5 <> 3";

    auto ResultOrErr = evalBool("5 <> 3");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineNotEqualFalse() {
    const char* Name = "Pipeline: 5 <> 5";

    auto ResultOrErr = evalBool("5 <> 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLessThanTrue() {
    const char* Name = "Pipeline: 3 < 5";

    auto ResultOrErr = evalBool("3 < 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLessThanFalse() {
    const char* Name = "Pipeline: 5 < 3";

    auto ResultOrErr = evalBool("5 < 3");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineGreaterThanTrue() {
    const char* Name = "Pipeline: 5 > 3";

    auto ResultOrErr = evalBool("5 > 3");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineGreaterThanFalse() {
    const char* Name = "Pipeline: 3 > 5";

    auto ResultOrErr = evalBool("3 > 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLessOrEqualTrue() {
    const char* Name = "Pipeline: 5 <= 5";

    auto ResultOrErr = evalBool("5 <= 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLessOrEqualFalse() {
    const char* Name = "Pipeline: 5 <= 3";

    auto ResultOrErr = evalBool("5 <= 3");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineGreaterOrEqualTrue() {
    const char* Name = "Pipeline: 5 >= 5";

    auto ResultOrErr = evalBool("5 >= 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineGreaterOrEqualFalse() {
    const char* Name = "Pipeline: 3 >= 5";

    auto ResultOrErr = evalBool("3 >= 5");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Boolean Binary Operator Tests
// ============================================================================

static bool testPipelineBoolAndTrueTrue() {
    const char* Name = "Pipeline: true & true";

    auto ResultOrErr = evalBool("true & true");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBoolAndTrueFalse() {
    const char* Name = "Pipeline: true & false";

    auto ResultOrErr = evalBool("true & false");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBoolOrFalseFalse() {
    const char* Name = "Pipeline: false | false";

    auto ResultOrErr = evalBool("false | false");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != false) {
        fail(Name, "expected false, got true");
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBoolOrTrueFalse() {
    const char* Name = "Pipeline: true | false";

    auto ResultOrErr = evalBool("true | false");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != true) {
        fail(Name, "expected true, got false");
        return false;
    }

    pass(Name);
    return true;
}

// ============================================================================
// Bit Shift Operator Tests
// ============================================================================

static bool testPipelineLeftShift() {
    const char* Name = "Pipeline: 1 << 4";

    // 1 << 4 = 16
    auto ResultOrErr = evalInt("1 << 4");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 16) {
        std::string Msg = "expected 16, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineLeftShiftLarger() {
    const char* Name = "Pipeline: 5 << 3";

    // 5 << 3 = 40
    auto ResultOrErr = evalInt("5 << 3");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 40) {
        std::string Msg = "expected 40, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineRightShift() {
    const char* Name = "Pipeline: 16 >> 2";

    // 16 >> 2 = 4
    auto ResultOrErr = evalInt("16 >> 2");
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

static bool testPipelineRightShiftOdd() {
    const char* Name = "Pipeline: 100 >> 3";

    // 100 >> 3 = 12 (100 / 8 = 12.5, truncated)
    auto ResultOrErr = evalInt("100 >> 3");
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

// Mutable var bindings and assignment tests

static bool testPipelineVarSimple() {
    const char* Name = "Pipeline: var x = 5: x";

    auto ResultOrErr = evalInt("var x = 5: x");
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

static bool testPipelineVarAssign() {
    const char* Name = "Pipeline: var x = 5: set x : 10: x";

    auto ResultOrErr = evalInt("var x = 5: set x : 10: x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineVarIncrement() {
    const char* Name = "Pipeline: var x = 3: set x : x + 1: x";

    auto ResultOrErr = evalInt("var x = 3: set x : x + 1: x");
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

static bool testPipelineVarMultipleAssign() {
    const char* Name = "Pipeline: var a = 2: var b = 3: set a : a * b: a";

    auto ResultOrErr = evalInt("var a = 2: var b = 3: set a : a * b: a");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 6) {
        std::string Msg = "expected 6, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// While loop tests

static bool testPipelineWhileSimple() {
    const char* Name = "Pipeline: var x = 0: while x < 3 : set x : x + 1: x";

    auto ResultOrErr = evalInt("var x = 0: while x < 3 : set x : x + 1: x");
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

static bool testPipelineWhileSum() {
    const char* Name = "Pipeline: var sum = 0: var i = 1: while i <= 3 : { set sum : sum + i: set i : i + 1 }: sum";

    auto ResultOrErr = evalInt("var sum = 0: var i = 1: while i <= 3 : { set sum : sum + i: set i : i + 1 }: sum");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // sum = 1 + 2 + 3 = 6
    if (*ResultOrErr != 6) {
        std::string Msg = "expected 6, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineWhileZeroIterations() {
    const char* Name = "Pipeline: var x = 10: while x < 5 : set x : x + 1: x";

    auto ResultOrErr = evalInt("var x = 10: while x < 5 : set x : x + 1: x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Condition is false from start, x stays 10
    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineWhileFactorial() {
    const char* Name = "Pipeline: var result = 1: var n = 5: while n > 0 : { set result : result * n: set n : n - 1 }: result";

    auto ResultOrErr = evalInt("var result = 1: var n = 5: while n > 0 : { set result : result * n: set n : n - 1 }: result");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 5! = 120
    if (*ResultOrErr != 120) {
        std::string Msg = "expected 120, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// If statement tests

static bool testPipelineIfTrueExpr() {
    const char* Name = "Pipeline: if true : 1 else 2";

    auto ResultOrErr = evalInt("if true : 1 else 2");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 1) {
        std::string Msg = "expected 1, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineIfFalseExpr() {
    const char* Name = "Pipeline: if false : 1 else 2";

    auto ResultOrErr = evalInt("if false : 1 else 2");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 2) {
        std::string Msg = "expected 2, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineIfComparison() {
    const char* Name = "Pipeline: if 5 > 3 : 10 else 20";

    auto ResultOrErr = evalInt("if 5 > 3 : 10 else 20");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineIfWithVar() {
    const char* Name = "Pipeline: let x = 5: if x > 3 : x * 2 else x";

    auto ResultOrErr = evalInt("let x = 5: if x > 3 : x * 2 else x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineIfSideEffect() {
    const char* Name = "Pipeline: var x = 0: if true : set x : 5: x";

    auto ResultOrErr = evalInt("var x = 0: if true : set x : 5: x");
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

static bool testPipelineIfMax() {
    const char* Name = "Pipeline: let a = 7: let b = 3: if a > b : a else b";

    auto ResultOrErr = evalInt("let a = 7: let b = 3: if a > b : a else b");
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

// For loop tests

static bool testPipelineForSum() {
    const char* Name = "Pipeline: var sum = 0: for i in 5 : set sum : sum + i: sum";

    auto ResultOrErr = evalInt("var sum = 0: for i in 5 : set sum : sum + i: sum");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 0 + 1 + 2 + 3 + 4 = 10
    if (*ResultOrErr != 10) {
        std::string Msg = "expected 10, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineForCount() {
    const char* Name = "Pipeline: var count = 0: for i in 3 : set count : count + 1: count";

    auto ResultOrErr = evalInt("var count = 0: for i in 3 : set count : count + 1: count");
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

static bool testPipelineForZeroIterations() {
    const char* Name = "Pipeline: var x = 42: for i in 0 : set x : 0: x";

    auto ResultOrErr = evalInt("var x = 42: for i in 0 : set x : 0: x");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // No iterations, x stays 42
    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineForFactorial() {
    const char* Name = "Pipeline: var prod = 1: for i in 4 : set prod : prod * (i + 1): prod";

    auto ResultOrErr = evalInt("var prod = 1: for i in 4 : set prod : prod * (i + 1): prod");
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // 1 * 2 * 3 * 4 = 24 (using i+1 for 1,2,3,4)
    if (*ResultOrErr != 24) {
        std::string Msg = "expected 24, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

// Break and continue tests

static bool testPipelineBreakInFor() {
    const char* Name = "Pipeline: break in for loop";

    // Sum until we hit 3, then break
    // for i in 10: if i = 3 then break, else add i to sum
    // sum = 0 + 1 + 2 = 3
    auto ResultOrErr = evalInt(
        "var sum = 0\n"
        "for i in 10 : {\n"
        "    if i = 3 : break else set sum : sum + i\n"
        "}\n"
        "sum"
    );
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

static bool testPipelineBreakInWhile() {
    const char* Name = "Pipeline: break in while loop";

    // Count up but break when x reaches 5
    auto ResultOrErr = evalInt(
        "var x = 0\n"
        "while x < 100 : {\n"
        "    set x : x + 1\n"
        "    if x = 5 : break\n"
        "}\n"
        "x"
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

static bool testPipelineContinueInFor() {
    const char* Name = "Pipeline: continue in for loop";

    // Sum only even numbers (skip odds with continue)
    // for i in 6: if i % 2 = 1 then continue, else add i to sum
    // sum = 0 + 2 + 4 = 6
    auto ResultOrErr = evalInt(
        "var sum = 0\n"
        "for i in 6 : {\n"
        "    if i % 2 = 1 : continue else set sum : sum + i\n"
        "}\n"
        "sum"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 6) {
        std::string Msg = "expected 6, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineContinueInWhile() {
    const char* Name = "Pipeline: continue in while loop";

    // Sum only even numbers from 0-9 using while
    // sum = 0 + 2 + 4 + 6 + 8 = 20
    auto ResultOrErr = evalInt(
        "var sum = 0\n"
        "var i = 0\n"
        "while i < 10 : {\n"
        "    let current = i\n"
        "    set i : i + 1\n"
        "    if current % 2 = 1 : continue else set sum : sum + current\n"
        "}\n"
        "sum"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 20) {
        std::string Msg = "expected 20, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBreakInNestedLoops() {
    const char* Name = "Pipeline: break in nested loops";

    // Outer loop runs 3 times, inner loop breaks at j=2 each time
    // count increments for j=0 and j=1 in each outer iteration
    // count = 3 * 2 = 6
    auto ResultOrErr = evalInt(
        "var count = 0\n"
        "for i in 3 : {\n"
        "    for j in 10 : {\n"
        "        if j = 2 : break else set count : count + 1\n"
        "    }\n"
        "}\n"
        "count"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 6) {
        std::string Msg = "expected 6, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineContinueInNestedLoops() {
    const char* Name = "Pipeline: continue in nested loops";

    // Outer loop runs 3 times, inner loop skips odd j values
    // Inner loop: j = 0, 1, 2, 3, 4 - count increments for even j (0, 2, 4)
    // count = 3 * 3 = 9
    auto ResultOrErr = evalInt(
        "var count = 0\n"
        "for i in 3 : {\n"
        "    for j in 5 : {\n"
        "        if j % 2 = 1 : continue else set count : count + 1\n"
        "    }\n"
        "}\n"
        "count"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 9) {
        std::string Msg = "expected 9, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testPipelineBreakWhileInFor() {
    const char* Name = "Pipeline: break in while nested in for";

    // Outer for loop runs 3 times, inner while breaks at j=2 each time
    // count increments for j=0 and j=1 in each outer iteration
    // count = 3 * 2 = 6
    auto ResultOrErr = evalInt(
        "var count = 0\n"
        "for i in 3 : {\n"
        "    var j = 0\n"
        "    while j < 10 : {\n"
        "        if j = 2 : break else set count : count + 1\n"
        "        set j : j + 1\n"
        "    }\n"
        "}\n"
        "count"
    );
    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    if (*ResultOrErr != 6) {
        std::string Msg = "expected 6, got " + std::to_string(*ResultOrErr);
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

// ============================================================================
// Variant Construction Tests
// ============================================================================

static bool testVariantConstructionOk() {
    const char* Name = "Pipeline: variant construction Ok";

    // Construct Result.Ok(42) and extract value via choose
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "choose Result.Ok(42): when x: Ok: x else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Should get 42 from the Ok variant
    if (*ResultOrErr != 42) {
        std::string Msg = "expected 42, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testVariantConstructionError() {
    const char* Name = "Pipeline: variant construction Error";

    // Construct Result.Error(100) and extract value via choose
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "choose Result.Error(100): when e: Error: e else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Should get 100 from the Error variant
    if (*ResultOrErr != 100) {
        std::string Msg = "expected 100, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testVariantConstructionWithIs() {
    const char* Name = "Pipeline: variant construction with is";

    // Construct Result.Ok(50) and test with 'is Ok'
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "if Result.Ok(50) is Ok : 1 else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Should be true (1) since it's an Ok variant
    if (*ResultOrErr != 1) {
        std::string Msg = "expected 1, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

static bool testVariantConstructionIsNotMatch() {
    const char* Name = "Pipeline: variant construction is not match";

    // Construct Result.Error(33) and test with 'is Ok' (should not match)
    auto ResultOrErr = evalInt(
        "define Result union: (Ok: int, Error: int)\n"
        "if Result.Error(33) is Ok : 1 else 0"
    );

    if (!ResultOrErr) {
        std::string ErrMsg;
        llvm::raw_string_ostream OS(ErrMsg);
        OS << ResultOrErr.takeError();
        fail(Name, ErrMsg.c_str());
        return false;
    }

    // Should be false (0) since it's an Error variant, not Ok
    if (*ResultOrErr != 0) {
        std::string Msg = "expected 0, got " + std::to_string(*ResultOrErr);
        fail(Name, Msg.c_str());
        return false;
    }

    pass(Name);
    return true;
}

bool runEmitterTests() {
    llvm::outs() << "Running Emitter tests...\n";
    llvm::outs().flush();

    TestsPassed = 0;
    TestsFailed = 0;

    // Direct Plan -> JIT tests
    llvm::outs() << "  Direct JIT tests:\n";
    llvm::outs().flush();
    testJitIntegerConstant();
    testJitBooleanTrue();
    testJitBooleanFalse();
    testJitNegativeInteger();
    testJitAddition();
    testJitSubtraction();
    testJitMultiplication();
    testJitDivision();
    testJitModulo();

    // Full pipeline tests (Source -> Parse -> Model -> Plan -> JIT)
    llvm::outs() << "  Full pipeline tests:\n";
    testPipelineIntegerLiteral();
    testPipelineNegativeLiteral();
    testPipelineAddition();
    testPipelineSubtraction();
    testPipelineMultiplication();
    testPipelineDivision();
    testPipelineModulo();
    testPipelineParentheses();
    testPipelineBoolLiteral();
    testPipelineBoolNegateTrue();
    testPipelineBoolNegateFalse();

    // Comparison operators
    llvm::outs() << "  Comparison operator tests:\n";
    testPipelineEqualTrue();
    testPipelineEqualFalse();
    testPipelineNotEqualTrue();
    testPipelineNotEqualFalse();
    testPipelineLessThanTrue();
    testPipelineLessThanFalse();
    testPipelineGreaterThanTrue();
    testPipelineGreaterThanFalse();
    testPipelineLessOrEqualTrue();
    testPipelineLessOrEqualFalse();
    testPipelineGreaterOrEqualTrue();
    testPipelineGreaterOrEqualFalse();

    // Boolean binary operators
    llvm::outs() << "  Boolean binary operator tests:\n";
    testPipelineBoolAndTrueTrue();
    testPipelineBoolAndTrueFalse();
    testPipelineBoolOrFalseFalse();
    testPipelineBoolOrTrueFalse();

    // Bit shift operators
    llvm::outs() << "  Bit shift operator tests:\n";
    testPipelineLeftShift();
    testPipelineLeftShiftLarger();
    testPipelineRightShift();
    testPipelineRightShiftOdd();

    // Variable bindings (immutable let)
    llvm::outs() << "  Variable binding tests:\n";
    testPipelineLetSimple();
    testPipelineLetWithOp();
    testPipelineLetNested();
    testPipelineLetMultiply();

    // Mutable var bindings and assignment
    llvm::outs() << "  Mutable var and assignment tests:\n";
    testPipelineVarSimple();
    testPipelineVarAssign();
    testPipelineVarIncrement();
    testPipelineVarMultipleAssign();

    // While loops
    llvm::outs() << "  While loop tests:\n";
    testPipelineWhileSimple();
    testPipelineWhileSum();
    testPipelineWhileZeroIterations();
    testPipelineWhileFactorial();

    // If statements
    llvm::outs() << "  If statement tests:\n";
    testPipelineIfTrueExpr();
    testPipelineIfFalseExpr();
    testPipelineIfComparison();
    testPipelineIfWithVar();
    testPipelineIfSideEffect();
    testPipelineIfMax();

    // For loops
    llvm::outs() << "  For loop tests:\n";
    testPipelineForSum();
    testPipelineForCount();
    testPipelineForZeroIterations();
    testPipelineForFactorial();

    // Break and continue
    llvm::outs() << "  Break and continue tests:\n";
    testPipelineBreakInFor();
    testPipelineBreakInWhile();
    testPipelineContinueInFor();
    testPipelineContinueInWhile();
    testPipelineBreakInNestedLoops();
    testPipelineContinueInNestedLoops();
    testPipelineBreakWhileInFor();

    // Match expressions
    llvm::outs() << "  Match expression tests:\n";
    testPipelineMatchSingleCase();
    testPipelineMatchElse();
    testPipelineMatchWithVar();
    testPipelineMatchFirstCase();

    // Try expressions
    llvm::outs() << "  Try expression tests:\n";
    testPipelineTrySimpleValue();
    testPipelineTryWithBinding();
    testPipelineTryExpression();

    // Throw statements
    llvm::outs() << "  Throw statement tests:\n";
    testThrowInFunction();

    // Choose expressions
    llvm::outs() << "  Choose expression tests:\n";
    testChooseMatchesError();
    testChooseWithDataBinding();
    testChooseElseFallback();
    testChooseMultipleCases();

    // Is expressions
    llvm::outs() << "  Is expression tests:\n";
    testIsExpressionMatchesError();
    testIsExpressionNoMatchOk();
    testIsExpressionWithIf();
    testIsExpressionNotMatching();

    // Variant construction
    llvm::outs() << "  Variant construction tests:\n";
    testVariantConstructionOk();
    testVariantConstructionError();
    testVariantConstructionWithIs();
    testVariantConstructionIsNotMatch();

    // SizeOf expressions
    llvm::outs() << "  SizeOf expression tests:\n";
    testPipelineSizeOfInt();
    testPipelineSizeOfBool();
    testPipelineSizeOfI32();
    testPipelineSizeOfPtr();

    llvm::outs() << "  Grouped expression tests:\n";
    testPipelineGroupedSimple();
    testPipelineGroupedAdd();
    testPipelineNestedGrouped();
    testPipelineComplexGrouped();

    llvm::outs() << "  Pointer dereference tests:\n";
    testPointerDerefCompiles();
    testPointerDerefSimple();
    testPointerDerefMemberAccess();

    llvm::outs() << "  Member access tests:\n";
    testPipelineMemberAccessSimple();
    testPipelineMemberAccessSecondField();
    testPipelineMemberAccessVariableOnly();
    testPipelineMemberAccessWithBinding();
    testPipelineMemberAccessMultiply();

    llvm::outs() << "  Method call tests:\n";
    testPipelineMethodCallSimple();
    testPipelineMethodCallSum();
    testPipelineMethodCallWithArgs();

    llvm::outs() << "  Initializer tests:\n";
    testPipelineInitializerSimple();
    testPipelineInitializerWithParams();
    testPipelineInitializerWithComputation();

    llvm::outs() << "  Deinitializer tests:\n";
    testPipelineDeinitializerSimple();
    testPipelineDeinitializerWithInit();

    llvm::outs() << "  Function call tests:\n";
    testPipelineFunctionCallSimple();
    testPipelineFunctionCallWithArgs();
    testPipelineFunctionCallChained();
    testPipelineFunctionCallWithExpression();

    llvm::outs() << "\nEmitter tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
