(define (parser-cs) ($
"using System;
using System.Collections.Generic;
using System.Text;

namespace scalyc
{
    class Parser
    {
        Lexer lexer;
        string fileName;

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        readonly string "(name keyword)" = \""(id keyword)"\";
"   )))
"
"   (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"        readonly string "(id punctuation)" = \""(value punctuation)"\";
"   )))
"
        public Parser(string theFileName, string text)
        {
            lexer = new Lexer(text);
            fileName = theFileName;
        }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
        public "(id syntax)"Syntax[] parse"(id syntax)"List()
        {
            List<"(id syntax)"Syntax> ret = null;
            while (true)
            {
                "(id syntax)"Syntax node = parse"(id syntax)"();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<"(id syntax)"Syntax>();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }
"       )"")
"
        public "(id syntax)"Syntax parse"(id syntax)"()
        {"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
            {
                "(link content)"Syntax node = parse"(link content)"();
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
"
            Position start = lexer.getPreviousPosition();
"                (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"
            "(link content)"Syntax"(if (multiple? content) "[]" "")" "(property content)" = parse"(link content)(if (multiple? content) "List" "")"();
"
                    (if (optional? content) "" ($
"            if ("(property content)" == null)
"
                        (if (equal? 1 (child-number content))
                            ($
"                return null;
"                           )
                            ($
"                throw new ParserException(fileName, lexer.line, lexer.column);
"                           )
                        )
                    ))
                           (if (and (top? syntax) (last-sibling? content)) ($
"            if ("(property content)" != null)
            {
                if (!isAtEnd())
                {
                    Position errorPos = lexer.getPreviousPosition();
                    throw new ParserException(fileName, errorPos.line, errorPos.column);
                }
            }
"                           )"")
                        )
                        ($ ; terminals
"
            "(case (type content)
                (("keyword" "punctuation") ($ "bool success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))
                (("literal") ($ "Literal "(property content)))
                (else ($ "string "(property content)))
            )
            " = lexer.parse"
            (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))");
            if ("(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))(("identifier") ($ "("(property content)" != null) && isIdentifier("(property content)")")) (else ($ (property content)" != null")))")
                lexer.advance();
"                           (if (optional? content) "" ($
"            else
"                               (if (equal? 1 (child-number content))
                                    ($
"                return null;
"                                   )
                                    ($
"                throw new ParserException(fileName, lexer.line, lexer.column);
"                                    )
                                )
                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"
            Position end = lexer.getPosition();

            "(id syntax)"Syntax ret = new "(id syntax)"Syntax(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "(property content)
                )))
                ");

"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"            if ("(property content)" != null)
            {
                foreach ("(link content)"Syntax item in "(property content)")
                    item.parent = ret;
            }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"            if ("(property content)" != null)
    "                       )"")
"            "(property content)".parent = ret;
"                       )"")
                    )
                ))
"
            return ret;
"
            ) ; $
        ) ; abstract or not
"        }
"   ))))
"
        bool isAtEnd()
        {
            return lexer.isAtEnd();
        }

        bool isIdentifier(string id)
        {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
            if (id == "(name keyword)")
                return false;
"   )))
"
            return true;
        }
    }
"

))

(define (parser) ($
"class scalyc.Parser(

    fileName: string,
    text: string

    ) {

    mutable lexer: Lexer new Lexer(text)

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    let "(name keyword)": string = new string(\""(id keyword)"\")
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    let "(id punctuation)": string = new string(\""(value punctuation)"\")
"   )))

    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    method parse"(id syntax)"List(): Array["(id syntax)"] {
        mutable ret: Array["(id syntax)"]$ = null
        while (true) {

            let node: "(id syntax)" parse"(id syntax)"()
            if (node = ()) {
                break }

            if (ret = ()) {
                ret = new Array["(id syntax)"]() }

            ret.add(node)
        }

        ret
    }
"       )"")
"
    function parse"(id syntax)"(): "(id syntax)(if (top? syntax) " throws ParserError" "")" {"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node: "(link content)" = parse"(link content)"()
            if (node <""> ()) {
                return node }
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
        let "(property content)": "(if (multiple? content) "Array[" "")(link content)(if (multiple? content) "]" "")" = parse"(link content)(if (multiple? content) "List" "")"()
"
                    (if (or (optional? content) (multiple? content)) "" ($
"        if ("(property content)" = ()) {
            return () }
"
                    ))
                           (if (top? syntax) ($
"        if ("(property content)" <""> null) {
            if (isAtEnd() = false) {
                let errorPos: Position$ = lexer.getPreviousPosition()
                throw syntax(errorPos.line, errorPos.column)
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
        if ("(case (type content) (("keyword" "punctuation") ($ "success"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))))(("identifier") ($ "("(property content)" <""> ()) && isIdentifier("(property content)")")) (else ($ (property content)" <""> ()")))") {
            lexer.advance() }
"                           (if (optional? content) "" ($
"        else {
            return () }
"
                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"
        let end: Position$ lexer.getPosition()

        let ret: "(id syntax)" new "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "new Position(start), new Position(end))
"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"        if ("(property content)" <""> ()) {
            for (item: "(link content)" in "(property content)") {
                item.parent = ret }
        }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"        if ("(property content)" <""> ()) {
    "                       )"")
"        "(property content)".parent = ret"(if (optional? content) " }" "")"
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
        if (id.equals("(name keyword)")) {
            return false }
"   )))
"
        true
    }
}
"

))
