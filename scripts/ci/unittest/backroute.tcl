# test that backrouting works correctly

package require tcltest


proc b_inject {} {
	inject p2
}

proc nop {} {

}

proc r1 {} {
	if [incoming [dir2int pe]] {
		route [dir2int pe] [dir2int pe]
	}
}

proc inject_instr {origin dest flitno} {
	upvar #0 injected injected
	set injected 1
}

proc route_instr {origin dest flitno spawned injected hops routefrom routeto} {
	upvar #0 routed routed
	set routed 1
}

proc backroute_instr {origin dest flitno spawned injected hops} {
	upvar #0 backrouted backrouted
	set backrouted 1
}

tcltest::test 001 {should be able to route a flit} -body {
	set backrouted 0
	set routed 0
	set injected 0
	router r1 0 0 r1
	PE p1 0 0 b_inject
	PE p2 0 0 nop
	link r1 p1
	link p1 r1
	registerinstrument backroute backroute_instr
	registerinstrument inject inject_instr
	registerinstrument route route_instr
	step 2

	return [ expr $backrouted + $routed + $injected ]
} -result {3}
