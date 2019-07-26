# this simple example shows a small deflection routed network using DOR routing

package require math

# implements DOR routing for a specific link
proc DORfrom {dir} {

	# want to route south
	if {[peek $dir to_row] > [nodeinfo [current] row]} {

		route_priority $dir {*}[dir2list south east west north PE]

	# want to route north
	} elseif {[peek $dir to_row] < [nodeinfo [current] row]} {

		route_priority $dir {*}[dir2list north west east south pe]

	# want to route east
	} elseif {[peek $dir to_col] < [nodeinfo [current] col]} {

		route_priority $dir {*}[dir2list east south north west PE]

	# want to route west
	} elseif {[peek $dir to_col] > [nodeinfo [current] col]} {

		route_priority $dir {*}[dir2list west north south east PE]

	# our PE is the destination
	} else {

		route_priority $dir {*}[dir2list PE west north south east]

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

create_mesh 5 5 simpleinject simpleDOR

step 200

conswrite "routed $nocsim_routed flits"
conswrite "$nocsim_arrived flits arrived"
conswrite "$nocsim_spawned flits spawned"
conswrite "$nocsim_injected flits injected"
conswrite "throughput = [expr (1.0 * $nocsim_routed /  $nocsim_num_PE) / $nocsim_tick ] flits per PE per cycle"
