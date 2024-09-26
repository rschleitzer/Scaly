namespace scaly {
namespace compiler {
namespace transpiler {

struct Transpiler : Object {

    TranspilerError* program(Page* _ep, Program& program) {
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

                    vscode_files(_ep, path, program.name);

                    bool children_modules_available  = check_for_children_modules(program.concept.definition);

                    if (program.statements.length > 0) {
                        main_file(_ep, path, program, children_modules_available);
                    }


                    StringBuilder& header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
                    StringBuilder& cpp_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
                    if (children_modules_available) {
                        main_include_file(_ep, path, program.concept.name);
                        forward_includes(_ep, path, program);
                        auto _path_result = extend_and_create_path(_r.get_page(), _ep, path, program.concept.name);
                        if (_path_result._tag == Result<String, FileError>::Error)
                            return new(alignof(TranspilerError), _ep) TranspilerError(_path_result._Error);
                        path = _path_result._Ok;
                        StringBuilder& namespace_open_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
                        namespace_open_builder.append_string(String(_r.get_page(), "namespace "));
                        namespace_open_builder.append_string(program.concept.name);
                        namespace_open_builder.append_string(String(_r.get_page(), " {\n"));
                        auto _result = concept(_ep, path, file, header_builder, cpp_builder, namespace_open_builder.to_string(_r.get_page()), String(_r.get_page(), "}\n"), program.concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }

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

    TranspilerError* main_file(Page* _ep, String path, Program& program, bool children_modules_available) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append_string(String(_r.get_page(), "#include \""));
        builder.append_string(program.concept.name);
        builder.append_string(String(_r.get_page(), ".h\"\n\n"));
        builder.append_string(String(_r.get_page(), "int main(int argc, char** argv) {\n"));

        auto iter = VectorIterator<Statement>(&program.statements);
        while(auto statement = iter.next()) {
            build_statement(_ep, builder, statement);
        }
        builder.append_string(String(_r.get_page(), "    return 0;\n}\n"));
        if (builder.get_length() > 0) {
            auto main_file_name = Path::join(_r.get_page(), path, String(_r.get_page(), "main.cpp"));
            auto _main_result = File::write_from_string(_ep, main_file_name, builder.to_string(_r.get_page()));
            if (_main_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_main_result);
        }

        return nullptr;
    }

    TranspilerError* build_statement(Page* _ep, StringBuilder& builder, Statement* statement) {
        return nullptr;
    }

    TranspilerError* module(Page* _ep, String path, const Module& module, String namespace_open, String namespace_close) {
        Region _r;

        auto children_modules_available = check_for_children_modules(module.concept.definition);
        if (children_modules_available) {
            auto _path_result = extend_and_create_path(_r.get_page(), _ep, path, module.concept.name);
            if (_path_result._tag == Result<String, FileError>::Error)
                return new(alignof(TranspilerError), _ep) TranspilerError(_path_result._Error);
            path = _path_result._Ok;
        }

        StringBuilder& header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        StringBuilder& cpp_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();

        if (!children_modules_available) {        
            header_builder.append_string(namespace_open);
        }
        cpp_builder.append_string(namespace_open);

        auto _concept_result = concept(_ep, path, module.file, header_builder, cpp_builder, namespace_open, namespace_close, module.concept);
        if (_concept_result)
            return _concept_result;

        if (!children_modules_available) {        
            header_builder.append_string(namespace_close);
        }
        cpp_builder.append_string(namespace_close);

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

    bool check_for_children_modules(Definition definition) {
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
        auto _create_directory_result = create_directory(_rp, _ep, path);
        if (_create_directory_result != nullptr)
            return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = *_create_directory_result };
        return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = path };
    }

    FileError* create_directory(Page* _rp, Page* _ep, String path) {
        auto _exists_result = Directory::exists(_ep, path);
        if (_exists_result._tag == scaly::containers::Result<bool, FileError>::Error)
            return new(alignof(FileError), _ep) FileError(_exists_result._Error);
        auto exists = _exists_result._Ok;
        if (!exists) {
            auto _result = Directory::create(_ep, path);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* namespace_(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String namespace_open, String namespace_close, const Namespace& namespace_) {
        Region _r;

        auto member_iterator = HashMapIterator<String, Nameable>(namespace_.code.symbols);
        while (auto member = member_iterator.next()) {
            switch(member->_tag) {
                case Nameable::Concept:
                    {
                        auto _result = concept(_ep, path, source, header_builder, cpp_builder, namespace_open, namespace_close, member->_Concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Module:
                    {
                        auto _module = member->_Module;
                        if (!name.equals(String(_r.get_page(), "memory")) || !_module.concept.name.equals(String(_r.get_page(), "Object"))) {
                            header_builder.append_string(String(_r.get_page(), "#include \""));
                            if (check_for_children_modules(_module.concept.definition)) {
                                header_builder.append_string(String(_r.get_page(), _module.concept.name));
                                header_builder.append_character('/');
                            }
                            header_builder.append_string(String(_r.get_page(), _module.concept.name));
                            header_builder.append_string(String(_r.get_page(), ".h\"\n"));
                        }

                        if (check_for_children_modules(_module.concept.definition)) {
                            StringBuilder& namespace_open_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_open);
                            namespace_open_builder.append_string(String(_r.get_page(), "namespace "));
                            namespace_open_builder.append_string(_module.concept.name);
                            namespace_open_builder.append_string(String(_r.get_page(), " {\n"));
                            StringBuilder& namespace_close_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_close);
                            namespace_close_builder.append_string(String(_r.get_page(), "}\n"));
                            auto _result = module(_ep, path, _module, namespace_open_builder.to_string(_r.get_page()), namespace_close_builder.to_string(_r.get_page()));
                            if (_result != nullptr)
                                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                        }
                        else {
                            auto _result = module(_ep, path, _module, namespace_open, namespace_close);
                            if (_result != nullptr)
                                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                        }

                    }
                    break;
                default:
                    // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                    continue;
            }
        }

        return nullptr;
    }

    TranspilerError* concept(Page* _ep, String path, String source, StringBuilder& header_builder, StringBuilder& cpp_builder, String namespace_open, String namespace_close, const Concept& concept) {
        Region _r;
 
        switch (concept.definition._tag) {
            case Definition::Namespace: {
                auto _namespace = concept.definition._Namespace;
                return namespace_(_ep, path, source, concept.name, header_builder, cpp_builder, namespace_open, namespace_close, _namespace);
            }

            case Definition::Structure: {
                auto _structure = concept.definition._Structure;
                return structure(_ep, header_builder, cpp_builder, concept.name, _structure, concept.parameters);
            }
            
            default:
                // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                return nullptr;
        }

        return nullptr;
    }

    TranspilerError* structure(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, const Structure& structure, Vector<GenericParameter> parameters) {
        Region _r;
        header_builder.append_string(String(_r.get_page(), "using namespace scaly::memory;\n"));
        full_struct_name(header_builder, name, parameters);
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

        auto initializers_iterator = VectorIterator<Initializer>(structure.code.initializers);
        while (auto initializer = initializers_iterator.next()) {
            build_initializer(_ep, header_builder, cpp_builder, name, initializer);
        }

        header_builder.append_string(String(_r.get_page(), "\n};\n"));
        return nullptr; 
    }

    TranspilerError* build_initializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Initializer* initializer) {
        Region _r;
        header_builder.append_string(String(_r.get_page(), "\n    "));
        header_builder.append_string(name);
        header_builder.append_character('(');
        StringBuilder& signature_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        auto input_iterator = VectorIterator<Property>(&initializer->input);
        bool first = true;
        while (auto property = input_iterator.next()) {
            if (first) {
                first = false;
            }
            else {
                signature_builder.append_character(',');
                signature_builder.append_character(' ');
            }
        }
        header_builder.append_character(')');
        header_builder.append_character(';');
        return nullptr; 
    }


    void full_struct_name(StringBuilder& builder, String name, Vector<GenericParameter> parameters) {
        Region _r;

        if (parameters.length > 0) {
            builder.append_string(String(_r.get_page(), "template<"));
            {
                auto generic_iterator = VectorIterator<GenericParameter>(&parameters);
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    builder.append_string(String(_r.get_page(), "class "));
                    builder.append_string(generic->name);
                    if (i < parameters.length - 1)
                        builder.append_string(String(_r.get_page(), ", "));
                }
            }
            builder.append_character('>');
            builder.append_character(' ');
        }
        builder.append_string(String(_r.get_page(), "struct "));
        builder.append_string(name);
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

    TranspilerError* forward_includes(Page* _ep, String path, Program& program) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append_string(String(_r.get_page(), "#include \"scaly/memory/Object.h\"\n"));
        builder.append_string(String(_r.get_page(), "namespace "));
        builder.append_string(program.concept.name);
        builder.append_string(String(_r.get_page(), " {\n"));
        auto _result = forward_include(_ep, builder,  program.concept);
        if (_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        builder.append_string(String(_r.get_page(), "}\n"));

        if (builder.get_length() > 0) {
            auto main_file_name = Path::join(_r.get_page(), path, String(_r.get_page(), "forwards.h"));
            auto _main_result = File::write_from_string(_ep, main_file_name, builder.to_string(_r.get_page()));
            if (_main_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_main_result);
        }

        return nullptr;
    }

    TranspilerError* forward_include(Page* _ep, StringBuilder& builder, const Concept& concept) {
        Region _r;
 
        switch (concept.definition._tag) {
            case Definition::Namespace: {
                auto _namespace = concept.definition._Namespace;
                return forward_includes_for_namespace(_ep, concept.name, builder, _namespace);
            }

            case Definition::Structure: {
                auto _structure = concept.definition._Structure;
                if (!concept.name.equals(String(_r.get_page(), "Object"))) {
                    full_struct_name(builder, concept.name, concept.parameters);
                    builder.append_character(';');
                    builder.append_character('\n');
                }
                break;
            }
            
            default:
                // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                return nullptr;
        }

        return nullptr;
    }

    TranspilerError* forward_includes_for_namespace(Page* _ep, String name, StringBuilder& builder, const Namespace& namespace_) {
        Region _r;

        auto member_iterator = HashMapIterator<String, Nameable>(namespace_.code.symbols);
        while (auto member = member_iterator.next()) {
            switch(member->_tag) {
                case Nameable::Concept:
                    {
                        auto _result = forward_include(_ep, builder, member->_Concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Module:
                    {
                        auto _module = member->_Module;
                        bool children_modules_available  = check_for_children_modules(_module.concept.definition);
                        if (children_modules_available) {
                            builder.append_string(String(_r.get_page(), "namespace "));
                            builder.append_string(_module.concept.name);
                            builder.append_string(String(_r.get_page(), " {\n"));
                        }
                        auto _result = forward_include(_ep, builder, _module.concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                        if (children_modules_available) {
                            builder.append_string(String(_r.get_page(), "}\n"));
                        }
                    }
                    break;
                default:
                    // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                    continue;
            }
        }

        return nullptr;
    }

    TranspilerError* vscode_files(Page* _ep, String path, String name){
        Region _r;
        auto vscode_dir = Path::join(_r.get_page(), path, String(_r.get_page(), ".vscode"));
        auto _create_directory_result = create_directory(_r.get_page(), _ep, vscode_dir);
        if (_create_directory_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_create_directory_result);

        {
            StringBuilder& tasks_file_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), vscode_dir, String(_r.get_page(), "tasks.json")));
            StringBuilder& tasks_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(String(_r.get_page(), "{\n\
	\"version\": \"2.0.0\",\n\
	\"tasks\": [\n\
		{\n\
			\"type\": \"cppbuild\",\n\
			\"label\": \"C/C++: clang++ Aktive Datei kompilieren\",\n\
			\"command\": \"/opt/homebrew/opt/llvm/bin/clang++\",\n\
			\"args\": [\n\
				\"-fcolor-diagnostics\",\n\
				\"-fansi-escape-codes\",\n\
				\"-g\",\n\
				\"${workspaceFolder}/main.cpp\",\n\
				\"-o\",\n\
				\"${workspaceFolder}/bin/"));
            tasks_builder.append_string(name);
            tasks_builder.append_string(String(_r.get_page(), "\",\n\
				//\"`llvm-config --cxxflags --ldflags --system-libs --libs core`\"\n\
				\"-I/opt/homebrew/Cellar/llvm/17.0.5/include\",\n\
				\"-std=c++17\",\n\
				\"-stdlib=libc++\",\n\
				\"-D__STDC_CONSTANT_MACROS\",\n\
				\"-D__STDC_FORMAT_MACROS\",\n\
				\"-D__STDC_LIMIT_MACROS\",\n\
				\"-L/opt/homebrew/Cellar/llvm/17.0.5/lib\",\n\
				\"-Wl,-search_paths_first\",\n\
				\"-Wl,-headerpad_max_install_names\",\n\
				\"-lLLVM-17\",\n\
			],\n\
			\"options\": {\n\
				\"cwd\": \"${workspaceFolder}\"\n\
			},\n\
			\"problemMatcher\": [\n\
				\"$gcc\"\n\
			],\n\
			\"group\": {\n\
				\"kind\": \"build\",\n\
				\"isDefault\": true\n\
			},\n\
			\"detail\": \"compiler: /usr/bin/clang++\"\n\
		}\n\
	]\n\
}"));
            auto _tasks_result = File::write_from_string(_ep, tasks_file_builder.to_string(_r.get_page()), tasks_builder.to_string(_r.get_page()));
            if (_tasks_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_tasks_result);
        }

        {
            StringBuilder& launch_file_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), vscode_dir, String(_r.get_page(), "launch.json")));
            StringBuilder& launch_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(String(_r.get_page(), "{\
    \"version\": \"0.2.0\",\n\
    \"configurations\": [\n\
        {\n\
            \"type\": \"lldb\",\n\
            \"request\": \"launch\",\n\
            \"name\": \"Debug\",\n\
            \"program\": \"${workspaceFolder}/bin/"));
            launch_builder.append_string(name);
            launch_builder.append_string(String(_r.get_page(), "\",\n\
            \"args\": [],\n\
            \"cwd\": \"${workspaceFolder}\"\n\
        }\n\
    ]\n\
}"));
            auto _launch_result = File::write_from_string(_ep, launch_file_builder.to_string(_r.get_page()), launch_builder.to_string(_r.get_page()));
            if (_launch_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_launch_result);
        }
        return nullptr;
    }

    TranspilerError* main_include_file(Page* _ep, String path, String name) {
        Region _r;
        StringBuilder& main_include_file_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), path, name));
        main_include_file_builder.append_string(String(_r.get_page(), ".h"));
        StringBuilder& main_include_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(String(_r.get_page(), "#include \"forwards.h\"\n"));
        main_include_builder.append_string(String(_r.get_page(), "#include \""));
        main_include_builder.append_string(name);
        main_include_builder.append_character('/');
        main_include_builder.append_string(name);
        main_include_builder.append_string(String(_r.get_page(), ".h\"\n"));
        auto _main_include_result = File::write_from_string(_ep, main_include_file_builder.to_string(_r.get_page()), main_include_builder.to_string(_r.get_page()));
        if (_main_include_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_main_include_result);
        return nullptr;
    }
};

}
}
}