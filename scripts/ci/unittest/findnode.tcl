# test that the findnode command works

package require tcltest

tcltest::test 001 {findnode should return all nodes} -body {
	router r001a 0 0 dummy
	router r001b 0 1 dummy
	router r001c 0 2 dummy

	# should find all three nodes
	if {[lsearch -exact [findnode] r001a] < 0} { return 1 }
	if {[lsearch -exact [findnode] r001b] < 0} { return 2 }
	if {[lsearch -exact [findnode] r001c] < 0} { return 3 }
	return 0

} -result {0}

tcltest::test 002 {findnode ROW COL work} -body {
	router r002a 0 0 dummy
	router r002b 0 0 dummy
	router r002c 0 1 dummy

	# should find a and b, but not c
	if {[lsearch -exact [findnode 0 0 ] r002a] < 0} { return 1 }
	if {[lsearch -exact [findnode 0 0 ] r002b] < 0} { return 2 }
	if {[lsearch -exact [findnode 0 0 ] r002c] >= 0} { return 3 }
	return 0

} -result {0}

tcltest::test 003 {findnode ROWL ROWU COLL COLU} -body {
	router r003a 0 0 dummy
	router r003b 0 1 dummy
	router r003c 0 2 dummy
	router r003d 1 0 dummy
	router r003e 1 1 dummy
	router r003f 1 2 dummy
	router r003g 2 0 dummy
	router r003h 2 1 dummy
	router r003i 2 2 dummy

	set res [findnode 0 1 0 1]

	# should find a,b,d,e, and no others
	if {[lsearch -exact $res r003a] < 0} { return 1 }
	if {[lsearch -exact $res r003b] < 0} { return 2 }
	if {[lsearch -exact $res r003d] < 0} { return 3 }
	if {[lsearch -exact $res r003e] < 0} { return 4 }
	if {[lsearch -exact $res r003c] >= 0} {return 5}
	if {[lsearch -exact $res r003f] >= 0} {return 6}
	if {[lsearch -exact $res r003g] >= 0} {return 7}
	if {[lsearch -exact $res r003h] >= 0} {return 8}
	if {[lsearch -exact $res r003i] >= 0} {return 9}
	return 0

} -result {0}
