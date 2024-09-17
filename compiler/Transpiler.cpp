namespace scaly {
namespace compiler {
namespace transpiler {

struct Transpiler : Object {

    TranspilerError* transpile(Page* _ep, const Program& program) {
        Region _r;

        switch (program.text._tag) {
            case Text::File: {
                auto path = String(_r.get_page(), Path::join(_r.get_page(), Path::get_directory_name(_r.get_page(), program.text._File), String(_r.get_page(), "output")));
                return build_program(_ep, path, program);
            }

            default:
                return new(alignof(TranspilerError), _ep) TranspilerError(OnlyFile());
        }
    }

    TranspilerError* build_program(Page* _ep, const String& path, const Program& program) {
        Region _r;

        switch (program.concept.definition._tag) {
            case Definition::Namespace: {
                auto member_iterator = new (alignof(HashMapIterator<String, Nameable>), _r.get_page()) HashMapIterator<String, Nameable>(program.concept.definition._Namespace.code.symbols);
                while (auto member = member_iterator->next()) {
                    switch(member->_tag) {
                        case Nameable::Concept:
                            return handle_concept(_ep, path, member->_Concept);
                        default:
                            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                    }
                }
            }
            default:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
        }

        return nullptr;
    }

    TranspilerError* handle_concept(Page* _ep, const String& path, const Concept& concept) {
        Region _r;
 
        switch (concept.definition._tag) {
            case Definition::Namespace: {
                auto _namespace = concept.definition._Namespace;
                return build_namespace(_ep, path, concept.name, _namespace);
            }
            
            default:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
        }

        return nullptr;
    }

    TranspilerError* build_namespace(Page* _ep, const String& path, const String& name, const Namespace& namespace_) {
        Region _r;

        auto member_iterator = new (alignof(HashMapIterator<String, Nameable>), _r.get_page()) HashMapIterator<String, Nameable>(namespace_.code.symbols);
        while (auto member = member_iterator->next()) {
            switch(member->_tag) {
                case Nameable::Concept:
                    return handle_concept(_ep, path, member->_Concept);
                default:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
            }
        }

        return nullptr;
    }
};

}
}
}