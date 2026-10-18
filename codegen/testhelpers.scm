<![CDATA[; testhelpers.scm - Reading test.dtd documents: the id, input,
; expected result and kind of a test, for testscaly.scm and testdoc.scm

; Helper: get the id of a node
(define (id node)
    (attribute-string "id" node))

; Define special characters via a string literal
; String contains: space, newline (literal in source)
(define newline-string "
")

; Helper: check if character is the newline
(define (is-newline? c)
    (char=? c (string-ref newline-string 0)))

; Helper: trim leading/trailing whitespace (simplified - just trim spaces and newlines)
(define (trim s)
    (drop-trailing-ws (drop-leading-ws s)))

(define (drop-leading-ws s)
    (let ((chars (string->list s)))
        (list->string (drop-while-ws chars))))

(define (drop-trailing-ws s)
    (list->string (reverse (drop-while-ws (reverse (string->list s))))))

(define (drop-while-ws chars)
    (cond
        ((null? chars) '())
        ((char-whitespace? (car chars)) (drop-while-ws (cdr chars)))
        (else chars)))

(define (char-whitespace? c)
    (or (char=? c #\space)
        (is-newline? c)))

; Get the input text from a test element
(define (test-input test)
    (let ((input-node (node-list-first (select-elements (children test) "input"))))
        (if (node-list-empty? input-node)
            ""
            (data input-node))))

; Get the expected result from a test element
(define (test-expect test)
    (let ((expect-node (node-list-first (select-elements (children test) "expect"))))
        (if (node-list-empty? expect-node)
            ""
            (data expect-node))))

; Check if test should be skipped
(define (test-skip? test)
    (attribute-string "skip" test))

; Check if test is a program (multi-statement)
(define (test-program? test)
    (attribute-string "program" test))

; Determine expected type from expect string
(define (expect-type expect-str input-str)
    (cond
        ((string=? expect-str "true") "bool")
        ((string=? expect-str "false") "bool")
        ((string-contains-dot? expect-str) "double")
        ((string-prefix? expect-str "{") "json")
        ((string-prefix? input-str "\"") "string")  ; String literal
        ((string-prefix? input-str "`") "char")     ; Character literal
        (else "int")))

; Check if string contains a dot (for float detection)
(define (string-contains-dot? s)
    (let loop ((chars (string->list s)))
        (cond
            ((null? chars) #f)
            ((char=? (car chars) #\.) #t)
            (else (loop (cdr chars))))))

; Check if string starts with a prefix
(define (string-prefix? s prefix)
    (let ((s-chars (string->list s))
          (p-chars (string->list prefix)))
        (and (>= (length s-chars) (length p-chars))
             (equal? (list-head s-chars (length p-chars)) p-chars))))

(define (list-head lst n)
    (if (or (null? lst) (<= n 0))
        '()
        (cons (car lst) (list-head (cdr lst) (- n 1)))))

]]>
