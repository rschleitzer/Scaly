#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long OptionsError::_getErrorCode() {
    return (long)errorCode;
}

void* OptionsError::_getErrorInfo() {
    return errorInfo;
}

_OptionsError_invalidOption::_OptionsError_invalidOption(string* option) 
: option(option) { }

_OptionsError_invalidOption* OptionsError::get_invalidOption() {
    return (_OptionsError_invalidOption*)errorInfo;
}

_OptionsError_unknownOption::_OptionsError_unknownOption(string* option) 
: option(option) { }

_OptionsError_unknownOption* OptionsError::get_unknownOption() {
    return (_OptionsError_unknownOption*)errorInfo;
}

long ParserError::_getErrorCode() {
    return (long)errorCode;
}

void* ParserError::_getErrorInfo() {
    return errorInfo;
}

_ParserError_identifierExpected::_ParserError_identifierExpected(Position* position) 
: position(position) { }

_ParserError_identifierExpected* ParserError::get_identifierExpected() {
    return (_ParserError_identifierExpected*)errorInfo;
}

_ParserError_literalExpected::_ParserError_literalExpected(Position* position) 
: position(position) { }

_ParserError_literalExpected* ParserError::get_literalExpected() {
    return (_ParserError_literalExpected*)errorInfo;
}

_ParserError_keywordExpected::_ParserError_keywordExpected(Position* position, string* keyword) 
: position(position), keyword(keyword) { }

_ParserError_keywordExpected* ParserError::get_keywordExpected() {
    return (_ParserError_keywordExpected*)errorInfo;
}

_ParserError_punctuationExpected::_ParserError_punctuationExpected(Position* position, string* punctuation) 
: position(position), punctuation(punctuation) { }

_ParserError_punctuationExpected* ParserError::get_punctuationExpected() {
    return (_ParserError_punctuationExpected*)errorInfo;
}

_ParserError_operatorExpected::_ParserError_operatorExpected(Position* position) 
: position(position) { }

_ParserError_operatorExpected* ParserError::get_operatorExpected() {
    return (_ParserError_operatorExpected*)errorInfo;
}

_ParserError_unableToParse::_ParserError_unableToParse(Position* position, _Array<ParserError>* errors) 
: position(position), errors(errors) { }

_ParserError_unableToParse* ParserError::get_unableToParse() {
    return (_ParserError_unableToParse*)errorInfo;
}

_ParserError_notAtEnd::_ParserError_notAtEnd(Position* position) 
: position(position) { }

_ParserError_notAtEnd* ParserError::get_notAtEnd() {
    return (_ParserError_notAtEnd*)errorInfo;
}

long CompilerError::_getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::_getErrorInfo() {
    return errorInfo;
}

_CompilerError_fileNotFound::_CompilerError_fileNotFound(string* file) 
: file(file) { }

_CompilerError_fileNotFound* CompilerError::get_fileNotFound() {
    return (_CompilerError_fileNotFound*)errorInfo;
}

_CompilerError_unableToReadFile::_CompilerError_unableToReadFile(string* file, FileError* error) 
: file(file), error(error) { }

_CompilerError_unableToReadFile* CompilerError::get_unableToReadFile() {
    return (_CompilerError_unableToReadFile*)errorInfo;
}

_CompilerError_syntaxError::_CompilerError_syntaxError(ParserError* error) 
: error(error) { }

_CompilerError_syntaxError* CompilerError::get_syntaxError() {
    return (_CompilerError_syntaxError*)errorInfo;
}

_CompilerError_unableToCreateOutputDirectory::_CompilerError_unableToCreateOutputDirectory(string* directory, DirectoryError* error) 
: directory(directory), error(error) { }

_CompilerError_unableToCreateOutputDirectory* CompilerError::get_unableToCreateOutputDirectory() {
    return (_CompilerError_unableToCreateOutputDirectory*)errorInfo;
}

long CppError::_getErrorCode() {
    return (long)errorCode;
}

void* CppError::_getErrorInfo() {
    return errorInfo;
}

_CppError_unableToCreateOutputDirectory::_CppError_unableToCreateOutputDirectory(string* directory, DirectoryError* error) 
: directory(directory), error(error) { }

_CppError_unableToCreateOutputDirectory* CppError::get_unableToCreateOutputDirectory() {
    return (_CppError_unableToCreateOutputDirectory*)errorInfo;
}


}
