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
    Result<Block, ModelError> handle_block(Page* rp, Page* ep, BlockSyntax& block, String file);
    Result<If, ModelError> handle_if(Page* rp, Page* ep, IfSyntax& if_syntax, String file);
    Result<Match, ModelError> handle_match(Page* rp, Page* ep, MatchSyntax& match_syntax, String file);
    Result<When, ModelError> handle_when(Page* rp, Page* ep, WhenSyntax& when_syntax, String file);
    Result<Choose, ModelError> handle_choose(Page* rp, Page* ep, ChooseSyntax& choose_syntax, String file);
    Result<For, ModelError> handle_for(Page* rp, Page* ep, ForSyntax& for_syntax, String file);
    Result<Binding, ModelError> handle_condition(Page* rp, Page* ep, ConditionSyntax& condition, String file);
}
Result<While, ModelError> handle_while(Page* rp, Page* ep, WhileSyntax& while_syntax, String file);
Result<Try, ModelError> handle_try(Page* rp, Page* ep, TrySyntax& try_syntax, String file);
Result<SizeOf, ModelError> handle_size_of(Page* rp, Page* ep, SizeOfSyntax& size_of_syntax, String file);
Result<Expression, ModelError> handle_expression(Page* rp, Page* ep, ExpressionSyntax& expression, String file);
Result<Operand, ModelError> handle_operand(Page* rp, Page* ep, OperandSyntax& operand, String file);
Result<Vector<Operand>, ModelError> handle_operands(Page* rp, Page* ep, Vector<OperandSyntax>* operands, String file);
#endif