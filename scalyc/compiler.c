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
}
