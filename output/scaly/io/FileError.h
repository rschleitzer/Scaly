#ifndef _FileError_h
#define _FileError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
using namespace scaly::containers;

struct UnknownFileError : Object {
    String file_name;
    UnknownFileError (String file_name);
};

struct NoSuchFileOrDirectoryError : Object {
    String file_name;
    NoSuchFileOrDirectoryError (String file_name);
};

struct FileError {
    FileError(UnknownFileError _UnknownFileError);
    enum {
        Unknown,
        NoSuchFileOrDirectory,
} _tag;
    union {
        UnknownFileError _Unknown;
        NoSuchFileOrDirectoryError _NoSuchFileOrDirectory;
    };
};
#endif