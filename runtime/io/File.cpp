#include <errno.h>
#include <cstdio>

namespace scaly {
namespace io {

using namespace scaly::memory;

struct UnknownFileError {
    UnknownFileError(String file_name) : file_name(file_name) {}
    String file_name;
};

struct NoSuchFileOrDirectoryError {
    NoSuchFileOrDirectoryError(String file_name) : file_name(file_name) {}
    String file_name;
};

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

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r) StringBuilder();
        switch (_tag) {
            case Unknown:
                message_builder.append_string(String(_r.get_page(), "An unknown file error occurred with the file "));
                message_builder.append_string(_Unknown.file_name);
                message_builder.append_string(String(_r.get_page(), "."));
            break;
            case NoSuchFileOrDirectory:
                message_builder.append_string(String(_r.get_page(), "A file or directory with the name "));
                message_builder.append_string(_NoSuchFileOrDirectory.file_name);
                message_builder.append_string(String(_r.get_page(), "does not exist."));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

struct File {
    static Result<String, FileError> read_to_string(Page* _rp, Page *_ep, const String& path) {
        Region _r;
        FILE* file = fopen(path.to_c_string(_r.get_page()), "rb");
        if (!file) {
            switch (errno) {
                case ENOENT: return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(NoSuchFileOrDirectoryError(String(_ep, path))) };
                default: return Result<String, FileError> { ._tag =  Result<String, FileError>::Error, ._Error = FileError(UnknownFileError(String(_ep, path))) }; 
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

    static FileError* write_from_string(Page *_ep, const String& path, const String& contents) {
        Region _r;
        FILE* file = fopen(path.to_c_string(_r.get_page()), "wb");
        if (!file) {
            switch (errno) {
                case ENOENT: return new(alignof(FileError), _ep) FileError(NoSuchFileOrDirectoryError(String(_ep, path)));
                default: return new(alignof(FileError), _ep) FileError(UnknownFileError(String(_ep, path)));
            };
        }

        fwrite(contents.to_c_string(_r.get_page()), 1, contents.get_length(), file);
        fclose (file);
        return nullptr;
    }
};
}
}