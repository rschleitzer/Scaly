namespace scaly {
namespace compiler {
namespace transpiler {

using namespace scaly::io;
using namespace scaly::compiler::model;

struct NotImplemented
{
    NotImplemented(String expected) : expected(expected) {}
    String expected;
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
                message_builder.append("The transpiler feature \"");
                message_builder.append(_NotImplemented.expected);
                message_builder.append("\" is not implemented.");
                break;
            case OnlyFile:
                message_builder.append("Only a file can be transpiled.");
                break;
            case FileError:
                message_builder.append(_FileError.to_string(_r.get_page()));
                break;
        }
        message_builder.append('\n');
        return message_builder.to_string(_rp);     
    }
};

}
}
}