using System;
using System.Collections.Generic;
using System.Text;

namespace Scaly.Compiler
{
    class Parser
    {
        Lexer lexer;
        string file_name = "";
        HashSet<string> keywords = new HashSet<string>(new string[] {
            "break",
            "catch",
            "case",
            "continue",
            "define",
            "default",
            "delegate",
            "drop",
            "else",
            "extends",
            "extern",
            "for",
            "function",
            "if",
            "is",
            "implement",
            "in",
            "instruction",
            "intrinsic",
            "label",
            "lambda",
            "let",
            "loop",
            "macro",
            "match",
            "module",
            "mutable",
            "operator",
            "procedure",
            "private",
            "return",
            "returns",
            "repeat",
            "set",
            "sizeof",
            "throw",
            "throws",
            "trait",
            "union",
            "use",
            "var",
            "while",
        });

        public Parser(string deck)
        {
            lexer = new Lexer(deck);
        }

        public FileSyntax parse_file(string file_name)
        {
            this.file_name = file_name;
            var declarations = parse_declaration_list();
            var statements = parse_statement_list();
            if (statements != null)
            {
                if (!is_at_end())
                {
                    throw new ParserException("Unable to parse statement list.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
                }
            }

            var ret = new FileSyntax
            {
                span = new Span
                {
                    file = file_name                },
                declarations = declarations,
                statements = statements,
            };

            return ret;
        }

        public object[] parse_declaration_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_declaration();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_declaration()
        {
            {
                var node = parse_private();
                if (node != null)
                    return node;
            }
            {
                var node = parse_definition();
                if (node != null)
                    return node;
            }
            {
                var node = parse_function();
                if (node != null)
                    return node;
            }
            {
                var node = parse_procedure();
                if (node != null)
                    return node;
            }
            {
                var node = parse_operator();
                if (node != null)
                    return node;
            }
            {
                var node = parse_use();
                if (node != null)
                    return node;
            }
            {
                var node = parse_implement();
                if (node != null)
                    return node;
            }
            {
                var node = parse_trait();
                if (node != null)
                    return node;
            }
            {
                var node = parse_macro();
                if (node != null)
                    return node;
            }
            {
                var node = parse_module();
                if (node != null)
                    return node;
            }

            return null;
        }

        public PrivateSyntax parse_private()
        {
            var start = lexer.get_previous_position();

            var success_private_1 = lexer.parse_keyword("private");
            if (!success_private_1)
                    return null;
            var export = parse_export();
            if (export == null)
                throw new ParserException("Unable to parse export.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new PrivateSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                export = export,
            };

            return ret;
        }

        public object parse_export()
        {
            {
                var node = parse_definition();
                if (node != null)
                    return node;
            }
            {
                var node = parse_function();
                if (node != null)
                    return node;
            }
            {
                var node = parse_procedure();
                if (node != null)
                    return node;
            }
            {
                var node = parse_operator();
                if (node != null)
                    return node;
            }
            {
                var node = parse_implement();
                if (node != null)
                    return node;
            }
            {
                var node = parse_trait();
                if (node != null)
                    return node;
            }
            {
                var node = parse_macro();
                if (node != null)
                    return node;
            }
            {
                var node = parse_module();
                if (node != null)
                    return node;
            }

            return null;
        }

        public DefinitionSyntax parse_definition()
        {
            var start = lexer.get_previous_position();

            var success_define_1 = lexer.parse_keyword("define");
            if (!success_define_1)
                    return null;
            var type = parse_type();
            if (type == null)
                throw new ParserException("Unable to parse type.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            lexer.parse_colon();
            var attributes = parse_attribute_list();
            var concept = parse_concept();
            if (concept == null)
                throw new ParserException("Unable to parse concept.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new DefinitionSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                type = type,
                attributes = attributes,
                concept = concept,
            };

            return ret;
        }

        public NameSyntax parse_name()
        {
            var start = lexer.get_previous_position();

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    return null;
                }
            }
            else
            {
                    return null;
            }
            var extensions = parse_extension_list();

            var end = lexer.get_position();

            var ret = new NameSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                extensions = extensions,
            };

            return ret;
        }

        public ExtensionSyntax[] parse_extension_list()
        {
            List<ExtensionSyntax> list = null;
            while (true)
            {
                var node = parse_extension();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<ExtensionSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public ExtensionSyntax parse_extension()
        {
            var start = lexer.get_previous_position();

            var success_dot_1 = lexer.parse_punctuation(".");
            if (!success_dot_1)
                    return null;

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
                }
            }
            else
            {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            }

            var end = lexer.get_position();

            var ret = new ExtensionSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
            };

