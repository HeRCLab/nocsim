# test that the avail command is working correctly

source ../../scripts/nocsim_load.tcl

namespace import ::nocsim::*

package require tcltest

proc b_inject {} {
	inject p2
}

proc nop {} {

}

proc r1 {} {
	upvar #0 res res

	if [incoming [dir2int pe]] {
		route [dir2int pe] [dir2int east]
		if {[avail [dir2int east]] != 1} {set res 1 ; return}
		if {[avail [dir2int west]] != 2} {set res 2 ; return}
		if {[avail [dir2int south]] != 0} {set res 3 ; return}
		set res 0
	}

}

proc dropall {} {
	foreach dir [allincoming] {
		drop $dir
	}
}

tcltest::test 001 {avail should work} -body {
	set res -1
	router r1 0 0 r1
	router r2 0 1 dropall
	router r3 1 0 dropall
	PE p1 0 0 b_inject
	PE p2 0 1 nop
	link p1 r1
	link r1 r2
	link r1 r3
	step 2
	return $res
} -result {0}

namespace delete nocsim

