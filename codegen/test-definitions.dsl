<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY testcpp   SYSTEM "testcpp.scm">
<!ENTITY testscaly SYSTEM "testscaly.scm">
<!ENTITY testdoc   SYSTEM "testdoc.scm">
<!ENTITY helpers   SYSTEM "helpers.scm">
<!ENTITY fodeclare SYSTEM "fodeclare.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&fodeclare;
&helpers;
&testcpp;
&testscaly;
&testdoc;

<![CDATA[
(element suite
    (sosofo-append
        (file "scalyc/DefinitionTests.h"
            (generate-tests-h "Definition")
        )
        (file "scalyc/DefinitionTests.cpp"
            (generate-tests-cpp "Definition")
        )
        (file "docs/scaly/generated-definitions.xml"
            (generate-testdoc)
        )
        (generate-selfhosted-tests "definitions")
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
