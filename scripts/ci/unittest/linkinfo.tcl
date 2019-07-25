# verify that linkinfo works correctly

package require tcltest

tcltest::test 001 {test that linkinfo works properly} -body {
	router r1 0 0 dummy
	router r2 0 1 dummy
	link r1 r2

	if { [ llength [linkinfo r1 r2 in_flight] ] != 0 } { return 100 }
	if { [linkinfo r1 r2 current_load] != 0 } {return 200}
	if { [linkinfo r1 r2 load] != 0} {return 300}
	if { ! [catch {linkinfo r2 r1 load} err] } { return 400}
	if { ! [catch {linkinfo r1 r3 load} err] } { return 500}
	if { ! [catch {linkinfo r3 r1 load} err] } { return 600}
	return 0
} -result {0}

