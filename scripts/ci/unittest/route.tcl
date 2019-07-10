# test that a flit can be routed correctly

package require tcltest


proc b_inject {} {
	inject p2
}

proc nop {} {

}

proc r1 {} {
	if [incoming [dir2int pe]] {
		route [dir2int pe] [dir2int east]
	}
}

proc r2 {} {
	if [incoming [dir2int west]] {
		route [dir2int west] [dir2int pe]
	}
}

proc arr_instr {origin dest flitno hops spawned injected} {
	upvar 1 arrived arrived
	set arrived 1
}

tcltest::test 001 {should be able to route a flit} -body {
	set arrived 0
	router r1 0 0 r1
	router r2 0 1 r2
	PE p1 0 0 b_inject
	PE p2 0 1 nop
	link p1 r1
	link r1 r2
	link r2 p2
	registerinstrument arrive arr_instr
	step 3

	return $arrived
} -result {1}
