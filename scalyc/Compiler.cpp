#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CompilerError* Compiler::compileFiles(_Page* _ep, Array<String>& files) {
    _Region _region; _Page* _p = _region.get();

    // Map the files array to a file contents array
    Array<String>* compilationUnits = 0;
    {
        size_t _length = files.length();
        compilationUnits = new (_p) Array<String>(_length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<String, FileError> _readToStringResult = File::readToString(_p, _ep, **files[_index]);
            if (!_readToStringResult.succeeded())
                return new(_ep) CompilerError(*new(_ep) UnableToReadFile(**files[_index], *_readToStringResult.getError()));
                
            else *(*compilationUnits)[_index] = _readToStringResult.getResult();
        }
    }
    
    // Map compilationUnits to a tld array
    Array<TopLevelDeclaration>* topLevelDeclarations = 0;
    {
        size_t _length = compilationUnits->length();
        topLevelDeclarations = new (_p) Array<TopLevelDeclaration>(_length);
        for (size_t _index = 0; _index < _length; _index++) {
            _Result<TopLevelDeclaration, ParserError> _compileUnitResult = compileUnit(_p, _ep, **((*compilationUnits)[_index]));
            if (!_compileUnitResult.succeeded()) {
                return new(_ep) CompilerError(*new(_ep) SyntaxError(*_compileUnitResult.getError()));
            }
            else {
                *(*topLevelDeclarations)[_index] = _compileUnitResult.getResult();
                MyVisitor visitor;
                (**(*topLevelDeclarations)[_index]).Accept(visitor);
            }
        }
    }
    

    // Everything went fine
    return 0;
}

_Result<TopLevelDeclaration, ParserError> Compiler::compileUnit(_Page* _rp, _Page* _ep, String& text)
{
    // Make a region for the current block and get the page
    _Region _region; _Page* _p = _region.get();

    Parser& parser = *new(_p) Parser(text);
    return parser.parseTopLevelDeclaration(_rp, _ep);
}

}
