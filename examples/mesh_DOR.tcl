# this simple example shows a small deflection routed network using DOR routing

source "../scripts/nocsim_load.tcl"

package require math::statistics
namespace import ::nocsim::*

# implements DOR routing for a specific link
proc DORfrom {dir} {

	set prilist {}

	# want to route south
	if {[peek $dir to_row] > [nodeinfo [current] row]} {

		set prilist [dir2list south north east west PE]

	# want to route north
	} elseif {[peek $dir to_row] < [nodeinfo [current] row]} {

		set prilist [dir2list north south east west PE]

	# want to route east
	} elseif {[peek $dir to_col] > [nodeinfo [current] col]} {

		set prilist [dir2list east west south north PE]

	# want to route west
	} elseif {[peek $dir to_col] < [nodeinfo [current] col]} {

		set prilist [dir2list west east south north PE]

	# our PE is the destination
	} else {

		set prilist [dir2list PE west north south east]

	}

	# make the direction we came from always the lowest priority
	# set prilist [lremove prilist $dir]
	# lappend prilist $dir

	route_priority $dir {*}$prilist
}

proc simpleDOR {} {
	if {[incoming [dir2int north]] == 1} {DORfrom [dir2int north]}
	if {[incoming [dir2int south]] == 1} {DORfrom [dir2int south]}
	if {[incoming [dir2int east]] == 1}  {DORfrom [dir2int east]}
	if {[incoming [dir2int west]] == 1}  {DORfrom [dir2int west]}
	if {[incoming [dir2int PE]] == 1}    {DORfrom [dir2int PE]}

}

proc with_P {P} {
	if { [::math::random] <= $P } { return 1 } else {return 0}
}

proc simpleinject {} {
	if {[with_P 0.2]} { inject [randnode [current]] }
}

proc on_arrive {origin dest flitno hops spawned_at injected_at} {
	lappend ::age_on_arrive [expr $::nocsim::nocsim_tick - $spawned_at]
}

proc on_route {origin dest flitno spawnedat injectedat hops fromnode tonode} {
	conswrite "(tick=$::nocsim::nocsim_tick) flitno $flitno routed from $fromnode to $tonode"
}

registerinstrument arrive on_arrive
#registerinstrument route on_route

create_mesh 10 10 simpleinject simpleDOR

set ::age_on_arrive {}

step 500

conswrite "routed $::nocsim::nocsim_routed flits"
conswrite "$::nocsim::nocsim_arrived flits arrived"
conswrite "$::nocsim::nocsim_spawned flits spawned"
conswrite "$::nocsim::nocsim_injected flits injected"
conswrite "throughput = [expr (1.0 * $::nocsim::nocsim_routed /  $::nocsim::nocsim_num_PE) / $::nocsim::nocsim_tick ] flits per PE per cycle"
conswrite "age on arrival... "
conswrite "\tmean    : [::math::statistics::mean $age_on_arrive]"
conswrite "\tmedian  : [::math::statistics::median $age_on_arrive]"
conswrite "\tstdev   : [::math::statistics::stdev $age_on_arrive]"
conswrite "\tvariance: [::math::statistics::var $age_on_arrive]"
