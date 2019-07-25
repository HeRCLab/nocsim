# this simple example shows a small deflection routed network using DOR routing

package require math

# implements DOR routing for a specific link
proc DORfrom {dir} {
	conswrite "\trouting from direction [int2dir $dir]"

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
	return ::math::random <= $P
}

proc simpleinject {} {
	if {[with_P 0.3]} { inject [randnode [current]] }
}

create_mesh 5 5 simpleinject simpleDOR

step 5000

conswrite "throughput = [expr $nocsim_routed / ( $nocsim_num_PE * $nocsim_tick ) ] flits per PE per cycle"
