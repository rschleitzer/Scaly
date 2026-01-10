<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY testcpp   SYSTEM "testcpp.scm">
<!ENTITY testdoc   SYSTEM "testdoc.scm">
<!ENTITY helpers   SYSTEM "helpers.scm">
<!ENTITY fodeclare SYSTEM "fodeclare.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&fodeclare;
&helpers;
&testcpp;
&testdoc;

<![CDATA[
(element suite
    (sosofo-append
        (file "scalyc/ControlFlowTests.h"
            (generate-tests-h "ControlFlow")
        )
        (file "scalyc/ControlFlowTests.cpp"
            (generate-tests-cpp "ControlFlow")
        )
        (file "docs/scaly/generated-controlflow.xml"
            (generate-testdoc)
        )
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
