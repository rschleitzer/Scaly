<![CDATA[
;; Rules for generating Scaly parser from scaly.sgm

(element grammar
    (file "compiler/parser.scaly"
        ($
            (generate-syntax-scaly)
            (generate-parser-scaly)
        )
    )
)
]]>
