(define (parser-scaly) ($

"class Parser {
    init(fileName: String, text: String) {
        lexer = Lexer(text)
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
    function parse"(id syntax)"List() -> ["(id syntax)"] throws ParserError {
        mutable (string-firstchar-downcase "(string-firstchar-downcase (id syntax))"): ["(id syntax)"]? = ["(id syntax)"]()
        while true
            "(string-firstchar-downcase (id syntax))".push(parse"(id syntax)"() catch _ break)
        return "(string-firstchar-downcase (id syntax))"
    }
"       )"")
"
    function parse"(id syntax)"() -> "(id syntax)" throws ParserError {"
        (if (abstract? syntax)
            ($
"
        let errors: [ParserError] = [ParserError]()
        let start: Position = lexer.getPreviousPosition()
"                (apply-to-children-of syntax (lambda (content) ($
"
        {
            let node: "(link content)" = parse"(link content)"() catch _(error) {
                errors.push(error)
                break
            }

            return node
        }
"
                )))
"
        throw ParserError.UnableToParse(start, errors)
"
            )
            ($ ; non-abstract syntax
"
        mutable "(string-firstchar-downcase (id syntax))": "(id syntax)"? = null
"                (apply-to-children-of syntax (lambda (content) ($
"        {
            let start: Position = lexer.getPosition()
"                   (if (string=? "syntax" (type content))

                        ($ ; non-terminals
"            let result: "(if (multiple? content) "[" "")(link content)(if (multiple? content) "]" "")" node = parse"(link content)(if (multiple? content) "List" "")"() catch _(error) {
"
                    (if (optional? content)
"                 null
"
                        ($
"                throw ("(if (string=? (type content) "syntax") "error" ($ "ParserError."
                (case (type content) (("keyword") "Keyword") (("punctuation") "Punctuation")(("identifier") "Identifier")(("literal") "Literal")(("prefixoperator" "binaryoperator" "postfixoperator") "Operator"))
                "Expected(start"(case (type content) (("keyword" "punctuation") ($ ", String("((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ""))"))"))")
"                       )
                    )
"            }
"                           (if (top? syntax) ($
"            if !isAtEnd() {
                let current: Position = lexer.getPreviousPosition()
                throw ParserError.NotAtEnd(current)
            }
"                           )"")
                        )
                        ($ ; terminals
"            let "(case (type content) (("keyword" "punctuation") "success: bool")(("literal") "literal: Literal?")(else ($ (property content)": String?")))
            " = lexer.parse"
            (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))")
            if "(case (type content) (("keyword" "punctuation") "success")(("identifier") ($ "("(property content)" != null) && ("(property content)" is Identifier)")) (else ($ (property content)" != null")))"
                lexer.advance()
"                       )
                    ) ; syntax or terminal
"
            if "(string-firstchar-downcase (id syntax))" == null
                "(string-firstchar-downcase (id syntax))" = "(id syntax)"(start, lexer.getPosition())
"                   (if (property content) ($
"
            "(string-firstchar-downcase (id syntax))"."(property content)" = "(if (string=? "syntax" (type content)) "node" (property content))"
"                   )"")
"        }
"                ))) ; apply to children of syntax
"
"               (if (top? syntax) ($
"        "(string-firstchar-downcase (id syntax))".fileName = fileName;
"
                )"")
"        return "(string-firstchar-downcase (id syntax))"
"
            ) ; $
        ) ; abstract or not
"    }
"   ))))
"
    function isAtEnd() -> bool {
        lexer.isAtEnd()
    }

    function isIdentifier(id: String) -> bool {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
        if id == "(name keyword)"
            return false
"   )))
"
        true
    }

    mutable lexer: Lexer

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    let "(name keyword)": string = \""(id keyword)"\"
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    let "(id punctuation)": string = \""(value punctuation)"\"
"   )))
"}
"

))

(define (parser-h) ($

"#ifndef __scalyc__Parser__
#define __scalyc__Parser__
#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

class Parser : public Object {
public:
    Parser(_LetString* fileName, _LetString* text);
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
"
"        (if (multiple? syntax) ($
"    _Result<""_Vector<"(id syntax)">, ParserError> parse"(id syntax)"List(_Page* _rp, _Page* _ep);
"       )"")
"    _Result<"(id syntax)", ParserError> parse"(id syntax)"(_Page* _rp, _Page* _ep);
"   ))))
"
    bool isAtEnd();
    bool isIdentifier(_LetString& id);

private:
    Lexer lexer;
    _LetString* fileName;

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    _LetString* "(name keyword)";
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    _LetString* "(id punctuation)";
"   )))
"};

}

#endif /* defined(__scalyc__Parser__) */
"

))

