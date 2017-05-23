#ifndef __scalyc__OptionsError__
#define __scalyc__OptionsError__
#include "scalycpp.h"
using namespace scaly;
namespace scalyc {

class OptionsError;

class _OptionsError_invalidOption : public Object {
public:
    _OptionsError_invalidOption(string* option);

    string* option;
};

class _OptionsError_unknownOption : public Object {
public:
    _OptionsError_unknownOption(string* option);

    string* option;
};
enum _OptionsErrorCode {
    _OptionsErrorCode_invalidOption = 1,
    _OptionsErrorCode_unknownOption,
    _OptionsErrorCode_noOutputOption,
    _OptionsErrorCode_noFilesToCompile,
};

class OptionsError : public Object {
public:
    OptionsError(_OptionsErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    OptionsError(_OptionsError_invalidOption* invalidOption)
    : errorCode(_OptionsErrorCode_invalidOption), errorInfo(invalidOption) {}

    OptionsError(_OptionsError_unknownOption* unknownOption)
    : errorCode(_OptionsErrorCode_unknownOption), errorInfo(unknownOption) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _OptionsError_invalidOption* get_invalidOption();
    _OptionsError_unknownOption* get_unknownOption();

private:
    _OptionsErrorCode errorCode;
    void* errorInfo;
};

class ParserError;

class _ParserError_syntax : public Object {
public:
    _ParserError_syntax(size_t line, size_t column);

    size_t line;
    size_t column;
};
enum _ParserErrorCode {
    _ParserErrorCode_syntax = 1,
};

class ParserError : public Object {
public:
    ParserError(_ParserErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    ParserError(_ParserError_syntax* syntax)
    : errorCode(_ParserErrorCode_syntax), errorInfo(syntax) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _ParserError_syntax* get_syntax();

private:
    _ParserErrorCode errorCode;
    void* errorInfo;
};

class CompilerError;

class _CompilerError_parser : public Object {
public:
    _CompilerError_parser(size_t line, size_t column);

    size_t line;
    size_t column;
};
enum _CompilerErrorCode {
    _CompilerErrorCode_parser = 1,
};

class CompilerError : public Object {
public:
    CompilerError(_CompilerErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CompilerError(_CompilerError_parser* parser)
    : errorCode(_CompilerErrorCode_parser), errorInfo(parser) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _CompilerError_parser* get_parser();

private:
    _CompilerErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__OptionsError__
