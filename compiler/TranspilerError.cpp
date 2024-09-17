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
    enum {
        NotImplemented,
        OnlyFile,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct OnlyFile _OnlyFile;
    };
};

}
}
}