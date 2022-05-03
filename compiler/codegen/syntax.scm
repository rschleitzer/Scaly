(define (syntax) ($
"
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"struct "(id syntax-node)"Syntax; 
"   )))

    (apply-to-selected-children "syntax" (lambda (syntax-node) (if (abstract? syntax-node) "" ($
"
struct "(id syntax-node)"Syntax : Object {"
        (if (top? syntax-node)
            ($
"
    String file_name;"
            )
            (if (program? syntax-node) "" ($
"
    size_t start;
    size_t end;"
            ))
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
        " "(property content)";"
            )"")
        )))
"
};
"   ))))

    (apply-to-selected-children "syntax" (lambda (syntax-node) (if (abstract? syntax-node) ($
"
struct "(id syntax-node)"Syntax : Object {"
"
    enum {"
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
