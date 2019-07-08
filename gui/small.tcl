# this simple example shows a small deflection routed network using DOR routing

package require math

proc simpletest {} {
	conswrite "simpletest behavior from node [current]"
}

# implements DOR routing for a specific link
proc DORfrom {dir} {

	# want to route south
	if {peek $dir to_row > [nodeinfo row [current]]} {
		if        {[avail [dir2int south]] == 0} {
			route $dir south
		} else if {[avail [dir2int east ]] == 0} {
			route $dir east
		} else if {[avail [dir2int west ]] == 0} {
			route $dir west
		} else if {[avail [dir2int north]] == 0} {
			route $dir north
		}

	# want to route north
	} else if {peek $dir to_row < [nodeinfo row [current]]} {
		if        {[avail [dir2int north]] == 0} {
			route $dir north
		} else if {[avail [dir2int west ]] == 0} {
			route $dir west
		} else if {[avail [dir2int east ]] == 0} {
			route $dir east
		} else if {[avail [dir2int south]] == 0} {
			route $dir south
		}

	# want to route east
	} else if {peek $dir to_col < [nodeinfo col [current]]} {
		if        {[avail [dir2int east ]] == 0} {
			route $dir east
		} else if {[avail [dir2int south]] == 0} {
			route $dir south
		} else if {[avail [dir2int north]] == 0} {
			route $dir north
		} else if {[avail [dir2int west ]] == 0} {
			route $dir west
		}
	}

	# want to route west
	} else {
		if        {[avail [dir2int west ]] == 0} {
			route $dir west
		} else if {[avail [dir2int north]] == 0} {
			route $dir north
		} else if {[avail [dir2int south]] == 0} {
			route $dir south
		} else if {[avail [dir2int east ]] == 0} {
			route $dir east
		}
	}
}

proc simpleDOR {} {
	conswrite "simpleDOR behavior for node [current]"
	foreach dir [allincoming] {
		conswrite "\trouting from direction [int2dir $dir]"
		DORfrom $dir
	}

}

proc with_P {P} {
	return ::math::random <= $P
}

proc simpleinject {} {
	if {[with_P 0.8]} { inject [randnode [current]] }
}

proc tickinstrument {} {
	upvar 1 tick tick
	conswrite "tick instrument! tick is $tick"
}

proc injectinstrument {origin dest id} {
	conswrite "inject instrument: origin=$origin, dest=$dest, id=$id"
}

registerinstrument tick tickinstrument
registerinstrument inject injectinstrument

router r1 0 0 simpletest
router r2 0 1 simpletest
router r3 1 0 simpletest
router r4 1 1 simpletest

PE p1 0 0 simpleinject
PE p2 0 1 simpleinject
PE p3 1 0 simpleinject
PE p4 1 1 simpleinject

link r1 r2
link r2 r1
link r1 r3
link r3 r1
link r3 r4
link r4 r3
link r2 r4
link r4 r2

link p1 r1
link r1 p1
link p2 r2
link r2 p2
link p3 r3
link r3 p3
link p4 r4
link r4 p4
