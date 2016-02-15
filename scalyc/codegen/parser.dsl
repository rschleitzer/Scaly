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
        mutable ret: ["(id syntax)"]? = null

        while true {
            let node: "(id syntax)" = parse"(id syntax)"()
                catch _ break
            
            if ret == null
                ret = ["(id syntax)"]
                
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
        mutable ret: "(id syntax)"? = null
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
            if ret == null
                ret = "(id syntax)"(start, lexer.getPosition())
"                   (if (property content) ($
"
            ret."(property content)" = "(if (string=? "syntax" (type content)) "node" (property content))"
"                   )"")
"        }
"                ))) ; apply to children of syntax
"
"               (if (top? syntax)
"        ret.fileName = fileName;
"
                "")
"        return ret
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
    Parser(_LetString* fileName, _LetString& text);
"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
"
"        (if (multiple? syntax) ($
"    _Result<""_Array<"(id syntax)">, ParserError> parse"(id syntax)"List(_Page* _rp, _Page* _ep);
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
"    const char* "(name keyword)" = \""(id keyword)"\";
"   )))
    (apply-to-selected-children "punctuation" (lambda (punctuation) ($
"    const char* "(id punctuation)" = \""(value punctuation)"\";
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

Parser::Parser(_LetString* fileName, _LetString& text)
: lexer(text), fileName(fileName) {
}
"    (apply-to-selected-children "syntax" (lambda (syntax) (if (program? syntax) "" ($
        (if (multiple? syntax) ($
"
_Result<""_Array<"(id syntax)">, ParserError> Parser::parse"(id syntax)"List(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    _Array<"(id syntax)">* ret = 0;

    while (true) {
        _Result<"(id syntax)", ParserError> nodeResult = parse"(id syntax)"(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) _Array<"(id syntax)">();

            ret->push(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<""_Array<"(id syntax)">, ParserError>(ret);
}
"       )"")
"
_Result<"(id syntax)", ParserError> Parser::parse"(id syntax)"(_Page* _rp, _Page* _ep) {
"
        (if (abstract? syntax)
            ($
"    _Array<""ParserError>& errors = *new(_ep) _Array<""ParserError>();
    Position start = lexer.getPreviousPosition();
"                (apply-to-children-of syntax (lambda (content) ($
"
    {
        // Make a region for the current block and get the Page
        _Region _r; _Page* _p = _r.get();
        _Result<"(link content)", ParserError> result = parse"(link content)"(_rp, _p);
        if (result.succeeded()) 
            return _Result<"(id syntax)", ParserError>(result.getResult());
        else
            errors.push(result.getError());
    }
"
                )))
"
    return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
"
            )
            ($ ; non-abstract syntax
"    "(id syntax)"* ret = 0;
"                (apply-to-children-of syntax (lambda (content) ($
"    {
        Position start = lexer.getPreviousPosition();
"                   (if (string=? "syntax" (type content))

                        ($ ; non-terminals
"        _Result<"(if (multiple? content) "_Array<" "")(link content)(if (multiple? content) ">" "")", ParserError> result = parse"(link content)(if (multiple? content) "List" "")"(_rp, _ep);
        if (result.succeeded()) {
"                           (if (top? syntax) ($
"            if (!isAtEnd()) {
                Position current = lexer.getPosition();
                return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(*new(_ep) NotAtEnd(current)));
            }
"                           )"")
                        )
                        ($ ; terminals
"        "(case (type content) (("keyword" "punctuation") "bool success")(("literal") "Literal* literal")(else ($ "_LetString* "(property content))))
        " = lexer.parse"
        (case (type content)(("prefixoperator") "PrefixOperator")(("binaryoperator") "BinaryOperator")(("postfixoperator") "PostfixOperator")(("identifier") "Identifier")(("literal") "Literal")(("keyword") "Keyword")(("punctuation") "Punctuation"))                            
        "("(case (type content)(("keyword") (name-of-link content)) (("punctuation") (link content)) (else "_rp"))");
        if "(case (type content) (("keyword" "punctuation") "(success)")(("identifier") ($ "(("(property content)") && (isIdentifier(*"(property content)")))")) (else ($"("(property content)")")))" {
            lexer.advance();
"                       )
                    ) ; syntax or terminal
"            if (!ret)
                ret = new(_rp) "(id syntax)"(start, lexer.getPosition());
"                   (if (property content) ($
"
            ret->"(property content)" = "(if (string=? "syntax" (type content)) "result.getResult()" (property content))";
"                   )"")
"        }
"                   (if (optional? content) "" ($
"        else {
            return _Result<"(id syntax)", ParserError>("(if (string=? (type content) "syntax") "result.getError()" ($ "new(_ep) ParserError(*new(_ep) "
            (case (type content) (("keyword") "Keyword") (("punctuation") "Punctuation")(("identifier") "Identifier")(("literal") "Literal")(("prefixoperator" "binaryoperator" "postfixoperator") "Operator"))
            "Expected(start"(case (type content) (("keyword" "punctuation") ($ ", _LetString::create(_ep, "((if (string=? (type content) "keyword") name-of-link link) content)")"))(else ""))"))"))");
        }
"                   ))
"    }
"                ))) ; apply to children of syntax
                (if (top? syntax) ($
"    ret->fileName = fileName;
"               )"")
"
    return _Result<"(id syntax)", ParserError>(ret);
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
    if (id == "(name keyword)")
        return false;
"   )))
"
    return true;
}

}
"

))
