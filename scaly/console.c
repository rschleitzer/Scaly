#include "scaly.h"
#include <stdio.h>
#include <errno.h>

_scaly_File_Result printRawString(scaly_Page* _ep, const char* message) {
    if (fputs(message, stdout) == EOF) {
        _scaly_File_Result_ErrorCode fileErrorCode = _scaly_File_Result_ErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _scaly_File_Result_ErrorCode_noSuchFileOrDirectory; break;
        }
        return (_scaly_File_Result) { fileErrorCode, 0 };
    }

    return (_scaly_File_Result) { 0, 0 };
}

_scaly_File_Result printString(scaly_Page* _ep, scaly_string* message) {
    return printRawString(_ep, scaly_string_getNativeString(message));
}
