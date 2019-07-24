# Test that the inject instrument is working properly. This is a bit tricky,
# since we have to make sure that back routing doesn't count

package require tcltest


proc b_inject {} {
	inject p4
}

proc nop {} {

}

proc r1 {} {
	if [incoming [dir2int pe]] {
		route [dir2int pe] [dir2int pe]
	}
}

proc r2 {} {
	if [incoming [dir2int pe]] {
		route [dir2int pe] [dir2int n]
	}
}

proc r3 {} {
	if [incoming [dir2int s]] {
		route [dir2int s] [dir2int pe]
	}
}

proc spawn_instr {origin dest flitno} {
	upvar #0 spawned spawned
	conswrite "spawn flit $flitno at $origin"
	incr spawned
}

proc route_instr {origin dest flitno spawned injected hops routefrom routeto} {
	upvar #0 routed routed
	conswrite "route $flitno from $routefrom to $routeto"
	incr routed
}

proc backroute_instr {origin dest flitno spawned injected hops} {
	upvar #0 backrouted backrouted
	conswrite "backroute flit $flitno"
	incr backrouted
}

proc inject_instr {origin dest flitno} {
	upvar #0 injected injected
	conswrite "inject flit $flitno"
	incr injected
}

proc dequeue_instr {origin dest flitno} {
	upvar #0 dequeued dequeued
	conswrite "dequeue flit $flitno"
	incr dequeued
}

proc arrive_instr {origin dest flitno nhops spawnedat injectedat} {
	upvar #0 arrived arrived
	conswrite "$flitno arrived at $dest after $nhops hops"
	incr arrived

}

proc print_nodeinfo {nodeid} {
	conswrite "!!! nodeinfo $nodeid"
	foreach attr {type row col behavior routed spawned injected dequeued backrouted arrived} {
		conswrite "\t$attr = [nodeinfo $nodeid $attr]"
	}

}

