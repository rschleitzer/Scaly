namespace scaly {
namespace compiler {
namespace transpiler {

struct Transpiler : Object {

    TranspilerError* program(Page* _ep, Program& program) {
        Region _r;

        auto file = program.module.file;
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

        vscode_files(_ep, path, program);

        if (program.statements.length > 0) {
            main_file(_ep, path, program);
        }

        main_include_file(_ep, path, program.module.name);
        forward_includes(_ep, path, program);
        StringBuilder& main_header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(String(_r.get_page(), "../"));
        main_header_builder.append_string(program.module.name);
        StringBuilder& namespace_open_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        namespace_open_builder.append_string(String(_r.get_page(), "namespace "));
        namespace_open_builder.append_string(program.module.name);
        namespace_open_builder.append_string(String(_r.get_page(), " {\n"));
        auto _result = build_module(_ep, path, program.module, main_header_builder.to_string(_r.get_page()), namespace_open_builder.to_string(_r.get_page()), String(_r.get_page(), "}\n"));
        if (_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);

        return nullptr;
    }

    TranspilerError* main_file(Page* _ep, String path, Program& program) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append_string(String(_r.get_page(), "#include \""));
        builder.append_string(program.module.name);
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

    TranspilerError* build_module(Page* _ep, String path, Module& module, String main_header, String namespace_open, String namespace_close) {
        Region _r;

        auto _path_result = extend_and_create_path(_r.get_page(), _ep, path, module.name);
        if (_path_result._tag == Result<String, FileError>::Error)
            return new(alignof(TranspilerError), _ep) TranspilerError(_path_result._Error);
        path = _path_result._Ok;

        StringBuilder& header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        StringBuilder& cpp_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();

        cpp_builder.append_string(String(_r.get_page(), "#include \""));
        cpp_builder.append_string(main_header);
        cpp_builder.append_string(String(_r.get_page(), ".h\"\n"));
        cpp_builder.append_string(namespace_open);

        auto _symbols_result = build_symbols(_ep, path, module.file, module.name, header_builder, cpp_builder, main_header, namespace_open, namespace_close, module.symbols);
        if (_symbols_result)
            return _symbols_result;

        cpp_builder.append_character('\n');
        cpp_builder.append_string(namespace_close);

        auto base_file_name = Path::join(_r.get_page(), path, module.name);
        StringBuilder& header_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(base_file_name);
        header_name_builder.append_string(String(_r.get_page(), ".h"));
        auto header_name = header_name_builder.to_string(_r.get_page());
        auto _header_result = File::write_from_string(_ep, header_name, header_builder.to_string(_r.get_page()));
        if (_header_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_header_result);

        StringBuilder& cpp_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(base_file_name);
        cpp_name_builder.append_string(String(_r.get_page(), ".cpp"));
        auto cpp_name = cpp_name_builder.to_string(_r.get_page());
        auto _cpp_result = File::write_from_string(_ep, cpp_name, cpp_builder.to_string(_r.get_page()));
        if (_cpp_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_cpp_result);

        return nullptr;
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

    TranspilerError* build_concept(Page* _ep, String path, String source, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Concept& concept) {
        Region _r;
 
        switch (concept.definition._tag) {
            case Definition::Namespace: {
                auto _namespace = concept.definition._Namespace;
                return build_namespace(_ep, path, source, concept.name, header_builder, cpp_builder, main_header, namespace_open, namespace_close, _namespace);
            }

            case Definition::Structure: {
                auto _structure = concept.definition._Structure;
                return build_structure(_ep, header_builder, cpp_builder, concept.name, _structure, concept.parameters);
            }

            case Definition::Intrinsic: {
                return build_intrinsic(_ep, header_builder, concept.name);
            }
            
            default:
                // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                return nullptr;
        }

        return nullptr;
    }

    TranspilerError* build_namespace(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, const Namespace& namespace_) {
        return build_symbols(_ep, path, source, name, header_builder, cpp_builder, main_header, namespace_open, namespace_close, namespace_.symbols);
    }

    TranspilerError* build_symbols(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, HashMap<String, Nameable> symbols) {
        Region _r;
        auto member_iterator = HashMapIterator<String, Nameable>(symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Concept:
                    {
                        auto _result = build_concept(_ep, path, source, header_builder, cpp_builder, main_header, namespace_open, namespace_close, member->_Concept);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Module:
                    {
                        auto _module = member->_Module;
                        header_builder.append_string(String(_r.get_page(), "#include \""));
                        header_builder.append_string(String(_r.get_page(), _module.name));
                        header_builder.append_character('/');
                        header_builder.append_string(String(_r.get_page(), _module.name));
                        header_builder.append_string(String(_r.get_page(), ".h\"\n"));
                        StringBuilder& main_header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(String(_r.get_page(), "../"));
                        main_header_builder.append_string(main_header);
                        StringBuilder& namespace_open_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_open);
                        namespace_open_builder.append_string(String(_r.get_page(), "namespace "));
                        namespace_open_builder.append_string(_module.name);
                        namespace_open_builder.append_string(String(_r.get_page(), " {"));
                        StringBuilder& namespace_close_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_close);
                        namespace_close_builder.append_string(String(_r.get_page(), "}"));
                        auto _result = build_module(_ep, path, _module, main_header_builder.to_string(_r.get_page()), namespace_open_builder.to_string(_r.get_page()), namespace_close_builder.to_string(_r.get_page()));
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Functions:
                    {
                        auto _result = build_functions(_ep, header_builder, cpp_builder, member->_Functions, nullptr, false);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    }
                    break;
                case Nameable::Operator:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "namespace local operator")));
                case Nameable::Package:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "namespace local package")));
            }
        }

        return nullptr;
    }

    TranspilerError* build_structure(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Structure& structure, Vector<GenericParameter> parameters) {
        Region _r;
        header_builder.append_string(String(_r.get_page(), "\nusing namespace scaly::memory;\n"));
        full_struct_name(header_builder, name, parameters);
        if (!name.equals(String(_r.get_page(), "Object")))
            header_builder.append_string(String(_r.get_page(), " : Object"));
        header_builder.append_string(String(_r.get_page(), " {"));

        auto property_iterator = HashMapIterator<String, Property>(structure.properties);
        while (auto property = property_iterator.next()) {
            if (property->type) {
                header_builder.append_character('\n');
                header_builder.append_string(String(_r.get_page(), "    "));
                build_type(header_builder, property->type);
            }

            if (property->name)
            {
                header_builder.append_character(' ');
                header_builder.append_string(*property->name);
                header_builder.append_character(';');
            }
        }

        auto initializers_iterator = VectorIterator<Initializer>(&structure.initializers);
        while (auto initializer = initializers_iterator.next()) {
            build_initializer(_ep, header_builder, cpp_builder, name, initializer);
        }

        if (structure.deinitializer != nullptr)
            build_deinitializer(_ep, header_builder, cpp_builder, name, structure.deinitializer);

        auto member_iterator = HashMapIterator<String, Nameable>(structure.symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Functions: {
                        auto _result = build_functions(_ep, header_builder, cpp_builder, member->_Functions, &name, parameters.length > 0);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    }
                    break;
                case Nameable::Operator: {
                        auto _result = build_operator(_ep, header_builder, cpp_builder, member->_Operator);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    }
                    break;
                case Nameable::Package:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local package")));
                case Nameable::Module:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local module")));
                case Nameable::Concept:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local concept")));
            }
        }

        header_builder.append_string(String(_r.get_page(), "\n};\n"));
        return nullptr; 
    }

    String build_uses(Page* _rp, Vector<Use>& uses) {
        Region _r;
        StringBuilder& builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        auto uses_iterator = VectorIterator<Use>(&uses);
        while (auto use = uses_iterator.next()) {
            builder.append_string(String(_r.get_page(), "\nusing namespace "));
            bool first = true;
            auto namespace_iterator = VectorIterator<String>(&use->path);
            while(auto namespace_ = namespace_iterator.next()) {
                if (first) {
                    first = false;
                }
                else {
                    builder.append_character(':');
                    builder.append_character(':');
                }
                builder.append_string(*namespace_);
            }
            builder.append_character(';');
            builder.append_character('\n');
        }
        return builder.to_string(_rp);
    }

    TranspilerError* build_functions(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Vector<Function>& functions, String* name, bool isTemplate) {
        Region _r;
        auto function_iterator = VectorIterator<Function>(&functions);
        bool first = true;
        while (auto function = function_iterator.next()) {
            function_prefix(header_builder, function, name != nullptr);
            if (!isTemplate) {
                function_prefix(cpp_builder, function, false);
                if (name != nullptr) {
                    cpp_builder.append_string(*name);
                    cpp_builder.append_character(':');
                    cpp_builder.append_character(':');
                }
                cpp_builder.append_string(function->name);
                build_input(cpp_builder, function->input);
                cpp_builder.append_character(' ');
                cpp_builder.append_character('{');
                cpp_builder.append_character('}');
                cpp_builder.append_character(';');
            }
            header_builder.append_string(function->name);
            build_input(header_builder, function->input);
            if (isTemplate) {
                header_builder.append_character(' ');
                header_builder.append_character('{');
                header_builder.append_character('}');
            }
            header_builder.append_character(';');
        }
        return nullptr; 
    }

    void function_prefix(StringBuilder& builder, Function* function, bool indent) {
        Region _r;
        builder.append_character('\n');
        if (indent)
            builder.append_string(String(_r.get_page(), "    "));
        if (function->output.length == 0)
            builder.append_string(String(_r.get_page(), "void"));
        else
            build_type(builder, function->output[0]->type);
        builder.append_character(' ');
    }

    TranspilerError* build_operator(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Operator& operator_) {
        Region _r;
        header_builder.append_string(String(_r.get_page(), "\n    "));
        if (operator_.output.length == 0)
            header_builder.append_string(String(_r.get_page(), "void"));
        else
            build_type(header_builder, operator_.output[0]->type);
        header_builder.append_string(String(_r.get_page(), " operator "));
        header_builder.append_string(operator_.name);
        build_input(header_builder, operator_.input);
        header_builder.append_character(';');
        return nullptr; 
    }

    TranspilerError* build_initializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Initializer* initializer) {
        Region _r;
        header_builder.append_string(String(_r.get_page(), "\n    "));
        header_builder.append_string(name);
        build_input(header_builder, initializer->input);
        header_builder.append_character(';');
        return nullptr; 
    }

    TranspilerError* build_deinitializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, DeInitializer* deInitializer) {
        Region _r;
        header_builder.append_string(String(_r.get_page(), "\n    "));
        header_builder.append_character('~');
        header_builder.append_string(name);
        header_builder.append_character('(');
        header_builder.append_character(')');
        header_builder.append_character(';');
        return nullptr; 
    }

    bool build_input(StringBuilder& builder, Vector<Property> input) {
        Region _r;
        builder.append_character('(');
        auto input_iterator = VectorIterator<Property>(&input);
        bool first = true;
        bool isStatic = true;
        while (auto property = input_iterator.next()) {
            if (first) {
                if (property->name->equals(String(_r.get_page(), "this"))) {
                    isStatic = false;
                    continue;
                }
                first = false;
            }
            else {
                builder.append_character(',');
                builder.append_character(' ');
            }
            if (property->type != nullptr) {
                build_type(builder, property->type);
                builder.append_character(' ');
            }
            builder.append_string(*property->name);
        }
        builder.append_character(')');
        return isStatic;
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

    void build_type(StringBuilder& builder, Type* type) {
        Region _r;
        if (type->name.length == 1 && type->name[0]->equals(String(_r.get_page(), "pointer"))) {
            auto generic_iterator = VectorIterator<Type>(&type->generics);
            while(auto generic = generic_iterator.next()) {
                build_type(builder, generic);
                break;
            }
            builder.append_character('*');
            return;
        }

        {
            auto type_name_iterator = VectorIterator<String>(&type->name);
            size_t i = 0;
            while (auto name_part = type_name_iterator.next()) {
                builder.append_string(*name_part);
                if (i < type->name.length - 1)
                    builder.append_string(String(_r.get_page(), "::"));
                i++;
            }
        }

        if (type->generics.length > 0) {
            builder.append_character('<');
            {
                auto generic_iterator = VectorIterator<Type>(&type->generics);
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    build_type(builder, generic);
                    if (i < type->generics.length - 1)
                        builder.append_string(String(_r.get_page(), ", "));
                }
            }
            builder.append_character('>');
        }
    }

    TranspilerError* build_intrinsic(Page* _ep, StringBuilder& header_builder, String name) {
        Region _r;

        header_builder.append_string(String(_r.get_page(), "typedef "));
        if (name.equals(String(_r.get_page(), "size_t")))
            header_builder.append_string(String(_r.get_page(), "__SIZE_TYPE__"));
        else
            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, name)));
            
        header_builder.append_character(' ');
        header_builder.append_string(name);
        header_builder.append_character(';');
        header_builder.append_character('\n');

        return nullptr;
    }

    TranspilerError* forward_includes(Page* _ep, String path, Program& program) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append_string(String(_r.get_page(), "namespace scaly { namespace memory { struct Page; } }\n"));
        builder.append_string(String(_r.get_page(), "typedef __SIZE_TYPE__ size_t;\n"));
        builder.append_string(String(_r.get_page(), "typedef const char const_char;\n"));
        builder.append_string(String(_r.get_page(), "#include \"scaly/memory/Object.h\"\n"));
        builder.append_string(String(_r.get_page(), "namespace "));
        builder.append_string(program.module.name);
        builder.append_string(String(_r.get_page(), " {\n"));
        auto _result = forward_includes_for_symbols(_ep, builder,  program.module.symbols);
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
                return nullptr;
            }
            
            default:
                // return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented());
                return nullptr;
        }

        return nullptr;
    }

    TranspilerError* forward_includes_for_namespace(Page* _ep, String name, StringBuilder& builder, const Namespace& namespace_) {
        Region _r;
        builder.append_string(String(_r.get_page(), "namespace "));
        builder.append_string(name);
        builder.append_string(String(_r.get_page(), " {\n"));
        auto _result = forward_includes_for_symbols(_ep, builder, namespace_.symbols);
        if (_result != nullptr)
            return _result;
        builder.append_string(String(_r.get_page(), "}\n"));
        return nullptr;
    }

    TranspilerError* forward_includes_for_symbols(Page* _ep, StringBuilder& builder, HashMap<String, Nameable> symbols) {
        Region _r;
        auto member_iterator = HashMapIterator<String, Nameable>(symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
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
                        auto _result = forward_includes_for_symbols(_ep, builder, _module.symbols);
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

    TranspilerError* vscode_files(Page* _ep, String path, Program& program) {
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
				\"${workspaceFolder}/main.cpp\",\n"));
            build_vscode_source_files(tasks_builder, String(), program.module);
            tasks_builder.append_string(String(_r.get_page(), "\
				\"-o\",\n\
				\"${workspaceFolder}/bin/"));
            tasks_builder.append_string(program.module.name);
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
            launch_builder.append_string(program.module.name);
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

    void build_vscode_source_files(StringBuilder& builder, String path, Module& module) {
        Region _r;
        builder.append_string(String(_r.get_page(), "				\"${workspaceFolder}/"));
        builder.append_string(path);
        builder.append_string(module.name);
        builder.append_string(String(_r.get_page(), ".cpp\",\n"));
        StringBuilder& path_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(path);
        path_builder.append_string(module.name);
        path_builder.append_character('/');
        build_vscode_source_files_list(builder, path_builder.to_string(_r.get_page()), module.symbols);
    }


    void build_vscode_source_files_list(StringBuilder& builder, String path, HashMap<String, Nameable> symbols) {
        Region _r;
        auto member_iterator = HashMapIterator<String, Nameable>(symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Concept:
                    {
                        auto concept = member->_Concept;
                        switch (concept.definition._tag) {
                            case Definition::Namespace: {
                                auto namespace_ = concept.definition._Namespace;
                                build_vscode_source_files_list(builder, path, namespace_.symbols);
                            }
                            break;

                            case Definition::Structure: {
                                auto structure = concept.definition._Structure;
                                build_vscode_source_files_list(builder, path, structure.symbols);
                            }
                            break;
                            
                            default:
                                break;
                        }
                    }
                    break;
                case Nameable::Module:
                    {
                        auto module = member->_Module;
                        build_vscode_source_files(builder, path, module);
                    }
                    break;
                default:
                    continue;
            }
        }
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