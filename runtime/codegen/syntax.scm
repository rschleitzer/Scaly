(define (syntax) ($

    (apply-to-selected-children "syntax" (lambda (syntax-node) (if (abstract? syntax-node) "" ($
"
    public class "(id syntax-node)"Syntax
    {
        public string file;"  

        (if (top? syntax-node) ""
            (if (program? syntax-node) "" ($
"
        public Position start;
        public Position end;"
            ))
        )

        (apply-to-children-of syntax-node (lambda (content) ($
        (if (property content) ($
"
        public "
            (case (type content)
                (("syntax") ($ (if (abstract? (element-with-id (link content))) "object" ($ (link content)"Syntax"))(if (multiple? content)"[]" "")))
                (("identifier" "attribute") "string")
                (("literal") "Literal")
                (("keyword" "punctuation") "bool")
            )" "(property content)";"
            )"")
        )))
"
    }
"   ))))
"}
"
))
