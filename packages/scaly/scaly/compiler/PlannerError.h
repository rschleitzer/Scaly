#ifndef _PlannerError_h
#define _PlannerError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct DuplicateIntrinsic : Object {
    String name;

    DuplicateIntrinsic(String name);
};

struct DuplicateFunction : Object {
    String name;

    DuplicateFunction(String name);
};

struct InstructionWithoutArguments : Object {
    String name;

    InstructionWithoutArguments(String name);
};

struct UnknownInstruction : Object {
    String name;

    UnknownInstruction(String name);
};

struct InstructionWithInvalidNumberOfArguments : Object {
    String name;

    InstructionWithInvalidNumberOfArguments(String name);
};

struct TupleComponentNamesNotSupported : Object {

};

struct UndefinedType : Object {
    String file;
    Span span;
    String name;

    UndefinedType(String file, Span span, String name);
    String to_string(Page* rp);
};

struct UndefinedTypeOrValue : Object {
    String file;
    Span span;
    String name;

    UndefinedTypeOrValue(String file, Span span, String name);
    String to_string(Page* rp);
};

struct ConceptExpected : Object {
    String file;
    Span span;
    String name;

    ConceptExpected(String file, Span span, String name);
    String to_string(Page* rp);
};

struct InvalidNumberOfArguments : Object {
    String file;
    Span span;
    String name;
    int expected;
    int provided;

    InvalidNumberOfArguments(String file, Span span, String name, int expected, int provided);
    String to_string(Page* rp);
};

struct TypeMismatch : Object {
    String file;
    Span span;
    String expected;
    String provided;

    TypeMismatch(String file, Span span, String expected, String provided);
    String to_string(Page* rp);
};

struct PlannerError : Object {
    PlannerError(FeatureNotImplemented);
    PlannerError(FileError);
    PlannerError(DuplicateIntrinsic);
    PlannerError(DuplicateFunction);
    PlannerError(InstructionWithoutArguments);
    PlannerError(UnknownInstruction);
    PlannerError(InstructionWithInvalidNumberOfArguments);
    PlannerError(TupleComponentNamesNotSupported);
    PlannerError(UndefinedType);
    PlannerError(UndefinedTypeOrValue);
    PlannerError(ConceptExpected);
    PlannerError(InvalidNumberOfArguments);
    PlannerError(TypeMismatch);
    enum {
        NotImplemented,
        FileError,
        DuplicateIntrinsic,
        DuplicateFunction,
        InstructionWithoutArguments,
        UnknownInstruction,
        InstructionWithInvalidNumberOfArguments,
        TupleComponentNamesNotSupported,
        UndefinedType,
        UndefinedTypeOrValue,
        ConceptExpected,
        InvalidNumberOfArguments,
        TypeMismatch,
    } _tag;
    union {
        struct FeatureNotImplemented _NotImplemented;
        struct FileError _FileError;
        struct DuplicateIntrinsic _DuplicateIntrinsic;
        struct DuplicateFunction _DuplicateFunction;
        struct InstructionWithoutArguments _InstructionWithoutArguments;
        struct UnknownInstruction _UnknownInstruction;
        struct InstructionWithInvalidNumberOfArguments _InstructionWithInvalidNumberOfArguments;
        struct TupleComponentNamesNotSupported _TupleComponentNamesNotSupported;
        struct UndefinedType _UndefinedType;
        struct UndefinedTypeOrValue _UndefinedTypeOrValue;
        struct ConceptExpected _ConceptExpected;
        struct InvalidNumberOfArguments _InvalidNumberOfArguments;
        struct TypeMismatch _TypeMismatch;
    };
    String to_string(Page* rp);
};
#endif