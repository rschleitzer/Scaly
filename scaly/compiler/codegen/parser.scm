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
            else {}

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
            else
                return Result[Literal, ParserError](ParserError(DifferentSyntax()))
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) ($
        (if (multiple? syntax) ($
"
    function parse_"(downcase-string (id syntax))"_list(this: Parser) returns pointer[Vector["(id syntax)"Syntax]] throws ParserError {
        var r Region()
        var list List["(id syntax)"Syntax]$()
        while true
        {
            try let node parse_"(downcase-string (id syntax))"(rp, ep)
                when i: ParserError.Invalid
                    throw i
                when d: ParserError.Different
                {
                    if (*list).head = 0
                    {
                        throw d
                    } else {
                        return Vector["(id syntax)"Syntax]#(rp, *list)
                    }
                }

            (*list).add(node)
        }
    }
"       )"")
"
    function parse_"(downcase-string (id syntax))"(this: Parser) returns "(id syntax)"Syntax throws ParserError {
"       (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        {
            choose parse_"(downcase-string (link content))"(rp, ep)
                when error: Success.Error
                {
                    choose error
                        when i: ParserError.Invalid throw i
                        when d: ParserError.Different {}
                }
                when node: Success.Ok
                    return "(id syntax)"Syntax("(link content)"Syntax(node))
        }
"
                )))
"        throw ParserError(DifferentSyntax())
"
            )
            ($ ; non-abstract syntax
"        let start lexer.previous_position
"               (apply-to-children-of syntax (lambda (content) ($
                    (case (type content)
                        (("syntax") ($
"
        let "(property content)"_start lexer.position
"
                            (if (optional? content) ($
"        var "(property content)": pointer["(if (multiple? content) "Vector[" "")(link content)"Syntax"(if (multiple? content) "]" "")"] null 
        choose parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(rp, ep)
            when error: Success.Error
            {
                choose error
                    when d: ParserError.Different {}
                    when i: ParserError.Invalid
                        throw i
            }
            when success: Success.Ok
                set "(property content)": "(if (multiple? content) "" ($ (link content)"Syntax#("))"success"(if (multiple? content) "" ")")"
"
                                ) ; end optional
                                ($ ; non-optional
"        try let "(property content)" parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(rp, ep)
"
                                    (if (equal? 1 (child-number content))
                                        ($
"            when d: ParserError.Different throw d
"                                       )
                                        ($
"            when d: ParserError.Different
                throw InvalidSyntax("(property content)"_start, lexer.position, String(ep, \"a valid "(link content)" syntax\"))
"                                       )
                                    )
"            when i: ParserError.Invalid throw i
"
                                ) ; non-optional or multiple
                            )
                        ))
                        (("literal") ($
"
        try let "(property content)" parse_literal_token(rp, ep)
            else throw
"
                        ))
                        (else (let ((syntax-moniker (if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content)))))) ($ ; terminals
"
        let start_"syntax-moniker" lexer.previous_position
        let "
                            (case (type content)
                                (("keyword" "punctuation" "colon" "semicolon") ($ "success_"syntax-moniker))
                                (else (property content))
                            )
            " lexer.parse_"(type content)"("
                            (case (type content)
                                (("keyword")     ($ "*keywords_index.get("(number->string (- (child-number (element-with-id (link content))) 1))")"))
                                (("punctuation") ($ "'"(value (element-with-id (link content)))"'"))
                                (("identifier")  ($ "rp, keywords"))
                                (("colon" "semicolon") "")
                                (("attribute") "rp")
                            )")
"
                            (let
                                ((null-handler
                                    (if (optional? content) "" ($
                                            (if (equal? 1 (child-number content))
                                                ($
"
            throw ParserError(DifferentSyntax())
"                                               )
                                                ($
"
            throw ParserError(InvalidSyntax(start_"syntax-moniker", lexer.position, String(ep, \""
                                                    (case (type content)
                                                        (("keyword")     (link content))
                                                        (("punctuation") (value (element-with-id (link content))))
                                                        (("identifier")  "an identifier")
                                                        (("attribute") "an attribute")
                                                        (("colon") "a colon or a line feed")
                                                        (("semicolon") "a semicolon")
                                                    )
            "\")))"
                                                )
                                            )
                                    ))
                                ))
                                (if (string=? "" null-handler) "" ($
                                (case (type content)
                                    (("keyword" "punctuation" "colon" "semicolon") ($ 
"        if success_"(if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content))))" = false
        {"
                                            null-handler
"        }
"
                                        ))
                                        (("identifier") ($ 
"        if "(property content)" = null
        {"
                                            null-handler
            "
        }
"                                       ))
                                        (else ($ 
"        if "(property content)" = null
        {
"                                           null-handler
"        }
"                                       ))
                                    )
                                ))
                            )
                        )))  ;terminal
                    ) ; syntax, literal or other stuff
                ))) ; apply to children of syntax
"
        let end lexer.position

        return "(id syntax)"Syntax(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "
                    (case (type content)
                        (("keyword" "punctuation") "*")
                        (("identifier" "attribute") (if (optional? content) "" "*"))
                        (else (if (and (multiple? content) (not (optional? content))) "*" ""))
                    )
                    (property content)
                )))
                ")
"
            ) ; $
        ) ; abstract or not
"    }
"   )))
"
    function is_at_end(this: Parser) returns bool {
        return lexer.is_at_end()
    }
}

"

))
