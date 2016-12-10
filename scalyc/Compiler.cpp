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
                {
                    _Region _region; _Page* _p = _region.get();
                    VarString* msg = new(_p) VarString("Can't read file: ");
                    msg->append(file);
                    msg->append("\n");
                    string* message = new(_p) string(msg);
                    auto _print_error = print(_p, message);
                    if (_print_error) { switch (_print_error->_getErrorCode()) {
                        default: {
                        return;
                        }
                    } }
                    return;
                }
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
            auto _compilationUnit_result = parseUnit(compilationUnits->_getPage(), _p, moduleName, source);
            CompilationUnit* compilationUnit = nullptr;
            if (_compilationUnit_result.succeeded()) {
                compilationUnit = _compilationUnit_result.getResult();
            }
            else if (_compilationUnit_result._getErrorCode() == _CompilerErrorCode_parser) {
                size_t line = _compilationUnit_result.getError()->get_parser()->line;
                size_t column = _compilationUnit_result.getError()->get_parser()->column;
                {
                    _Region _region; _Page* _p = _region.get();
                    VarString* msg = new(_p) VarString("Syntax error in ");
                    msg->append(*(*files)[index]);
                    msg->append(" at ");
                    string* lineString = Number::toString(_p, line);
                    msg->append(lineString);
                    msg->append(", ");
                    string* columnString = Number::toString(_p, column);
                    msg->append(columnString);
                    msg->append("\n");
                    string* message = new(_p) string(msg);
                    auto _print_error = print(_p, message);
                    if (_print_error) { switch (_print_error->_getErrorCode()) {
                        default: {
                        return;
                        }
                    } }
                    return;
                }
            }
            compilationUnits->push(compilationUnit);
            index++;
        }
    }
    Program* program = new(_p) Program(options->outputName, new(_p) _Array<CompilationUnit>(compilationUnits));
    CompilationUnit* item = nullptr;
    size_t _compilationUnits_length = compilationUnits->length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        item = *(*compilationUnits)[_i];
        item->parent = program;
    }
    HeaderVisitor* headerVisitor = new(_p) HeaderVisitor(options->directory);
    program->accept(headerVisitor);
    SourceVisitor* sourceVisitor = new(_p) SourceVisitor(options->directory);
    program->accept(sourceVisitor);
    ModelVisitor* modelVisitor = new(_p) ModelVisitor();
    program->accept(modelVisitor);
    Model* model = modelVisitor->model;
    VarString* msg = new(_p) VarString(model->name);
    msg->append("\n");
    string* message = new(_p) string(msg);
    auto _print_error = print(_p, message);
    if (_print_error) { switch (_print_error->_getErrorCode()) {
        default: {
        return;
        }
    } }
}

_Result<CompilationUnit, CompilerError> Compiler::parseUnit(_Page* _rp, _Page* _ep, string* moduleName, string* text) {
    _Region _region; _Page* _p = _region.get();
    Parser* parser = new(_p) Parser(moduleName, text);
    auto _compilationUnit_result = parser->parseCompilationUnit(_rp, _ep);
    CompilationUnit* compilationUnit = nullptr;
    if (_compilationUnit_result.succeeded()) {
        compilationUnit = _compilationUnit_result.getResult();
    }
    else if (_compilationUnit_result._getErrorCode() == _ParserErrorCode_syntax) {
        size_t line = _compilationUnit_result.getError()->get_syntax()->line;
        size_t column = _compilationUnit_result.getError()->get_syntax()->column;
        {
            return _Result<CompilationUnit, CompilerError>(new(_ep) CompilerError(new(_ep) _CompilerError_parser(line, column)));
        }
    }
    return compilationUnit;
}


}
