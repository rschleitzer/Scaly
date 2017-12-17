(define (parser) ($
"using System;
using System.Collections.Generic;
using System.Text;

namespace scalysh
{
    class Parser
    {
        Lexer lexer;
        string fileName;

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        string "(name keyword)" = \""(id keyword)"\";
"   )))
"
"   (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"        string "(id punctuation)" = \""(value punctuation)"\";
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
        public "(id syntax)"[] parse"(id syntax)"List()
        {
            List<"(id syntax)"> ret = null;
            while (true)
            {
                "(id syntax)" node = parse"(id syntax)"();
                if (node == null)
                    break;

                if (ret == null)
                    ret = new List<"(id syntax)">();

                ret.Add(node);
            }

            if (ret != null)
                return ret.ToArray();
            else
                return null;
        }
"       )"")
"
        public "(id syntax)" parse"(id syntax)"()
        {"
        (if (abstract? syntax)
            ($
                (apply-to-children-of syntax (lambda (content) ($
"
            {
                "(link content)" node = parse"(link content)"();
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
            "(link content)(if (multiple? content) "[]" "")" "(property content)" = parse"(link content)(if (multiple? content) "List" "")"();
"
                    (if (or (optional? content) (multiple? content)) "" ($
"            if ("(property content)" == null)
                return null;
"
                    ))
                           (if (top? syntax) ($
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
                return null;
"
                           ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"
            Position end = lexer.getPosition();

            "(id syntax)" ret = new "(id syntax)"(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "(property content)
                )))
                ");

"                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"            if ("(property content)" != null)
            {
                foreach ("(link content)" item in "(property content)")
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
