#ifndef __scaly__file__
#define __scaly__file__
    
typedef enum _scaly_File_Result_ErrorCode _scaly_File_Result_ErrorCode; enum _scaly_File_Result_ErrorCode {
    _scaly_File_Result_ErrorCode_unknownError = 1,
    _scaly_File_Result_ErrorCode_noSuchFileOrDirectory,
};

typedef struct _scaly_File_Result _scaly_File_Result; struct _scaly_File_Result {
    enum _scaly_File_Result_ErrorCode errorCode;
    void* payload;
};

_scaly_File_Result scaly_File_readToString(scaly_Page* _rp, scaly_Page *_ep, scaly_string* path);
_scaly_File_Result scaly_File_writeFromString(scaly_Page *_ep, scaly_VarString* path, scaly_VarString* contents);

#endif // __scaly__file__
