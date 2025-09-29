#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;

using namespace scaly::compiler::Plan;


Planner::Planner(Program& program, HashSetBuilder<String> intrinsics_builder, HashMapBuilder<String, Plan::PlanType> types_builder, HashMapBuilder<String, Plan::PlanFunction> functions_builder, List<Plan::Source> module_list) : program(program), intrinsics_builder(intrinsics_builder), types_builder(types_builder), functions_builder(functions_builder), module_list(module_list) {}

Planner::Planner(Program& program) : program(program), intrinsics_builder(HashSetBuilder<String>()), types_builder(HashMapBuilder<String, Plan::PlanType>()), functions_builder(HashMapBuilder<String, Plan::PlanFunction>()){
}

Result<Plan::PlanProgram, PlannerError> Planner::plan_program(Page* rp, Page* ep) {
    auto r = Region();
    const auto file = program.module_.file;
    auto path = Path::get_directory_name(r.get_page(), file);
    if (path.get_length()>0) {
        const auto _exists_result = Directory::exists(ep, path);
        auto exists = _exists_result._Ok;
        if (_exists_result._tag == Success::Error) {
            const auto _exists_Error = _exists_result._Error;
            switch (_exists_Error._tag) {
            default:
                return Result<Plan::PlanProgram, PlannerError>(_exists_result._Error);

            }
        };
        if (exists == false) {
            {
                const auto _void_result = Directory::create(ep, path);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Plan::PlanProgram, PlannerError>(_void_result._Error);

                    }
                }}
                ;
        };
    };
    const auto _source_result = plan_module(rp, ep, program.module_);
    auto source = _source_result._Ok;
    if (_source_result._tag == Success::Error) {
        const auto _source_Error = _source_result._Error;
        switch (_source_Error._tag) {
        default:
            return Result<Plan::PlanProgram, PlannerError>(_source_result._Error);

        }
    };
    
    auto _module__iterator = program.module_.modules.get_iterator();
    while (auto _module_ = _module__iterator.next()) {
        auto module_ = *_module_;{
            const auto _source_result = plan_module(rp, ep, module_);
            auto source = _source_result._Ok;
            if (_source_result._tag == Success::Error) {
                const auto _source_Error = _source_result._Error;
                switch (_source_Error._tag) {
                default:
                    return Result<Plan::PlanProgram, PlannerError>(_source_result._Error);

                }
            };
        }
    };
    if (program.statements.length>0) {
        {
            const auto _void_result = plan_main_function(ep, source, program);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Plan::PlanProgram, PlannerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Plan::PlanProgram, PlannerError>(Plan::PlanProgram(source, Plan::PlanModule(String(), String(), String(), String(), HashMap<String, String>(rp, HashMapBuilder<String, String>()), nullptr, HashMap<String, Vector<Metadata>>(rp, HashMapBuilder<String, Vector<Metadata>>())), HashMap<String, PlanType>(rp, types_builder), HashMap<String, LLVMType>(rp, HashMapBuilder<String, LLVMType>()), HashMap<String, PlanFunction>(rp, functions_builder), HashMap<String, GlobalVariable>(rp, HashMapBuilder<String, GlobalVariable>()), HashMap<String, Metadata>(rp, HashMapBuilder<String, Metadata>()), Vector<DebugInfo>(rp, List<DebugInfo>())));
}

Result<Void, PlannerError> Planner::plan_main_function(Page* ep, Plan::Source* source, Program& program) {
    auto r = Region();
    const auto main_name = String(get_page(), "main");
    if (functions_builder.contains(main_name)) 
        return Result<Void, PlannerError>(DuplicateFunction(String(ep, main_name)));
    List<PlanArgument>& input_list = *new (alignof(List<PlanArgument>), r.get_page()) List<PlanArgument>();
    input_list.add(Plan::PlanArgument(String(get_page(), "argc"), String(get_page(), "int32"), Vector<String>(get_page(), List<String>()), nullptr));
    input_list.add(Plan::PlanArgument(String(get_page(), "argv"), String(get_page(), "ptr"), Vector<String>(get_page(), List<String>()), nullptr));
    List<BasicBlock>& blocks = *new (alignof(List<BasicBlock>), r.get_page()) List<BasicBlock>();
    List<PlanInstruction>& instructions = *new (alignof(List<PlanInstruction>), r.get_page()) List<PlanInstruction>();
    List<String>& values = *new (alignof(List<String>), r.get_page()) List<String>();
    auto statement_iterator = program.statements.get_iterator();
    auto next_statement = statement_iterator.next();
    String* required_type = nullptr;
    HashMapBuilder<String, String>& input_environment_builder = *new (alignof(HashMapBuilder<String, String>), r.get_page()) HashMapBuilder<String, String>();
    input_environment_builder.add(String(get_page(), "argc"), String(get_page(), "int32"));
    input_environment_builder.add(String(get_page(), "argv"), String(get_page(), "ptr"));
    auto page = get_page();
    List<HashMap<String, String>>& environment = *new (alignof(List<HashMap<String, String>>), page) List<HashMap<String, String>>();
    environment.add(HashMap<String, String>(page, input_environment_builder));
    while (true) {
        auto statement = *next_statement;
        next_statement = statement_iterator.next();
        if (next_statement == nullptr) 
            required_type = new (alignof(String), r.get_page()) String(r.get_page(), "int32");
        const auto _new_instructions_result = plan_statement(get_page(), ep, program.module_.file, environment, program.module_.symbols, nullptr, statement, required_type, blocks, &instructions);
        auto new_instructions = _new_instructions_result._Ok;
        if (_new_instructions_result._tag == Success::Error) {
            const auto _new_instructions_Error = _new_instructions_result._Error;
            switch (_new_instructions_Error._tag) {
            default:
                return Result<Void, PlannerError>(_new_instructions_result._Error);

            }
        };
        instructions = *new_instructions;
        if (next_statement == nullptr) 
            break;
    };
    if ((*(instructions.get_head())).result) {
        List<String>& ret_list = *new (alignof(List<String>), r.get_page()) List<String>();
        ret_list.add(*(*(instructions.get_head())).result);
        instructions.add(Plan::PlanInstruction(nullptr, String(get_page(), "ret"), Vector<String>(get_page(), ret_list), String(get_page(), "void"), nullptr, String(get_page(), "ret"), HashMap<String, String>(get_page(), HashMapBuilder<String, String>()), false, false, String(), 0, nullptr));
    }
    else {
        instructions.add(Plan::PlanInstruction(nullptr, String(get_page(), "ret"), Vector<String>(get_page(), List<String>()), String(get_page(), "void"), nullptr, String(get_page(), "ret"), HashMap<String, String>(get_page(), HashMapBuilder<String, String>()), false, false, String(), 0, nullptr));
    };
    auto block = Plan::BasicBlock(String(get_page(), "entry"), Vector<PlanInstruction>(get_page(), instructions), Vector<String>(get_page(), List<String>()), Vector<String>(get_page(), List<String>()), true, nullptr);
    blocks.add(block);
    const auto int_name = String(get_page(), "int32");
    auto plan_function = Plan::PlanFunction(source, main_name, Vector<PlanArgument>(get_page(), input_list), int_name, Vector<BasicBlock>(get_page(), blocks), nullptr, String(get_page(), "external"), String(), Vector<String>(get_page(), List<String>()), Vector<Variable>(get_page(), List<Variable>()), false, true, String(get_page(), "default"), String());
    functions_builder.add(main_name, plan_function);
    return Result<Void, PlannerError>(Void());
}

Result<Plan::Source*, PlannerError> Planner::plan_module(Page* rp, Page* ep, Module& module_) {
    auto r = Region();
    const auto file = module_.file;
    auto path = Path::get_directory_name(r.get_page(), file);
    StringBuilder& path_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(".");
    if (path.get_length()>0) 
        path_builder.append('/');
    path_builder.append(path);
    StringBuilder& file_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(module_.name);
    file_builder.append(".scaly");
    auto page = get_page();
    auto source = new (alignof(Plan::Source), page) Plan::Source(path_builder.to_string(page), file_builder.to_string(page));
    {
        const auto _void_result = plan_symbols(ep, source, module_.file, module_.symbols, module_.members);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Plan::Source*, PlannerError>(_void_result._Error);

            }
        }}
        ;
    return Result<Plan::Source*, PlannerError>(source);
}

