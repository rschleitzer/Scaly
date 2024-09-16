namespace scaly {
namespace compiler {
namespace transpiler {

TranspilerError* transpile_namespace(Page* _ep, const Namespace& namespace_) {
    Region _r;

    auto members = namespace_.code.symbols.get_values(_r.get_page());

    return nullptr;
}

TranspilerError* transpile(Page* _ep, const Program& program) {
    Region _r;

    switch (program.concept.body._tag) {
        case Body::Namespace:
            return transpile_namespace(_ep, program.concept.body._Namespace);
        default:
            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
    }

    return nullptr;
}

}
}
}
