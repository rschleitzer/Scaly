#ifndef __scalyc__result__
#define __scalyc__result__

typedef enum _scalyc_Options_Result_ErrorCode _scalyc_Options_Result_ErrorCode; enum _scalyc_Options_Result_ErrorCode {
    _scalyc_Options_Result_ErrorCode_invalidOption = 1,
    _scalyc_Options_Result_ErrorCode_unknownOption,
    _scalyc_Options_Result_ErrorCode_noOutputOption,
    _scalyc_Options_Result_ErrorCode_noFilesToCompile,
};

typedef struct _scalyc_Options_Result_invalidOption _scalyc_Options_Result_invalidOption; struct _scalyc_Options_Result_invalidOption {
    scaly_string* option;
};

_scalyc_Options_Result_invalidOption* _new_scalyc_Options_Result_invalidOption(scaly_Page* _p, scaly_string* option);

typedef struct _scalyc_Options_Result_unknownOption _scalyc_Options_Result_unknownOption; struct _scalyc_Options_Result_unknownOption {
    scaly_string* option;
};

_scalyc_Options_Result_unknownOption* _new_scalyc_Options_Result_unknownOption(scaly_Page* _p, scaly_string* option);

typedef struct _scalyc_Options_Result _scalyc_Options_Result; struct _scalyc_Options_Result {
    enum _scalyc_Options_Result_ErrorCode errorCode;
    void* payload;
};

typedef enum _scalyc_Parser_Result_ErrorCode _scalyc_Parser_Result_ErrorCode; enum _scalyc_Parser_Result_ErrorCode {
    _scalyc_Parser_Result_ErrorCode_syntax = 1,
};

typedef struct _scalyc_Parser_Result_syntax _scalyc_Parser_Result_syntax; struct _scalyc_Parser_Result_syntax {
    size_t line;
    size_t column;
};

_scalyc_Parser_Result_syntax* _new_scalyc_Parser_Result_syntax(scaly_Page* _p, size_t line, size_t column);

typedef struct _scalyc_Parser_Result _scalyc_Parser_Result; struct _scalyc_Parser_Result {
    enum _scalyc_Options_Result_ErrorCode errorCode;
    void* payload;
};

typedef enum _scalyc_Compiler_Result_ErrorCode _scalyc_Compiler_Result_ErrorCode; enum _scalyc_Compiler_Result_ErrorCode {
    _scalyc_Compiler_Result_ErrorCode_parser = 1,
};

typedef struct _scalyc_Compiler_Result_syntax _scalyc_Compiler_Result_syntax; struct _scalyc_Compiler_Result_syntax {
    size_t line;
    size_t column;
};

_scalyc_Compiler_Result_syntax* _new_scalyc_Compiler_Result_syntax(scaly_Page* _p, size_t line, size_t column);

typedef struct _scalyc_Compiler_Result _scalyc_Compiler_Result; struct _scalyc_Compiler_Result {
    enum _scalyc_Compiler_Result_ErrorCode errorCode;
    void* payload;
};

#endif // __scalyc__result__
