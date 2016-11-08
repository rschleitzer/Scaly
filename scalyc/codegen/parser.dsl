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
"    "(name keyword)" = new(getPage()) String(\""(id keyword)"\");
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    "(id punctuation)" = new(getPage()) String(\""(value punctuation)"\");
"   )))
"}
"    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
_Result<""_Vector<"(id syntax)">, ParserError> Parser::parse"(id syntax)"List(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
    _Array<"(id syntax)">* ret = nullptr;
    while (true) {
        auto _node_result = parse"(id syntax)"(_rp, _ep);
        "(id syntax)"* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            break;
        }
        if (ret == nullptr)
            ret = new(_p) _Array<"(id syntax)">();
        ret->push(node);
    }
    return _Result<""_Vector<"(id syntax)">, ParserError>(ret ? &_Vector<"(id syntax)">::create(_rp, *ret) : 0);
}
"       )"")
"
_Result<"(id syntax)", ParserError> Parser::parse"(id syntax)"(_Page* _rp, _Page* _ep) {
    _Region _region; _Page* _p = _region.get();
"
        (if (abstract? syntax)
            ($
"    _Array<""ParserError>* errors = new(_p) _Array<""ParserError>();
    Position* start = lexer->getPreviousPosition(_p);
"                (apply-to-children-of syntax (lambda (content) ($
"    {
        auto _node_result = parse"(link content)"(_rp, _ep);
        "(link content)"* node = nullptr;
        if (_node_result.succeeded()) {
            node = _node_result.getResult();
        }
        else {
            auto error = _node_result.getError();
            {
                errors->push(error);
            }
        }
        if (node != nullptr)
            return _Result<"(id syntax)", ParserError>(node);
    }
"
                )))
"    return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _ParserError_unableToParse(new(_ep) Position(start), &_Vector<""ParserError>::create(_ep, *(errors)))));
"
            )
            ($ ; non-abstract syntax
"    Position* start = lexer->getPreviousPosition(_p);
"               (apply-to-children-of syntax (lambda (content) ($
                   (if (string=? "syntax" (type content))
                        ($ ; non-terminals
"    auto _"(property content)"_result = parse"(link content)(if (multiple? content) "List" "")"(_rp, _ep);
    "(if (string=? "syntax" (type content)) ($ (if (multiple? content) "_Vector<" "")(link content)(if (multiple? content) ">" "") "* ") "")(property content)" = nullptr;
    if (_"(property content)"_result.succeeded()) {
        "(property content)" = _"(property content)"_result.getResult();
    }
    else {
"                            (if (optional? content)
                                ($
"        "(property content)" = nullptr;
"                               )
                                ($
"        auto error = _"(property content)"_result.getError();
        return _Result<"(id syntax)", ParserError>(error);
"                               )
                            )
"    }
"
                            (if (top? syntax) ($
"    if ("(property content)" != nullptr) {
        if (!isAtEnd()) {
            _Region _region; _Page* _p = _region.get();
            Position* current = lexer->getPosition(_p);
            return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _ParserError_notAtEnd(new(_ep) Position(current))));
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
    if "(case (type content) (("keyword" "punctuation") ($ "("($ "success"(string-firstchar-upcase (link content))(number->string (child-number content)))")"))(("identifier") ($ "(("(property content)" != nullptr) && isIdentifier("(property content)"))")) (else ($"("(property content)" != nullptr)")))"
        lexer->advance();
"                            (if (optional? content) "" ($
"    else
        return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(new(_ep) _ParserError"
        (case (type content) (("keyword") "_keyword") (("punctuation") "_punctuation")(("identifier") "_identifier")(("literal") "_literal")(("prefixoperator" "binaryoperator" "postfixoperator") "_operator"))
        "Expected(new(_ep) Position(start"(if (property content) (string-firstchar-upcase (property content)) ($ (string-firstchar-upcase (link content))(number->string (child-number content))))(case (type content) (("keyword" "punctuation") ($ "), new(_ep) String("((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ")"))")));
"                          ))
                        )
                    ) ; syntax or terminal
                ))) ; apply to children of syntax
"    Position* end = lexer->getPosition(_p);
    "(id syntax)"* ret = new(_rp) "(id syntax)"("
                (apply-to-property-children-of syntax (lambda (content) ($
                    (property content)(if (properties-remaining? content syntax) ", " "")
                )))
                (if (node-list-empty? (properties syntax)) "" ", ")
                "new(_rp) Position(start), new(_rp) Position(end));
"                (if (top? syntax) ($
"    ret->fileName = fileName;
"               )"")
                (apply-to-property-children-of syntax (lambda (content)
                    (if (multiple? content)
                        ($
"    if ("(property content)" != nullptr) {
        "(link content)"* item = nullptr;
        size_t _"(property content)"_length = "(property content)"->length();
        for (size_t _i = 0; _i < _"(property content)"_length; _i++) {
            item = *(*"(property content)")[_i];
            item->parent = ret;
        }
    }
"                       )
                        (if (string=? "syntax" (type content)) ($
                            (if (optional? content) ($
"    if ("(property content)" != nullptr)
    "                       )"")
"    "(property content)"->parent = ret;
"                       )"")
                    )
                ))
"    return _Result<"(id syntax)", ParserError>(ret);
"
            ) ; $
        ) ; abstract or not
"}
"   ))))
"
bool Parser::isAtEnd() {
    return lexer->isAtEnd();
}

bool Parser::isIdentifier(String* id) {
"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"    if (id->equals("(name keyword)"))
        return false;
"   )))
"    return true;
}


}
"

))
