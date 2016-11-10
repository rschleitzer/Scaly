#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CompilerError* Compiler::compileFiles(_Page* _ep, Options* options) {
    _Region _region; _Page* _p = _region.get();
    _Vector<String>* files = options->files;
    _Array<String>* sources = new(_p) _Array<String>();
    String* file = nullptr;
    size_t _files_length = files->length();
    for (size_t _i = 0; _i < _files_length; _i++) {
        file = *(*files)[_i];
        {
            auto _source_result = File::readToString(_p, _ep, file);
            String* source = nullptr;
            if (_source_result.succeeded()) {
                source = _source_result.getResult();
            }
            else if (_source_result.getErrorCode() == _FileErrorCode_noSuchFileOrDirectory) {
                return new(_ep) CompilerError(new(_ep) _CompilerError_fileNotFound(file));
            }
            else {
                auto error = _source_result.getError();
                return new(_ep) CompilerError(new(_ep) _CompilerError_unableToReadFile(file, error));
            }
            sources->push(source);
        }
    }
    _Array<CompilationUnit>* compilationUnits = new(_p) _Array<CompilationUnit>();
    size_t index = 0;
    String* source = nullptr;
    size_t _sources_length = sources->length();
    for (size_t _i = 0; _i < _sources_length; _i++) {
        source = *(*sources)[_i];
        {
            auto _compilationUnit_result = parseUnit(_p, _ep, *(*files)[index], source);
            CompilationUnit* compilationUnit = nullptr;
            if (_compilationUnit_result.succeeded()) {
                compilationUnit = _compilationUnit_result.getResult();
            }
            else {
                auto error = _compilationUnit_result.getError();
                return new(_ep) CompilerError(new(_ep) _CompilerError_syntaxError(error));
            }
            compilationUnits->push(compilationUnit);
            index++;
        }
    }
    Program* program = new(_p) Program(options->outputName, options->directory, &_Vector<CompilationUnit>::create(_p, *(compilationUnits)));
    CompilationUnit* item = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        item = *(*compilationUnits)[_i];
        item->parent = program;
    }
    CppVisitor* visitor = new(_p) CppVisitor();
    visitor->execute(program);
    return nullptr;
}

_Result<CompilationUnit, ParserError> Compiler::parseUnit(_Page* _rp, _Page* _ep, String* fileName, String* text) {
    _Region _region; _Page* _p = _region.get();
    Parser* parser = new(_p) Parser(fileName, text);
    auto _compilationUnit_result = parser->parseCompilationUnit(_rp, _ep);
    CompilationUnit* compilationUnit = nullptr;
    if (_compilationUnit_result.succeeded()) {
        compilationUnit = _compilationUnit_result.getResult();
    }
    else {
        auto error = _compilationUnit_result.getError();
        return _Result<CompilationUnit, ParserError>(error);
    }
    return _Result<CompilationUnit, ParserError>(compilationUnit);
}


}
