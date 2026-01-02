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
