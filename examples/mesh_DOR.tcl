# this simple example shows a small deflection routed network using DOR routing

package require math

# implements DOR routing for a specific link
proc DORfrom {dir} {

	# want to route south
	if {[peek $dir to_row] > [nodeinfo [current] row]} {
		if        {[avail [dir2int south]] == 0} {
			route $dir [dir2int south]
		} elseif {[avail [dir2int east ]] == 0} {
			route $dir [dir2int east]
		} elseif {[avail [dir2int west ]] == 0} {
			route $dir [dir2int west]
		} elseif {[avail [dir2int north]] == 0} {
			route $dir [dir2int north]
		} elseif {[avail [dir2int PE]] == 0} {
			route $dir [dir2int PE]
		} else {
			conswrite "\tno available outgoing links from [current]"
		}
	# want to route north
	} elseif {[peek $dir to_row] < [nodeinfo [current] row]} {
		if        {[avail [dir2int north]] == 0} {
			route $dir [dir2int north]
		} elseif {[avail [dir2int west ]] == 0} {
			route $dir [dir2int west]
		} elseif {[avail [dir2int east ]] == 0} {
			route $dir [dir2int east]
		} elseif {[avail [dir2int south]] == 0} {
			route $dir [dir2int south]
		} elseif {[avail [dir2int PE]] == 0} {
			route $dir [dir2int PE]
		} else {
			conswrite "\tno available outgoing links from [current]"
		}

	# want to route east
	} elseif {[peek $dir to_col] < [nodeinfo [current] col]} {
		if        {[avail [dir2int east ]] == 0} {
			route $dir [dir2int east]
		} elseif {[avail [dir2int south]] == 0} {
			route $dir [dir2int south]
		} elseif {[avail [dir2int north]] == 0} {
			route $dir [dir2int north]
		} elseif {[avail [dir2int west ]] == 0} {
			route $dir [dir2int west]
		} elseif {[avail [dir2int PE]] == 0} {
			route $dir [dir2int PE]
		} else {
			conswrite "\tno available outgoing links from [current]"
		}
	# want to route west
	} elseif {[peek $dir to_col] > [nodeinfo [current] col]} {
		if        {[avail [dir2int west ]] == 0} {
			route $dir [dir2int west]
		} elseif {[avail [dir2int north]] == 0} {
			route $dir [dir2int north]
		} elseif {[avail [dir2int south]] == 0} {
			route $dir [dir2int south]
		} elseif {[avail [dir2int east ]] == 0} {
			route $dir [dir2int east]
		} elseif {[avail [dir2int PE]] == 0} {
			route $dir [dir2int PE]
		} else {
			conswrite "\tno available outgoing links from [current]"
		}

	# our PE is the destination
	} else {
		if {[avail [dir2int PE]] == 0} {
			route $dir [dir2int PE]
		} elseif {[avail [dir2int west ]] == 0} {
			route $dir [dir2int west]
		} elseif {[avail [dir2int north]] == 0} {
			route $dir [dir2int north]
		} elseif {[avail [dir2int south]] == 0} {
			route $dir [dir2int south]
		} elseif {[avail [dir2int east ]] == 0} {
			route $dir [dir2int east]
		} else {
			conswrite "\tno available outgoing links from [current]"
		}
	}
}

proc simpleDOR {} {
	if {[incoming [dir2int north]]} {DORfrom [dir2int north]}
	if {[incoming [dir2int south]]} {DORfrom [dir2int south]}
	if {[incoming [dir2int east]]}  {DORfrom [dir2int east]}
	if {[incoming [dir2int west]]}  {DORfrom [dir2int west]}
	if {[incoming [dir2int PE]]}    {DORfrom [dir2int PE]}

}

proc with_P {P} {
	if { [::math::random] <= $P } { return 1 } else {return 0}
}

proc simpleinject {} {
	if {[with_P 0.3]} { inject [randnode [current]] }
}

proc on_arrive {origin dest flitno hops spawned_at injected_at} {
	conswrite "flit $flitno arrives at $dest after $hops hops"
}

proc on_route {origin dest flitno spawnedat injectedat hops fromnode tonode} {
	upvar #0 nocsim_tick nocsim_tick
	if {$flitno == 37} {
		conswrite "(tick=$nocsim_tick) flitno $flitno routed from $fromnode to $tonode"
	}
}

registerinstrument arrive on_arrive
registerinstrument route on_route

create_mesh 10 10 simpleinject simpleDOR

step 5000

conswrite "routed $nocsim_routed flits"
conswrite "$nocsim_arrived flits arrived"
conswrite "$nocsim_spawned flits spawned"
conswrite "$nocsim_injected flits injected"
conswrite "throughput = [expr (1.0 * $nocsim_routed /  $nocsim_num_PE) / $nocsim_tick ] flits per PE per cycle"
