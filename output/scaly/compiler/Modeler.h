#ifndef _Modeler_h
#define _Modeler_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace model {

    Result<ProgramSyntax, ParserError> parse_program(Page* rp, Page* ep, String program);
    Result<Property, ModelError> handle_property(Page* rp, Page* ep, bool private_, PropertySyntax& property, String file);
    Result<Constant, ModelError> handle_literal(Page* rp, Page* ep, LiteralSyntax& literal, String file);
    Result<Type*, ModelError> handle_type(Page* rp, Page* ep, TypeSyntax& type_syntax, String file);
    Result<Component, ModelError> handle_component(Page* rp, Page* ep, ComponentSyntax& component, String file);
    Result<Tuple, ModelError> handle_object(Page* rp, Page* ep, ObjectSyntax& object, String file);
    Result<Matrix, ModelError> handle_vector(Page* rp, Page* ep, VectorSyntax& vector, String file);
    Result<Model, ModelError> handle_model(Page* rp, Page* ep, ModelSyntax& model, String file);
    Result<Attribute, ModelError> handle_attribute(Page* rp, Page* ep, AttributeSyntax& attribute, String file);
    Result<Operand, ModelError> handle_operand(Page* rp, Page* ep, OperandSyntax& operand, String file);
    Result<Vector<Operand>, ModelError> handle_operands(Page* rp, Page* ep, Vector<OperandSyntax>* operands, String file);
}
#endif