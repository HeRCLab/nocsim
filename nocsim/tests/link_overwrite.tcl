# this test case verifies that link overwriting behaves as expected

package require tcltest

source ../../scripts/nocsim_load.tcl
namespace import ::nocsim::*

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
	router r3 1 1 dummy
	router r4 1 2 dummy
	router r5 1 3 dummy

	link r3 r4 [dir2int east] [dir2int west]

	if { [catch {link r3 r5 [dir2int east] [dir2int west]} err] } {
		return 0
	} else {
		return 100
	}

} -result 0


tcltest::test 003 {should not be possible to overwrite a link on dest end } -body {
	router r6 2 1 dummy
	router r7 2 2 dummy
	router r8 2 3 dummy

	link r6 r7 [dir2int east] [dir2int west]

	if { [catch {link r8 r7 [dir2int east] [dir2int west]} err] } {
		return 0
	} else {
		return 100
	}

} -result {0}

namespace delete nocsim

