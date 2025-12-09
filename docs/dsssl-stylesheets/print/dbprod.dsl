;; dbprod.dsl - DSSSL stylesheet for BNF productions
;; Custom addition for dazzzledoc project

;; ==================== PARAMETERS ====================

;; Enable section numbering down to sect3 (1.1.1)
(define %section-autolabel% #t)

;; Increase paragraph separation for better readability
(define %para-sep%
  (* %bf-size% 0.75))  ; Increased from default 0.5 to 0.75

;; ==================== NOTES ====================

;; Override note to use smaller font and add "NOTE N" or "NOTES" prefix
(element note
  (let* ((note-num (element-number))
         ;; Check if this note is part of a multi-note sequence
         ;; by checking if previous or next sibling is also a note
         (prev-sib (ipreced (current-node)))
         (next-sib (ifollow (current-node)))
         (prev-is-note (and (not (node-list-empty? prev-sib))
                            (equal? (gi prev-sib) (normalize "note"))))
         (next-is-note (and (not (node-list-empty? next-sib))
                            (equal? (gi next-sib) (normalize "note"))))
         (is-multi-note-seq (or prev-is-note next-is-note))
         (is-first-in-seq (and (not prev-is-note) next-is-note)))
    (make display-group
      space-before: %para-sep%
      space-after: %para-sep%
      font-size: (* %bf-size% 0.85)
      line-spacing: (* (* %bf-size% 0.85) %line-spacing-factor%)
      ;; Process children with custom handling for first para
      (let loop ((nl (children (current-node))) (first-para? #t))
        (if (node-list-empty? nl)
            (empty-sosofo)
            (let ((node (node-list-first nl))
                  (rest (node-list-rest nl)))
              (sosofo-append
                (if (equal? (gi node) (normalize "para"))
                    ;; Para element - render with optional NOTE/NOTES prefix
                    (make paragraph
                      space-before: (if first-para? 0pt %para-sep%)
                      space-after: %para-sep%
                      start-indent: (inherited-start-indent)
                      (make sequence
                        ;; Add prefix only for first para
                        (if first-para?
                            (cond
                              ;; First note in multi-note sequence: "NOTES" heading + number
                              (is-first-in-seq
                               (make sequence
                                 (literal "NOTES")
                                 (make paragraph-break)
                                 (literal (string-append (number->string note-num) "  "))))
                              ;; Subsequent note in multi-note sequence: just the number
                              ((and is-multi-note-seq prev-is-note)
                               (literal (string-append (number->string note-num) "  ")))
                              ;; Regular single note: "NOTE N "
                              (else
                               (literal (string-append "NOTE " (number->string note-num) " "))))
                            (empty-sosofo))
                        ;; Process para's children (not the para element itself)
                        (with-mode note-para-mode
                          (process-node-list (children node)))))
                    ;; Other elements - process normally
                    (process-node-list node))
                (loop rest (if (equal? (gi node) (normalize "para")) #f first-para?)))))

      ))))

;; Mode for processing para content without para wrapper
(mode note-para-mode
  ;; Inline elements need explicit rules to preserve their formatting
  (element code ($mono-seq$))
  (element literal ($mono-seq$))
  (element replaceable ($italic-seq$))
  (element emphasis
    (if (and (attribute-string (normalize "role"))
             (or (equal? (attribute-string (normalize "role")) "strong")
                 (equal? (attribute-string (normalize "role")) "bold")))
        ($bold-seq$)
        ($italic-seq$)))
  (element superscript ($ss-seq$ +))
  (element subscript ($ss-seq$ -))
  ;; Default for other elements
  (default (process-children)))

;; ==================== EXAMPLES ====================

;; Override informalexample to add titles based on xml:id
;; This overrides the default in dbblock.dsl
(mode informalexample-mode
  (element programlisting
    (process-children)))

(element informalexample
  (let* ((id (attribute-string (normalize "xml:id")))
         (example-num (if id
                          (substring id 8 (string-length id))  ; Extract number from "example-N"
                          #f)))
    (if example-num
        ;; Has xml:id - add EXAMPLE title
        (make display-group
          space-before: %para-sep%
          space-after: 0pt
          start-indent: (inherited-start-indent)
          ;; Add title
          (make paragraph
            space-after: 3pt  ; Small space after EXAMPLE title
            (literal (string-append "EXAMPLE " example-num)))
          ;; Process content
          (process-children))
        ;; No xml:id - use default handling
        ($informal-object$ %informalexample-rules% %informalexample-rules%))))

;; ==================== PRODUCTIONS ====================

(element productionset
  (make display-group
    space-before: %para-sep%
    space-after: %para-sep%
    start-indent: (inherited-start-indent)
    (process-children)))

(element production
  (make paragraph
    space-before: %para-sep%
    space-after: 0pt
    start-indent: (inherited-start-indent)
    (process-children)))

(element (production lhs)
  (make sequence
    (process-children)
    (literal " = ")))

(element (production rhs)
  (make sequence
    font-family-name: %mono-font-family%
    (process-children)))

(element (production rhs lineannotation)
  (let ((role (attribute-string (normalize "role"))))
    (if (equal? role "meta")
        ;; Meta-characters in proportional font
        (make sequence
          font-family-name: %body-font-family%
          (process-children))
        ;; Default lineannotation handling
        (process-children))))

(element (production rhs sbr)
  (make sequence
    font-family-name: %body-font-family%
    (literal " ")
    (make line-field
      field-width: 0pt)
    (literal "| ")))

(element (production rhs nonterminal)
  (let ((role (attribute-string (normalize "role"))))
    (make sequence
      font-family-name: %body-font-family%
      font-posture: 'italic
      font-weight: (if (equal? role "strong") 'bold 'medium)
      (process-children))))
