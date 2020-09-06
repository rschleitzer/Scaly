(define (syntax) ($

    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
pub "(if (abstract? syntax-node) "enum" "struct")" "(id syntax-node)"Syntax {"
        (if (abstract? syntax-node)
            (apply-to-children-of syntax-node (lambda (content) ($
"
    "(link content)"("(link content)"Syntax),"
            )))
            ($
                (if (top? syntax-node)
                    ($
"
    pub file_name: String,"
                    )
                    (if (program? syntax-node) "" ($
"
    pub start: Position,
    pub end: Position,"
                    ))
                )

                (apply-to-children-of syntax-node (lambda (content) ($
                    (if (property content) ($
"
    pub "(property content)": "
                (case (type content)
                    (("syntax") ($ (if (optional? content)"Option<" "")(if (multiple? content) ($ "Vec<") "")(link content)"Syntax"(if (multiple? content)">" "")(if (optional? content)">" "")))
                    (("identifier") ($ (if (optional? content)"Option<" "")"String"(if (optional? content)">" "")))
                    (("literal") ($ (if (optional? content)"Option<" "")"Literal"(if (optional? content)">" "")))
                    (("keyword" "punctuation") "bool")
                )
                ","
                    )"")
                )))
            )
        )
"
}
"   )))
))
