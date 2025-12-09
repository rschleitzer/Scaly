;; dbprodn.dsl -- Productionset handling for DocBook DSSSL
;; Custom styling for grammar productions with linking

;; Production set container
(element productionset
  (make element gi: "TABLE"
    attributes: '(("CLASS" "productionset") ("BORDER" "0") ("CELLSPACING" "0") ("CELLPADDING" "4"))
    (process-children)))

;; Individual production rule
(element production
  (let ((id (attribute-string "xml:id")))
    (make element gi: "TR"
      (make element gi: "TD"
        attributes: '(("VALIGN" "TOP") ("ALIGN" "RIGHT"))
        (if id
            (make element gi: "A"
              attributes: (list (list "NAME" id))
              (empty-sosofo))
            (empty-sosofo))
        (process-first-descendant "lhs"))
      (make element gi: "TD"
        attributes: '(("VALIGN" "TOP"))
        (literal " ::= "))
      (make element gi: "TD"
        attributes: '(("VALIGN" "TOP"))
        (process-first-descendant "rhs")))))

;; Left-hand side (production name)
(element lhs
  (make element gi: "B"
    (process-children)))

;; Right-hand side (production body)
(element rhs
  (process-children))

;; Nonterminal with linking
(element nonterminal
  (let ((def (attribute-string "def")))
    (if def
        (make element gi: "A"
          attributes: (list (list "HREF" (string-append "#" def)))
          (make element gi: "I"
            (process-children)))
        (make element gi: "I"
          (process-children)))))
