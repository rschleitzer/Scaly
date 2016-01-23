(define (parser-h) ($

"#ifndef __scalyc__Parser__
#define __scalyc__Parser__
#include \"scalyc.h\"
using namespace scaly;
namespace scalyc {

class Parser : public Object {
public:
    Parser(String& text);
"
    (apply-to-selected-children "syntax" (lambda (syntax) ($
"
"        (if (multiple? syntax) ($
"    _Result<""Array<"(id syntax)">, ParserError> parse"(id syntax)"List(_Page* _rp, _Page* _ep);
"       )"")
"    _Result<"(id syntax)", ParserError> parse"(id syntax)"(_Page* _rp, _Page* _ep);
"   )))
"
    bool isAtEnd();
    bool isIdentifier(String& id);

private:
    Lexer lexer;

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

Parser::Parser(String& text)
: lexer(text) {
}
"
    (apply-to-selected-children "syntax" (lambda (syntax) ($
        (if (multiple? syntax) ($
"
_Result<""Array<"(id syntax)">, ParserError> Parser::parse"(id syntax)"List(_Page* _rp, _Page* _ep) {
    // Make a region for the current block and get the Page
    _Region _r; _Page* _p = _r.get();
    Array<"(id syntax)">* ret = 0;

    while (true) {
        _Result<"(id syntax)", ParserError> nodeResult = parse"(id syntax)"(_rp, _p);
        if (nodeResult.succeeded()) {
            if (!ret)
                ret = new(_rp) Array<"(id syntax)">();

            ret->append(nodeResult.getResult());
        }
        else {
            break;
        }
    }

    return _Result<""Array<"(id syntax)">, ParserError>(ret);
}
"       )"")
"
_Result<"(id syntax)", ParserError> Parser::parse"(id syntax)"(_Page* _rp, _Page* _ep) {
"
        (if (abstract? syntax)
            ($
"    Array<""ParserError>& errors = *new(_ep) Array<""ParserError>();
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
            errors.append(result.getError());
    }
"
                )))
"
    return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(*new(_ep) UnableToParse(start, errors)));
"
            )
            ($
"    "(id syntax)"* ret = 0;
"                (apply-to-children-of syntax (lambda (content) ($
"    {
        Position start = lexer.getPreviousPosition();
"                    (case (type content)
                        (("syntax") ($
"
        _Result<"(if (multiple? content) "Array<" "")(link content)(if (multiple? content) ">" "")", ParserError> result = parse"(link content)(if (multiple? content) "List" "")"(_rp, _ep);
        if (result.succeeded()) {
"
                            (if (top? syntax) ($
"            if (!isAtEnd()) {
                Position current = lexer.getPreviousPosition();
                return _Result<"(id syntax)", ParserError>(new(_ep) ParserError(*new(_ep) NotAtEnd(current)));
            }
"                           )"")
"            if (!ret)
                ret = new(_rp) "(id syntax)"(start, lexer.getPosition());

            ret->"(property content)" = result.getResult();
        }
"                       ))
                        (("keyword" "punctuation") ($
"        if (lexer.parse"(if (string=? (type content) "keyword") "Keyword" "Punctuation")"("((if (string=? (type content) "keyword") name-of-link link) content)")) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) "(id syntax)"(start, lexer.getPosition());
"
                            (if (property content) ($
"
            ret->"(property content)" = true;"
                            )"")
"        }
"                       ))
                        (("operator" "prefixoperator" "binaryoperator" "postfixoperator") ($
"
        String* "(property content)" = lexer.parse"(case (type content) (("prefixoperator") "Prefix") (("binaryoperator") "Binary") (("postfixoperator") "Postfix")(else "") )"Operator(_rp); 
        if ("(property content)") {
            lexer.advance();
            if (!ret)
                ret = new (_rp) "(id syntax)"(start, lexer.getPosition()); 

            ret->"(property content)" = "(property content)";
	    }
"                       ))
                        (("identifier") ($
"
        String* id"(property content)" = lexer.parseIdentifier(_rp);
        if ((id"(property content)") && (isIdentifier(*id"(property content)"))) {
            lexer.advance();
            if (!ret)
                ret = new(_rp) "(id syntax)"(start, lexer.getPosition());

            ret->"(property content)" = id"(property content)";
        }
"                       ))
                        (("literal")    ($
"
        Literal* literal = lexer.parseLiteral(_rp);
        if (literal) {
        	lexer.advance();
            if (!ret)
                ret = new(_rp) "(id syntax)"(start, lexer.getPosition());

            ret->"(property content)" = literal;
        }
"                       ))
                        (else "")
                    ) ; case
                    (if (optional? content) "" ($
"        else {
            return _Result<"(id syntax)", ParserError>("(if (string=? (type content) "syntax") "result.getError()" ($ "new(_ep) ParserError(*new(_ep) "
                        (case (type content) 
                            (("keyword") "Keyword")
                            (("punctuation") "Punctuation")
                            (("identifier") "Identifier")
                            (("literal") "Literal")
                            (("operator" "prefixoperator" "binaryoperator" "postfixoperator") "Operator")
                            (else "")
                        )
                "Expected(start"
                        (case (type content) 
                            (("keyword" "punctuation") ($ ", *new(_ep) String("((if (string=? (type content) "keyword") name-of-link link) content)")"))
                            (else "")
                        )
                "))"))");
        }
"                   ))
"    }
"                ))) ; apply to children of syntax
"
    return _Result<"(id syntax)", ParserError>(ret);
"
            ) ; $
        ) ; abstract or not
"}
"   )))
"
bool Parser::isAtEnd() {
    return lexer.isAtEnd();
}
    
bool Parser::isIdentifier(String& id) {"
   (apply-to-selected-children "keyword" (lambda (keyword) ($
"
    if (id == "(name keyword)")
        return false;
"   )))
"
    return true;
}

}"

))
