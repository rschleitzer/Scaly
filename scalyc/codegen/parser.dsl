(define (parser) ($
        (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"class "(id syntax-node)"
"
        )))
"
class Parser {
    constructor(theFileName: string, text: string) {
        lexer = new Lexer(text)
        fileName = theFileName
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        "(name keyword)" = new string(\""(id keyword)"\")
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"        "(id punctuation)" = new string(\""(value punctuation)"\")
"   )))
"    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    function parse"(id syntax)"List(): "(id syntax)"[] {
        mutable ret: "(id syntax)"[]$ = null
        while true {

            let node: "(id syntax)" = parse"(id syntax)"()
            if node == null
                break

            if ret == null
                ret = new "(id syntax)"[]()

            ret.push(node)
        }

        return(ret)
    }
"       )"")
"
    function parse"(id syntax)"(): "(id syntax)" {"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node: "(link content)" = parse"(link content)"()
            if node != null
                return(node)
        }
"
                )))
"
        null
"
            )
            ($ ; non-abstract syntax
"
        let start: Position$ = lexer.getPreviousPosition()
"                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
        let "(property content)": "(link content)(if (multiple? content) "[]" "")" = parse"(link content)(if (multiple? content) "List" "")"()
"
                    (if (or (optional? content) (multiple? content)) "" ($
"        if "(property content)" == null
            return(null)
"
                    ))
                           (if (top? syntax) ($
"        if "(property content)" != null {
            if !isAtEnd() {
                return(null)
            }
        }
"                           )"")
                        )
                        ($ ; terminals
"
        let "(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))": bool"))(("literal") ($ (property content)": Literal"))(else ($ (property content)": string")))
            " = lexer.parse"
            (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))")
        if "(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))(("identifier") ($ "("(property content)" != null) && isIdentifier("(property content)")")) (else ($ (property content)" != null")))"
            lexer.advance()
"                           (if (optional? content) "" ($
"        else
            return(null)
"
                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"
        let end: Position$ = lexer.getPosition()

        let ret: "(id syntax)" = new "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "new Position(start), new Position(end))
"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"        if "(property content)" != null {
            for item: "(link content)" in "(property content)"
                item.parent = ret
        }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"        if "(property content)" != null
    "                       )"")
"        "(property content)".parent = ret
"                       )"")
                    )
                ))
"
        ret
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
"
    function isAtEnd(): bool {
        lexer.isAtEnd()
    }

    function isIdentifier(id: string): bool {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
        if id.equals("(name keyword)")
            return(false)
"   )))
"
        return(true)
    }

    mutable lexer: Lexer
    let fileName: string

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    let "(name keyword)": string
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    let "(id punctuation)": string
"   )))
"}
"

))
