#ifndef _ModelError_h
#define _ModelError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct Position : Object {
    size_t line;
    size_t column;

    Position(size_t line, size_t column);
};

Position calculate_position_from_string(String text, size_t offset);
String build_hint_lines_from_string(Page* rp, String text, size_t start_offset, size_t end_offset, Position start_position, Position end_position);
Result<String, FileError> build_hint_lines(Page* rp, Page* ep, String file, size_t start_offset, size_t end_offset, Position start_position, Position end_position);
Result<Position, FileError> calculate_position(Page* rp, Page* ep, String file, size_t offset);
String to_string(Page* rp, size_t number);
void append_error_message_header(StringBuilder& builder, String file, size_t offset);
void append_hint_lines(StringBuilder& builder, String file, size_t start, size_t end);
struct IoModelError : Object {
    IoModelError(FileError);
    enum {
        File,
    } _tag;
    union {
        struct FileError _File;
    };
    String to_string(Page* rp);
};
struct ParserModelError : Object {
    String file;
    ParserError error;

    ParserModelError(String file, ParserError error);
    String to_string(Page* rp);
    String build_error_message(Page* rp, InvalidSyntax invalid_syntax);
};

struct NotImplemented : Object {
    String file;
    String name;
    Span span;

    NotImplemented(String file, String name, Span span);
    String to_string(Page* rp);
};

struct DuplicateName : Object {
    String file;
    Span span;

    DuplicateName(String file, Span span);
    String to_string(Page* rp);
};

struct NonFunctionSymbolExists : Object {
    String file;
    Span span;

    NonFunctionSymbolExists(String file, Span span);
    String to_string(Page* rp);
};

struct FunctionSymbolExists : Object {
    String file;
    Span span;

    FunctionSymbolExists(String file, Span span);
    String to_string(Page* rp);
};

struct DeInitializerExists : Object {
    String file;
    Span span;

    DeInitializerExists(String file, Span span);
    String to_string(Page* rp);
};

struct InvalidConstant : Object {
    String file;
    Span span;

    InvalidConstant(String file, Span span);
    String to_string(Page* rp);
};

struct InvalidComponentName : Object {
    String file;
    Span span;

    InvalidComponentName(String file, Span span);
    String to_string(Page* rp);
};

struct ModuleRootMustBeConcept : Object {
    String file;
    Span span;

    ModuleRootMustBeConcept(String file, Span span);
    String to_string(Page* rp);
};

struct ModelBuilderError : Object {
    ModelBuilderError(NotImplemented);
    ModelBuilderError(DuplicateName);
    ModelBuilderError(NonFunctionSymbolExists);
    ModelBuilderError(FunctionSymbolExists);
    ModelBuilderError(DeInitializerExists);
    ModelBuilderError(InvalidConstant);
    ModelBuilderError(InvalidComponentName);
    ModelBuilderError(ModuleRootMustBeConcept);
    enum {
        NotImplemented,
        DuplicateName,
        NonFunctionSymbolExists,
        FunctionSymbolExists,
        DeInitializerExists,
        InvalidConstant,
        InvalidComponentName,
        ModuleRootMustBeConcept,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct DuplicateName _DuplicateName;
        struct NonFunctionSymbolExists _NonFunctionSymbolExists;
        struct FunctionSymbolExists _FunctionSymbolExists;
        struct DeInitializerExists _DeInitializerExists;
        struct InvalidConstant _InvalidConstant;
        struct InvalidComponentName _InvalidComponentName;
        struct ModuleRootMustBeConcept _ModuleRootMustBeConcept;
    };
    String to_string(Page* rp);
};
struct ModelError : Object {
    ModelError(IoModelError);
    ModelError(ParserModelError);
    ModelError(ModelBuilderError);
    enum {
        Io,
        Parser,
        Builder,
    } _tag;
    union {
        struct IoModelError _Io;
        struct ParserModelError _Parser;
        struct ModelBuilderError _Builder;
    };
    String to_string(Page* rp);
};
#endif