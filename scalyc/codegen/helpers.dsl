(define (string-firstchar-upcase s)
    (string-firstchar-case s char-upcase-ascii))

(define (string-firstchar-downcase s)
    (string-firstchar-case s char-downcase-ascii))

(define (string-firstchar-case s func)
    (list->string
        (let ((charlist (string->list s)))
            (cons
                (func (car charlist))
                (cdr charlist)))))

(define (char-upcase-ascii c)
    (case c
        ((#\A #\a) #\A) ((#\B #\b) #\B)
        ((#\C #\c) #\C) ((#\D #\d) #\D)
        ((#\E #\e) #\E) ((#\F #\f) #\F)
        ((#\G #\g) #\G) ((#\H #\h) #\H)
        ((#\I #\i) #\I) ((#\J #\j) #\J)
        ((#\K #\k) #\K) ((#\L #\l) #\L)
        ((#\M #\m) #\M) ((#\N #\n) #\N)
        ((#\O #\o) #\O) ((#\P #\p) #\P)
        ((#\Q #\q) #\Q) ((#\R #\r) #\R)
        ((#\S #\s) #\S) ((#\T #\t) #\T)
        ((#\U #\u) #\U) ((#\V #\v) #\V)
        ((#\W #\w) #\W) ((#\X #\x) #\X)
        ((#\Y #\y) #\Y) ((#\Z #\z) #\Z)
        (else c)
    )
)

(define (char-downcase-ascii c)
    (case c
        ((#\A #\a) #\a) ((#\B #\b) #\b)
        ((#\C #\c) #\c) ((#\D #\d) #\d)
        ((#\E #\e) #\e) ((#\F #\f) #\f)
        ((#\G #\g) #\g) ((#\H #\h) #\h)
        ((#\I #\i) #\i) ((#\J #\j) #\j)
        ((#\K #\k) #\k) ((#\L #\l) #\l)
        ((#\M #\m) #\m) ((#\N #\n) #\n)
        ((#\O #\o) #\o) ((#\P #\p) #\p)
        ((#\Q #\q) #\q) ((#\R #\r) #\r)
        ((#\S #\s) #\s) ((#\T #\t) #\t)
        ((#\U #\u) #\u) ((#\V #\v) #\v)
        ((#\W #\w) #\w) ((#\X #\x) #\x)
        ((#\Y #\y) #\y) ((#\Z #\z) #\z)
        (else c)
    )
)

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

(define (apply-to-property-children-of node func)
    (apply-to-nodelist (properties node) func))

(define (properties node)
    (node-list-filter property (children node)))

(define (properties-remaining? node syntax)
    (not (node-list=? (node-list-last (properties syntax)) node)))

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
    (string=? "syntax" (type node)))

(define (identifier? node)
    (string=? "identifier" (type node)))

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

(define (has-syntax-children? node)
    (not (node-list-empty?
        (node-list-filter (lambda (snl) (and (syntax? snl) (string=? "syntax" (gi (element-with-id (attribute-string "link" snl))))))
            (children node)))))