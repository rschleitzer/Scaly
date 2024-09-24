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
                    auto _exists_result = Directory::exists(_ep, path);
                    if (_exists_result._tag == scaly::containers::Result<bool, FileError>::Error)
                        return new(alignof(TranspilerError), _ep) TranspilerError(_exists_result._Error);
                    auto exists = _exists_result._Ok;
                    if (!exists) {
                        auto _result = Directory::create(_ep, path);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }

                    if (children_modules_available(program.concept.definition)) {
                        auto _path_result = extend_and_create_path(_r.get_page(), _ep, path, program.concept.name);
                        if (_path_result._tag == Result<String, FileError>::Error)
                            return new(alignof(TranspilerError), _ep) TranspilerError(_path_result._Error);
                        path = _path_result._Ok;
                    }

                    StringBuilder& header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
                    StringBuilder& cpp_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();

                    auto _result = concept(_ep, path, file, header_builder, cpp_builder,  program.concept);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);

                    auto base_file_name = Path::join(_r.get_page(), path, program.concept.name);
                    StringBuilder& header_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(base_file_name);
                    header_name_builder.append_string(String(_r.get_page(), ".h"));
                    auto header_name = header_name_builder.to_string(_r.get_page());
                    auto _header_result = File::write_from_string(_ep, header_name, header_builder.to_string(_r.get_page()));
                    if (_header_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_header_result);
                    if (cpp_builder.get_length() > 0) {
                        StringBuilder& cpp_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(base_file_name);
                        cpp_name_builder.append_string(String(_r.get_page(), ".cpp"));
                        auto cpp_name = cpp_name_builder.to_string(_r.get_page());
                        auto _cpp_result = File::write_from_string(_ep, cpp_name, cpp_builder.to_string(_r.get_page()));
                        if (_cpp_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_cpp_result);
                    }
                }
                break;
            default:
                return new(alignof(TranspilerError), _ep) TranspilerError(OnlyFile());
        }

        return nullptr;
    }

    TranspilerError* module(Page* _ep, String path, const Module& module) {
        Region _r;

        if (children_modules_available(module.concept.definition)) {
            auto _path_result = extend_and_create_path(_r.get_page(), _ep, path, module.concept.name);
            if (_path_result._tag == Result<String, FileError>::Error)
                return new(alignof(TranspilerError), _ep) TranspilerError(_path_result._Error);
            path = _path_result._Ok;
        }

        StringBuilder& header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        StringBuilder& cpp_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();

        auto _concept_result = concept(_ep, path, module.file, header_builder, cpp_builder, module.concept);
        if (_concept_result)
            return _concept_result;

        auto base_file_name = Path::join(_r.get_page(), path, module.concept.name);
        StringBuilder& header_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(base_file_name);
        header_name_builder.append_string(String(_r.get_page(), ".h"));
        auto header_name = header_name_builder.to_string(_r.get_page());
        auto _header_result = File::write_from_string(_ep, header_name, header_builder.to_string(_r.get_page()));
        if (_header_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_header_result);

        if (cpp_builder.get_length() > 0) {
            StringBuilder& cpp_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(base_file_name);
            cpp_name_builder.append_string(String(_r.get_page(), ".cpp"));
            auto cpp_name = cpp_name_builder.to_string(_r.get_page());
            auto _cpp_result = File::write_from_string(_ep, cpp_name, cpp_builder.to_string(_r.get_page()));
            if (_cpp_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_cpp_result);
        }

        return nullptr;
    }

    bool children_modules_available(Definition definition) {
        switch (definition._tag) {
            case Definition::Namespace:
                {
                    auto namespace_ = definition._Namespace;
                    auto member_iterator = HashMapIterator<String, Nameable>(namespace_.code.symbols);
                    while (auto member = member_iterator.next()) {
                        switch(member->_tag) {
                            case Nameable::Module:
                                return true;
                            default:
                                continue;
                        }
                    }
                }
                break;
            default:
                return false;
        }
        return false;
    }

    Result<String, FileError> extend_and_create_path(Page* _rp, Page* _ep, String path, String name) {
        path = Path::join(_rp, path, name);
        auto _exists_result = Directory::exists(_ep, path);
        if (_exists_result._tag == scaly::containers::Result<bool, FileError>::Error)
            return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(_exists_result._Error) };
        auto exists = _exists_result._Ok;
        if (!exists) {
            auto _result = Directory::create(_ep, path);
            if (_result != nullptr)
                return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(*_result) };
        }
        return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = path };
    }

    TranspilerError* namespace_(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, const Namespace& namespace_) {
        Region _r;

        auto member_iterator = HashMapIterator<String, Nameable>(namespace_.code.symbols);
        while (auto member = member_iterator.next()) {
            switch(member->_tag) {
                case Nameable::Concept:
                    {
                        auto _result = concept(_ep, path, source, header_builder, cpp_builder, member->_Concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Module:
                    {
                        auto _module = member->_Module;
                        header_builder.append_string(String(_r.get_page(), "#include \""));
                        if (children_modules_available(_module.concept.definition)) {
                            header_builder.append_string(String(_r.get_page(), _module.concept.name));
                            header_builder.append_character('/');
                        }
                        header_builder.append_string(String(_r.get_page(), _module.concept.name));
                        header_builder.append_string(String(_r.get_page(), ".h\"\n"));

                        auto _result = module(_ep, path, _module);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                default:
                    // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                    continue;
            }
        }

        return nullptr;
    }

    TranspilerError* concept(Page* _ep, String path, String source, StringBuilder& header_builder, StringBuilder& cpp_builder, const Concept& concept) {
        Region _r;
 
        switch (concept.definition._tag) {
            case Definition::Namespace: {
                auto _namespace = concept.definition._Namespace;
                return namespace_(_ep, path, source, concept.name, header_builder, cpp_builder,  _namespace);
            }

            case Definition::Structure: {
                auto _structure = concept.definition._Structure;
                auto _result = structure(_ep, header_builder, cpp_builder, concept.name, _structure, concept.parameters);
            }
            
            default:
                // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                return nullptr;
        }

        return nullptr;
    }

    TranspilerError* structure(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, const Structure& structure, Vector<GenericParameter> parameters) {
        Region _r;
        if (parameters.length > 0) {
            header_builder.append_string(String(_r.get_page(), "template<"));
            {
                auto generic_iterator = VectorIterator<GenericParameter>(&parameters);
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    header_builder.append_string(String(_r.get_page(), "class "));
                    header_builder.append_string(generic->name);
                    if (i < parameters.length - 1)
                        header_builder.append_string(String(_r.get_page(), ", "));
                }
            }
            header_builder.append_character('>');
            header_builder.append_character(' ');
        }
        header_builder.append_string(String(_r.get_page(), "struct "));
        header_builder.append_string(name);
        if (!name.equals(String(_r.get_page(), "Object")))
            header_builder.append_string(String(_r.get_page(), " : Object"));
        header_builder.append_string(String(_r.get_page(), " {"));
        auto property_iterator = HashMapIterator<String, Property>(structure.properties);
        while (auto property = property_iterator.next()) {
            if (property->type) {
                header_builder.append_character('\n');
                header_builder.append_string(String(_r.get_page(), "    "));
                type(_ep, header_builder, property->type);
            }

            if (property->name)
            {
                header_builder.append_character(' ');
                header_builder.append_string(*property->name);
                header_builder.append_character(';');
            }
        }
        header_builder.append_string(String(_r.get_page(), "\n};"));
        return nullptr;
    }

    void type(Page* _ep, StringBuilder& builder, Type* type_) {
        Region _r;
        if (type_->name.length == 1 && type_->name[0]->equals(String(_r.get_page(), "pointer"))) {
            auto generic_iterator = VectorIterator<Type>(&type_->generics);
            while(auto generic = generic_iterator.next()) {
                type(_ep, builder, generic);
                break;
            }
            builder.append_character('*');
            return;
        }

        {
            auto type_name_iterator = VectorIterator<String>(&type_->name);
            size_t i = 0;
            while (auto name_part = type_name_iterator.next()) {
                builder.append_string(*name_part);
                if (i < type_->name.length - 1)
                    builder.append_string(String(_r.get_page(), "::"));
                i++;
            }
        }

        if (type_->generics.length > 0) {
            builder.append_character('<');
            {
                auto generic_iterator = VectorIterator<Type>(&type_->generics);
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    type(_ep, builder, generic);
                    if (i < type_->generics.length - 1)
                        builder.append_string(String(_r.get_page(), ", "));
                }
            }
            builder.append_character('>');
        }
    }
};

}
}
}