            return ret;
        }

        public AttributeSyntax[] parse_attribute_list()
        {
            List<AttributeSyntax> list = null;
            while (true)
            {
                var node = parse_attribute();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<AttributeSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public AttributeSyntax parse_attribute()
        {
            var start = lexer.get_previous_position();

            var attribute = lexer.parse_attribute();
            if (attribute == null)
            {
                    return null;
            }
            var value = parse_model();
            if (value == null)
                throw new ParserException("Unable to parse model.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new AttributeSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                attribute = attribute,
                value = value,
            };

            return ret;
        }

        public object parse_concept()
        {
            {
                var node = parse_class();
                if (node != null)
                    return node;
            }
            {
                var node = parse_namespace();
                if (node != null)
                    return node;
            }
            {
                var node = parse_union();
                if (node != null)
                    return node;
            }
            {
                var node = parse_constant();
                if (node != null)
                    return node;
            }
            {
                var node = parse_delegate();
                if (node != null)
                    return node;
            }
            {
                var node = parse_intrinsic();
                if (node != null)
                    return node;
            }

            return null;
        }

        public ClassSyntax parse_class()
        {
            var start = lexer.get_previous_position();
            var structure = parse_structure();
            if (structure == null)
                return null;

            lexer.parse_colon();
            var body = parse_body();

            var end = lexer.get_position();

            var ret = new ClassSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                structure = structure,
                body = body,
            };

            return ret;
        }

        public NamespaceSyntax parse_namespace()
        {
            var start = lexer.get_previous_position();
            var body = parse_body();
            if (body == null)
                return null;

            var end = lexer.get_position();

            var ret = new NamespaceSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                body = body,
            };

            return ret;
        }

        public UnionSyntax parse_union()
        {
            var start = lexer.get_previous_position();

            var success_union_1 = lexer.parse_keyword("union");
            if (!success_union_1)
                    return null;

            lexer.parse_colon();

            var success_left_paren_3 = lexer.parse_punctuation("(");
            if (!success_left_paren_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            var tags = parse_tag_list();
            if (tags == null)
                throw new ParserException("Unable to parse tag list.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var success_right_paren_5 = lexer.parse_punctuation(")");
            if (!success_right_paren_5)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            lexer.parse_colon();
            var body = parse_body();
            if (body == null)
                throw new ParserException("Unable to parse body.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new UnionSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                tags = tags,
                body = body,
            };

            return ret;
        }

        public TagSyntax[] parse_tag_list()
        {
            List<TagSyntax> list = null;
            while (true)
            {
                var node = parse_tag();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<TagSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public TagSyntax parse_tag()
        {
            var start = lexer.get_previous_position();

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    return null;
                }
            }
            else
            {
                    return null;
            }
            var attributes = parse_attribute_list();
            var item = parse_item();

            var end = lexer.get_position();

            var ret = new TagSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                attributes = attributes,
                item = item,
            };

            return ret;
        }

        public object parse_item()
        {
            {
                var node = parse_variant();
                if (node != null)
                    return node;
            }
            {
                var node = parse_enum();
                if (node != null)
                    return node;
            }

            return null;
        }

        public VariantSyntax parse_variant()
        {
            var start = lexer.get_previous_position();

            lexer.parse_colon();
            var structure = parse_structure();
            if (structure == null)
                throw new ParserException("Unable to parse structure.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            lexer.parse_colon();
            var body = parse_body();

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new VariantSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                structure = structure,
                body = body,
            };

            return ret;
        }

        public EnumSyntax parse_enum()
        {
            var start = lexer.get_previous_position();

            var literal = lexer.parse_literal();
            if (literal == null)
            {
                    return null;
            }

            var end = lexer.get_position();

            var ret = new EnumSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                literal = literal,
            };

            return ret;
        }

        public ConstantSyntax parse_constant()
        {
            var start = lexer.get_previous_position();
            var operation = parse_operation();
            if (operation == null)
                return null;

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new ConstantSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                operation = operation,
            };

            return ret;
        }

        public DelegateSyntax parse_delegate()
        {
            var start = lexer.get_previous_position();

            var success_delegate_1 = lexer.parse_keyword("delegate");
            if (!success_delegate_1)
                    return null;
            var parameters = parse_parameterset();
            var attributes = parse_attribute_list();
            var result = parse_returns();
            var error = parse_throws();

            var end = lexer.get_position();

            var ret = new DelegateSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                parameters = parameters,
                attributes = attributes,
                result = result,
                error = error,
            };

            return ret;
        }

        public StructureSyntax parse_structure()
        {
            var start = lexer.get_previous_position();

            var success_left_paren_1 = lexer.parse_punctuation("(");
            if (!success_left_paren_1)
                    return null;
            var members = parse_member_list();

            var success_right_paren_3 = lexer.parse_punctuation(")");
            if (!success_right_paren_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new StructureSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                members = members,
            };

            return ret;
        }

        public BodySyntax parse_body()
        {
            var start = lexer.get_previous_position();

            var success_left_curly_1 = lexer.parse_punctuation("{");
            if (!success_left_curly_1)
                    return null;
            var declarations = parse_declaration_list();

            var success_right_curly_3 = lexer.parse_punctuation("}");
            if (!success_right_curly_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new BodySyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                declarations = declarations,
            };

            return ret;
        }

        public object[] parse_member_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_member();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_member()
        {
            {
                var node = parse_field();
                if (node != null)
                    return node;
            }
            {
                var node = parse_property();
                if (node != null)
                    return node;
            }

            return null;
        }

        public FieldSyntax parse_field()
        {
            var start = lexer.get_previous_position();

            var success_private_1 = lexer.parse_keyword("private");
            if (!success_private_1)
                    return null;
            var export = parse_property();
            if (export == null)
                throw new ParserException("Unable to parse property.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new FieldSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                export = export,
            };

            return ret;
        }

        public PropertySyntax[] parse_property_list()
        {
            List<PropertySyntax> list = null;
            while (true)
            {
                var node = parse_property();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<PropertySyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public PropertySyntax parse_property()
        {
            var start = lexer.get_previous_position();

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    return null;
                }
            }
            else
            {
                    return null;
            }
            var annotation = parse_typeannotation();
            var attributes = parse_attribute_list();

            lexer.parse_punctuation(",");

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new PropertySyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                annotation = annotation,
                attributes = attributes,
            };

            return ret;
        }

        public TypeAnnotationSyntax parse_typeannotation()
        {
            var start = lexer.get_previous_position();

            var success_colon_1 = lexer.parse_colon();
            if (!success_colon_1)
                    return null;
            var spec = parse_typespec();
            if (spec == null)
                throw new ParserException("Unable to parse typespec.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new TypeAnnotationSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                spec = spec,
            };

            return ret;
        }

        public object[] parse_typespec_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_typespec();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_typespec()
        {
            {
                var node = parse_structure();
                if (node != null)
                    return node;
            }
            {
                var node = parse_type();
                if (node != null)
                    return node;
            }

            return null;
        }

        public TypeSyntax parse_type()
        {
            var start = lexer.get_previous_position();
            var name = parse_name();
            if (name == null)
                return null;
            var generics = parse_genericarguments();
            var optional = parse_optional();

            var end = lexer.get_position();

            var ret = new TypeSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                generics = generics,
                optional = optional,
            };

            return ret;
        }

        public ArraySyntax parse_array()
        {
            var start = lexer.get_previous_position();

            var success_left_bracket_1 = lexer.parse_punctuation("[");
            if (!success_left_bracket_1)
                    return null;
            var members = parse_typespec_list();

            var success_right_bracket_3 = lexer.parse_punctuation("]");
            if (!success_right_bracket_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new ArraySyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                members = members,
            };

            return ret;
        }

        public GenericArgumentsSyntax parse_genericarguments()
        {
            var start = lexer.get_previous_position();

            var success_left_bracket_1 = lexer.parse_punctuation("[");
            if (!success_left_bracket_1)
                    return null;
            var generics = parse_genericargument_list();

            var success_right_bracket_3 = lexer.parse_punctuation("]");
            if (!success_right_bracket_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new GenericArgumentsSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                generics = generics,
            };

            return ret;
        }

        public GenericArgumentSyntax[] parse_genericargument_list()
        {
            List<GenericArgumentSyntax> list = null;
            while (true)
            {
                var node = parse_genericargument();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<GenericArgumentSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public GenericArgumentSyntax parse_genericargument()
        {
            var start = lexer.get_previous_position();
            var spec = parse_type();
            if (spec == null)
                return null;

            lexer.parse_punctuation(",");

            var end = lexer.get_position();

            var ret = new GenericArgumentSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                spec = spec,
            };

            return ret;
        }

        public OptionalSyntax parse_optional()
        {
            var start = lexer.get_previous_position();

            var success_question_1 = lexer.parse_punctuation("?");
            if (!success_question_1)
                    return null;

            var end = lexer.get_position();

            var ret = new OptionalSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
            };

            return ret;
        }

        public FunctionSyntax parse_function()
        {
            var start = lexer.get_previous_position();

            var success_function_1 = lexer.parse_keyword("function");
            if (!success_function_1)
                    return null;

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
                }
            }
            else
            {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            }
            var generics = parse_genericarguments();
            var routine = parse_routine();
            if (routine == null)
                throw new ParserException("Unable to parse routine.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new FunctionSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                generics = generics,
                routine = routine,
            };

            return ret;
        }

        public ProcedureSyntax parse_procedure()
        {
            var start = lexer.get_previous_position();

            var success_procedure_1 = lexer.parse_keyword("procedure");
            if (!success_procedure_1)
                    return null;

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
                }
            }
            else
            {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            }
            var generics = parse_genericarguments();
            var routine = parse_routine();
            if (routine == null)
                throw new ParserException("Unable to parse routine.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new ProcedureSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                generics = generics,
                routine = routine,
            };

            return ret;
        }

        public OperatorSyntax parse_operator()
        {
            var start = lexer.get_previous_position();

            var success_operator_1 = lexer.parse_keyword("operator");
            if (!success_operator_1)
                    return null;
            var target = parse_target();
            if (target == null)
                throw new ParserException("Unable to parse target.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new OperatorSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                target = target,
            };

            return ret;
        }

        public object parse_target()
        {
            {
                var node = parse_symbol();
                if (node != null)
                    return node;
            }
            {
                var node = parse_routine();
                if (node != null)
                    return node;
            }

            return null;
        }

        public RoutineSyntax parse_routine()
        {
            var start = lexer.get_previous_position();
            var parameters = parse_parameterset();
            var attributes = parse_attribute_list();
            var returns = parse_returns();

            lexer.parse_colon();
            var throws = parse_throws();

            lexer.parse_colon();
            var implementation = parse_implementation();
            if (implementation == null)
                throw new ParserException("Unable to parse implementation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new RoutineSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                parameters = parameters,
                attributes = attributes,
                returns = returns,
                throws = throws,
                implementation = implementation,
            };

            return ret;
        }

        public SymbolSyntax parse_symbol()
        {
            var start = lexer.get_previous_position();

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    return null;
                }
            }
            else
            {
                    return null;
            }
            var attributes = parse_attribute_list();
            var returns = parse_returns();

            lexer.parse_colon();
            var throws = parse_throws();

            lexer.parse_colon();
            var implementation = parse_implementation();
            if (implementation == null)
                throw new ParserException("Unable to parse implementation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new SymbolSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                attributes = attributes,
                returns = returns,
                throws = throws,
                implementation = implementation,
            };

            return ret;
        }

        public object parse_parameterset()
        {
            {
                var node = parse_parameters();
                if (node != null)
                    return node;
            }
            {
                var node = parse_type();
                if (node != null)
                    return node;
            }

            return null;
        }

        public ParametersSyntax parse_parameters()
        {
            var start = lexer.get_previous_position();

            var success_left_paren_1 = lexer.parse_punctuation("(");
            if (!success_left_paren_1)
                    return null;
            var properties = parse_property_list();

            var success_right_paren_3 = lexer.parse_punctuation(")");
            if (!success_right_paren_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new ParametersSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                properties = properties,
            };

            return ret;
        }

        public ReturnsSyntax parse_returns()
        {
            var start = lexer.get_previous_position();

            var success_returns_1 = lexer.parse_keyword("returns");
            if (!success_returns_1)
                    return null;
            var parameters = parse_parameterset();
            if (parameters == null)
                throw new ParserException("Unable to parse parameterset.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var attributes = parse_attribute_list();

            var end = lexer.get_position();

            var ret = new ReturnsSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                parameters = parameters,
                attributes = attributes,
            };

            return ret;
        }

        public ThrowsSyntax parse_throws()
        {
            var start = lexer.get_previous_position();

            var success_throws_1 = lexer.parse_keyword("throws");
            if (!success_throws_1)
                    return null;
            var type = parse_type();
            if (type == null)
                throw new ParserException("Unable to parse type.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var attributes = parse_attribute_list();

            var end = lexer.get_position();

            var ret = new ThrowsSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                type = type,
                attributes = attributes,
            };

            return ret;
        }

        public object parse_implementation()
        {
            {
                var node = parse_statement();
                if (node != null)
                    return node;
            }
            {
                var node = parse_extern();
                if (node != null)
                    return node;
            }
            {
                var node = parse_instruction();
                if (node != null)
                    return node;
            }
            {
                var node = parse_intrinsic();
                if (node != null)
                    return node;
            }

            return null;
        }

        public ExternSyntax parse_extern()
        {
            var start = lexer.get_previous_position();

            var success_extern_1 = lexer.parse_keyword("extern");
            if (!success_extern_1)
                    return null;

            var end = lexer.get_position();

            var ret = new ExternSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
            };

            return ret;
        }

        public InstructionSyntax parse_instruction()
        {
            var start = lexer.get_previous_position();

            var success_instruction_1 = lexer.parse_keyword("instruction");
            if (!success_instruction_1)
                    return null;

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new InstructionSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
            };

            return ret;
        }

        public IntrinsicSyntax parse_intrinsic()
        {
            var start = lexer.get_previous_position();

            var success_intrinsic_1 = lexer.parse_keyword("intrinsic");
            if (!success_intrinsic_1)
                    return null;

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new IntrinsicSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
            };

            return ret;
        }

        public UseSyntax parse_use()
        {
            var start = lexer.get_previous_position();

            var success_use_1 = lexer.parse_keyword("use");
            if (!success_use_1)
                    return null;
            var name = parse_name();
            if (name == null)
                throw new ParserException("Unable to parse name.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new UseSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
            };

            return ret;
        }

        public ImplementSyntax parse_implement()
        {
            var start = lexer.get_previous_position();

            var success_implement_1 = lexer.parse_keyword("implement");
            if (!success_implement_1)
                    return null;
            var type = parse_type();
            if (type == null)
                throw new ParserException("Unable to parse type.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var attributes = parse_attribute_list();

            lexer.parse_colon();

            var success_left_curly_5 = lexer.parse_punctuation("{");
            if (!success_left_curly_5)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            var methods = parse_method_list();

            var success_right_curly_7 = lexer.parse_punctuation("}");
            if (!success_right_curly_7)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new ImplementSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                type = type,
                attributes = attributes,
                methods = methods,
            };

            return ret;
        }

        public TraitSyntax parse_trait()
        {
            var start = lexer.get_previous_position();

            var success_trait_1 = lexer.parse_keyword("trait");
            if (!success_trait_1)
                    return null;
            var name = parse_name();
            if (name == null)
                throw new ParserException("Unable to parse name.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var extension = parse_extends();
            if (extension == null)
                throw new ParserException("Unable to parse extends.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var attributes = parse_attribute_list();

            var success_left_curly_5 = lexer.parse_punctuation("{");
            if (!success_left_curly_5)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            var functions = parse_method_list();

            var success_right_curly_7 = lexer.parse_punctuation("}");
            if (!success_right_curly_7)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new TraitSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                extension = extension,
                attributes = attributes,
                functions = functions,
            };

            return ret;
        }

        public object[] parse_method_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_method();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_method()
        {
            {
                var node = parse_function();
                if (node != null)
                    return node;
            }
            {
                var node = parse_procedure();
                if (node != null)
                    return node;
            }
            {
                var node = parse_operator();
                if (node != null)
                    return node;
            }

            return null;
        }

        public ExtendsSyntax parse_extends()
        {
            var start = lexer.get_previous_position();

            var success_extends_1 = lexer.parse_keyword("extends");
            if (!success_extends_1)
                    return null;
            var extensions = parse_extend_list();

            var end = lexer.get_position();

            var ret = new ExtendsSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                extensions = extensions,
            };

            return ret;
        }

        public ExtendSyntax[] parse_extend_list()
        {
            List<ExtendSyntax> list = null;
            while (true)
            {
                var node = parse_extend();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<ExtendSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public ExtendSyntax parse_extend()
        {
            var start = lexer.get_previous_position();
            var spec = parse_typespec();
            if (spec == null)
                return null;

            lexer.parse_punctuation(",");

            var end = lexer.get_position();

            var ret = new ExtendSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                spec = spec,
            };

            return ret;
        }

        public MacroSyntax parse_macro()
        {
            var start = lexer.get_previous_position();

            var success_macro_1 = lexer.parse_keyword("macro");
            if (!success_macro_1)
                    return null;

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
                }
            }
            else
            {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            }
            var model = parse_model();
            if (model == null)
                throw new ParserException("Unable to parse model.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var rule = parse_operation();
            if (rule == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new MacroSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                model = model,
                rule = rule,
            };

            return ret;
        }

        public ModuleSyntax parse_module()
        {
            var start = lexer.get_previous_position();

            var success_module_1 = lexer.parse_keyword("module");
            if (!success_module_1)
                    return null;
            var name = parse_name();
            if (name == null)
                throw new ParserException("Unable to parse name.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new ModuleSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
            };

            return ret;
        }

        public object parse_model()
        {
            {
                var node = parse_literal();
                if (node != null)
                    return node;
            }
            {
                var node = parse_name();
                if (node != null)
                    return node;
            }
            {
                var node = parse_object();
                if (node != null)
                    return node;
            }
            {
                var node = parse_vector();
                if (node != null)
                    return node;
            }

            return null;
        }

        public object[] parse_statement_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_statement();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_statement()
        {
            {
                var node = parse_operation();
                if (node != null)
                    return node;
            }
            {
                var node = parse_let();
                if (node != null)
                    return node;
            }
            {
                var node = parse_var();
                if (node != null)
                    return node;
            }
            {
                var node = parse_mutable();
                if (node != null)
                    return node;
            }
            {
                var node = parse_set();
                if (node != null)
                    return node;
            }

            return null;
        }

        public LetSyntax parse_let()
        {
            var start = lexer.get_previous_position();

            var success_let_1 = lexer.parse_keyword("let");
            if (!success_let_1)
                    return null;
            var binding = parse_binding();
            if (binding == null)
                throw new ParserException("Unable to parse binding.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new LetSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                binding = binding,
            };

            return ret;
        }

        public VarSyntax parse_var()
        {
            var start = lexer.get_previous_position();

            var success_var_1 = lexer.parse_keyword("var");
            if (!success_var_1)
                    return null;
            var binding = parse_binding();
            if (binding == null)
                throw new ParserException("Unable to parse binding.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new VarSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                binding = binding,
            };

            return ret;
        }

        public MutableSyntax parse_mutable()
        {
            var start = lexer.get_previous_position();

            var success_mutable_1 = lexer.parse_keyword("mutable");
            if (!success_mutable_1)
                    return null;
            var binding = parse_binding();
            if (binding == null)
                throw new ParserException("Unable to parse binding.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new MutableSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                binding = binding,
            };

            return ret;
        }

        public BindingSyntax parse_binding()
        {
            var start = lexer.get_previous_position();

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    return null;
                }
            }
            else
            {
                    return null;
            }
            var annotation = parse_bindingannotation();
            var operation = parse_operation();
            if (operation == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new BindingSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                annotation = annotation,
                operation = operation,
            };

            return ret;
        }

        public BindingAnnotationSyntax parse_bindingannotation()
        {
            var start = lexer.get_previous_position();

            var success_colon_1 = lexer.parse_colon();
            if (!success_colon_1)
                    return null;
            var spec = parse_bindingspec();
            if (spec == null)
                throw new ParserException("Unable to parse bindingspec.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new BindingAnnotationSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                spec = spec,
            };

            return ret;
        }

        public object[] parse_bindingspec_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_bindingspec();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_bindingspec()
        {
            {
                var node = parse_structure();
                if (node != null)
                    return node;
            }
            {
                var node = parse_type();
                if (node != null)
                    return node;
            }
            {
                var node = parse_array();
                if (node != null)
                    return node;
            }

            return null;
        }

        public SetSyntax parse_set()
        {
            var start = lexer.get_previous_position();

            var success_set_1 = lexer.parse_keyword("set");
            if (!success_set_1)
                    return null;
            var target = parse_operation();
            if (target == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var source = parse_operation();
            if (source == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new SetSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                target = target,
                source = source,
            };

            return ret;
        }

        public OperationSyntax parse_operation()
        {
            var start = lexer.get_previous_position();
            var operands = parse_operand_list();
            if (operands == null)
                return null;

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new OperationSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                operands = operands,
            };

            return ret;
        }

        public OperandSyntax[] parse_operand_list()
        {
            List<OperandSyntax> list = null;
            while (true)
            {
                var node = parse_operand();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<OperandSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public OperandSyntax parse_operand()
        {
            var start = lexer.get_previous_position();
            var expression = parse_expression();
            if (expression == null)
                return null;
            var postfixes = parse_postfix_list();

            var end = lexer.get_position();

            var ret = new OperandSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                expression = expression,
                postfixes = postfixes,
            };

            return ret;
        }

        public object[] parse_postfix_list()
        {
            List<object> list = null;
            while (true)
            {
                var node = parse_postfix();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<object>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public object parse_postfix()
        {
            {
                var node = parse_memberaccess();
                if (node != null)
                    return node;
            }
            {
                var node = parse_catcher();
                if (node != null)
                    return node;
            }
            {
                var node = parse_is();
                if (node != null)
                    return node;
            }

            return null;
        }

        public MemberAccessSyntax parse_memberaccess()
        {
            var start = lexer.get_previous_position();

            var success_dot_1 = lexer.parse_punctuation(".");
            if (!success_dot_1)
                    return null;
            var member = parse_name();
            if (member == null)
                throw new ParserException("Unable to parse name.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new MemberAccessSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                member = member,
            };

            return ret;
        }

        public CatcherSyntax parse_catcher()
        {
            var start = lexer.get_previous_position();
            var catchers = parse_catch_list();
            if (catchers == null)
                return null;
            var dropper = parse_drop();

            var end = lexer.get_position();

            var ret = new CatcherSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                catchers = catchers,
                dropper = dropper,
            };

            return ret;
        }

        public CatchSyntax[] parse_catch_list()
        {
            List<CatchSyntax> list = null;
            while (true)
            {
                var node = parse_catch();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<CatchSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public CatchSyntax parse_catch()
        {
            var start = lexer.get_previous_position();

            var success_catch_1 = lexer.parse_keyword("catch");
            if (!success_catch_1)
                    return null;
            var condition = parse_operation();
            if (condition == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var handler = parse_operation();
            if (handler == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new CatchSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                condition = condition,
                handler = handler,
            };

            return ret;
        }

        public DropSyntax parse_drop()
        {
            var start = lexer.get_previous_position();

            var success_drop_1 = lexer.parse_keyword("drop");
            if (!success_drop_1)
                    return null;
            var handler = parse_operation();
            if (handler == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new DropSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                handler = handler,
            };

            return ret;
        }

        public IsSyntax parse_is()
        {
            var start = lexer.get_previous_position();

            var success_is_1 = lexer.parse_keyword("is");
            if (!success_is_1)
                    return null;
            var condition = parse_operand_list();
            if (condition == null)
                throw new ParserException("Unable to parse operand list.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new IsSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                condition = condition,
            };

            return ret;
        }

        public ContinueSyntax parse_continue()
        {
            var start = lexer.get_previous_position();

            var success_continue_1 = lexer.parse_keyword("continue");
            if (!success_continue_1)
                    return null;
            var name = parse_loop();

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new ContinueSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
            };

            return ret;
        }

        public LoopSyntax parse_loop()
        {
            var start = lexer.get_previous_position();

            var success_loop_1 = lexer.parse_keyword("loop");
            if (!success_loop_1)
                    return null;

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
                }
            }
            else
            {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            }

            var end = lexer.get_position();

            var ret = new LoopSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
            };

            return ret;
        }

        public BreakSyntax parse_break()
        {
            var start = lexer.get_previous_position();

            var success_break_1 = lexer.parse_keyword("break");
            if (!success_break_1)
                    return null;
            var name = parse_loop();
            var result = parse_operation();

            lexer.parse_colon();

            var end = lexer.get_position();

            var ret = new BreakSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                result = result,
            };

            return ret;
        }

        public ReturnSyntax parse_return()
        {
            var start = lexer.get_previous_position();

            var success_return_1 = lexer.parse_keyword("return");
            if (!success_return_1)
                    return null;
            var result = parse_operation();

            var end = lexer.get_position();

            var ret = new ReturnSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                result = result,
            };

            return ret;
        }

        public ThrowSyntax parse_throw()
        {
            var start = lexer.get_previous_position();

            var success_throw_1 = lexer.parse_keyword("throw");
            if (!success_throw_1)
                    return null;
            var result = parse_operation();

            var end = lexer.get_position();

            var ret = new ThrowSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                result = result,
            };

            return ret;
        }

        public object parse_expression()
        {
            {
                var node = parse_literal();
                if (node != null)
                    return node;
            }
            {
                var node = parse_name();
                if (node != null)
                    return node;
            }
            {
                var node = parse_object();
                if (node != null)
                    return node;
            }
            {
                var node = parse_vector();
                if (node != null)
                    return node;
            }
            {
                var node = parse_block();
                if (node != null)
                    return node;
            }
            {
                var node = parse_if();
                if (node != null)
                    return node;
            }
            {
                var node = parse_match();
                if (node != null)
                    return node;
            }
            {
                var node = parse_lambda();
                if (node != null)
                    return node;
            }
            {
                var node = parse_for();
                if (node != null)
                    return node;
            }
            {
                var node = parse_while();
                if (node != null)
                    return node;
            }
            {
                var node = parse_repeat();
                if (node != null)
                    return node;
            }
            {
                var node = parse_sizeof();
                if (node != null)
                    return node;
            }
            {
                var node = parse_continue();
                if (node != null)
                    return node;
            }
            {
                var node = parse_break();
                if (node != null)
                    return node;
            }
            {
                var node = parse_return();
                if (node != null)
                    return node;
            }
            {
                var node = parse_throw();
                if (node != null)
                    return node;
            }

            return null;
        }

        public LiteralSyntax parse_literal()
        {
            var start = lexer.get_previous_position();

            var literal = lexer.parse_literal();
            if (literal == null)
            {
                    return null;
            }

            var end = lexer.get_position();

            var ret = new LiteralSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                literal = literal,
            };

            return ret;
        }

        public ObjectSyntax parse_object()
        {
            var start = lexer.get_previous_position();

            var success_left_paren_1 = lexer.parse_punctuation("(");
            if (!success_left_paren_1)
                    return null;
            var components = parse_component_list();

            var success_right_paren_3 = lexer.parse_punctuation(")");
            if (!success_right_paren_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new ObjectSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                components = components,
            };

            return ret;
        }

        public ComponentSyntax[] parse_component_list()
        {
            List<ComponentSyntax> list = null;
            while (true)
            {
                var node = parse_component();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<ComponentSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public ComponentSyntax parse_component()
        {
            var start = lexer.get_previous_position();
            var operands = parse_operand_list();
            if (operands == null)
                return null;
            var attributes = parse_attribute_list();
            var value = parse_value();

            lexer.parse_punctuation(",");

            var end = lexer.get_position();

            var ret = new ComponentSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                operands = operands,
                attributes = attributes,
                value = value,
            };

            return ret;
        }

        public ValueSyntax parse_value()
        {
            var start = lexer.get_previous_position();

            var success_colon_1 = lexer.parse_colon();
            if (!success_colon_1)
                    return null;
            var value = parse_operation();
            if (value == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var attributes = parse_attribute_list();

            var end = lexer.get_position();

            var ret = new ValueSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                value = value,
                attributes = attributes,
            };

            return ret;
        }

        public VectorSyntax parse_vector()
        {
            var start = lexer.get_previous_position();

            var success_left_bracket_1 = lexer.parse_punctuation("[");
            if (!success_left_bracket_1)
                    return null;
            var elements = parse_element_list();
            if (elements == null)
                throw new ParserException("Unable to parse element list.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var success_right_bracket_3 = lexer.parse_punctuation("]");
            if (!success_right_bracket_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new VectorSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                elements = elements,
            };

            return ret;
        }

        public ElementSyntax[] parse_element_list()
        {
            List<ElementSyntax> list = null;
            while (true)
            {
                var node = parse_element();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<ElementSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public ElementSyntax parse_element()
        {
            var start = lexer.get_previous_position();
            var operation = parse_operation();
            if (operation == null)
                return null;
            var attributes = parse_attribute_list();

            lexer.parse_punctuation(",");

            var end = lexer.get_position();

            var ret = new ElementSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                operation = operation,
                attributes = attributes,
            };

            return ret;
        }

        public BlockSyntax parse_block()
        {
            var start = lexer.get_previous_position();

            var success_left_curly_1 = lexer.parse_punctuation("{");
            if (!success_left_curly_1)
                    return null;
            var statements = parse_statement_list();

            var success_right_curly_3 = lexer.parse_punctuation("}");
            if (!success_right_curly_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );

            var end = lexer.get_position();

            var ret = new BlockSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                statements = statements,
            };

            return ret;
        }

        public IfSyntax parse_if()
        {
            var start = lexer.get_previous_position();

            var success_if_1 = lexer.parse_keyword("if");
            if (!success_if_1)
                    return null;
            var condition = parse_operation();
            if (condition == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var consequent = parse_statement();
            if (consequent == null)
                throw new ParserException("Unable to parse statement.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var alternative = parse_else();

            var end = lexer.get_position();

            var ret = new IfSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                condition = condition,
                consequent = consequent,
                alternative = alternative,
            };

            return ret;
        }

        public ElseSyntax parse_else()
        {
            var start = lexer.get_previous_position();

            var success_else_1 = lexer.parse_keyword("else");
            if (!success_else_1)
                    return null;

            lexer.parse_colon();
            var alternative = parse_statement();
            if (alternative == null)
                throw new ParserException("Unable to parse statement.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new ElseSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                alternative = alternative,
            };

            return ret;
        }

        public MatchSyntax parse_match()
        {
            var start = lexer.get_previous_position();

            var success_match_1 = lexer.parse_keyword("match");
            if (!success_match_1)
                    return null;
            var scrutinee = parse_operation();
            if (scrutinee == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var cases = parse_case_list();
            if (cases == null)
                throw new ParserException("Unable to parse case list.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var alternative = parse_default();

            var end = lexer.get_position();

            var ret = new MatchSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                scrutinee = scrutinee,
                cases = cases,
                alternative = alternative,
            };

            return ret;
        }

        public CaseSyntax[] parse_case_list()
        {
            List<CaseSyntax> list = null;
            while (true)
            {
                var node = parse_case();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<CaseSyntax>();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }

        public CaseSyntax parse_case()
        {
            var start = lexer.get_previous_position();

            var success_case_1 = lexer.parse_keyword("case");
            if (!success_case_1)
                    return null;
            var condition = parse_operation();
            if (condition == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var consequent = parse_statement();
            if (consequent == null)
                throw new ParserException("Unable to parse statement.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new CaseSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                condition = condition,
                consequent = consequent,
            };

            return ret;
        }

        public DefaultSyntax parse_default()
        {
            var start = lexer.get_previous_position();

            var success_default_1 = lexer.parse_keyword("default");
            if (!success_default_1)
                    return null;
            var alternative = parse_statement();

            var end = lexer.get_position();

            var ret = new DefaultSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                alternative = alternative,
            };

            return ret;
        }

        public LambdaSyntax parse_lambda()
        {
            var start = lexer.get_previous_position();

            var success_lambda_1 = lexer.parse_keyword("lambda");
            if (!success_lambda_1)
                    return null;
            var input = parse_operation();
            if (input == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var success_colon_3 = lexer.parse_colon();
            if (!success_colon_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            var block = parse_statement();
            if (block == null)
                throw new ParserException("Unable to parse statement.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new LambdaSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                input = input,
                block = block,
            };

            return ret;
        }

        public ForSyntax parse_for()
        {
            var start = lexer.get_previous_position();

            var success_for_1 = lexer.parse_keyword("for");
            if (!success_for_1)
                    return null;
            var condition = parse_operation();
            if (condition == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var success_in_3 = lexer.parse_keyword("in");
            if (!success_in_3)
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            var expression = parse_operation();
            if (expression == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var name = parse_label();
            var action = parse_operation();
            if (action == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new ForSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                condition = condition,
                expression = expression,
                name = name,
                action = action,
            };

            return ret;
        }

        public LabelSyntax parse_label()
        {
            var start = lexer.get_previous_position();

            var success_label_1 = lexer.parse_keyword("label");
            if (!success_label_1)
                    return null;

            var name = lexer.parse_identifier(keywords);
            if (name != null)
            {
                if (!is_identifier(name))
                {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
                }
            }
            else
            {
                    throw new ParserException("Unable to parse.", new Span { file = file_name, start = start, end = new Position { line = lexer.previous_line, column = lexer.previous_column } } );
            }

            var end = lexer.get_position();

            var ret = new LabelSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
            };

            return ret;
        }

        public WhileSyntax parse_while()
        {
            var start = lexer.get_previous_position();

            var success_while_1 = lexer.parse_keyword("while");
            if (!success_while_1)
                    return null;
            var condition = parse_operation();
            if (condition == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );
            var name = parse_label();
            var action = parse_operation();
            if (action == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new WhileSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                condition = condition,
                name = name,
                action = action,
            };

            return ret;
        }

        public RepeatSyntax parse_repeat()
        {
            var start = lexer.get_previous_position();

            var success_repeat_1 = lexer.parse_keyword("repeat");
            if (!success_repeat_1)
                    return null;
            var name = parse_label();
            var action = parse_operation();
            if (action == null)
                throw new ParserException("Unable to parse operation.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new RepeatSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                name = name,
                action = action,
            };

            return ret;
        }

        public SizeOfSyntax parse_sizeof()
        {
            var start = lexer.get_previous_position();

            var success_sizeof_1 = lexer.parse_keyword("sizeof");
            if (!success_sizeof_1)
                    return null;
            var spec = parse_type();
            if (spec == null)
                throw new ParserException("Unable to parse type.", new Span { file = file_name, start = new Position { line = lexer.previous_line, column = lexer.previous_column }, end = new Position { line = lexer.line, column = lexer.column } } );

            var end = lexer.get_position();

            var ret = new SizeOfSyntax
            {
                span = new Span
                {
                    file = file_name,
                    start = start,
                    end = end
                },
                spec = spec,
            };

            return ret;
        }

        public bool is_at_end()
        {
            return lexer.is_at_end();
        }

        public ulong get_current_line()
        {
            return lexer.get_position().line;
        }

        public ulong get_current_column()
        {
            return lexer.get_position().column;
        }

        public ulong get_previous_line()
        {
            return lexer.get_previous_position().line;
        }

        public ulong get_previous_column()
        {
            return lexer.get_previous_position().column;
        }

        bool is_identifier(string id)
        {
            if (keywords.Contains(id))
                return false;
            return true;
        }
    }

    public class ProgramSyntax
    {
        public Span span;
        public FileSyntax[] files;
    }

    public class FileSyntax
    {
        public Span span;
        public object[] declarations;
        public object[] statements;
    }

    public class PrivateSyntax
    {
        public Span span;
        public object export;
    }

    public class DefinitionSyntax
    {
        public Span span;
        public TypeSyntax type;
        public AttributeSyntax[] attributes;
        public object concept;
    }

    public class NameSyntax
    {
        public Span span;
        public string name;
        public ExtensionSyntax[] extensions;
    }

    public class ExtensionSyntax
    {
        public Span span;
        public string name;
    }

    public class AttributeSyntax
    {
        public Span span;
        public string attribute;
        public object value;
    }

    public class ClassSyntax
    {
        public Span span;
        public StructureSyntax structure;
        public BodySyntax body;
    }

    public class NamespaceSyntax
    {
        public Span span;
        public BodySyntax body;
    }

    public class UnionSyntax
    {
        public Span span;
        public TagSyntax[] tags;
        public BodySyntax body;
    }

    public class TagSyntax
    {
        public Span span;
        public string name;
        public AttributeSyntax[] attributes;
        public object item;
    }

    public class VariantSyntax
    {
        public Span span;
        public StructureSyntax structure;
        public BodySyntax body;
    }

    public class EnumSyntax
    {
        public Span span;
        public Literal literal;
    }

    public class ConstantSyntax
    {
        public Span span;
        public OperationSyntax operation;
    }

    public class DelegateSyntax
    {
        public Span span;
        public object parameters;
        public AttributeSyntax[] attributes;
        public ReturnsSyntax result;
        public ThrowsSyntax error;
    }

    public class StructureSyntax
    {
        public Span span;
        public object[] members;
    }

    public class BodySyntax
    {
        public Span span;
        public object[] declarations;
    }

    public class FieldSyntax
    {
        public Span span;
        public PropertySyntax export;
    }

    public class PropertySyntax
    {
        public Span span;
        public string name;
        public TypeAnnotationSyntax annotation;
        public AttributeSyntax[] attributes;
    }

    public class TypeAnnotationSyntax
    {
        public Span span;
        public object spec;
    }

    public class TypeSyntax
    {
        public Span span;
        public NameSyntax name;
        public GenericArgumentsSyntax generics;
        public OptionalSyntax optional;
    }

    public class ArraySyntax
    {
        public Span span;
        public object[] members;
    }

    public class GenericArgumentsSyntax
    {
        public Span span;
        public GenericArgumentSyntax[] generics;
    }

    public class GenericArgumentSyntax
    {
        public Span span;
        public TypeSyntax spec;
    }

    public class OptionalSyntax
    {
        public Span span;
    }

    public class FunctionSyntax
    {
        public Span span;
        public string name;
        public GenericArgumentsSyntax generics;
        public RoutineSyntax routine;
    }

    public class ProcedureSyntax
    {
        public Span span;
        public string name;
        public GenericArgumentsSyntax generics;
        public RoutineSyntax routine;
    }

    public class OperatorSyntax
    {
        public Span span;
        public object target;
    }

    public class RoutineSyntax
    {
        public Span span;
        public object parameters;
        public AttributeSyntax[] attributes;
        public ReturnsSyntax returns;
        public ThrowsSyntax throws;
        public object implementation;
    }

    public class SymbolSyntax
    {
        public Span span;
        public string name;
        public AttributeSyntax[] attributes;
        public ReturnsSyntax returns;
        public ThrowsSyntax throws;
        public object implementation;
    }

    public class ParametersSyntax
    {
        public Span span;
        public PropertySyntax[] properties;
    }

    public class ReturnsSyntax
    {
        public Span span;
        public object parameters;
        public AttributeSyntax[] attributes;
    }

    public class ThrowsSyntax
    {
        public Span span;
        public TypeSyntax type;
        public AttributeSyntax[] attributes;
    }

    public class ExternSyntax
    {
        public Span span;
    }

    public class InstructionSyntax
    {
        public Span span;
    }

    public class IntrinsicSyntax
    {
        public Span span;
    }

    public class UseSyntax
    {
        public Span span;
        public NameSyntax name;
    }

    public class ImplementSyntax
    {
        public Span span;
        public TypeSyntax type;
        public AttributeSyntax[] attributes;
        public object[] methods;
    }

    public class TraitSyntax
    {
        public Span span;
        public NameSyntax name;
        public ExtendsSyntax extension;
        public AttributeSyntax[] attributes;
        public object[] functions;
    }

    public class ExtendsSyntax
    {
        public Span span;
        public ExtendSyntax[] extensions;
    }

    public class ExtendSyntax
    {
        public Span span;
        public object spec;
    }

    public class MacroSyntax
    {
        public Span span;
        public string name;
        public object model;
        public OperationSyntax rule;
    }

    public class ModuleSyntax
    {
        public Span span;
        public NameSyntax name;
    }

    public class LetSyntax
    {
        public Span span;
        public BindingSyntax binding;
    }

    public class VarSyntax
    {
        public Span span;
        public BindingSyntax binding;
    }

    public class MutableSyntax
    {
        public Span span;
        public BindingSyntax binding;
    }

    public class BindingSyntax
    {
        public Span span;
        public string name;
        public BindingAnnotationSyntax annotation;
        public OperationSyntax operation;
    }

    public class BindingAnnotationSyntax
    {
        public Span span;
        public object spec;
    }

    public class SetSyntax
    {
        public Span span;
        public OperationSyntax target;
        public OperationSyntax source;
    }

    public class OperationSyntax
    {
        public Span span;
        public OperandSyntax[] operands;
    }

    public class OperandSyntax
    {
        public Span span;
        public object expression;
        public object[] postfixes;
    }

    public class MemberAccessSyntax
    {
        public Span span;
        public NameSyntax member;
    }

    public class CatcherSyntax
    {
        public Span span;
        public CatchSyntax[] catchers;
        public DropSyntax dropper;
    }

    public class CatchSyntax
    {
        public Span span;
        public OperationSyntax condition;
        public OperationSyntax handler;
    }

    public class DropSyntax
    {
        public Span span;
        public OperationSyntax handler;
    }

    public class IsSyntax
    {
        public Span span;
        public OperandSyntax[] condition;
    }

    public class ContinueSyntax
    {
        public Span span;
        public LoopSyntax name;
    }

    public class LoopSyntax
    {
        public Span span;
        public string name;
    }

    public class BreakSyntax
    {
        public Span span;
        public LoopSyntax name;
        public OperationSyntax result;
    }

    public class ReturnSyntax
    {
        public Span span;
        public OperationSyntax result;
    }

    public class ThrowSyntax
    {
        public Span span;
        public OperationSyntax result;
    }

    public class LiteralSyntax
    {
        public Span span;
        public Literal literal;
    }

    public class ObjectSyntax
    {
        public Span span;
        public ComponentSyntax[] components;
    }

    public class ComponentSyntax
    {
        public Span span;
        public OperandSyntax[] operands;
        public AttributeSyntax[] attributes;
        public ValueSyntax value;
    }

    public class ValueSyntax
    {
        public Span span;
        public OperationSyntax value;
        public AttributeSyntax[] attributes;
    }

    public class VectorSyntax
    {
        public Span span;
        public ElementSyntax[] elements;
    }

    public class ElementSyntax
    {
        public Span span;
        public OperationSyntax operation;
        public AttributeSyntax[] attributes;
    }

    public class BlockSyntax
    {
        public Span span;
        public object[] statements;
    }

    public class IfSyntax
    {
        public Span span;
        public OperationSyntax condition;
        public object consequent;
        public ElseSyntax alternative;
    }

    public class ElseSyntax
    {
        public Span span;
        public object alternative;
    }

    public class MatchSyntax
    {
        public Span span;
        public OperationSyntax scrutinee;
        public CaseSyntax[] cases;
        public DefaultSyntax alternative;
    }

    public class CaseSyntax
    {
        public Span span;
        public OperationSyntax condition;
        public object consequent;
    }

    public class DefaultSyntax
    {
        public Span span;
        public object alternative;
    }

    public class LambdaSyntax
    {
        public Span span;
        public OperationSyntax input;
        public object block;
    }

    public class ForSyntax
    {
        public Span span;
        public OperationSyntax condition;
        public OperationSyntax expression;
        public LabelSyntax name;
        public OperationSyntax action;
    }

    public class LabelSyntax
    {
        public Span span;
        public string name;
    }

    public class WhileSyntax
    {
        public Span span;
        public OperationSyntax condition;
        public LabelSyntax name;
        public OperationSyntax action;
    }

    public class RepeatSyntax
    {
        public Span span;
        public LabelSyntax name;
        public OperationSyntax action;
    }

    public class SizeOfSyntax
    {
        public Span span;
        public TypeSyntax spec;
    }
}
