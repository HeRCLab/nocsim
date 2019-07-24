# this simple example shows a small deflection routed network using DOR routing

package require math

proc simpletest {} {
	conswrite "simpletest behavior from node [current]"
}

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
	conswrite "simpleDOR behavior for node [current]"
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
	if {[with_P 0.8]} { inject [randnode [current]] }
}

proc tickinstrument {} {
	upvar 1 nocsim_tick nocsim_tick
	conswrite "tick instrument! tick is $nocsim_tick"
}

proc injectinstrument {origin dest id} {
	conswrite "inject instrument: origin=$origin, dest=$dest, id=$id"
}

proc routeinstrument {origin dest flitno spawnedat injectedat numhops routefrom routeto} {
	conswrite "route flit $flitno to $routeto from $routefrom"
}

registerinstrument tick tickinstrument
registerinstrument inject injectinstrument
registerinstrument route routeinstrument

create_mesh 4 4 simpleinject simpleDOR

step 1000
