(define (syntax) ($
"
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"struct "(id syntax-node)"Syntax; 
"   )))

    (apply-to-selected-children "syntax" (lambda (syntax-node) (if (abstract? syntax-node) "" ($
"
struct "(id syntax-node)"Syntax : Object {
    "(id syntax-node)"Syntax("
        (if (top? syntax-node)
        "String file_name"
        (if (program? syntax-node) "" "size_t start, size_t end")
        )
        (apply-to-children-of syntax-node (lambda (content) ($
            (if (property content) ($
                (if (program? syntax-node) "" ", ")
                    (case (type content)
                        (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")"*"))
                        (("identifier" "attribute") ($ "String*"))
                        (("literal") ($ "LiteralToken"(if (optional? content)"*" "")))
                        (("keyword" "punctuation") "bool")
                    )
                    " "
                    (property content)
            )"")
        )))
    ") : "
        (if (top? syntax-node)
            "file_name(file_name)"
            (if (program? syntax-node) "" "start(start), end(end)")
        )
        (apply-to-children-of syntax-node (lambda (content) ($
            (if (property content) ($
                (if (program? syntax-node) "" ", ")(property content)"("(property content)")"
            )"")
        )))
        " {}"
        (if (top? syntax-node)
            ($
"
    String file_name;"
            )
            (if (program? syntax-node) ""
"
    size_t start;
    size_t end;"
            )
        )

        (apply-to-children-of syntax-node (lambda (content) ($
            (if (property content) ($
"
    "
                (case (type content)
                    (("syntax") ($ (if (multiple? content) ($ "Vector<") "")(link content)"Syntax"(if (multiple? content)">" "")"*"))
                    (("identifier" "attribute") ($ "String*"))
                    (("literal") ($ "LiteralToken"(if (optional? content)"*" "")))
                    (("keyword" "punctuation") "bool")
                )
" "
                (property content)";"
            )"")
        )))
"
};
"   ))))

    (apply-to-selected-children "syntax" (lambda (syntax-node) (if (abstract? syntax-node) ($
"
struct "(id syntax-node)"Syntax : Object {
"       (apply-to-children-of syntax-node (lambda (content) ($
"    "(id syntax-node)"Syntax("(link content)"Syntax "(string-firstchar-downcase (link content))"Syntax) : tag("(link content)"), "(string-firstchar-downcase (link content))"Syntax("(string-firstchar-downcase (link content))"Syntax) {}
"       )))

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
"                
"
};
"   )"")))
))
