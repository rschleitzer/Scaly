#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


DuplicateIntrinsic::DuplicateIntrinsic(String name) : name(name) {}

DuplicateFunction::DuplicateFunction(String name) : name(name) {}

InstructionWithoutArguments::InstructionWithoutArguments(String name) : name(name) {}

UnknownInstruction::UnknownInstruction(String name) : name(name) {}

InstructionWithInvalidNumberOfArguments::InstructionWithInvalidNumberOfArguments(String name) : name(name) {}

UndefinedType::UndefinedType(String file, Span span, String name) : file(file), span(span), name(name) {}

String UndefinedType::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("The ");
    message_builder.append(name);
    message_builder.append(" type is not defined.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

ConceptExpected::ConceptExpected(String file, Span span, String name) : file(file), span(span), name(name) {}

String ConceptExpected::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("A type is not expected here.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

InvalidNumberOfArguments::InvalidNumberOfArguments(String file, Span span, String name, int expected, int provided) : file(file), span(span), name(name), expected(expected), provided(provided) {}

String InvalidNumberOfArguments::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("The function ");
    message_builder.append(name);
    message_builder.append(" expects ");
    message_builder.append(number_to_string(r.get_page(), expected));
    message_builder.append(" arguments, provided were ");
    message_builder.append(number_to_string(r.get_page(), provided));
    message_builder.append(".");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

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
                    return ni.to_string(rp);
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
            case PlannerError::UndefinedType:
            {
                auto ut = _result._UndefinedType;
                {
                    return ut.to_string(rp);
                };
                break;
            }
            case PlannerError::ConceptExpected:
            {
                auto ce = _result._ConceptExpected;
                {
                    return ce.to_string(rp);
                };
                break;
            }
            case PlannerError::InvalidNumberOfArguments:
            {
                auto ina = _result._InvalidNumberOfArguments;
                {
                    return ina.to_string(rp);
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

PlannerError::PlannerError(struct UndefinedType _UndefinedType) : _tag(UndefinedType), _UndefinedType(_UndefinedType) {}

PlannerError::PlannerError(struct ConceptExpected _ConceptExpected) : _tag(ConceptExpected), _ConceptExpected(_ConceptExpected) {}

PlannerError::PlannerError(struct InvalidNumberOfArguments _InvalidNumberOfArguments) : _tag(InvalidNumberOfArguments), _InvalidNumberOfArguments(_InvalidNumberOfArguments) {}


}
}