tcltest::test 001 {should be able to route a flit} -body {
	set backrouted 0
	set routed 0
	set spawned 0
	set injected 0
	set dequeued 0
	set arrived 0

	# this sub-graph only ever backroutes
	router r1 0 0 r1
	PE p1 0 0 b_inject
	PE p2 0 0 nop
	link r1 p1
	link p1 r1

	# this sub-graph only ever routes 
	router r2 1 1 r2
	PE p3 1 1 b_inject
	router r3 0 1 r3
	PE p4 0 1 nop
	link p3 r2
	link r2 r3
	link r3 p4

	registerinstrument backroute backroute_instr
	registerinstrument spawn spawn_instr
	registerinstrument route route_instr
	registerinstrument dequeue dequeue_instr
	registerinstrument inject inject_instr
	registerinstrument arrive arrive_instr

	step 10

	conswrite "!!! global performance counters"
	foreach var {backrouted routed spawned injected dequeued arrived} {
		conswrite "\t$var = [subst $$var]"
	}

	foreach var {nocsim_backrouted nocsim_routed nocsim_spawned nocsim_injected nocsim_dequeued nocsim_arrived} {
		conswrite "\t$var = [subst $$var]"
	}

	# we should have backrouted 9 times on the first subgraph
	if { $backrouted != 9 } { return 100; }

	# we should have routed 9 times on the first subgraph. On the second,
	# we will have routed from from r2 to r3 9 times, and from r3 to p4 8
	# times, for a total of 9+9+8 = 26 route route events. Remember that
	# flits traveling from a PE after being dequeued are not counted as
	# having been routed.
	if { $routed != 26 } { return 200; }

	# we should have spawned 20 times on each subgraph
	if { $spawned != 20 } { return 300; }

	# we should have injected 9 flits, all on the second subgraph
	if { $injected != 9 } { return 400; }

	# we should have dequeued 10 flits on each subgraph
	if { $dequeued != 20 } { return 500; }

	# 8 flits should have arrived via r3 to p4
	if { $arrived != 8} { return 600; }

	# assert that the relevant performance counters match the number of
	# times each instrument was called
	if { $backrouted != $nocsim_backrouted } { return 700;  }
	if { $routed     != $nocsim_routed     } { return 800;  }
	if { $spawned    != $nocsim_spawned    } { return 900;  }
	if { $injected   != $nocsim_injected   } { return 1000; }
	if { $dequeued   != $nocsim_dequeued   } { return 1100; }
	if { $arrived    != $nocsim_arrived    } { return 1200; }


	print_nodeinfo p1
	print_nodeinfo p2
	print_nodeinfo p3
	print_nodeinfo p4
	print_nodeinfo r1
	print_nodeinfo r2
	print_nodeinfo r3

	# assert that every performance counter matches what we expect
	if { [nodeinfo p1 spawned]    != 10 } { return 1300; }
	if { [nodeinfo p1 routed]     != 0  } { return 1400; }
	if { [nodeinfo p1 injected]   != 0  } { return 1500; }
	if { [nodeinfo p1 dequeued]   != 10 } { return 1600; }
	if { [nodeinfo p1 arrived]    != 0  } { return 1700; }
	if { [nodeinfo p1 backrouted] != 9  } { return 1800; }

	if { [nodeinfo p2 spawned]    != 0  } { return 1900; }
	if { [nodeinfo p2 routed]     != 0  } { return 2000; }
	if { [nodeinfo p2 injected]   != 0  } { return 2100; }
	if { [nodeinfo p2 dequeued]   != 0  } { return 2200; }
	if { [nodeinfo p2 arrived]    != 0  } { return 2300; }
	if { [nodeinfo p2 backrouted] != 0  } { return 2400; }

	if { [nodeinfo p3 spawned]    != 10 } { return 2400; }
	if { [nodeinfo p3 routed]     != 0  } { return 2500; }
	if { [nodeinfo p3 injected]   != 9  } { return 2600; }
	if { [nodeinfo p3 dequeued]   != 10 } { return 2700; }
	if { [nodeinfo p3 arrived]    != 0  } { return 2800; }
	if { [nodeinfo p3 backrouted] != 0  } { return 2900; }

	if { [nodeinfo p4 spawned]    != 0  } { return 3000; }
	if { [nodeinfo p4 routed]     != 0  } { return 3100; }
	if { [nodeinfo p4 injected]   != 0  } { return 3200; }
	if { [nodeinfo p4 dequeued]   != 0  } { return 3300; }
	if { [nodeinfo p4 arrived]    != 8  } { return 3400; }
	if { [nodeinfo p4 backrouted] != 0  } { return 3500; }

	if { [nodeinfo r1 spawned]    != 0  } { return 3600; }
	if { [nodeinfo r1 routed]     != 9  } { return 3700; }
	if { [nodeinfo r1 injected]   != 0  } { return 3800; }
	if { [nodeinfo r1 dequeued]   != 0  } { return 3900; }
	if { [nodeinfo r1 arrived]    != 0  } { return 4000; }
	if { [nodeinfo r1 backrouted] != 9  } { return 4100; }

	if { [nodeinfo r2 spawned]    != 0  } { return 4200; }
	if { [nodeinfo r2 routed]     != 9  } { return 4300; }
	if { [nodeinfo r2 injected]   != 0  } { return 4400; }
	if { [nodeinfo r2 dequeued]   != 0  } { return 4500; }
	if { [nodeinfo r2 arrived]    != 0  } { return 4600; }
	if { [nodeinfo r2 backrouted] != 0  } { return 4700; }

	if { [nodeinfo r3 spawned]    != 0  } { return 4800; }
	if { [nodeinfo r3 routed]     != 8  } { return 4900; }
	if { [nodeinfo r3 injected]   != 0  } { return 5000; }
	if { [nodeinfo r3 dequeued]   != 0  } { return 5100; }
	if { [nodeinfo r3 arrived]    != 0  } { return 5200; }
	if { [nodeinfo r3 backrouted] != 0  } { return 5300; }


	return 0;

} -result {0}
