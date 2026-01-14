<![CDATA[
;; Generate Scaly Parser from scaly.sgm

;; Helper to generate char constant name from punctuation value
;; Returns function call syntax like ParserConstants.CHAR_LPAREN()
(define (punct-const-name value)
    (case value
        (("(") "ParserConstants.CHAR_LPAREN()")
        ((")") "ParserConstants.CHAR_RPAREN()")
        (("[") "ParserConstants.CHAR_LBRACKET()")
        (("]") "ParserConstants.CHAR_RBRACKET()")
        (("{") "ParserConstants.CHAR_LBRACE()")
        (("}") "ParserConstants.CHAR_RBRACE()")
        ((",") "ParserConstants.CHAR_COMMA()")
        ((".") "ParserConstants.CHAR_DOT()")
        (("!") "ParserConstants.CHAR_EXCL()")
        (("?") "ParserConstants.CHAR_QUESTION()")
        (("#") "ParserConstants.CHAR_HASH()")
        (("$") "ParserConstants.CHAR_DOLLAR()")
        (("^") "ParserConstants.CHAR_CARET()")
        (("\\\\") "ParserConstants.CHAR_BACKSLASH()")
        (else ($ "ParserConstants.CHAR_" value "()"))))

(define (generate-parser-scaly) ($
"
; Lexer import (uses its own char constants)
module lexer

; Character constants for punctuation (char literals use double-quote + type annotation)
define ParserConstants
{
    function CHAR_LPAREN() returns char
        \"(\" as char
    function CHAR_RPAREN() returns char
        \")\" as char
    function CHAR_LBRACKET() returns char
        \"[\" as char
    function CHAR_RBRACKET() returns char
        \"]\" as char
    function CHAR_LBRACE() returns char
        \"{\" as char
    function CHAR_RBRACE() returns char
        \"}\" as char
    function CHAR_COMMA() returns char
        \",\" as char
    function CHAR_DOT() returns char
        \".\" as char
    function CHAR_EXCL() returns char
        \"!\" as char
    function CHAR_QUESTION() returns char
        \"?\" as char
    function CHAR_HASH() returns char
        \"#\" as char
    function CHAR_DOLLAR() returns char
        \"$\" as char
    function CHAR_CARET() returns char
        \"^\" as char
    function CHAR_BACKSLASH() returns char
        \"\\\\\" as char
}

define Parser
(
    lexer: Lexer
    keywords_index: Vector[String]
    keywords: HashSet[String]
)
{
    init#(rp, text: String)
    {
        set lexer: Lexer(text)
        set keywords_index: initialize_keywords_index#()
        set keywords: initialize_keywords#()
    }

    function initialize_keywords_index#(rp, this: Parser) returns Vector[String]
    {
        var keywords_builder Array[String]$()
"   (apply-to-selected-children "keyword" (lambda (keyword) ($
"        keywords_builder.add(String#(\""(id keyword)"\"))
"   )))
"        Vector[String]#(keywords_builder)
    }

    function initialize_keywords#(rp, this: Parser) returns HashSet[String]
    {
        var hash_set_builder HashSetBuilder[String]$(keywords_index)
        HashSet[String]#(hash_set_builder)
    }

    function parse_literal_token#(rp, this: Parser) returns Literal throws ParserError
    {
        choose lexer.token
            when Empty: empty
                lexer.advance#()
            else {}

        choose lexer.token
            when Literal: literal
            {
                let ret literal.value
                lexer.empty()
                return ret
            }
            else
                throw ParserError.Different(DifferentSyntax())
    }
"
    (apply-to-selected-children "syntax" (lambda (syntax) ($
        (if (multiple? syntax) ($
"
    function parse_"(downcase-string (id syntax))"_list#(rp, this: Parser) returns ref[Vector["(id syntax)"Syntax]]? throws ParserError
    {
        var list List["(id syntax)"Syntax]$()
        while true
        {
            choose parse_"(downcase-string (id syntax))"#()
                when Error: err
                {
                    choose err
                        when Invalid: i
                            throw ParserError.Invalid(i)
                        when Different: d
                        {
                            if list.head = null
                                throw ParserError.Different(d)
                            else
                                return Vector["(id syntax)"Syntax]#(list)
                        }
                }
                when Success: node
                    list.add$(node)
        }
    }
"       ) "")
"
    function parse_"(downcase-string (id syntax))"#(rp, this: Parser) returns "(id syntax)"Syntax throws ParserError
    {
"       (if (abstract? syntax)
            ;; Abstract syntax - try each alternative
            ($
                (apply-to-children-of syntax (lambda (content) ($
"        choose parse_"(downcase-string (link content))"#()
            when Error: err
            {
                choose err
                    when Invalid: i
                        throw ParserError.Invalid(i)
                    when Different: d {}
            }
            when Success: node
                return "(id syntax)"Syntax."(link content)"(node)
"
                )))
"        throw ParserError.Different(DifferentSyntax())
"
            )
            ;; Concrete syntax - parse each field
            ($
"        let start lexer.previous_position
"               (apply-to-children-of syntax (lambda (content)
                    (case (type content)
                        (("syntax")
                            (if (property content)
                            (let ((prop (property content))
                                  (type-name (if (multiple? content)
                                                 ($ "ref[Vector[" (link content) "Syntax]]?")
                                                 ($ (link content) "Syntax"))))
                            (if (optional? content)
                                ($
"
        let " prop "_start lexer.position
        var " prop ": ref[" (if (multiple? content) ($ "Vector[" (link content) "Syntax]") ($ (link content) "Syntax")) "]? null
        choose parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"#()
            when Error: err
            {
                choose err
                    when Different: d {}
                    when Invalid: i
                        throw ParserError.Invalid(i)
            }
            when Success: success
                set " prop ": success
"
                                )
                                ;; non-optional syntax
                                ($
"
        let " prop "_start lexer.position
        var " prop ": " type-name "
        choose parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"#()
            when Error: err
"
                                    (if (equal? 1 (child-number content))
                                        "                throw err
"
                                        ($
"            {
                choose err
                    when Different: d
                        throw ParserError.Invalid(InvalidSyntax(" prop "_start, lexer.position, String#(\"expected "(link content)"\")))
                    when Invalid: i
                        throw ParserError.Invalid(i)
            }
"
                                        )
                                    )
"            when Success: success
                set " prop ": success
"
                                )
                            )) "")  ; close (if optional), (let), else "", close (if property)
                        )
                        (("literal")
                            (let ((prop (property content)))
                            ($
"
        var " prop ": Literal
        choose parse_literal_token#()
            when Error: err
                throw err
            when Success: success
                set " prop ": success
"
                            ))
                        )
                        (("keyword")
                            ($
"
        if ~lexer.parse_keyword#(String#(\""(id (element-with-id (link content)))"\"))"
                                (if (equal? 1 (child-number content))
                                    "
            throw ParserError.Different(DifferentSyntax())
"
                                    ($ "
            throw ParserError.Invalid(InvalidSyntax(start, lexer.position, String#(\"expected '"(id (element-with-id (link content)))"'\")))
")
                                )
                            )
                        )
                        (("punctuation")
                            (if (optional? content)
                                ;; Optional punctuation - just try it
                                ($
"
        lexer.parse_punctuation#("(punct-const-name (value (element-with-id (link content))))")
"                               )
                                ;; Required punctuation
                                ($
"
        if ~lexer.parse_punctuation#("(punct-const-name (value (element-with-id (link content))))")"
                                    (if (equal? 1 (child-number content))
                                        "
            throw ParserError.Different(DifferentSyntax())
"
                                        ($ "
            throw ParserError.Invalid(InvalidSyntax(start, lexer.position, String#(\"expected '"(value (element-with-id (link content)))"'\")))
")
                                    )
                                )
                            )
                        )
                        (("colon")
                            (if (optional? content)
                                ;; Optional colon - just try it
                                ($
"
        lexer.parse_colon#()
"                               )
                                ;; Required colon
                                ($
"
        if ~lexer.parse_colon#()"
                                    (if (equal? 1 (child-number content))
                                        "
            throw ParserError.Different(DifferentSyntax())
"
                                        "
            throw ParserError.Invalid(InvalidSyntax(start, lexer.position, String#(\"expected colon or newline\")))
")
                                )
                            )
                        )
                        (("identifier")
                            (let ((prop (property content)))
                            ($
"
        let " prop " lexer.parse_identifier#(keywords)
        if " prop ".length() = 0"
                                (if (equal? 1 (child-number content))
                                    "
            throw ParserError.Different(DifferentSyntax())
"
                                    "
            throw ParserError.Invalid(InvalidSyntax(start, lexer.position, String#(\"expected identifier\")))
")
                            ))
                        )
                        (("attribute")
                            (let ((prop (property content)))
                            ($
"
        let " prop " lexer.parse_attribute#()
        if " prop ".length() = 0"
                                (if (equal? 1 (child-number content))
                                    "
            throw ParserError.Different(DifferentSyntax())
"
                                    "
            throw ParserError.Invalid(InvalidSyntax(start, lexer.position, String#(\"expected attribute\")))
")
                            ))
                        )
                        (else "")
                    )
                ))
"
        let end lexer.position

        "(id syntax)"Syntax(start, end"
                (apply-to-property-children-of syntax (lambda (content) ($
                    ", "
                    (if (or (equal? (type content) "keyword") (equal? (type content) "punctuation"))
                        "true"
                        (property content))
                )))
                ")
"
            )
        )
"    }
"   )))
"
    function is_at_end(this: Parser) returns bool
        lexer.is_at_end()
}

function test() returns int
{
    ; Test 1: Parse a simple literal
    var parser Parser#(String#(\"42\"))
    choose parser.parse_literal#()
        when Error: err
            return 1
        when Success: literal
        {
            if ~parser.is_at_end()
                return 2
        }

    ; Test 2: Parse an identifier expression
    set parser: Parser#(String#(\"foo\"))
    choose parser.parse_name#()
        when Error: err
            return 3
        when Success: name
        {
            if ~parser.is_at_end()
                return 4
        }

    ; Test 3: Parse a function definition
    set parser: Parser#(String#(\"function hello() returns int 42\"))
    choose parser.parse_function#()
        when Error: err
            return 5
        when Success: func
        {
            if ~parser.is_at_end()
                return 6
        }

    0
}

"

))
]]>
