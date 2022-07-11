(define (parser) ($
"
using namespace scaly::containers;

struct Parser : Object {
    Lexer lexer;
    HashSet<""String> keywords;

    Parser(Region& _pr, Page* _rp, String text)
      : lexer(*new(alignof(Lexer), _rp) Lexer(text)),
        keywords(initialize_keywords(_pr, _rp)) {}

    HashSet<""String> initialize_keywords(Region& _pr, Page* _rp) {
        Region _r(_pr);
        HashSetBuilder<""String>& hash_set_builder = *new(alignof(HashSetBuilder<""String>), _r.page) HashSetBuilder<""String>();
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        hash_set_builder.add(String(Page::get(this), \""(id keyword)"\"));
"   )))
"        keywords = HashSet<""String>(_r, _rp, hash_set_builder);
        return keywords;
    }

    Result<""Literal, ParserError> parse_literal_token(Region& _pr, Page* _rp) {
        Region _r(_pr);
        if (this->lexer.token._tag == Token::Empty)
            lexer.advance(_r);

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
    Result<""Vector<"(id syntax)"Syntax>*, ParserError> parse_"(downcase-string (id syntax))"_list(Region& _pr, Page* _rp, Page* _ep) {
        Region _r(_pr);
        {
            Region _r_1(_r);
            Array<"(id syntax)"Syntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_"(downcase-string (id syntax))"(_r_1, _rp, _ep);
                if ((node_result._tag == Result<"(id syntax)"Syntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                    return Result<""Vector<"(id syntax)"Syntax>*, ParserError> { ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Error, ._Error = node_result._Error };
                if (node_result._tag == Result<"(id syntax)"Syntax, ParserError>::Ok) {
                    auto node = node_result._Ok;
                    if (array == nullptr)
                        array = new(alignof(Array<"(id syntax)"Syntax>), _r_1.page) Array<"(id syntax)"Syntax>();
                    array->add(node);
                } else {
                    if ((array == nullptr) && (node_result._tag == Result<"(id syntax)"Syntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                        return Result<""Vector<"(id syntax)"Syntax>*, ParserError> { ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Error, ._Error = node_result._Error };
                    break;
                }
            }

            if (array == nullptr)
                return Result<""Vector<"(id syntax)"Syntax>*, ParserError> { ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Ok, ._Ok = nullptr };
            
            return Result<""Vector<"(id syntax)"Syntax>*, ParserError> {
                ._tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError>::Ok,
                ._Ok = new(alignof(Vector<"(id syntax)"Syntax>), _rp) Vector<"(id syntax)"Syntax>(_rp, *array) };
        }
    }
"       )"")
"
    Result<"(id syntax)"Syntax, ParserError> parse_"(downcase-string (id syntax))"(Region& _pr, Page* _rp, Page* _ep) {
        Region _r(_pr);
"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        {
            Region _r_1(_r);
            auto node_result = this->parse_"(downcase-string (link content))"(_r_1, _rp, _ep);
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
        auto "(property content)"_result = this->parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(_r, _rp, _ep);
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
                                ($
"                return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = "(property content)"_result._Error };
"                               )
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
        auto "(property content)"_result = this->parse_literal_token(_r, _rp);
        if ("(property content)"_result._tag == Result<""Literal, ParserError>::Error)
        {
"                           (if (optional? content) "" ($
"            if ("(property content)"_result._Error._tag == ParserError::OtherSyntax)
"                               (if (equal? 1 (child-number content))
                                    ($
"               return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
"                                   )
                                    ($
"               return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax("(property content)"_start, lexer.position)) };
"                                   )
                                )
                            ))
"        }

        "                   (if (and (optional? content)(not (multiple? content)))
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
            " = this->lexer.parse_"(type content)"(_r, _rp"
                            (case (type content)
                                (("keyword")     ($ ", String(_r.page, \""(link content)"\")"))
                                (("punctuation") ($ ", String(_r.page, \""(value (element-with-id (link content)))"\")"))
                                (("identifier")  ($ ", this->keywords"))
                                (("attribute" "colon" "semicolon") "")
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
            return Result<"(id syntax)"Syntax, ParserError> { ._tag = Result<"(id syntax)"Syntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_"syntax-moniker", lexer.position)) };"
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
                        (("keyword" "punctuation" "identifier" "attribute") "*")
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
