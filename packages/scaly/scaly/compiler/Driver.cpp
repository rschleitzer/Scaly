#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;

using namespace scaly::compiler::model;

using namespace scaly::compiler::transpiler;

using namespace scaly::compiler::planner;

using namespace scaly::compiler::coder;


Result<Void, CompilerError> compiler::transpile(Page* ep, String file_name, String program_name) {
    auto r = Region();
    const auto _prog_result = build_program(r.get_page(), ep, file_name, program_name);
    auto prog = _prog_result._Ok;
    if (_prog_result._tag == Success::Error) {
        const auto _prog_Error = _prog_result._Error;
        switch (_prog_Error._tag) {
        default:
            return Result<Void, CompilerError>(_prog_result._Error);

        }
    };
    {
        const auto _void_result = transpile_program(r.get_page(), prog);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, CompilerError>(_void_result._Error);

            }
        }}
        ;
    return Result<Void, CompilerError>(Void());
}

Result<Void, CompilerError> compiler::compile(Page* ep, String file_name, String program_name) {
    auto r = Region();
    const auto _prog_result = build_program(r.get_page(), ep, file_name, program_name);
    auto prog = _prog_result._Ok;
    if (_prog_result._tag == Success::Error) {
        const auto _prog_Error = _prog_result._Error;
        switch (_prog_Error._tag) {
        default:
            return Result<Void, CompilerError>(_prog_result._Error);

        }
    };
    const auto _plan_result = plan_program(r.get_page(), ep, prog);
    auto plan = _plan_result._Ok;
    if (_plan_result._tag == Success::Error) {
        const auto _plan_Error = _plan_result._Error;
        switch (_plan_Error._tag) {
        default:
            return Result<Void, CompilerError>(_plan_result._Error);

        }
    };
    {
        const auto _void_result = code_plan(r.get_page(), plan);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, CompilerError>(_void_result._Error);

            }
        }}
        ;
    return Result<Void, CompilerError>(Void());
}

}
}