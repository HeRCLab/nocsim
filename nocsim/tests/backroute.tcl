# test that backrouting works correctly

source ../../scripts/noc_tools_load.tcl
namespace import ::nocsim::*

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

proc spawn_instr {origin dest flitno} {
	upvar #0 spawned spawned
	conswrite "spawn flit $flitno"
	set spawned [ expr $spawned + 1 ]
}

proc route_instr {origin dest flitno spawned injected hops routefrom routeto} {
	upvar #0 routed routed
	set routed [ expr $routed + 1 ]
}

proc backroute_instr {origin dest flitno spawned injected hops} {
	upvar #0 backrouted backrouted
	conswrite "backroute flit $flitno"
	incr backrouted
}

proc inject_instr {origin dest flitno} {
	upvar #0 injected injected
	conswrite "inject flit $flitno"
	set injected [expr $injected + 1]
}

proc dequeue_instr {origin dest flitno} {
	upvar #0 dequeued dequeued
	conswrite "dequeue flit $flitno"
	set dequeued [expr $dequeued + 1]
}

tcltest::test 001 {should be able to route a flit} -body {
	set backrouted 0
	set routed 0
	set spawned 0
	set injected 0
	set dequeued 0
	router r1 0 0 r1
	PE p1 0 0 b_inject
	PE p2 0 0 nop
	link r1 p1
	link p1 r1
	registerinstrument backroute backroute_instr
	registerinstrument spawn spawn_instr
	registerinstrument route route_instr
	registerinstrument dequeue dequeue_instr
	registerinstrument inject inject_instr
	step 2

	conswrite "counters: $backrouted $routed $spawned $injected $dequeued"

	# we routed one flit, which was backrouted, the other flit is en-route
	# from the PE to the router and hasn't been routed yet
	if { $backrouted != 1 } { return 100; }
	if { $routed != 1 } { return 200; }

	# we spawned two flits, the second of which is currently in the
	# outgoing link from the PE
	if { $spawned != 2 } { return 300; }

	# we injected no flits, since the only flit we ever routed was
	# backrouted
	if { $injected != 0 } { return 400; }

	# we DE queued two flits, since neither had to wait after being spawned
	if { $dequeued != 2 } { return 500; }

	return 0;

} -result {0}

namespace delete nocsim
namespace delete nocviz
