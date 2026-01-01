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

bool runEmitterTests() {
    llvm::outs() << "Running Emitter tests...\n";

    TestsPassed = 0;
    TestsFailed = 0;

    // Direct Plan -> JIT tests
    llvm::outs() << "  Direct JIT tests:\n";
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

    llvm::outs() << "\nEmitter tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
