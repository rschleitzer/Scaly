(define (syntax) ($
"use scaly.containers
"
   (apply $ (map
        (lambda (tag payload type) ($
"define "tag"Literal("payload": "type")
"       ))
        (list "Identifier" "String" "Character" "Fragment" "Integer" "Boolean" "FloatingPoint" "Hex")
        (list "name"       "value"  "value"     "value"    "value"   "value"   "value"         "value")
        (list "String"     "String" "String"    "String"   "String"  "bool"    "String"        "String")
    ))
"
define Literal union (
"   (apply $ (map
        (lambda (tag) ($
"    "tag": "tag"Literal
"       ))
        (list "String" "Character" "Fragment" "Integer" "Boolean" "FloatingPoint" "Hex")
    ))
")
"
    (apply-to-nodelist (node-list-reverse (select-elements (children (current-node)) "syntax")) (lambda (syntax-node)
        (if (not (abstract? syntax-node))
            ($
"
define "(id syntax-node)"Syntax
(
    start: size_t
    end: size_t"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
"
    "
                        (property content)
": "
                        (case (type content)
                            (("syntax") ($ (if (optional? content) "pointer[" "")(if (multiple? content) ($ "Vector[") "")(link content)"Syntax"(if (multiple? content)"]" "")(if (or (optional? content))"]" "")))
                            (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                            (("literal") "Literal")
                            (("keyword" "punctuation") "bool")
                        )
                    )"")
                )))
"
)
"           )
            ($
"
define "(id syntax-node)"Syntax union ("
    (apply-to-children-of syntax-node (lambda (content) ($
"
    "(link content)": "(link content)"Syntax"
                )))
"
)
"           )
        )
    ))
))
