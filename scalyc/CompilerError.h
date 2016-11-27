#ifndef __scalyc__CompilerError__
#define __scalyc__CompilerError__
#include "scalyc.h"
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

class _ParserError_identifierExpected : public Object {
public:
    _ParserError_identifierExpected(Position* position);

    Position* position;
};

class _ParserError_literalExpected : public Object {
public:
    _ParserError_literalExpected(Position* position);

    Position* position;
};

class _ParserError_keywordExpected : public Object {
public:
    _ParserError_keywordExpected(Position* position, string* keyword);

    Position* position;
    string* keyword;
};

class _ParserError_punctuationExpected : public Object {
public:
    _ParserError_punctuationExpected(Position* position, string* punctuation);

    Position* position;
    string* punctuation;
};

class _ParserError_operatorExpected : public Object {
public:
    _ParserError_operatorExpected(Position* position);

    Position* position;
};

class _ParserError_unableToParse : public Object {
public:
    _ParserError_unableToParse(Position* position, _Array<ParserError>* errors);

    Position* position;
    _Array<ParserError>* errors;
};

class _ParserError_notAtEnd : public Object {
public:
    _ParserError_notAtEnd(Position* position);

    Position* position;
};
enum _ParserErrorCode {
    _ParserErrorCode_identifierExpected = 1,
    _ParserErrorCode_literalExpected,
    _ParserErrorCode_keywordExpected,
    _ParserErrorCode_punctuationExpected,
    _ParserErrorCode_operatorExpected,
    _ParserErrorCode_unableToParse,
    _ParserErrorCode_notAtEnd,
};

class ParserError : public Object {
public:
    ParserError(_ParserErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    ParserError(_ParserError_identifierExpected* identifierExpected)
    : errorCode(_ParserErrorCode_identifierExpected), errorInfo(identifierExpected) {}

    ParserError(_ParserError_literalExpected* literalExpected)
    : errorCode(_ParserErrorCode_literalExpected), errorInfo(literalExpected) {}

    ParserError(_ParserError_keywordExpected* keywordExpected)
    : errorCode(_ParserErrorCode_keywordExpected), errorInfo(keywordExpected) {}

    ParserError(_ParserError_punctuationExpected* punctuationExpected)
    : errorCode(_ParserErrorCode_punctuationExpected), errorInfo(punctuationExpected) {}

    ParserError(_ParserError_operatorExpected* operatorExpected)
    : errorCode(_ParserErrorCode_operatorExpected), errorInfo(operatorExpected) {}

    ParserError(_ParserError_unableToParse* unableToParse)
    : errorCode(_ParserErrorCode_unableToParse), errorInfo(unableToParse) {}

    ParserError(_ParserError_notAtEnd* notAtEnd)
    : errorCode(_ParserErrorCode_notAtEnd), errorInfo(notAtEnd) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _ParserError_identifierExpected* get_identifierExpected();
    _ParserError_literalExpected* get_literalExpected();
    _ParserError_keywordExpected* get_keywordExpected();
    _ParserError_punctuationExpected* get_punctuationExpected();
    _ParserError_operatorExpected* get_operatorExpected();
    _ParserError_unableToParse* get_unableToParse();
    _ParserError_notAtEnd* get_notAtEnd();

private:
    _ParserErrorCode errorCode;
    void* errorInfo;
};

class CompilerError;

class _CompilerError_fileNotFound : public Object {
public:
    _CompilerError_fileNotFound(string* file);

    string* file;
};

class _CompilerError_unableToReadFile : public Object {
public:
    _CompilerError_unableToReadFile(string* file, FileError* error);

    string* file;
    FileError* error;
};

class _CompilerError_syntaxError : public Object {
public:
    _CompilerError_syntaxError(ParserError* error);

    ParserError* error;
};

class _CompilerError_unableToCreateOutputDirectory : public Object {
public:
    _CompilerError_unableToCreateOutputDirectory(string* directory, DirectoryError* error);

    string* directory;
    DirectoryError* error;
};
enum _CompilerErrorCode {
    _CompilerErrorCode_fileNotFound = 1,
    _CompilerErrorCode_unableToReadFile,
    _CompilerErrorCode_syntaxError,
    _CompilerErrorCode_unableToCreateOutputDirectory,
};

class CompilerError : public Object {
public:
    CompilerError(_CompilerErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CompilerError(_CompilerError_fileNotFound* fileNotFound)
    : errorCode(_CompilerErrorCode_fileNotFound), errorInfo(fileNotFound) {}

    CompilerError(_CompilerError_unableToReadFile* unableToReadFile)
    : errorCode(_CompilerErrorCode_unableToReadFile), errorInfo(unableToReadFile) {}

    CompilerError(_CompilerError_syntaxError* syntaxError)
    : errorCode(_CompilerErrorCode_syntaxError), errorInfo(syntaxError) {}

    CompilerError(_CompilerError_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CompilerErrorCode_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _CompilerError_fileNotFound* get_fileNotFound();
    _CompilerError_unableToReadFile* get_unableToReadFile();
    _CompilerError_syntaxError* get_syntaxError();
    _CompilerError_unableToCreateOutputDirectory* get_unableToCreateOutputDirectory();

private:
    _CompilerErrorCode errorCode;
    void* errorInfo;
};

class CppError;

class _CppError_unableToCreateOutputDirectory : public Object {
public:
    _CppError_unableToCreateOutputDirectory(string* directory, DirectoryError* error);

    string* directory;
    DirectoryError* error;
};
enum _CppErrorCode {
    _CppErrorCode_unableToCreateOutputDirectory = 1,
};

class CppError : public Object {
public:
    CppError(_CppErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CppError(_CppError_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CppErrorCode_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _CppError_unableToCreateOutputDirectory* get_unableToCreateOutputDirectory();

private:
    _CppErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CompilerError__
