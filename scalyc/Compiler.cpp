#include "scalyc.h"
using namespace scaly;
namespace scalyc {

void Compiler::compileFiles(Options* options) {
    _Region _region; _Page* _p = _region.get();
    _Array<string>* files = options->files;
    _Array<string>* sources = new(_p) _Array<string>();
    string* file = nullptr;
    size_t _files_length = files->length();
    for (size_t _i = 0; _i < _files_length; _i++) {
        file = *(*files)[_i];
        {
            auto _source_result = File::readToString(sources->_getPage(), _p, file);
            string* source = nullptr;
            if (_source_result.succeeded()) {
                source = _source_result.getResult();
            }
            else if (_source_result._getErrorCode() == _FileErrorCode_noSuchFileOrDirectory) {
                return;
            }
            sources->push(source);
        }
    }
    _Array<CompilationUnit>* compilationUnits = new(_p) _Array<CompilationUnit>();
    size_t index = 0;
    string* source = nullptr;
    size_t _sources_length = sources->length();
    for (size_t _i = 0; _i < _sources_length; _i++) {
        source = *(*sources)[_i];
        {
            string* moduleName = Path::getFileNameWithoutExtension(compilationUnits->_getPage(), *(*files)[index]);
            CompilationUnit* compilationUnit = parseUnit(compilationUnits->_getPage(), moduleName, source);
            if (compilationUnit == nullptr)
                return;
            compilationUnits->push(compilationUnit);
            index++;
        }
    }
    Program* program = new(_p) Program(options->outputName, options->directory, new(_p) _Array<CompilationUnit>(compilationUnits));
    CompilationUnit* item = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        item = *(*compilationUnits)[_i];
        item->parent = program;
    }
    HeaderVisitor* headerVisitor = new(_p) HeaderVisitor();
    headerVisitor->execute(program);
    SourceVisitor* sourceVisitor = new(_p) SourceVisitor();
    sourceVisitor->execute(program);
}

CompilationUnit* Compiler::parseUnit(_Page* _rp, string* moduleName, string* text) {
    _Region _region; _Page* _p = _region.get();
    Parser* parser = new(_p) Parser(moduleName, text);
    CompilationUnit* compilationUnit = parser->parseCompilationUnit(_rp);
    return compilationUnit;
}


}
