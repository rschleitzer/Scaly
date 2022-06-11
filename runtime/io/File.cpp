#include <errno.h>
#include <cstdio>

namespace scaly {
namespace io {

using namespace scaly::memory;

    
struct UnknownFileError {};
struct NoSuchFileOrDirectoryError {};

struct FileError : Object {
    FileError(UnknownFileError _UnknownFileError) : _tag(Unknown) { _Unknown = _UnknownFileError; }
    FileError(NoSuchFileOrDirectoryError _NoSuchFileOrDirectoryError) : _tag(NoSuchFileOrDirectory) { _NoSuchFileOrDirectory = _NoSuchFileOrDirectoryError; }
    enum {
        Unknown,
        NoSuchFileOrDirectory,
    } _tag;
    union {
        UnknownFileError _Unknown;
        NoSuchFileOrDirectoryError _NoSuchFileOrDirectory;
    };
};

struct File {
    static Result<String, FileError> read_to_string(Region& _pr, Page* _rp, Page *_ep, const String& path) {
        Region _r(_pr);
        FILE* file = fopen(path.to_c_string(_r.page), "rb");
        if (!file) {
            switch (errno) {
                case ENOENT: return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(NoSuchFileOrDirectoryError()) };
                default: return Result<String, FileError> { ._tag =  Result<String, FileError>::Error, ._Error = FileError(UnknownFileError()) }; 
            }
        }
        
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        rewind(file);
        String ret = String(_rp, (size_t)size); 
        char* buffer = ret.get_buffer();
        fread (buffer, 1, size, file);
        fclose (file);
        
        return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = ret };
    }

    static FileError* write_from_string(Region& _pr, Page *_ep, const String& path, const String& contents) {
        Region _r(_pr);
        FILE* file = fopen(path.to_c_string(_r.page), "wb");
        if (!file) {
            switch (errno) {
                case ENOENT: return new(alignof(FileError), _ep) FileError(NoSuchFileOrDirectoryError());
                default: return new(alignof(FileError), _ep) FileError(UnknownFileError());
            };
        }

        fwrite(contents.to_c_string(_r.page), 1, contents.get_length(), file);
        fclose (file);
        return nullptr;
    }
};
}
}