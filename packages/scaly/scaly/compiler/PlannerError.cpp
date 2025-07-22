#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


DuplicateIntrinsic::DuplicateIntrinsic(String name) : name(name) {}

String PlannerError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case PlannerError::NotImplemented:
            {
                auto ni = _result._NotImplemented;
                {
                    message_builder.append("The planner feature \"");
                    message_builder.append(ni.expected);
                    message_builder.append("\" is not implemented.");
                };
                break;
            }
            case PlannerError::FileError:
            {
                auto fe = _result._FileError;
                message_builder.append(fe.to_string(r.get_page()));
                break;
            }
            case PlannerError::DuplicateIntrinsic:
            {
                auto di = _result._DuplicateIntrinsic;
                {
                    message_builder.append("The planner feature \"");
                    message_builder.append(di.name);
                    message_builder.append("\" is not implemented.");
                };
                break;
            }
        }
    }message_builder.append('\n');
    return message_builder.to_string(rp);
}
PlannerError::PlannerError(struct FeatureNotImplemented _NotImplemented) : _tag(NotImplemented), _NotImplemented(_NotImplemented) {}

PlannerError::PlannerError(struct FileError _FileError) : _tag(FileError), _FileError(_FileError) {}

PlannerError::PlannerError(struct DuplicateIntrinsic _DuplicateIntrinsic) : _tag(DuplicateIntrinsic), _DuplicateIntrinsic(_DuplicateIntrinsic) {}


}
}