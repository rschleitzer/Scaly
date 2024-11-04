(define (parser) ($
"
define Parser
(
    lexer: Lexer
    keywords_index: Vector[String]
    keywords: HashSet[String]
)
{
    init(text: String) (lexer(Lexer(text))) (keywords_index(initialize_keywords_index())) (keywords(initialize_keywords())) {}

    function initialize_keywords_index(this: Parser) returns Vector[String]$ {
        var r Region()
        var keywords_builder Array[String]$()
        var p Page->get(this)
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        (*keywords_builder).add(String(p, \""(id keyword)"\"))
"   )))
"        return Vector[String](p, *keywords_builder)
    }

    function initialize_keywords(this: Parser) returns HashSet[String]$ {
        var r Region()
        var hash_set_builder HashSetBuilder[String]$(keywords_index)
        return HashSet[String](Page->get(this), *hash_set_builder)
    }

    function parse_literal_token(this: Parser) returns Literal throws ParserError {
        choose lexer.token
            when empty: Token.Empty lexer.advance()
            default {}

        choose lexer.token
            when literal: Token.Literal
            {
                choose literal
                    when string: LiteralToken.String {
                        let ret Literal(StringLiteral(String(rp, string.value)))
                        lexer.empty()
                        return ret
                    }

                    when character: LiteralToken.Character {
                        let ret Literal(CharacterLiteral(String(rp, character.value)))
                        lexer.empty()
                        return ret
                    }

                    when integer: LiteralToken.Integer {
                        let ret Literal(IntegerLiteral(String(rp, integer.value)))
                        lexer.empty()
                        return ret
                    }

                    when fp: LiteralToken.FloatingPoint {
                        let ret Literal(FloatingPointLiteral(String(rp, fp.value)))
                        lexer.empty()
                        return ret
                    }

                    when hex: LiteralToken.Hex {
                        let ret Literal(HexLiteral(String(rp, hex.value)))
                        lexer.empty()
                        return ret
                    }

                    when boolean: LiteralToken.Boolean {
                        let ret Literal(BooleanLiteral(boolean.value))
                        lexer.empty()
                        return ret
                    }

                    when fragment: LiteralToken.Fragment {
                        let ret Literal(FragmentLiteral(String(rp, fragment.value)))
                        lexer.empty()
                        return ret
                    }
            }
            default
                return Result[Literal, ParserError](ParserError(DifferentSyntax()))
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) ($
        (if (multiple? syntax) ($
"
    function parse_"(downcase-string (id syntax))"_list(this: Parser) returns pointer[Vector["(id syntax)"Syntax]] throws ParserError {
        var r Region()
        List<"(id syntax)"Syntax]& list = *new(alignof(List<"(id syntax)"Syntax]), _r.get_page()) List<"(id syntax)"Syntax]()
        while(true) {
            let node_result = parse_"(downcase-string (id syntax))"(rp, ep)
            if ((node_result._tag == Result["(id syntax)"Syntax, ParserError]::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result[Vector<"(id syntax)"Syntax]*, ParserError] { ._tag = Result[Vector<"(id syntax)"Syntax]*, ParserError]::Error, ._Error = node_result._Error }
            if (node_result._tag == Result["(id syntax)"Syntax, ParserError]::Ok) {
                let node = node_result._Ok
                list.add(node)
            } else {
                if ((list.count() == 0) && (node_result._tag == Result["(id syntax)"Syntax, ParserError]::Error) && (node_result._Error._tag == ParserError::DifferentSyntax))
                    return Result[Vector<"(id syntax)"Syntax]*, ParserError] { ._tag = Result[Vector<"(id syntax)"Syntax]*, ParserError]::Error, ._Error = node_result._Error }
                break
            }
        }

        if (list.count() == 0)
            return Result[Vector<"(id syntax)"Syntax]*, ParserError] { ._tag = Result[Vector<"(id syntax)"Syntax]*, ParserError]::Ok, ._Ok = nullptr }
        
        return Result[Vector<"(id syntax)"Syntax]*, ParserError] {
            ._tag = Result[Vector<"(id syntax)"Syntax]*, ParserError]::Ok,
            ._Ok = new(alignof(Vector<"(id syntax)"Syntax]), _rp) Vector<"(id syntax)"Syntax](_rp, list) }
    }
