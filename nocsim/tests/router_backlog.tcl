# test that flit buffering in routers works correctly.
source ../../scripts/noc_tools_load.tcl

# * import here to allow this legacy test to run.
namespace import ::nocsim::*

package require tcltest

# Network topology:
#
# p1 -- r1 ---
#             \
# p2 -- r2 -- r4 -- r5 -- p5
#             /
# p3 -- r3 ---
#

set has_injected(0) false

# Each PE puts its name in the has_injected array after injecting a packet.
proc inject_once {} {
	global has_injected
	set current [current]
	set already_injected [lindex [array get has_injected $current] 1]
	# Only inject if we haven't done so before.
	if {$already_injected eq ""} {
		inject p5
		set has_injected($current) true
	}
}

proc nop {} {
}

# Router behaviors
proc route_to_r4 {} {
	# Try all possible link directions to find the valid one.
	# This works here because only one link exists to r4 per router.
	if [incoming [dir2int pe]] {
		foreach direction {N E S W} {
			set direction [dir2int $direction]
			if {[avail $direction] == 0} {
			puts "[current]: Routing packet from PE->[int2dir $direction]"
				route [dir2int pe] $direction
				break
			}
		}
	}
}

proc route_to_r5_with_backlog {} {
	foreach direction {N W S E} {
		set direction [dir2int $direction]
		# Try to route flits east.
		# If link is occupied, put flit in the backlog buffer.
		if {[incoming $direction] == 1} {
			if {[avail [dir2int east]] == 0} {
				puts "[current]: Routing packet from [int2dir $direction]->E"
				route $direction [dir2int east]
			} else {
				puts "[current]: Routing packet from [int2dir $direction]->backlog"
				route $direction [dir2int backlog]
			}
		}
	}
	# If east link is open, drain the backlog buffer.
	#puts "Evaluating backlog drain"
	if {[incoming [dir2int backlog]] && [avail [dir2int east]] == 0} {
		puts "[current]: Routing packet from backlog->E"
		route [dir2int backlog] [dir2int east]
	}
}

proc route_to_p5 {} {
	if [incoming [dir2int west]] {
		puts "[current]: Routing packet from W->PE"
		route [dir2int west] [dir2int pe]
	}
}

# Instruments
proc arr_instr {origin dest flitno hops spawned injected} {
	upvar #0 arrived arrived
	set arrived 1
}

proc inject_instr {origin dest flitno} {
	upvar #0 injected injected
	set injected 1
}

proc route_instr {origin dest flitno spawned injected hops routefrom routeto} {
	upvar #0 routed routed
	set routed 1
}

# Test case(s)
tcltest::test 001 {should be able to route a flit} -body {
	set arrived 0
	set routed 0
	set injected 0
	router r1 0 0 route_to_r4
	router r2 1 0 route_to_r4
	router r3 2 0 route_to_r4
	router r4 1 1 route_to_r5_with_backlog
	router r5 1 2 route_to_p5
	PE p1 0 0 inject_once
	PE p2 1 0 inject_once
	PE p3 2 0 inject_once
	PE p5 1 2 nop
	link p1 r1
	link p2 r2
	link p3 r3
	link r1 r4
	link r2 r4
	link r3 r4
	link r4 r5
	link r5 p5
	registerinstrument arrive arr_instr
	registerinstrument inject inject_instr
	registerinstrument route route_instr
	step 10

	return [ expr $arrived + $routed + $injected ]
} -result {3}

namespace delete nocsim
namespace delete nocviz
