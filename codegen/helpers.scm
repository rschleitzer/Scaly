(define (string-firstchar-upcase s)
    (string-firstchar-case s char-upcase))

(define (string-firstchar-downcase s)
    (string-firstchar-case s char-downcase))

(define (string-firstchar-case s func)
    (list->string
        (let ((charlist (string->list s)))
            (cons
                (func (car charlist))
                (cdr charlist)))))


(define-language latin
    (toupper
        (#\a #\A) (#\b #\B) (#\c #\C) (#\d #\D) (#\e #\E) (#\f #\F) (#\g #\G) (#\h #\H) (#\i #\I) (#\j #\J)
        (#\k #\K) (#\l #\L) (#\m #\M) (#\n #\N) (#\o #\O) (#\p #\P) (#\q #\Q) (#\r #\R) (#\s #\S) (#\t #\T)
        (#\u #\U) (#\v #\V) (#\w #\W) (#\x #\X) (#\y #\Y) (#\z #\Z)
    )
    (tolower
        (#\A #\a) (#\B #\b) (#\C #\c) (#\D #\d) (#\E #\e) (#\F #\f) (#\G #\g) (#\H #\h) (#\I #\i) (#\J #\j)
        (#\K #\k) (#\L #\l) (#\M #\m) (#\N #\n) (#\O #\o) (#\P #\p) (#\Q #\q) (#\R #\r) (#\S #\s) (#\T #\t)
        (#\U #\u) (#\V #\v) (#\W #\w) (#\X #\x) (#\Y #\y) (#\Z #\z)
    )
)

(declare-default-language latin)

(define (downcase-string s)
    (list->string
        (map char-downcase
            (string->list s))))

(define (upcase-string s)
    (list->string
        (map char-upcase
            (string->list s))))

(define (file filename contents)
    (make entity system-id: filename (make formatting-instruction data: contents)))

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

;; A "peek" syntax speculatively consumes a leading separator colon before its
;; discriminating keyword, snapshotting the lexer first and restoring it if the
;; keyword is absent. Used for Else so a brace-less consequent's trailing
;; newline stays a statement separator when no else follows (no-else if).
(define (peek? node)
    (string=? "peek" (attribute-string "peek" node)))

(define (syntax? node)
    (string=? "syntax" (type node)))

(define (identifier? node)
    (string=? "identifier" (type node)))

(define (top? node)
    (string=? "top" (attribute-string "top" node)))

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

;; The type ("colon"/"keyword"/"punctuation"/"literal"/"syntax"/...) of a
;; syntax's first content child -- i.e. the token a parse_X commits on. A
;; separator newline lexes as a Colon, so a colon-opened optional field
;; (Value) false-commits on a separator and must backtrack by swallowing its
;; Invalid. TypeAnnotation and BindingAnnotation open with the SIGN ':'
;; (colon_sign, a colon written in the source) and are no longer of this
;; kind: `let a: 42` swallowed its "expected BindingSpec" and bound 42, and a
;; binding's name followed by a line feed took the next line for its type
;; (2026-10-04). Every other opener ([ { ( keyword
;; literal ^ ?) is unambiguous, so a committed Invalid is a real syntax error
;; and must propagate -- otherwise class/union/trait bodies silently swallow
;; inner errors (and re-surface as a bogus outer "expected '}'").
(define (first-content-type node)
    (type (node-list-first (children node))))

(define (colon-opened? content)
    (string=? "colon" (first-content-type (element-with-id (link content)))))

(define (has-syntax-children? node)
    (not (node-list-empty?
        (node-list-filter (lambda (snl) (and (syntax? snl) (string=? "syntax" (gi (element-with-id (attribute-string "link" snl))))))
            (children node)))))

;; Check if a content reference creates a recursive type:
;; True when content links to an abstract syntax whose variant includes syntax-node.
(define (is-recursive-ref? syntax-node content)
    (let ((target (element-with-id (link content))))
        (and (abstract? target)
             (not (node-list-empty?
                (node-list-filter
                    (lambda (child) (string=? (link child) (id syntax-node)))
                    (children target)))))))