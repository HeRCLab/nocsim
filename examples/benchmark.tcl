source "../scripts/noc_tools_load.tcl"

# This is a simple program that attempts to measure how many updates per second
# nocviz can sustain.

nocviz::launch_gui

after 1000

nocviz::node create n1
nocviz::node data set n1 string_field "this is a string!"
nocviz::node data set n1 int_field 7
nocviz::node data set n1 float_field 1234.5678
nocviz::node data set n1 formatted_float_field 1234.5678
nocviz::node data fmt n1 formatted_float_field %2.2f
nocviz::node data show n1 example string_field int_field float_field formatted_float_field
nocviz::node row n1 5
nocviz::node col n1 5

nocviz::node create n2
nocviz::node data set n2 string_field "formatting works on strings too..."
nocviz::node data set n2 float_field "1234.5678"
nocviz::node data set n2 formatted_float_field "1234.5678"
nocviz::node data fmt n2 formatted_float_field %2.2f
nocviz::node data show n2 example string_field float_field formatted_float_field
nocviz::node row n2 5
nocviz::node col n2 4

nocviz::node create random
nocviz::node data set random num1 ""
nocviz::node data set random num2 ""
nocviz::node data set random num3 ""
nocviz::node data set random num4 ""
nocviz::node data set random num5 ""
nocviz::node data set random "elapsed (ms)" "0"
nocviz::node data set random "updates" "0"
nocviz::node data set random "updates/s (avg)" "0"
nocviz::node data show random "some random numbers" num1 num2 num3 num4 num5
nocviz::node data show random "performance measurements" "elapsed (ms)" "updates" "updates/s (avg)"
nocviz::node data fmt random "updates/s (avg)" "%3.2f"

set st [clock clicks -milliseconds]
set updates 0

while {1} {
    nocviz::node data set random num1 [expr rand()]
    nocviz::node data set random num2 [expr rand()]
    nocviz::node data set random num3 [expr rand()]
    nocviz::node data set random num4 [expr rand()]
    nocviz::node data set random num5 [expr rand()]
    set now [clock clicks -milliseconds]
    incr updates
    nocviz::node data set random "elapsed (ms)" [expr $now - $st]
    nocviz::node data set random updates $updates
    nocviz::node data set random "updates/s (avg)" [expr ( 1.0 * $updates) / ( ( 1.0 * $now - 1.0 * $st + 1.0) / 1000.0 ) ]
    # after 100
}
