package require tcltest

source ../../scripts/noc_tools_load.tcl
namespace import ::nocsim::*

tcltest::test 001 {dir2list should work correctly with valid inputs} -body {
	set mylist {north south east west pe}
	set intlist [dir2list {*}$mylist]
	set expected {}

	lappend expected [dir2int north]
	lappend expected [dir2int south]
	lappend expected [dir2int east]
	lappend expected [dir2int west]
	lappend expected [dir2int pe]

	conswrite "converted list: $intlist"
	conswrite "expected: $expected"

	if { [lindex $intlist 0 ] != [lindex $expected 0] } { return 100; }
	if { [lindex $intlist 1 ] != [lindex $expected 1] } { return 200; }
	if { [lindex $intlist 2 ] != [lindex $expected 2] } { return 300; }
	if { [lindex $intlist 3 ] != [lindex $expected 3] } { return 400; }
	if { [lindex $intlist 4 ] != [lindex $expected 4] } { return 500; }

	return 0;

} -result {0}

tcltest::test 002 {dir2list should error with invalid inputs} -body {

	if { [ catch {dir2list north invalid south} err ] } {
		return 0
	} else {
		return 100
	}

} -result {0}

namespace delete nocsim
namespace delete nocviz