"       )"")
"
    function parse_"(downcase-string (id syntax))"(this: Parser) returns "(id syntax)"Syntax throws ParserError {
"       (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        try let node parse_"(downcase-string (link content))"(rp, ep)
            when error: ParserError.InvalidSyntax
                throw error
        return"(id syntax)"Syntax("(link content)"Syntax(node))
"
                )))
"        throw ParserError(DifferentSyntax())
"
            )
            ($ ; non-abstract syntax
"        let start = lexer.previous_position
"               (apply-to-children-of syntax (lambda (content) ($
                    (case (type content)
                        (("syntax") ($
"
        let "(property content)"_start = lexer.position
"
                            (if (and (optional? content)(not (multiple? content)))
                                ($
"        let "(property content)"_result = parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(rp, ep)
        if ("(property content)"_result._tag == Result["(if (multiple? content) "Vector<" "")(link content)"Syntax"(if (multiple? content) "]" "")", ParserError]::Error)
        {
            switch ("(property content)"_result._Error._tag) {
                case ParserError::DifferentSyntax:
                    break
                case ParserError::InvalidSyntax:
                    return Result["(id syntax)"Syntax, ParserError] { ._tag = Result["(id syntax)"Syntax, ParserError]::Error, ._Error = "(property content)"_result._Error }
            }
        }

        "(link content)"Syntax* "(property content)" = "(property content)"_result._tag == Result["(link content)"Syntax, ParserError]::Error ? nullptr : new(alignof("(link content)"Syntax), _rp) "(link content)"Syntax("(property content)
                                    "_result._Ok)
    "
                                ) ; end optional and not multiple
                                ($ ; non-optional or multiple
"        let "(property content)"_result = parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(rp, ep)
        choose "(property content)"_result
            when error: Success.Error
            {
                choose error
"
                                    (if (optional? content)
                                        ($
"                    when d: ParserError.DifferentSyntax {}
"                                       )
                                        (if (equal? 1 (child-number content))
                                            ($
"                    when d: ParserError.DifferentSyntax throw d
"                                           )
                                            ($
"                    when d: ParserError.DifferentSyntax
                        throw InvalidSyntax("(property content)"_start, lexer.position, String(ep, \"a valid "(link content)" syntax\"))
"                                           )
                                        )
                                    )
"                    when i: ParserError.InvalidSyntax throw i
            }
        let "
                                    (property content)
                                    " = "
                                    (if (optional? content)
                                        ($
                                            (property content)
                                            "_result._tag == Result["
                                            (if (multiple? content) "Vector[" "")
                                            (link content)
                                            "Syntax"
                                            (if (multiple? content) "]" "")
                                            ", ParserError]::Error ? nullptr : "
                                            (if (multiple? content) "" ($
                                            "new(alignof("(link content)"Syntax), _rp) "(link content)"Syntax("
                                            ))
                                        )
                                        ""
                                    )
                                    (property content)                            
                                    "_result._Ok"
                                    (if (and (optional? content)(not (multiple? content))) ")" "")
                                "
    "
                                ) ; non-optional or multiple
                            )
                        ))
                        (("literal") ($
"
        let "(property content)"_start = lexer.previous_position
        let "(property content)"_result = parse_literal_token(_rp)
        if ("(property content)"_result._tag == Result[Literal, ParserError]::Error)
        {
"                           (if (optional? content) "" ($
"            if ("(property content)"_result._Error._tag == ParserError::DifferentSyntax)
"                               (if (equal? 1 (child-number content))
                                    ($
"               return Result["(id syntax)"Syntax, ParserError] { ._tag = Result["(id syntax)"Syntax, ParserError]::Error, ._Error = ParserError(DifferentSyntax()) }
"                                   )
                                    ($
"               return Result["(id syntax)"Syntax, ParserError] { ._tag = Result["(id syntax)"Syntax, ParserError]::Error, ._Error = ParserError(InvalidSyntax("(property content)"_start, lexer.position, String(_ep, \"a literal\"))) }
"                                   )
                                )
                            ))
"        }

        "                   (if (and (optional? content)(not (multiple? content))(link content))
                                ($ (link content)"Syntax*")
                                "let"
                            )
                            " "
                            (property content)
                            " = "
                            (if (optional? content)
                                ($
                                    (property content)
                                    "_result._tag == Result["
                                    (if (multiple? content) "Vector[" "")
                                    (link content)
                                    "Syntax"
                                    (if (multiple? content) "]" "")
                                    ", ParserError]::Error ? nullptr : "
                                    (if (multiple? content) "" ($
                                    "new(alignof("(link content)"Syntax), _rp) "(link content)"Syntax("
                                    ))
                                )
                                ""
                            )
                            (property content)                            
                            "_result._Ok"
                            (if (and (optional? content)(not (multiple? content))) ")" "")
                            "
"
                        ))
                        (else (let ((syntax-moniker (if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content)))))) ($ ; terminals
"
        let start_"syntax-moniker" = lexer.previous_position
        let "
                            (case (type content)
                                (("keyword" "punctuation" "colon" "semicolon") ($ "success_"syntax-moniker))
                                (else (property content))
                            )
            " = lexer.parse_"(type content)"("
                            (case (type content)
                                (("keyword")     ($ "_rp, *keywords_index["(number->string (- (child-number (element-with-id (link content))) 1))"]"))
                                (("punctuation") ($ "'"(value (element-with-id (link content)))"'"))
                                (("identifier")  ($ "_rp, keywords"))
                                (("attribute" "colon" "semicolon") "_rp")
                            )")
"
                            (let
                                ((null-handler
                                    (if (optional? content) 
"
"                                       ($
                                            (if (equal? 1 (child-number content))
                                                ($
"
            return Result["(id syntax)"Syntax, ParserError] { ._tag = Result["(id syntax)"Syntax, ParserError]::Error, ._Error = ParserError(DifferentSyntax()) }
"                                               )
                                                ($
"
            return Result["(id syntax)"Syntax, ParserError] { ._tag = Result["(id syntax)"Syntax, ParserError]::Error, ._Error = ParserError(InvalidSyntax(start_"syntax-moniker", lexer.position, String(_ep, \""
                                                    (case (type content)
                                                        (("keyword")     (link content))
                                                        (("punctuation") (value (element-with-id (link content))))
                                                        (("identifier")  "an identifier")
                                                        (("attribute") "an attribute")
                                                        (("colon") "a colon or a line feed")
                                                        (("semicolon") "a semicolon")
                                                    )
            "\"))) }"
                                                )
                                            )
                                        )
                                    )
                                ))
                                (case (type content)
                                    (("keyword" "punctuation" "colon" "semicolon") ($ 
"        if (!success_"(if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content))))") {"
                                    null-handler
"        }
"
                                    ))
                                    (("identifier") ($ 
"        if ("(property content)" == nullptr)
        {"
                                    null-handler
            "
        }
"                                   ))
                                    (else ($ 
"        if ("(property content)" == nullptr) {
"                                       null-handler
"        }
"                                   ))
                                )
                            )
                        )))  ;terminal
                    ) ; syntax, literal or other stuff
                ))) ; apply to children of syntax
"
        let end = lexer.position

        let ret = "(id syntax)"Syntax(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "
                    (case (type content)
                        (("keyword" "punctuation") "*")
                        (("identifier" "attribute") (if (optional? content) "" "*"))
                        (else "")
                    )
                    (property content)
                )))
                ")

        return Result["(id syntax)"Syntax, ParserError] { ._tag = Result["(id syntax)"Syntax, ParserError]::Ok, ._Ok = ret }
"
            ) ; $
        ) ; abstract or not
"    }
"   )))
"
    bool is_at_end() {
        return lexer.is_at_end()
    }
}

"

))
