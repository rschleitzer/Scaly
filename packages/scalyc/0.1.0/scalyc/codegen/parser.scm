<![CDATA[
;; Generate Scaly Parser from scaly.sgm

;; Helper to generate char constant name from punctuation value
(define (punct-const-name value)
    (case value
        (("(") "CHAR_LPAREN")
        ((")") "CHAR_RPAREN")
        (("[") "CHAR_LBRACKET")
        (("]") "CHAR_RBRACKET")
        (("{") "CHAR_LBRACE")
        (("}") "CHAR_RBRACE")
        ((",") "CHAR_COMMA")
        ((".") "CHAR_DOT")
        (("!") "CHAR_EXCL")
        (("?") "CHAR_QUESTION")
        (("#") "CHAR_HASH")
        (("$") "CHAR_DOLLAR")
        (("^") "CHAR_CARET")
        (("\\") "CHAR_BACKSLASH")
        (else ($ "CHAR_" value))))

(define (generate-parser-scaly) ($
"
use scaly.memory.Page
use scaly.containers.Array
use scaly.containers.List
use scaly.containers.HashSet
use scaly.containers.HashSetBuilder

; Character constants for punctuation (char literals use double-quote + type annotation)
let CHAR_LPAREN: char \"(\"
let CHAR_RPAREN: char \")\"
let CHAR_LBRACKET: char \"[\"
let CHAR_RBRACKET: char \"]\"
let CHAR_LBRACE: char \"{\"
let CHAR_RBRACE: char \"}\"
let CHAR_COMMA: char \",\"
let CHAR_DOT: char \".\"
let CHAR_EXCL: char \"!\"
let CHAR_QUESTION: char \"?\"
let CHAR_HASH: char \"#\"
let CHAR_DOLLAR: char \"$\"
let CHAR_CARET: char \"^\"
let CHAR_BACKSLASH: char \"\\\\\"

; Lexer import (uses its own char constants)
module lexer

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
            when empty: Empty
                lexer.advance#()
            else {}

        choose lexer.token
            when literal: Literal
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
                when error: ParserError
                {
                    choose error
                        when i: Invalid
                            throw ParserError.Invalid(i)
                        when d: Different
                        {
                            if list.head = null
                                throw ParserError.Different(d)
                            else
                                return Vector["(id syntax)"Syntax]#(list)
                        }
                }
                when node: "(id syntax)"Syntax
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
            when error: ParserError
            {
                choose error
                    when i: Invalid
                        throw ParserError.Invalid(i)
                    when d: Different {}
            }
            when node: "(link content)"Syntax
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
            when error: ParserError
            {
                choose error
                    when d: Different {}
                    when i: Invalid
                        throw ParserError.Invalid(i)
            }
            when success: " type-name "
                set " prop ": success
"
                                )
                                ;; non-optional syntax
                                ($
"
        let " prop "_start lexer.position
        var " prop ": " type-name "
        choose parse_"(downcase-string (link content))(if (multiple? content) "_list" "")"#()
            when error: ParserError
"
                                    (if (equal? 1 (child-number content))
                                        "                throw error
"
                                        ($
"            {
                choose error
                    when d: Different
                        throw ParserError.Invalid(InvalidSyntax(" prop "_start, lexer.position, String#(\"expected "(link content)"\")))
                    when i: Invalid
                        throw ParserError.Invalid(i)
            }
"
                                        )
                                    )
"            when success: " type-name "
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
            when error: ParserError
                throw error
            when success: Literal
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

"

))
]]>
