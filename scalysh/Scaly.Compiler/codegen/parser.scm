(define (parser) ($
"    class Parser
    {
        Lexer lexer;
        string file_name = \"\";
        HashSet<""string> keywords = new HashSet<""string>(new string[] {
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"            \""(id keyword)"\",
"   )))
"        });

        public Parser(string deck)
        {
            lexer = new Lexer(deck);
        }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
        public "(if (abstract? syntax) "object" ($ (id syntax)"Syntax"))"[] parse_"(downcase-string (id syntax))"_list()
        {
            List<"(if (abstract? syntax) "object" ($ (id syntax)"Syntax"))"> list = null;
            while (true)
            {
                var node = parse_"(downcase-string (id syntax))"();
                if (node == null)
                    break;

                if (list == null)
                    list = new List<"(if (abstract? syntax) "object" ($ (id syntax)"Syntax"))">();

                list.Add(node);
            }

            if (list != null)
                return list.ToArray();
            else
                return null;
        }
"       )"")
"
        public "(if (abstract? syntax) "object" ($ (id syntax)"Syntax"))" parse_"(downcase-string (id syntax))"("(if (top? syntax) "string file_name" "")")
        {
"       (if (top? syntax) ($
"            this.file_name = file_name;
"       )"")
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"            {
                var node = parse_"(downcase-string (link content))"();
                if (node != null)
                    return node;
            }
"
                )))
"
            return null;
"
            )
            ($ ; non-abstract syntax
                (if (top? syntax) ""
"            var start = lexer.get_previous_position();
"               )
                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"            var "(property content)" = parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"();
"
                            (if (optional? content) "" ($
"            if ("(property content)" == null)
"                               (if (equal? 1 (child-number content))
                                    ($
"                return null;
"                                   )
                                    ($
"                throw new ParserException(file_name, lexer.line, lexer.column);
"                                   )
                                )
                           ))
                            (if (and (top? syntax) (last-sibling? content)) ($
"            if ("(property content)" != null)
            {
                if (!is_at_end())
                {
                    throw new ParserException(file_name, lexer.previous_line, lexer.previous_column);
                }
            }
"                           )"")
                        )
                        ($ ; terminals
"
            "
                            (case (type content)
                                (("keyword" "punctuation" "colon" "semicolon") (if (optional? content) "" ($ "var success_"(if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content))))" = ")))
                                (else ($ "var "(property content)" = "))
                            )
            "lexer.parse_"(type content)"("
                            (case (type content)
                                (("keyword")     ($ "\""(link content)"\""))
                                (("punctuation") ($ "\""(value (element-with-id (link content)))"\""))
                                (("identifier") ($ "keywords"))
                                (("attribute" "literal" "colon" "semicolon") "")
                            )");
"
                            (let
                                ((null-handler
                                    (if (optional? content) ""
                                        ($
                                            (if (equal? 1 (child-number content))
                                                ($
"                    return null;
"                                               )
                                                ($
"                    throw new ParserException(file_name, lexer.line, lexer.column);
"
                                                )
                                            )
                                        )
                                    )
                                ))
                                (case (type content)
                                    (("keyword" "punctuation" "colon" "semicolon") (if (optional? content) "" ($ 
"            if (!success_"(if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content))))")
"                                       null-handler
                                    )))
                                    (("identifier") ($ 
"            if ("(property content)" != null)
            {
                if (!is_identifier("(property content)"))
                {
"                                       null-handler
"                }
            }
            else
            {
"                                       null-handler
"            }
"                                   ))
                                    (else ($ 
"            if ("(property content)" == null)
            {
"                                       null-handler
"            }
"                                   ))
                                )
                            )
                        ) ; terminal
                    ) ; syntax or terminal
                ))) ; apply to children of syntax

                (if (top? syntax) ""
"
            var end = lexer.get_position();
"               )
"
            var ret = new "(id syntax)"Syntax
            {
"                (if (top? syntax) 
"                file_name = file_name"
"                start = start,
                end = end"
                )
                (apply-to-property-children-of syntax (lambda (content) ($
                    ",
                "(property content)" = "(property content)
                )))
                ",
            };

            return ret;
"
            ) ; $
        ) ; abstract or not
"        }
"   ))))
"
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
"

))
