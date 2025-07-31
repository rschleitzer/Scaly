#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;


Planner::Planner(Program& program, HashSetBuilder<String> intrinsics_builder, List<Plan::Type> types_list, HashMapBuilder<String, Plan::Type> types_builder, List<Plan::Function> functions_list, HashMapBuilder<String, Plan::Function> functions_builder) : program(program), intrinsics_builder(intrinsics_builder), types_list(types_list), types_builder(types_builder), functions_list(functions_list), functions_builder(functions_builder) {}

Planner::Planner(Program& program) : program(program), intrinsics_builder(HashSetBuilder<String>()), types_builder(HashMapBuilder<String, Plan::Type>()), functions_builder(HashMapBuilder<String, Plan::Function>()){
}

Result<Plan::Module, PlannerError> Planner::plan_program(Page* rp, Page* ep) {
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
                return Result<Plan::Module, PlannerError>(_exists_result._Error);

            }
        };
        if (exists == false) {
            {
                const auto _void_result = Directory::create(ep, path);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Plan::Module, PlannerError>(_void_result._Error);

                    }
                }}
                ;
        };
    };
    {
        const auto _void_result = plan_module(ep, program.module_);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Plan::Module, PlannerError>(_void_result._Error);

            }
        }}
        ;
    
    auto _module__iterator = program.module_.modules.get_iterator();
    while (auto _module_ = _module__iterator.next()) {
        auto module_ = *_module_;{
            {
                const auto _void_result = plan_module(ep, module_);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Plan::Module, PlannerError>(_void_result._Error);

                    }
                }}
                ;
        }
    };
    return Result<Plan::Module, PlannerError>(Plan::Module(path, program.module_.name, Vector<Plan::Type>(rp, types_list), HashMap<String, Plan::Type>(rp, types_builder), Vector<Plan::Function>(rp, functions_list), HashMap<String, Plan::Function>(rp, functions_builder)));
}

Result<Void, PlannerError> Planner::plan_module(Page* ep, Module& module_) {
    auto r = Region();
    {
        const auto _void_result = plan_symbols(ep, module_.members);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, PlannerError>(_void_result._Error);

            }
        }}
        ;
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> Planner::plan_symbols(Page* ep, Vector<Member> symbols) {
    auto r = Region();
    
    auto _member_iterator = symbols.get_iterator();
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
                            const auto _void_result = plan_function(ep, func);
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
                            const auto _void_result = plan_concept(ep, concept);
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
                        return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "operator symbol")));
                        break;
                    }
                    case Member::Package:
                    {
                        auto p = _result._Package;
                        return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "package")));
                        break;
                    }
                }
            };
        }
    };
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> Planner::plan_concept(Page* ep, Concept& concept) {
    {
        auto _result = concept.definition;
        switch (_result._tag)
        {
            case Definition::Namespace:
            {
                auto ns = _result._Namespace;
                return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "namespace")));
                break;
            }
            case Definition::Structure:
            {
                auto structure = _result._Structure;
                return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "structure")));
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
                return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "global")));
                break;
            }
            case Definition::Union:
            {
                auto u = _result._Union;
                return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "union")));
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

String Planner::resolve_type(Page* rp, Type type) {
    return String(rp, *type.name.get(0));
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_operation(Page* rp, Page* ep, Vector<Operand>& operation, String result, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions) {
    auto r = Region();
    
    auto _operand_iterator = operation.get_iterator();
    while (auto _operand = _operand_iterator.next()) {
        auto operand = *_operand;{
            (*instructions).add(Plan::Instruction(Plan::FMul(String(get_page(), "a"), String(get_page(), "b"), result)));
        }
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_action(Page* rp, Page* ep, Action& action, String result, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions) {
    auto r = Region();
    if (action.target.length>0) 
        return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "set")));
    const auto _new_instructions_result = plan_operation(get_page(), ep, action.source, result, blocks, instructions);
    auto new_instructions = _new_instructions_result._Ok;
    if (_new_instructions_result._tag == Success::Error) {
        const auto _new_instructions_Error = _new_instructions_result._Error;
        switch (_new_instructions_Error._tag) {
        default:
            return Result<List<Plan::Instruction>*, PlannerError>(_new_instructions_result._Error);

        }
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<Void, PlannerError> Planner::plan_function(Page* ep, Function& func) {
    auto r = Region();
    if (functions_builder.contains(func.name)) 
        return Result<Void, PlannerError>(DuplicateFunction(String(ep, func.name)));
    auto returnType = resolve_type(get_page(), *func.returns_);
    List<Plan::Argument>& input_list = *new (alignof(List<Plan::Argument>), r.get_page()) List<Plan::Argument>();
    
    auto _item_iterator = func.input.get_iterator();
    while (auto _item = _item_iterator.next()) {
        auto item = *_item;{
            input_list.add(Plan::Argument(*item.name, resolve_type(get_page(), *item.type)));
        }
    };
    List<Plan::Block>& blocks = *new (alignof(List<Plan::Block>), r.get_page()) List<Plan::Block>();
    List<Plan::Instruction>& instructions = *new (alignof(List<Plan::Instruction>), r.get_page()) List<Plan::Instruction>();
    {
        auto _result = func.implementation;
        switch (_result._tag)
        {
            case Implementation::Action:
            {
                auto action = _result._Action;
                {
                    auto ret = String(get_page(), "_ret");
                    const auto _new_instructions_result = plan_action(get_page(), ep, action, ret, blocks, &instructions);
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
                        instructions.add(Plan::Instruction(Plan::Ret(ret)));
                    }
                    else {
                        instructions.add(Plan::Instruction(Plan::RetVoid()));
                    };
                    auto block = Plan::Block(String(get_page(), "entry"), Vector<Plan::Instruction>(get_page(), instructions));
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
    Plan::Function plan_function = Plan::Function(func.name, Vector<Plan::Argument>(get_page(), input_list), returnType, Vector<Plan::Block>(get_page(), blocks));
    functions_builder.add(func.name, plan_function);
    functions_list.add(plan_function);
    return Result<Void, PlannerError>(Void());
}

}
}