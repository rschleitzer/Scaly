#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CompilerError* Compiler::compileFiles(_Page* _ep, Options& options) {
    _Region _region; _Page* _p = _region.get();
    Array<String>* files = options.files;
    Array<String>* sources = 0;
    {
        size_t _length = files->length();
        sources = new (_p) Array<String>(_length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<String, FileError> _readToStringResult = File::readToString(_p, _ep, **(*files)[_index]);
            if (!_readToStringResult.succeeded())
                return new(_ep) CompilerError(*new(_ep) UnableToReadFile(**(*files)[_index], *_readToStringResult.getError()));
                
            else *(*sources)[_index] = _readToStringResult.getResult();
        }
    }
    
    Array<CompilationUnit>* compilationUnits = 0;
    {
        size_t _length = sources->length();
        compilationUnits = new (_p) Array<CompilationUnit>(_length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<CompilationUnit, ParserError> _compileUnitResult = compileUnit(_p, _ep, *(*files)[_index], **(*sources)[_index]);
            if (!_compileUnitResult.succeeded()) {
                return new(_ep) CompilerError(*new(_ep) SyntaxError(*_compileUnitResult.getError()));
            }
            else {
                *(*compilationUnits)[_index] = _compileUnitResult.getResult();
            }
        }
    }
    
    Program& program = *new(_p) Program();
    program.name = options.outputName;
    program.compilationUnits = compilationUnits;
    
    CppVisitor visitor;
    program.Accept(visitor);

    // Everything went fine
    return 0;
}

_Result<CompilationUnit, ParserError> Compiler::compileUnit(_Page* _rp, _Page* _ep, String* fileName, String& text)
{
    // Make a region for the current block and get the page
    _Region _region; _Page* _p = _region.get();

    Parser& parser = *new(_p) Parser(fileName, text);
    return parser.parseCompilationUnit(_rp, _ep);
}

}
