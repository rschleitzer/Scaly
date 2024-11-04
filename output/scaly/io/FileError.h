#ifndef _FileError_h
#define _FileError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

struct UnknownFileError : Object {
    String file_name;

    UnknownFileError(String file_name);
};

struct NoSuchFileOrDirectoryError : Object {
    String file_name;

    NoSuchFileOrDirectoryError(String file_name);
};

struct FileError : Object {
    FileError(UnknownFileError);
    FileError(NoSuchFileOrDirectoryError);
    enum {
        Unknown,
        NoSuchFileOrDirectory,
    } _tag;
    union {
        struct UnknownFileError _Unknown;
        struct NoSuchFileOrDirectoryError _NoSuchFileOrDirectory;
    };
};
#endif