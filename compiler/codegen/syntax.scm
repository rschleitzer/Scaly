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
                                (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (optional? content)"*" "")))
                                (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                                (("literal") ($ "LiteralToken"(if (optional? content)"*" "")))
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
                            (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (optional? content)"*" "")))
                            (("identifier" "attribute") ($ "String"(if (optional? content)"*" "")))
                            (("literal") ($ "LiteralToken"(if (optional? content)"*" "")))
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
"    "          (id syntax-node)"Syntax("(link content)"Syntax "(string-firstchar-downcase (link content))"Syntax) : tag("(link content)") { "(string-firstchar-downcase (link content))"Syntax = "(string-firstchar-downcase (link content))"Syntax; }
"               )))

"    enum {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
        "(link content)","
                )))
"
    } tag;
    union {"
                (apply-to-children-of syntax-node (lambda (content) ($
"
        "(link content)"Syntax "(string-firstchar-downcase (link content))"Syntax;"
                )))
"
    };
};
"           )
        )
    ))
))
