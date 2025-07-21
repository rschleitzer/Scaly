#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;

namespace transpiler {


Result<Void, TranspilerError> transpile_program(Page* ep, Program& program) {
    auto r = Region();
    const auto file = program.module_.file;
    auto path = Path::get_directory_name(r.get_page(), file);
    if (path.get_length()>0) {
        const auto _exists_result = Directory::exists(ep, path);
        auto exists = _exists_result._Ok;
        if (_exists_result._tag == Success::Error) {
            const auto _exists_Error = _exists_result._Error;
            switch (_exists_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_exists_result._Error);

            }
        };
        if (exists == false) {
            {
                const auto _void_result = Directory::create(ep, path);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
        };
    };
    {
        const auto _void_result = build_module(ep, path, program.module_, program.module_.name, String(), String());
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    
    auto _module__iterator = program.module_.modules.get_iterator();
    while (auto _module_ = _module__iterator.next()) {
        auto module_ = *_module_;{
            {
                const auto _void_result = build_module(ep, path, module_, module_.name, String(), String());
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
        }
    };
    if (program.statements.length>0) 
        main_file(ep, path, program);
    main_include_file(ep, path, program.module_.name);
    forward_includes(ep, path, program);
    vscode_files(ep, path, program);
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_module(Page* ep, String path, Module& module_, String main_header, String namespace_open, String namespace_close) {
    auto r = Region();
    {
        const auto _void_result = create_directory(ep, path);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    path = Path::join(r.get_page(), path, module_.name);
    StringBuilder& header_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    StringBuilder& cpp_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    cpp_builder.append("#include \"");
    cpp_builder.append(main_header);
    cpp_builder.append(".h\"");
    cpp_builder.append(namespace_open);
    header_builder.append("#ifndef _");
    header_builder.append(module_.name);
    header_builder.append("_h\n");
    header_builder.append("#define _");
    header_builder.append(module_.name);
    header_builder.append("_h\n");
    if (module_.name.equals(main_header)) {
        if (module_.name.equals("scaly")) {
            header_builder.append("#include <stdlib.h>\n\
#include <memory.h>\n\
#include <stdio.h>\n\
#include <unistd.h>\n\
#include <sys/stat.h>\n\
#include <errno.h>\n\
#include <math.h>\n\
#include <libgen.h>\n\
#include <fcntl.h>\n\
#include \"forwards.h\"\n\
\n\
struct Void {};\n\
enum Success { Ok, Error };\n\
\n\
template<class OK, class ERROR>\n\
struct Result {\n\
    Result(OK _Ok) : _tag(Ok), _Ok(_Ok) {}\n\
    Result(ERROR _ERROR) : _tag(Error), _Error(_ERROR) {}\n\
    Success _tag;\n\
    union {\n\
        OK _Ok;\n\
        ERROR _Error;\n\
    };\n\
};\n\
\n\
");
        }
        else {
            header_builder.append("#include \"../scaly/scaly.h\"\n#include \"forwards.h\"\n");
        };
    }
    else {
        header_builder.append("#include \"");
        header_builder.append(main_header);
        header_builder.append(".h\"\n");
    };
    if (module_.name.equals("scaly") == false&&module_.name.equals("memory") == false) 
        header_builder.append("using namespace scaly;\nusing namespace scaly::memory;\n");
    build_uses(header_builder, module_.uses);
    build_uses(cpp_builder, module_.uses);
    StringBuilder& namespace_open_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(namespace_open);
    {
        const auto _void_result = build_symbols(ep, path, module_.file, nullptr, header_builder, cpp_builder, main_header, namespace_open, namespace_close, module_.members);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    cpp_builder.append('\n');
    cpp_builder.append(namespace_close);
    header_builder.append("\n#endif");
    StringBuilder& header_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(path);
    header_name_builder.append(".h");
    auto header_name = header_name_builder.to_string(r.get_page());
    if (header_name.equals("scaly/compiler/Generator.h") == false) 
        {
        const auto _void_result = File::write_from_string(ep, header_name, header_builder.to_string(r.get_page()));
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    StringBuilder& cpp_name_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(path);
    cpp_name_builder.append(".cpp");
    auto cpp_name = cpp_name_builder.to_string(r.get_page());
    if (cpp_name.equals("scaly/compiler/Generator.cpp") == false) 
        {
        const auto _void_result = File::write_from_string(ep, cpp_name, cpp_builder.to_string(r.get_page()));
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

Result<Void, FileError> create_directory(Page* ep, String path) {
    if (path.get_length()>0) {
        const auto _exists_result = Directory::exists(ep, path);
        auto exists = _exists_result._Ok;
        if (_exists_result._tag == Success::Error) {
            const auto _exists_Error = _exists_result._Error;
            switch (_exists_Error._tag) {
            default:
                return Result<Void, FileError>(_exists_result._Error);

            }
        };
        if (exists == false) {
            {
                const auto _void_result = Directory::create(ep, path);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, FileError>(_void_result._Error);

                    }
                }}
                ;
        };
    };
    return Result<Void, FileError>(Void());
}

Result<Void, TranspilerError> build_concept(Page* ep, String path, String source, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Concept& concept) {
    {
        auto _result = concept.definition;
        switch (_result._tag)
        {
            case Definition::Namespace:
            {
                auto ns = _result._Namespace;
                return Result<Void, TranspilerError>(build_namespace(ep, path, source, concept.name, header_builder, cpp_builder, main_header, namespace_open, namespace_close, ns));
                break;
            }
            case Definition::Structure:
            {
                auto structure = _result._Structure;
                return Result<Void, TranspilerError>(build_structure(ep, header_builder, cpp_builder, concept.name, structure, concept.parameters));
                break;
            }
            case Definition::Intrinsic:
            {
                auto i = _result._Intrinsic;
                return Result<Void, TranspilerError>(build_intrinsic(ep, header_builder, concept.name));
                break;
            }
            case Definition::Global:
            {
                auto g = _result._Global;
                return Result<Void, TranspilerError>(build_global(ep, header_builder, concept.name, g));
                break;
            }
            case Definition::Union:
            {
                auto u = _result._Union;
                return Result<Void, TranspilerError>(build_union(ep, header_builder, cpp_builder, concept.name, u, concept.parameters));
                break;
            }
        }
    };
}

Result<Void, TranspilerError> build_union(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Union& union_, Vector<GenericParameter> parameters) {
    header_builder.append('\n');
    full_struct_name(header_builder, name, parameters);
    header_builder.append(" : Object");
    header_builder.append(" {\n");
    {
        
        auto _variant_iterator = union_.variants.get_iterator();
        while (auto _variant = _variant_iterator.next()) {
            auto variant = *_variant;{
                header_builder.append("    ");
                header_builder.append(name);
                header_builder.append('(');
                build_type(header_builder, variant.type, true);
                header_builder.append(")");
                if ((parameters.length>0)) {
                    header_builder.append(" : _tag(");
                    header_builder.append(variant.name);
                    header_builder.append(") { _");
                    header_builder.append(variant.name);
                    header_builder.append(" = _");
                    build_type(header_builder, variant.type, true);
                    header_builder.append("; }");
                }
                else {
                    header_builder.append(";");
                };
                header_builder.append("\n");
            }
        };
    };
    header_builder.append("    enum {\n");
    
    auto _variant_iterator = union_.variants.get_iterator();
    while (auto _variant = _variant_iterator.next()) {
        auto variant = *_variant;{
            header_builder.append("        ");
            header_builder.append(variant.name);
            header_builder.append(",\n");
        }
    };
    header_builder.append("    } _tag;\n    union {\n");
    {
        
        auto _variant_iterator = union_.variants.get_iterator();
        while (auto _variant = _variant_iterator.next()) {
            auto variant = *_variant;{
                if (variant.type == nullptr) 
                    continue;header_builder.append("        struct ");
                build_type(header_builder, variant.type, true);
                header_builder.append(" _");
                header_builder.append(variant.name);
                header_builder.append(";\n");
            }
        };
    };
    header_builder.append("    };");
    
    auto _member_iterator = union_.members.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Function:
                    {
                        auto func = _result._Function;
                        {
                            const auto _void_result = build_function(ep, header_builder, cpp_builder, func, &name, parameters.length>0, true);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Operator:
                    {
                        auto op = _result._Operator;
                        {
                            const auto _void_result = build_operator(ep, header_builder, cpp_builder, op, &name, parameters.length>0);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Package:
                    {
                        auto p = _result._Package;
                        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "structure local package")));
                        break;
                    }
                    case Member::Concept:
                    {
                        auto concept = _result._Concept;
                        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "structure local concept")));
                        break;
                    }
                }
            };
        }
    };
    header_builder.append("\n};");
    if (parameters.length == 0) {
        
        auto _variant_iterator = union_.variants.get_iterator();
        while (auto _variant = _variant_iterator.next()) {
            auto variant = *_variant;{
                cpp_builder.append('\n');
                cpp_builder.append(name);
                cpp_builder.append("::");
                cpp_builder.append(name);
                cpp_builder.append("(struct ");
                build_type(cpp_builder, variant.type, true);
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
        };
    };
    return Result<Void, TranspilerError>(Void());
}

bool build_type(StringBuilder& builder, Type* type, bool colons) {
    auto r = Region();
    if ((*type).name.length==1&&(*(*type).name.get(0)).equals(String(r.get_page(), "pointer"))) {
        
        auto _generic_iterator = (*(*type).generics).get_iterator();
        while (auto _generic = _generic_iterator.next()) {
            auto generic = *_generic;{
                build_type(builder, &generic, colons);
                break;
            }
        };
        builder.append('*');
        return false;
    };
    if ((*type).name.length==1&&(*(*type).name.get(0)).equals(String(r.get_page(), "ref"))) {
        
        auto _generic_iterator = (*(*type).generics).get_iterator();
        while (auto _generic = _generic_iterator.next()) {
            auto generic = *_generic;{
                build_type(builder, &generic, colons);
                break;
            }
        };
        builder.append('&');
        return false;
    };
    {
        size_t i = 0;
        
        auto _name_part_iterator = (*type).name.get_iterator();
        while (auto _name_part = _name_part_iterator.next()) {
            auto name_part = *_name_part;{
                builder.append(name_part);
                if (i<(*type).name.length-1) {
                    if (colons) {
                        builder.append("::");
                    }
                    else {
                        builder.append(".");
                    };
                };
                i = i+1;
            }
        };
    };
    if ((*type).generics != nullptr) {
        if ((*(*type).generics).length>0) {
            builder.append('<');
            size_t i = 0;
            
            auto _generic_iterator = (*(*type).generics).get_iterator();
            while (auto _generic = _generic_iterator.next()) {
                auto generic = *_generic;{
                    build_type(builder, &generic, colons);
                    if (i<(*(*type).generics).length-1) 
                        builder.append(", ");
                    i = i+1;
                }
            };
            builder.append('>');
            return false;
        }
        else {
            return true;
        };
    };
    return false;
}

Result<Void, TranspilerError> build_namespace(Page* ep, String path, String source, String name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Namespace& namespace_) {
    auto r = Region();
    header_builder.append("namespace ");
    header_builder.append(name);
    header_builder.append(" {\n");
    cpp_builder.append("\nnamespace ");
    cpp_builder.append(name);
    cpp_builder.append(" {\n");
    StringBuilder& namespace_open_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(namespace_open);
    namespace_open_builder.append("\nnamespace ");
    namespace_open_builder.append(name);
    namespace_open_builder.append(" {");
    namespace_open = namespace_open_builder.to_string(r.get_page());
    StringBuilder& namespace_close_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(namespace_close);
    namespace_close_builder.append("\n}");
    namespace_close = namespace_close_builder.to_string(r.get_page());
    StringBuilder& main_header_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder("../");
    main_header_builder.append(main_header);
    {
        const auto _void_result = build_modules(ep, path, name, header_builder, namespace_.modules, main_header_builder.to_string(r.get_page()), namespace_open, namespace_close);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    {
        const auto _void_result = build_symbols(ep, path, source, nullptr, header_builder, cpp_builder, main_header_builder.to_string(r.get_page()), namespace_open, namespace_close, namespace_.members);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    header_builder.append("\n}");
    cpp_builder.append("\n\n}");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_modules(Page* ep, String path, String name, StringBuilder& header_builder, Vector<Module>& modules, String main_header, String namespace_open, String namespace_close) {
    auto r = Region();
    
    auto _module__iterator = modules.get_iterator();
    while (auto _module_ = _module__iterator.next()) {
        auto module_ = *_module_;{
            header_builder.append("#include \"");
            header_builder.append(name);
            header_builder.append('/');
            header_builder.append(String(r.get_page(), module_.name));
            header_builder.append(".h\"\n");
            {
                const auto _void_result = build_module(ep, path, module_, main_header, namespace_open, namespace_close);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_symbols(Page* ep, String path, String source, String* name, StringBuilder& header_builder, StringBuilder& cpp_builder, String main_header, String namespace_open, String namespace_close, Vector<Member> symbols) {
    auto r = Region();
    
    auto _member_iterator = symbols.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Function:
                    {
                        auto func = _result._Function;
                        {
                            const auto _void_result = build_function(ep, header_builder, cpp_builder, func, name, false, false);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Concept:
                    {
                        auto concept = _result._Concept;
                        {
                            const auto _void_result = build_concept(ep, path, source, header_builder, cpp_builder, main_header, namespace_open, namespace_close, concept);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Operator:
                    {
                        auto op = _result._Operator;
                        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "namespace local operator")));
                        break;
                    }
                    case Member::Package:
                    {
                        auto p = _result._Package;
                        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "namespace local package")));
                        break;
                    }
                }
            };
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_structure(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, Structure& structure, Vector<GenericParameter> parameters) {
    auto r = Region();
    header_builder.append('\n');
    full_struct_name(header_builder, name, parameters);
    if (name.equals(String(r.get_page(), "Object")) == false&&name.equals(String(r.get_page(), "Void")) == false) 
        header_builder.append(" : Object");
    header_builder.append(" {");
    auto must_build_properties_initializer = false;
    
    auto _property_iterator = structure.properties.get_iterator();
    while (auto _property = _property_iterator.next()) {
        auto property = *_property;{
            if ((property.type)) {
                header_builder.append('\n');
                header_builder.append("    ");
                build_type(header_builder, property.type, true);
            };
            header_builder.append(' ');
            header_builder.append(property.name);
            header_builder.append(';');
            if (property.initializer != nullptr) 
                must_build_properties_initializer = true;
        }
    };
    if (parameters.length == 0) 
        header_builder.append('\n');
    if (structure.properties.length>0) 
        build_default_initializer(ep, header_builder, cpp_builder, name, parameters.length>0, structure.properties);
    if (must_build_properties_initializer) 
        {
        const auto _void_result = build_properties_initializer(ep, header_builder, cpp_builder, name, parameters.length>0, structure.properties);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    
    auto _initializer_iterator = structure.initializers.get_iterator();
    while (auto _initializer = _initializer_iterator.next()) {
        auto initializer = *_initializer;{
            {
                const auto _void_result = build_initializer(ep, header_builder, cpp_builder, name, parameters.length>0, &initializer);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
        }
    };
    if (structure.deinitializer != nullptr) 
        {
        const auto _void_result = build_deinitializer(ep, header_builder, cpp_builder, name, parameters.length>0, structure.deinitializer);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    
    auto _member_iterator = structure.members.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Function:
                    {
                        auto func = _result._Function;
                        {
                            const auto _void_result = build_function(ep, header_builder, cpp_builder, func, &name, parameters.length>0, true);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Operator:
                    {
                        auto op = _result._Operator;
                        {
                            const auto _void_result = build_operator(ep, header_builder, cpp_builder, op, &name, parameters.length>0);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Member::Package:
                    {
                        auto p = _result._Package;
                        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "structure local package")));
                        break;
                    }
                    case Member::Concept:
                    {
                        auto c = _result._Concept;
                        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "structure local concept")));
                        break;
                    }
                }
            };
        }
    };
    header_builder.append("\n};\n");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_default_initializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Vector<Property>& properties) {
    auto r = Region();
    build_initializer_header(header_builder, cpp_builder, name, is_generic);
    if (is_generic) {
        build_default_initializer_list(ep, header_builder, is_generic, properties, String(r.get_page(), "        "));
        header_builder.append(" {}");
    }
    else {
        build_default_initializer_list(ep, cpp_builder, is_generic, properties, String(r.get_page(), "    "));
        cpp_builder.append(" {}");
        header_builder.append("(");
        auto first = true;
        
        auto _property_iterator = properties.get_iterator();
        while (auto _property = _property_iterator.next()) {
            auto property = *_property;{
                if ((first)) {
                    first = false;
                }
                else {
                    header_builder.append(", ");
                };
                build_type(header_builder, property.type, true);
                header_builder.append(' ');
                header_builder.append(property.name);
            }
        };
        header_builder.append(");");
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_default_initializer_list(Page* ep, StringBuilder& builder, bool is_generic, Vector<Property>& properties, String indent) {
    builder.append('(');
    {
        auto first = true;
        
        auto _property_iterator = properties.get_iterator();
        while (auto _property = _property_iterator.next()) {
            auto property = *_property;{
                if (first) {
                    first = false;
                }
                else {
                    builder.append(", ");
                };
                build_type(builder, property.type, true);
                builder.append(' ');
                builder.append(property.name);
            }
        };
    };
    builder.append(") : ");
    {
        auto first = true;
        
        auto _property_iterator = properties.get_iterator();
        while (auto _property = _property_iterator.next()) {
            auto property = *_property;{
                if (first) {
                    first = false;
                }
                else {
                    builder.append(", ");
                };
                builder.append(property.name);
                builder.append('(');
                builder.append(property.name);
                builder.append(')');
            }
        };
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_properties_initializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Vector<Property>& properties) {
    auto r = Region();
    build_initializer_header(header_builder, cpp_builder, name, is_generic);
    if ((is_generic)) {
        build_properties_initializer_list(ep, header_builder, is_generic, properties, String(r.get_page(), "        "));
        header_builder.append(" {}");
    }
    else {
        build_properties_initializer_list(ep, cpp_builder, is_generic, properties, String(r.get_page(), "    "));
        cpp_builder.append(" {}");
        header_builder.append(" ();");
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_properties_initializer_list(Page* ep, StringBuilder& builder, bool is_generic, Vector<Property>& properties, String indent) {
    builder.append("() : ");
    auto first = true;
    
    auto _property_iterator = properties.get_iterator();
    while (auto _property = _property_iterator.next()) {
        auto property = *_property;{
            if (property.initializer == nullptr) 
                continue;if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            };
            builder.append(property.name);
            builder.append('(');
            {
                const auto _void_result = build_operation(ep, builder, *property.initializer, nullptr, nullptr, nullptr, indent);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
            builder.append(')');
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_initializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, Initializer* initializer) {
    auto r = Region();
    build_initializer_header(header_builder, cpp_builder, name, is_generic);
    if (is_generic == false) {
        build_input(cpp_builder, (*initializer).input, nullptr, nullptr, nullptr, Lifetime(Unspecified()));
        cpp_builder.append(' ');
        {
            const auto _void_result = build_implementation(ep, cpp_builder, (*initializer).implementation, nullptr, nullptr, String(), true);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    build_input(header_builder, (*initializer).input, nullptr, nullptr, nullptr, Lifetime(Unspecified()));
    if (is_generic) {
        header_builder.append(' ');
        {
            const auto _void_result = build_implementation(ep, header_builder, (*initializer).implementation, nullptr, nullptr, String(r.get_page(), "    "), true);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    }
    else {
        header_builder.append(';');
    };
    return Result<Void, TranspilerError>(Void());
}

void build_initializer_header(StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic) {
    if ((is_generic)) 
        header_builder.append('\n');
    header_builder.append("\n    ");
    header_builder.append(name);
    if (is_generic == false) {
        cpp_builder.append("\n\n");
        cpp_builder.append(name);
        cpp_builder.append("::");
        cpp_builder.append(name);
    };
}

Result<Void, TranspilerError> build_deinitializer(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, String name, bool is_generic, DeInitializer* de_initializer) {
    auto r = Region();
    header_builder.append("\n    ");
    header_builder.append('~');
    header_builder.append(name);
    if (is_generic == false) {
        cpp_builder.append('\n');
        cpp_builder.append(name);
        cpp_builder.append("::~");
        cpp_builder.append(name);
        cpp_builder.append("() ");
        {
            const auto _void_result = build_implementation(ep, cpp_builder, (*de_initializer).implementation, nullptr, nullptr, String(r.get_page(), String()), true);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
        cpp_builder.append(';');
    };
    if (is_generic) {
        header_builder.append("() ");
        {
            const auto _void_result = build_implementation(ep, header_builder, (*de_initializer).implementation, nullptr, nullptr, String(r.get_page(), "    "), true);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
        header_builder.append(';');
    }
    else {
        header_builder.append("();");
    };
    return Result<Void, TranspilerError>(Void());
}

void build_uses(StringBuilder& builder, Vector<Use>& uses) {
    auto r = Region();
    
    auto _use__iterator = uses.get_iterator();
    while (auto _use_ = _use__iterator.next()) {
        auto use_ = *_use_;{
            builder.append("\nusing namespace ");
            auto first = true;
            
            auto _namespace__iterator = use_.path.get_iterator();
            while (auto _namespace_ = _namespace__iterator.next()) {
                auto namespace_ = *_namespace_;{
                    if (first) {
                        first = false;
                    }
                    else {
                        builder.append("::");
                    };
                    builder.append(namespace_);
                }
            };
            builder.append(";\n");
        }
    };
}

Result<Void, TranspilerError> build_intrinsic(Page* ep, StringBuilder& header_builder, String name) {
    auto r = Region();
    if (name.equals("size_t")) {
        header_builder.append("typedef __SIZE_TYPE__ size_t\n");
    }
    else {
        return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, name)));
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_global(Page* ep, StringBuilder& header_builder, String name, Global& global) {
    auto r = Region();
    if (global.type.generics != nullptr&&(*global.type.generics).length == 0&&global.value.length == 1&&(*global.value.get(0)).expression._tag == Expression::Matrix) {
        {
            auto _result = (*global.value.get(0)).expression;
            switch (_result._tag)
            {
                case Expression::Matrix:
                {
                    auto matrix = _result._Matrix;
                    {
                        header_builder.append("static ");
                        build_type(header_builder, &global.type, true);
                        header_builder.append(' ');
                        header_builder.append(name);
                        header_builder.append("[]");
                        header_builder.append(" = { ");
                        
                        auto _operation_iterator = matrix.operations.get_iterator();
                        while (auto _operation = _operation_iterator.next()) {
                            auto operation = *_operation;{
                                build_operation(ep, header_builder, operation, nullptr, nullptr, nullptr, String());
                                header_builder.append(", ");
                            }
                        };
                        header_builder.append('}');
                    };
                    break;
                }
                default:
                    {
                };
            }
        };
    }
    else {
        header_builder.append("const ");
        build_type(header_builder, &global.type, true);
        header_builder.append(' ');
        header_builder.append(name);
        header_builder.append(" = ");
        build_operation(ep, header_builder, global.value, nullptr, nullptr, nullptr, String());
    };
    header_builder.append(";\n");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_function(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Function& func, String* name, bool is_template, bool in_class) {
    auto r = Region();
    String* location = nullptr;
    {
        auto _result = func.lifetime;
        switch (_result._tag)
        {
            case Lifetime::Call:
            {
                auto c = _result._Call;
                location = new (alignof(String), r.get_page()) String(r.get_page(), "rp");
                break;
            }
            case Lifetime::Reference:
            {
                auto reference = _result._Reference;
                location = &reference.location;
                break;
            }
            case Lifetime::Unspecified:
            {
                auto u = _result._Unspecified;
                {
                };
                break;
            }
            case Lifetime::Local:
            {
                auto l = _result._Local;
                {
                };
                break;
            }
            case Lifetime::Thrown:
            {
                auto t = _result._Thrown;
                {
                };
                break;
            }
        }
    }{
        auto _result = func.implementation;
        switch (_result._tag)
        {
            case Implementation::Extern:
            {
                auto e = _result._Extern;
                {
                    header_builder.append("extern \"C\" ");
                    build_output_type(header_builder, func.returns_, func.throws_);
                    header_builder.append(' ');
                    header_builder.append(func.name);
                    build_input(header_builder, func.input, location, func.returns_, func.throws_, func.lifetime);
                    header_builder.append(";\n");
                    return Result<Void, TranspilerError>(Void());
                };
                break;
            }
            default:
                {
            };
        }
    };
    if (is_template) 
        header_builder.append('\n');
    function_prefix(header_builder, func, name != nullptr, in_class);
    if (is_template == false) {
        cpp_builder.append('\n');
        function_prefix(cpp_builder, func, false, false);
        if (name != nullptr) {
            cpp_builder.append(*name);
            cpp_builder.append("::");
        };
        cpp_builder.append(func.name);
        build_input(cpp_builder, func.input, location, func.returns_, func.throws_, func.lifetime);
        cpp_builder.append(' ');
        {
            const auto _void_result = build_implementation(ep, cpp_builder, func.implementation, func.returns_, func.throws_, String(), false);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    header_builder.append(func.name);
    build_input(header_builder, func.input, location, func.returns_, func.throws_, func.lifetime);
    if (is_template) {
        header_builder.append(' ');
        {
            const auto _void_result = build_implementation(ep, header_builder, func.implementation, func.returns_, func.throws_, String(r.get_page(), "    "), false);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    header_builder.append(';');
    return Result<Void, TranspilerError>(Void());
}

void build_output_type(StringBuilder& builder, Type* returns_, Type* throws_) {
    if (throws_ != nullptr) {
        builder.append("Result<");
        if (returns_ == nullptr) {
            builder.append("Void");
        }
        else {
            build_type(builder, returns_, true);
        };
        builder.append(", ");
        build_type(builder, throws_, true);
        builder.append('>');
    }
    else {
        if (returns_ == nullptr) {
            builder.append("void");
        }
        else {
            build_type(builder, returns_, true);
        };
    };
}

Result<Void, TranspilerError> build_implementation(Page* ep, StringBuilder& builder, Implementation& implementation, Type* returns_, Type* throws_, String indent, bool is_initializer) {
    {
        auto _result = implementation;
        switch (_result._tag)
        {
            case Implementation::Action:
            {
                auto action = _result._Action;
                {
                    if (action.target.length==0&&action.source.length==1&&(*action.source.get(0)).expression._tag == Expression::Block) {
                        {
                            const auto _void_result = build_action(ep, builder, action, returns_, throws_, nullptr, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                    }
                    else {
                        if (is_initializer) {
                            auto first = true;
                            
                            auto _operand_iterator = action.source.get_iterator();
                            while (auto _operand = _operand_iterator.next()) {
                                auto operand = *_operand;{
                                    {
                                        auto _result = operand.expression;
                                        switch (_result._tag)
                                        {
                                            case Expression::Tuple:
                                            {
                                                auto tuple = _result._Tuple;
                                                {
                                                    
                                                    auto _component_iterator = tuple.components.get_iterator();
                                                    while (auto _component = _component_iterator.next()) {
                                                        auto component = *_component;{
                                                            if ((first)) {
                                                                first = false;
                                                                builder.append(": ");
                                                            }
                                                            else {
                                                                builder.append(", ");
                                                            };
                                                            {
                                                                const auto _void_result = build_operation(ep, builder, component.value, returns_, throws_, nullptr, indent);
                                                                if (_void_result._tag == Success::Error) {
                                                                    const auto _void_Error = _void_result._Error;
                                                                    switch (_void_Error._tag) {
                                                                    default:
                                                                        return Result<Void, TranspilerError>(_void_result._Error);

                                                                    }
                                                                }}
                                                                ;
                                                        }
                                                    };
                                                };
                                                break;
                                            }
                                            case Expression::Block:
                                            {
                                                auto block = _result._Block;
                                                {
                                                    {
                                                        const auto _void_result = build_block(ep, builder, block, returns_, throws_, nullptr, indent);
                                                        if (_void_result._tag == Success::Error) {
                                                            const auto _void_Error = _void_result._Error;
                                                            switch (_void_Error._tag) {
                                                            default:
                                                                return Result<Void, TranspilerError>(_void_result._Error);

                                                            }
                                                        }}
                                                        ;
                                                };
                                                break;
                                            }
                                            default:
                                                return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Other than Tuple or Block in initializer implementation")));
                                        }
                                    };
                                }
                            };
                        }
                        else {
                            builder.append("{\n");
                            builder.append(indent);
                            builder.append("    return ");
                            {
                                const auto _void_result = build_action(ep, builder, action, returns_, throws_, nullptr, indent);
                                if (_void_result._tag == Success::Error) {
                                    const auto _void_Error = _void_result._Error;
                                    switch (_void_Error._tag) {
                                    default:
                                        return Result<Void, TranspilerError>(_void_result._Error);

                                    }
                                }}
                                ;
                            builder.append("; }");
                        };
                    };
                };
                break;
            }
            case Implementation::Extern:
            {
                auto e = _result._Extern;
                return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Extern")));
                break;
            }
            case Implementation::Instruction:
            {
                auto i = _result._Instruction;
                return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Instruction")));
                break;
            }
            case Implementation::Intrinsic:
            {
                auto i = _result._Intrinsic;
                return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Intrinsic")));
                break;
            }
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_binding(Page* ep, StringBuilder& builder, Binding& binding, Type* returns_, Type* throws_, String* re_throw, String indent) {
    if (binding.binding_type.equals("const")) 
        builder.append("const ");
    auto simple_array = false;
    if (binding.item.type != nullptr) {
        simple_array = build_type(builder, binding.item.type, true);
    }
    else {
        builder.append("auto");
    };
    if (binding.item.name != nullptr) {
        builder.append(' ');
        builder.append(*binding.item.name);
    };
    if (simple_array) {
        builder.append('[');
        {
            const auto _void_result = build_operation(ep, builder, binding.operation, returns_, throws_, re_throw, indent);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
        builder.append(']');
    }
    else {
        builder.append(" = ");
        {
            const auto _void_result = build_operation(ep, builder, binding.operation, returns_, throws_, re_throw, indent);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_action(Page* ep, StringBuilder& builder, Action& action, Type* returns_, Type* throws_, String* re_throw, String indent) {
    if (action.target.length>0) {
        {
            const auto _void_result = build_operation(ep, builder, action.target, returns_, throws_, re_throw, indent);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
        builder.append(" = ");
    };
    {
        const auto _void_result = build_operation(ep, builder, action.source, returns_, throws_, re_throw, indent);
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

Result<Void, TranspilerError> build_operation(Page* ep, StringBuilder& builder, Vector<Operand>& operation, Type* returns_, Type* throws_, String* re_throw, String indent) {
    
    auto _operand_iterator = operation.get_iterator();
    while (auto _operand = _operand_iterator.next()) {
        auto operand = *_operand;{
            {
                auto _result = operand.expression;
                switch (_result._tag)
                {
                    case Expression::Constant:
                    {
                        auto constant = _result._Constant;
                        {
                            const auto _void_result = build_constant(ep, builder, constant);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Type:
                    {
                        auto type = _result._Type;
                        {
                            const auto _void_result = build_variable(ep, builder, type, operand.member_access);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Tuple:
                    {
                        auto tuple = _result._Tuple;
                        {
                            const auto _void_result = build_tuple(ep, builder, tuple, operand.member_access, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Matrix:
                    {
                        auto matrix = _result._Matrix;
                        {
                            const auto _void_result = build_matrix(ep, builder, matrix, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Block:
                    {
                        auto block = _result._Block;
                        {
                            const auto _void_result = build_block(ep, builder, block, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::If:
                    {
                        auto if_ = _result._If;
                        {
                            const auto _void_result = build_if(ep, builder, if_, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Match:
                    {
                        auto match_ = _result._Match;
                        {
                            const auto _void_result = build_match(ep, builder, match_, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Choose:
                    {
                        auto choose_ = _result._Choose;
                        {
                            const auto _void_result = build_choose(ep, builder, choose_, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::For:
                    {
                        auto for_ = _result._For;
                        {
                            const auto _void_result = build_for(ep, builder, for_, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::While:
                    {
                        auto while_ = _result._While;
                        {
                            const auto _void_result = build_while(ep, builder, while_, returns_, throws_, re_throw, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::Try:
                    {
                        auto try_ = _result._Try;
                        {
                            const auto _void_result = build_try(ep, builder, try_, returns_, throws_, nullptr, indent);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    case Expression::SizeOf:
                    {
                        auto sizeof_ = _result._SizeOf;
                        build_sizeof(builder, sizeof_, indent);
                        break;
                    }
                    case Expression::Is:
                    {
                        auto is_ = _result._Is;
                        build_is(builder, is_, indent);
                        break;
                    }
                    case Expression::New:
                    {
                        auto new__ = _result._New;
                        build_new(ep, builder, new__, returns_, throws_, re_throw, indent);
                        break;
                    }
                }
            };
        }
    };
    return Result<Void, TranspilerError>(Void());
}

void build_string(StringBuilder& builder, String string) {
    builder.append('\"');
    
    auto _c_iterator = string.get_iterator();
    while (auto _c = _c_iterator.next()) {
        auto c = *_c;{
            if (c == '\n') 
                builder.append("\\n\\");
            builder.append(c);
        }
    };
    builder.append('\"');
}

Result<Void, TranspilerError> build_constant(Page* ep, StringBuilder& builder, Constant& constant) {
    auto r = Region();
    {
        auto _result = constant;
        switch (_result._tag)
        {
            case Constant::Boolean:
            {
                auto boolean = _result._Boolean;
                {
                    if (boolean.value) {
                        builder.append("true");
                    }
                    else {
                        builder.append("false");
                    };
                };
                break;
            }
            case Constant::Integer:
            {
                auto integer = _result._Integer;
                {
                    auto str = (char*)(*r.get_page()).allocate(32, 1);
                    snprintf(str, 32, "%zd", integer.value);
                    builder.append(String(r.get_page(), str));
                };
                break;
            }
            case Constant::Hex:
            {
                auto hex = _result._Hex;
                {
                    auto str = (char*)(*r.get_page()).allocate(32, 1);
                    snprintf(str, 32, "0x%zx", hex.value);
                    builder.append(String(r.get_page(), str));
                };
                break;
            }
            case Constant::FloatingPoint:
            {
                auto floating_point = _result._FloatingPoint;
                {
                    auto str = (char*)(*r.get_page()).allocate(32, 1);
                    snprintf(str, 32, "%lg", floating_point.value);
                    builder.append(String(r.get_page(), str));
                };
                break;
            }
            case Constant::String:
            {
                auto string = _result._String;
                {
                    build_string(builder, string.value);
                };
                break;
            }
            case Constant::Character:
            {
                auto string = _result._Character;
                {
                    builder.append('\'');
                    builder.append(string.value);
                    builder.append('\'');
                };
                break;
            }
            case Constant::Fragment:
            {
                auto fragment = _result._Fragment;
                {
                    return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Bool")));
                };
                break;
            }
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_variable(Page* ep, StringBuilder& builder, Type& type, Vector<String>* member_access) {
    auto name_iterator = type.name.get_iterator();
    const auto first_name_part = name_iterator.next();
    if ((*first_name_part).equals("=")) {
        builder.append(" == ");
        return Result<Void, TranspilerError>(Void());
    };
    if ((*first_name_part).equals("<>")) {
        builder.append(" != ");
        return Result<Void, TranspilerError>(Void());
    };
    if ((*first_name_part).equals("//")) {
        builder.append(" ^ ");
        return Result<Void, TranspilerError>(Void());
    };
    if ((*first_name_part).equals("->")) {
        builder.append("::");
        return Result<Void, TranspilerError>(Void());
    };
    if ((*first_name_part).equals("null")) {
        builder.append("nullptr");
        return Result<Void, TranspilerError>(Void());
    };
    if ((*first_name_part).equals("pointer")) {
        if (type.generics != nullptr) {
            auto generics = *(type.generics);
            build_variable(ep, builder, *generics.get(0), member_access);
        };
        builder.append('*');
        return Result<Void, TranspilerError>(Void());
    };
    {
        auto _result = type.lifetime;
        switch (_result._tag)
        {
            case Lifetime::Unspecified:
            {
                auto u = _result._Unspecified;
                {
                };
                break;
            }
            case Lifetime::Local:
            {
                auto l = _result._Local;
                {
                    builder.append("new (alignof(");
                    build_type(builder, &type, true);
                    builder.append("), r.get_page()) ");
                };
                break;
            }
            case Lifetime::Call:
            {
                auto call = _result._Call;
                {
                    builder.append("new (alignof(");
                    build_type(builder, &type, true);
                    builder.append("), rp) ");
                };
                break;
            }
            case Lifetime::Reference:
            {
                auto reference = _result._Reference;
                {
                    builder.append("new (alignof(");
                    build_type(builder, &type, true);
                    builder.append("), ");
                    builder.append(reference.location);
                    builder.append(") ");
                };
                break;
            }
            case Lifetime::Thrown:
            {
                auto t = _result._Thrown;
                return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Thrown")));
                break;
            }
        }
    }build_type(builder, &type, false);
    if (member_access != nullptr) {
        
        auto _member_iterator = (*member_access).get_iterator();
        while (auto _member = _member_iterator.next()) {
            auto member = *_member;{
                if ((*first_name_part).equals("this")) {
                    builder.append("->");
                }
                else {
                    builder.append('.');
                };
                builder.append(member);
            }
        };
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_tuple(Page* ep, StringBuilder& builder, Tuple& tuple, Vector<String>* member_access, Type* returns_, Type* throws_, String* re_throw, String indent) {
    builder.append('(');
    auto first = true;
    
    auto _property_iterator = tuple.components.get_iterator();
    while (auto _property = _property_iterator.next()) {
        auto property = *_property;{
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            };
            build_operation(ep, builder, property.value, returns_, throws_, re_throw, indent);
        }
    };
    builder.append(')');
    if (member_access != nullptr) {
        
        auto _member_iterator = (*member_access).get_iterator();
        while (auto _member = _member_iterator.next()) {
            auto member = *_member;{
                builder.append('.');
                builder.append(member);
            }
        };
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_matrix(Page* ep, StringBuilder& builder, Matrix& matrix, Type* returns_, Type* throws_, String* re_throw, String indent) {
    builder.append('[');
    auto first = true;
    
    auto _operation_iterator = matrix.operations.get_iterator();
    while (auto _operation = _operation_iterator.next()) {
        auto operation = *_operation;{
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            };
            {
                const auto _void_result = build_operation(ep, builder, operation, returns_, throws_, re_throw, indent);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
        }
    };
    builder.append(']');
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_block(Page* ep, StringBuilder& builder, Block& block, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    StringBuilder& indent_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(indent);
    indent_builder.append("    ");
    builder.append('{');
    {
        const auto _void_result = build_statements(ep, builder, block.statements, returns_, throws_, re_throw, indent_builder.to_string(r.get_page()));
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append('\n');
    builder.append(indent);
    builder.append('}');
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_if(Page* ep, StringBuilder& builder, If& if_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("if (");
    {
        const auto _void_result = build_operation(ep, builder, if_.condition, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append(") ");
    auto is_block = false;
    {
        auto _result = if_.consequent;
        switch (_result._tag)
        {
            case Statement::Action:
            {
                auto action = _result._Action;
                {
                    if ((*action.source.get(0)).expression._tag == Expression::Block) 
                        is_block = true;
                };
                break;
            }
            default:
                {
            };
        }
    };
    if (is_block == false) {
        builder.append('\n');
        builder.append(indent);
        builder.append("    ");
    };
    {
        const auto _void_result = build_statement(ep, builder, if_.consequent, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    if (if_.alternative != nullptr) {
        builder.append('\n');
        builder.append(indent);
        builder.append("else ");
        {
            const auto _void_result = build_statement(ep, builder, *if_.alternative, returns_, throws_, re_throw, indent);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_match(Page* ep, StringBuilder& builder, Match& match_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("switch (");
    {
        const auto _void_result = build_operation(ep, builder, match_.condition, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append(")\n");
    builder.append(indent);
    builder.append("{");
    StringBuilder& indent_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(indent);
    indent_builder.append("        ");
    auto indented = indent_builder.to_string(r.get_page());
    
    auto _branch_iterator = match_.branches.get_iterator();
    while (auto _branch = _branch_iterator.next()) {
        auto branch = *_branch;{
            builder.append('\n');
            builder.append(indent);
            builder.append("    ");
            
            auto _case__iterator = branch.cases.get_iterator();
            while (auto _case_ = _case__iterator.next()) {
                auto case_ = *_case_;{
                    builder.append("case ");
                    {
                        {
                            const auto _void_result = build_operation(ep, builder, case_.condition, returns_, throws_, re_throw, indented);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                    };
                    builder.append(": ");
                }
            };
            builder.append('\n');
            builder.append(indented);
            {
                const auto _void_result = build_statement(ep, builder, branch.consequent, returns_, throws_, re_throw, indented);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
            builder.append(";\n");
            builder.append(indented);
            builder.append("break;");
        }
    };
    if (match_.alternative != nullptr) {
        builder.append('\n');
        builder.append(indent);
        builder.append("    default:\n");
        builder.append(indented);
        {
            const auto _void_result = build_statement(ep, builder, *match_.alternative, returns_, throws_, re_throw, indented);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
        builder.append(';');
    };
    builder.append('\n');
    builder.append(indent);
    builder.append("}");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_choose(Page* ep, StringBuilder& builder, Choose& choose_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("{\n");
    StringBuilder& indent_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(indent);
    indent_builder.append("    ");
    const auto indent1 = indent_builder.to_string(r.get_page());
    builder.append(indent1);
    builder.append("auto _result = ");
    {
        const auto _void_result = build_operation(ep, builder, choose_.condition, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append(";\n");
    builder.append(indent1);
    builder.append("switch (_result._tag)\n");
    builder.append(indent1);
    builder.append("{");
    indent_builder.append("    ");
    const auto indented = indent_builder.to_string(r.get_page());
    indent_builder.append("    ");
    const auto indented2 = indent_builder.to_string(r.get_page());
    
    auto _case__iterator = choose_.cases.get_iterator();
    while (auto _case_ = _case__iterator.next()) {
        auto case_ = *_case_;{
            builder.append('\n');
            builder.append(indented);
            builder.append("case ");
            auto first = true;
            {
                
                auto _variant_iterator = case_.variant.get_iterator();
                while (auto _variant = _variant_iterator.next()) {
                    auto variant = *_variant;{
                        if (first) {
                            first = false;
                        }
                        else {
                            builder.append("::");
                        };
                        builder.append(variant);
                    }
                };
            };
            builder.append(":\n");
            builder.append(indent1);
            builder.append("    {\n");
            builder.append(indented2);
            builder.append("auto ");
            builder.append(case_.name);
            builder.append(" = _result._");
            first = true;
            {
                
                auto _variant_iterator = case_.variant.get_iterator();
                while (auto _variant = _variant_iterator.next()) {
                    auto variant = *_variant;{
                        if ((first)) {
                            first = false;
                        }
                        else {
                            builder.append(variant);
                        };
                    }
                };
            };
            builder.append(";\n");
            builder.append(indented2);
            {
                const auto _void_result = build_statement(ep, builder, case_.consequent, returns_, throws_, re_throw, indented2);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
            builder.append(";\n");
            builder.append(indented2);
            builder.append("break;");
            builder.append('\n');
            builder.append(indent1);
            builder.append("    }");
        }
    };
    if (choose_.alternative != nullptr) {
        builder.append('\n');
        builder.append(indent1);
        builder.append("    default:\n");
        builder.append(indented2);
        {
            const auto _void_result = build_statement(ep, builder, *choose_.alternative, returns_, throws_, re_throw, indented);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
        builder.append(';');
    };
    builder.append('\n');
    builder.append(indent);
    builder.append("    }\n");
    builder.append(indent);
    builder.append("}");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_for(Page* ep, StringBuilder& builder, For& for_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append('\n');
    builder.append(indent);
    builder.append("auto _");
    builder.append(for_.identifier);
    builder.append("_iterator = ");
    build_operation(ep, builder, for_.expression, returns_, throws_, re_throw, indent);
    builder.append(".get_iterator();\n");
    builder.append(indent);
    builder.append("while (auto _");
    builder.append(for_.identifier);
    builder.append(" = _");
    builder.append(for_.identifier);
    builder.append("_iterator.next()) {\n");
    StringBuilder& indent_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(indent);
    indent_builder.append("    ");
    const auto indented = indent_builder.to_string(r.get_page());
    builder.append(indented);
    builder.append("auto ");
    builder.append(for_.identifier);
    builder.append(" = *_");
    builder.append(for_.identifier);
    builder.append(';');
    {
        const auto _void_result = build_action(ep, builder, for_.action, returns_, throws_, re_throw, indented);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append('\n');
    builder.append(indent);
    builder.append('}');
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_condition(Page* ep, StringBuilder& builder, Binding& binding, Type* returns_, Type* throws_, String* re_throw, String indent) {
    if (binding.item.name != nullptr) {
        if (binding.item.type != nullptr) {
            build_type(builder, binding.item.type, false);
        }
        else {
            builder.append("auto");
        };
        builder.append(' ');
        builder.append(*binding.item.name);
        builder.append(" = ");
    };
    {
        const auto _void_result = build_operation(ep, builder, binding.operation, returns_, throws_, re_throw, indent);
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

Result<Void, TranspilerError> build_while(Page* ep, StringBuilder& builder, While& while_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("while (");
    {
        const auto _void_result = build_condition(ep, builder, while_.condition, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append(") ");
    {
        const auto _void_result = build_action(ep, builder, while_.action, returns_, throws_, re_throw, indent);
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

Result<Void, TranspilerError> build_drop(Page* ep, StringBuilder& builder, Drop& drop_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append('\n');
    builder.append(indent);
    builder.append("default: {\n");
    StringBuilder& indent_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(indent);
    indent_builder.append("    ");
    const auto indented = indent_builder.to_string(r.get_page());
    builder.append(indented);
    build_action(ep, builder, drop_.action, returns_, throws_, re_throw, indented);
    builder.append(";\n");
    builder.append(indented);
    builder.append("break;\n");
    builder.append(indent);
    builder.append("}\n");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_try(Page* ep, StringBuilder& builder, Try& try_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    auto name = String(r.get_page(), "void");
    StringBuilder& indent_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(indent);
    if (try_.binding.item.name != nullptr) {
        name = *try_.binding.item.name;
    }
    else {
        builder.append("{\n");
        indent_builder.append("    ");
        indent = indent_builder.to_string(r.get_page());
        builder.append(indent);
    };
    builder.append("const auto _");
    builder.append(name);
    builder.append("_result = ");
    {
        const auto _void_result = build_operation(ep, builder, try_.binding.operation, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append(";\n");
    builder.append(indent);
    if (name.equals("void") == false) {
        builder.append("auto ");
        builder.append(name);
        builder.append(" = _");
        builder.append(name);
        builder.append("_result._Ok;\n");
        builder.append(indent);
    };
    builder.append("if (_");
    builder.append(name);
    builder.append("_result._tag == Success::Error) {\n");
    indent_builder.append("    ");
    auto indented = indent_builder.to_string(r.get_page());
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
    indent_builder.append("    ");
    const auto indented2 = indent_builder.to_string(r.get_page());
    indent_builder.append("    ");
    const auto indented3 = indent_builder.to_string(r.get_page());
    
    auto _catch__iterator = try_.catches.get_iterator();
    while (auto _catch_ = _catch__iterator.next()) {
        auto catch_ = *_catch_;{
            builder.append('\n');
            builder.append(indented);
            builder.append("    case ");
            auto first = true;
            
            auto _error_iterator = catch_.variant.get_iterator();
            while (auto _error = _error_iterator.next()) {
                auto error = *_error;{
                    if (first) {
                        first = false;
                    }
                    else {
                        builder.append("::");
                    };
                    builder.append(error);
                }
            };
            builder.append(": {\n");
            builder.append(indented3);
            builder.append("const auto ");
            builder.append(catch_.name);
            builder.append(" = _");
            builder.append(name);
            builder.append("_Error._");
            builder.append(*catch_.variant.get(1));
            builder.append(";\n");
            builder.append(indented3);
            {
                const auto _void_result = build_statement(ep, builder, catch_.consequent, returns_, throws_, nullptr, indented3);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
            builder.append(";\n");
            builder.append(indented3);
            builder.append("break;\n");
            builder.append(indented2);
            builder.append("}");
        }
    };
    if (try_.alternative != nullptr) {
        builder.append('\n');
        builder.append(indented);
        builder.append("default:\n");
        builder.append(indented2);
        {
            StringBuilder& re_throw_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder("_");
            re_throw_builder.append(name);
            re_throw_builder.append("_result._Error");
            auto re_throw = re_throw_builder.to_string(r.get_page());
            {
                const auto _void_result = build_statement(ep, builder, *try_.alternative, returns_, throws_, &re_throw, indented2);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
            builder.append(";\n");
        };
    };
    builder.append('\n');
    builder.append(indented);
    builder.append("}\n");
    builder.append(indent);
    builder.append("}");
    if (name.equals("void")) {
        builder.append("}\n");
        builder.append(indent);
    };
    return Result<Void, TranspilerError>(Void());
}

void build_sizeof(StringBuilder& builder, SizeOf& sizeof_, String indent) {
    auto r = Region();
    builder.append("sizeof(");
    build_type(builder, &sizeof_.type, true);
    builder.append(")");
}

void build_is(StringBuilder& builder, Is& is_, String indent) {
    auto r = Region();
    builder.append("._tag == ");
    auto first = true;
    
    auto _name_iterator = is_.name.get_iterator();
    while (auto _name = _name_iterator.next()) {
        auto name = *_name;{
            if (first) {
                first = false;
            }
            else {
                builder.append("::");
            };
            builder.append(name);
        }
    };
}

Result<Void, TranspilerError> build_new(Page* ep, StringBuilder& builder, New& new__, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    {
        auto _result = new__.type.lifetime;
        switch (_result._tag)
        {
            case Lifetime::Unspecified:
            {
                auto u = _result._Unspecified;
                {
                };
                break;
            }
            case Lifetime::Local:
            {
                auto l = _result._Local;
                {
                    builder.append("new (alignof(");
                    build_type(builder, &new__.type, true);
                    builder.append("), r.get_page()) ");
                };
                break;
            }
            case Lifetime::Call:
            {
                auto call = _result._Call;
                {
                    builder.append("new (alignof(");
                    build_type(builder, &new__.type, true);
                    builder.append("), rp) ");
                };
                break;
            }
            case Lifetime::Reference:
            {
                auto reference = _result._Reference;
                {
                    builder.append("new (alignof(");
                    build_type(builder, &new__.type, true);
                    builder.append("), ");
                    builder.append(reference.location);
                    builder.append(") ");
                };
                break;
            }
            case Lifetime::Thrown:
            {
                auto t = _result._Thrown;
                return Result<Void, TranspilerError>(FeatureNotImplemented(String(ep, "Thrown")));
                break;
            }
        }
    }build_type(builder, &new__.type, true);
    builder.append('(');
    auto first = true;
    
    auto _property_iterator = new__.arguments.components.get_iterator();
    while (auto _property = _property_iterator.next()) {
        auto property = *_property;{
            if (first) {
                first = false;
            }
            else {
                builder.append(", ");
            };
            build_operation(ep, builder, property.value, returns_, throws_, re_throw, indent);
        }
    };
    builder.append(')');
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_break(Page* ep, StringBuilder& builder, Break& break_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("break");
    if ((break_.result.length>0)) 
        builder.append(' ');
    {
        const auto _void_result = build_operation(ep, builder, break_.result, returns_, throws_, re_throw, indent);
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

void build_continue(StringBuilder& builder, Continue& continue_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    builder.append("continue;");
}

Result<Void, TranspilerError> build_return(Page* ep, StringBuilder& builder, Return& return_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("return");
    if ((return_.result.length>0)||(throws_ != nullptr)) 
        builder.append(' ');
    if (throws_ != nullptr) {
        builder.append("Result<");
        if (returns_ == nullptr) {
            builder.append("Void");
        }
        else {
            build_type(builder, returns_, true);
        };
        builder.append(", ");
        build_type(builder, throws_, true);
        builder.append(">(");
        if (return_.result.length == 0) 
            builder.append("Void()");
    };
    {
        const auto _void_result = build_operation(ep, builder, return_.result, returns_, throws_, re_throw, indent);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    if (throws_ != nullptr) 
        builder.append(")");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_throw(Page* ep, StringBuilder& builder, Throw& throw_, Type* returns_, Type* throws_, String* re_throw, String indent) {
    auto r = Region();
    builder.append("return Result<");
    if (returns_ == nullptr) {
        builder.append("Void");
    }
    else {
        build_type(builder, returns_, true);
    };
    builder.append(", ");
    build_type(builder, throws_, true);
    builder.append(">(");
    if (throw_.result.length == 0&&re_throw) {
        builder.append(*re_throw);
    }
    else {
        {
            const auto _void_result = build_operation(ep, builder, throw_.result, returns_, throws_, nullptr, indent);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    builder.append(")");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_statements(Page* ep, StringBuilder& builder, Vector<Statement>& statements, Type* returns_, Type* throws_, String* re_throw, String indent) {
    
    auto _statement_iterator = statements.get_iterator();
    while (auto _statement = _statement_iterator.next()) {
        auto statement = *_statement;{
            builder.append('\n');
            builder.append(indent);
            {
                const auto _void_result = build_statement(ep, builder, statement, returns_, throws_, re_throw, indent);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
            builder.append(';');
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> build_statement(Page* ep, StringBuilder& builder, Statement& statement, Type* returns_, Type* throws_, String* re_throw, String indent) {
    {
        auto _result = statement;
        switch (_result._tag)
        {
            case Statement::Action:
            {
                auto action = _result._Action;
                {
                    const auto _void_result = build_action(ep, builder, action, returns_, throws_, re_throw, indent);
                    if (_void_result._tag == Success::Error) {
                        const auto _void_Error = _void_result._Error;
                        switch (_void_Error._tag) {
                        default:
                            return Result<Void, TranspilerError>(_void_result._Error);

                        }
                    }}
                    ;
                break;
            }
            case Statement::Binding:
            {
                auto binding = _result._Binding;
                {
                    const auto _void_result = build_binding(ep, builder, binding, returns_, throws_, re_throw, indent);
                    if (_void_result._tag == Success::Error) {
                        const auto _void_Error = _void_result._Error;
                        switch (_void_Error._tag) {
                        default:
                            return Result<Void, TranspilerError>(_void_result._Error);

                        }
                    }}
                    ;
                break;
            }
            case Statement::Break:
            {
                auto break_ = _result._Break;
                {
                    const auto _void_result = build_break(ep, builder, break_, returns_, throws_, re_throw, indent);
                    if (_void_result._tag == Success::Error) {
                        const auto _void_Error = _void_result._Error;
                        switch (_void_Error._tag) {
                        default:
                            return Result<Void, TranspilerError>(_void_result._Error);

                        }
                    }}
                    ;
                break;
            }
            case Statement::Continue:
            {
                auto continue_ = _result._Continue;
                build_continue(builder, continue_, returns_, throws_, re_throw, indent);
                break;
            }
            case Statement::Return:
            {
                auto return_ = _result._Return;
                {
                    const auto _void_result = build_return(ep, builder, return_, returns_, throws_, re_throw, indent);
                    if (_void_result._tag == Success::Error) {
                        const auto _void_Error = _void_result._Error;
                        switch (_void_Error._tag) {
                        default:
                            return Result<Void, TranspilerError>(_void_result._Error);

                        }
                    }}
                    ;
                break;
            }
            case Statement::Throw:
            {
                auto throw_ = _result._Throw;
                {
                    const auto _void_result = build_throw(ep, builder, throw_, returns_, throws_, re_throw, indent);
                    if (_void_result._tag == Success::Error) {
                        const auto _void_Error = _void_result._Error;
                        switch (_void_Error._tag) {
                        default:
                            return Result<Void, TranspilerError>(_void_result._Error);

                        }
                    }}
                    ;
                break;
            }
        }
    };
    return Result<Void, TranspilerError>(Void());
}

void function_prefix(StringBuilder& builder, Function& func, bool indent, bool static_if_applicable) {
    auto r = Region();
    builder.append('\n');
    if (indent) 
        builder.append("    ");
    if (static_if_applicable&&((func.input.length==0)||((*func.input.get(0)).name==nullptr)||((*(*func.input.get(0)).name).equals("this")) == false)) 
        builder.append("static ");
    build_output_type(builder, func.returns_, func.throws_);
    builder.append(' ');
}

Result<Void, TranspilerError> build_operator(Page* ep, StringBuilder& header_builder, StringBuilder& cpp_builder, Operator& operator_, String* name, bool is_template) {
    auto r = Region();
    if (is_template) 
        header_builder.append('\n');
    header_builder.append("\n    ");
    build_output_type(header_builder, operator_.returns_, operator_.throws_);
    header_builder.append(" operator ");
    header_builder.append(operator_.name);
    build_input(header_builder, operator_.input, nullptr, operator_.returns_, operator_.throws_, Lifetime(Unspecified()));
    if (is_template) {
        {
            const auto _void_result = build_implementation(ep, header_builder, operator_.implementation, operator_.returns_, operator_.throws_, String(r.get_page(), "    "), false);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    }
    else {
        header_builder.append(';');
        cpp_builder.append("\n\n");
        build_output_type(cpp_builder, operator_.returns_, operator_.throws_);
        cpp_builder.append(' ');
        cpp_builder.append(*name);
        cpp_builder.append("::operator ");
        cpp_builder.append(operator_.name);
        const auto is_static = build_input(cpp_builder, operator_.input, nullptr, operator_.returns_, operator_.throws_, Lifetime(Unspecified()));
        cpp_builder.append(' ');
        {
            const auto _void_result = build_implementation(ep, cpp_builder, operator_.implementation, operator_.returns_, operator_.throws_, String(), false);
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Void, TranspilerError>(Void());
}

bool needs_return_page(Type* type) {
    if (type == nullptr) 
        return false;
    if ((*type).name.length == 1) {
        auto name = *(*type).name.get(0);
        if (name.equals("bool")||name.equals("int")||name.equals("size_t")) 
            return false;
    };
    if ((*type).lifetime._tag == Lifetime::Local) 
        return false;
    return true;
}

bool build_input(StringBuilder& builder, Vector<Item> input, String* location, Type* returns_, Type* throws_, Lifetime lifetime) {
    auto r = Region();
    builder.append('(');
    auto parameters_there = false;
    if (input.length>0) {
        if ((*(*input.get(0)).name).equals("this")) {
            parameters_there = input.length>1;
        }
        else {
            parameters_there = true;
        };
    };
    const auto needs_rp = needs_return_page(returns_);
    if ((location != nullptr)) {
        builder.append("Page* ");
        builder.append(*location);
        if (throws_ != nullptr||parameters_there||needs_rp) 
            builder.append(", ");
    };
    if (needs_rp) {
        builder.append("Page* rp");
        if ((throws_ != nullptr||parameters_there)) 
            builder.append(", ");
    };
    if (throws_ != nullptr) {
        builder.append("Page* ep");
        if (parameters_there) 
            builder.append(", ");
    };
    auto first = true;
    auto is_static = true;
    
    auto _property_iterator = input.get_iterator();
    while (auto _property = _property_iterator.next()) {
        auto property = *_property;{
            if (first) {
                if (property.name&&(*property.name).equals(String(r.get_page(), "this"))) {
                    is_static = false;
                    continue;;
                };
                first = false;
            }
            else {
                builder.append(", ");
            };
            if ((property.type != nullptr)) {
                build_type(builder, property.type, true);
                builder.append(' ');
            };
            if (property.name != nullptr) 
                builder.append(*property.name);
        }
    };
    builder.append(')');
    return is_static;
}

void full_struct_name(StringBuilder& builder, String name, Vector<GenericParameter> parameters) {
    auto r = Region();
    if (parameters.length>0) {
        builder.append("template<");
        {
            size_t i = 0;
            
            auto _generic_iterator = parameters.get_iterator();
            while (auto _generic = _generic_iterator.next()) {
                auto generic = *_generic;{
                    builder.append("class ");
                    builder.append(generic.name);
                    if ((i<parameters.length-1)) 
                        builder.append(", ");
                    i = i+1;
                }
            };
        };
        builder.append(">\n");
    };
    builder.append("struct ");
    builder.append(name);
}

Result<Void, TranspilerError> forward_includes(Page* ep, String path, Program& program) {
    auto r = Region();
    StringBuilder& builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    builder.append("typedef const char const_char;\n\
typedef struct stat struct_stat;\n\
typedef const void const_void;\n");
    {
        const auto _void_result = forward_includes_for_modules(ep, builder, program.module_.modules);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    {
        const auto _void_result = forward_includes_for_symbols(ep, builder, program.module_.members);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    if (builder.get_length()>0) {
        const auto main_file_name = Path::join(r.get_page(), path, String(r.get_page(), "forwards.h"));
        {
            const auto _void_result = File::write_from_string(ep, main_file_name, builder.to_string(r.get_page()));
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> forward_include(Page* ep, StringBuilder& builder, Concept& concept) {
    auto r = Region();
    {
        auto _result = concept.definition;
        switch (_result._tag)
        {
            case Definition::Namespace:
            {
                auto namespace_ = _result._Namespace;
                {
                    const auto _void_result = forward_includes_for_namespace(ep, concept.name, builder, namespace_);
                    if (_void_result._tag == Success::Error) {
                        const auto _void_Error = _void_result._Error;
                        switch (_void_Error._tag) {
                        default:
                            return Result<Void, TranspilerError>(_void_result._Error);

                        }
                    }}
                    ;
                break;
            }
            case Definition::Structure:
            {
                auto structure = _result._Structure;
                {
                    full_struct_name(builder, concept.name, concept.parameters);
                    builder.append(";\n");
                    {
                        const auto _void_result = forward_includes_for_modules(ep, builder, structure.modules);
                        if (_void_result._tag == Success::Error) {
                            const auto _void_Error = _void_result._Error;
                            switch (_void_Error._tag) {
                            default:
                                return Result<Void, TranspilerError>(_void_result._Error);

                            }
                        }}
                        ;
                    return Result<Void, TranspilerError>(Void());
                };
                break;
            }
            case Definition::Union:
            {
                auto u = _result._Union;
                {
                    builder.append("struct ");
                    builder.append(concept.name);
                    builder.append(";\n");
                    return Result<Void, TranspilerError>(Void());
                };
                break;
            }
            default:
                return Result<Void, TranspilerError>(Void());
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> forward_includes_for_namespace(Page* ep, String name, StringBuilder& builder, Namespace& namespace_) {
    auto r = Region();
    builder.append("namespace ");
    builder.append(name);
    builder.append(" {\n");
    {
        const auto _void_result = forward_includes_for_modules(ep, builder, namespace_.modules);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    {
        const auto _void_result = forward_includes_for_symbols(ep, builder, namespace_.members);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    builder.append("}\n");
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> forward_includes_for_modules(Page* ep, StringBuilder& builder, Vector<Module>& modules) {
    auto r = Region();
    
    auto _module__iterator = modules.get_iterator();
    while (auto _module_ = _module__iterator.next()) {
        auto module_ = *_module_;{
            {
                const auto _void_result = forward_includes_for_symbols(ep, builder, module_.members);
                if (_void_result._tag == Success::Error) {
                    const auto _void_Error = _void_result._Error;
                    switch (_void_Error._tag) {
                    default:
                        return Result<Void, TranspilerError>(_void_result._Error);

                    }
                }}
                ;
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> forward_includes_for_symbols(Page* ep, StringBuilder& builder, Vector<Member>& members) {
    auto r = Region();
    
    auto _member_iterator = members.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Concept:
                    {
                        auto concept = _result._Concept;
                        {
                            const auto _void_result = forward_include(ep, builder, concept);
                            if (_void_result._tag == Success::Error) {
                                const auto _void_Error = _void_result._Error;
                                switch (_void_Error._tag) {
                                default:
                                    return Result<Void, TranspilerError>(_void_result._Error);

                                }
                            }}
                            ;
                        break;
                    }
                    default:
                        continue;;
                }
            };
        }
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> vscode_files(Page* ep, String path, Program& program) {
    auto r = Region();
    const auto vscode_dir = Path::join(r.get_page(), path, String(r.get_page(), ".vscode"));
    {
        const auto _void_result = create_directory(ep, vscode_dir);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    StringBuilder& script_file_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(Path::join(r.get_page(), path, String(r.get_page(), "build.sh")));
    StringBuilder& script_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder("#!/usr/bin/env bash\n\
set -x\n\
set -e\n\
scalyc ");
    script_builder.append(program.module_.name);
    script_builder.append(".scaly ");
    script_builder.append(program.module_.name);
    script_builder.append("\n\
clang++ -fcolor-diagnostics -fansi-escape-codes -ferror-limit=5 \\\n\
    -g \\\n\
");
    build_script_files(script_builder, String(), program.module_, String(r.get_page(), ".cpp"), true);
    
    auto _package__iterator = program.packages.get_iterator();
    while (auto _package_ = _package__iterator.next()) {
        auto package_ = *_package_;{
            script_builder.append("    ../");
            script_builder.append(package_.name);
            script_builder.append('/');
            script_builder.append(package_.name);
            script_builder.append(".a \\\n\
");
        }
    };
    if (program.statements.length>0) {
        script_builder.append("    main.cpp \\\n\
");
        script_builder.append("    -o bin/");
        script_builder.append(program.module_.name);
        script_builder.append("\\\n\
    `llvm-config --cxxflags --ldflags --system-libs --libs core`\n\
");
    }
    else {
        script_builder.append("    -c\\\n\
    `llvm-config --cxxflags`\n\
\n\
/opt/homebrew/opt/llvm/bin/llvm-ar \\\n\
    rc ");
        script_builder.append(program.module_.name);
        script_builder.append(".a \\\n\
");
        build_script_files(script_builder, String(), program.module_, String(r.get_page(), ".o"), false);
        script_builder.append("\n\
rm \\\n\
");
        build_script_files(script_builder, String(), program.module_, String(r.get_page(), ".o"), false);
    };
    auto script_file = script_file_builder.to_string(r.get_page());
    {
        const auto _void_result = File::write_from_string(ep, script_file, script_builder.to_string(r.get_page()));
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                return Result<Void, TranspilerError>(_void_result._Error);

            }
        }}
        ;
    auto mask_string = String(r.get_page(), "0755");
    auto i = strtol(mask_string.get_buffer(), 0, 8);
    chmod(script_file.to_c_string(r.get_page()), i);
    StringBuilder& tasks_file_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(Path::join(r.get_page(), vscode_dir, String(r.get_page(), "tasks.json")));
    {
        const auto _void_result = File::write_from_string(ep, tasks_file_builder.to_string(r.get_page()), String(r.get_page(), "{\n\
	\"version\": \"2.0.0\",\n\
	\"tasks\": [\n\
		{\n\
			\"label\": \"Build Scaly package\",\n\
			\"type\": \"shell\",\n\
			\"command\": \"./build.sh\",\n\
			\"group\": {\n\
				\"kind\": \"build\",\n\
				\"isDefault\": true\n\
			},\n\
			\"presentation\": {\n\
				\"reveal\": \"always\",\n\
				\"panel\": \"shared\"\n\
			},\n\
			\"problemMatcher\": []\n\
		}\n\
	]\n\
}"));
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

void build_script_files(StringBuilder& builder, String path, Module& module_, String extension, bool include_path) {
    auto r = Region();
    builder.append("    ");
    if (include_path) 
        builder.append(path);
    builder.append(module_.name);
    builder.append(extension);
    builder.append(" \\\n");
    if (include_path) {
        StringBuilder& path_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(path);
        path_builder.append(module_.name);
        path_builder.append('/');
        build_script_files_list(builder, path_builder.to_string(r.get_page()), module_.modules, module_.members, extension, include_path);
    }
    else {
        build_script_files_list(builder, path, module_.modules, module_.members, extension, include_path);
    };
}

void build_script_files_list(StringBuilder& builder, String path, Vector<Module>& modules, Vector<Member>& members, String extension, bool include_path) {
    auto r = Region();
    
    auto _module__iterator = modules.get_iterator();
    while (auto _module_ = _module__iterator.next()) {
        auto module_ = *_module_;{
            build_script_files(builder, path, module_, extension, include_path);
        }
    };
    
    auto _member_iterator = members.get_iterator();
    while (auto _member = _member_iterator.next()) {
        auto member = *_member;{
            {
                auto _result = member;
                switch (_result._tag)
                {
                    case Member::Concept:
                    {
                        auto concept = _result._Concept;
                        {
                            {
                                auto _result = concept.definition;
                                switch (_result._tag)
                                {
                                    case Definition::Namespace:
                                    {
                                        auto namespace_ = _result._Namespace;
                                        build_script_files_list(builder, path, namespace_.modules, namespace_.members, extension, include_path);
                                        break;
                                    }
                                    case Definition::Structure:
                                    {
                                        auto structure = _result._Structure;
                                        build_script_files_list(builder, path, structure.modules, structure.members, extension, include_path);
                                        break;
                                    }
                                    default:
                                        {
                                    };
                                }
                            };
                        };
                        break;
                    }
                    default:
                        continue;;
                }
            };
        }
    };
}

Result<Void, TranspilerError> main_file(Page* ep, String path, Program& program) {
    auto r = Region();
    StringBuilder& builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder("#include \"main.h\"\n\
\n\
int main(int argc, char** argv) {");
    
    auto _statement_iterator = program.statements.get_iterator();
    while (auto _statement = _statement_iterator.next()) {
        auto statement = *_statement;{
            builder.append("\n    ");
            build_statement(ep, builder, statement, nullptr, nullptr, nullptr, String());
            builder.append(';');
        }
    };
    builder.append("\n    return 0;\n}\n");
    if (builder.get_length()>0) {
        const auto main_file_name = Path::join(r.get_page(), path, String(r.get_page(), "main.cpp"));
        {
            const auto _void_result = File::write_from_string(ep, main_file_name, builder.to_string(r.get_page()));
            if (_void_result._tag == Success::Error) {
                const auto _void_Error = _void_result._Error;
                switch (_void_Error._tag) {
                default:
                    return Result<Void, TranspilerError>(_void_result._Error);

                }
            }}
            ;
    };
    return Result<Void, TranspilerError>(Void());
}

Result<Void, TranspilerError> main_include_file(Page* ep, String path, String name) {
    auto r = Region();
    StringBuilder& main_include_file_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(Path::join(r.get_page(), path, String(r.get_page(), "main")));
    main_include_file_builder.append(".h");
    StringBuilder& main_include_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder("#include \"");
    main_include_builder.append(name);
    main_include_builder.append(".h\"\n");
    {
        const auto _void_result = File::write_from_string(ep, main_include_file_builder.to_string(r.get_page()), main_include_builder.to_string(r.get_page()));
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
}