Result<Void, PlannerError> Planner::plan_symbols(Page* ep, Plan::Source* source, String file, HashMap<String, Nameable>& symbols, Vector<Member> members) {
    auto r = Region();
    
    auto _member_iterator = members.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Function:
                    {
                        auto func = _result._Function;
                        {
                            const auto _void_result = plan_function(ep, source, file, symbols, func);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, PlannerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Concept:
                    {
                        auto concept = _result._Concept;
                        {
                            const auto _void_result = plan_concept(ep, file, symbols, concept);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, PlannerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Operator:
                    {
                        auto op = _result._Operator;
                        return Result<Void, PlannerError>(FeatureNotImplemented(file, op.span, String(ep, "operator symbol")));
                        break;
                    }
                    case Member::Package:
                    {
                        auto p = _result._Package;
                        return Result<Void, PlannerError>(FeatureNotImplemented(file, Span(0, 0), String(ep, "package")));
                        break;
                    }
                }
            };
        }
    };
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> Planner::plan_concept(Page* ep, String file, HashMap<String, Nameable>& symbols, Concept& concept) {
    {
        auto _result = concept.definition;
        switch (_result._tag)
        {
            case Definition::Namespace:
            {
                auto ns = _result._Namespace;
                return Result<Void, PlannerError>(plan_namespace(ep, concept.name, file, ns));
                break;
            }
            case Definition::Structure:
            {
                auto structure = _result._Structure;
                return Result<Void, PlannerError>(FeatureNotImplemented(file, structure.span, String(ep, "structure")));
                break;
            }
            case Definition::Intrinsic:
            {
                auto i = _result._Intrinsic;
                return Result<Void, PlannerError>(plan_intrinsic(ep, concept.name));
                break;
            }
            case Definition::Global:
            {
                auto g = _result._Global;
                return Result<Void, PlannerError>(FeatureNotImplemented(file, g.span, String(ep, "global")));
                break;
            }
            case Definition::Union:
            {
                auto u = _result._Union;
                return Result<Void, PlannerError>(FeatureNotImplemented(file, u.span, String(ep, "union")));
                break;
            }
            case Definition::Type:
            {
                auto t = _result._Type;
                return Result<Void, PlannerError>(plan_type_definition(ep, concept.name, t));
                break;
            }
        }
    };
}

Result<Void, PlannerError> Planner::plan_intrinsic(Page* ep, String name) {
    auto r = Region();
    if (intrinsics_builder.contains(name)) 
        return Result<Void, PlannerError>(DuplicateIntrinsic(String(ep, name)));
    intrinsics_builder.add(name);
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> Planner::plan_namespace(Page* ep, String namespace_name, String file, Namespace& ns) {
    auto r = Region();
    
    auto _member_iterator = ns.members.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Concept:
                    {
                        auto concept = _result._Concept;
                        {
                            {
                                auto _result = concept.definition;
                                switch (_result._tag)
                                {
                                    case Definition::Intrinsic:
                                    {
                                        auto i = _result._Intrinsic;
                                        {
                                            StringBuilder& qualified_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
                                            qualified_name_builder.append(namespace_name);
                                            qualified_name_builder.append('.');
                                            qualified_name_builder.append(concept.name);
                                            const auto qualified_name = qualified_name_builder.to_string(r.get_page());
                                            if (intrinsics_builder.contains(qualified_name)) 
                                                return Result<Void, PlannerError>(DuplicateIntrinsic(String(ep, qualified_name)));
                                            intrinsics_builder.add(qualified_name);
                                        };
                                        break;
                                    }
                                    default:
                                        return Result<Void, PlannerError>(FeatureNotImplemented(file, concept.span, String(ep, "Non-intrinsic namespace member")));
                                }
                            };
                        };
                        break;
                    }
                    default:
                        return Result<Void, PlannerError>(FeatureNotImplemented(file, Span(0, 0), String(ep, "Non-concept namespace member")));
                }
            };
        }
    };
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> Planner::plan_type_definition(Page* ep, String name, Type& type_def) {
    auto r = Region();
    const auto target_type_name = *type_def.name.get(0);
    const auto plan_type = Plan::PlanType(name, Vector<String>(get_page(), List<String>()));
    types_builder.add(name, plan_type);
    return Result<Void, PlannerError>(Void());
}

