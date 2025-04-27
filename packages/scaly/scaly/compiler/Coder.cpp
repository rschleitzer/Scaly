#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;


Result<Void, TranspilerError> coder::code_plan(Page* ep, Plan& plan) {
    auto r = Region();
    StringBuilder& ll_file_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(Path::join(r.get_page(), plan.path, plan.name));
    ll_file_builder.append(".ll");
    StringBuilder& ll_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        const auto _void_result = File::write_from_string(ep, ll_file_builder.to_string(r.get_page()), ll_builder.to_string(r.get_page()));
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    return Result<Void, TranspilerError>(Void());
}

}
}