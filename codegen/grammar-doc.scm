(define (generate-grammar-doc) ($
"&#60;productionset xmlns=\"http://docbook.org/ns/docbook\"&#62;
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
  &#60;production xml:id=\"prod."(id syntax-node)"\"&#62;
    &#60;lhs&#62;"(id syntax-node)"&#60;/lhs&#62;
    &#60;rhs&#62;"
        (if (abstract? syntax-node)
            ; Abstract syntax - alternatives separated by |
            (let ((children-list (node-list->list (children syntax-node))))
                (apply $ (map-with-index
                    (lambda (content idx) ($
                        (if (> idx 0) " | " "")
                        "&#60;nonterminal def=\"prod."(link content)"\"&#62;"(link content)"&#60;/nonterminal&#62;"
                    ))
                    children-list
                ))
            )
            ; Concrete syntax - sequence of content
            (apply-to-children-of syntax-node (lambda (content) ($
                (case (type content)
                    (("syntax") ($
                        "&#60;nonterminal def=\"prod."(link content)"\"&#62;"(link content)"&#60;/nonterminal&#62;"
                        (cond
                            ((and (multiple? content) (optional? content)) "*")
                            ((multiple? content) "+")
                            ((optional? content) "?")
                            (else "")
                        )
                        " "
                    ))
                    (("keyword") ($
                        "'"(id (element-with-id (link content)))"' "
                    ))
                    (("punctuation") ($
                        "'"(value (element-with-id (link content)))"' "
                    ))
                    (("identifier") ($
                        "IDENTIFIER"
                        (if (optional? content) "?" "")
                        " "
                    ))
                    (("attribute") ($
                        "ATTRIBUTE"
                        (if (optional? content) "?" "")
                        " "
                    ))
                    (("literal") "LITERAL ")
                    (("colon") ($
                        "':'"
                        (if (optional? content) "?" "")
                        " "
                    ))
                    (else "")
                )
            )))
        )
"&#60;/rhs&#62;
  &#60;/production&#62;
"   )))
"
&#60;/productionset&#62;
"
))

; Helper to map with index
(define (map-with-index proc lst)
    (let loop ((lst lst) (idx 0) (result '()))
        (if (null? lst)
            (reverse result)
            (loop (cdr lst) (+ idx 1) (cons (proc (car lst) idx) result)))))