Result<String, PlannerError> Planner::resolve_type(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Type type) {
    if (type.name.length == 1) {
        const auto name = *type.name.get(0);
        if (intrinsics_builder.contains(name)) 
            return Result<String, PlannerError>(name);
        if (types_builder.contains(name)) {
            if (symbols.contains(name)) {
                {
                    auto _result = *symbols.get(name);
                    switch (_result._tag)
                    {
                        case Nameable::Concept:
                        {
                            auto concept = _result._Concept;
                            {
                                {
                                    auto _result = concept.definition;
                                    switch (_result._tag)
                                    {
                                        case Definition::Type:
                                        {
                                            auto t = _result._Type;
                                            {
                                                return Result<String, PlannerError>(resolve_type(rp, ep, file, symbols, t));
                                            };
                                            break;
                                        }
                                        case Definition::Intrinsic:
                                        {
                                            auto i = _result._Intrinsic;
                                            {
                                                return Result<String, PlannerError>(name);
                                            };
                                            break;
                                        }
                                        default:
                                            return Result<String, PlannerError>(FeatureNotImplemented(file, concept.span, String(ep, "Concept type other than intrinsic or type")));
                                    }
                                };
                            };
                            break;
                        }
                        default:
                            {
                            return Result<String, PlannerError>(ConceptExpected(file, type.span, name));
                        };
                    }
                };
            };
        };
        if (symbols.contains(name)) {
            {
                auto _result = *symbols.get(name);
                switch (_result._tag)
                {
                    case Nameable::Concept:
                    {
                        auto concept = _result._Concept;
                        {
                            {
                                auto _result = concept.definition;
                                switch (_result._tag)
                                {
                                    case Definition::Intrinsic:
                                    {
                                        auto i = _result._Intrinsic;
                                        {
                                            return Result<String, PlannerError>(name);
                                        };
                                        break;
                                    }
                                    default:
                                        return Result<String, PlannerError>(FeatureNotImplemented(file, concept.span, String(ep, "Concept type other than intrinsic")));
                                }
                            };
                        };
                        break;
                    }
                    default:
                        return Result<String, PlannerError>(ConceptExpected(file, type.span, name));
                }
            };
        };
        return Result<String, PlannerError>(UndefinedType(file, type.span, String(ep, name)));
    }
    else {
        return Result<String, PlannerError>(resolve_qualified_type(rp, ep, file, symbols, type.name, type.span));
    };
}

