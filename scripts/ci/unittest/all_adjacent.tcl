# test the all_adjacent function

package require tcltest

proc nop {} {

}

tcltest::test 001 {all_adjacent should work} -body {

	router r1 1 1 nop

	# north of r1
	router r2 0 1 nop

	# west of r1
	router r3 1 0 nop

	# south of r1
	router r4 2 1 nop

	# east of r1
	router r5 1 2 nop

	# not adjacent to r1
	router r6 0 0 nop
	router r7 2 2 nop

	set r1_adj [all_adjacent r1]

	# ensure r1 adj list contains these
	if {[lsearch -exact $r1_adj "r2"] < 0} { return 100 }
	if {[lsearch -exact $r1_adj "r3"] < 0} { return 200 }
	if {[lsearch -exact $r1_adj "r4"] < 0} { return 300 }
	if {[lsearch -exact $r1_adj "r5"] < 0} { return 400 }

	# ... and not these
	if {[lsearch -exact $r1_adj "r6"] >= 0} { return 500 }
	if {[lsearch -exact $r1_adj "r7"] >= 0} { return 600 }

	set r2_adj [all_adjacent r2]

	if {[lsearch -exact $r2_adj "r1"] < 0} { return 700 }
	if {[lsearch -exact $r2_adj "r6"] < 0} { return 800 }

	if {[lsearch -exact $r2_adj "r2"] >= 0} { return 900 }
	if {[lsearch -exact $r2_adj "r3"] >= 0} { return 1000 }
	if {[lsearch -exact $r2_adj "r4"] >- 0} { return 1100 }
	if {[lsearch -exact $r2_adj "r5"] >= 0} { return 1200 }
	if {[lsearch -exact $r2_adj "r7"] >= 0} { return 1300 }


	return 0

} -result {0}
