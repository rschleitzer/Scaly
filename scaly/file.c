#include "scaly.h"
#include <stdio.h>
#include <errno.h>

_scaly_File_Result scaly_File_readToString(scaly_Page* _rp, scaly_Page *_ep, scaly_string* path) {
    FILE* file = fopen(scaly_string_getNativeString(path), "rb");
    if (!file) {
        _scaly_File_Result_ErrorCode fileErrorCode = _scaly_File_Result_ErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _scaly_File_Result_ErrorCode_noSuchFileOrDirectory; break;
        }
        return (_scaly_File_Result) { fileErrorCode, 0 };
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    scaly_string* ret = scaly_string_newWithLength(_rp, size);
    char* buffer = scaly_string_getNativeString(ret);
    fread (buffer, 1, size, file);
    fclose (file);

    return (_scaly_File_Result) { 0, ret };
}

_scaly_File_Result scaly_File_writeFromString(scaly_Page *_ep, scaly_VarString* path, scaly_VarString* contents) {
    FILE* file = fopen(scaly_string_getNativeString(path), "wb");
    if (!file) {
        _scaly_File_Result_ErrorCode fileErrorCode = _scaly_File_Result_ErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _scaly_File_Result_ErrorCode_noSuchFileOrDirectory; break;
        }
        return (_scaly_File_Result) { fileErrorCode, 0 };
    }

    fwrite(scaly_string_getNativeString(contents), 1, scaly_string_getLength(contents), file);
    fclose (file);
    return (_scaly_File_Result) { 0, 0 };
}