Result<String, PlannerError> Planner::resolve_qualified_type(Page* rp, Page* ep, String file, HashMap<String, Nameable>& symbols, Vector<String> name_path, Span span) {
    auto r = Region();
    StringBuilder& qualified_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    auto i = 0;
    while (i<name_path.length) {
        if (i>0) 
            qualified_name_builder.append('.');
        qualified_name_builder.append(*name_path.get(i));
        i = i+1;
    };
    const auto qualified_name = qualified_name_builder.to_string(r.get_page());
    if (intrinsics_builder.contains(qualified_name)) {
        const auto last_index = name_path.length-1;
        const auto intrinsic_name = *name_path.get(last_index);
        return Result<String, PlannerError>(String(rp, intrinsic_name));
    };
    const auto namespace_name = *name_path.get(0);
    if (symbols.contains(namespace_name) == false) 
        return Result<String, PlannerError>(UndefinedType(file, span, String(ep, namespace_name)));
    {
        auto _result = *symbols.get(namespace_name);
        switch (_result._tag)
        {
            case Nameable::Concept:
            {
                auto concept = _result._Concept;
                {
                    {
                        auto _result = concept.definition;
                        switch (_result._tag)
                        {
                            case Definition::Namespace:
                            {
                                auto ns = _result._Namespace;
                                {
                                    if (name_path.length == 2) {
                                        const auto member_name = *name_path.get(1);
                                        if (ns.symbols.contains(member_name)) {
                                            {
                                                auto _result = *ns.symbols.get(member_name);
                                                switch (_result._tag)
                                                {
                                                    case Nameable::Concept:
                                                    {
                                                        auto member_concept = _result._Concept;
                                                        {
                                                            {
                                                                auto _result = member_concept.definition;
                                                                switch (_result._tag)
                                                                {
                                                                    case Definition::Intrinsic:
                                                                    {
                                                                        auto i = _result._Intrinsic;
                                                                        {
                                                                            return Result<String, PlannerError>(String(r.get_page(), *name_path.get(name_path.length-1)));
                                                                        };
                                                                        break;
                                                                    }
                                                                    default:
                                                                        return Result<String, PlannerError>(FeatureNotImplemented(file, span, String(ep, "Non-intrinsic namespace member")));
                                                                }
                                                            };
                                                        };
                                                        break;
                                                    }
                                                    default:
                                                        return Result<String, PlannerError>(FeatureNotImplemented(file, span, String(ep, "Non-concept namespace member")));
                                                }
                                            };
                                        }
                                        else {
                                            return Result<String, PlannerError>(UndefinedType(file, span, String(ep, member_name)));
                                        };
                                    }
                                    else {
                                        return Result<String, PlannerError>(FeatureNotImplemented(file, span, String(ep, "Nested namespace paths")));
                                    };
                                };
                                break;
                            }
                            default:
                                return Result<String, PlannerError>(FeatureNotImplemented(file, span, String(ep, "Non-namespace qualified reference")));
                        }
                    };
                };
                break;
            }
            default:
                return Result<String, PlannerError>(ConceptExpected(file, span, namespace_name));
        }
    };
}

String* Planner::get_type_from_environment(String name, List<HashMap<String, String>>& environment) {
    
    auto _scope_iterator = environment.get_iterator();
    while (auto _scope = _scope_iterator.next()) {
        auto scope = *_scope;{
            if (scope.contains(name)) 
                return scope.get(name);
        }
    };
    return nullptr;
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_function_arguments(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, Function& function_, Tuple& tuple, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values) {
    if (function_.input.length != tuple.components.length) 
        return Result<List<PlanInstruction>*, PlannerError>(InvalidNumberOfArguments(file, tuple.span, String(ep, function_.name), function_.input.length, tuple.components.length));
    auto item_iterator = function_.input.get_iterator();
    
    auto _actual_argument_iterator = tuple.components.get_iterator();
    while (auto _actual_argument = _actual_argument_iterator.next()) {
        auto actual_argument = *_actual_argument;{
            auto item = *item_iterator.next();
            auto formal_argument_type = item.type;
            if (formal_argument_type == nullptr) 
                FeatureNotImplemented(file, item.span, String(ep, "Type inference"));
            const auto _required_argument_type_result = resolve_type(get_page(), ep, file, symbols, *formal_argument_type);
            auto required_argument_type = _required_argument_type_result._Ok;
            if (_required_argument_type_result._tag == Success::Error) {
                const auto _required_argument_type_Error = _required_argument_type_result._Error;
                switch (_required_argument_type_Error._tag) {
                default:
                    return Result<List<PlanInstruction>*, PlannerError>(_required_argument_type_result._Error);

                }
            };
            if (actual_argument.name) 
                return Result<List<PlanInstruction>*, PlannerError>(TupleComponentNamesNotSupported());
            {
                const auto _void_result = plan_operation(get_page(), ep, file, environment, symbols, actual_argument.value, &required_argument_type, blocks, instructions, values);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<List<PlanInstruction>*, PlannerError>(_void_result._Error);

                    }
                }}
                ;
            const auto last_instruction = (*instructions).get_head();
            if (last_instruction) 
                values.add(*(*last_instruction).result);
        }
    };
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

