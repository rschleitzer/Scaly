#include "scalyc.h"
#include <errno.h>

scaly_Page* __CurrentPage;
scaly_Task* __CurrentTask;

int main(int argc, char **argv) {

    // Allocate the root page for the main thread
    posix_memalign((void**)&__CurrentPage, scaly_pageSize, scaly_pageSize * scaly_maxStackPages);
    if (!__CurrentPage)
        return -1;
    scaly_Page_reset(__CurrentPage);

    scaly_Task* task = scaly_Task_new(__CurrentPage);
    __CurrentTask = task;

    // Collect the arguments into a string Array
    scaly_Array* arguments = scaly_Array_newWithCapacity(__CurrentPage, argc - 1);
    for (int i = 1; i < argc; i++)
        scaly_Array_push(arguments, scaly_string_fromRawString(__CurrentPage, argv[i]));

    // Call Scaly's top-level code
    _scaly_File_Result _File_error = scalyc_main(__CurrentPage, arguments);
    int ret = 0;

    // Convert Scaly's error enum back to OS errno values
    if (_File_error.errorCode) {
        switch(_File_error.errorCode) {
            case _scaly_File_Result_ErrorCode_noSuchFileOrDirectory:
                ret = ENOENT;
                break;
            default:
                ret = -1;
                break;
        }
    }

    // Only for monitoring, debugging and stuff
    scaly_Task_dispose(__CurrentTask);

    // Give back the return code of the top-level code
	return ret;
}

_scaly_File_Result scalyc_main(scaly_Page* _ep,  scaly_Array* arguments) {
    scaly_Page* _p1 = scaly_Page_alloc();

    _scalyc_Options_Result _options_result = scalyc_Options_parseArguments(_p1, _p1, arguments);
    scalyc_Options* options = 0;
    if (!_options_result.errorCode) {
        options = _options_result.payload;
    }
    else if (_options_result.errorCode == _scalyc_Options_Result_ErrorCode_invalidOption) {
        scaly_string* option = ((_scalyc_Options_Result_invalidOption*)_options_result.payload)->option;
        {
            scaly_Page* _p2 = scaly_Page_alloc();
            scaly_VarString* msg = scaly_VarString_fromRawString(_p2, "Invalid option ");
            scaly_VarString_appendString(msg, option);
            scaly_VarString_appendRawString(msg, " .\n");
            scaly_string* message = scaly_string_fromVarString(_p2, msg);
            _scaly_File_Result _print_error = printString(_p2, message);
            if (_print_error.errorCode) { switch (_print_error.errorCode) {
                default: {
                scaly_Page_free(); // _p2
                scaly_Page_free(); // _p1
                return (_scaly_File_Result) { _print_error.errorCode, 0 };
                }
            } }
            scaly_Page_free(); // _p2
            scaly_Page_free(); // _p1
            return (_scaly_File_Result) { 0, 0 };
        }
    }
    else if (_options_result.errorCode == _scalyc_Options_Result_ErrorCode_noOutputOption) {
        {
            _scaly_File_Result _print_error = printRawString(_ep, "No output option.\n");
            if (_print_error.errorCode) { switch (_print_error.errorCode) {
                default: {
                scaly_Page_free(); // _p1
                return (_scaly_File_Result) { _print_error.errorCode, 0 };
                }
            } }
            scaly_Page_free(); // _p1
            return (_scaly_File_Result) { 0, 0 };
        }
    }
    else if (_options_result.errorCode == _scalyc_Options_Result_ErrorCode_noFilesToCompile) {
        {
            _scaly_File_Result _print_error = printRawString(_ep, "No files to compile.\n");
            if (_print_error.errorCode) { switch (_print_error.errorCode) {
                default: {
                scaly_Page_free(); // _p1
                return (_scaly_File_Result) { _print_error.errorCode, 0 };
                }
            } }
            scaly_Page_free(); // _p1
            return (_scaly_File_Result) { 0, 0 };
        }
    }
    scaly_Compiler_compileFiles(options);

    scaly_Page_free(); return (_scaly_File_Result) { 0, 0 };
}
