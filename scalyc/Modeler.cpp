// Modeler.cpp - Syntax to semantic model transformation
#include "Modeler.h"
#include "Parser.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <cerrno>
#include <cstdlib>

namespace scaly {

// Process escape sequences in a string literal
// Converts \n, \r, \t, \\, \", \', \0, and \uXXXX to their byte values
static std::string processEscapes(llvm::StringRef Input) {
    std::string Result;
    Result.reserve(Input.size());

    for (size_t I = 0; I < Input.size(); ++I) {
        if (Input[I] != '\\') {
            Result.push_back(Input[I]);
            continue;
        }

        // Escape sequence - look at next character
        if (I + 1 >= Input.size()) {
            // Trailing backslash - keep it (shouldn't happen if lexer is correct)
            Result.push_back('\\');
            continue;
        }

        char Next = Input[I + 1];
        switch (Next) {
        case 'n':  Result.push_back('\n'); ++I; break;
        case 'r':  Result.push_back('\r'); ++I; break;
        case 't':  Result.push_back('\t'); ++I; break;
        case '\\': Result.push_back('\\'); ++I; break;
        case '"':  Result.push_back('"');  ++I; break;
        case '\'': Result.push_back('\''); ++I; break;
        case '0':  Result.push_back('\0'); ++I; break;
        case 'u': {
            // Unicode escape: \uXXXX (4 hex digits)
            if (I + 5 >= Input.size()) {
                // Not enough characters - keep raw
                Result.push_back('\\');
                break;
            }

            // Parse 4 hex digits
            llvm::StringRef Hex = Input.substr(I + 2, 4);
            unsigned CodePoint = 0;
            bool Valid = true;
            for (char C : Hex) {
                CodePoint <<= 4;
                if (C >= '0' && C <= '9') {
                    CodePoint |= (C - '0');
                } else if (C >= 'a' && C <= 'f') {
                    CodePoint |= (C - 'a' + 10);
                } else if (C >= 'A' && C <= 'F') {
                    CodePoint |= (C - 'A' + 10);
                } else {
                    Valid = false;
                    break;
                }
            }

            if (!Valid) {
                // Invalid hex - keep raw
                Result.push_back('\\');
                break;
            }

            // Encode code point as UTF-8
            if (CodePoint < 0x80) {
                Result.push_back(static_cast<char>(CodePoint));
            } else if (CodePoint < 0x800) {
                Result.push_back(static_cast<char>(0xC0 | (CodePoint >> 6)));
                Result.push_back(static_cast<char>(0x80 | (CodePoint & 0x3F)));
            } else {
                Result.push_back(static_cast<char>(0xE0 | (CodePoint >> 12)));
                Result.push_back(static_cast<char>(0x80 | ((CodePoint >> 6) & 0x3F)));
                Result.push_back(static_cast<char>(0x80 | (CodePoint & 0x3F)));
            }
            I += 5; // Skip \uXXXX
            break;
        }
        default:
            // Unknown escape - keep the backslash and character as-is
            Result.push_back('\\');
            break;
        }
    }

    return Result;
}

Modeler::Modeler(llvm::StringRef FileName) : File(FileName.str()) {}

void Modeler::addPackageSearchPath(llvm::StringRef Path) {
    PackageSearchPaths.push_back(Path.str());
}

// Name to Type conversion (for expressions - no generics attached)
Type Modeler::nameToType(const NameSyntax &Syntax) {
    std::vector<std::string> Path;
    Path.push_back(std::string(Syntax.name));

    if (Syntax.extensions) {
        for (const auto &Ext : *Syntax.extensions) {
            Path.push_back(std::string(Ext.name));
        }
    }

    return Type{
        Span{Syntax.Start, Syntax.End},
        std::move(Path),
        nullptr,  // No generics
        UnspecifiedLifetime{}
    };
}

// Type handling

llvm::Expected<std::unique_ptr<Type>> Modeler::handleType(
    const TypeSyntax &Syntax) {

    std::vector<std::string> Path;
    Path.push_back(std::string(Syntax.name.name));

    if (Syntax.name.extensions) {
        for (const auto &Ext : *Syntax.name.extensions) {
            Path.push_back(std::string(Ext.name));
        }
    }

    std::unique_ptr<std::vector<Type>> Generics;
    if (Syntax.generics && Syntax.generics->generics) {
        Generics = std::make_unique<std::vector<Type>>();
        for (const auto &GA : *Syntax.generics->generics) {
            auto TypeResult = handleType(GA.type);
            if (!TypeResult)
                return TypeResult.takeError();
            Generics->push_back(std::move(**TypeResult));
        }
    }

    Lifetime Life = handleLifetime(Syntax.lifetime);

    return std::make_unique<Type>(Type{
        Span{Syntax.Start, Syntax.End},
        std::move(Path),
        std::move(Generics),
        std::move(Life)
    });
}

Lifetime Modeler::handleLifetime(const LifetimeSyntax *Syntax) {
    if (!Syntax)
        return UnspecifiedLifetime{};

    return std::visit([](const auto &L) -> Lifetime {
        using T = std::decay_t<decltype(L)>;
        if constexpr (std::is_same_v<T, CallSyntax>) {
            return CallLifetime{Span{L.Start, L.End}};
        } else if constexpr (std::is_same_v<T, LocalSyntax>) {
            return LocalLifetime{Span{L.Start, L.End}};
        } else if constexpr (std::is_same_v<T, ReferenceSyntax>) {
            return ReferenceLifetime{Span{L.Start, L.End}, std::string(L.location)};
        } else if constexpr (std::is_same_v<T, ThrownSyntax>) {
            return ThrownLifetime{Span{L.Start, L.End}};
        }
        return UnspecifiedLifetime{};
    }, Syntax->Value);
}

// Literal handling

llvm::Expected<Constant> Modeler::handleLiteral(const LiteralSyntax &Syntax) {
    Span Loc{Syntax.Start, Syntax.End};

    return std::visit([&](const auto &Lit) -> llvm::Expected<Constant> {
        using T = std::decay_t<decltype(Lit)>;
        if constexpr (std::is_same_v<T, BooleanLiteral>) {
            return BooleanConstant{Loc, Lit.Value};
        } else if constexpr (std::is_same_v<T, IntegerLiteral>) {
            char *EndPtr;
            errno = 0;
            int64_t Value = std::strtoll(Lit.Value.str().c_str(), &EndPtr, 10);
            if (errno == ERANGE)
                return makeInvalidConstantError(File, Loc);
            return IntegerConstant{Loc, Value};
        } else if constexpr (std::is_same_v<T, HexLiteral>) {
            char *EndPtr;
            errno = 0;
            uint64_t Value = std::strtoull(Lit.Value.str().c_str(), &EndPtr, 16);
            if (errno == ERANGE)
                return makeInvalidConstantError(File, Loc);
            return HexConstant{Loc, Value};
        } else if constexpr (std::is_same_v<T, FloatingPointLiteral>) {
            char *EndPtr;
            errno = 0;
            double Value = std::strtod(Lit.Value.str().c_str(), &EndPtr);
            if (errno == ERANGE)
                return makeInvalidConstantError(File, Loc);
            return FloatingPointConstant{Loc, Value};
        } else if constexpr (std::is_same_v<T, StringLiteral>) {
            return StringConstant{Loc, processEscapes(Lit.Value)};
        } else if constexpr (std::is_same_v<T, CharacterLiteral>) {
            return CharacterConstant{Loc, processEscapes(Lit.Value)};
        } else if constexpr (std::is_same_v<T, FragmentLiteral>) {
            return FragmentConstant{Loc, Lit.Value.str()};
        }
        return makeInvalidConstantError(File, Loc);
    }, Syntax.literal);
}

// Operand handling

llvm::Expected<std::vector<Operand>> Modeler::handleOperands(
    const std::vector<OperandSyntax> *Syntax) {
    std::vector<Operand> Result;
    if (!Syntax)
        return Result;

    for (const auto &OS : *Syntax) {
        auto Op = handleOperand(OS);
        if (!Op)
            return Op.takeError();
        Result.push_back(std::move(*Op));
    }
    return Result;
}

llvm::Expected<Operand> Modeler::handleOperand(const OperandSyntax &Syntax) {
    auto Expr = handleExpression(Syntax.expression);
    if (!Expr)
        return Expr.takeError();

    std::unique_ptr<std::vector<Type>> MemberAccess;
    if (Syntax.members) {
        MemberAccess = std::make_unique<std::vector<Type>>();
        for (const auto &M : *Syntax.members) {
            MemberAccess->push_back(nameToType(M.name));
        }
    }

    return Operand{
        Span{Syntax.Start, Syntax.End},
        std::move(*Expr),
        std::move(MemberAccess)
    };
}

llvm::Expected<std::vector<Operand>> Modeler::handleOperation(
    const OperationSyntax &Syntax) {
    return handleOperands(Syntax.operands);
}

// Expression handling

llvm::Expected<Expression> Modeler::handleExpression(
    const ExpressionSyntax &Syntax) {

    return std::visit([&](const auto &E) -> llvm::Expected<Expression> {
        using T = std::decay_t<decltype(E)>;

        if constexpr (std::is_same_v<T, LiteralSyntax>) {
            auto Const = handleLiteral(E);
            if (!Const)
                return Const.takeError();
            return *Const;
        } else if constexpr (std::is_same_v<T, NameSyntax>) {
            return nameToType(E);
        } else if constexpr (std::is_same_v<T, LifetimeSyntax>) {
            // Lifetime as standalone expression (for generic instantiation)
            // Convert to a Type with just the lifetime marker
            Lifetime Life = handleLifetime(&E);
            // Get span from the inner variant
            Span LifeSpan = std::visit([](const auto &L) -> Span {
                return Span{L.Start, L.End};
            }, E.Value);
            return Type{
                LifeSpan,
                {},  // Empty name - just lifetime marker
                nullptr,
                std::move(Life)
            };
        } else if constexpr (std::is_same_v<T, ObjectSyntax>) {
            auto Obj = handleObject(E);
            if (!Obj)
                return Obj.takeError();
            return std::move(*Obj);
        } else if constexpr (std::is_same_v<T, VectorSyntax>) {
            auto Vec = handleVector(E);
            if (!Vec)
                return Vec.takeError();
            return std::move(*Vec);
        } else if constexpr (std::is_same_v<T, BlockSyntax>) {
            auto Blk = handleBlock(E);
            if (!Blk)
                return Blk.takeError();
            return std::move(*Blk);
        } else if constexpr (std::is_same_v<T, IfSyntax>) {
            auto IfResult = handleIf(E);
            if (!IfResult)
                return IfResult.takeError();
            return std::move(*IfResult);
        } else if constexpr (std::is_same_v<T, MatchSyntax>) {
            auto MatchResult = handleMatch(E);
            if (!MatchResult)
                return MatchResult.takeError();
            return std::move(*MatchResult);
        } else if constexpr (std::is_same_v<T, ChooseSyntax>) {
            auto ChooseResult = handleChoose(E);
            if (!ChooseResult)
                return ChooseResult.takeError();
            return std::move(*ChooseResult);
        } else if constexpr (std::is_same_v<T, ForSyntax>) {
            auto ForResult = handleFor(E);
            if (!ForResult)
                return ForResult.takeError();
            return std::move(*ForResult);
        } else if constexpr (std::is_same_v<T, WhileSyntax>) {
            auto WhileResult = handleWhile(E);
            if (!WhileResult)
                return WhileResult.takeError();
            return std::move(*WhileResult);
        } else if constexpr (std::is_same_v<T, TrySyntax>) {
            auto TryResult = handleTry(E);
            if (!TryResult)
                return TryResult.takeError();
            return std::move(*TryResult);
        } else if constexpr (std::is_same_v<T, SizeOfSyntax>) {
            auto SizeResult = handleSizeOf(E);
            if (!SizeResult)
                return SizeResult.takeError();
            return std::move(*SizeResult);
        } else if constexpr (std::is_same_v<T, AlignOfSyntax>) {
            auto AlignResult = handleAlignOf(E);
            if (!AlignResult)
                return AlignResult.takeError();
            return std::move(*AlignResult);
        } else if constexpr (std::is_same_v<T, IsSyntax>) {
            auto IsResult = handleIs(E);
            if (!IsResult)
                return IsResult.takeError();
            return std::move(*IsResult);
        } else if constexpr (std::is_same_v<T, AsSyntax>) {
            auto AsResult = handleAs(E);
            if (!AsResult)
                return AsResult.takeError();
            return std::move(*AsResult);
        } else if constexpr (std::is_same_v<T, LambdaSyntax>) {
            return makeNotImplementedError(File, "Lambda",
                                           Span{E.Start, E.End});
        } else if constexpr (std::is_same_v<T, RepeatSyntax>) {
            return makeNotImplementedError(File, "Repeat",
                                           Span{E.Start, E.End});
        }
        return makeNotImplementedError(File, "Unknown expression", Span{0, 0});
    }, Syntax.Value);
}

// Object/Tuple handling

llvm::Expected<Tuple> Modeler::handleObject(const ObjectSyntax &Syntax) {
    std::vector<Component> Components;
    if (Syntax.components) {
        for (const auto &C : *Syntax.components) {
            auto Comp = handleComponent(C);
            if (!Comp)
                return Comp.takeError();
            Components.push_back(std::move(*Comp));
        }
    }
    return Tuple{Span{Syntax.Start, Syntax.End}, std::move(Components)};
}

llvm::Expected<Component> Modeler::handleComponent(
    const ComponentSyntax &Syntax) {

    std::unique_ptr<std::string> Name;
    std::vector<Operand> Value;

    if (Syntax.value) {
        // Named component
        if (Syntax.operands && !Syntax.operands->empty()) {
            const auto &NameOp = (*Syntax.operands)[0];
            if (auto *NameExpr = std::get_if<NameSyntax>(&NameOp.expression.Value)) {
                Name = std::make_unique<std::string>(NameExpr->name.str());
            }
        }
        if (Syntax.value->value) {
            auto Ops = handleOperands(Syntax.value->value);
            if (!Ops)
                return Ops.takeError();
            Value = std::move(*Ops);
        }
    } else {
        // Unnamed component
        auto Ops = handleOperands(Syntax.operands);
        if (!Ops)
            return Ops.takeError();
        Value = std::move(*Ops);
    }

    std::vector<Attribute> Attributes;
    if (Syntax.attributes) {
        for (const auto &A : *Syntax.attributes) {
            auto Attr = handleAttribute(A);
            if (!Attr)
                return Attr.takeError();
            Attributes.push_back(std::move(*Attr));
        }
    }

    return Component{
        Span{Syntax.Start, Syntax.End},
        std::move(Name),
        std::move(Value),
        std::move(Attributes)
    };
}

llvm::Expected<Matrix> Modeler::handleVector(const VectorSyntax &Syntax) {
    std::vector<std::vector<Operand>> Operations;
    if (Syntax.elements) {
        for (const auto &E : *Syntax.elements) {
            auto Ops = handleOperands(E.operation);
            if (!Ops)
                return Ops.takeError();
            Operations.push_back(std::move(*Ops));
        }
    }
    Lifetime Life = handleLifetime(Syntax.lifetime);
    return Matrix{Span{Syntax.Start, Syntax.End}, std::move(Operations), std::move(Life)};
}

// Block handling

llvm::Expected<Block> Modeler::handleBlock(const BlockSyntax &Syntax) {
    auto Stmts = handleStatements(Syntax.statements);
    if (!Stmts)
        return Stmts.takeError();
    return Block{Span{Syntax.Start, Syntax.End}, std::move(*Stmts)};
}

llvm::Expected<std::vector<Statement>> Modeler::handleStatements(
    const std::vector<StatementSyntax> *Syntax) {
    std::vector<Statement> Result;
    if (!Syntax)
        return Result;

    for (const auto &S : *Syntax) {
        auto Stmt = handleStatement(S);
        if (!Stmt)
            return Stmt.takeError();
        Result.push_back(std::move(*Stmt));
    }
    return Result;
}

llvm::Expected<Statement> Modeler::handleStatement(
    const StatementSyntax &Syntax) {
    return handleCommand(Syntax.command);
}

llvm::Expected<Statement> Modeler::handleCommand(const CommandSyntax &Syntax) {
    return std::visit([&](const auto &C) -> llvm::Expected<Statement> {
        using T = std::decay_t<decltype(C)>;

        if constexpr (std::is_same_v<T, OperationSyntax>) {
            auto Ops = handleOperation(C);
            if (!Ops)
                return Ops.takeError();
            return Action{std::move(*Ops), {}};
        } else if constexpr (std::is_same_v<T, LetSyntax>) {
            std::unique_ptr<Type> BindType;
            if (C.binding.annotation) {
                auto T = handleBindingAnnotation(*C.binding.annotation);
                if (!T)
                    return T.takeError();
                BindType = std::move(*T);
            }
            auto Ops = handleOperands(C.binding.operation);
            if (!Ops)
                return Ops.takeError();

            return Binding{
                Span{C.Start, C.End},
                "const",
                Item{
                    Span{C.Start, C.End},
                    false,
                    std::make_unique<std::string>(C.binding.name.str()),
                    std::move(BindType),
                    {}
                },
                std::move(*Ops)
            };
        } else if constexpr (std::is_same_v<T, VarSyntax>) {
            std::unique_ptr<Type> BindType;
            if (C.binding.annotation) {
                auto T = handleBindingAnnotation(*C.binding.annotation);
                if (!T)
                    return T.takeError();
                BindType = std::move(*T);
            }
            auto Ops = handleOperands(C.binding.operation);
            if (!Ops)
                return Ops.takeError();

            return Binding{
                Span{C.Start, C.End},
                "var",
                Item{
                    Span{C.Start, C.End},
                    false,
                    std::make_unique<std::string>(C.binding.name.str()),
                    std::move(BindType),
                    {}
                },
                std::move(*Ops)
            };
        } else if constexpr (std::is_same_v<T, MutableSyntax>) {
            std::unique_ptr<Type> BindType;
            if (C.binding.annotation) {
                auto T = handleBindingAnnotation(*C.binding.annotation);
                if (!T)
                    return T.takeError();
                BindType = std::move(*T);
            }
            auto Ops = handleOperands(C.binding.operation);
            if (!Ops)
                return Ops.takeError();

            return Binding{
                Span{C.Start, C.End},
                "mutable",
                Item{
                    Span{C.Start, C.End},
                    false,
                    std::make_unique<std::string>(C.binding.name.str()),
                    std::move(BindType),
                    {}
                },
                std::move(*Ops)
            };
        } else if constexpr (std::is_same_v<T, SetSyntax>) {
            auto Target = handleOperands(C.target);
            if (!Target)
                return Target.takeError();
            auto Source = handleOperands(C.source);
            if (!Source)
                return Source.takeError();
            return Action{std::move(*Source), std::move(*Target)};
        } else if constexpr (std::is_same_v<T, ContinueSyntax>) {
            auto Cont = handleContinue(C);
            if (!Cont)
                return Cont.takeError();
            return std::move(*Cont);
        } else if constexpr (std::is_same_v<T, BreakSyntax>) {
            auto Brk = handleBreak(C);
            if (!Brk)
                return Brk.takeError();
            return std::move(*Brk);
        } else if constexpr (std::is_same_v<T, ReturnSyntax>) {
            auto Ret = handleReturn(C);
            if (!Ret)
                return Ret.takeError();
            return std::move(*Ret);
        } else if constexpr (std::is_same_v<T, ThrowSyntax>) {
            auto Thr = handleThrow(C);
            if (!Thr)
                return Thr.takeError();
            return std::move(*Thr);
        }
        return makeNotImplementedError(File, "Unknown command", Span{0, 0});
    }, Syntax.Value);
}

// Binding annotation handling

llvm::Expected<std::unique_ptr<Type>> Modeler::handleBindingAnnotation(
    const BindingAnnotationSyntax &Syntax) {

    return std::visit([&](const auto &S) -> llvm::Expected<std::unique_ptr<Type>> {
        using T = std::decay_t<decltype(S)>;
        if constexpr (std::is_same_v<T, TypeSyntax>) {
            return handleType(S);
        } else if constexpr (std::is_same_v<T, StructureSyntax>) {
            return makeNotImplementedError(File, "Structure binding",
                                           Span{S.Start, S.End});
        } else if constexpr (std::is_same_v<T, ArraySyntax>) {
            return makeNotImplementedError(File, "Array binding",
                                           Span{S.Start, S.End});
        }
        return std::unique_ptr<Type>();
    }, Syntax.spec.Value);
}

llvm::Expected<Binding> Modeler::handleCondition(
    const ConditionSyntax &Syntax) {

    return std::visit([&](const auto &C) -> llvm::Expected<Binding> {
        using T = std::decay_t<decltype(C)>;

        if constexpr (std::is_same_v<T, OperationSyntax>) {
            auto Ops = handleOperation(C);
            if (!Ops)
                return Ops.takeError();
            return Binding{
                Span{C.Start, C.End},
                "const",
                Item{Span{C.Start, C.End}, false, nullptr, nullptr, {}},
                std::move(*Ops)
            };
        } else if constexpr (std::is_same_v<T, LetSyntax>) {
            std::unique_ptr<Type> BindType;
            if (C.binding.annotation) {
                auto T = handleBindingAnnotation(*C.binding.annotation);
                if (!T)
                    return T.takeError();
                BindType = std::move(*T);
            }
            auto Ops = handleOperands(C.binding.operation);
            if (!Ops)
                return Ops.takeError();
            return Binding{
                Span{C.Start, C.End},
                "const",
                Item{
                    Span{C.Start, C.End},
                    false,
                    std::make_unique<std::string>(C.binding.name.str()),
                    std::move(BindType),
                    {}
                },
                std::move(*Ops)
            };
        }
        return makeNotImplementedError(File, "Unknown condition", Span{0, 0});
    }, Syntax.Value);
}

// Control flow handling

llvm::Expected<If> Modeler::handleIf(const IfSyntax &Syntax) {
    auto Cond = handleOperands(Syntax.condition);
    if (!Cond)
        return Cond.takeError();

    auto Cons = handleCommand(Syntax.consequent.command);
    if (!Cons)
        return Cons.takeError();

    std::unique_ptr<Statement> Alt;
    if (Syntax.alternative) {
        auto AltResult = handleCommand(Syntax.alternative->alternative);
        if (!AltResult)
            return AltResult.takeError();
        Alt = std::make_unique<Statement>(std::move(*AltResult));
    }

    return If{
        Span{Syntax.Start, Syntax.End},
        std::move(*Cond),
        nullptr,  // Property
        std::make_unique<Statement>(std::move(*Cons)),
        std::move(Alt)
    };
}

llvm::Expected<Match> Modeler::handleMatch(const MatchSyntax &Syntax) {
    auto Cond = handleOperands(Syntax.scrutinee);
    if (!Cond)
        return Cond.takeError();

    std::vector<Branch> Branches;
    if (Syntax.branches) {
        for (const auto &B : *Syntax.branches) {
            std::vector<Case> Cases;
            if (B.cases) {
                for (const auto &C : *B.cases) {
                    auto CaseCond = handleOperands(C.condition);
                    if (!CaseCond)
                        return CaseCond.takeError();
                    Cases.push_back(Case{
                        Span{C.Start, C.End},
                        std::move(*CaseCond)
                    });
                }
            }
            auto Cons = handleStatement(B.consequent);
            if (!Cons)
                return Cons.takeError();
            Branches.push_back(Branch{
                Span{B.Start, B.End},
                std::move(Cases),
                std::make_unique<Statement>(std::move(*Cons))
            });
        }
    }

    std::unique_ptr<Statement> Alt;
    if (Syntax.alternative) {
        auto AltResult = handleCommand(Syntax.alternative->alternative);
        if (!AltResult)
            return AltResult.takeError();
        Alt = std::make_unique<Statement>(std::move(*AltResult));
    }

    return Match{
        Span{Syntax.Start, Syntax.End},
        std::move(*Cond),
        std::move(Branches),
        std::move(Alt)
    };
}

llvm::Expected<When> Modeler::handleWhen(const WhenSyntax &Syntax) {
    std::vector<std::string> VariantPath;
    VariantPath.push_back(std::string(Syntax.variant.name));
    if (Syntax.variant.extensions) {
        for (const auto &E : *Syntax.variant.extensions) {
            VariantPath.push_back(std::string(E.name));
        }
    }

    auto Cons = handleCommand(Syntax.command);
    if (!Cons)
        return Cons.takeError();

    return When{
        Span{Syntax.Start, Syntax.End},
        std::string(Syntax.name),
        std::move(VariantPath),
        std::make_unique<Statement>(std::move(*Cons))
    };
}

llvm::Expected<Choose> Modeler::handleChoose(const ChooseSyntax &Syntax) {
    auto Cond = handleOperands(Syntax.condition);
    if (!Cond)
        return Cond.takeError();

    std::vector<When> Cases;
    if (Syntax.cases) {
        for (const auto &W : *Syntax.cases) {
            auto WhenResult = handleWhen(W);
            if (!WhenResult)
                return WhenResult.takeError();
            Cases.push_back(std::move(*WhenResult));
        }
    }

    std::unique_ptr<Statement> Alt;
    if (Syntax.alternative) {
        auto AltResult = handleCommand(Syntax.alternative->alternative);
        if (!AltResult)
            return AltResult.takeError();
        Alt = std::make_unique<Statement>(std::move(*AltResult));
    }

    return Choose{
        Span{Syntax.Start, Syntax.End},
        std::move(*Cond),
        std::move(Cases),
        std::move(Alt)
    };
}

llvm::Expected<For> Modeler::handleFor(const ForSyntax &Syntax) {
    if (Syntax.name) {
        return makeNotImplementedError(File, "Label in For",
                                       Span{Syntax.name->Start, Syntax.name->End});
    }

    auto Expr = handleOperands(Syntax.operation);
    if (!Expr)
        return Expr.takeError();

    auto Act = handleAction(Syntax.action);
    if (!Act)
        return Act.takeError();

    return For{
        Span{Syntax.Start, Syntax.End},
        std::string(Syntax.variable),
        std::move(*Expr),
        std::move(*Act)
    };
}

llvm::Expected<While> Modeler::handleWhile(const WhileSyntax &Syntax) {
    if (Syntax.name) {
        return makeNotImplementedError(File, "Label in While",
                                       Span{Syntax.name->Start, Syntax.name->End});
    }

    auto Cond = handleCondition(Syntax.condition);
    if (!Cond)
        return Cond.takeError();

    auto Act = handleAction(Syntax.action);
    if (!Act)
        return Act.takeError();

    return While{
        Span{Syntax.Start, Syntax.End},
        std::move(*Cond),
        std::move(*Act)
    };
}

llvm::Expected<Try> Modeler::handleTry(const TrySyntax &Syntax) {
    auto Cond = handleCondition(Syntax.condition);
    if (!Cond)
        return Cond.takeError();

    std::vector<When> Catches;
    if (Syntax.cases) {
        for (const auto &W : *Syntax.cases) {
            auto WhenResult = handleWhen(W);
            if (!WhenResult)
                return WhenResult.takeError();
            Catches.push_back(std::move(*WhenResult));
        }
    }

    std::unique_ptr<Statement> Alt;
    if (Syntax.dropper) {
        auto AltResult = handleCommand(Syntax.dropper->alternative);
        if (!AltResult)
            return AltResult.takeError();
        Alt = std::make_unique<Statement>(std::move(*AltResult));
    }

    return Try{
        Span{Syntax.Start, Syntax.End},
        std::move(*Cond),
        std::move(Catches),
        std::move(Alt)
    };
}

// Special expressions

llvm::Expected<SizeOf> Modeler::handleSizeOf(const SizeOfSyntax &Syntax) {
    auto TypeResult = handleType(Syntax.type);
    if (!TypeResult)
        return TypeResult.takeError();
    return SizeOf{Span{Syntax.Start, Syntax.End}, std::move(**TypeResult)};
}

llvm::Expected<AlignOf> Modeler::handleAlignOf(const AlignOfSyntax &Syntax) {
    auto TypeResult = handleType(Syntax.type);
    if (!TypeResult)
        return TypeResult.takeError();
    return AlignOf{Span{Syntax.Start, Syntax.End}, std::move(**TypeResult)};
}

llvm::Expected<Is> Modeler::handleIs(const IsSyntax &Syntax) {
    std::vector<std::string> Name;
    Name.push_back(std::string(Syntax.name.name));
    if (Syntax.name.extensions) {
        for (const auto &E : *Syntax.name.extensions) {
            Name.push_back(std::string(E.name));
        }
    }
    return Is{Span{Syntax.Start, Syntax.End}, std::move(Name)};
}

llvm::Expected<As> Modeler::handleAs(const AsSyntax &Syntax) {
    auto TypeResult = handleType(Syntax.type);
    if (!TypeResult)
        return TypeResult.takeError();
    return As{Span{Syntax.Start, Syntax.End}, std::move(**TypeResult)};
}

// Action handling

llvm::Expected<Action> Modeler::handleAction(const ActionSyntax &Syntax) {
    return std::visit([&](const auto &A) -> llvm::Expected<Action> {
        using T = std::decay_t<decltype(A)>;
        if constexpr (std::is_same_v<T, OperationSyntax>) {
            auto Ops = handleOperation(A);
            if (!Ops)
                return Ops.takeError();
            return Action{std::move(*Ops), {}};
        } else if constexpr (std::is_same_v<T, SetSyntax>) {
            auto Target = handleOperands(A.target);
            if (!Target)
                return Target.takeError();
            auto Source = handleOperands(A.source);
            if (!Source)
                return Source.takeError();
            return Action{std::move(*Source), std::move(*Target)};
        }
        return makeNotImplementedError(File, "Unknown action", Span{0, 0});
    }, Syntax.Value);
}

// Break, continue, return, throw

llvm::Expected<Break> Modeler::handleBreak(const BreakSyntax &Syntax) {
    auto Result = handleOperands(Syntax.result);
    if (!Result)
        return Result.takeError();
    return Break{Span{Syntax.Start, Syntax.End}, std::move(*Result)};
}

llvm::Expected<Continue> Modeler::handleContinue(const ContinueSyntax &Syntax) {
    return Continue{Span{Syntax.Start, Syntax.End}};
}

llvm::Expected<Return> Modeler::handleReturn(const ReturnSyntax &Syntax) {
    auto Result = handleOperands(Syntax.result);
    if (!Result)
        return Result.takeError();
    return Return{Span{Syntax.Start, Syntax.End}, std::move(*Result)};
}

llvm::Expected<Throw> Modeler::handleThrow(const ThrowSyntax &Syntax) {
    auto Result = handleOperands(Syntax.result);
    if (!Result)
        return Result.takeError();
    return Throw{Span{Syntax.Start, Syntax.End}, std::move(*Result)};
}

// Item and property handling

llvm::Expected<Item> Modeler::handleItem(bool Private,
                                          const ItemSyntax &Syntax) {
    std::unique_ptr<Type> ItemType;
    if (Syntax.annotation) {
        auto T = handleType(Syntax.annotation->type);
        if (!T)
            return T.takeError();
        ItemType = std::move(*T);
    }

    std::vector<Attribute> Attrs;
    if (Syntax.attributes) {
        for (const auto &A : *Syntax.attributes) {
            auto Attr = handleAttribute(A);
            if (!Attr)
                return Attr.takeError();
            Attrs.push_back(std::move(*Attr));
        }
    }

    return Item{
        Span{Syntax.Start, Syntax.End},
        Private,
        std::make_unique<std::string>(Syntax.name.str()),
        std::move(ItemType),
        std::move(Attrs)
    };
}

llvm::Expected<Property> Modeler::handleProperty(bool Private,
                                                  const PropertySyntax &Syntax) {
    auto TypeResult = handleType(Syntax.annotation.type);
    if (!TypeResult)
        return TypeResult.takeError();

    std::unique_ptr<std::vector<Operand>> Init;
    if (Syntax.initializer && Syntax.initializer->operands) {
        auto Ops = handleOperands(Syntax.initializer->operands);
        if (!Ops)
            return Ops.takeError();
        Init = std::make_unique<std::vector<Operand>>(std::move(*Ops));
    }

    std::vector<Attribute> Attrs;
    if (Syntax.attributes) {
        for (const auto &A : *Syntax.attributes) {
            auto Attr = handleAttribute(A);
            if (!Attr)
                return Attr.takeError();
            Attrs.push_back(std::move(*Attr));
        }
    }

    return Property{
        Span{Syntax.Start, Syntax.End},
        Private,
        std::string(Syntax.name),
        std::move(*TypeResult),
        std::move(Init),
        std::move(Attrs)
    };
}

llvm::Expected<std::vector<Item>> Modeler::handleParameterSet(
    const ParameterSetSyntax &Syntax) {
    std::vector<Item> Items;

    std::visit([&](const auto &P) {
        using T = std::decay_t<decltype(P)>;
        if constexpr (std::is_same_v<T, ParametersSyntax>) {
            if (P.items) {
                for (const auto &I : *P.items) {
                    auto ItemResult = handleItem(false, I);
                    if (ItemResult)
                        Items.push_back(std::move(*ItemResult));
                }
            }
        } else if constexpr (std::is_same_v<T, TypeSyntax>) {
            auto TypeResult = handleType(P);
            if (TypeResult) {
                Items.push_back(Item{
                    Span{P.Start, P.End},
                    false,
                    nullptr,
                    std::move(*TypeResult),
                    {}
                });
            }
        }
    }, Syntax.Value);

    return Items;
}

// Attribute handling

llvm::Expected<Attribute> Modeler::handleAttribute(
    const AttributeSyntax &Syntax) {
    auto ModelResult = handleModel(Syntax.model);
    if (!ModelResult)
        return ModelResult.takeError();

    return Attribute{
        Span{Syntax.Start, Syntax.End},
        std::string(Syntax.name),
        std::move(*ModelResult)
    };
}

llvm::Expected<Model> Modeler::handleModel(const ModelSyntax &Syntax) {
    return std::visit([&](const auto &M) -> llvm::Expected<Model> {
        using T = std::decay_t<decltype(M)>;
        if constexpr (std::is_same_v<T, LiteralSyntax>) {
            auto Const = handleLiteral(M);
            if (!Const)
                return Const.takeError();
            return *Const;
        } else if constexpr (std::is_same_v<T, NameSyntax>) {
            return ModelVariable{std::string(M.name)};
        } else if constexpr (std::is_same_v<T, ObjectSyntax>) {
            auto Obj = handleObject(M);
            if (!Obj)
                return Obj.takeError();
            return std::make_unique<Tuple>(std::move(*Obj));
        } else if constexpr (std::is_same_v<T, VectorSyntax>) {
            auto Vec = handleVector(M);
            if (!Vec)
                return Vec.takeError();
            return std::make_unique<Matrix>(std::move(*Vec));
        }
        return ModelVariable{""};
    }, Syntax.Value);
}

// Use handling

llvm::Expected<Use> Modeler::handleUse(const UseSyntax &Syntax) {
    std::vector<std::string> Path;
    Path.push_back(std::string(Syntax.name.name));
    if (Syntax.name.extensions) {
        for (const auto &E : *Syntax.name.extensions) {
            Path.push_back(std::string(E.name));
        }
    }
    return Use{Span{Syntax.Start, Syntax.End}, std::move(Path)};
}

// Generic parameter handling

llvm::Expected<GenericParameter> Modeler::handleGenericParameter(
    const GenericParameterSyntax &Syntax) {
    std::vector<Attribute> Attrs;
    if (Syntax.attributes) {
        for (const auto &A : *Syntax.attributes) {
            auto Attr = handleAttribute(A);
            if (!Attr)
                return Attr.takeError();
            Attrs.push_back(std::move(*Attr));
        }
    }
    return GenericParameter{
        Span{Syntax.Start, Syntax.End},
        std::string(Syntax.name),
        std::move(Attrs)
    };
}

// Extract generic parameters from GenericArgumentsSyntax (for routines)
// In routines, generic params are written as types: function foo[T](x: T)
// We extract the type name as the parameter name
std::vector<GenericParameter> Modeler::extractGenericParams(
    const GenericArgumentsSyntax &Syntax) {
    std::vector<GenericParameter> Params;
    if (Syntax.generics) {
        for (const auto &GA : *Syntax.generics) {
            // Extract the type name as the parameter name
            // For simple generic params like [T], name.name is a single identifier
            if (!GA.type.name.name.empty()) {
                GenericParameter Param;
                Param.Loc = Span{GA.Start, GA.End};
                Param.Name = std::string(GA.type.name.name);
                Params.push_back(std::move(Param));
            }
        }
    }
    return Params;
}

// Initializer handling

llvm::Expected<Initializer> Modeler::handleInitializer(
    const InitSyntax &Syntax, bool Private) {

    // Handle page parameter from lifetime annotation
    std::optional<std::string> PageParameter;
    if (Syntax.lifetime) {
        const auto &Life = Syntax.lifetime->Value;
        if (std::holds_alternative<CallSyntax>(Life)) {
            // init# - valid, first parameter is page name
            // Will extract page name from first parameter below
        } else if (std::holds_alternative<LocalSyntax>(Life)) {
            return makeInvalidInitLifetimeError(File,
                Span{Syntax.Start, Syntax.End},
                "init$ is reserved - use init# for page parameter");
        } else if (std::holds_alternative<ReferenceSyntax>(Life)) {
            return makeInvalidInitLifetimeError(File,
                Span{Syntax.Start, Syntax.End},
                "init^ is reserved - use init# for page parameter");
        } else if (std::holds_alternative<ThrownSyntax>(Life)) {
            return makeInvalidInitLifetimeError(File,
                Span{Syntax.Start, Syntax.End},
                "init! is reserved - use init# for page parameter");
        }
    }

    std::vector<Item> Input;
    if (Syntax.parameters) {
        auto Params = handleParameterSet(*Syntax.parameters);
        if (!Params)
            return Params.takeError();
        Input = std::move(*Params);
    }

    // If init# was used, extract the page parameter name from first param
    if (Syntax.lifetime && std::holds_alternative<CallSyntax>(Syntax.lifetime->Value)) {
        if (Input.empty()) {
            return makeInvalidInitLifetimeError(File,
                Span{Syntax.Start, Syntax.End},
                "init# requires a page parameter as first argument");
        }
        // First parameter is the page name - extract and remove from Input
        if (!Input[0].Name) {
            return makeInvalidInitLifetimeError(File,
                Span{Syntax.Start, Syntax.End},
                "init# page parameter must have a name");
        }
        PageParameter = *Input[0].Name;
        Input.erase(Input.begin());
    }

    auto Act = handleAction(Syntax.action);
    if (!Act)
        return Act.takeError();

    return Initializer{
        Span{Syntax.Start, Syntax.End},
        Private,
        PageParameter,
        std::move(Input),
        std::move(*Act)
    };
}

llvm::Expected<std::shared_ptr<DeInitializer>> Modeler::handleDeInitializer(
    const DeInitSyntax &Syntax) {
    auto Act = handleAction(Syntax.action);
    if (!Act)
        return Act.takeError();

    return std::make_shared<DeInitializer>(DeInitializer{
        Span{Syntax.Start, Syntax.End},
        std::move(*Act)
    });
}

// Function building

llvm::Expected<Function> Modeler::buildFunction(size_t Start, size_t End,
                                                 const TargetSyntax &Syntax,
                                                 bool Private, bool Pure) {
    return std::visit([&](const auto &T) -> llvm::Expected<Function> {
        using TT = std::decay_t<decltype(T)>;

        if constexpr (std::is_same_v<TT, RoutineSyntax>) {
            return makeNotImplementedError(File, "Non-Symbol Function",
                                           Span{Start, End});
        } else if constexpr (std::is_same_v<TT, NamedSyntax>) {
            // Extract generic parameters if present
            std::vector<GenericParameter> GenericParams;
            if (T.routine.generics) {
                GenericParams = extractGenericParams(*T.routine.generics);
            }

            // Handle page parameter from lifetime annotation (function#)
            std::optional<std::string> PageParameter;
            if (T.routine.lifetime) {
                const auto &Life = T.routine.lifetime->Value;
                if (std::holds_alternative<CallSyntax>(Life)) {
                    // function# - valid, first parameter is page name
                    // Will extract page name from first parameter below
                } else if (std::holds_alternative<LocalSyntax>(Life)) {
                    return makeInvalidInitLifetimeError(File,
                        Span{Start, End},
                        "function$ is not supported - use function# for page parameter");
                } else if (std::holds_alternative<ReferenceSyntax>(Life)) {
                    return makeInvalidInitLifetimeError(File,
                        Span{Start, End},
                        "function^ is not supported - use function# for page parameter");
                } else if (std::holds_alternative<ThrownSyntax>(Life)) {
                    return makeInvalidInitLifetimeError(File,
                        Span{Start, End},
                        "function! is not supported - use function# for page parameter");
                }
            }

            std::vector<Item> Input;
            if (T.routine.parameters) {
                auto Params = handleParameterSet(*T.routine.parameters);
                if (!Params)
                    return Params.takeError();
                Input = std::move(*Params);
            }

            // If function# was used, extract the page parameter name from first param
            if (T.routine.lifetime && std::holds_alternative<CallSyntax>(T.routine.lifetime->Value)) {
                if (Input.empty()) {
                    return makeInvalidInitLifetimeError(File,
                        Span{Start, End},
                        "function# requires a page parameter as first argument");
                }
                // First parameter is the page name - extract and remove from Input
                if (!Input[0].Name) {
                    return makeInvalidInitLifetimeError(File,
                        Span{Start, End},
                        "function# page parameter must have a name");
                }
                PageParameter = *Input[0].Name;
                Input.erase(Input.begin());
            }

            std::unique_ptr<Type> Returns;
            if (T.routine.returns_) {
                auto RT = handleType(T.routine.returns_->type);
                if (!RT)
                    return RT.takeError();
                Returns = std::move(*RT);
            }

            std::unique_ptr<Type> Throws;
            if (T.routine.throws_) {
                auto TT = handleType(T.routine.throws_->type);
                if (!TT)
                    return TT.takeError();
                Throws = std::move(*TT);
            }

            Lifetime Life = handleLifetime(T.routine.lifetime);

            Implementation Impl = std::visit([&](const auto &I) -> Implementation {
                using IT = std::decay_t<decltype(I)>;
                if constexpr (std::is_same_v<IT, ActionSyntax>) {
                    auto Act = handleAction(I);
                    if (Act)
                        return std::move(*Act);
                    return Action{{}, {}};
                } else if constexpr (std::is_same_v<IT, ExternSyntax>) {
                    return ExternImpl{Span{I.Start, I.End}};
                } else if constexpr (std::is_same_v<IT, InstructionSyntax>) {
                    return InstructionImpl{Span{I.Start, I.End}};
                } else if constexpr (std::is_same_v<IT, IntrinsicSyntax>) {
                    return IntrinsicImpl{Span{I.Start, I.End}};
                }
                return Action{{}, {}};
            }, T.routine.implementation.Value);

            return Function{
                Span{Start, End},
                Private,
                Pure,
                std::string(T.name),
                std::move(GenericParams),
                PageParameter,
                std::move(Input),
                std::move(Returns),
                std::move(Throws),
                std::move(Life),
                std::move(Impl)
            };
        }
        return makeNotImplementedError(File, "Unknown target", Span{Start, End});
    }, Syntax.Value);
}

// Operator handling

llvm::Expected<Operator> Modeler::handleOperator(const OperatorSyntax &Syntax,
                                                  bool Private) {
    return std::visit([&](const auto &T) -> llvm::Expected<Operator> {
        using TT = std::decay_t<decltype(T)>;

        if constexpr (std::is_same_v<TT, RoutineSyntax>) {
            // Index operator []
            // Extract generic parameters if present
            std::vector<GenericParameter> GenericParams;
            if (T.generics) {
                GenericParams = extractGenericParams(*T.generics);
            }

            std::vector<Item> Input;
            if (T.parameters) {
                auto Params = handleParameterSet(*T.parameters);
                if (!Params)
                    return Params.takeError();
                Input = std::move(*Params);
            }

            std::unique_ptr<Type> Returns;
            if (T.returns_) {
                auto RT = handleType(T.returns_->type);
                if (!RT)
                    return RT.takeError();
                Returns = std::move(*RT);
            }

            std::unique_ptr<Type> Throws;
            if (T.throws_) {
                auto TT = handleType(T.throws_->type);
                if (!TT)
                    return TT.takeError();
                Throws = std::move(*TT);
            }

            Implementation Impl = std::visit([&](const auto &I) -> Implementation {
                using IT = std::decay_t<decltype(I)>;
                if constexpr (std::is_same_v<IT, ActionSyntax>) {
                    auto Act = handleAction(I);
                    if (Act)
                        return std::move(*Act);
                    return Action{{}, {}};
                } else if constexpr (std::is_same_v<IT, ExternSyntax>) {
                    return ExternImpl{Span{I.Start, I.End}};
                } else if constexpr (std::is_same_v<IT, InstructionSyntax>) {
                    return InstructionImpl{Span{I.Start, I.End}};
                } else if constexpr (std::is_same_v<IT, IntrinsicSyntax>) {
                    return IntrinsicImpl{Span{I.Start, I.End}};
                }
                return Action{{}, {}};
            }, T.implementation.Value);

            return Operator{
                Span{Syntax.Start, Syntax.End},
                Private,
                "[]",
                std::move(GenericParams),
                std::move(Input),
                std::move(Returns),
                std::move(Throws),
                std::move(Impl)
            };
        } else if constexpr (std::is_same_v<TT, NamedSyntax>) {
            // Extract generic parameters if present
            std::vector<GenericParameter> GenericParams;
            if (T.routine.generics) {
                GenericParams = extractGenericParams(*T.routine.generics);
            }

            std::vector<Item> Input;
            if (T.routine.parameters) {
                auto Params = handleParameterSet(*T.routine.parameters);
                if (!Params)
                    return Params.takeError();
                Input = std::move(*Params);
            }

            std::unique_ptr<Type> Returns;
            if (T.routine.returns_) {
                auto RT = handleType(T.routine.returns_->type);
                if (!RT)
                    return RT.takeError();
                Returns = std::move(*RT);
            }

            std::unique_ptr<Type> Throws;
            if (T.routine.throws_) {
                auto TT = handleType(T.routine.throws_->type);
                if (!TT)
                    return TT.takeError();
                Throws = std::move(*TT);
            }

            Implementation Impl = std::visit([&](const auto &I) -> Implementation {
                using IT = std::decay_t<decltype(I)>;
                if constexpr (std::is_same_v<IT, ActionSyntax>) {
                    auto Act = handleAction(I);
                    if (Act)
                        return std::move(*Act);
                    return Action{{}, {}};
                } else if constexpr (std::is_same_v<IT, ExternSyntax>) {
                    return ExternImpl{Span{I.Start, I.End}};
                } else if constexpr (std::is_same_v<IT, InstructionSyntax>) {
                    return InstructionImpl{Span{I.Start, I.End}};
                } else if constexpr (std::is_same_v<IT, IntrinsicSyntax>) {
                    return IntrinsicImpl{Span{I.Start, I.End}};
                }
                return Action{{}, {}};
            }, T.routine.implementation.Value);

            return Operator{
                Span{Syntax.Start, Syntax.End},
                Private,
                std::string(T.name),
                std::move(GenericParams),
                std::move(Input),
                std::move(Returns),
                std::move(Throws),
                std::move(Impl)
            };
        }
        return makeNotImplementedError(File, "Unknown operator target",
                                       Span{Syntax.Start, Syntax.End});
    }, Syntax.target.Value);
}

// Definition handling

llvm::Expected<Concept> Modeler::handleDefinition(llvm::StringRef Path,
                                                   const DefinitionSyntax &Syntax,
                                                   bool Private) {
    Span Loc{Syntax.Start, Syntax.End};

    std::vector<GenericParameter> Params;
    if (Syntax.parameters && Syntax.parameters->parameters) {
        for (const auto &GP : *Syntax.parameters->parameters) {
            auto Param = handleGenericParameter(GP);
            if (!Param)
                return Param.takeError();
            Params.push_back(std::move(*Param));
        }
    }

    std::vector<Attribute> Attrs;
    if (Syntax.attributes) {
        for (const auto &A : *Syntax.attributes) {
            auto Attr = handleAttribute(A);
            if (!Attr)
                return Attr.takeError();
            Attrs.push_back(std::move(*Attr));
        }
    }

    auto DefResult = std::visit([&](const auto &C) -> llvm::Expected<Definition> {
        using CT = std::decay_t<decltype(C)>;

        if constexpr (std::is_same_v<CT, ClassSyntax>) {
            auto Struct = handleClass(Syntax.name, Path, C, Private);
            if (!Struct)
                return Struct.takeError();
            return std::move(*Struct);
        } else if constexpr (std::is_same_v<CT, NamespaceSyntax>) {
            auto NS = handleNamespace(Syntax.name, Path, C, Private);
            if (!NS)
                return NS.takeError();
            return std::move(*NS);
        } else if constexpr (std::is_same_v<CT, UnionSyntax>) {
            auto Un = handleUnion(Syntax.name, Path, C, Private);
            if (!Un)
                return Un.takeError();
            return std::move(*Un);
        } else if constexpr (std::is_same_v<CT, ConstantSyntax>) {
            if (!C.operation) {
                // Type alias
                auto TypeRef = handleType(C.type);
                if (!TypeRef)
                    return TypeRef.takeError();
                return std::move(**TypeRef);
            }
            // Global constant
            auto Ops = handleOperands(C.operation);
            if (!Ops)
                return Ops.takeError();
            auto TypeResult = handleType(C.type);
            if (!TypeResult)
                return TypeResult.takeError();
            return Global{Loc, std::move(**TypeResult), std::move(*Ops)};
        } else if constexpr (std::is_same_v<CT, DelegateSyntax>) {
            return makeNotImplementedError(File, "Delegate", Span{C.Start, C.End});
        } else if constexpr (std::is_same_v<CT, IntrinsicSyntax>) {
            return Definition{IntrinsicImpl{Span{C.Start, C.End}}};
        }
        return makeNotImplementedError(File, "Unknown concept", Loc);
    }, Syntax.concept_.Value);

    if (!DefResult)
        return DefResult.takeError();

    return Concept{
        Loc,
        std::string(Syntax.name),
        std::move(Params),
        std::move(Attrs),
        std::move(*DefResult)
    };
}

// Structure handling

llvm::Expected<Structure> Modeler::handleClass(llvm::StringRef Name,
                                                llvm::StringRef Path,
                                                const ClassSyntax &Syntax,
                                                bool Private) {
    std::vector<Property> Props;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;

    if (Syntax.structure.parts) {
        for (const auto &P : *Syntax.structure.parts) {
            std::visit([&](const auto &Part) {
                using PT = std::decay_t<decltype(Part)>;
                if constexpr (std::is_same_v<PT, FieldSyntax>) {
                    auto Prop = handleProperty(true, Part.property);
                    if (Prop) {
                        Props.push_back(std::move(*Prop));
                    }
                } else if constexpr (std::is_same_v<PT, PropertySyntax>) {
                    auto Prop = handleProperty(false, Part);
                    if (Prop) {
                        Props.push_back(std::move(*Prop));
                    }
                }
            }, P.Value);
        }
    }

    std::vector<Module> Modules;
    std::vector<std::vector<std::string>> Uses;
    std::vector<Initializer> Initializers;
    std::shared_ptr<DeInitializer> Deinitializer;
    std::vector<std::variant<Concept, Operator, Function>> Members;

    if (Syntax.body) {
        auto Body = handleBody(Name, Path, *Syntax.body);
        if (!Body)
            return Body.takeError();

        Modules = std::move(Body->Modules);
        for (const auto &U : Body->Uses) {
            Uses.push_back(U.Path);
        }
        Initializers = std::move(Body->Initializers);
        Deinitializer = std::move(Body->Deinitializer);
        Members = std::move(Body->Members);
        Symbols = std::move(Body->Symbols);
    }

    return Structure{
        Span{Syntax.Start, Syntax.End},
        Private,
        std::move(Props),
        std::move(Modules),
        std::move(Uses),
        std::move(Initializers),
        std::move(Deinitializer),
        std::move(Members),
        std::move(Symbols)
    };
}

// Union handling

llvm::Expected<Union> Modeler::handleUnion(llvm::StringRef Name,
                                            llvm::StringRef Path,
                                            const UnionSyntax &Syntax,
                                            bool Private) {
    std::vector<Variant> Variants;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;

    if (Syntax.variants) {
        for (const auto &V : *Syntax.variants) {
            std::unique_ptr<Type> VType;
            if (V.annotation) {
                auto T = handleType(V.annotation->type);
                if (!T)
                    return T.takeError();
                VType = std::move(*T);
            }

            std::vector<Attribute> Attrs;
            if (V.attributes) {
                for (const auto &A : *V.attributes) {
                    auto Attr = handleAttribute(A);
                    if (!Attr)
                        return Attr.takeError();
                    Attrs.push_back(std::move(*Attr));
                }
            }

            Variants.push_back(Variant{
                Span{V.Start, V.End},
                std::string(V.name),
                std::move(VType),
                std::move(Attrs)
            });
        }
    }

    std::vector<std::variant<Concept, Operator, Function>> Members;

    if (Syntax.body) {
        auto Body = handleBody(Name, Path, *Syntax.body);
        if (!Body)
            return Body.takeError();
        Members = std::move(Body->Members);
        Symbols = std::move(Body->Symbols);
    }

    return Union{
        Span{Syntax.Start, Syntax.End},
        Private,
        std::move(Variants),
        std::move(Members),
        std::move(Symbols)
    };
}

// Namespace handling

llvm::Expected<Namespace> Modeler::handleNamespace(llvm::StringRef Name,
                                                    llvm::StringRef Path,
                                                    const NamespaceSyntax &Syntax,
                                                    bool Private) {
    std::vector<Module> Modules;
    std::vector<std::variant<Concept, Operator, Function>> Members;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;

    // Construct namespace path: Path/Name/ for module lookups
    llvm::SmallString<256> NamespacePath(Path);
    llvm::sys::path::append(NamespacePath, Name);

    if (Syntax.declarations) {
        for (const auto &D : *Syntax.declarations) {
            std::visit([&](const auto &S) {
                using ST = std::decay_t<decltype(S)>;
                if constexpr (std::is_same_v<ST, PrivateSyntax>) {
                    std::visit([&](const auto &E) {
                        using ET = std::decay_t<decltype(E)>;
                        if constexpr (std::is_same_v<ET, DefinitionSyntax>) {
                            auto Def = handleDefinition(NamespacePath.str(), E, true);
                            if (Def) {
                                Members.push_back(std::move(*Def));
                            }
                        } else if constexpr (std::is_same_v<ET, FunctionSyntax>) {
                            auto Func = buildFunction(E.Start, E.End, E.target, true, true);
                            if (Func) {
                                Members.push_back(std::move(*Func));
                            }
                        } else if constexpr (std::is_same_v<ET, OperatorSyntax>) {
                            auto Op = handleOperator(E, true);
                            if (Op) {
                                Members.push_back(std::move(*Op));
                            }
                        } else if constexpr (std::is_same_v<ET, ModuleSyntax>) {
                            auto Mod = handleModule(NamespacePath.str(), E, true);
                            if (Mod) {
                                Modules.push_back(std::move(*Mod));
                            } else {
                                llvm::consumeError(Mod.takeError());
                            }
                        }
                    }, S.export_.Value);
                } else if constexpr (std::is_same_v<ST, DefinitionSyntax>) {
                    auto Def = handleDefinition(NamespacePath.str(), S, false);
                    if (Def) {
                        Members.push_back(std::move(*Def));
                    }
                } else if constexpr (std::is_same_v<ST, FunctionSyntax>) {
                    auto Func = buildFunction(S.Start, S.End, S.target, false, true);
                    if (Func) {
                        Members.push_back(std::move(*Func));
                    }
                } else if constexpr (std::is_same_v<ST, OperatorSyntax>) {
                    auto Op = handleOperator(S, false);
                    if (Op) {
                        Members.push_back(std::move(*Op));
                    }
                } else if constexpr (std::is_same_v<ST, ModuleSyntax>) {
                    auto Mod = handleModule(NamespacePath.str(), S, false);
                    if (Mod) {
                        Modules.push_back(std::move(*Mod));
                    } else {
                        llvm::consumeError(Mod.takeError());
                    }
                }
            }, D.symbol.Value);
        }
    }

    return Namespace{
        Span{Syntax.Start, Syntax.End},
        std::move(Modules),
        std::move(Members),
        std::move(Symbols)
    };
}

// Body handling

llvm::Expected<Modeler::BodyResult> Modeler::handleBody(
    llvm::StringRef Name, llvm::StringRef Path, const BodySyntax &Syntax) {

    BodyResult Result;

    // Construct sub-path for module lookups: Path/Name/
    llvm::SmallString<256> SubPath(Path);
    llvm::sys::path::append(SubPath, Name);

    if (Syntax.uses) {
        for (const auto &U : *Syntax.uses) {
            auto UseResult = handleUse(U);
            if (!UseResult)
                return UseResult.takeError();
            Result.Uses.push_back(std::move(*UseResult));
        }
    }

    if (Syntax.inits) {
        for (const auto &I : *Syntax.inits) {
            auto Init = handleInitializer(I, false);
            if (!Init)
                return Init.takeError();
            Result.Initializers.push_back(std::move(*Init));
        }
    }

    if (Syntax.deInit) {
        auto DeInit = handleDeInitializer(*Syntax.deInit);
        if (!DeInit)
            return DeInit.takeError();
        Result.Deinitializer = std::move(*DeInit);
    }

    if (Syntax.members) {
        for (const auto &M : *Syntax.members) {
            std::visit([&](const auto &C) {
                using CT = std::decay_t<decltype(C)>;
                if constexpr (std::is_same_v<CT, DefinitionSyntax>) {
                    auto Def = handleDefinition(Path, C, false);
                    if (Def) {
                        Result.Members.push_back(std::move(*Def));
                    }
                } else if constexpr (std::is_same_v<CT, FunctionSyntax>) {
                    auto Func = buildFunction(C.Start, C.End, C.target, false, true);
                    if (Func) {
                        Result.Members.push_back(std::move(*Func));
                    } else {
                        llvm::consumeError(Func.takeError());
                    }
                } else if constexpr (std::is_same_v<CT, ProcedureSyntax>) {
                    auto Func = buildFunction(C.Start, C.End, C.target, false, false);
                    if (Func) {
                        Result.Members.push_back(std::move(*Func));
                    } else {
                        llvm::consumeError(Func.takeError());
                    }
                } else if constexpr (std::is_same_v<CT, OperatorSyntax>) {
                    auto Op = handleOperator(C, false);
                    if (Op) {
                        Result.Members.push_back(std::move(*Op));
                    }
                } else if constexpr (std::is_same_v<CT, ModuleSyntax>) {
                    auto Mod = handleModule(SubPath.str(), C, false);
                    if (Mod) {
                        Result.Modules.push_back(std::move(*Mod));
                    } else {
                        llvm::consumeError(Mod.takeError());
                    }
                }
            }, M.constituent.Value);
        }
    }

    return Result;
}

// Module resolution

llvm::Expected<Module> Modeler::buildReferencedModule(llvm::StringRef Path,
                                                       llvm::StringRef Name,
                                                       bool Private) {
    // Build filename: path/name.scaly
    llvm::SmallString<256> FilePath;
    if (!Path.empty()) {
        FilePath = Path;
        llvm::sys::path::append(FilePath, Name);
    } else {
        FilePath = Name;
    }
    FilePath += ".scaly";

    // Read the file
    auto BufOrErr = llvm::MemoryBuffer::getFile(FilePath);
    if (!BufOrErr) {
        return llvm::make_error<llvm::StringError>(
            "cannot open module file: " + FilePath.str().str(),
            BufOrErr.getError());
    }

    // Parse the file
    Parser ModuleParser((*BufOrErr)->getBuffer());
    auto FileResult = ModuleParser.parseFile();
    if (!FileResult) {
        return llvm::make_error<llvm::StringError>(
            FilePath.str().str() + ": " + llvm::toString(FileResult.takeError()),
            llvm::inconvertibleErrorCode());
    }

    // Build the module
    return buildModule(Path.str(), FilePath.str().str(), Name.str(), *FileResult, Private);
}

llvm::Expected<Module> Modeler::handleModule(llvm::StringRef Path,
                                              const ModuleSyntax &Syntax,
                                              bool Private) {
    return buildReferencedModule(Path, Syntax.name, Private);
}

// Module building

llvm::Expected<Module> Modeler::buildModule(llvm::StringRef Path,
                                             llvm::StringRef FileName,
                                             llvm::StringRef Name,
                                             const FileSyntax &Syntax,
                                             bool Private) {
    std::vector<Use> Uses;
    if (Syntax.uses) {
        for (const auto &U : *Syntax.uses) {
            auto UseResult = handleUse(U);
            if (!UseResult)
                return UseResult.takeError();
            Uses.push_back(std::move(*UseResult));
        }
    }

    std::vector<Module> Modules;
    std::vector<Member> Members;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;
    llvm::Error Err = llvm::Error::success();

    if (Syntax.declarations) {
        // llvm::errs() << "DEBUG buildModule: " << Name << " has " << Syntax.declarations->size() << " declarations\n";
        int declIdx = 0;
        for (const auto &D : *Syntax.declarations) {
            if (Err) {
                // llvm::errs() << "DEBUG buildModule: Error occurred at decl " << declIdx << "\n";
                break;
            }

            std::visit([&](const auto &S) {
                using ST = std::decay_t<decltype(S)>;
                if constexpr (std::is_same_v<ST, PrivateSyntax>) {
                    std::visit([&](const auto &E) {
                        using ET = std::decay_t<decltype(E)>;
                        if constexpr (std::is_same_v<ET, DefinitionSyntax>) {
                            auto Def = handleDefinition(Path.str(), E, true);
                            if (Def) {
                                Members.push_back(std::move(*Def));
                            } else {
                                Err = Def.takeError();
                            }
                        } else if constexpr (std::is_same_v<ET, FunctionSyntax>) {
                            auto Func = buildFunction(E.Start, E.End, E.target, true, true);
                            if (Func) {
                                Members.push_back(std::move(*Func));
                            } else {
                                Err = Func.takeError();
                            }
                        } else if constexpr (std::is_same_v<ET, OperatorSyntax>) {
                            auto Op = handleOperator(E, true);
                            if (Op) {
                                Members.push_back(std::move(*Op));
                            } else {
                                Err = Op.takeError();
                            }
                        } else if constexpr (std::is_same_v<ET, ModuleSyntax>) {
                            auto Mod = handleModule(Path, E, true);
                            if (Mod) {
                                Modules.push_back(std::move(*Mod));
                            } else {
                                Err = Mod.takeError();
                            }
                        }
                    }, S.export_.Value);
                } else if constexpr (std::is_same_v<ST, DefinitionSyntax>) {
                    // llvm::errs() << "DEBUG buildModule: Processing definition at decl " << declIdx << "\n";
                    auto Def = handleDefinition(Path.str(), S, false);
                    if (Def) {
                        // llvm::errs() << "DEBUG buildModule: Added definition: " << Def->Name << "\n";
                        Members.push_back(std::move(*Def));
                    } else {
                        // llvm::errs() << "DEBUG buildModule: Definition failed\n";
                        Err = Def.takeError();
                    }
                } else if constexpr (std::is_same_v<ST, FunctionSyntax>) {
                    // llvm::errs() << "DEBUG buildModule: Processing function at decl " << declIdx << "\n";
                    auto Func = buildFunction(S.Start, S.End, S.target, false, true);
                    if (Func) {
                        // llvm::errs() << "DEBUG buildModule: Added function: " << Func->Name << "\n";
                        Members.push_back(std::move(*Func));
                    } else {
                        // llvm::errs() << "DEBUG buildModule: Function failed\n";
                        Err = Func.takeError();
                    }
                } else if constexpr (std::is_same_v<ST, OperatorSyntax>) {
                    auto Op = handleOperator(S, false);
                    if (Op) {
                        Members.push_back(std::move(*Op));
                    } else {
                        Err = Op.takeError();
                    }
                } else if constexpr (std::is_same_v<ST, ModuleSyntax>) {
                    auto Mod = handleModule(Path, S, false);
                    if (Mod) {
                        Modules.push_back(std::move(*Mod));
                    } else {
                        Err = Mod.takeError();
                    }
                }
            }, D.symbol.Value);
            declIdx++;
        }
    }

    if (Err)
        return std::move(Err);

    return Module{
        Private,
        FileName.str(),
        Name.str(),
        std::move(Modules),
        std::move(Uses),
        std::move(Members),
        std::move(Symbols)
    };
}

// Package resolution with multi-path search

llvm::Expected<Module> Modeler::resolvePackage(llvm::StringRef Name, const Version &Ver) {
    std::string Key = Name.str() + "@" + Ver.toString();

    // Check if already resolved (avoid duplicate loading)
    auto CacheIt = ResolvedPackages.find(Key);
    if (CacheIt != ResolvedPackages.end()) {
        // Return a copy of the cached module
        return *CacheIt->second.Root;
    }

    // Check for circular dependency
    if (LoadingPackages.count(Key)) {
        return llvm::make_error<llvm::StringError>(
            "circular package dependency: " + Key,
            llvm::inconvertibleErrorCode()
        );
    }
    LoadingPackages.insert(Key);

    // Build list of search paths per CLAUDE.md:
    // 1. ./packages/ relative to current working directory
    // 2. ./packages/ relative to source file directory
    // 3. Walk up from source file looking for packages/ directory
    // 4. /usr/share/scaly/packages/ (system-wide, skip on Windows)
    // 5. -I paths in order
    std::vector<std::string> SearchPaths;

    // Current working directory's packages/
    SearchPaths.push_back("packages");

    // Source file directory's packages/
    llvm::SmallString<256> LocalPath(File);
    llvm::sys::path::remove_filename(LocalPath);
    llvm::SmallString<256> SourcePackages(LocalPath);
    llvm::sys::path::append(SourcePackages, "packages");
    SearchPaths.push_back(SourcePackages.str().str());

    // Walk up directory tree from source file looking for packages/
    llvm::SmallString<256> WalkUp(LocalPath);
    for (int i = 0; i < 10 && !WalkUp.empty(); ++i) {
        llvm::sys::path::remove_filename(WalkUp);
        if (WalkUp.empty()) break;
        llvm::SmallString<256> ParentPackages(WalkUp);
        llvm::sys::path::append(ParentPackages, "packages");
        if (llvm::sys::fs::is_directory(ParentPackages)) {
            SearchPaths.push_back(ParentPackages.str().str());
            break;
        }
    }

    // System-wide (skip on Windows)
#ifndef _WIN32
    SearchPaths.push_back("/usr/share/scaly/packages");
#endif

    // User-provided -I paths
    for (const auto &P : PackageSearchPaths) {
        SearchPaths.push_back(P);
    }

    // Try each search path
    for (const auto &Base : SearchPaths) {
        llvm::SmallString<256> PkgPath(Base);
        llvm::sys::path::append(PkgPath, Name, Ver.toString(), Name.str() + ".scaly");

        if (llvm::sys::fs::exists(PkgPath)) {
            // Read and parse with parseProgram to get package declarations
            auto BufOrErr = llvm::MemoryBuffer::getFile(PkgPath);
            if (!BufOrErr) {
                LoadingPackages.erase(Key);
                return llvm::make_error<llvm::StringError>(
                    "cannot open package file: " + PkgPath.str().str(),
                    BufOrErr.getError());
            }

            Parser PkgParser((*BufOrErr)->getBuffer());
            auto ParseResult = PkgParser.parseProgram();
            if (!ParseResult) {
                LoadingPackages.erase(Key);
                return llvm::make_error<llvm::StringError>(
                    PkgPath.str().str() + ": " + llvm::toString(ParseResult.takeError()),
                    llvm::inconvertibleErrorCode());
            }

            // Recursively resolve transitive package dependencies
            if (ParseResult->file.packages) {
                for (const auto &Pkg : *ParseResult->file.packages) {
                    std::string TransPkgName(Pkg.name.name);
                    Version TransVer{0, 0, 0};
                    if (Pkg.version) {
                        std::visit([&TransVer](const auto &Lit) {
                            using T = std::decay_t<decltype(Lit)>;
                            if constexpr (std::is_same_v<T, FloatingPointLiteral>) {
                                llvm::StringRef MajorMinor(Lit.Value);
                                auto DotPos = MajorMinor.find('.');
                                if (DotPos != llvm::StringRef::npos) {
                                    MajorMinor.substr(0, DotPos).getAsInteger(10, TransVer.Major);
                                    MajorMinor.substr(DotPos + 1).getAsInteger(10, TransVer.Minor);
                                }
                            } else if constexpr (std::is_same_v<T, IntegerLiteral>) {
                                Lit.Value.getAsInteger(10, TransVer.Major);
                            }
                        }, Pkg.version->majorMinor);

                        std::visit([&TransVer](const auto &Lit) {
                            using T = std::decay_t<decltype(Lit)>;
                            if constexpr (std::is_same_v<T, IntegerLiteral>) {
                                Lit.Value.getAsInteger(10, TransVer.Patch);
                            }
                        }, Pkg.version->patch);
                    }

                    // Recursively resolve (will detect cycles)
                    auto TransResult = resolvePackage(TransPkgName, TransVer);
                    if (!TransResult) {
                        LoadingPackages.erase(Key);
                        return TransResult.takeError();
                    }
                }
            }

            // Build the module from the file syntax
            auto Result = buildModule(
                llvm::sys::path::parent_path(PkgPath).str(),
                PkgPath.str().str(),
                Name.str(),
                ParseResult->file,
                false
            );

            if (!Result) {
                LoadingPackages.erase(Key);
                return Result.takeError();
            }

            // Cache the resolved package
            ResolvedPackages[Key] = Package{
                Name.str(),
                Ver,
                std::make_shared<Module>(std::move(*Result))
            };

            LoadingPackages.erase(Key);
            return *ResolvedPackages[Key].Root;
        }
    }

    LoadingPackages.erase(Key);
    return llvm::make_error<llvm::StringError>(
        "package not found: " + Name.str() + " " + Ver.toString() +
        " (searched " + std::to_string(SearchPaths.size()) + " paths)",
        llvm::inconvertibleErrorCode()
    );
}

// Program building

llvm::Expected<Program> Modeler::buildProgram(const ProgramSyntax &Syntax) {
    // Get the directory containing the source file
    llvm::SmallString<256> BasePath(File);
    llvm::sys::path::remove_filename(BasePath);

    // Load packages (resolvePackage handles transitive deps and caches in ResolvedPackages)
    if (Syntax.file.packages) {
        for (const auto &Pkg : *Syntax.file.packages) {
            // Get package name from NameSyntax
            std::string PkgName(Pkg.name.name);

            // Parse version if present
            Version Ver{0, 0, 0};
            if (Pkg.version) {
                // majorMinor is "X.Y" (FloatingPointLiteral), patch is "Z" (IntegerLiteral)
                std::visit([&Ver](const auto &Lit) {
                    using T = std::decay_t<decltype(Lit)>;
                    if constexpr (std::is_same_v<T, FloatingPointLiteral>) {
                        llvm::StringRef MajorMinor(Lit.Value);
                        auto DotPos = MajorMinor.find('.');
                        if (DotPos != llvm::StringRef::npos) {
                            MajorMinor.substr(0, DotPos).getAsInteger(10, Ver.Major);
                            MajorMinor.substr(DotPos + 1).getAsInteger(10, Ver.Minor);
                        }
                    } else if constexpr (std::is_same_v<T, IntegerLiteral>) {
                        // Handle case where majorMinor might be just an integer
                        Lit.Value.getAsInteger(10, Ver.Major);
                    }
                }, Pkg.version->majorMinor);

                std::visit([&Ver](const auto &Lit) {
                    using T = std::decay_t<decltype(Lit)>;
                    if constexpr (std::is_same_v<T, IntegerLiteral>) {
                        Lit.Value.getAsInteger(10, Ver.Patch);
                    }
                }, Pkg.version->patch);
            }

            // Resolve package (recursively resolves transitive dependencies)
            auto PkgResult = resolvePackage(PkgName, Ver);
            if (!PkgResult)
                return PkgResult.takeError();
        }
    }

    // Collect all resolved packages (including transitive dependencies)
    std::vector<Package> Packages;
    for (auto &[Key, Pkg] : ResolvedPackages) {
        Packages.push_back(std::move(Pkg));
    }
    ResolvedPackages.clear();  // Clear for next use

    // Build main module
    auto MainModule = buildModule(BasePath.str(), File, "", Syntax.file, false);
    if (!MainModule)
        return MainModule.takeError();

    auto Stmts = handleStatements(Syntax.statements);
    if (!Stmts)
        return Stmts.takeError();

    return Program{
        std::move(Packages),
        std::move(*MainModule),
        std::move(*Stmts)
    };
}

} // namespace scaly