String Planner::allocate_value_name(Page* rp, List<BasicBlock>& blocks, List<PlanInstruction>& instructions) {
    auto r = Region();
    StringBuilder& value_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    value_name_builder.append('v');
    value_name_builder.append(number_to_string(r.get_page(), blocks.count()));
    value_name_builder.append('_');
    value_name_builder.append(number_to_string(r.get_page(), instructions.count()));
    return String(value_name_builder.to_string(rp));
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_instruction_call(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Function& function_, Tuple& tuple, List<BasicBlock>& blocks, List<PlanInstruction>* instructions) {
    auto r = Region();
    List<String>& values_list = *new (alignof(List<String>), r.get_page()) List<String>();
    {
        const auto _void_result = plan_function_arguments(get_page(), ep, file, environment, symbols, function_, tuple, blocks, instructions, values_list);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<List<PlanInstruction>*, PlannerError>(_void_result._Error);

            }
        }}
        ;
    auto values = Vector<String>(get_page(), values_list);
    auto page = get_page();
    const auto value = allocate_value_name(get_page(), blocks, *instructions);
    (*instructions).add(Plan::PlanInstruction(new (alignof(String), page) String(get_page(), value), function_.name, values, String(get_page(), "void"), nullptr, String(get_page(), "call"), HashMap<String, String>(get_page(), HashMapBuilder<String, String>()), false, false, String(), 0, nullptr));
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_type_expression(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Type& type, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values) {
    if (type.name.length>1) 
        return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, type.span, String(ep, "Qualified type name")));
    if (type.generics) 
        return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, type.span, String(ep, "Generic type")));
    {
        auto _result = type.lifetime;
        switch (_result._tag)
        {
            case Lifetime::Unspecified:
            {
                auto unspecified = _result._Unspecified;
                {
                };
                break;
            }
            case Lifetime::Call:
            {
                auto call = _result._Call;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, call.span, String(ep, "Call lifetime")));
                break;
            }
            case Lifetime::Local:
            {
                auto local = _result._Local;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, local.span, String(ep, "Local lifetime")));
                break;
            }
            case Lifetime::Reference:
            {
                auto reference = _result._Reference;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, reference.span, String(ep, "Reference lifetime")));
                break;
            }
            case Lifetime::Thrown:
            {
                auto thrown = _result._Thrown;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, thrown.span, String(ep, "Thrown lifetime")));
                break;
            }
        }
    };
    const auto name = *type.name.get(0);
    if (symbols.contains(name)) {
        {
            auto _result = *symbols.get(name);
            switch (_result._tag)
            {
                case Nameable::Package:
                {
                    auto modules = _result._Package;
                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, Span(0, 0), String(ep, "Package reference")));
                    break;
                }
                case Nameable::Concept:
                {
                    auto concept = _result._Concept;
                    {
                        {
                            auto _result = concept.definition;
                            switch (_result._tag)
                            {
                                case Definition::Intrinsic:
                                {
                                    auto i = _result._Intrinsic;
                                    return Result<List<PlanInstruction>*, PlannerError>(UndefinedTypeOrValue(file, type.span, String(ep, name)));
                                    break;
                                }
                                default:
                                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, concept.span, String(ep, "Non-intrinsic concept reference")));
                            }
                        };
                    };
                    break;
                }
                case Nameable::Operator:
                {
                    auto operator_ = _result._Operator;
                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, operator_.span, String(ep, "Operator reference")));
                    break;
                }
                case Nameable::Functions:
                {
                    auto functions = _result._Functions;
                    {
                        if (functions.length>1) 
                            return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, (*(functions.get(0))).span, String(ep, "Overloaded functions")));
                        auto function_ = *functions.get(0);
                        if (required_type) {
                            if (function_.returns_ == nullptr) 
                                return Result<List<PlanInstruction>*, PlannerError>(TypeMismatch(file, function_.span, *required_type, String(ep, "void")));
                            const auto function_return_type = *function_.returns_;
                            const auto _resolved_function_return_type_result = resolve_type(get_page(), ep, file, symbols, function_return_type);
                            auto resolved_function_return_type = _resolved_function_return_type_result._Ok;
                            if (_resolved_function_return_type_result._tag == Success::Error) {
                                const auto _resolved_function_return_type_Error = _resolved_function_return_type_result._Error;
                                switch (_resolved_function_return_type_Error._tag) {
                                default:
                                    return Result<List<PlanInstruction>*, PlannerError>(_resolved_function_return_type_result._Error);

                                }
                            };
                            if (resolved_function_return_type.equals(*required_type) == false) 
                                return Result<List<PlanInstruction>*, PlannerError>(TypeMismatch(file, function_return_type.span, *required_type, resolved_function_return_type));
                        };
                        {
                            auto _result = function_.implementation;
                            switch (_result._tag)
                            {
                                case Implementation::Action:
                                {
                                    auto action = _result._Action;
                                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, function_.span, String(ep, "Internal Function call")));
                                    break;
                                }
                                case Implementation::Extern:
                                {
                                    auto extern_ = _result._Extern;
                                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, extern_.span, String(ep, "External Function call")));
                                    break;
                                }
                                case Implementation::Instruction:
                                {
                                    auto instruction_ = _result._Instruction;
                                    {
                                        if (operation == nullptr) 
                                            return Result<List<PlanInstruction>*, PlannerError>(InstructionWithoutArguments(String(ep, name)));
                                        auto next_operand = (*operation).next();
                                        if (next_operand == nullptr) 
                                            return Result<List<PlanInstruction>*, PlannerError>(InstructionWithoutArguments(String(ep, name)));
                                        {
                                            auto _result = (*next_operand).expression;
                                            switch (_result._tag)
                                            {
                                                case Expression::Tuple:
                                                {
                                                    auto tuple = _result._Tuple;
                                                    {
                                                        return Result<List<PlanInstruction>*, PlannerError>(plan_instruction_call(get_page(), ep, file, environment, symbols, operation, function_, tuple, blocks, instructions));
                                                    };
                                                    break;
                                                }
                                                default:
                                                    return Result<List<PlanInstruction>*, PlannerError>(InstructionWithoutArguments(String(ep, name)));
                                            }
                                        };
                                    };
                                    break;
                                }
                                case Implementation::Intrinsic:
                                {
                                    auto intrinsic_ = _result._Intrinsic;
                                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, intrinsic_.span, String(ep, "Intrinsic call")));
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Nameable::Property:
                {
                    auto property = _result._Property;
                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, property.span, String(ep, "Property reference")));
                    break;
                }
                case Nameable::Variant:
                {
                    auto variant = _result._Variant;
                    return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, variant.span, String(ep, "Variant reference")));
                    break;
                }
            }
        };
    }
    else {
        auto actual_type = get_type_from_environment(name, environment);
        if (actual_type == nullptr) 
            return Result<List<PlanInstruction>*, PlannerError>(UndefinedTypeOrValue(file, type.span, String(ep, name)));
        if (required_type) {
            if ((*required_type).equals(*actual_type) == false) 
                return Result<List<PlanInstruction>*, PlannerError>(TypeMismatch(file, type.span, *required_type, *actual_type));
        };
        values.add(name);
    };
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_statement(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Statement& statement, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions) {
    {
        auto _result = statement;
        switch (_result._tag)
        {
            case Statement::Action:
            {
                auto action = _result._Action;
                {
                    auto r = Region();
                    List<String>& values = *new (alignof(List<String>), r.get_page()) List<String>();
                    return Result<List<PlanInstruction>*, PlannerError>(plan_action(get_page(), ep, file, environment, symbols, operation, action, required_type, blocks, instructions, values));
                };
                break;
            }
            case Statement::Binding:
            {
                auto binding = _result._Binding;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, binding.span, String(ep, "Binding statement")));
                break;
            }
            case Statement::Break:
            {
                auto break_ = _result._Break;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, break_.span, String(ep, "Break statement")));
                break;
            }
            case Statement::Continue:
            {
                auto continue_ = _result._Continue;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, continue_.span, String(ep, "Continue statement")));
                break;
            }
            case Statement::Return:
            {
                auto return_ = _result._Return;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, return_.span, String(ep, "Return statement")));
                break;
            }
            case Statement::Throw:
            {
                auto throw_ = _result._Throw;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, throw_.span, String(ep, "Throw statement")));
                break;
            }
        }
    };
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_block(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Block& block, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions) {
    
    auto _statement_iterator = block.statements.get_iterator();
    while (auto _statement = _statement_iterator.next()) {
        auto statement = *_statement;{
            const auto _new_instructions_result = plan_statement(get_page(), ep, file, environment, symbols, operation, statement, required_type, blocks, instructions);
            auto new_instructions = _new_instructions_result._Ok;
            if (_new_instructions_result._tag == Success::Error) {
                const auto _new_instructions_Error = _new_instructions_result._Error;
                switch (_new_instructions_Error._tag) {
                default:
                    return Result<List<PlanInstruction>*, PlannerError>(_new_instructions_result._Error);

                }
            };
            instructions = new_instructions;
        }
    };
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_operand(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Operand& operand, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values) {
    auto r = Region();
    if (operand.member_access) 
        return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, operand.span, String(ep, "member access")));
    {
        auto _result = operand.expression;
        switch (_result._tag)
        {
            case Expression::Constant:
            {
                auto constant = _result._Constant;
                {
                    {
                        auto _result = constant;
                        switch (_result._tag)
                        {
                            case Constant::Boolean:
                            {
                                auto boolean = _result._Boolean;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, boolean.span, String(ep, "Boolean expression")));
                                break;
                            }
                            case Constant::Integer:
                            {
                                auto integer = _result._Integer;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, integer.span, String(ep, "Integer expression")));
                                break;
                            }
                            case Constant::Hex:
                            {
                                auto hex = _result._Hex;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, hex.span, String(ep, "Hex expression")));
                                break;
                            }
                            case Constant::FloatingPoint:
                            {
                                auto floating_point = _result._FloatingPoint;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, floating_point.span, String(ep, "Floating point expression")));
                                break;
                            }
                            case Constant::String:
                            {
                                auto string = _result._String;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, string.span, String(ep, "String expression")));
                                break;
                            }
                            case Constant::Character:
                            {
                                auto string = _result._Character;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, string.span, String(ep, "Character expression")));
                                break;
                            }
                            case Constant::Fragment:
                            {
                                auto fragment = _result._Fragment;
                                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, fragment.span, String(ep, "Fragment expression")));
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Expression::Type:
            {
                auto type = _result._Type;
                return Result<List<PlanInstruction>*, PlannerError>(plan_type_expression(get_page(), ep, file, environment, symbols, operation, type, required_type, blocks, instructions, values));
                break;
            }
            case Expression::Tuple:
            {
                auto tuple = _result._Tuple;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, tuple.span, String(ep, "Tuple expression")));
                break;
            }
            case Expression::Matrix:
            {
                auto matrix = _result._Matrix;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, matrix.span, String(ep, "Matrix expression")));
                break;
            }
            case Expression::Block:
            {
                auto block = _result._Block;
                return Result<List<PlanInstruction>*, PlannerError>(plan_block(get_page(), ep, file, environment, symbols, operation, block, required_type, blocks, instructions));
                break;
            }
            case Expression::If:
            {
                auto if_ = _result._If;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, if_.span, String(ep, "If expression")));
                break;
            }
            case Expression::Match:
            {
                auto match_ = _result._Match;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, match_.span, String(ep, "Match expression")));
                break;
            }
            case Expression::Choose:
            {
                auto choose_ = _result._Choose;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, choose_.span, String(ep, "Choose expression")));
                break;
            }
            case Expression::For:
            {
                auto for_ = _result._For;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, for_.span, String(ep, "For expression")));
                break;
            }
            case Expression::While:
            {
                auto while_ = _result._While;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, while_.span, String(ep, "While expression")));
                break;
            }
            case Expression::Try:
            {
                auto try_ = _result._Try;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, try_.span, String(ep, "Try expression")));
                break;
            }
            case Expression::SizeOf:
            {
                auto sizeof_ = _result._SizeOf;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, sizeof_.span, String(ep, "SizeOf expression")));
                break;
            }
            case Expression::Is:
            {
                auto is_ = _result._Is;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, is_.span, String(ep, "Is expression")));
                break;
            }
            case Expression::New:
            {
                auto new__ = _result._New;
                return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, new__.span, String(ep, "New expression")));
                break;
            }
        }
    };
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_operation(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, Vector<Operand>& operation, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values) {
    auto r = Region();
    auto operand_iterator = operation.get_iterator();
    while (auto operand = operand_iterator.next()) {
        const auto _new_instructions_result = plan_operand(get_page(), ep, file, environment, symbols, &operand_iterator, *operand, required_type, blocks, instructions, values);
        auto new_instructions = _new_instructions_result._Ok;
        if (_new_instructions_result._tag == Success::Error) {
            const auto _new_instructions_Error = _new_instructions_result._Error;
            switch (_new_instructions_Error._tag) {
            default:
                return Result<List<PlanInstruction>*, PlannerError>(_new_instructions_result._Error);

            }
        };
    };
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

Result<List<PlanInstruction>*, PlannerError> Planner::plan_action(Page* rp, Page* ep, String file, List<HashMap<String, String>>& environment, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Action& action, String* required_type, List<BasicBlock>& blocks, List<PlanInstruction>* instructions, List<String>& values) {
    auto r = Region();
    if (action.target.length>0) 
        return Result<List<PlanInstruction>*, PlannerError>(FeatureNotImplemented(file, (*(action.target.get(0))).span, String(ep, "set")));
    const auto _new_instructions_result = plan_operation(get_page(), ep, file, environment, symbols, action.source, required_type, blocks, instructions, values);
    auto new_instructions = _new_instructions_result._Ok;
    if (_new_instructions_result._tag == Success::Error) {
        const auto _new_instructions_Error = _new_instructions_result._Error;
        switch (_new_instructions_Error._tag) {
        default:
            return Result<List<PlanInstruction>*, PlannerError>(_new_instructions_result._Error);

        }
    };
    return Result<List<PlanInstruction>*, PlannerError>(instructions);
}

Result<Void, PlannerError> Planner::plan_function(Page* ep, Plan::Source* source, String file, HashMap<String, Nameable>& symbols, Function& func) {
    auto r = Region();
    if (functions_builder.contains(func.name)) 
        return Result<Void, PlannerError>(DuplicateFunction(String(ep, func.name)));
    const auto _returnType_result = resolve_type(get_page(), ep, file, symbols, *func.returns_);
    auto returnType = _returnType_result._Ok;
    if (_returnType_result._tag == Success::Error) {
        const auto _returnType_Error = _returnType_result._Error;
        switch (_returnType_Error._tag) {
        default:
            return Result<Void, PlannerError>(_returnType_result._Error);

        }
    };
    List<PlanArgument>& input_list = *new (alignof(List<PlanArgument>), r.get_page()) List<PlanArgument>();
    HashMapBuilder<String, String>& input_environment_builder = *new (alignof(HashMapBuilder<String, String>), r.get_page()) HashMapBuilder<String, String>();
    
    auto _item_iterator = func.input.get_iterator();
    while (auto _item = _item_iterator.next()) {
        auto item = *_item;{
            if (item.type == nullptr) 
                return Result<Void, PlannerError>(FeatureNotImplemented(file, item.span, String(ep, "Type inference")));
            const auto _type_result = resolve_type(get_page(), ep, file, symbols, *item.type);
            auto type = _type_result._Ok;
            if (_type_result._tag == Success::Error) {
                const auto _type_Error = _type_result._Error;
                switch (_type_Error._tag) {
                default:
                    return Result<Void, PlannerError>(_type_result._Error);

                }
            };
            input_list.add(Plan::PlanArgument(*item.name, type, Vector<String>(get_page(), List<String>()), nullptr));
            input_environment_builder.add(*item.name, type);
        }
    };
    auto page = get_page();
    List<HashMap<String, String>>& environment = *new (alignof(List<HashMap<String, String>>), page) List<HashMap<String, String>>();
    environment.add(HashMap<String, String>(page, input_environment_builder));
    List<BasicBlock>& blocks = *new (alignof(List<BasicBlock>), r.get_page()) List<BasicBlock>();
    List<PlanInstruction>& instructions = *new (alignof(List<PlanInstruction>), r.get_page()) List<PlanInstruction>();
    {
        auto _result = func.implementation;
        switch (_result._tag)
        {
            case Implementation::Action:
            {
                auto action = _result._Action;
                {
                    List<String>& values = *new (alignof(List<String>), r.get_page()) List<String>();
                    String* required_argument_type = nullptr;
                    if (func.returns_) {
                    };
                    const auto _new_instructions_result = plan_action(get_page(), ep, file, environment, symbols, nullptr, action, required_argument_type, blocks, &instructions, values);
                    auto new_instructions = _new_instructions_result._Ok;
                    if (_new_instructions_result._tag == Success::Error) {
                        const auto _new_instructions_Error = _new_instructions_result._Error;
                        switch (_new_instructions_Error._tag) {
                        default:
                            return Result<Void, PlannerError>(_new_instructions_result._Error);

                        }
                    };
                    instructions = *new_instructions;
                    if (func.returns_) {
                        List<String>& ret_list = *new (alignof(List<String>), r.get_page()) List<String>();
                        ret_list.add(*(*(instructions.get_head())).result);
                        instructions.add(Plan::PlanInstruction(nullptr, String(get_page(), "ret"), Vector<String>(get_page(), ret_list), String(get_page(), "void"), nullptr, String(get_page(), "ret"), HashMap<String, String>(get_page(), HashMapBuilder<String, String>()), false, false, String(), 0, nullptr));
                    }
                    else {
                        instructions.add(Plan::PlanInstruction(nullptr, String(get_page(), "ret"), Vector<String>(get_page(), List<String>()), String(get_page(), "void"), nullptr, String(get_page(), "ret"), HashMap<String, String>(get_page(), HashMapBuilder<String, String>()), false, false, String(), 0, nullptr));
                    };
                    auto block = Plan::BasicBlock(String(get_page(), "entry"), Vector<PlanInstruction>(get_page(), instructions), Vector<String>(get_page(), List<String>()), Vector<String>(get_page(), List<String>()), true, nullptr);
                    blocks.add(block);
                };
                break;
            }
            case Implementation::Extern:
            {
                auto e = _result._Extern;
                {
                };
                break;
            }
            case Implementation::Instruction:
            {
                auto i = _result._Instruction;
                return Result<Void, PlannerError>(Void());
                break;
            }
            case Implementation::Intrinsic:
            {
                auto i = _result._Intrinsic;
                return Result<Void, PlannerError>(Void());
                break;
            }
        }
    };
    auto plan_function = Plan::PlanFunction(source, func.name, Vector<PlanArgument>(get_page(), input_list), returnType, Vector<BasicBlock>(get_page(), blocks), nullptr, String(get_page(), "external"), String(), Vector<String>(get_page(), List<String>()), Vector<Variable>(get_page(), List<Variable>()), false, true, String(get_page(), "default"), String());
    functions_builder.add(func.name, plan_function);
    return Result<Void, PlannerError>(Void());
}

}
}