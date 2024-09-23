namespace scaly {
namespace compiler {
namespace transpiler {

using namespace scaly::io;
using namespace scaly::compiler::model;

struct NotImplemented
{
    NotImplemented() {}
};

struct OnlyFile
{
    OnlyFile() {}
};

struct TranspilerError : Object {
    TranspilerError(NotImplemented _NotImplemented) : _tag(NotImplemented), _NotImplemented(_NotImplemented) {}
    TranspilerError(OnlyFile _OnlyFile) : _tag(OnlyFile), _OnlyFile() {}
    TranspilerError(FileError _FileError) : _tag(FileError), _FileError(_FileError) {}
    enum {
        NotImplemented,
        OnlyFile,
        FileError,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct OnlyFile _OnlyFile;
        struct FileError _FileError;
    };

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        switch (_tag) {
            case NotImplemented:
                message_builder.append_string(String(_rp, "This transpiler feature is not implemented.."));
                break;
            case OnlyFile:
                message_builder.append_string(String(_rp, "Only a file can be transpiled."));
                break;
            case FileError:
                message_builder.append_string(_FileError.to_string(_rp));
                break;
        }
        return message_builder.to_string(_rp);     
    }
};

}
}
}