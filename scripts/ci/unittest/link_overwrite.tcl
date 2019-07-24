# this test case verifies that link overwriting behaves as expected

package require tcltest

tcltest::test 001 {should not be possible to overwrite a link on both ends} -body {
	router r1 0 0 dummy
	router r2 0 1 dummy
	link r1 r2
	if { [catch {link r1 r2} err] } {
		return 0
	} else {
		return 100
	}
} -result 0

tcltest::test 002 {should not be possible to overwrite a link on source end } -body {
	router r2 0 1 dummy
	router r3 0 2 dummy
	router r4 0 3 dummy

	link r2 r3

	if { [catch {link r2 r4} err] } {
		return 0
	} else {
		return 100
	}

} -result 0


tcltest::test 003 {should not be possible to overwrite a link on dest end } -body {
	router r5 1 1 dummy
	router r6 1 2 dummy
	router r7 1 3 dummy

	link r5 r7

	if { [catch {link r7 r6} err] } {
		return 0
	} else {
		return 100
	}
} -result {0}
