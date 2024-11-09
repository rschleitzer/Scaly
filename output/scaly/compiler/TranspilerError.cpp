#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


FeatureNotImplemented::FeatureNotImplemented(String expected) : expected(expected) {}

String TranspilerError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case TranspilerError::NotImplemented:
            {
                auto ni = _result._NotImplemented;
                {
                    message_builder.append("The transpiler feature \"");
                    message_builder.append(ni.expected);
                    message_builder.append("\" is not implemented.");
                };
                break;
            }
            case TranspilerError::OnlyFile:
            {
                auto o = _result._OnlyFile;
                message_builder.append("Only a file can be transpiled.");
                break;
            }
            case TranspilerError::FileError:
            {
                auto fe = _result._FileError;
                message_builder.append(fe.to_string(r.get_page()));
                break;
            }
        }
    }message_builder.append('\n');
    return message_builder.to_string(rp);
}
TranspilerError::TranspilerError(struct FeatureNotImplemented _NotImplemented) : _tag(NotImplemented), _NotImplemented(_NotImplemented) {}

TranspilerError::TranspilerError(struct OnlyFile _OnlyFile) : _tag(OnlyFile), _OnlyFile(_OnlyFile) {}

TranspilerError::TranspilerError(struct FileError _FileError) : _tag(FileError), _FileError(_FileError) {}


}
}