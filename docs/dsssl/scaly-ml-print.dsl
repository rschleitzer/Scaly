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
;; parentheses around an mrow a fence --, a letter of an mi its mathematical
;; italic, and each mo carries its math class (binary, relation, opening,
;; ...), from which the engine spaces the formula. The pictures are those of
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

(define %accent% (color (color-space "ISO/IEC 10179:1996//Color-Space Family::Device RGB")
                        (/ 42 255) (/ 93 255) (/ 176 255)))

;; ----------------------------------------------------------------- the book

;; the abstract of the book is its lead, before the first chapter
(element info (empty-sosofo))

;; a link to a page: its text in the accent colour (the engine's PDF has
;; links within the document only)
(element link
  (let ((href (attribute-string "xlink:href"))
        (linkend (attribute-string "linkend")))
    (if href
        (make sequence
          color: %accent%
          (process-children))
        (let ((target (if linkend (element-with-id linkend) (empty-node-list))))
          (if (node-list-empty? target)
              (process-children)
              (make link
                destination: (node-list-address target)
                (process-children)))))))

;; "Try it": a box with its word above, in the accent colour
(element sidebar
  (if (equal? (attribute-string "role") "tryit")
      (make box
        display?: #t
        box-type: 'border
        line-thickness: 0.6pt
        color: %accent%
        space-before: %block-sep%
        space-after: %block-sep%
        start-indent: (inherited-start-indent)
        end-indent: (inherited-end-indent)
        (make display-group
          start-indent: (+ (inherited-start-indent) 8pt)
          end-indent: (+ (inherited-end-indent) 8pt)
          space-before: 6pt
          space-after: 6pt
          color: (color (color-space "ISO/IEC 10179:1996//Color-Space Family::Device RGB") 0 0 0)
          (make paragraph
            font-family-name: %title-font-family%
            font-weight: 'bold
            font-size: (* %bf-size% 0.8)
            color: %accent%
            space-after: 3pt
            keep-with-next?: #t
            (literal (if (german?) "AUSPROBIEREN" "TRY IT")))
          (process-children)))
      (make box
        display?: #t
        box-type: 'border
        line-thickness: 0.6pt
        (process-children))))

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
                max-width: 4.5cm
                max-height: 4.5cm
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

;; the mathematical italic of a Latin or Greek letter (U+1D44E on; the
;; italic h is Planck's constant, U+210E, the block leaves its place empty);
;; by table, as the engine, like jade, has no char->integer
(define %upright% "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZαβγδεζηθικλμνξοπρςστυφχψω")
(define %italic% "𝑎𝑏𝑐𝑑𝑒𝑓𝑔ℎ𝑖𝑗𝑘𝑙𝑚𝑛𝑜𝑝𝑞𝑟𝑠𝑡𝑢𝑣𝑤𝑥𝑦𝑧𝐴𝐵𝐶𝐷𝐸𝐹𝐺𝐻𝐼𝐽𝐾𝐿𝑀𝑁𝑂𝑃𝑄𝑅𝑆𝑇𝑈𝑉𝑊𝑋𝑌𝑍𝛼𝛽𝛾𝛿𝜀𝜁𝜂𝜃𝜄𝜅𝜆𝜇𝜈𝜉𝜊𝜋𝜌𝜍𝜎𝜏𝜐𝜑𝜒𝜓𝜔")

(define (math-italic c)
  (let loop ((i 0))
    (cond ((>= i (string-length %upright%)) c)
          ((equal? (string-ref %upright% i) c) (string-ref %italic% i))
          (else (loop (+ i 1))))))

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
  (let* ((up (parent nd))
         (sibs (element-children up))
         (n (node-list-length sibs)))
    (cond ((<= n 1) 'infix)
          ((node-list=? nd (node-list-first sibs)) 'prefix)
          ((node-list=? nd (node-list-ref sibs (- n 1))) 'postfix)
          (else 'infix))))

(define (node-list=? a b)
  (equal? (all-element-number a) (all-element-number b)))

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

(define (math-token text italic? class)
  (make sequence
    font-family-name: %math-font%
    font-posture: 'upright
    font-weight: 'medium
    (let loop ((cs (string->list text)) (out (empty-sosofo)))
      (if (null? cs)
          out
          (loop (cdr cs)
                (sosofo-append out
                  (make character
                    char: (if italic? (math-italic (car cs)) (car cs))
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
     (make character char: #\U-2009 math-class: 'space))
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
