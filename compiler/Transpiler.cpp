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
        auto _result = build_module(_ep, path, program.module, program.module.name, String(), String());
        if (_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);

        if (program.statements.length > 0) {
            main_file(_ep, path, program);
        }
        main_include_file(_ep, path, program.module.name);
        forward_includes(_ep, path, program);
        vscode_files(_ep, path, program);

        return nullptr;
    }

    TranspilerError* build_module(Page* _ep, String path, Module& module, String main_header, String namespace_open, String namespace_close) {
        Region _r;

        auto _path_result = create_directory(_ep, path);
        if (_path_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_path_result);

        path = Path::join(_r.get_page(), path, module.name);

        StringBuilder& header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        StringBuilder& cpp_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();

        cpp_builder.append("#include \"");
        cpp_builder.append(main_header);
        cpp_builder.append(".h\"");
        cpp_builder.append(namespace_open);

        header_builder.append("#ifndef _");
        header_builder.append(module.name);
        header_builder.append("_h\n");
        header_builder.append("#define _");
        header_builder.append(module.name);
        header_builder.append("_h\n");
        if (module.name.equals("scaly")) {
            header_builder.append("#include \"forwards.h\"\n");
        }
        else {
            header_builder.append("#include \"");
            header_builder.append(main_header);
            header_builder.append(".h\"\n");
        }
        if (!module.name.equals("scaly") && !module.name.equals("memory"))
            header_builder.append("using namespace scaly::memory;\n");
        
        build_uses(header_builder, module.uses);
        build_uses(cpp_builder, module.uses);

        StringBuilder& namespace_open_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_open);

        bool leaf_module = is_leaf_module(module);
        if (!leaf_module) {
            header_builder.append("namespace ");
            header_builder.append(module.name);
            header_builder.append(" {\n");
        }

        auto _symbols_result = build_symbols(_ep, path, module.file, module.name, header_builder, cpp_builder, main_header, namespace_open, namespace_close, module.symbols);
        if (_symbols_result)
            return _symbols_result;

        cpp_builder.append('\n');
        cpp_builder.append(namespace_close);

        if (!leaf_module) {
            header_builder.append("\n}");
        }
        header_builder.append("\n#endif");

        StringBuilder& header_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(path);
        header_name_builder.append(".h");
        auto header_name = header_name_builder.to_string(_r.get_page());
        auto _header_result = File::write_from_string(_ep, header_name, header_builder.to_string(_r.get_page()));
        if (_header_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_header_result);

        StringBuilder& cpp_name_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(path);
        cpp_name_builder.append(".cpp");
        auto cpp_name = cpp_name_builder.to_string(_r.get_page());
        auto _cpp_result = File::write_from_string(_ep, cpp_name, cpp_builder.to_string(_r.get_page()));
        if (_cpp_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_cpp_result);

        return nullptr;
    }

    FileError* create_directory(Page* _ep, String path) {
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

    bool is_leaf_module(const Module& module) {
        if (module.modules.length > 0)
            return false;
        auto member_iterator = HashMapIterator<String, Nameable>(module.symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Concept: {
                    auto concept = member->_Concept;
                    switch (concept.definition._tag) {
                        case Definition::Namespace: {
                            auto namespace_ = concept.definition._Namespace;
                            if (namespace_.modules.length > 0)
                                return false;
                            break;
                        }
                        
                        default:
                            break;
                    }
                    break;
                }
                default:
                    continue;
            }
        }
        return true;
    }

    TranspilerError* build_concept(Page* _ep, String path, String source, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Concept& concept) { 
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

            case Definition::Constant: {
                auto operation = concept.definition._Constant;
                return build_constant_definition(_ep, header_builder, concept.name, operation);
            }

            case Definition::Union: {
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Union")));
            }
        }

        return nullptr;
    }

    TranspilerError* build_namespace(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Namespace& namespace_) {
        Region _r;
        StringBuilder& namespace_open_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_open);
        namespace_open_builder.append("\nnamespace ");
        namespace_open_builder.append(name);
        namespace_open_builder.append(" {");
        namespace_open = namespace_open_builder.to_string(_r.get_page());
        StringBuilder& namespace_close_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_close);
        namespace_close_builder.append("\n}");
        namespace_close = namespace_close_builder.to_string(_r.get_page());
        StringBuilder& main_header_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder("../");
        main_header_builder.append(main_header);
        {
            auto _result = build_modules(_ep, path, name, header_builder, namespace_.modules, main_header_builder.to_string(_r.get_page()), namespace_open, namespace_close);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }
        {
            auto _result = build_symbols(_ep, path, source, name, header_builder, cpp_builder, main_header_builder.to_string(_r.get_page()), namespace_open, namespace_close, namespace_.symbols);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }

        return nullptr;
    }

    TranspilerError* build_modules(Page* _ep, String path, String name, StringBuilder& header_builder, Vector<Module>& modules, String main_header, String namespace_open, String namespace_close) {
        Region _r;
        auto module_iterator = VectorIterator<Module>(&modules);
        while (auto module = module_iterator.next()) {
            header_builder.append("#include \"");
            header_builder.append(name);
            header_builder.append('/');
            header_builder.append(String(_r.get_page(), module->name));
            header_builder.append(".h\"\n");
            StringBuilder& namespace_close_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_close);
            auto _result = build_module(_ep, path, *module, main_header, namespace_open, namespace_close);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }
        return nullptr;
    }

    TranspilerError* build_symbols(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, HashMap<String, Nameable> symbols) {
        Region _r;
        auto member_iterator = HashMapIterator<String, Nameable>(symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Functions: {
                    auto _result = build_functions(_ep, header_builder, cpp_builder, member->_Functions, &name, false);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    break;
                }
                case Nameable::Concept: {
                    auto _result = build_concept(_ep, path, source, header_builder, cpp_builder, main_header, namespace_open, namespace_close, member->_Concept);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    break;
                }
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
        header_builder.append('\n');
        full_struct_name(header_builder, name, parameters);
        if (!name.equals(String(_r.get_page(), "Object")))
            header_builder.append(" : Object");
        header_builder.append(" {");

        bool must_build_default_initializer = false;
        auto property_iterator = HashMapIterator<String, Property>(structure.properties);
        while (auto property = property_iterator.next()) {
            if (property->type) {
                header_builder.append('\n');
                header_builder.append("    ");
                build_type(header_builder, property->type);
            }

            if (property->name)
            {
                header_builder.append(' ');
                header_builder.append(*property->name);
                header_builder.append(';');
            }

            if (property->initializer != nullptr)
                must_build_default_initializer = true;
        }

        if (must_build_default_initializer)
            build_default_initializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, structure.properties);

        auto initializers_iterator = VectorIterator<Initializer>(&structure.initializers);
        while (auto initializer = initializers_iterator.next()) {
            build_initializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, initializer);
        }

        if (structure.deinitializer != nullptr)
            build_deinitializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, structure.deinitializer);

        auto member_iterator = HashMapIterator<String, Nameable>(structure.symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Functions: {
                    auto _result = build_functions(_ep, header_builder, cpp_builder, member->_Functions, &name, parameters.length > 0);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    break;
                }
                case Nameable::Operator: {
                        auto _result = build_operator(_ep, header_builder, cpp_builder, member->_Operator, &name);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    }
                    break;
                case Nameable::Package:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local package")));
                case Nameable::Concept:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local concept")));
            }
        }

        header_builder.append("\n};\n");
        return nullptr; 
    }

    TranspilerError* build_default_initializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, HashMap<String, Property>& properties) {
        Region _r;
        build_initializer_header(header_builder, cpp_builder, name, is_generic);
        if (is_generic) {
            build_initializer_list(_ep, header_builder, is_generic, properties, String(_r.get_page(), "        "));
            header_builder.append(" {}");
        }
        else {
            build_initializer_list(_ep, cpp_builder, is_generic, properties, String(_r.get_page(), "    "));
            cpp_builder.append(" {}");
            header_builder.append(" ();");
        }
        return nullptr;
    }

    TranspilerError* build_initializer_list(Page* _ep, StringBuilder& builder, bool is_generic, HashMap<String, Property>& properties, String indent) {
        builder.append("() : ");
        auto property_iterator = HashMapIterator<String, Property>(properties);
        bool first = true;
        while (auto property = property_iterator.next()) {
            if (property->initializer == nullptr)
                continue;
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            }
            builder.append(property->name[0]);
            builder.append('(');
            auto _result = build_operation(_ep, builder, *property->initializer, indent);
            if (_result != nullptr)
                return _result;
            builder.append(')');
        }
        return nullptr;
    }

    TranspilerError* build_initializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Initializer* initializer) {
        Region _r;
        build_initializer_header(header_builder, cpp_builder, name, is_generic);
        if (!is_generic) {
            build_input(cpp_builder, initializer->input, Lifetime(Unspecified()));
            cpp_builder.append(' ');
            cpp_builder.append('{');
            cpp_builder.append("};");
        }
        build_input(header_builder, initializer->input, Lifetime(Unspecified()));
        if (is_generic) {
            header_builder.append('{');
            header_builder.append("};");
        }
        else {
            header_builder.append(';');
        }

        return nullptr; 
    }

    void build_initializer_header(StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic) {
        header_builder.append("\n    ");
        header_builder.append(name);
        if (!is_generic) {
            cpp_builder.append("\n\n");
            cpp_builder.append(name);
            cpp_builder.append("::");
            cpp_builder.append(name);
        }        
    }
    TranspilerError* build_deinitializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, DeInitializer* deInitializer) {
        Region _r;
        header_builder.append("\n    ");
        header_builder.append('~');
        header_builder.append(name);
        if (!is_generic) {
            cpp_builder.append('\n');
            cpp_builder.append(name);
            cpp_builder.append("::");
            cpp_builder.append('~');
            cpp_builder.append(name);
            cpp_builder.append("() {");
            cpp_builder.append("};");
        }
        if (is_generic) {
            header_builder.append("() {");
            header_builder.append("};");
        }
        else {
            header_builder.append("();");
        }

        return nullptr; 
    }

    void build_uses(StringBuilder& builder, Vector<Use>& uses) {
        Region _r;
        auto uses_iterator = VectorIterator<Use>(&uses);
        while (auto use = uses_iterator.next()) {
            builder.append("using namespace ");
            bool first = true;
            auto namespace_iterator = VectorIterator<String>(&use->path);
            while(auto namespace_ = namespace_iterator.next()) {
                if (first) {
                    first = false;
                }
                else {
                    builder.append("::");
                }
                builder.append(*namespace_);
            }
            builder.append(";\n");
        }
    }

    TranspilerError* build_functions(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Vector<Function>& functions, String* name, bool isTemplate) {
        Region _r;
        auto function_iterator = VectorIterator<Function>(&functions);
        bool first = true;
        while (auto function = function_iterator.next()) {
            function_prefix(header_builder, function, name != nullptr, true);
            if (!isTemplate) {
                cpp_builder.append('\n');
                function_prefix(cpp_builder, function, false, false);
                if (name != nullptr) {
                    cpp_builder.append(*name);
                    cpp_builder.append("::");
                }
                cpp_builder.append(function->name);
                build_input(cpp_builder, function->input, function->lifetime);
                auto _result = build_implementation(_ep, cpp_builder, function->implementation, String());
                if (_result != nullptr)
                    return _result;
            }
            header_builder.append(function->name);
            build_input(header_builder, function->input, function->lifetime);
            if (isTemplate) {
                auto _result = build_implementation(_ep, header_builder, function->implementation, String());
                if (_result != nullptr)
                    return _result;
            }
            header_builder.append(';');
        }
        return nullptr; 
    }

    TranspilerError* build_implementation(Page* _ep, StringBuilder& builder, Implementation& implementation, String indent) {
        switch (implementation._tag) {
            case Implementation::Action: {
                auto action = implementation._Action;
                auto _result = build_action(_ep, builder, action, indent);
                if (_result != nullptr)
                    return _result;
                break;
            }
            case Implementation::Extern:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Extern")));
            case Implementation::Instruction:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Instruction")));
            case Implementation::Intrinsic:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Intrinsic")));

        }
        return nullptr;
    }

    TranspilerError* build_binding(Page* _ep, StringBuilder& builder, Binding& binding, String indent) {
        builder.append('\n');
        builder.append(indent);
        switch (binding.binding_type) {
            case Binding::Constant: {
                builder.append("const ");
                break;
            }
            case Binding::Extendable: {
                break;
            }
            case Binding::Mutable: {
                break;
            }
        }
        if (binding.item.type != nullptr)
            build_type(builder, binding.item.type);
        else
            builder.append("auto");

        if (binding.item.name != nullptr) {
            builder.append(' ');
            builder.append(*binding.item.name);
        }

        builder.append(" = ");

        auto _result = build_operation(_ep, builder, binding.operation, indent);
        if (_result != nullptr)
            return _result;
        builder.append(';');
        return nullptr;
    }

    TranspilerError* build_action(Page* _ep, StringBuilder& builder, Action& action, String indent) {
        builder.append('\n');
        builder.append(indent);
        if (action.target.length > 0) {
            {
                auto _result = build_operation(_ep, builder, action.target, indent);
                if (_result != nullptr)
                    return _result;
            }
            builder.append(" = ");
        }
        {
            auto _result = build_operation(_ep, builder, action.source, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(';');
        return nullptr;
    }

    TranspilerError* build_operation(Page* _ep, StringBuilder& builder, Vector<Operand>& operation, String indent) {
        auto operation_iterator = VectorIterator<Operand>(&operation);
        while (auto operand = operation_iterator.next()) {
            switch (operand->expression._tag) {
                case Expression::Constant: {
                    auto constant = operand->expression._Constant;
                    auto _result = build_constant(_ep, builder, constant);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Type: {
                    auto type = operand->expression._Type;
                    auto _result = build_variable(_ep, builder, type, *operand->postfixes);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Tuple: {
                    auto tuple = operand->expression._Tuple;
                    auto _result = build_tuple(_ep, builder, tuple, *operand->postfixes, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Matrix: {
                    auto matrix = operand->expression._Matrix;
                    auto _result = build_matrix(_ep, builder, matrix, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Block: {
                    auto block = operand->expression._Block;
                    auto _result = build_block(_ep, builder, block, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::If: {
                    auto if_ = operand->expression._If;
                    auto _result = build_if(_ep, builder, if_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::For: {
                    auto for_ = operand->expression._For;
                    auto _result = build_for(_ep, builder, for_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::While: {
                    auto while_ = operand->expression._While;
                    auto _result = build_while(_ep, builder, while_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::SizeOf: {
                    auto sizeof_ = operand->expression._SizeOf;
                    auto _result = build_sizeof(_ep, builder, sizeof_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Return: {
                    auto return_ = operand->expression._Return;
                    auto _result = build_return(_ep, builder, return_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
            }
        }
        return nullptr;
    }

    TranspilerError* build_constant(Page* _ep, StringBuilder& builder, Constant& constant) {
        Region _r;
        switch (constant._tag) {
            case Constant::Boolean: {
                auto boolean = constant._Boolean;
                if (boolean)
                    builder.append("true");
                else
                    builder.append("false");
                break;
            }
            case Constant::Integer: {
                auto integer = constant._Integer;
                char str[32];
                snprintf(str, 32, "%zd", integer);
                builder.append(String(_r.get_page(), str));
                break;
            }

            case Constant::Hex:  {
                auto integer = constant._Integer;
                char str[32];
                snprintf(str, 32, "0x%zx", integer);
                builder.append(String(_r.get_page(), str));
                break;
            }

            case Constant::FloatingPoint: {
                auto floating_point = constant._FloatingPoint;
                char str[32];
                snprintf(str, 32, "%lg", floating_point);
                builder.append(String(_r.get_page(), str));
                break;
            }
            case Constant::String: {
                auto string = constant._String;
                builder.append('\"');
                builder.append(string);
                builder.append('\"');
            }
            case Constant::Fragment: {
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Bool")));
            }
        }

        return nullptr;
    }

    TranspilerError* build_variable(Page* _ep, StringBuilder& builder, Type& type, Vector<Postfix>& postfixes) {
        auto name_iterator = VectorIterator<String>(&type.name);
        auto first_name_part = name_iterator.next();
        if (first_name_part->equals("=")) {
            builder.append(" == ");
            return nullptr;
        }            
        if (first_name_part->equals("<>")) {
            builder.append(" != ");
            return nullptr;
        }            
        if (first_name_part->equals("null")) {
            builder.append("nullptr");
            return nullptr;
        }

        switch (type.lifetime._tag) {
            case Lifetime::Unspecified:
                break;
            case Lifetime::Root:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Root")));
            case Lifetime::Local: {
                auto local = type.lifetime._Local;
                builder.append("new (alignof(");
                builder.append(*first_name_part);
                builder.append("), _");
                builder.append(local.location);
                builder.append("p) ");
                break;
            }
            case Lifetime::Reference:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Reference")));
            case Lifetime::Thrown:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Thrown")));
        }

        builder.append(*first_name_part);

        while (auto extension = name_iterator.next()) {
            if (first_name_part->equals("this"))
                builder.append("->");
            else
                builder.append('.');
            builder.append(*extension);
        }

        auto postfixes_iterator = VectorIterator<Postfix>(&postfixes);
        auto postfix = postfixes_iterator.next();
        if (postfix != nullptr) {
            switch (postfix->_tag) {
                case Postfix::Catcher:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Catcher")));
                case Postfix::MemberAccess: {
                    if (first_name_part->equals("this"))
                        builder.append("->");
                    else
                        builder.append('.');
                    auto member_access = postfix->_MemberAccess;
                    auto member_access_iterator = VectorIterator<String>(&member_access);
                    auto member = member_access_iterator.next();
                    builder.append(*member);
                    return nullptr;
                }
            }
        }

        return nullptr;
    }

    TranspilerError* build_tuple(Page* _ep, StringBuilder& builder, Tuple& tuple, Vector<Postfix>& postfixes, String indent) {
        auto tuple_iterator = VectorIterator<Component>(&tuple.components);
        bool first = true;
        bool isStatic = true;
        builder.append('(');
        while (auto property = tuple_iterator.next()) {
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            }
            build_operation(_ep, builder, property->value, indent);
        }
        builder.append(')');

        auto postfixes_iterator = VectorIterator<Postfix>(&postfixes);
        auto postfix = postfixes_iterator.next();
        if (postfix != nullptr) {
            switch (postfix->_tag) {
                case Postfix::Catcher:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Catcher")));
                case Postfix::MemberAccess: {
                    builder.append('.');
                    auto member_access = postfix->_MemberAccess;
                    auto member_access_iterator = VectorIterator<String>(&member_access);
                    auto member = member_access_iterator.next();
                    builder.append(*member);
                    return nullptr;
                }
            }
        }

        return nullptr;
    }

    TranspilerError* build_matrix(Page* _ep, StringBuilder& builder, Matrix& matrix, String indent) {
        auto operations_iterator = VectorIterator<Vector<Operand>>(&matrix.operations);
        bool first = true;
        bool isStatic = true;
        builder.append('[');
        while (auto operation = operations_iterator.next()) {
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            }
            build_operation(_ep, builder, *operation, indent);
        }
        builder.append(']');
        return nullptr;
    }

    TranspilerError* build_block(Page* _ep, StringBuilder& builder, Block& block, String indent) {
        Region _r;
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        indent_builder.append("    ");
        builder.append('{');
        {
            auto _result = build_statements(_ep, builder, block.statements, indent_builder.to_string(_r.get_page()));
            if (_result != nullptr)
                return _result;
        }
        builder.append('\n');
        builder.append(indent);
        builder.append('}');
        return nullptr;
    }

    TranspilerError* build_if(Page* _ep, StringBuilder& builder, If& if_, String indent) {
        Region _r;
        builder.append("if (");
        {
            auto _result = build_operation(_ep, builder, if_.condition, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(")");
        {
            auto _result = build_action(_ep, builder, if_.consequent, indent);
            if (_result != nullptr)
                return _result;
        }
        if (if_.alternative != nullptr) {
            builder.append('\n');
            builder.append(indent);
            builder.append("else");
            {
                auto _result = build_action(_ep, builder, *if_.alternative, indent);
                if (_result != nullptr)
                    return _result;
            }
        }

        return nullptr;
    }

    TranspilerError* build_for(Page* _ep, StringBuilder& builder, For& for_, String indent) {
        Region _r;
        builder.append('\n');
        builder.append(indent);
        builder.append("auto _");
        builder.append(for_.identifier);
        builder.append("_iterator = ");
        build_operation(_ep, builder, for_.expression, indent);
        builder.append(".get_iterator();\n");
        builder.append(indent);
        builder.append("while (auto _");
        builder.append(for_.identifier);
        builder.append(" = _");
        builder.append(for_.identifier);
        builder.append("_iterator.next()) {\n");
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        indent_builder.append("    ");
        String indented = indent_builder.to_string(_r.get_page());
        builder.append(indented);
        builder.append("auto ");
        builder.append(for_.identifier);
        builder.append(" = _*");
        builder.append(for_.identifier);
        builder.append(";");
        {
            auto _result = build_action(_ep, builder, for_.action, indented);
            if (_result != nullptr)
                return _result;
        }
        builder.append('\n');
        builder.append(indent);
        builder.append('}');

        return nullptr;
    }

    TranspilerError* build_while(Page* _ep, StringBuilder& builder, While& while_, String indent) {
        Region _r;
        builder.append('\n');
        builder.append(indent);
        builder.append("while (");
        {
            auto _result = build_operation(_ep, builder, while_.condition, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(")");
        {
            auto _result = build_action(_ep, builder, while_.action, indent);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* build_sizeof(Page* _ep, StringBuilder& builder, SizeOf& sizeof_, String indent) {
        Region _r;
        builder.append("sizeof(");
        build_type(builder, &sizeof_.type);
        builder.append(")");
        return nullptr;
    }

    TranspilerError* build_return(Page* _ep, StringBuilder& builder, Return& return_, String indent) {
        Region _r;
        builder.append('\n');
        builder.append(indent);
        builder.append("return");
        if (return_.result.length > 0)
            builder.append(' ');
        {
            auto _result = build_operation(_ep, builder, return_.result, indent);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* build_statements(Page* _ep, StringBuilder& builder, Vector<Statement>& statements, String indent) {
        auto statment_iterator = VectorIterator<Statement>(&statements);
        while (auto statement = statment_iterator.next()) {
            switch (statement->_tag) {
                case Statement::Action: {
                    auto action = statement->_Action;
                    auto _result = build_action(_ep, builder, action, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Statement::Binding:{
                    auto binding = statement->_Binding;
                    auto _result = build_binding(_ep, builder, binding, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
            }
        }
        return nullptr;
    }

    void function_prefix(StringBuilder& builder, Function* function, bool indent, bool static_if_applicable) {
        Region _r;
        builder.append('\n');
        if (indent)
            builder.append("    ");
        if (static_if_applicable && ((function->input.length == 0) || (function->input[0]->name == nullptr) || (!function->input[0]->name->equals("this"))))
            builder.append("static ");
        if (function->output.length == 0)
            builder.append("void");
        else
            build_type(builder, function->output[0]->type);
        builder.append(' ');
    }

    TranspilerError* build_operator(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Operator& operator_, String* name) {
        Region _r;
        header_builder.append("\n    ");
        if (operator_.output.length == 0)
            header_builder.append("void");
        else
            build_type(header_builder, operator_.output[0]->type);
        header_builder.append(" operator ");
        header_builder.append(operator_.name);
        build_input(header_builder, operator_.input, Lifetime(Unspecified()));
        header_builder.append(';');

        cpp_builder.append("\n\n");
        if (operator_.output.length == 0)
            cpp_builder.append("void");
        else
            build_type(cpp_builder, operator_.output[0]->type);

        cpp_builder.append(' ');
        cpp_builder.append(*name);
        cpp_builder.append("::operator ");
        cpp_builder.append(operator_.name);
        auto is_static = build_input(cpp_builder, operator_.input, Lifetime(Unspecified()));
        auto _result = build_implementation(_ep, cpp_builder, operator_.implementation, String());
        if (_result != nullptr)
            return _result;

        return nullptr; 
    }

    bool build_input(StringBuilder& builder, Vector<Item> input, Lifetime lifetime) {
        Region _r;
        builder.append('(');

        switch (lifetime._tag) {
            case Lifetime::Unspecified:
                break;
            case Lifetime::Root:
                break;
            case Lifetime::Local: {
                auto local = lifetime._Local;
                builder.append("Page* _");
                builder.append(local.location);
                builder.append("p, ");
                break;
            }
            case Lifetime::Reference:
                break;
            case Lifetime::Thrown:
                break;
        }

        auto input_iterator = VectorIterator<Item>(&input);
        bool first = true;
        bool is_static = true;
        while (auto property = input_iterator.next()) {
            if (first) {
                if (property->name->equals(String(_r.get_page(), "this"))) {
                    is_static = false;
                    continue;
                }
                first = false;
            }
            else {
                builder.append(", ");
            }
            if (property->type != nullptr) {
                build_type(builder, property->type);
                builder.append(' ');
            }
            builder.append(*property->name);
        }
        builder.append(')');
        return is_static;
    }

    void full_struct_name(StringBuilder& builder, String name, Vector<GenericParameter> parameters) {
        Region _r;

        if (parameters.length > 0) {
            builder.append("template<");
            {
                auto generic_iterator = VectorIterator<GenericParameter>(&parameters);
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    builder.append("class ");
                    builder.append(generic->name);
                    if (i < parameters.length - 1)
                        builder.append(", ");
                }
            }
            builder.append("> ");
        }
        builder.append("struct ");
        builder.append(name);
    }

    void build_type(StringBuilder& builder, Type* type) {
        Region _r;
        if (type->name.length == 1 && type->name[0]->equals(String(_r.get_page(), "pointer"))) {
            auto generic_iterator = VectorIterator<Type>(&type->generics);
            while(auto generic = generic_iterator.next()) {
                build_type(builder, generic);
                break;
            }
            builder.append('*');
            return;
        }

        {
            auto type_name_iterator = VectorIterator<String>(&type->name);
            size_t i = 0;
            while (auto name_part = type_name_iterator.next()) {
                builder.append(*name_part);
                if (i < type->name.length - 1)
                    builder.append("::");
                i++;
            }
        }

        if (type->generics.length > 0) {
            builder.append('<');
            {
                auto generic_iterator = VectorIterator<Type>(&type->generics);
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    build_type(builder, generic);
                    if (i < type->generics.length - 1)
                        builder.append(", ");
                }
            }
            builder.append('>');
        }
    }

    TranspilerError* build_intrinsic(Page* _ep, StringBuilder& header_builder, String name) {
        Region _r;

        header_builder.append("typedef ");
        if (name.equals(String(_r.get_page(), "size_t")))
            header_builder.append("__SIZE_TYPE__");
        else
            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, name)));
            
        header_builder.append(' ');
        header_builder.append(name);
        header_builder.append(";\n");

        return nullptr;
    }

    TranspilerError* build_constant_definition(Page* _ep, StringBuilder& header_builder, String name, Vector<Operand>& operation) {
        Region _r;

        header_builder.append("const int ");            
        header_builder.append(name);
        header_builder.append(" = ");
        build_operation(_ep, header_builder, operation, String());
        header_builder.append(";\n");
        return nullptr;
    }

    TranspilerError* build_statement(Page* _ep, StringBuilder& builder, Statement* statement) {
        return nullptr;
    }

    TranspilerError* forward_includes(Page* _ep, String path, Program& program) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append("\
typedef __SIZE_TYPE__ size_t;\n\
typedef const char const_char;\n");
        {
            auto _result = forward_includes_for_modules(_ep, builder,  program.module.modules);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }

        {
            auto _result = forward_includes_for_symbols(_ep, builder,  program.module.symbols);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }

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
                full_struct_name(builder, concept.name, concept.parameters);
                builder.append(";\n");
                {
                    auto _result = forward_includes_for_modules(_ep, builder, _structure.modules);
                    if (_result != nullptr)
                        return _result;
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
        builder.append("namespace ");
        builder.append(name);
        builder.append(" {\n");
        {
            auto _result = forward_includes_for_modules(_ep, builder, namespace_.modules);
            if (_result != nullptr)
                return _result;
        }
        {
            auto _result = forward_includes_for_symbols(_ep, builder, namespace_.symbols);
            if (_result != nullptr)
                return _result;
        }
        builder.append("}\n");
        return nullptr;
    }

    TranspilerError* forward_includes_for_modules(Page* _ep, StringBuilder& builder, Vector<Module> modules) {
        Region _r;
        auto module_iterator = VectorIterator<Module>(&modules);
        while (auto module = module_iterator.next()) {
            {
                auto _result = forward_includes_for_symbols(_ep, builder, module->symbols);
                if (_result != nullptr)
                    return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
            }
        }

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
        auto _create_directory_result = create_directory(_ep, vscode_dir);
        if (_create_directory_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_create_directory_result);
        {
            StringBuilder& tasks_file_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), vscode_dir, String(_r.get_page(), "tasks.json")));
            StringBuilder& tasks_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder("{\n\
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
				\"${workspaceFolder}/main.cpp\",\n");
            build_vscode_source_files(tasks_builder, String(), program.module);
            tasks_builder.append("\
				\"-o\",\n\
				\"${workspaceFolder}/bin/");
            tasks_builder.append(program.module.name);
            tasks_builder.append("\",\n\
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
}");
            auto _tasks_result = File::write_from_string(_ep, tasks_file_builder.to_string(_r.get_page()), tasks_builder.to_string(_r.get_page()));
            if (_tasks_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_tasks_result);
        }

        {
            StringBuilder& launch_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder("{\
    \"version\": \"0.2.0\",\n\
    \"configurations\": [\n\
        {\n\
            \"type\": \"lldb\",\n\
            \"request\": \"launch\",\n\
            \"name\": \"Debug\",\n\
            \"program\": \"${workspaceFolder}/bin/");
            launch_builder.append(program.module.name);
            launch_builder.append("\",\n\
            \"args\": [],\n\
            \"cwd\": \"${workspaceFolder}\"\n\
        }\n\
    ]\n\
}");
            auto _launch_result = File::write_from_string(_ep, Path::join(_r.get_page(), vscode_dir, String(_r.get_page(), "launch.json")), launch_builder.to_string(_r.get_page()));
            if (_launch_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_launch_result);
        }
        return nullptr;
    }

    void build_vscode_source_files(StringBuilder& builder, String path, Module& module) {
        Region _r;
        builder.append("				\"${workspaceFolder}/");
        builder.append(path);
        builder.append(module.name);
        builder.append(".cpp\",\n");
        StringBuilder& path_builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder(path);
        path_builder.append(module.name);
        path_builder.append('/');
        build_vscode_source_files_list(builder, path_builder.to_string(_r.get_page()), module.modules, module.symbols);
    }


    void build_vscode_source_files_list(StringBuilder& builder, String path, Vector<Module>& modules, HashMap<String, Nameable> symbols) {
        Region _r;
        auto _module_iterator = VectorIterator<Module>(&modules);
        while (auto module = _module_iterator.next()) {
            build_vscode_source_files(builder, path, *module);
        }

        auto member_iterator = HashMapIterator<String, Nameable>(symbols);
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Nameable::Concept:
                    {
                        auto concept = member->_Concept;
                        switch (concept.definition._tag) {
                            case Definition::Namespace: {
                                auto namespace_ = concept.definition._Namespace;
                                build_vscode_source_files_list(builder, path, namespace_.modules, namespace_.symbols);
                            }
                            break;

                            case Definition::Structure: {
                                auto structure = concept.definition._Structure;
                                build_vscode_source_files_list(builder, path, structure.modules, structure.symbols);
                            }
                            break;
                            
                            default:
                                break;
                        }
                    }
                    break;
                default:
                    continue;
            }
        }
    }

    TranspilerError* main_file(Page* _ep, String path, Program& program) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append("#include \"main.h\"\n\n");
        builder.append("int main(int argc, char** argv) {\n");

        auto iter = VectorIterator<Statement>(&program.statements);
        while(auto statement = iter.next()) {
            build_statement(_ep, builder, statement);
        }
        builder.append("    return 0;\n}\n");
        if (builder.get_length() > 0) {
            auto main_file_name = Path::join(_r.get_page(), path, String(_r.get_page(), "main.cpp"));
            auto _main_result = File::write_from_string(_ep, main_file_name, builder.to_string(_r.get_page()));
            if (_main_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_main_result);
        }

        return nullptr;
    }

    TranspilerError* main_include_file(Page* _ep, String path, String name) {
        Region _r;
        StringBuilder& main_include_file_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(Path::join(_r.get_page(), path, String(_r.get_page(), "main")));
        main_include_file_builder.append(".h");
        StringBuilder& main_include_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        main_include_builder.append("#include \"");
        main_include_builder.append(name);
        main_include_builder.append(".h\"\n");
        auto _main_include_result = File::write_from_string(_ep, main_include_file_builder.to_string(_r.get_page()), main_include_builder.to_string(_r.get_page()));
        if (_main_include_result != nullptr)
            return new(alignof(TranspilerError), _ep) TranspilerError(*_main_include_result);
        return nullptr;
    }
};

}
}
}