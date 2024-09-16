namespace scaly {
namespace compiler {
namespace transpiler {

using namespace scaly::io;
using namespace scaly::compiler::model;

struct NotImplemented
{
    NotImplemented() {}
};

struct TranspilerError : Object {
    TranspilerError(NotImplemented _NotImplementedModelError) : _tag(NotImplemented), _NotImplemented(_NotImplementedModelError) {}
    enum {
        NotImplemented,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
    };
};

}
}
}