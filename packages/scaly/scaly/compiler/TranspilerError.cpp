#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


FeatureNotImplemented::FeatureNotImplemented(String file, Span span, String feature) : file(file), span(span), feature(feature) {}

String FeatureNotImplemented::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("The feature \"");
    message_builder.append(feature);
    message_builder.append("\" is not implemented.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

CantThrowInNonThrowingFunction::CantThrowInNonThrowingFunction(String file, Span span) : file(file), span(span) {}

String CantThrowInNonThrowingFunction::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("Impossible to throw in a non-throwing function.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

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
                    return ni.to_string(rp);
                };
                break;
            }
            case TranspilerError::FileError:
            {
                auto fe = _result._FileError;
                message_builder.append(fe.to_string(r.get_page()));
                break;
            }
            case TranspilerError::CantThrowInNonThrowingFunction:
            {
                auto ct = _result._CantThrowInNonThrowingFunction;
                {
                    return ct.to_string(rp);
                };
                break;
            }
        }
    }message_builder.append('\n');
    return message_builder.to_string(rp);
}
TranspilerError::TranspilerError(struct FeatureNotImplemented _NotImplemented) : _tag(NotImplemented), _NotImplemented(_NotImplemented) {}

TranspilerError::TranspilerError(struct FileError _FileError) : _tag(FileError), _FileError(_FileError) {}

TranspilerError::TranspilerError(struct CantThrowInNonThrowingFunction _CantThrowInNonThrowingFunction) : _tag(CantThrowInNonThrowingFunction), _CantThrowInNonThrowingFunction(_CantThrowInNonThrowingFunction) {}


}
}