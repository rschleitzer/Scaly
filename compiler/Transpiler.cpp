namespace scaly {
namespace compiler {
namespace transpiler {

struct Transpiler : Object {

    TranspilerError* program(Page* _ep, const Program& program) {
        Region _r;

        switch (program.text._tag) {
            case Text::File:
                {
                    auto file = program.text._File;
                    auto path = String(_r.get_page(), Path::join(_r.get_page(), Path::get_directory_name(_r.get_page(), file), String(_r.get_page(), "output")));
                    auto _result = Directory::exists(_ep, path);
                    if (_result._tag == scaly::containers::Result<bool, FileError>::Error)
                        return new(alignof(TranspilerError), _ep) TranspilerError(_result._Error);
                    
                    if (!_result._Ok)
                    {
                        auto _result = Directory::create(_ep, path);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }

                    path = Path::join(_r.get_page(), path, program.name);
                    auto _exists_result = Directory::exists(_ep, path);
                    if (_exists_result._tag == scaly::containers::Result<bool, FileError>::Error)
                        return new(alignof(TranspilerError), _ep) TranspilerError(_exists_result._Error);
                    auto exists = _exists_result._Ok;
                    if (!exists) {
                        auto _result = Directory::create(_ep, path);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }

                    switch (program.concept.definition._tag) {
                        case Definition::Namespace:
                            {
                                auto member_iterator = HashMapIterator<String, Nameable>(program.concept.definition._Namespace.code.symbols);
                                //auto member_iterator = VectorIterator(program.concept.definition._Namespace.code.symbols.get_values(_r.get_page()));
                                while (auto member = member_iterator.next()) {
                                    switch(member->_tag) {
                                        case Nameable::Concept:
                                            {
                                                auto _result = concept(_ep, path, file, member->_Concept);
                                                if (_result != nullptr)
                                                    return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                                            }
                                            break;
                                        default:
                                            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                                    }
                                }
                            }
                            break;
                        default:
                            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                    }
                }
                break;
            default:
                return new(alignof(TranspilerError), _ep) TranspilerError(OnlyFile());
        }

        return nullptr;
    }

    bool children_modules_available(Definition definition) {
        return true;
    }

    TranspilerError* concept(Page* _ep, const String& path, const String& file, const Concept& concept) {
        Region _r;

        if (children_modules_available(concept.definition)) {
            String namespace_path = Path::join(_r.get_page(), path, concept.name);
            auto _exists_result = Directory::exists(_ep, namespace_path);
            if (_exists_result._tag == scaly::containers::Result<bool, FileError>::Error)
                return new(alignof(TranspilerError), _ep) TranspilerError(_exists_result._Error);
            auto exists = _exists_result._Ok;
            if (!exists) {
                auto _result = Directory::create(_ep, namespace_path);
                if (_result != nullptr)
                    return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
            }
            switch (concept.definition._tag) {
                case Definition::Namespace: {
                    auto _namespace = concept.definition._Namespace;
                    return namespace_(_ep, namespace_path, concept.name, file, _namespace);
                }
                
                default:
                    // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                    return nullptr;
            }
        }
 
        switch (concept.definition._tag) {
            case Definition::Namespace: {
                auto _namespace = concept.definition._Namespace;
                return namespace_(_ep, path, concept.name, file, _namespace);
            }
            
            default:
                // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                return nullptr;
        }

        return nullptr;
    }

    TranspilerError* namespace_(Page* _ep, const String& path, const String& file, const String& name, const Namespace& namespace_) {
        Region _r;

        auto member_iterator = HashMapIterator<String, Nameable>(namespace_.code.symbols);
        //auto member_iterator = VectorIterator(namespace_.code.symbols.get_values(_r.get_page()));
        while (auto member = member_iterator.next()) {
            switch(member->_tag) {
                case Nameable::Concept:
                    {
                        auto _result = concept(_ep, path, file, member->_Concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Module:
                    {
                        auto _result = module(_ep, path, member->_Module);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                default:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
            }
        }

        return nullptr;
    }

    TranspilerError* module(Page* _ep, const String& path, const Module& module) {
        Region _r;
        return concept(_ep, path, module.file, module.concept);
    }
};

}
}
}