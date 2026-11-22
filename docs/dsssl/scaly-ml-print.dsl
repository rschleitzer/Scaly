<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "../dsssl-stylesheets/print/docbook.dsl" CDATA DSSSL>
]>
<style-sheet>
<style-specification use="docbook">
<style-specification-body>

;; Machine Learning with Scaly as PDF, set by the Scaly DSSSL engine:
;;
;;   dazzle -t pdf -d dsssl/scaly-ml-print.dsl scaly-ml.xml   (docs/ml-pdf.sh)
;;
;; A layer over the DocBook print stylesheet (dsssl-stylesheets/print, which
;; sets the specification's PDF as it is), for what the book has that the
;; print stylesheet does not know: DocBook 5's links (xlink:href) and its
;; language (xml:lang), the boxes that send the reader to a page
;; (sidebar role="tryit"), a picture beside a formula, and the formulas,
;; MathML: each element becomes the DSSSL math flow object it is -- mfrac a
;; fraction, msub and msup a script, mrow a math sequence, a pair of
;; parentheses around an mrow a fence --, each mo carries its math class
;; (binary, relation, opening, ...), and a letter alone in an mi is italic;
;; the engine sets them (dazzle's PDF backend: spacing by class, the
;; constants of the font's MATH table, its mathematical italic). The
;; pictures are those of
;; the web pages (fig/, beside the pages), so the book is set in website/ml.

;; ----------------------------------------------------------------- the page

(define %paper-type% "A4")
(define %two-side% #f)
(define %generate-book-titlepage% #t)
(define %section-autolabel% #t)
(define %chapter-autolabel% #t)
(define %graphic-default-extension% "png")
(define %admon-graphics% #f)

;; the language is DocBook 5's xml:lang
(define ($lang$ #!optional (target (current-node)) (xref-context #f))
  (let loop ((n target))
    (if (node-list-empty? n)
        (lang-fix %default-language%)
        (let ((l (attribute-string "xml:lang" n)))
          (if l (lang-fix l) (loop (parent n)))))))

(define (book-lang) (lang-fix ($lang$)))
(define (german?) (equal? (book-lang) "de"))

(define (rgb r g b)
  (color (color-space "ISO/IEC 10179:1996//Color-Space Family::Device RGB")
         (/ r 255) (/ g 255) (/ b 255)))

;; the web pages' green (book/book.css: --accent-strong, --accent-faint)
(define %accent% (rgb 4 120 87))
(define %accent-faint% (rgb 236 253 245))

;; ----------------------------------------------------------------- the book

;; the abstract of the book is its lead, before the first chapter
(element info (empty-sosofo))

;; a link to a page: its text in the accent colour, and the page's address
;; as the link's destination, which the PDF opens (an address the book gives
;; relative to the site's /ml/ made absolute)
(define (absolute-address href)
  (cond ((and (>= (string-length href) 3) (equal? (substring href 0 3) "../"))
         (string-append "https://scaly.io/" (substring href 3 (string-length href))))
        ((and (>= (string-length href) 4) (equal? (substring href 0 4) "http")) href)
        (else (string-append "https://scaly.io/ml/" href))))

(element link
  (let ((href (attribute-string "xlink:href"))
        (linkend (attribute-string "linkend")))
    (if href
        (make link
          destination: (sgml-document-address (absolute-address href) "")
          (make sequence
            color: %accent%
            (process-children)))
        (let ((target (if linkend (element-with-id linkend) (empty-node-list))))
          (if (node-list-empty? target)
              (process-children)
              (make link
                destination: (node-list-address target)
                (process-children)))))))

;; "Try it": its word above, in the accent colour, all of it on the faint
;; green of the web pages' boxes
(element sidebar
  (if (equal? (attribute-string "role") "tryit")
      (make display-group
        space-before: %block-sep%
        space-after: %block-sep%
        start-indent: (+ (inherited-start-indent) 8pt)
        end-indent: (+ (inherited-end-indent) 8pt)
        background-color: %accent-faint%
        (make paragraph
          font-family-name: %title-font-family%
          font-weight: 'bold
          font-size: (* %bf-size% 0.8)
          color: %accent%
          space-after: 0pt
          keep-with-next?: #t
          background-color: %accent-faint%
          (literal (if (german?) "AUSPROBIEREN" "TRY IT")))
        (process-children))
      (make display-group
        space-before: %block-sep%
        space-after: %block-sep%
        (process-children))))

;; the paragraphs of a box on its green
(element (sidebar para)
  (make paragraph
    space-before: 0pt
    space-after: 0pt
    background-color: (if (equal? (attribute-string "role" (parent (current-node))) "tryit") %accent-faint% #f)
    (process-children)))

;; a picture: a mediaobject's image, as wide as it may be
(element mediaobject
  (let ((image (select-elements (descendants (current-node)) "imagedata")))
    (if (node-list-empty? image)
        (empty-sosofo)
        (make external-graphic
          display?: #t
          display-alignment: 'center
          space-before: 4pt
          space-after: 4pt
          entity-system-id: (attribute-string "fileref" (node-list-first image))
          max-width: (- %text-width% (inherited-start-indent))
          max-height: 9cm
          scale: 'max-uniform))))

;; a formula with a picture beside it: the formula, then the picture, small
(element informalfigure
  (if (equal? (attribute-string "role") "beside")
      (make display-group
        space-before: %block-sep%
        space-after: %block-sep%
        (process-node-list (select-elements (children (current-node)) "informalequation"))
        (let ((image (select-elements (descendants (current-node)) "imagedata")))
          (if (node-list-empty? image)
              (empty-sosofo)
              (make external-graphic
                display?: #t
                display-alignment: 'center
                entity-system-id: (attribute-string "fileref" (node-list-first image))
                max-width: 5.5cm
                max-height: 7cm
                scale: 'max-uniform))))
      (make display-group
        space-before: %block-sep%
        space-after: %block-sep%
        (process-children))))

(element alt (empty-sosofo))

;; a formula on a line of its own, centred
(element informalequation
  (make paragraph
    quadding: 'center
    space-before: (* %block-sep% 0.75)
    space-after: (* %block-sep% 0.75)
    keep-with-next?: #f
    (process-children)))

(element inlineequation (process-children))

;; --------------------------------------------------------------- formulas

(define %math-font% "Latin Modern Math")

(define (mml-gi nd)
  (let ((g (gi nd)))
    (if (and g (> (string-length g) 4) (equal? (substring g 0 4) "mml:"))
        (substring g 4 (string-length g))
        #f)))

;; the element children of the current node, without the text between them
(define (element-children nd)
  (let loop ((nl (children nd)) (out (empty-node-list)))
    (if (node-list-empty? nl)
        out
        (loop (node-list-rest nl)
              (if (gi (node-list-first nl))
                  (node-list out (node-list-first nl))
                  out)))))

(define (nth-child nd k)
  (node-list-ref (element-children nd) k))

;; the text of an mo, an mi or an mn, without the white space around it
(define (token-text nd) (mml-trim (data nd)))

(define (mml-trim s)
  (let* ((n (string-length s))
         (start (let loop ((i 0))
                  (if (and (< i n) (mml-space? (string-ref s i))) (loop (+ i 1)) i)))
         (end (let loop ((i n))
                (if (and (> i start) (mml-space? (string-ref s (- i 1)))) (loop (- i 1)) i))))
    (substring s start end)))

(define (mml-space? c)
  (member c (list #\space #\U-0009 #\U-000A #\U-000D)))

;; where an mo stands in its row: alone, first, last or between
(define (mo-form nd)
  (let ((first? (absolute-first-sibling? nd))
        (last? (absolute-last-sibling? nd)))
    (cond ((and first? last?) 'infix)
          (first? 'prefix)
          (last? 'postfix)
          (else 'infix))))

;; the math class of an operator: what the engine spaces it by (the TeX
;; classes, which the DSSSL standard names)
(define (mo-class nd)
  (let ((op (token-text nd))
        (spaced (not (equal? (attribute-string "lspace" nd) "0"))))
    (cond ((not spaced) 'ordinary)
          ((member op '("(" "[" "{")) 'opening)
          ((member op '(")" "]" "}")) 'closing)
          ((member op '("," ";")) 'punctuation)
          ((member op '("=" "<" ">" "≤" "≥" "≠" "≈" "←" "→" "∈")) 'relation)
          ((equal? op "⁡") 'ordinary)
          ((member op '("+" "−" "-" "·" "×" "±"))
           (if (equal? (mo-form nd) 'prefix) 'ordinary 'binary))
          (else 'ordinary))))

;; a token of a formula: its characters, each with the math class the
;; engine spaces it by; a letter alone in an mi italic, which the engine sets
;; in the mathematical italic of the formula's font
(define (math-token text italic? class)
  (make sequence
    font-family-name: %math-font%
    font-posture: (if italic? 'italic 'upright)
    font-weight: 'medium
    (let loop ((cs (string->list text)) (out (empty-sosofo)))
      (if (null? cs)
          out
          (loop (cdr cs)
                (sosofo-append out
                  (make character
                    char: (car cs)
                    math-class: class)))))))

(define (mml-print name)
  (cond
    ((equal? name "math")
     (make math-sequence
       font-family-name: %math-font%
       (process-children)))
    ((equal? name "mrow")
     (let* ((kids (element-children (current-node)))
            (n (node-list-length kids))
            (first (if (> n 1) (node-list-first kids) #f))
            (last (if (> n 1) (node-list-ref kids (- n 1)) #f)))
       (if (and first (equal? (mml-gi first) "mo") (equal? (token-text first) "(")
                (equal? (mml-gi last) "mo") (equal? (token-text last) ")"))
           (make fence
             (make sequence label: 'open
               (math-token "(" #f 'opening))
             (make sequence label: 'close
               (math-token ")" #f 'closing))
             (process-node-list (node-list-rest (node-list-head kids (- n 1)))))
           (make math-sequence (process-children)))))
    ((equal? name "mi")
     (let ((text (token-text (current-node))))
       (math-token text (= (string-length text) 1) 'ordinary)))
    ((equal? name "mn")
     (math-token (token-text (current-node)) #f 'ordinary))
    ((equal? name "mo")
     (math-token (token-text (current-node)) #f (mo-class (current-node))))
    ((equal? name "mtext")
     (make unmath (process-children)))
    ((equal? name "mspace")
     ;; a width in whole em: that many em spaces, else a thin space
     (let* ((w (attribute-string "width"))
            (n (if (and w (> (string-length w) 2)
                        (equal? (substring w (- (string-length w) 2) (string-length w)) "em"))
                   (string->number (substring w 0 (- (string-length w) 2)))
                   #f)))
       (if (and n (>= n 1))
           (let loop ((k n) (out (empty-sosofo)))
             (if (< k 1)
                 out
                 (loop (- k 1) (sosofo-append out (make character char: #\U-2003 math-class: 'space)))))
           (make character char: #\U-2009 math-class: 'space))))
    ((equal? name "mfrac")
     (make fraction
       (make math-sequence label: 'numerator
         (process-node-list (nth-child (current-node) 0)))
       (make math-sequence label: 'denominator
         (process-node-list (nth-child (current-node) 1)))))
    ((equal? name "msup")
     (make script
       (process-node-list (nth-child (current-node) 0))
       (make math-sequence label: 'post-sup
         (process-node-list (nth-child (current-node) 1)))))
    ((equal? name "msub")
     (make script
       (process-node-list (nth-child (current-node) 0))
       (make math-sequence label: 'post-sub
         (process-node-list (nth-child (current-node) 1)))))
    ((equal? name "msubsup")
     (make script
       (process-node-list (nth-child (current-node) 0))
       (make math-sequence label: 'post-sub
         (process-node-list (nth-child (current-node) 1)))
       (make math-sequence label: 'post-sup
         (process-node-list (nth-child (current-node) 2)))))
    ((equal? name "msqrt")
     (make radical (process-children)))
    (else (process-children))))

;; one rule for each element, not a default rule: a default rule here
;; would be more important than every rule of the stylesheet underneath
(element mml:math (mml-print "math"))
(element mml:mrow (mml-print "mrow"))
(element mml:mi (mml-print "mi"))
(element mml:mn (mml-print "mn"))
(element mml:mo (mml-print "mo"))
(element mml:mtext (mml-print "mtext"))
(element mml:mspace (mml-print "mspace"))
(element mml:mfrac (mml-print "mfrac"))
(element mml:msup (mml-print "msup"))
(element mml:msub (mml-print "msub"))
(element mml:msubsup (mml-print "msubsup"))
(element mml:msqrt (mml-print "msqrt"))

</style-specification-body>
</style-specification>

<external-specification id="docbook" document="docbook.dsl">

</style-sheet>
