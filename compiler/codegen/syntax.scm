(define (syntax) ($
"
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"struct "(id syntax-node)"Syntax; 
"   )))

    (apply-to-nodelist (node-list-reverse (select-elements (children (current-node)) "syntax")) (lambda (syntax-node)
        (if (not (abstract? syntax-node))
            ($
"
struct "(id syntax-node)"Syntax : Object {
    "           (id syntax-node)"Syntax(size_t start, size_t end"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
                            ", "
                            (case (type content)
                                (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (or (optional? content) (multiple? content))"*" "")))
                                (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                                (("literal") "LiteralToken*")
                                (("keyword" "punctuation") "bool")
                            )
                            " "
                            (property content)
                    )"")
                )))
    ") : start(start), end(end)"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
                ", "
                (property content)"("(property content)")"
                    )"")
                )))
        " {}
    size_t start;
    size_t end;"
                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
"
    "
                        (case (type content)
                            (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (or (optional? content) (multiple? content))"*" "")))
                            (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                            (("literal") "LiteralToken*")
                            (("keyword" "punctuation") "bool")
                        )
" "
                        (property content)";"
                    )"")
                )))
"
};
"           )
            ($
"
struct "(id syntax-node)"Syntax : Object {
"               (apply-to-children-of syntax-node (lambda (content) ($
"    "          (id syntax-node)"Syntax("(link content)"Syntax _"(link content)"Syntax) : _tag("(link content)") { _"(link content)" = _"(link content)"Syntax; }
"               )))

"    enum {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
        "(link content)","
                )))
"
    } _tag;
    union {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
        "(link content)"Syntax _"(link content)";"
                )))
"
    };
};
"           )
        )
    ))
))
