#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


DuplicateIntrinsic::DuplicateIntrinsic(String name) : name(name) {}

DuplicateFunction::DuplicateFunction(String name) : name(name) {}

InstructionWithoutArguments::InstructionWithoutArguments(String name) : name(name) {}

UnknownInstruction::UnknownInstruction(String name) : name(name) {}

InstructionWithInvalidNumberOfArguments::InstructionWithInvalidNumberOfArguments(String name) : name(name) {}

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
                    message_builder.append("The intrinsic \"");
                    message_builder.append(di.name);
                    message_builder.append("\" is already defined.");
                };
                break;
            }
            case PlannerError::DuplicateFunction:
            {
                auto df = _result._DuplicateFunction;
                {
                    message_builder.append("The function \"");
                    message_builder.append(df.name);
                    message_builder.append("\" is already defined.");
                };
                break;
            }
            case PlannerError::InstructionWithoutArguments:
            {
                auto iwa = _result._InstructionWithoutArguments;
                {
                    message_builder.append("The instruction \"");
                    message_builder.append(iwa.name);
                    message_builder.append("\" must be provided with arguments.");
                };
                break;
            }
            case PlannerError::UnknownInstruction:
            {
                auto ui = _result._UnknownInstruction;
                {
                    message_builder.append("The instruction \"");
                    message_builder.append(ui.name);
                    message_builder.append("\" is unknown.");
                };
                break;
            }
            case PlannerError::InstructionWithInvalidNumberOfArguments:
            {
                auto ina = _result._InstructionWithInvalidNumberOfArguments;
                {
                    message_builder.append("The instruction \"");
                    message_builder.append(ina.name);
                    message_builder.append("\" is called with an invalid number of arguments.");
                };
                break;
            }
            case PlannerError::TupleComponentNamesNotSupported:
            {
                auto tcns = _result._TupleComponentNamesNotSupported;
                {
                    message_builder.append("Tuple component names are not supported.");
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

PlannerError::PlannerError(struct DuplicateFunction _DuplicateFunction) : _tag(DuplicateFunction), _DuplicateFunction(_DuplicateFunction) {}

PlannerError::PlannerError(struct InstructionWithoutArguments _InstructionWithoutArguments) : _tag(InstructionWithoutArguments), _InstructionWithoutArguments(_InstructionWithoutArguments) {}

PlannerError::PlannerError(struct UnknownInstruction _UnknownInstruction) : _tag(UnknownInstruction), _UnknownInstruction(_UnknownInstruction) {}

PlannerError::PlannerError(struct InstructionWithInvalidNumberOfArguments _InstructionWithInvalidNumberOfArguments) : _tag(InstructionWithInvalidNumberOfArguments), _InstructionWithInvalidNumberOfArguments(_InstructionWithInvalidNumberOfArguments) {}

PlannerError::PlannerError(struct TupleComponentNamesNotSupported _TupleComponentNamesNotSupported) : _tag(TupleComponentNamesNotSupported), _TupleComponentNamesNotSupported(_TupleComponentNamesNotSupported) {}


}
}