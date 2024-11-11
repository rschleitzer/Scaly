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
    Result<Item, ModelError> handle_item(Page* rp, Page* ep, bool private_, ItemSyntax& item, String file);
    Result<Vector<Item>, ModelError> handle_parameterset(Page* rp, Page* ep, ParameterSetSyntax& parameterSetSyntax, String file);
    Result<DeInitializer*, ModelError> handle_body(Page* rp, Page* ep, String name, String path, BodySyntax& body, List<Use>& uses, HashSetBuilder<String>& modules_checker, List<Module>& modules, List<Initializer>& initializers_builder, MultiMapBuilder<String, Function>& functions_builder, Array<Member>& members_builder, HashMapBuilder<String, Nameable>& symbols_builder, String file);
    Result<Structure, ModelError> handle_class(Page* rp, Page* ep, String name, String path, ClassSyntax& class_syntax, bool private_, String file);
    Result<Namespace, ModelError> handle_namespace(Page* rp, Page* ep, String name, String path, NamespaceSyntax& namespace_syntax, bool private_, String file);
    Result<Union, ModelError> handle_union(Page* rp, Page* ep, String name, String path, UnionSyntax& union_syntax, bool private_, String file);
    Result<Constant, ModelError> handle_literal(Page* rp, Page* ep, LiteralSyntax& literal, String file);
    Result<Type*, ModelError> handle_type(Page* rp, Page* ep, TypeSyntax& type_syntax, String file);
    Result<Type*, ModelError> handle_binding_annotation(Page* rp, Page* ep, BindingAnnotationSyntax& binding_annotation, String file);
    Result<Break, ModelError> handle_break(Page* rp, Page* ep, BreakSyntax& break_syntax, String file);
    Result<Continue, ModelError> handle_continue(Page* rp, Page* ep, ContinueSyntax& continue_syntax, String file);
    Result<Return, ModelError> handle_return(Page* rp, Page* ep, ReturnSyntax& return_syntax, String file);
    Result<Return, ModelError> handle_throw(Page* rp, Page* ep, ThrowSyntax& throw_syntax, String file);
    Result<Statement, ModelError> handle_command(Page* rp, Page* ep, CommandSyntax& command_syntax, String file);
    Result<Statement, ModelError> handle_statement(Page* rp, Page* ep, StatementSyntax& statement_syntax, String file);
    Result<Vector<Statement>, ModelError> handle_statements(Page* rp, Page* ep, Vector<StatementSyntax>& statements, String file);
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
    Result<While, ModelError> handle_while(Page* rp, Page* ep, WhileSyntax& while_syntax, String file);
    Result<Try, ModelError> handle_try(Page* rp, Page* ep, TrySyntax& try_syntax, String file);
    Result<SizeOf, ModelError> handle_size_of(Page* rp, Page* ep, SizeOfSyntax& size_of_syntax, String file);
    Result<Expression, ModelError> handle_expression(Page* rp, Page* ep, ExpressionSyntax& expression, String file);
    Result<Operand, ModelError> handle_operand(Page* rp, Page* ep, OperandSyntax& operand, String file);
    Result<Vector<Operand>, ModelError> handle_operands(Page* rp, Page* ep, Vector<OperandSyntax>* operands, String file);
    Result<Vector<Operand>, ModelError> handle_operation(Page* rp, Page* ep, OperationSyntax& operation, String file);
    Result<GenericParameter, ModelError> handle_generic_parameter(Page* rp, Page* ep, GenericParameterSyntax& generic_parameter, String file);
    Result<Use, ModelError> handle_use(Page* rp, Page* ep, UseSyntax& use_syntax);
    Result<Concept, ModelError> handle_definition(Page* rp, Page* ep, String path, DefinitionSyntax& definition, bool private_, String file);
    Result<Action, ModelError> handle_action(Page* rp, Page* ep, ActionSyntax& action, String file);
    Result<Function, ModelError> build_function(Page* rp, Page* ep, size_t start, size_t end, TargetSyntax& target_syntax, bool private_, bool pure, String file);
    Result<Initializer, ModelError> handle_initializer(Page* rp, Page* ep, InitSyntax& init_syntax, bool private_, String file);
    Result<DeInitializer*, ModelError> handle_deinitializer(Page* rp, Page* ep, DeInitSyntax& de_init_syntax, String file);
    Result<Operator, ModelError> handle_operator(Page* rp, Page* ep, OperatorSyntax& operator_syntax, bool private_, String file);
    Result<Module, ModelError> build_module(Page* rp, Page* ep, String path, String file_name, String name, FileSyntax& file_syntax, bool private_);
    Result<Module, ModelError> build_referenced_module(Page* rp, Page* ep, String path, String name, bool private_);
    Result<Module, ModelError> handle_module(Page* rp, Page* ep, String path, ModuleSyntax& module_syntax, bool private_);
    Result<Module, ModelError> handle_package(Page* rp, Page* ep, String path, PackageSyntax& package_syntax);
    Result<Program, ModelError> build_program(Page* rp, Page* ep, String file_name, String program_name);
}
#endif