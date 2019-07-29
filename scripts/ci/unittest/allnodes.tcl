# test the allnodes function

package require tcltest

proc nop {} {}

tcltest::test 001 {allnodes should work} -body {
	router r1 0 0 nop
	router r2 0 1 nop
	router r3 0 2 nop
	PE p1 0 0 nop

	if {[lsearch -exact [allnodes] "r1"] < 0} {return 100}
	if {[lsearch -exact [allnodes] "r2"] < 0} {return 200}
	if {[lsearch -exact [allnodes] "r3"] < 0} {return 300}
	if {[lsearch -exact [allnodes] "p1"] < 0} {return 400}

	return 0
} -result {0}
