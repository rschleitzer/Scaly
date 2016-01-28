(define (create-file filename content)
    (make entity system-id: filename content))

(define (output content)
    (make formatting-instruction data: content))

(define (apply-to-nodelist nl func)
    (apply $ (map func (node-list->list nl))))

(define (apply-to-children-of node func)
    (apply-to-nodelist (children node) func))

(define (apply-to-children func)
    (apply-to-children-of (current-node) func))

(define (apply-to-selected-children-of node element func)
    (apply-to-nodelist (select-elements (children node) element ) func))

(define (apply-to-selected-children element func)
    (apply-to-selected-children-of (current-node) element func))

(define (apply-to-descendants-of node func)
    (apply-to-nodelist (descendants node) func))

(define (apply-to-descendants func)
    (apply-to-descendants-of (current-node) func))

(define (apply-to-selected-descendants-of node element func)
    (apply-to-nodelist (select-elements (descendants node) element) func))

(define (apply-to-selected-descendants element func)
    (apply-to-selected-descendants-of (current-node) element func))

(define $ string-append)

(define (link node)
    (attribute-string "link" node))

(define (value node)
    (attribute-string "value" node))

(define (abstract? node)
    (string=? "abstract" (attribute-string "abstract" node)))

(define (multiple? node)
    (string=? "multiple" (attribute-string "multiple" node)))

(define (optional? node)
    (string=? "optional" (attribute-string "optional" node)))

(define (syntax? node)
    (string=? "syntax" (attribute-string "type" node)))

(define (top? node)
    (string=? "top" (attribute-string "top" node)))

(define (program? node)
    (string=? "program" (attribute-string "program" node)))

(define (property node)
    (attribute-string "property" node))

(define (base node)
    (attribute-string "base" node))

(define (name node)
    (if (string=? "keyword" (gi node))
        ($ (id node) "Keyword")
        (attribute-string "name" node)))

(define (name-of-link node)
    (name (element-with-id (attribute-string "link" node))))

(define (type node)
    (if (attribute-string "link" node)
        (gi (element-with-id (attribute-string "link" node)))
        (attribute-string "type" node)))
