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
    if (program.statements.length>0) {
        {
            const auto _void_result = plan_main_function(ep, program);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Plan::Module, PlannerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Plan::Module, PlannerError>(Plan::Module(path, program.module_.name, Vector<Plan::Type>(rp, types_list), HashMap<String, Plan::Type>(rp, types_builder), Vector<Plan::Function>(rp, functions_list), HashMap<String, Plan::Function>(rp, functions_builder)));
}

Result<Void, PlannerError> Planner::plan_main_function(Page* ep, Program& program) {
    auto r = Region();
    const auto main_name = String(get_page(), "main");
    if (functions_builder.contains(main_name)) 
        return Result<Void, PlannerError>(DuplicateFunction(String(ep, main_name)));
    List<Plan::Argument>& input_list = *new (alignof(List<Plan::Argument>), r.get_page()) List<Plan::Argument>();
    input_list.add(Plan::Argument(String(get_page(), "argc"), String(get_page(), "int32")));
    input_list.add(Plan::Argument(String(get_page(), "argv"), String(get_page(), "ptr")));
    List<Plan::Block>& blocks = *new (alignof(List<Plan::Block>), r.get_page()) List<Plan::Block>();
    List<Plan::Instruction>& instructions = *new (alignof(List<Plan::Instruction>), r.get_page()) List<Plan::Instruction>();
    List<String>& values = *new (alignof(List<String>), r.get_page()) List<String>();
    
    auto _statement_iterator = program.statements.get_iterator();
    while (auto _statement = _statement_iterator.next()) {
        auto statement = *_statement;{
            const auto _new_instructions_result = plan_statement(get_page(), ep, program.module_.symbols, nullptr, statement, blocks, &instructions);
            auto new_instructions = _new_instructions_result._Ok;
            if (_new_instructions_result._tag == Success::Error) {
                const auto _new_instructions_Error = _new_instructions_result._Error;
                switch (_new_instructions_Error._tag) {
                default:
                    return Result<Void, PlannerError>(_new_instructions_result._Error);

                }
            };
            instructions = *new_instructions;
        }
    };
    if ((*(instructions.get_head())).result) {
        List<String>& ret_list = *new (alignof(List<String>), r.get_page()) List<String>();
        ret_list.add(*(*(instructions.get_head())).result);
        instructions.add(Plan::Instruction(nullptr, String(get_page(), "ret"), Vector<String>(get_page(), ret_list)));
    }
    else {
        instructions.add(Plan::Instruction(nullptr, String(get_page(), "ret"), Vector<String>()));
    };
    auto block = Plan::Block(String(get_page(), "entry"), Vector<Plan::Instruction>(get_page(), instructions));
    blocks.add(block);
    const auto int_name = String(get_page(), "int32");
    Plan::Function plan_function = Plan::Function(main_name, Vector<Plan::Argument>(get_page(), input_list), int_name, Vector<Plan::Block>(get_page(), blocks));
    functions_builder.add(main_name, plan_function);
    functions_list.add(plan_function);
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> Planner::plan_module(Page* ep, Module& module_) {
    auto r = Region();
    {
        const auto _void_result = plan_symbols(ep, module_.symbols, module_.members);
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

Result<Void, PlannerError> Planner::plan_symbols(Page* ep, HashMap<String, Nameable>& symbols, Vector<Member> members) {
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
                            const auto _void_result = plan_function(ep, symbols, func);
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
                            const auto _void_result = plan_concept(ep, symbols, concept);
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

Result<Void, PlannerError> Planner::plan_concept(Page* ep, HashMap<String, Nameable>& symbols, Concept& concept) {
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

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_tuple(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, Tuple& tuple, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values) {
    
    auto _component_iterator = tuple.components.get_iterator();
    while (auto _component = _component_iterator.next()) {
        auto component = *_component;{
            if (component.name) 
                return Result<List<Plan::Instruction>*, PlannerError>(TupleComponentNamesNotSupported());
            plan_operation(get_page(), ep, symbols, component.value, blocks, instructions, values);
            const auto last_instruction = (*instructions).get_head();
            if (last_instruction) 
                values.add(*(*last_instruction).result);
        }
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

String Planner::allocate_value_name(Page* rp, List<Plan::Block>& blocks, List<Plan::Instruction>& instructions) {
    auto r = Region();
    StringBuilder& value_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    value_name_builder.append('v');
    value_name_builder.append(to_string(r.get_page(), blocks.count()));
    value_name_builder.append('_');
    value_name_builder.append(to_string(r.get_page(), instructions.count()));
    return String(value_name_builder.to_string(rp));
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_instruction_call(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, String name, Tuple& tuple, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions) {
    auto r = Region();
    List<String>& values_list = *new (alignof(List<String>), r.get_page()) List<String>();
    plan_tuple(get_page(), ep, symbols, tuple, blocks, instructions, values_list);
    auto values = Vector<String>(get_page(), values_list);
    auto page = get_page();
    const auto value = allocate_value_name(get_page(), blocks, *instructions);
    (*instructions).add(Plan::Instruction(new (alignof(String), page) String(get_page(), value), name, values));
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_type(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Type& type, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values) {
    if (type.name.length>1) 
        return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Qualified type name")));
    if (type.generics) 
        return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Generic type")));
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
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Call lifetime")));
                break;
            }
            case Lifetime::Local:
            {
                auto local = _result._Local;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Local lifetime")));
                break;
            }
            case Lifetime::Reference:
            {
                auto reference = _result._Reference;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Reference lifetime")));
                break;
            }
            case Lifetime::Thrown:
            {
                auto thrown = _result._Thrown;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Thrown lifetime")));
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
                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Package reference")));
                    break;
                }
                case Nameable::Concept:
                {
                    auto concept = _result._Concept;
                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Concept reference")));
                    break;
                }
                case Nameable::Operator:
                {
                    auto operator_ = _result._Operator;
                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Operator reference")));
                    break;
                }
                case Nameable::Functions:
                {
                    auto functions = _result._Functions;
                    {
                        if (functions.length>1) 
                            return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Overloaded functions")));
                        const auto function_ = *functions.get(0);
                        {
                            auto _result = function_.implementation;
                            switch (_result._tag)
                            {
                                case Implementation::Action:
                                {
                                    auto action = _result._Action;
                                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Internal Function call")));
                                    break;
                                }
                                case Implementation::Extern:
                                {
                                    auto extern_ = _result._Extern;
                                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "External Function call")));
                                    break;
                                }
                                case Implementation::Instruction:
                                {
                                    auto instruction_ = _result._Instruction;
                                    {
                                        if (operation == nullptr) 
                                            return Result<List<Plan::Instruction>*, PlannerError>(InstructionWithoutArguments(String(ep, name)));
                                        auto next_operand = (*operation).next();
                                        if (next_operand == nullptr) 
                                            return Result<List<Plan::Instruction>*, PlannerError>(InstructionWithoutArguments(String(ep, name)));
                                        {
                                            auto _result = (*next_operand).expression;
                                            switch (_result._tag)
                                            {
                                                case Expression::Tuple:
                                                {
                                                    auto tuple = _result._Tuple;
                                                    {
                                                        return Result<List<Plan::Instruction>*, PlannerError>(plan_instruction_call(get_page(), ep, symbols, operation, name, tuple, blocks, instructions));
                                                    };
                                                    break;
                                                }
                                                default:
                                                    return Result<List<Plan::Instruction>*, PlannerError>(InstructionWithoutArguments(String(ep, name)));
                                            }
                                        };
                                    };
                                    break;
                                }
                                case Implementation::Intrinsic:
                                {
                                    auto intrinsic_ = _result._Intrinsic;
                                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Intrinsic call")));
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
                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Property reference")));
                    break;
                }
                case Nameable::Variant:
                {
                    auto variant = _result._Variant;
                    return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Variant reference")));
                    break;
                }
            }
        };
    }
    else {
        values.add(name);
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_statement(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Statement& statement, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions) {
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
                    return Result<List<Plan::Instruction>*, PlannerError>(plan_action(get_page(), ep, symbols, operation, action, blocks, instructions, values));
                };
                break;
            }
            case Statement::Binding:
            {
                auto binding = _result._Binding;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Binding statement")));
                break;
            }
            case Statement::Break:
            {
                auto break_ = _result._Break;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Break statement")));
                break;
            }
            case Statement::Continue:
            {
                auto continue_ = _result._Continue;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Continue statement")));
                break;
            }
            case Statement::Return:
            {
                auto return_ = _result._Return;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Return statement")));
                break;
            }
            case Statement::Throw:
            {
                auto throw_ = _result._Throw;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Throw statement")));
                break;
            }
        }
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_block(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Block& block, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions) {
    
    auto _statement_iterator = block.statements.get_iterator();
    while (auto _statement = _statement_iterator.next()) {
        auto statement = *_statement;{
            const auto _new_instructions_result = plan_statement(get_page(), ep, symbols, operation, statement, blocks, instructions);
            auto new_instructions = _new_instructions_result._Ok;
            if (_new_instructions_result._tag == Success::Error) {
                const auto _new_instructions_Error = _new_instructions_result._Error;
                switch (_new_instructions_Error._tag) {
                default:
                    return Result<List<Plan::Instruction>*, PlannerError>(_new_instructions_result._Error);

                }
            };
            instructions = new_instructions;
        }
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_operand(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Operand& operand, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values) {
    auto r = Region();
    if (operand.member_access) 
        return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "member access")));
    {
        auto _result = operand.expression;
        switch (_result._tag)
        {
            case Expression::Constant:
            {
                auto constant = _result._Constant;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Constant expression")));
                break;
            }
            case Expression::Type:
            {
                auto type = _result._Type;
                return Result<List<Plan::Instruction>*, PlannerError>(plan_type(get_page(), ep, symbols, operation, type, blocks, instructions, values));
                break;
            }
            case Expression::Tuple:
            {
                auto tuple = _result._Tuple;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Tuple expression")));
                break;
            }
            case Expression::Matrix:
            {
                auto matrix = _result._Matrix;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Matrix expression")));
                break;
            }
            case Expression::Block:
            {
                auto block = _result._Block;
                return Result<List<Plan::Instruction>*, PlannerError>(plan_block(get_page(), ep, symbols, operation, block, blocks, instructions));
                break;
            }
            case Expression::If:
            {
                auto if_ = _result._If;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "If expression")));
                break;
            }
            case Expression::Match:
            {
                auto match_ = _result._Match;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Match expression")));
                break;
            }
            case Expression::Choose:
            {
                auto choose_ = _result._Choose;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Choose expression")));
                break;
            }
            case Expression::For:
            {
                auto for_ = _result._For;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "For expression")));
                break;
            }
            case Expression::While:
            {
                auto while_ = _result._While;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "While expression")));
                break;
            }
            case Expression::Try:
            {
                auto try_ = _result._Try;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Try expression")));
                break;
            }
            case Expression::SizeOf:
            {
                auto sizeof_ = _result._SizeOf;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "SizeOf expression")));
                break;
            }
            case Expression::Is:
            {
                auto is_ = _result._Is;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "Is expression")));
                break;
            }
            case Expression::New:
            {
                auto new__ = _result._New;
                return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "New expression")));
                break;
            }
        }
    };
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_operation(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, Vector<Operand>& operation, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values) {
    auto r = Region();
    auto operand_iterator = operation.get_iterator();
    while (auto operand = operand_iterator.next()) {
        const auto _new_instructions_result = plan_operand(get_page(), ep, symbols, &operand_iterator, *operand, blocks, instructions, values);
        auto new_instructions = _new_instructions_result._Ok;
        if (_new_instructions_result._tag == Success::Error) {
            const auto _new_instructions_Error = _new_instructions_result._Error;
            switch (_new_instructions_Error._tag) {
            default:
                return Result<List<Plan::Instruction>*, PlannerError>(_new_instructions_result._Error);

            }
        };
    };
    return Result<List<Plan::Instruction>*, PlannerError>(instructions);
}

Result<List<Plan::Instruction>*, PlannerError> Planner::plan_action(Page* rp, Page* ep, HashMap<String, Nameable>& symbols, VectorIterator<Operand>* operation, Action& action, List<Plan::Block>& blocks, List<Plan::Instruction>* instructions, List<String>& values) {
    auto r = Region();
    if (action.target.length>0) 
        return Result<List<Plan::Instruction>*, PlannerError>(FeatureNotImplemented(String(ep, "set")));
    const auto _new_instructions_result = plan_operation(get_page(), ep, symbols, action.source, blocks, instructions, values);
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

Result<Void, PlannerError> Planner::plan_function(Page* ep, HashMap<String, Nameable>& symbols, Function& func) {
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
                    List<String>& values = *new (alignof(List<String>), r.get_page()) List<String>();
                    const auto _new_instructions_result = plan_action(get_page(), ep, symbols, nullptr, action, blocks, &instructions, values);
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
                        instructions.add(Plan::Instruction(nullptr, String(get_page(), "ret"), Vector<String>(get_page(), ret_list)));
                    }
                    else {
                        instructions.add(Plan::Instruction(nullptr, String(get_page(), "ret"), Vector<String>()));
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