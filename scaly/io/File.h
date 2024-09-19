#include "../containers/Containers.cpp"

namespace scaly {
namespace io {

using namespace scaly::memory;
using namespace scaly::containers;

struct UnknownFileError {
    UnknownFileError(String file_name);
    String file_name;
};

struct NoSuchFileOrDirectoryError {
    NoSuchFileOrDirectoryError(String file_name);
    String file_name;
};

struct FileError : Object {
    FileError(UnknownFileError _UnknownFileError);
    FileError(NoSuchFileOrDirectoryError _NoSuchFileOrDirectoryError);
    enum {
        Unknown,
        NoSuchFileOrDirectory,
    } _tag;
    union {
        UnknownFileError _Unknown;
        NoSuchFileOrDirectoryError _NoSuchFileOrDirectory;
    };

    String to_string(Page* _rp);
};

struct File {
    static Result<String, FileError> read_to_string(Page* _rp, Page *_ep, const String& path);
    static FileError* write_from_string(Page *_ep, const String& path, const String& contents);
};
}
}