(define (parser) ($
; "
; pub struct ParserError {
;     pub file_name: String,
;     pub line: usize,
;     pub column: usize,
; }

; "

"
using namespace scaly::containers;

struct Parser : Object {
    Lexer lexer;
    String file_name;
    HashSet<""String> keywords;

    Parser(String& deck)
      : lexer(*new(alignof(Lexer), Page::get(this)) Lexer(deck)),
        file_name(*String::from_c_string(Page::get(this), \"\"))
    {
        auto r = Region::create_from_page(Page::get(this));
        Array<""String>& array = *Array<""String>::create(r.page);
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        array.add(*String::from_c_string(r.page, \""(id keyword)"\"));
"   )))
"        Vector<""String>& vector = *Vector<""String>::from_array(r.page, array);
        keywords = *HashSet<""String>::from_vector(Page::get(this), vector);

    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    Result<""Vector<"(id syntax)"Syntax>*, ParserError*> parse_"(downcase-string (id syntax))"_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<"(id syntax)"Syntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_"(downcase-string (id syntax))"(_r_1, _rp, _ep);
                if (node_result.tag == Result<"(id syntax)"Syntax*, ParserError*>::Error)
                    return Result<""Vector<"(id syntax)"Syntax>*, ParserError*> { .tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<"(id syntax)"Syntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<""Vector<"(id syntax)"Syntax>*, ParserError*> { .tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<""Vector<"(id syntax)"Syntax>*, ParserError*> { .tag = Result<""Vector<"(id syntax)"Syntax>*, ParserError*>::Ok, .ok = Vector<"(id syntax)"Syntax>::from_array(_rp, *array) };
        }
    }
"       )"")
"
    Result<"(id syntax)"Syntax*, ParserError*> parse_"(downcase-string (id syntax))"(Region& _pr, Page* _rp, Page* _ep"(if (top? syntax) ", String& file_name" "")") {
        auto _r = Region::create(_pr);
"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_"(downcase-string (link content))"(_r_1, _rp, _ep);
            if (node_result.tag == Result<"(link content)"Syntax*, ParserError*>::Error)
                return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Ok, .ok = 
                    new (alignof("(id syntax)"Syntax), _rp) "(id syntax)"Syntax {
                        .tag = "(id syntax)"Syntax::"(link content)",
                        ."(string-firstchar-downcase (link content))"Syntax = *node,
                    }
                };
            }
        }
"
                )))
"        return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Ok, .ok = nullptr };
"
            )
            ($ ; non-abstract syntax
                (if (top? syntax) ""
"        auto start = this->lexer.previous_position;
"               )
                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
        auto "(property content)"_result = this->parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"(_r, _rp, _ep);
        if ("(property content)"_result.tag == Result<"(if (multiple? content) "Vector<" "")(link content)"Syntax"(if (multiple? content) ">" "")"*, ParserError*>::Error)
            return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Error, .error = "(property content)"_result.error };
        auto "(property content)" = "(property content)"_result.ok;
"
                            (if (optional? content) "" ($
"        if ("(property content)" == nullptr) {
"                               (if (equal? 1 (child-number content))
                                    ($
"            return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Ok, .ok = nullptr };
"                                   )
                                    ($
"            return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
"                                   )
                                )
"        }
"                           ))
                            (if (and (top? syntax) (last-sibling? content)) ($
"        if ("(property content)" != nullptr) {
            if (!this->is_at_end()) {
                auto error_pos = this->lexer.previous_position;
                return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                    .file_name = file_name,
                    .position = lexer.position,
                }};
            };
        };
"                           )"")
                        )
                        ($ ; terminals
"
        auto "
                            (case (type content)
                                (("keyword" "punctuation" "colon" "semicolon") ($ "success_"(if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content))))))
                                (else (property content))
                            )
            " = this->lexer.parse_"(type content)"(_r.page"
                            (case (type content)
                                (("keyword")     ($ ", *String::from_c_string(_r.page, \""(link content)"\")"))
                                (("punctuation") ($ ", *String::from_c_string(_r.page, \""(value (element-with-id (link content)))"\")"))
                                (("identifier")  ($ ", this->keywords"))
                                (("attribute" "literal" "colon" "semicolon") "")
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
                return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Ok, .ok = nullptr };
"                                               )
                                                ($
"
            return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};"
                                                )
                                            )
                                        )
                                    )
                                ))
                                (case (type content)
                                    (("keyword" "punctuation" "colon" "semicolon") ($ 
"        if (!success_"(if (property content) (property content) ($ (case (type content) (("colon" "semicolon") (type content)) (else (link content)))"_"(number->string (child-number content))))") {
"                                       null-handler
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
                        ) ; terminal
                    ) ; syntax or terminal
                ))) ; apply to children of syntax

                (if (top? syntax) ""
"
        auto end = this->lexer.position;
"               )
"
        auto ret = new(alignof("(id syntax)"Syntax), _rp) "(id syntax)"Syntax {
"
                (if (top? syntax) 
"            .file_name = file_name"
"            .start = start,
            .end = end"
                )
                (apply-to-property-children-of syntax (lambda (content) ($
                    ",
            ."(property content)" = "(case (type content) (("literal") "*") (else ""))(property content)
                )))
                ",
        };

        return Result<"(id syntax)"Syntax*, ParserError*> { .tag = Result<"(id syntax)"Syntax*, ParserError*>::Ok, .ok = ret };
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
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

}"

))
