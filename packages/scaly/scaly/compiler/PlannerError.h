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
    String name;
    Span span;

    UndefinedType(String file, String name, Span span);
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
    };
    String to_string(Page* rp);
};
#endif