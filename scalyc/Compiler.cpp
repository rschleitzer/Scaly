#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CompilerError* Compiler::compileFiles(_Page* _ep, Options& options) {
    _Region _region; _Page* _p = _region.get();
    _Vector<_LetString>* files = options.files;
    _Vector<_LetString>* sources = 0;
    {
        size_t _length = files->length();
        sources = &_Vector<_LetString>::createUninitialized(_p, _length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<_LetString, FileError> _readToStringResult = File::readToString(_p, _ep, **(*files)[_index]);
            if (!_readToStringResult.succeeded())
                return new(_ep) CompilerError(new(_ep) _unableToReadFile(*(*files)[_index], _readToStringResult.getError()));

            *(*sources)[_index] = _readToStringResult.getResult();
        }
    }

    _Vector<CompilationUnit>* compilationUnits = 0;
    {
        size_t _length = sources->length();
        compilationUnits = &_Vector<CompilationUnit>::createUninitialized(_p, _length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<CompilationUnit, ParserError> _compileUnitResult = compileUnit(_p, _ep, *(*files)[_index], *(*sources)[_index]);
            if (!_compileUnitResult.succeeded()) {
                return new(_ep) CompilerError(new(_ep) _syntaxError(_compileUnitResult.getError()));
            }
            else {
                *(*compilationUnits)[_index] = _compileUnitResult.getResult();
            }
        }
    }

    Program* program = new(_p) Program(options.outputName, options.directory, compilationUnits);

    CppVisitor& visitor = *new(_p) CppVisitor();
    CppError* cppError = visitor.execute(program);
    if (cppError) {
        switch (cppError->getErrorCode()) {
            case _CppError_unableToCreateOutputDirectory:
                _unableToCreateOutputDirectory* uTCOD = cppError->get_unableToCreateOutputDirectory();
                return new(_ep) CompilerError(new(_ep) _CompilerError_unableToCreateOutputDirectory(uTCOD->directory, uTCOD->error));
        }
    }

    // Everything went fine
    return 0;
}

_Result<CompilationUnit, ParserError> Compiler::compileUnit(_Page* _rp, _Page* _ep, _LetString* fileName, _LetString* text)
{
    // Make a region for the current block and get the page
    _Region _region; _Page* _p = _region.get();

    Parser& parser = *new(_p) Parser(fileName, text);
    return parser.parseCompilationUnit(_rp, _ep);
}

}
