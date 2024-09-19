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
};

}
}
}