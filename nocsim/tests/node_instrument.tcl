package require tcltest

source ../../scripts/noc_tools_load.tcl
namespace import ::nocsim::*

proc testinstrument {id type row col behavior} {
	set ::node_list [list $id $type $row $col $behavior]
}

tcltest::test 001 {node instruments should run when router instantiated} -body {
	set ::node_list {}
	registerinstrument node testinstrument
	router r001a 0 1 dummy
	if { ! [string equal [lindex $::node_list 0] "r001a"] } { return 100; }
	if { ! [string equal [lindex $::node_list 1] [type2int router]] } { return 100; }
	if { ! [string equal [lindex $::node_list 2] "0"] } { return 100; }
	if { ! [string equal [lindex $::node_list 3] "1"] } { return 100; }
	if { ! [string equal [lindex $::node_list 4] "dummy"] } { return 100; }

	return 0

} -result {0}

tcltest::test 002 {node instruments should run when PE instantiated} -body {
	set ::node_list {}
	registerinstrument node testinstrument
	PE PE002a 2 3 dummy

	if { ! [string equal [lindex $::node_list 0] "PE002a"] } { return 100; }
	if { ! [string equal [lindex $::node_list 1] [type2int PE]] } { return 100; }
	if { ! [string equal [lindex $::node_list 2] "2"] } { return 100; }
	if { ! [string equal [lindex $::node_list 3] "3"] } { return 100; }
	if { ! [string equal [lindex $::node_list 4] "dummy"] } { return 100; }

	return 0
} -result {0}

namespace delete nocsim
namespace delete nocviz