(define (parser-cpp) ($

"#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

Parser::Parser(_LetString* fileName, _LetString* text)
: lexer(text), fileName(fileName)"
    (apply-to-selected-children "keyword" (lambda (keyword) ($
",
  "(name keyword)"(&_LetString::create(getPage(), \""(id keyword)"\"))"
    )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
",
  "(id punctuation)"(&_LetString::create(getPage(), \""(value punctuation)"\"))"
    )))
" {}
"    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
_Result<""_Vector<"(id syntax)">, ParserError> Parser::parse"(id syntax)"List(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<"(id syntax)">* "(string-firstchar-downcase (id syntax))" = 0;
    while (true) {
        _Result<"(id syntax)", ParserError> nodeResult = parse"(id syntax)"(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!"(string-firstchar-downcase (id syntax))")
                "(string-firstchar-downcase (id syntax))" = new(_p) _Array<"(id syntax)">();
            "(string-firstchar-downcase (id syntax))"->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }
    return _Result<""_Vector<"(id syntax)">, ParserError>("(string-firstchar-downcase (id syntax))" ? &_Vector<"(id syntax)">::create(_rp, *"(string-firstchar-downcase (id syntax))") : 0);
}
"       )"")
"
_Result<"(id syntax)", ParserError> Parser::parse"(id syntax)"(_Page* _rp, _Page* _ep) {
"
        (if (abstract? syntax)
            ($
"    _Array<""ParserError>* errors = new(_ep) _Array<""ParserError>();
    Position* start = lexer.getPreviousPosition(_rp);
"                (apply-to-children-of syntax (lambda (content) ($
"    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<"(link content)", ParserError> result = parse"(link content)"(_rp, _p);
        if (result.succeeded())
            return _Result<"(id syntax)", ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
"
                )))
"    return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _unableToParse(start, errors)));
"
            )
            ($ ; non-abstract syntax
"    Position* start = lexer.getPreviousPosition(_rp);
"               (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"    _Result<"(if (multiple? content) "_Vector<" "")(link content)(if (multiple? content) ">" "")", ParserError> _result_"(property content)" = parse"(link content)(if (multiple? content) "List" "")"(_rp, _ep);
"                   (if (property content) ($
"    "(if (string=? "syntax" (type content)) ($ (if (multiple? content) "_Vector<" "")(link content)(if (multiple? content) ">" "") "* ") "")(property content)" = 0;
"                  )"")
"    if (_result_"(property content)".succeeded()) {
"                           (if (top? syntax) ($
"        if (!isAtEnd()) {
            Position* current = lexer.getPosition(_ep);
            return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _notAtEnd(current)));
        }
"                           )"")
                            (if (property content) ($
"        "(property content)" = "(if (string=? "syntax" (type content)) ($ "_result_"(property content)".getResult()") (property content))";
"                           )"")
                        )
                        ($ ; terminals
                            (if (optional? content) "" ($
"    Position* start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))" = lexer.getPreviousPosition(_rp);
"                           ))
"    "(case (type content) (("keyword" "punctuation") ($ "bool success"(string-firstchar-upcase (link content))(number->string (child-number content))))(("literal") "Literal* literal")(else ($ "_LetString* "(property content))))
        " = lexer.parse"
        (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
        "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else "_rp"))");
    if "(case (type content) (("keyword" "punctuation") ($ "("($ "success"(string-firstchar-upcase (link content))(number->string (child-number content)))")"))(("identifier") ($ "(("(property content)") && (isIdentifier(*"(property content)")))")) (else ($"("(property content)")")))" {
        lexer.advance();
"                       )
                    ) ; syntax or terminal
"    }
"                   (if (optional? content) "" ($
"    else {
        return _Result<"(id syntax)", ParserError>("(if (string=? (type content) "syntax") ($ "_result_"(property content)".getError()") ($ "new(_ep) ParserError(new(_ep) "
        (case (type content) (("keyword") "_keyword") (("punctuation") "_punctuation")(("identifier") "_identifier")(("literal") "_literal")(("prefixoperator" "binaryoperator" "postfixoperator") "_operator"))
        "Expected(start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))(case (type content) (("keyword" "punctuation") ($ ", &_LetString::create(_ep, *"((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ""))"))"))");
    }
"                   ))
                ))) ; apply to children of syntax
"    "(id syntax)"* "(string-firstchar-downcase (id syntax))" = new(_rp) "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "start, lexer.getPosition(_rp));
"                (if (top? syntax) ($
"    "(string-firstchar-downcase (id syntax))"->fileName = fileName;
"               )"")
"    return _Result<"(id syntax)", ParserError>("(string-firstchar-downcase (id syntax))");
"
            ) ; $
        ) ; abstract or not
"}
"   ))))
"
bool Parser::isAtEnd() {
    return lexer.isAtEnd();
}

bool Parser::isIdentifier(_LetString& id) {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
    if (id == *"(name keyword)")
        return false;
"   )))
"
    return true;
}

}
"

))
