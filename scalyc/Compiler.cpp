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
            else if (_source_result.getErrorCode() == _FileError_noSuchFileOrDirectory) {
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
            auto _compilationUnit_result = compileUnit(_p, _ep, *(*files)[index], source);
            if (!_compilationUnit_result.succeeded())
                return new(_ep) CompilerError(new(_ep) _CompilerError_syntaxError(_compilationUnit_result.getError()));
            CompilationUnit* compilationUnit = _compilationUnit_result.getResult();
            compilationUnits->push(compilationUnit);
            index++;
        }
    }

    Program* program = new(_p) Program(options->outputName, options->directory, &_Vector<CompilationUnit>::create(_p, *compilationUnits));
    if (compilationUnits) {
        size_t _compilationUnits_length = compilationUnits->length();
        for (size_t _i = 0; _i < _compilationUnits_length; _i++)
            (*(*compilationUnits)[_i])->parent = program;
    }

    CppVisitor& visitor = *new(_p) CppVisitor();
    CppError* cppError = visitor.execute(_p, program);
    if (cppError) {
        switch (cppError->getErrorCode()) {
            case _CppErrorCode_unableToCreateOutputDirectory:
                _CppError_unableToCreateOutputDirectory* uTCOD = cppError->get_unableToCreateOutputDirectory();
                return new(_ep) CompilerError(new(_ep) _CompilerError_unableToCreateOutputDirectory(uTCOD->directory, uTCOD->error));
        }
    }

    // Everything went fine
    return 0;
}

_Result<CompilationUnit, ParserError> Compiler::compileUnit(_Page* _rp, _Page* _ep, String* fileName, String* text)
{
    // Make a region for the current block and get the page
    _Region _region; _Page* _p = _region.get();

    Parser& parser = *new(_p) Parser(fileName, text);
    return parser.parseCompilationUnit(_rp, _ep);
}

}
