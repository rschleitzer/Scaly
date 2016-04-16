(define (parser-scaly) ($

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
        mutable "(string-firstchar-downcase (string-firstchar-downcase (id syntax)))": ["(id syntax)"]? = ["(id syntax)"]()
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
        mutable start: Position = lexer.getPreviousPosition()
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
        throw ParserError.UnableToParse(Position(start), errors)
"
            )
            ($ ; non-abstract syntax
"
"                (apply-to-children-of syntax (lambda (content) ($
"        mutable start: Position = lexer.getPreviousPosition()
"                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"        let "(property content)": "(if (multiple? content) "[" "")(link content)(if (multiple? content) "]" "")" = parse"(link content)(if (multiple? content) "List" "")"() catch _(error)"
                    (if (optional? content)
                        " null
"                       ($
"
            throw ("(if (string=? (type content) "syntax") "error" ($ "ParserError."
                (case (type content) (("keyword") "Keyword") (("punctuation") "Punctuation")(("identifier") "Identifier")(("literal") "Literal")(("prefixoperator" "binaryoperator" "postfixoperator") "Operator"))
                "Expected(start"(case (type content) (("keyword" "punctuation") ($ ", String("((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ""))"))"))")
"                       )
                    )
                           (if (top? syntax) ($
"        if "(property content)" != null {
            if !isAtEnd() {
                let current: Position = lexer.getPosition()
                throw notAtEnd(current)
            }
        }
"                           )"")
                        )
                        ($ ; terminals
"        let "(case (type content) (("keyword" "punctuation") "success: bool")(("literal") "literal: Literal?")(else ($ (property content)": String?")))
            " = lexer.parse"
            (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
            "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else ""))")
        if "(case (type content) (("keyword" "punctuation") "success")(("identifier") ($ "("(property content)" != null) && ("(property content)" is Identifier)")) (else ($ (property content)" != null")))"
            lexer.advance()
"                       )
                    ) ; syntax or terminal
"
        if "(string-firstchar-downcase (id syntax))" == null
            "(string-firstchar-downcase (id syntax))" = "(id syntax)"(Position(start), lexer.getPosition())
"                   (if (property content) ($
"
        "(string-firstchar-downcase (id syntax))"."(property content)" = "(if (string=? "syntax" (type content)) "node" (property content))"
"                   )"")
                ))) ; apply to children of syntax
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
        if id.equals("(name keyword)")
            return false
"   )))
"
        true
    }

    mutable lexer: Lexer
    let fileName: String

"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    let "(name keyword)": String = \""(id keyword)"\"
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    let "(id punctuation)": String = \""(value punctuation)"\"
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
    Parser(String* theFileName, String* text);
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"    virtual _Result<""_Vector<"(id syntax)">, ParserError> parse"(id syntax)"List(_Page* _rp, _Page* _ep);
"       )"")
"    virtual _Result<"(id syntax)", ParserError> parse"(id syntax)"(_Page* _rp, _Page* _ep);
"   ))))
"    virtual bool isAtEnd();
    virtual bool isIdentifier(String* id);
    Lexer* lexer;
    String* fileName;
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    String* "(name keyword)";
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    String* "(id punctuation)";
"   )))
"
};

}
#endif // __scalyc__Parser__
"

))

