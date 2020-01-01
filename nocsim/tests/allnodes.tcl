# test the allnodes function
#
source ../../scripts/nocsim_load.tcl

package require tcltest

proc nop {} {}

tcltest::test 001 {allnodes should work} -body {
	nocsim::router r1 0 0 nop
	nocsim::router r2 0 1 nop
	nocsim::router r3 0 2 nop
	nocsim::PE p1 0 0 nop

	if {[lsearch -exact [nocsim::allnodes] "r1"] < 0} {return 100}
	if {[lsearch -exact [nocsim::allnodes] "r2"] < 0} {return 200}
	if {[lsearch -exact [nocsim::allnodes] "r3"] < 0} {return 300}
	if {[lsearch -exact [nocsim::allnodes] "p1"] < 0} {return 400}

	return 0
} -result {0}

namespace delete nocsim

