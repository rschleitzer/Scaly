<![CDATA[; grammar-doc.scm - Generate DocBook productionset from grammar

(define (generate-grammar-doc) ($
"<productionset xmlns=\"http://docbook.org/ns/docbook\">
"
    (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"
  <production xml:id=\"prod."(id syntax-node)"\">
    <lhs>"(id syntax-node)"</lhs>
    <rhs>"
        (if (abstract? syntax-node)
            ; Abstract syntax - alternatives separated by |
            (let ((children-list (node-list->list (children syntax-node))))
                (apply $ (map-with-index
                    (lambda (content idx) ($
                        (if (> idx 0) " | " "")
                        "<nonterminal def=\"prod."(link content)"\">"(link content)"</nonterminal>"
                    ))
                    children-list
                ))
            )
            ; Concrete syntax - sequence of content
            (apply-to-children-of syntax-node (lambda (content) ($
                (case (type content)
                    (("syntax") ($
                        "<nonterminal def=\"prod."(link content)"\">"(link content)"</nonterminal>"
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
"</rhs>
  </production>
"   )))
"
</productionset>
"
))

; Helper to map with index
(define (map-with-index proc lst)
    (let loop ((lst lst) (idx 0) (result '()))
        (if (null? lst)
            (reverse result)
            (loop (cdr lst) (+ idx 1) (cons (proc (car lst) idx) result)))))
]]>