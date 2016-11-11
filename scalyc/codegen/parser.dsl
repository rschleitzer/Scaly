(define (parser) ($

"class Parser {
    init(theFileName: String, text: String) {
        lexer = Lexer(text)
        fileName = theFileName
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        "(name keyword)" = String(\""(id keyword)"\")
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"        "(id punctuation)" = String(\""(value punctuation)"\")
"   )))
"    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    function parse"(id syntax)"List() -> ["(id syntax)"] throws ParserError {
        mutable ret: ["(id syntax)"]? = null
        while true {
            let node: "(id syntax)" = parse"(id syntax)"()
                catch _ break;
            if ret == null
                ret = ["(id syntax)"]()
            ret.push(node)
        }
        return ret
    }
"       )"")
"
    function parse"(id syntax)"() -> "(id syntax)" throws ParserError {"
        (if (abstract? syntax)
            ($
"
        mutable errors: [ParserError] = [ParserError]()
        mutable start: Position = lexer.getPreviousPosition()
"                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node: "(link content)" = parse"(link content)"() catch _ (error) {
                errors.push(error)
            }

            if node != null
                return node
        }
"
                )))
"
        throw unableToParse(Position(start), [ParserError](errors))
"
            )
            ($ ; non-abstract syntax
"
        mutable start: Position = lexer.getPreviousPosition()
"                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"        let "(property content)": "(if (multiple? content) "[" "")(link content)(if (multiple? content) "]" "")" = parse"(link content)(if (multiple? content) "List" "")"() catch _ "
                    (if (optional? content)
                        "null
"                       ($ "(error)
            throw "(if (string=? (type content) "syntax") "error" ($ "ParserError."
                (case (type content) (("keyword") "Keyword") (("punctuation") "Punctuation")(("identifier") "Identifier")(("literal") "Literal")(("prefixoperator" "binaryoperator" "postfixoperator") "Operator"))
                "Expected(start"(case (type content) (("keyword" "punctuation") ($ ", String("((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ""))"))"))"
"                       )
                    )
                           (if (top? syntax) ($
"        if "(property content)" != null {
            if !isAtEnd() {
                mutable current: Position = lexer.getPosition()
                throw notAtEnd(Position(current))
            }
        }
"                           )"")
                        )
                        ($ ; terminals
                            (if (optional? content) "" ($
"        mutable start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))": Position = lexer.getPreviousPosition()
"                           ))
"        let "(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))": bool"))(("literal") "literal: Literal?")(else ($ (property content)": String?")))
            " = lexer.parse"
            (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))")
        if "(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))(("identifier") ($ "("(property content)" != null) && isIdentifier("(property content)")")) (else ($ (property content)" != null")))"
            lexer.advance()
"                           (if (optional? content) "" ($
"        else
            throw "
        (case (type content) (("keyword") "keyword") (("punctuation") "punctuation")(("identifier") "identifier")(("literal") "literal")(("prefixoperator" "binaryoperator" "postfixoperator") "operator"))
        "Expected(Position(start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))(case (type content) (("keyword" "punctuation") ($ "), String("((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ")"))")

"                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"        mutable end: Position = lexer.getPosition()
        var ret: "(id syntax)" = "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "Position(start), Position(end))
"                (if (top? syntax) ($
"        ret.fileName = fileName
"               )"")
                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"        if "(property content)" != null {
            for item: "(link content)" in "(property content)"
                item.parent = ret
        }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"       if "(property content)" != null
    "                       )"")
"        "(property content)".parent = ret
"                       )"")
                    )
                ))
"
        return ret
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
"
    function isAtEnd() -> bool {
        return lexer.isAtEnd()
    }

    function isIdentifier(id: String) -> bool {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
        if id.equals("(name keyword)")
            return false
"   )))
"
        return true
    }

    mutable lexer: Lexer
    let fileName: String

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    let "(name keyword)": String
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    let "(id punctuation)": String
"   )))
"}
"

))
