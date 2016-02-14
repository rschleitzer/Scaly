#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CompilerError* Compiler::compileFiles(_Page* _ep, Options& options) {
    _Region _region; _Page* _p = _region.get();
    _Array<_VarString>* files = options.files;
    _Array<_VarString>* sources = 0;
    {
        size_t _length = files->length();
        sources = new (_p) _Array<_VarString>(_length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<_VarString, FileError> _readToStringResult = File::readToString(_p, _ep, **(*files)[_index]);
            if (!_readToStringResult.succeeded())
                return new(_ep) CompilerError(*new(_ep) UnableToReadFile(**(*files)[_index], *_readToStringResult.getError()));
                
            else sources->push(_readToStringResult.getResult());
        }
    }
    
    _Array<CompilationUnit>* compilationUnits = 0;
    {
        size_t _length = sources->length();
        compilationUnits = new (_p) _Array<CompilationUnit>(_length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<CompilationUnit, ParserError> _compileUnitResult = compileUnit(_p, _ep, *(*files)[_index], **(*sources)[_index]);
            if (!_compileUnitResult.succeeded()) {
                return new(_ep) CompilerError(*new(_ep) SyntaxError(*_compileUnitResult.getError()));
            }
            else {
                compilationUnits->push(_compileUnitResult.getResult());
            }
        }
    }
    
    Program& program = *new(_p) Program();
    program.name = options.outputName;
    program.directory = options.directory;
    program.compilationUnits = compilationUnits;
    
    CppVisitor& visitor = *new(_p) CppVisitor();
    CppError* cppError = visitor.execute(program);
    if (cppError) {
        switch (cppError->getErrorCode()) {
            case _CppError_unableToCreateOutputDirectory:
                return new (_ep) CompilerError(cppError->getUnableToCreateOutputDirectory());
        }
    }
    
    // Everything went fine
    return 0;
}

_Result<CompilationUnit, ParserError> Compiler::compileUnit(_Page* _rp, _Page* _ep, _VarString* fileName, _VarString& text)
{
    // Make a region for the current block and get the page
    _Region _region; _Page* _p = _region.get();

    Parser& parser = *new(_p) Parser(fileName, text);
    return parser.parseCompilationUnit(_rp, _ep);
}

}