(define (parser-cpp) ($

"#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

Parser::Parser(String* theFileName, String* text) {
    lexer = new(getPage()->allocateExclusivePage()) Lexer(text);
    fileName = theFileName;
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    "(name keyword)" = &""String::create(getPage(), \""(id keyword)"\");
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    "(id punctuation)" = &""String::create(getPage(), \""(value punctuation)"\");
"   )))
"}
"    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
_Result<""_Vector<"(id syntax)">, ParserError> Parser::parse"(id syntax)"List(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
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
    _Region _region; _Page* _p = _region.get();
"
        (if (abstract? syntax)
            ($
"    _Array<""ParserError>* errors = new(_ep) _Array<""ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
"                (apply-to-children-of syntax (lambda (content) ($
"    {
        _Region _region; _Page* _p = _region.get();
        _Result<"(link content)", ParserError> result = parse"(link content)"(_rp, _p);
        if (result.succeeded())
            return _Result<"(id syntax)", ParserError>(result.getResult());
        else
            errors->push(result.getError());
    }
"
                )))
"    return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<""ParserError>::create(_ep, *errors))));
"
            )
            ($ ; non-abstract syntax
"    Position* start = lexer->getPreviousPosition(_p);
"               (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"    auto _"(property content)"_result = parse"(link content)(if (multiple? content) "List" "")"(_rp, _ep);
    "(if (string=? "syntax" (type content)) ($ (if (multiple? content) "_Vector<" "")(link content)(if (multiple? content) ">" "") "* ") "")(property content)";
    if (_"(property content)"_result.succeeded())
        "(property content)" = _"(property content)"_result.getResult();
    else
"                            (if (optional? content)
                                ($
"        "(property content)" = nullptr;
"                               )
                                ($
"        return _Result<"(id syntax)", ParserError>(_"(property content)"_result.getError());
"                               )
                            )
                            (if (top? syntax) ($
"    if ("(property content)" != nullptr) {
        if (!isAtEnd()) {
            Position* current = lexer->getPosition(_ep);
            return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _ParserError_notAtEnd(current)));
        }
    }
"                           )"")
                        )
                        ($ ; terminals
                            (if (optional? content) "" ($
"    Position* start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))" = lexer->getPreviousPosition(_p);
"                           ))
"    "(case (type content) (("keyword" "punctuation") ($ "bool success"(string-firstchar-upcase (link content))(number->string (child-number content))))(("literal") "Literal* literal")(else ($ "String* "(property content))))
        " = lexer->parse"
        (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))
        "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else "_rp"))");
    if "(case (type content) (("keyword" "punctuation") ($ "("($ "success"(string-firstchar-upcase (link content))(number->string (child-number content)))")"))(("identifier") ($ "(("(property content)") && (isIdentifier("(property content)")))")) (else ($"("(property content)")")))" {
        lexer->advance();
    }
"                            (if (optional? content) "" ($
"    else {
        return _Result<"(id syntax)", ParserError>("(if (string=? (type content) "syntax") ($ "_"(property content)"_result.getError()") ($ "new(_ep) ParserError(new(_ep) _ParserError"
        (case (type content) (("keyword") "_keyword") (("punctuation") "_punctuation")(("identifier") "_identifier")(("literal") "_literal")(("prefixoperator" "binaryoperator" "postfixoperator") "_operator"))
        "Expected(new(_ep) Position(start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))(case (type content) (("keyword" "punctuation") ($ "), &""String::create(_ep, "((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ")"))"))"))");
    }
"                          ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"    "(id syntax)"* "(string-firstchar-downcase (id syntax))" = new(_rp) "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "new(_rp) Position(start), lexer->getPosition(_rp));
"                (if (top? syntax) ($
"    "(string-firstchar-downcase (id syntax))"->fileName = fileName;
"               )"")
                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"    if ("(property content)") {
        size_t _"(property content)"_length = "(property content)"->length();
        for (size_t _i = 0; _i < _"(property content)"_length; _i++)
            (*(*"(property content)")[_i])->parent = "(string-firstchar-downcase (id syntax))";
    }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"    if ("(property content)")
    "                       )"")
"    "(property content)"->parent = "(string-firstchar-downcase (id syntax))";
"                       )"")
                    )
                ))
"    return _Result<"(id syntax)", ParserError>("(string-firstchar-downcase (id syntax))");
"
            ) ; $
        ) ; abstract or not
"}
"   ))))
"
bool Parser::isAtEnd() {
    return lexer->isAtEnd();
}

bool Parser::isIdentifier(String* id) {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
    if (id->equals("(name keyword)"))
        return false;
"   )))
"
    return true;
}

}
"

))
