// Modeler.cpp - Syntax to semantic model transformation
#include "Modeler.h"
#include "Parser.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <cerrno>
#include <cstdlib>

namespace scaly {

Modeler::Modeler(llvm::StringRef FileName) : File(FileName.str()) {}

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
            return StringConstant{Loc, Lit.Value.str()};
        } else if constexpr (std::is_same_v<T, CharacterLiteral>) {
            return CharacterConstant{Loc, Lit.Value.str()};
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

    std::unique_ptr<std::vector<std::string>> MemberAccess;
    if (Syntax.members) {
        MemberAccess = std::make_unique<std::vector<std::string>>();
        for (const auto &M : *Syntax.members) {
            MemberAccess->push_back(std::string(M.name.name));
            if (M.name.extensions) {
                for (const auto &Ext : *M.name.extensions) {
                    MemberAccess->push_back(std::string(Ext.name));
                }
            }
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
        } else if constexpr (std::is_same_v<T, TypeSyntax>) {
            auto TypeResult = handleType(E);
            if (!TypeResult)
                return TypeResult.takeError();
            return std::move(**TypeResult);
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
        } else if constexpr (std::is_same_v<T, IsSyntax>) {
            auto IsResult = handleIs(E);
            if (!IsResult)
                return IsResult.takeError();
            return std::move(*IsResult);
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
            if (auto *TypeExpr = std::get_if<TypeSyntax>(&NameOp.expression.Value)) {
                Name = std::make_unique<std::string>(TypeExpr->name.name.str());
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
    return Matrix{Span{Syntax.Start, Syntax.End}, std::move(Operations)};
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

    auto Cons = handleCommand(Syntax.consequent);
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

// Initializer handling

llvm::Expected<Initializer> Modeler::handleInitializer(
    const InitSyntax &Syntax, bool Private) {
    std::vector<Item> Input;
    if (Syntax.parameters) {
        auto Params = handleParameterSet(*Syntax.parameters);
        if (!Params)
            return Params.takeError();
        Input = std::move(*Params);
    }

    auto Act = handleAction(Syntax.action);
    if (!Act)
        return Act.takeError();

    return Initializer{
        Span{Syntax.Start, Syntax.End},
        Private,
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
                std::move(Input),
                std::move(Returns),
                std::move(Throws),
                std::move(Impl)
            };
        } else if constexpr (std::is_same_v<TT, NamedSyntax>) {
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
                    }
                } else if constexpr (std::is_same_v<CT, ProcedureSyntax>) {
                    auto Func = buildFunction(C.Start, C.End, C.target, false, false);
                    if (Func) {
                        Result.Members.push_back(std::move(*Func));
                    }
                } else if constexpr (std::is_same_v<CT, OperatorSyntax>) {
                    auto Op = handleOperator(C, false);
                    if (Op) {
                        Result.Members.push_back(std::move(*Op));
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

    if (!ModuleParser.isAtEnd()) {
        return llvm::make_error<llvm::StringError>(
            FilePath.str().str() + ": unexpected content after file",
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
        for (const auto &D : *Syntax.declarations) {
            if (Err)
                break;

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
                    auto Def = handleDefinition(Path.str(), S, false);
                    if (Def) {
                        Members.push_back(std::move(*Def));
                    } else {
                        Err = Def.takeError();
                    }
                } else if constexpr (std::is_same_v<ST, FunctionSyntax>) {
                    auto Func = buildFunction(S.Start, S.End, S.target, false, true);
                    if (Func) {
                        Members.push_back(std::move(*Func));
                    } else {
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

// Program building

llvm::Expected<Program> Modeler::buildProgram(const ProgramSyntax &Syntax) {
    // Get the directory containing the source file
    llvm::SmallString<256> BasePath(File);
    llvm::sys::path::remove_filename(BasePath);

    // Load packages
    std::vector<Package> Packages;
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

            // Package path: packages/<name>/<version>/<name>.scaly (if versioned)
            // Or fallback: ../<name> (legacy unversioned)
            llvm::SmallString<256> PkgPath(BasePath);
            if (Pkg.version) {
                llvm::sys::path::append(PkgPath, "packages", PkgName, Ver.toString());
            } else {
                llvm::sys::path::append(PkgPath, "..", PkgName);
            }

            auto PkgResult = buildReferencedModule(PkgPath.str(), PkgName, false);
            if (!PkgResult)
                return PkgResult.takeError();

            Packages.push_back(Package{
                PkgName,
                Ver,
                std::make_shared<Module>(std::move(*PkgResult))
            });
        }
    }

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
