#include "scalyc.h"

void scaly_Compiler_compileFiles(scalyc_Options* options) {
    scaly_Page* _p1 = scaly_Page_alloc();
    scaly_Array* files = options->files;
    scaly_Array* sources = scaly_Array_new(_p1);
    scaly_string* file = 0;
    size_t _files_length = files->_size;
    for (size_t _i = 0; _i < _files_length; _i++) {
        file = (scaly_string*)scaly_Array_elementAt(files, _i);
        {
            _scaly_File_Result _source_result = scaly_File_readToString(scaly_Page_getPage(sources), _p1, file);
            scaly_string* source = 0;
            if (!_source_result.errorCode) {
                source = (scaly_string*)_source_result.payload;
            }
            else if (_source_result.errorCode == _scaly_File_Result_ErrorCode_noSuchFileOrDirectory) {
                {
                    scaly_Page* _p2 = scaly_Page_alloc();
                    scaly_VarString* msg = scaly_VarString_fromRawString(_p2, "Can't read file: ");
                    scaly_VarString_appendString(msg, file);
                    scaly_VarString_appendRawString(msg, "\n");
                    scaly_string* message = scaly_string_fromVarString(_p2, msg);
                    _scaly_File_Result _print_error = printString(_p2, message);
                    if (_print_error.errorCode) { switch (_print_error.errorCode) {
                        default: {
                        scaly_Page_free(); // _p2
                        scaly_Page_free(); // _p1
                        return;
                        }
                    } }
                    scaly_Page_free(); // _p2
                    scaly_Page_free(); // _p1
                    return;
                }
            }
            scaly_Array_push(sources, source);
        }
    }
    scaly_Array* modules = scaly_Array_new(_p1);
    size_t index = 0;
    scaly_string* source = 0;
    size_t _sources_length = sources->_size;
    for (size_t _i = 0; _i < _sources_length; _i++) {
        source = (scaly_string*)(*scaly_Array_elementAt(sources, _i));
        {
            scaly_string* moduleName = Path::getFileNameWithoutExtension(modules->_getPage(), (scaly_string*)(*scaly_Array_elementAt(files, index)));
            auto _module_result = parseUnit(modules->_getPage(), _p, moduleName, source);
            Module* module = nullptr;
            if (_module_result.succeeded()) {
                module = _module_result.getResult();
            }
            else if (_module_result._getErrorCode() == _CompilerErrorCode_parser) {
                size_t line = _module_result.getError()->get_parser()->line;
                size_t column = _module_result.getError()->get_parser()->column;
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
            modules->push(module);
            index++;
        }
    }
    Program* program = new(_p) Program(options->outputName, new(_p) _Array<Module>(modules));
    Module* item = nullptr;
    size_t _modules_length = modules->length();
    for (size_t _i = 0; _i < _modules_length; _i++) {
        item = *(*modules)[_i];
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

_Result<Module, CompilerError> Compiler::parseUnit(_Page* _rp, _Page* _ep, string* moduleName, string* text) {
    _Region _region; _Page* _p = _region.get();
    Parser* parser = new(_p) Parser(moduleName, text);
    auto _module_result = parser->parseModule(_rp, _ep);
    Module* module = nullptr;
    if (_module_result.succeeded()) {
        module = _module_result.getResult();
    }
    else if (_module_result._getErrorCode() == _ParserErrorCode_syntax) {
        size_t line = _module_result.getError()->get_syntax()->line;
        size_t column = _module_result.getError()->get_syntax()->column;
        {
            return _Result<Module, CompilerError>(new(_ep) CompilerError(new(_ep) _CompilerError_parser(line, column)));
        }
    }
    return module;
}
