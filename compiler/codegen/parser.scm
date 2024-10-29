(define (parser) ($
"
using namespace scaly::containers;

struct Parser : Object {
    Lexer lexer;
    Vector<""String> keywords_index;
    HashSet<""String> keywords;

    Parser(String text)
      : lexer(Lexer(text)),
        keywords_index(initialize_keywords_index(Page::get(this))),
        keywords(initialize_keywords(Page::get(this))) {}

    Vector<""String> initialize_keywords_index(Page* _rp) {
        Region _r;
        Array<""String>& keywords_builder = *new(alignof(Array<""String>), _r.get_page()) Array<""String>();
        auto p = Page::get(this);
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        keywords_builder.add(String(p, \""(id keyword)"\"));
"   )))
"        return Vector<""String>(_rp, keywords_builder);
    }

    HashSet<""String> initialize_keywords(Page* _rp) {
        Region _r;
        HashSetBuilder<""String>& hash_set_builder = *new(alignof(HashSetBuilder<""String>), _r.get_page()) HashSetBuilder<""String>(this->keywords_index);
        return HashSet<""String>(_rp, hash_set_builder);
    }

    Result<""Literal, ParserError> parse_literal_token(Page* _rp) {
        if (this->lexer.token._tag == Token::Empty)
            lexer.advance();

        switch (this->lexer.token._tag)
        {
            case Token::Literal:
                switch (this->lexer.token._Literal._tag)
                {
                    case LiteralToken::String:
                    {
                        auto ret = Literal(StringLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Character:
                    {
                        auto ret = Literal(CharacterLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Integer:
                    {
                        auto ret = Literal(IntegerLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::FloatingPoint:
                    {
                        auto ret = Literal(FloatingPointLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Hex:
                    {
                        auto ret = Literal(HexLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Boolean:
                    {
                        auto ret = Literal(BooleanLiteral(this->lexer.token._Literal._Boolean.value));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Fragment:
                    {
                        auto ret = Literal(FragmentLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Ok, ._Ok = ret };
                    }
                }
            default:
                return Result<""Literal, ParserError> { ._tag = Result<""Literal, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) ($
        (if (multiple? syntax) ($
"
    Result<""Vector<"(id syntax)"Syntax>*, ParserError> parse_"(downcase-string (id syntax))"_list(Page* _rp, Page* _ep) {
        Region _r;
        List<"(id syntax)"Syntax>& list = *new(alignof(List<"(id syntax)"Syntax>), _r.get_page()) List<"(id syntax)"Syntax>();;
        while(true) {
            auto node_result = this->parse_"(downcase-string (id syntax))"(_rp, _ep);
            if ((node_result._tag == Result<"(id syntax)"Syntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<""Vector<"(id syntax)"Syntax>*, ParserError> { ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<"(id syntax)"Syntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<"(id syntax)"Syntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<""Vector<"(id syntax)"Syntax>*, ParserError> { ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<""Vector<"(id syntax)"Syntax>*, ParserError> { ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<""Vector<"(id syntax)"Syntax>*, ParserError> {
            ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<"(id syntax)"Syntax>), _rp) Vector<"(id syntax)"Syntax>(_rp, list) };
    }
"       )"")
"
    Result<"(id syntax)"Syntax, ParserError> parse_"(downcase-string (id syntax))"(Page* _rp, Page* _ep) {
"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        {
            auto node_result = this->parse_"(downcase-string (link content))"(_rp, _ep);
            if (node_result._tag == Result<"(link content)"Syntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Ok, ._Ok = 
                    "(id syntax)"Syntax("(link content)"Syntax(node))
                };
            }
        }
"
                )))
"        return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
"
            )
            ($ ; non-abstract syntax
"        auto start = this->lexer.previous_position;
"               (apply-to-children-of syntax (lambda (content) ($
                    (case (type content)
                        (("syntax") ($
"
        auto "(property content)"_start = this->lexer.position;
        auto "(property content)"_result = this->parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(_rp, _ep);
        if ("(property content)"_result._tag == Result<"(if (multiple? content) "Vector<" "")(link content)"Syntax"(if (multiple? content) ">" "")", ParserError>::Error)
        {
"
                            (if (optional? content)
                                ($
"            switch ("(property content)"_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = "(property content)"_result._Error };
            }
"
                                )
                                (if (equal? 1 (child-number content))
                                    ($
"            return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = "(property content)"_result._Error };
"                                   )
                                    ($
"            switch ("(property content)"_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax("(property content)"_start, lexer.position, String(_ep, \"a valid "(link content)" syntax\"))) };
                case ParserError::InvalidSyntax:
                    return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = "(property content)"_result._Error };
            }
"                                   )
                                )
                            )

"        }

        "                   (if (and (optional? content)(not (multiple? content)))
                                ($ ""(link content)"Syntax*")
                                "auto"
                            )
                            " "
                            (property content)
                            " = "
                            (if (optional? content)
                                ($
                                    (property content)
                                    "_result._tag == Result<"
                                    (if (multiple? content) "Vector<" "")
                                    (link content)
                                    "Syntax"
                                    (if (multiple? content) ">" "")
                                    ", ParserError>::Error ? nullptr : "
                                    (if (multiple? content) "" ($
                                    "new(alignof("(link content)"Syntax), _rp) "(link content)"Syntax("
                                    ))
                                )
                                ""
                            )
                            (property content)                            
                            "_result._Ok"
                            (if (and (optional? content)(not (multiple? content))) ")" "")
                            ";
"
                        ))
                        (("literal") ($
"
        auto "(property content)"_start = this->lexer.previous_position;
        auto "(property content)"_result = this->parse_literal_token(_rp);
        if ("(property content)"_result._tag == Result<""Literal, ParserError>::Error)
        {
"                           (if (optional? content) "" ($
"            if ("(property content)"_result._Error._tag == ParserError::OtherSyntax)
"                               (if (equal? 1 (child-number content))
                                    ($
"               return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
"                                   )
                                    ($
"               return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax("(property content)"_start, lexer.position, String(_ep, \"a literal\"))) };
"                                   )
                                )
                            ))
"        }

        "                   (if (and (optional? content)(not (multiple? content))(link content))
                                ($ (link content)"Syntax*")
                                "auto"
                            )
                            " "
                            (property content)
                            " = "
                            (if (optional? content)
                                ($
                                    (property content)
                                    "_result._tag == Result<"
                                    (if (multiple? content) "Vector<" "")
                                    (link content)
                                    "Syntax"
                                    (if (multiple? content) ">" "")
                                    ", ParserError>::Error ? nullptr : "
                                    (if (multiple? content) "" ($
                                    "new(alignof("(link content)"Syntax), _rp) "(link content)"Syntax("
                                    ))
                                )
                                ""
                            )
                            (property content)                            
                            "_result._Ok"
                            (if (and (optional? content)(not (multiple? content))) ")" "")
                            ";
"
                        ))
                        (else (let ((syntax-moniker (if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content)))))) ($ ; terminals
"
        auto start_"syntax-moniker" = this->lexer.previous_position;
        auto "
                            (case (type content)
                                (("keyword" "punctuation" "colon" "semicolon") ($ "success_"syntax-moniker))
                                (else (property content))
                            )
            " = this->lexer.parse_"(type content)"("
                            (case (type content)
                                (("keyword")     ($ "_rp, *this->keywords_index["(number->string (- (child-number (element-with-id (link content))) 1))"]"))
                                (("punctuation") ($ "'"(value (element-with-id (link content)))"'"))
                                (("identifier")  ($ "_rp, this->keywords"))
                                (("attribute" "colon" "semicolon") "_rp")
                            )");
"
                            (let
                                ((null-handler
                                    (if (optional? content) 
"
"                                       ($
                                            (if (equal? 1 (child-number content))
                                                ($
"
            return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
"                                               )
                                                ($
"
            return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_"syntax-moniker", lexer.position, String(_ep, \""
                                                    (case (type content)
                                                        (("keyword")     (link content))
                                                        (("punctuation") (value (element-with-id (link content))))
                                                        (("identifier")  "an identifier")
                                                        (("attribute") "an attribute")
                                                        (("colon") "a colon or a line feed")
                                                        (("semicolon") "a semicolon")
                                                    )
            "\"))) };"
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
"        if ("(property content)" != nullptr) {
            if (!this->is_identifier(*"(property content)")) {"
                                    null-handler
"
            }
        }
        else {"
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
                        ))) ; terminal
                    ) ; syntax, literal or other stuff
                ))) ; apply to children of syntax
"
        auto end = this->lexer.position;

        auto ret = "(id syntax)"Syntax(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "
                    (case (type content)
                        (("keyword" "punctuation") "*")
                        (("identifier" "attribute") (if (optional? content) "" "*"))
                        (else "")
                    )
                    (property content)
                )))
                ");

        return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Ok, ._Ok = ret };
"
            ) ; $
        ) ; abstract or not
"    }
"   )))
"
    bool is_at_end() {
        return this->lexer.is_at_end();
    }

    bool is_identifier(String& id) {
        if (this->keywords.contains(id))
            return false;
        else
            return true;
    }
};

"

))
