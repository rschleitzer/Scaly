#ifndef _Transpiler_h
#define _Transpiler_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace transpiler {

    Result<Void, TranspilerError> transpile_program(Page* ep, Program& program);
    Result<Void, TranspilerError> build_module(Page* ep, String path, Module& module_, String main_header, String namespace_open, String namespace_close);
    Result<Void, FileError> create_directory(Page* ep, String path);
    Result<Void, TranspilerError> build_concept(Page* ep, String path, String source, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Concept& concept);
    Result<Void, TranspilerError> build_union(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Union& union_, Vector<GenericParameter> parameters);
    bool build_type(StringBuilder& builder, Type* type);
    Result<Void, TranspilerError> build_namespace(Page* ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Namespace& namespace_);
    Result<Void, TranspilerError> build_modules(Page* ep, String path, String name, StringBuilder& header_builder, Vector<Module>& modules, String main_header, String namespace_open, String namespace_close);
    Result<Void, TranspilerError> build_symbols(Page* ep, String path, String source, String* name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Vector<Member> symbols);
    Result<Void, TranspilerError> build_structure(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Structure& structure, Vector<GenericParameter> parameters);
    Result<Void, TranspilerError> build_default_initializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Vector<Property>& properties);
    Result<Void, TranspilerError> build_default_initializer_list(Page* ep, StringBuilder& builder, bool is_generic, Vector<Property>& properties, String indent);
    Result<Void, TranspilerError> build_properties_initializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Vector<Property>& properties);
    Result<Void, TranspilerError> build_properties_initializer_list(Page* ep, StringBuilder& builder, bool is_generic, Vector<Property>& properties, String indent);
    Result<Void, TranspilerError> build_initializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Initializer* initializer);
    void build_initializer_header(StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic);
    Result<Void, TranspilerError> build_deinitializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, DeInitializer* de_initializer);
    void build_uses(StringBuilder& builder, Vector<Use>& uses);
    Result<Void, TranspilerError> build_intrinsic(Page* ep, StringBuilder& header_builder, String name);
    Result<Void, TranspilerError> build_global(Page* ep, StringBuilder& header_builder, String name, Global& global);
    Result<Void, TranspilerError> build_function(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Function& func, String* name, bool is_template, bool in_class);
    void build_output_type(StringBuilder& builder, Type* returns_, Type* throws_);
    Result<Void, TranspilerError> build_implementation(Page* ep, StringBuilder& builder, Implementation& implementation, Type* returns_, Type* throws_, String indent, bool is_initializer);
    Result<Void, TranspilerError> build_binding(Page* ep, StringBuilder& builder, Binding& binding, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_action(Page* ep, StringBuilder& builder, Action& action, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_operation(Page* ep, StringBuilder& builder, Vector<Operand>& operation, Type* returns_, Type* throws_, String* re_throw, String indent);
    void build_string(StringBuilder& builder, String string);
    Result<Void, TranspilerError> build_constant(Page* ep, StringBuilder& builder, Constant& constant);
    Result<Void, TranspilerError> build_variable(Page* ep, StringBuilder& builder, Type& type, Vector<String>* member_access);
    Result<Void, TranspilerError> build_tuple(Page* ep, StringBuilder& builder, Tuple& tuple, Vector<String>* member_access, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_matrix(Page* ep, StringBuilder& builder, Matrix& matrix, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_block(Page* ep, StringBuilder& builder, Block& block, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_if(Page* ep, StringBuilder& builder, If& if_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_match(Page* ep, StringBuilder& builder, Match& match_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_choose(Page* ep, StringBuilder& builder, Choose& choose_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_for(Page* ep, StringBuilder& builder, For& for_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_condition(Page* ep, StringBuilder& builder, Binding& binding, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_while(Page* ep, StringBuilder& builder, While& while_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_drop(Page* ep, StringBuilder& builder, Drop& drop_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_try(Page* ep, StringBuilder& builder, Try& try_, Type* returns_, Type* throws_, String* re_throw, String indent);
    void build_sizeof(StringBuilder& builder, SizeOf& sizeof_, String indent);
    void build_is(StringBuilder& builder, Is& is_, String indent);
    Result<Void, TranspilerError> build_break(Page* ep, StringBuilder& builder, Break& break_, Type* returns_, Type* throws_, String* re_throw, String indent);
    void build_continue(StringBuilder& builder, Continue& continue_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_return(Page* ep, StringBuilder& builder, Return& return_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_throw(Page* ep, StringBuilder& builder, Throw& throw_, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_statements(Page* ep, StringBuilder& builder, Vector<Statement>& statements, Type* returns_, Type* throws_, String* re_throw, String indent);
    Result<Void, TranspilerError> build_statement(Page* ep, StringBuilder& builder, Statement& statement, Type* returns_, Type* throws_, String* re_throw, String indent);
    void function_prefix(StringBuilder& builder, Function& func, bool indent, bool static_if_applicable);
    Result<Void, TranspilerError> build_operator(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Operator& operator_, String* name, bool is_template);
    bool needs_return_page(Type* type);
    bool build_input(StringBuilder& builder, Vector<Item> input, String* location, Type* returns_, Type* throws_, Lifetime lifetime);
    void full_struct_name(StringBuilder& builder, String name, Vector<GenericParameter> parameters);
    Result<Void, TranspilerError> forward_includes(Page* ep, String path, Program& program);
    Result<Void, TranspilerError> forward_include(Page* ep, StringBuilder& builder, Concept& concept);
    Result<Void, TranspilerError> forward_includes_for_namespace(Page* ep, String name, StringBuilder& builder, Namespace& namespace_);
    Result<Void, TranspilerError> forward_includes_for_modules(Page* ep, StringBuilder& builder, Vector<Module>& modules);
    Result<Void, TranspilerError> forward_includes_for_symbols(Page* ep, StringBuilder& builder, Vector<Member>& members);
    Result<Void, TranspilerError> vscode_files(Page* ep, String path, Program& program);
    void build_script_files(StringBuilder& builder, String path, Module& module_, String extension, bool include_path);
    void build_script_files_list(StringBuilder& builder, String path, Vector<Module>& modules, Vector<Member>& members, String extension, bool include_path);
    Result<Void, TranspilerError> main_file(Page* ep, String path, Program& program);
    Result<Void, TranspilerError> main_include_file(Page* ep, String path, String name);
}
#endif