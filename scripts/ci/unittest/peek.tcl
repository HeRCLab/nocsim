# test that the peek command is working

package require tcltest

proc b_inject {} {
	inject pe2
}

proc nop {} {

}

proc r1 {} {
	upvar #0 res res

	if [incoming [dir2int pe]] {
		conswrite "peek from, to: [peek [dir2int pe] from] [peek [dir2int pe] to]"
		if {[peek [dir2int pe] from] != "pe1"} { set res 1; drop [dir2int pe]; return}
		if {[peek [dir2int pe] to] != "pe2"} { set res 2; drop [dir2int pe]; return}
		if {[peek [dir2int pe] from_row] != 0} { set res 3; drop [dir2int pe]; return}
		if {[peek [dir2int pe] from_col] != 1} { set res 4; drop [dir2int pe]; return}
		if {[peek [dir2int pe] to_row] != 2} { set res 5; drop [dir2int pe]; return}
		if {[peek [dir2int pe] to_col] != 3} { set res 6; drop [dir2int pe]; return}
		drop [dir2int pe]
		set res 0
	}
}


tcltest::test 001 {peek command should work} -body {
	set res -1
	router r1 0 1 r1
	PE pe1 0 1 b_inject
	PE pe2 2 3 nop
	link pe1 r1
	step 2
	return $res
} -result {0}
