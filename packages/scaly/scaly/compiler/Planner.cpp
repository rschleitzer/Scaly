#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;

namespace planner {


Result<Plan::Module, PlannerError> plan_program(Page* rp, Page* ep, Program& program) {
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
    HashSetBuilder<String>& intrinsics_builder = *new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>();
    HashMapBuilder<String, Plan::Function>& functions_builder = *new (alignof(HashMapBuilder<String, Plan::Function>), r.get_page()) HashMapBuilder<String, Plan::Function>();
    
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
    return Result<Plan::Module, PlannerError>(Plan::Module(path, program.module_.name));
}

Result<Void, PlannerError> plan_module(Page* ep, Module& module_) {
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

Result<Void, PlannerError> plan_symbols(Page* ep, Vector<Member> symbols) {
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
                        return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "namespace local operator")));
                        break;
                    }
                    case Member::Package:
                    {
                        auto p = _result._Package;
                        return Result<Void, PlannerError>(FeatureNotImplemented(String(ep, "namespace local package")));
                        break;
                    }
                }
            };
        }
    };
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> plan_concept(Page* ep, Concept& concept) {
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

Result<Void, PlannerError> plan_intrinsic(Page* ep, String name) {
    auto r = Region();
    return Result<Void, PlannerError>(Void());
}

Result<Void, PlannerError> plan_function(Page* ep, Function& func) {
    auto r = Region();
    return Result<Void, PlannerError>(Void());
}

}

}
}