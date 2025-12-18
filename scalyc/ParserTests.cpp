#include "ParserTests.h"
#include "Parser.h"
#include <iostream>

namespace scaly {

namespace {

struct TestCase {
    const char* Name;
    const char* Input;
    bool ShouldSucceed;
};

bool runTest(const TestCase& Test) {
    Parser P(Test.Input);
    auto Result = P.parseProgram();

    if (!Result) {
        llvm::consumeError(Result.takeError());
        return !Test.ShouldSucceed;  // Fail expected
    }

    return Test.ShouldSucceed;  // Success expected
}

} // anonymous namespace

bool runParserTests() {
    std::cout << "Running Parser tests..." << std::endl;

    int Passed = 0;
    int Failed = 0;

    // Expression tests from tests/expressions.sgm
    std::cout << "  Literals" << std::endl;
    {
        TestCase Tests[] = {
            {"bool-true", "true", true},
            {"bool-false", "false", true},
            {"int-positive", "42", true},
            {"int-zero", "0", true},
            {"int-negative", "-7", true},
            {"hex-ff", "0xFF", true},
            {"hex-1a", "0x1A", true},
            {"float-pi", "3.14", true},
            {"float-two", "2.0", true},
            {"string-hello", R"("hello")", true},
            {"string-empty", R"("")", true},
            {"char-a", "`a`", true},
            {"char-z", "`Z`", true},
        };

        for (const auto& T : Tests) {
            if (runTest(T)) {
                std::cout << "    PASS: " << T.Name << std::endl;
                Passed++;
            } else {
                std::cout << "    FAIL: " << T.Name << std::endl;
                Failed++;
            }
        }
    }

    std::cout << "  Operations" << std::endl;
    {
        TestCase Tests[] = {
            {"simple-add", "3 + 4", true},
            {"simple-subtract", "10 - 3", true},
            {"simple-multiply", "6 * 7", true},
            {"chain-ops", "3 + 4 * 2", true},
            {"parens-grouping", "2 * (3 + 4)", true},
            {"nested-parens", "(2 + 3) * (4 + 1)", true},
        };

        for (const auto& T : Tests) {
            if (runTest(T)) {
                std::cout << "    PASS: " << T.Name << std::endl;
                Passed++;
            } else {
                std::cout << "    FAIL: " << T.Name << std::endl;
                Failed++;
            }
        }
    }

    std::cout << "  Bindings" << std::endl;
    {
        TestCase Tests[] = {
            {"let-simple", "let x 5\nx", true},
            {"let-use-binding", "let x 5\nx + 3", true},
            {"let-nested", "let x 5\nlet y 3\nx + y", true},
            {"let-expr-value", "let x 2 + 3\nx * 2", true},
        };

        for (const auto& T : Tests) {
            if (runTest(T)) {
                std::cout << "    PASS: " << T.Name << std::endl;
                Passed++;
            } else {
                std::cout << "    FAIL: " << T.Name << std::endl;
                Failed++;
            }
        }
    }

    // Definition tests from tests/definitions.sgm
    std::cout << "  Definitions" << std::endl;
    {
        TestCase Tests[] = {
            {"define-point", "define Point(x: int, y: int)\nPoint(3, 4)", true},
            {"define-field-access", "define Point(x: int, y: int)\nPoint(3, 4).x", true},
            {"define-with-let", "define Point(x: int, y: int)\nlet p Point(3, 4)\np.x + p.y", true},
            {"define-union", "define Option union(Some: int, None)\nSome(5)", true},
            {"define-result", "define Result union(Ok: int, Err: String)\nOk(42)", true},
        };

        for (const auto& T : Tests) {
            if (runTest(T)) {
                std::cout << "    PASS: " << T.Name << std::endl;
                Passed++;
            } else {
                std::cout << "    FAIL: " << T.Name << std::endl;
                Failed++;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "Parser tests: " << Passed << " passed, "
              << Failed << " failed" << std::endl;

    return Failed == 0;
}

} // namespace scaly
