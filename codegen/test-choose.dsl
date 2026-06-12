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
        (file "scalyc/ChooseTests.h"
            (generate-tests-h "Choose")
        )
        (file "scalyc/ChooseTests.cpp"
            (generate-tests-cpp "Choose")
        )
        (file "docs/scaly/generated-choose.xml"
            (generate-testdoc)
        )
        (generate-selfhosted-tests "choose")
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
