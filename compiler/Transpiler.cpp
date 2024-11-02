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
            header_builder.append("\
#include <stdlib.h>\n\
#include <memory.h>\n\
#include <stdio.h>\n\
#include <unistd.h>\n\
#include <sys/stat.h>\n\
#include <errno.h>\n\
#include <math.h>\n\
#include <libgen.h>\n\
#include \"forwards.h\"\n\n\
struct Void {};\n\
enum Success { Ok, Error };\n\n\
template<class OK, class ERROR>\n\
struct Result {\n\
    Result(OK _Ok) : _tag(Ok), _Ok(_Ok) {}\n\
    Result(ERROR _ERROR) : _tag(Error), _Error(_ERROR) {}\n\
    Success _tag;\n\
    union {\n\
        OK _Ok;\n\
        ERROR _Error;\n\
    };\n\
};\n\n");
        }
        else {
            header_builder.append("#include \"");
            header_builder.append(main_header);
            header_builder.append(".h\"\n");
        }
        if (!module.name.equals("scaly") && !module.name.equals("memory"))
            header_builder.append("using namespace scaly;\nusing namespace scaly::memory;\n");
        
        build_uses(header_builder, module.uses);
        build_uses(cpp_builder, module.uses);

        StringBuilder& namespace_open_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(namespace_open);

        auto _symbols_result = build_symbols(_ep, path, module.file, module.name, header_builder, cpp_builder, main_header, namespace_open, namespace_close, module.members);
        if (_symbols_result)
            return _symbols_result;

        cpp_builder.append('\n');
        cpp_builder.append(namespace_close);

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

            case Definition::Global: {
                auto global = concept.definition._Global;
                return build_global(_ep, header_builder, concept.name, global);
            }

            case Definition::Union: {
                auto union_ = concept.definition._Union;
                build_union(_ep, header_builder, cpp_builder, concept.name, union_, concept.parameters);
                return nullptr;
            }
        }
    }

    void build_union(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Union& union_, Vector<GenericParameter> parameters) {
        header_builder.append('\n');
        full_struct_name(header_builder, name, parameters);
        header_builder.append(" {\n");
        {
            auto variants = union_.variants;
            {
                auto _variant_iterator = variants.get_iterator();
                while (auto _variant = _variant_iterator.next()) {
                    Variant& variant = *_variant;
                    header_builder.append("    ");
                    header_builder.append(name);
                    header_builder.append('(');
                    build_type(header_builder, variant.type);
                    header_builder.append(")");
                    if (parameters.length > 0)
                    {
                        header_builder.append(" : _tag(");
                        header_builder.append(variant.name);
                        header_builder.append(") { _");
                        header_builder.append(variant.name);
                        header_builder.append(" = _");
                        build_type(header_builder, variant.type);
                        header_builder.append(";");
                        header_builder.append(" }");
                    } else {
                        header_builder.append(";");
                    }
                    header_builder.append("\n");
                }
            }
        }
        header_builder.append("    enum {\n");
        auto variants = union_.variants;
        {
            {
                auto _variant_iterator = variants.get_iterator();
                while (auto _variant = _variant_iterator.next()) {
                    Variant& variant = *_variant;
                    header_builder.append("        ");
                    header_builder.append(variant.name);
                    header_builder.append(",\n");
                }
            }
        }
        header_builder.append("    } _tag;\n    union {\n");
        {
            auto _variant_iterator = variants.get_iterator();
            while (auto _variant = _variant_iterator.next()) {
                Variant& variant = *_variant;
                if (variant.type == nullptr)
                    continue;
                header_builder.append("        ");
                build_type(header_builder, variant.type);
                header_builder.append(" _");
                header_builder.append(variant.name);
                header_builder.append(";\n");
            }
            header_builder.append("    };\n};");
        }
        if (parameters.length == 0) {
            auto _variant_iterator = variants.get_iterator();
            while (auto _variant = _variant_iterator.next())
            {
                Variant& variant = *_variant;
                cpp_builder.append('\n');
                cpp_builder.append(name);
                cpp_builder.append("::");
                cpp_builder.append(name);
                cpp_builder.append('(');
                build_type(cpp_builder, variant.type);
                cpp_builder.append(" _");
                cpp_builder.append(variant.name);
                cpp_builder.append(") : _tag(");
                cpp_builder.append(variant.name);
                cpp_builder.append("), _");
                cpp_builder.append(variant.name);
                cpp_builder.append("(_");
                cpp_builder.append(variant.name);
                cpp_builder.append(") {}\n");
            }
        }
    }

    TranspilerError* build_namespace(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Namespace& namespace_) {
        Region _r;
        header_builder.append("namespace ");
        header_builder.append(name);
        header_builder.append(" {\n");
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
            auto _result = build_symbols(_ep, path, source, name, header_builder, cpp_builder, main_header_builder.to_string(_r.get_page()), namespace_open, namespace_close, namespace_.members);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }
        header_builder.append("\n}");

        return nullptr;
    }

    TranspilerError* build_modules(Page* _ep, String path, String name, StringBuilder& header_builder, Vector<Module>& modules, String main_header, String namespace_open, String namespace_close) {
        Region _r;
        auto module_iterator = modules.get_iterator();
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

    TranspilerError* build_symbols(Page* _ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Vector<Member> symbols) {
        Region _r;
        auto member_iterator = symbols.get_iterator();
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Member::Function: {
                    auto _result = build_function(_ep, header_builder, cpp_builder, member->_Function, &name, false, false);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    break;
                }
                case Member::Concept: {
                    auto _result = build_concept(_ep, path, source, header_builder, cpp_builder, main_header, namespace_open, namespace_close, member->_Concept);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
                    break;
                }
                case Member::Operator:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "namespace local operator")));
                case Member::Package:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "namespace local package")));
            }
        }

        return nullptr;
    }

    TranspilerError* build_structure(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Structure& structure, Vector<GenericParameter> parameters) {
        Region _r;
        header_builder.append('\n');
        full_struct_name(header_builder, name, parameters);
        if (!name.equals(String(_r.get_page(), "Object")) && !name.equals(String(_r.get_page(), "Void")))
            header_builder.append(" : Object");
        header_builder.append(" {");

        bool must_build_properties_initializer = false;
        auto property_iterator = structure.properties.get_iterator();
        while (auto property = property_iterator.next()) {
            if (property->type) {
                header_builder.append('\n');
                header_builder.append("    ");
                build_type(header_builder, property->type);
            }

            header_builder.append(' ');
            header_builder.append(property->name);
            header_builder.append(';');

            if (property->initializer != nullptr)
                must_build_properties_initializer = true;
        }

        if (parameters.length == 0)
            header_builder.append('\n');

        if (structure.properties.length > 0)
            build_default_initializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, structure.properties);

        if (must_build_properties_initializer) {
            auto _result = build_properties_initializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, structure.properties);
            if (_result != nullptr)
                return _result;
        }

        auto initializers_iterator = structure.initializers.get_iterator();
        while (auto initializer = initializers_iterator.next()) {
            auto _result = build_initializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, initializer);
            if (_result != nullptr)
                return _result;
        }

        if (structure.deinitializer != nullptr) {
            auto _result = build_deinitializer(_ep, header_builder, cpp_builder, name, parameters.length > 0, structure.deinitializer);
            if (_result != nullptr)
                return _result;
        }

        auto member_iterator = structure.members.get_iterator();
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Member::Function: {
                    auto _result = build_function(_ep, header_builder, cpp_builder, member->_Function, &name, parameters.length > 0, true);
                    if (_result != nullptr)
                        return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    break;
                }
                case Member::Operator: {
                        auto _result = build_operator(_ep, header_builder, cpp_builder, member->_Operator, &name, parameters.length > 0);
                        if (_result != nullptr)
                            return new(alignof(TranspilerError), _ep) TranspilerError(*_result);                    
                    }
                    break;
                case Member::Package:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local package")));
                case Member::Concept:
                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "structure local concept")));
            }
        }

        header_builder.append("\n};\n");
        return nullptr; 
    }

    TranspilerError* build_default_initializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Vector<Property>& properties) {
        Region _r;
        build_initializer_header(header_builder, cpp_builder, name, is_generic);
        if (is_generic) {
            build_default_initializer_list(_ep, header_builder, is_generic, properties, String(_r.get_page(), "        "));
            header_builder.append(" {}");
        }
        else {
            build_default_initializer_list(_ep, cpp_builder, is_generic, properties, String(_r.get_page(), "    "));
            cpp_builder.append(" {}");
            header_builder.append("(");
            {
                auto property_iterator = properties.get_iterator();
                bool first = true;
                while (auto property = property_iterator.next()) {
                    if (first) {
                        first = false;
                    }
                    else {
                        header_builder.append(", ");
                    }
                    build_type(header_builder, property->type);
                    header_builder.append(' ');
                    header_builder.append(property->name);
                }
            }
            header_builder.append(");");
        }
        return nullptr;
    }

    TranspilerError* build_default_initializer_list(Page* _ep, StringBuilder& builder, bool is_generic, Vector<Property>& properties, String indent) {
        builder.append('(');
        {
            auto property_iterator = properties.get_iterator();
            bool first = true;
            while (auto property = property_iterator.next()) {
                if (first) {
                    first = false;
                }
                else {
                    builder.append(", ");
                }
                build_type(builder, property->type);
                builder.append(' ');
                builder.append(property->name);
            }
        }
        builder.append(") : ");
        {
            auto property_iterator = properties.get_iterator();
            bool first = true;
            while (auto property = property_iterator.next()) {
                if (first) {
                    first = false;
                }
                else {
                    builder.append(", ");
                }
                builder.append(property->name);
                builder.append('(');
                builder.append(property->name);
                builder.append(')');
            }
        }
        return nullptr;
    }

    TranspilerError* build_properties_initializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Vector<Property>& properties) {
        Region _r;
        build_initializer_header(header_builder, cpp_builder, name, is_generic);
        if (is_generic) {
            build_properties_initializer_list(_ep, header_builder, is_generic, properties, String(_r.get_page(), "        "));
            header_builder.append(" {}");
        }
        else {
            build_properties_initializer_list(_ep, cpp_builder, is_generic, properties, String(_r.get_page(), "    "));
            cpp_builder.append(" {}");
            header_builder.append(" ();");
        }
        return nullptr;
    }

    TranspilerError* build_properties_initializer_list(Page* _ep, StringBuilder& builder, bool is_generic, Vector<Property>& properties, String indent) {
        builder.append("() : ");
        auto property_iterator = properties.get_iterator();
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
            builder.append(property->name);
            builder.append('(');
            auto _result = build_operation(_ep, builder, *property->initializer, nullptr, nullptr, indent);
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
            build_input(cpp_builder, initializer->input, nullptr, nullptr, nullptr, Lifetime(Unspecified()));
            cpp_builder.append(' ');
            auto _result = build_implementation(_ep, cpp_builder, initializer->implementation, nullptr, nullptr, String(), true);
            if (_result != nullptr)
                return _result;
        }
        build_input(header_builder, initializer->input, nullptr, nullptr, nullptr, Lifetime(Unspecified()));
        if (is_generic) {
            header_builder.append(' ');
            auto _result = build_implementation(_ep, header_builder, initializer->implementation, nullptr, nullptr, String(_r.get_page(), "    "), true);
            if (_result != nullptr)
                return _result;
        }
        else {
            header_builder.append(';');
        }

        return nullptr; 
    }

    void build_initializer_header(StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic) {
        if (is_generic)
            header_builder.append('\n');
        header_builder.append("\n    ");
        header_builder.append(name);
        if (!is_generic) {
            cpp_builder.append("\n\n");
            cpp_builder.append(name);
            cpp_builder.append("::");
            cpp_builder.append(name);
        }        
    }
    TranspilerError* build_deinitializer(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, DeInitializer* de_initializer) {
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
            cpp_builder.append("() ");
            auto _result = build_implementation(_ep, cpp_builder, de_initializer->implementation, nullptr, nullptr, String(_r.get_page(), String()), true);
            if (_result != nullptr)
                return _result;
            cpp_builder.append(";");
        }
        if (is_generic) {
            header_builder.append("() ");
            auto _result = build_implementation(_ep, header_builder, de_initializer->implementation, nullptr, nullptr, String(_r.get_page(), "    "), true);
            if (_result != nullptr)
                return _result;
            header_builder.append(";");
        }
        else {
            header_builder.append("();");
        }

        return nullptr; 
    }

    void build_uses(StringBuilder& builder, Vector<Use>& uses) {
        Region _r;
        auto uses_iterator = uses.get_iterator();
        while (auto use = uses_iterator.next()) {
            builder.append("\nusing namespace ");
            bool first = true;
            auto namespace_iterator = use->path.get_iterator();
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

    TranspilerError* build_function(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Function& function, String* name, bool is_template, bool in_class) {
        Region _r;
        String* location = nullptr;
        switch (function.lifetime._tag) {
            case Lifetime::Call:
                location = new (alignof(String), _r.get_page()) String(_r.get_page(), "rp");
                break;
            case Lifetime::Reference: {
                auto reference = function.lifetime._Reference;
                location = &reference.location;
                break;
            }
            case Lifetime::Unspecified:
                break;
            case Lifetime::Local:
                break;
            case Lifetime::Thrown:
                break;
        }
        if (function.implementation._tag == Implementation::Extern) {
            header_builder.append("extern \"C\" ");
            build_output_type(header_builder, function.returns_, function.throws_);
            header_builder.append(' ');
            header_builder.append(function.name);
            build_input(header_builder, function.input, location, function.returns_, function.throws_, function.lifetime);
            header_builder.append(";\n");
            return nullptr;
        }
        if (is_template)
            header_builder.append('\n');
        function_prefix(header_builder, function, name != nullptr, in_class);
        if (!is_template) {
            cpp_builder.append('\n');
            function_prefix(cpp_builder, function, false, false);
            if (name != nullptr) {
                cpp_builder.append(*name);
                cpp_builder.append("::");
            }
            cpp_builder.append(function.name);
            build_input(cpp_builder, function.input, location, function.returns_, function.throws_, function.lifetime);
            cpp_builder.append(' ');
            auto _result = build_implementation(_ep, cpp_builder, function.implementation, function.returns_, function.throws_, String(), false);
            if (_result != nullptr)
                return _result;
        }
        header_builder.append(function.name);
        build_input(header_builder, function.input, location, function.returns_, function.throws_, function.lifetime);
        if (is_template) {
            header_builder.append(' ');
            auto _result = build_implementation(_ep, header_builder, function.implementation, function.returns_, function.throws_, String(_r.get_page(), "    "), false);
            if (_result != nullptr)
                return _result;
        }
        header_builder.append(';');

        return nullptr; 
    }

    void build_output_type(StringBuilder& builder, Type* returns_, Type* throws_) {
        if (throws_ != nullptr) 
        {
            builder.append("Result<");
            if (returns_ == nullptr)
                builder.append("Void");
            else
                build_type(builder, returns_);
            builder.append(", ");
            build_type(builder, throws_);
            builder.append('>');
            
        } else if (returns_ == nullptr) {
            builder.append("void");
        } else
            build_type(builder, returns_);
    }

    TranspilerError* build_implementation(Page* _ep, StringBuilder& builder, Implementation& implementation, Type* returns_, Type* throws_, String indent, bool is_initializer) {
        switch (implementation._tag) {
            case Implementation::Action: {
                auto action = implementation._Action;
                if (action.target.length == 0 && action.source.length == 1 && action.source[0]->expression._tag == Expression::Block) {
                    auto _result = build_action(_ep, builder, action, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                }
                else {
                    if (is_initializer) {
                        auto _operand_iterator = action.source.get_iterator();
                        while (auto _operand = _operand_iterator.next()) {
                            Operand& operand = *_operand;
                            switch (operand.expression._tag) {
                                case Expression::Tuple: {
                                    auto tuple = operand.expression._Tuple;
                                    bool first = true;
                                    auto _component_iterator = tuple.components.get_iterator();
                                    while (auto _component = _component_iterator.next()) {
                                        Component& component = *_component;
                                        if (first) {
                                            first = false;
                                            builder.append(": ");
                                        }
                                        else {
                                            builder.append(", ");
                                        }
                                        auto _result = build_operation(_ep, builder, component.value, returns_, throws_, indent);
                                        if (_result != nullptr)
                                            return _result;
                                    }
                                    break;
                                }
                                case Expression::Block: {
                                    auto block = operand.expression._Block;
                                    auto _result = build_block(_ep, builder, block, returns_, throws_, indent);
                                    if (_result != nullptr)
                                        return _result;
                                    break;
                                }
                                default:
                                    return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Other than Tuple or Block in initializer implementation")));
                            }
                        }
                    }
                    else {
                        builder.append("{\n");
                        builder.append(indent);
                        builder.append("    return ");
                        auto _result = build_action(_ep, builder, action, returns_, throws_, indent);
                        if (_result != nullptr)
                            return _result;
                        builder.append("; }");
                    }

                }
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

    TranspilerError* build_binding(Page* _ep, StringBuilder& builder, Binding& binding, Type* returns_, Type* throws_, String indent) {
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
        bool simple_array = false;
        if (binding.item.type != nullptr)
        {
            simple_array = build_type(builder, binding.item.type);
        }
        else
        {
            builder.append("auto");
        }

        if (binding.item.name != nullptr) {
            builder.append(' ');
            builder.append(*binding.item.name);
        }
        if (simple_array) {
            builder.append('[');
            auto _result = build_operation(_ep, builder, binding.operation, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
            builder.append(']');
        } else {
            builder.append(" = ");

            auto _result = build_operation(_ep, builder, binding.operation, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* build_action(Page* _ep, StringBuilder& builder, Action& action, Type* returns_, Type* throws_, String indent) {
        if (action.target.length > 0) {
            {
                auto _result = build_operation(_ep, builder, action.target, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
            }
            builder.append(" = ");
        }
        {
            auto _result = build_operation(_ep, builder, action.source, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* build_operation(Page* _ep, StringBuilder& builder, Vector<Operand>& operation, Type* returns_, Type* throws_, String indent) {
        auto operation_iterator = operation.get_iterator();
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
                    auto _result = build_variable(_ep, builder, type, operand->member_access);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Tuple: {
                    auto tuple = operand->expression._Tuple;
                    auto _result = build_tuple(_ep, builder, tuple, operand->member_access, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Matrix: {
                    auto matrix = operand->expression._Matrix;
                    auto _result = build_matrix(_ep, builder, matrix, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Block: {
                    auto block = operand->expression._Block;
                    auto _result = build_block(_ep, builder, block, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::If: {
                    auto if_ = operand->expression._If;
                    auto _result = build_if(_ep, builder, if_, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Match: {
                    auto match_ = operand->expression._Match;
                    auto _result = build_match(_ep, builder, match_, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Choose: {
                    auto choose_ = operand->expression._Choose;
                    auto _result = build_choose(_ep, builder, choose_, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::For: {
                    auto for_ = operand->expression._For;
                    auto _result = build_for(_ep, builder, for_, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::While: {
                    auto while_ = operand->expression._While;
                    auto _result = build_while(_ep, builder, while_, returns_, throws_, indent);
                    if (_result != nullptr)
                        return _result;
                    break;
                }
                case Expression::Try: {
                    auto try_ = operand->expression._Try;
                    auto _result = build_try(_ep, builder, try_, returns_, throws_, indent);
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
            }
        }
        return nullptr;
    }

    void build_string(StringBuilder& builder, String string) {
        builder.append('\"');
        auto _string_iterator = string.get_iterator();
        while (auto _c = _string_iterator.next())
        {
            auto c = *_c;
            if (c == '\n')
                builder.append("\\n\\");
            builder.append(c);
        }
        builder.append('\"');
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
                build_string(builder, constant._String);
                break;
            }
            case Constant::Character: {
                auto string = constant._String;
                builder.append('\'');
                builder.append(string);
                builder.append('\'');
                break;
            }
            case Constant::Fragment: {
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Bool")));
            }
        }

        return nullptr;
    }

    TranspilerError* build_variable(Page* _ep, StringBuilder& builder, Type& type, Vector<String>* member_access) {
        auto name_iterator = type.name.get_iterator();
        auto first_name_part = name_iterator.next();
        if (first_name_part->equals("=")) {
            builder.append(" == ");
            return nullptr;
        }
        if (first_name_part->equals("<>")) {
            builder.append(" != ");
            return nullptr;
        }
        if (first_name_part->equals("//")) {
            builder.append(" ^ ");
            return nullptr;
        }
        if (first_name_part->equals("->")) {
            builder.append("::");
            return nullptr;
        }
        if (first_name_part->equals("null")) {
            builder.append("nullptr");
            return nullptr;
        }
        if (first_name_part->equals("pointer")) {
            if (type.generics != nullptr) {
                auto generics = *(type.generics);
                build_variable(_ep, builder, *generics[0], member_access);
            }
            builder.append('*');
            return nullptr;
        }

        switch (type.lifetime._tag) {
            case Lifetime::Unspecified:
                break;
            case Lifetime::Local: {
                builder.append("new (alignof(");
                build_type(builder, &type);
                builder.append("), r.get_page()) ");
                break;
            }
            case Lifetime::Call: {
                auto local = type.lifetime._Local;
                builder.append("new (alignof(");
                build_type(builder, &type);
                builder.append("), rp) ");
                break;
            }
            case Lifetime::Reference: {
                auto reference = type.lifetime._Reference;
                builder.append("new (alignof(");
                build_type(builder, &type);
                builder.append("), ");
                builder.append(reference.location);
                builder.append(") ");
                break;
            }
            case Lifetime::Thrown:
                return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, "Thrown")));
        }

        build_type(builder, &type);

        if (member_access != nullptr)
        {
            auto members_iterator = member_access->get_iterator();
            while (auto _member = members_iterator.next()) {
                if (_member != nullptr) {
                    auto member = *_member;
                    if (first_name_part->equals("this"))
                        builder.append("->");
                    else
                        builder.append('.');
                    builder.append(member);
                }
            }
        }

        return nullptr;
    }

    TranspilerError* build_tuple(Page* _ep, StringBuilder& builder, Tuple& tuple, Vector<String>* member_access, Type* returns_, Type* throws_, String indent) {
        auto tuple_iterator = tuple.components.get_iterator();
        bool first = true;
        builder.append('(');
        while (auto property = tuple_iterator.next()) {
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            }
            build_operation(_ep, builder, property->value, returns_, throws_, indent);
        }
        builder.append(')');

        if (member_access != nullptr)
        {
            auto _members_iterator = member_access->get_iterator();
            while(auto _member = _members_iterator.next()) {
                builder.append('.');
                builder.append(*_member);
            }
        }

        return nullptr;
    }

    TranspilerError* build_matrix(Page* _ep, StringBuilder& builder, Matrix& matrix, Type* returns_, Type* throws_, String indent) {
        auto operations_iterator = matrix.operations.get_iterator();
        bool first = true;
        builder.append('[');
        while (auto operation = operations_iterator.next()) {
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            }
            build_operation(_ep, builder, *operation, returns_, throws_, indent);
        }
        builder.append(']');
        return nullptr;
    }

    TranspilerError* build_block(Page* _ep, StringBuilder& builder, Block& block, Type* returns_, Type* throws_, String indent) {
        Region _r;
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        indent_builder.append("    ");
        builder.append('{');
        {
            auto _result = build_statements(_ep, builder, block.statements, returns_, throws_, indent_builder.to_string(_r.get_page()));
            if (_result != nullptr)
                return _result;
        }
        builder.append('\n');
        builder.append(indent);
        builder.append('}');
        return nullptr;
    }

    TranspilerError* build_if(Page* _ep, StringBuilder& builder, If& if_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("if (");
        {
            auto _result = build_operation(_ep, builder, if_.condition, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(") ");
        {
            bool is_block = (if_.consequent._tag == Statement::Action && if_.consequent._Action.source[0]->expression._tag == Expression::Block);
            if (!is_block) {
                builder.append('\n');
                builder.append(indent);
                builder.append("    ");
            }
            auto _result = build_statement(_ep, builder, &if_.consequent, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        if (if_.alternative != nullptr) {
            builder.append('\n');
            builder.append(indent);
            builder.append("else ");
            {
                auto _result = build_statement(_ep, builder, if_.alternative, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
            }
        }

        return nullptr;
    }

    TranspilerError* build_match(Page* _ep, StringBuilder& builder, Match& match_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("switch (");
        {
            auto _result = build_operation(_ep, builder, match_.condition, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(")\n");
        builder.append(indent);
        builder.append("{");
        auto _case__iterator = match_.cases.get_iterator();
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        indent_builder.append("        ");
        String indented = indent_builder.to_string(_r.get_page());
        while (auto _case_ = _case__iterator.next()) {
            auto case_ = *_case_;
            builder.append('\n');
            builder.append(indent);
            builder.append("    case ");
            {
                auto _result = build_operation(_ep, builder, case_.condition, returns_, throws_, indented);
                if (_result != nullptr)
                    return _result;
            }
            builder.append(":\n");
            builder.append(indented);
            {
                auto _result = build_statement(_ep, builder, &case_.consequent, returns_, throws_, indented);
                if (_result != nullptr)
                    return _result;
                builder.append(';');
            }
            builder.append('\n');
            builder.append(indented);
            builder.append("break;");
        }
        if (match_.alternative != nullptr) {
            builder.append('\n');
            builder.append(indent);
            builder.append("    default:\n");
            builder.append(indented);
            {
                auto _result = build_statement(_ep, builder, match_.alternative, returns_, throws_, indented);
                if (_result != nullptr)
                    return _result;
                builder.append(';');
            }
        }
        builder.append('\n');
        builder.append(indent);
        builder.append("}");

        return nullptr;
    }

    TranspilerError* build_choose(Page* _ep, StringBuilder& builder, Choose& choose_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("{\n");
        builder.append(indent);
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        builder.append(indent);
        builder.append("auto _result = ");
        {
            auto _result = build_operation(_ep, builder, choose_.condition, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(";\n");
        indent_builder.append("    ");
        String indent1 = indent_builder.to_string(_r.get_page());
        builder.append(indent1);
        builder.append("switch (_result._tag)\n");
        builder.append(indent1);
        builder.append("{");
        auto _case__iterator = choose_.cases.get_iterator();
        indent_builder.append(indent1);
        String indented = indent_builder.to_string(_r.get_page());
        while (auto _case_ = _case__iterator.next()) {
            auto case_ = *_case_;
            builder.append('\n');
            builder.append(indent1);
            builder.append("    case ");
            bool first = true;
            auto _variant_iterator = case_.variant.get_iterator();
            while (auto _variant = _variant_iterator.next()) {
                if(first)
                    first = false;
                else
                    builder.append("::");
                builder.append(*_variant);
            }
            builder.append(":\n");
            builder.append(indent1);
            builder.append("    {\n");
            builder.append(indented);
            builder.append("auto ");
            builder.append(case_.name);
            builder.append(" = _result._");
            _variant_iterator = case_.variant.get_iterator();
            first = true;
            while (auto _variant = _variant_iterator.next()) {
                if(first)
                    first = false;
                else
                    builder.append(*_variant);
            }
            builder.append(";\n");
            builder.append(indented);
            {
                auto _result = build_action(_ep, builder, case_.action, returns_, throws_, indented);
                if (_result != nullptr)
                    return _result;
                builder.append(";\n");
            }
            builder.append(indented);
            builder.append("break;");
            builder.append('\n');
            builder.append(indent1);
            builder.append("    }");
        }
        if (choose_.alternative != nullptr) {
            builder.append('\n');
            builder.append(indent1);
            builder.append("    default:\n");
            builder.append(indented);
            {
                auto _result = build_statement(_ep, builder, choose_.alternative, returns_, throws_, indented);
                if (_result != nullptr)
                    return _result;
                builder.append(';');
            }
        }
        builder.append('\n');
        builder.append(indent);
        builder.append("    }\n");
        builder.append(indent);
        builder.append("}");

        return nullptr;
    }

    TranspilerError* build_for(Page* _ep, StringBuilder& builder, For& for_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append('\n');
        builder.append(indent);
        builder.append("auto _");
        builder.append(for_.identifier);
        builder.append("_iterator = ");
        build_operation(_ep, builder, for_.expression, returns_, throws_, indent);
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
        builder.append(" = *_");
        builder.append(for_.identifier);
        builder.append(";");
        {
            auto _result = build_action(_ep, builder, for_.action, returns_, throws_, indented);
            if (_result != nullptr)
                return _result;
        }
        builder.append('\n');
        builder.append(indent);
        builder.append('}');

        return nullptr;
    }

    TranspilerError* build_condition(Page* _ep, StringBuilder& builder, Binding& binding, Type* returns_, Type* throws_, String indent) {

        if (binding.item.name != nullptr) {
            if (binding.item.type != nullptr)
                build_type(builder, binding.item.type);
            else
                builder.append("auto");
            builder.append(' ');
            builder.append(*binding.item.name);

            builder.append(" = ");
        }

        auto _result = build_operation(_ep, builder, binding.operation, returns_, throws_, indent);
        if (_result != nullptr)
            return _result;
        return nullptr;
    }

    TranspilerError* build_while(Page* _ep, StringBuilder& builder, While& while_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("while (");
        {
            auto _result = build_condition(_ep, builder, while_.condition, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(") ");
        {
            auto _result = build_action(_ep, builder, while_.action, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* build_drop(Page* _ep, StringBuilder& builder, Drop& drop_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append('\n');
        builder.append(indent);
        builder.append("default: {\n");
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        indent_builder.append("    ");
        String indented = indent_builder.to_string(_r.get_page());
        builder.append(indented);
        build_action(_ep, builder, drop_.action, returns_, throws_, indented);
        builder.append(";\n");
        builder.append(indented);
        builder.append("break;\n");
        builder.append(indent);
        builder.append("}\n");
        return nullptr;
    }

    TranspilerError* build_try(Page* _ep, StringBuilder& builder, Try& try_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        auto name = String(_r.get_page(), "void");
        StringBuilder& indent_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder(indent);
        if (try_.binding.item.name != nullptr) {
            name = *try_.binding.item.name;
        } else {
            builder.append("{\n");
            indent_builder.append("    ");
            indent = indent_builder.to_string(_r.get_page());
            builder.append(indent);
        }
        builder.append("const auto _");
        builder.append(name);
        builder.append("_result = ");
        {
            auto _result = build_operation(_ep, builder, try_.binding.operation, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(";\n");
        builder.append(indent);
        if (!name.equals("void")) {
            builder.append("auto ");
            builder.append(name);
            builder.append(" = _");
            builder.append(name);
            builder.append("_result._Ok;\n");
        }
        builder.append(indent);
        builder.append("if (_");
        builder.append(name);
        builder.append("_result._tag == Success::Error) {\n");
        indent_builder.append("    ");
        auto indented = indent_builder.to_string(_r.get_page());
        builder.append(indented);
        builder.append("const auto _");
        builder.append(name);
        builder.append("_Error = _");
        builder.append(name);
        builder.append("_result._Error;\n");
        builder.append(indented);
        builder.append("switch (_");
        builder.append(name);
        builder.append("_Error._tag) {");
        auto _catch__iterator = try_.catches.get_iterator();
        indent_builder.append("    ");
        String indented2 = indent_builder.to_string(_r.get_page());
        indent_builder.append("    ");
        String indented3 = indent_builder.to_string(_r.get_page());
        while (auto _catch_ = _catch__iterator.next()) {
            auto catch_ = *_catch_;
            builder.append('\n');
            builder.append(indented);
            builder.append("    case ");
            bool first = true;
            auto _error_iterator = catch_.error.get_iterator();
            while (auto _error = _error_iterator.next()) {
                if(first)
                    first = false;
                else
                    builder.append("::");
                builder.append(*_error);
            }
            builder.append(": {\n");
            builder.append(indented3);
            builder.append("const auto error = _");
            builder.append(name);
            builder.append("_Error._");
            builder.append(*catch_.error.get(1));
            builder.append(";\n");
            builder.append(indented3);
            {
                auto _result = build_action(_ep, builder, catch_.action, returns_, throws_, indented3);
                if (_result != nullptr)
                    return _result;
            }
            builder.append("\n");
            builder.append(indented3);
            builder.append("break;\n");
            builder.append(indented2);
            builder.append("}");
        }
        if (try_.drop_ != nullptr) {
            {
                auto _result = build_drop(_ep, builder, *try_.drop_, returns_, throws_, indented2);
                if (_result != nullptr)
                    return _result;
            }
        }
        builder.append(indented);
        builder.append("}\n");
        builder.append(indent);
        builder.append("}\n");
        builder.append(indent);
        if (name.equals("void")) {
            builder.append("}\n");
            builder.append(indent);
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

    TranspilerError* build_break(Page* _ep, StringBuilder& builder, Break& break_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("break");
        if (break_.result.length > 0)
            builder.append(' ');
        {
            auto _result = build_operation(_ep, builder, break_.result, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        return nullptr;
    }

    TranspilerError* build_continue(Page* _ep, StringBuilder& builder, Continue& continue_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("continue;");
        return nullptr;
    }

    TranspilerError* build_return(Page* _ep, StringBuilder& builder, Return& return_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("return");
        if ((return_.result.length > 0) || (throws_ != nullptr))
        {
            builder.append(' ');
        }
        if (throws_ != nullptr)
        {
            builder.append("Result<");
            if (returns_ == nullptr)
                builder.append("Void");
            else
                build_type(builder, returns_);
            builder.append(", ");
            build_type(builder, throws_);
            builder.append(">(");
            if (return_.result.length == 0)
            {
                builder.append("Void()");
            }
        }
        {
            auto _result = build_operation(_ep, builder, return_.result, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }

        if (throws_ != nullptr)
            builder.append(")");

        return nullptr;
    }

    TranspilerError* build_throw(Page* _ep, StringBuilder& builder, Throw& throw_, Type* returns_, Type* throws_, String indent) {
        Region _r;
        builder.append("return Result<");
        if (returns_ == nullptr)
            builder.append("Void");
        else
            build_type(builder, returns_);
        builder.append(", ");
        build_type(builder, throws_);
        builder.append(">(");
        {
            auto _result = build_operation(_ep, builder, throw_.result, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
        }
        builder.append(")");
        return nullptr;
    }

    TranspilerError* build_statements(Page* _ep, StringBuilder& builder, Vector<Statement>& statements, Type* returns_, Type* throws_, String indent) {
        auto statment_iterator = statements.get_iterator();
        while (auto statement = statment_iterator.next()) {
            builder.append('\n');
            builder.append(indent);
            auto _result = build_statement(_ep, builder, statement, returns_, throws_, indent);
            if (_result != nullptr)
                return _result;
            builder.append(';');
        }
        return nullptr;
    }

    TranspilerError* build_statement(Page* _ep, StringBuilder& builder, Statement* statement, Type* returns_, Type* throws_, String indent) {
        switch (statement->_tag) {
            case Statement::Action: {
                auto action = statement->_Action;
                auto _result = build_action(_ep, builder, action, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
                break;
            }
            case Statement::Binding:{
                auto binding = statement->_Binding;
                auto _result = build_binding(_ep, builder, binding, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
                break;
            }
            case Statement::Break: {
                auto break_ = statement->_Break;
                auto _result = build_break(_ep, builder, break_, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
                break;
            }
            case Statement::Continue: {
                auto continue_ = statement->_Continue;
                auto _result = build_continue(_ep, builder, continue_, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
                break;
            }
            case Statement::Return: {
                auto return_ = statement->_Return;
                auto _result = build_return(_ep, builder, return_, returns_, throws_, indent);
                if (_result != nullptr)
                    return _result;
                break;
            }
            case Statement::Throw: {
                auto throw_ = statement->_Throw;
                auto _throw = build_throw(_ep, builder, throw_, returns_, throws_, indent);
                if (_throw != nullptr)
                    return _throw;
                break;
            }
        }
        return nullptr;
    }

    void function_prefix(StringBuilder& builder, Function& function, bool indent, bool static_if_applicable) {
        Region _r;
        builder.append('\n');
        if (indent)
            builder.append("    ");
        if (static_if_applicable && ((function.input.length == 0) || (function.input[0]->name == nullptr) || (!function.input[0]->name->equals("this"))))
            builder.append("static ");
        build_output_type(builder, function.returns_, function.throws_);
        builder.append(' ');
    }

    TranspilerError* build_operator(Page* _ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Operator& operator_, String* name, bool is_template) {
        Region _r;
        if (is_template)
            header_builder.append('\n');
        header_builder.append("\n    ");
        build_output_type(header_builder, operator_.returns_, operator_.throws_);
        header_builder.append(" operator ");
        header_builder.append(operator_.name);
        build_input(header_builder, operator_.input, nullptr, operator_.returns_, operator_.throws_, Lifetime(Unspecified()));
        if (is_template) {
            auto _result = build_implementation(_ep, header_builder, operator_.implementation, operator_.returns_, operator_.throws_, String(_r.get_page(), "    "), false);
            if (_result != nullptr)
                return _result;
        }
        else {
            header_builder.append(';');
            cpp_builder.append("\n\n");
            build_output_type(cpp_builder, operator_.returns_, operator_.throws_);

            cpp_builder.append(' ');
            cpp_builder.append(*name);
            cpp_builder.append("::operator ");
            cpp_builder.append(operator_.name);
            auto is_static = build_input(cpp_builder, operator_.input, nullptr, operator_.returns_, operator_.throws_, Lifetime(Unspecified()));
            cpp_builder.append(' ');
            auto _result = build_implementation(_ep, cpp_builder, operator_.implementation, operator_.returns_, operator_.throws_, String(), false);
            if (_result != nullptr)
                return _result;
        }
        return nullptr; 
    }

    bool needs_return_page(Type* type) {
        if (type == nullptr)
            return false;

        if (type->name.length == 1)
        {
            auto name = *type->name.get(0);
            if (name.equals("bool") || name.equals("int") || name.equals("size_t"))
                return false;
        }
        
        if (type->lifetime._tag == Lifetime::Local)
            return false;
        
        return true;
    }

    bool build_input(StringBuilder& builder, Vector<Item> input, String* location, Type* returns_, Type* throws_, Lifetime lifetime) {
        Region _r;
        builder.append('(');

        bool parameters_there = false;
        if (input.length > 0)
        {
            if (input.get(0)->name->equals("this"))
                parameters_there = input.length > 1;
            else
                parameters_there = true;
        }

        auto needs_rp = needs_return_page(returns_);
        if (location != nullptr)
        {
            builder.append("Page* ");
            builder.append(*location);
            if (throws_ != nullptr || parameters_there || needs_rp)
                builder.append(", ");
        }

        if (needs_rp)
        {
            builder.append("Page* rp");
            if (throws_ != nullptr || parameters_there)
                builder.append(", ");
        }

        if (throws_ != nullptr)
        {
            builder.append("Page* ep");
            if (parameters_there)
                builder.append(", ");
        }

        auto input_iterator = input.get_iterator();
        bool first = true;
        bool is_static = true;
        while (auto property = input_iterator.next()) {
            if (first) {
                if (property->name && property->name->equals(String(_r.get_page(), "this"))) {
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

            if (property->name)
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
                auto generic_iterator = parameters.get_iterator();
                size_t i = 0;
                while(auto generic = generic_iterator.next()) {
                    builder.append("class ");
                    builder.append(generic->name);
                    if (i < parameters.length - 1)
                        builder.append(", ");
                    i = i + 1;
                }
            }
            builder.append(">\n");
        }
        builder.append("struct ");
        builder.append(name);
    }

    bool build_type(StringBuilder& builder, Type* type) {
        Region _r;
        if (type->name.length == 1 && type->name[0]->equals(String(_r.get_page(), "pointer"))) {
            auto generic_iterator = type->generics->get_iterator();
            while(auto generic = generic_iterator.next()) {
                build_type(builder, generic);
                break;
            }
            builder.append('*');
            return false;
        }

        {
            auto type_name_iterator = type->name.get_iterator();
            size_t i = 0;
            while (auto name_part = type_name_iterator.next()) {
                builder.append(*name_part);
                if (i < type->name.length - 1)
                    builder.append(".");
                i++;
            }
        }

        if (type->generics != nullptr) {
            if (type->generics->length > 0) {
                builder.append('<');
                {
                    auto generic_iterator = type->generics->get_iterator();
                    size_t i = 0;
                    while(auto generic = generic_iterator.next()) {
                        build_type(builder, generic);
                        if (i < type->generics->length - 1)
                            builder.append(", ");
                        i++;
                    }
                }
                builder.append('>');
                return false;
            } else {
                return true;
            }
        }
        return false;
    }

    TranspilerError* build_intrinsic(Page* _ep, StringBuilder& header_builder, String name) {
        Region _r;

        if (name.equals("size_t")) {
            header_builder.append("typedef __SIZE_TYPE__ size_t;\n");
        }
        else
            return new(alignof(TranspilerError), _ep) TranspilerError(NotImplemented(String(_ep, name)));
            

        return nullptr;
    }

    TranspilerError* build_global(Page* _ep, StringBuilder& header_builder, String name, Global& global) {
        Region _r;

        auto operation = global.value;
        if (global.type.generics != nullptr && global.type.generics->length == 0 && global.value.length == 1 && global.value[0]->expression._tag == Expression::Matrix) {
            header_builder.append("static ");
            build_type(header_builder, &global.type);
            header_builder.append(' ');
            header_builder.append(name);
            header_builder.append("[]");
            header_builder.append(" = { ");
            auto operations = global.value[0]->expression._Matrix.operations;
            auto _operation_iterator = operations.get_iterator();
            while (auto operation = _operation_iterator.next()) {
                build_operation(_ep, header_builder, *operation, nullptr, nullptr, String());
                header_builder.append(", ");
            }
            header_builder.append('}');
        } else {
            header_builder.append("const ");            
            build_type(header_builder, &global.type);
            header_builder.append(' ');
            header_builder.append(name);
            header_builder.append(" = ");
            build_operation(_ep, header_builder, operation, nullptr, nullptr, String());
        }
        header_builder.append(";\n");
        return nullptr;
    }

    TranspilerError* forward_includes(Page* _ep, String path, Program& program) {
        Region _r;
        StringBuilder& builder = *new (alignof(StringBuilder), _r.get_page()) StringBuilder();
        builder.append("\
typedef const char const_char;\n\
typedef struct stat struct_stat;\n\
typedef const void const_void;\n");
        {
            auto _result = forward_includes_for_modules(_ep, builder,  program.module.modules);
            if (_result != nullptr)
                return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
        }

        {
            auto _result = forward_includes_for_symbols(_ep, builder,  program.module.members);
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
            auto _result = forward_includes_for_symbols(_ep, builder, namespace_.members);
            if (_result != nullptr)
                return _result;
        }
        builder.append("}\n");
        return nullptr;
    }

    TranspilerError* forward_includes_for_modules(Page* _ep, StringBuilder& builder, Vector<Module> modules) {
        Region _r;
        auto module_iterator = modules.get_iterator();
        while (auto module = module_iterator.next()) {
            {
                auto _result = forward_includes_for_symbols(_ep, builder, module->members);
                if (_result != nullptr)
                    return new(alignof(TranspilerError), _ep) TranspilerError(*_result);
            }
        }

        return nullptr;
    }

    TranspilerError* forward_includes_for_symbols(Page* _ep, StringBuilder& builder, Vector<Member> members) {
        Region _r;
        auto member_iterator = members.get_iterator();
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Member::Concept:
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
				\"-ferror-limit=1\",\n\
				\"-g\",\n\
				\"${workspaceFolder}/main.cpp\",\n");
            build_vscode_source_files(tasks_builder, String(), program.module);
            tasks_builder.append("\
				\"-o\",\n\
				\"${workspaceFolder}/bin/");
            tasks_builder.append(program.module.name);
            tasks_builder.append("\",\n\
				//\"`llvm-config --cxxflags --ldflags --system-libs --libs core`\"\n\
				\"-I/opt/homebrew/Cellar/llvm/19.1.1/include\",\n\
				\"-std=c++17\",\n\
				\"-stdlib=libc++\",\n\
				\"-D__STDC_CONSTANT_MACROS\",\n\
				\"-D__STDC_FORMAT_MACROS\",\n\
				\"-D__STDC_LIMIT_MACROS\",\n\
				\"-L/opt/homebrew/Cellar/llvm/19.1.1/lib\",\n\
				\"-Wl,-search_paths_first\",\n\
				\"-Wl,-headerpad_max_install_names\",\n\
				\"-lLLVM-19\",\n\
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
        build_vscode_source_files_list(builder, path_builder.to_string(_r.get_page()), module.modules, module.members);
    }


    void build_vscode_source_files_list(StringBuilder& builder, String path, Vector<Module>& modules, Vector<Member> members) {
        Region _r;
        auto _module_iterator = modules.get_iterator();
        while (auto module = _module_iterator.next()) {
            build_vscode_source_files(builder, path, *module);
        }

        auto member_iterator = members.get_iterator();
        while (auto member = member_iterator.next()) {
            switch (member->_tag) {
                case Member::Concept:
                    {
                        auto concept = member->_Concept;
                        switch (concept.definition._tag) {
                            case Definition::Namespace: {
                                auto namespace_ = concept.definition._Namespace;
                                build_vscode_source_files_list(builder, path, namespace_.modules, namespace_.members);
                            }
                            break;

                            case Definition::Structure: {
                                auto structure = concept.definition._Structure;
                                build_vscode_source_files_list(builder, path, structure.modules, structure.members);
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
        builder.append("int main(int argc, char** argv) {");

        auto iter = program.statements.get_iterator();
        while(auto statement = iter.next()) {
            builder.append("\n    ");
            build_statement(_ep, builder, statement, nullptr, nullptr, String());
            builder.append(';');
        }
        builder.append("\n    return 0;\n}\n");
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