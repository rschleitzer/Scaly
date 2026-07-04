#include "LexerTests.h"
#include "Lexer.h"
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

static bool testEmptyInput() {
    const char* Name = "empty input";
    Lexer L("");

    if (!is<EmptyToken>(L.token())) {
        fail(Name, "expected EmptyToken initially");
        return false;
    }

    L.advance();

    if (!is<EmptyToken>(L.token())) {
        fail(Name, "expected EmptyToken after advance on empty input");
        return false;
    }

    pass(Name);
    return true;
}

static bool testComprehensiveTokenization() {
    const char* Name = "comprehensive tokenization";

    const char* Input =
        " \t\r\n"
        ";single line comment\n"
        ";*multi\n"
        "line\n"
        "comment;*nested comment*;\n"
        "comment end*;\n"
        "abc_AZ0815_7 42\n"
        ": 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1 "
        "@ttribute + -0815 /* <> \"a string\" \"\\\"\\n\\r\\t\" "
        "'a string identifier' `a string fragment \\`\\n\\r\\t`";

    Lexer L(Input);

    // Initially empty
    if (!is<EmptyToken>(L.token())) {
        fail(Name, "expected EmptyToken initially");
        return false;
    }
    L.advance();

    // Identifier: abc_AZ0815_7
    if (auto* Id = get_if<IdentifierToken>(L.token())) {
        if (Id->Name != "abc_AZ0815_7") {
            fail(Name, "expected identifier 'abc_AZ0815_7'");
            return false;
        }
    } else {
        fail(Name, "expected IdentifierToken for 'abc_AZ0815_7'");
        return false;
    }
    L.advance();

    // Integer: 42
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Int = std::get_if<IntegerLiteral>(&Lit->Value)) {
            if (Int->Value != "42") {
                fail(Name, "expected integer '42'");
                return false;
            }
        } else {
            fail(Name, "expected IntegerLiteral for '42'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '42'");
        return false;
    }
    L.advance();

    // Colon (from newline)
    if (!is<ColonToken>(L.token())) {
        fail(Name, "expected ColonToken from newline");
        return false;
    }
    L.advance();

    // Colon (explicit :)
    if (!is<ColonToken>(L.token())) {
        fail(Name, "expected explicit ColonToken");
        return false;
    }
    L.advance();

    // Integer: 0
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Int = std::get_if<IntegerLiteral>(&Lit->Value)) {
            if (Int->Value != "0") {
                fail(Name, "expected integer '0'");
                return false;
            }
        } else {
            fail(Name, "expected IntegerLiteral for '0'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0'");
        return false;
    }
    L.advance();

    // Integer: 012
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Int = std::get_if<IntegerLiteral>(&Lit->Value)) {
            if (Int->Value != "012") {
                fail(Name, "expected integer '012'");
                return false;
            }
        } else {
            fail(Name, "expected IntegerLiteral for '012'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '012'");
        return false;
    }
    L.advance();

    // Float: 0.34
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Fp = std::get_if<FloatingPointLiteral>(&Lit->Value)) {
            if (Fp->Value != "0.34") {
                fail(Name, "expected float '0.34'");
                return false;
            }
        } else {
            fail(Name, "expected FloatingPointLiteral for '0.34'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0.34'");
        return false;
    }
    L.advance();

    // Float: 0.56E12
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Fp = std::get_if<FloatingPointLiteral>(&Lit->Value)) {
            if (Fp->Value != "0.56E12") {
                fail(Name, "expected float '0.56E12'");
                return false;
            }
        } else {
            fail(Name, "expected FloatingPointLiteral for '0.56E12'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0.56E12'");
        return false;
    }
    L.advance();

    // Float: 0.78e13
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Fp = std::get_if<FloatingPointLiteral>(&Lit->Value)) {
            if (Fp->Value != "0.78e13") {
                fail(Name, "expected float '0.78e13'");
                return false;
            }
        } else {
            fail(Name, "expected FloatingPointLiteral for '0.78e13'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0.78e13'");
        return false;
    }
    L.advance();

    // Hex: 0xaB
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Hex = std::get_if<HexLiteral>(&Lit->Value)) {
            if (Hex->Value != "0xaB") {
                fail(Name, "expected hex '0xaB'");
                return false;
            }
        } else {
            fail(Name, "expected HexLiteral for '0xaB'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0xaB'");
        return false;
    }
    L.advance();

    // Hex: 0xCdEf02
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Hex = std::get_if<HexLiteral>(&Lit->Value)) {
            if (Hex->Value != "0xCdEf02") {
                fail(Name, "expected hex '0xCdEf02'");
                return false;
            }
        } else {
            fail(Name, "expected HexLiteral for '0xCdEf02'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0xCdEf02'");
        return false;
    }
    L.advance();

    // Boolean: false (0B0)
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Bool = std::get_if<BooleanLiteral>(&Lit->Value)) {
            if (Bool->Value != false) {
                fail(Name, "expected boolean false");
                return false;
            }
        } else {
            fail(Name, "expected BooleanLiteral for '0B0'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0B0'");
        return false;
    }
    L.advance();

    // Boolean: true (0B1)
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Bool = std::get_if<BooleanLiteral>(&Lit->Value)) {
            if (Bool->Value != true) {
                fail(Name, "expected boolean true");
                return false;
            }
        } else {
            fail(Name, "expected BooleanLiteral for '0B1'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0B1'");
        return false;
    }
    L.advance();

    // Attribute: ttribute (from @ttribute)
    if (auto* Attr = get_if<AttributeToken>(L.token())) {
        if (Attr->Name != "ttribute") {
            fail(Name, "expected attribute 'ttribute'");
            return false;
        }
    } else {
        fail(Name, "expected AttributeToken for '@ttribute'");
        return false;
    }
    L.advance();

    // Operator: +
    if (auto* Id = get_if<IdentifierToken>(L.token())) {
        if (Id->Name != "+") {
            fail(Name, "expected operator '+'");
            return false;
        }
    } else {
        fail(Name, "expected IdentifierToken for '+'");
        return false;
    }
    L.advance();

    // Operator: -
    if (auto* Id = get_if<IdentifierToken>(L.token())) {
        if (Id->Name != "-") {
            fail(Name, "expected operator '-'");
            return false;
        }
    } else {
        fail(Name, "expected IdentifierToken for '-'");
        return false;
    }
    L.advance();

    // Integer: 0815
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Int = std::get_if<IntegerLiteral>(&Lit->Value)) {
            if (Int->Value != "0815") {
                fail(Name, "expected integer '0815'");
                return false;
            }
        } else {
            fail(Name, "expected IntegerLiteral for '0815'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '0815'");
        return false;
    }
    L.advance();

    // Operator: /*
    if (auto* Id = get_if<IdentifierToken>(L.token())) {
        if (Id->Name != "/*") {
            fail(Name, "expected operator '/*'");
            return false;
        }
    } else {
        fail(Name, "expected IdentifierToken for '/*'");
        return false;
    }
    L.advance();

    // Operator: <>
    if (auto* Id = get_if<IdentifierToken>(L.token())) {
        if (Id->Name != "<>") {
            fail(Name, "expected operator '<>'");
            return false;
        }
    } else {
        fail(Name, "expected IdentifierToken for '<>'");
        return false;
    }
    L.advance();

    // String: "a string"
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Str = std::get_if<StringLiteral>(&Lit->Value)) {
            if (Str->Value != "a string") {
                fail(Name, "expected string 'a string'");
                return false;
            }
        } else {
            fail(Name, "expected StringLiteral for '\"a string\"'");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for '\"a string\"'");
        return false;
    }
    L.advance();

    // String with escapes: "\"\n\r\t"
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Str = std::get_if<StringLiteral>(&Lit->Value)) {
            // The lexer stores the raw escape sequences, not interpreted
            if (Str->Value != "\\\"\\n\\r\\t") {
                fail(Name, "expected string with escapes");
                return false;
            }
        } else {
            fail(Name, "expected StringLiteral for escaped string");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for escaped string");
        return false;
    }
    L.advance();

    // Freeform identifier: 'a string identifier'
    if (auto* Id = get_if<IdentifierToken>(L.token())) {
        if (Id->Name != "a string identifier") {
            fail(Name, "expected identifier 'a string identifier'");
            return false;
        }
    } else {
        fail(Name, "expected IdentifierToken for single-quoted identifier");
        return false;
    }
    L.advance();

    // Fragment literal: `a string fragment \`\n\r\t`
    if (auto* Lit = get_if<LiteralToken>(L.token())) {
        if (auto* Frag = std::get_if<FragmentLiteral>(&Lit->Value)) {
            // The lexer stores the raw escape sequences
            if (Frag->Value != "a string fragment \\`\\n\\r\\t") {
                fail(Name, "expected fragment literal");
                return false;
            }
        } else {
            fail(Name, "expected FragmentLiteral");
            return false;
        }
    } else {
        fail(Name, "expected LiteralToken for fragment literal");
        return false;
    }

    pass(Name);
    return true;
}

bool runLexerTests() {
    llvm::outs() << "Running Lexer tests...\n";

    TestsPassed = 0;
    TestsFailed = 0;

    testEmptyInput();
    testComprehensiveTokenization();

    llvm::outs() << "\nLexer tests: " << TestsPassed << " passed, "
                 << TestsFailed << " failed\n";

    return TestsFailed == 0;
}

} // namespace scaly
