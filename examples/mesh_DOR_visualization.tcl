#!/usr/bin/env wish

# This shows the same simulation as the mesh_DOR example, but with
# a simple Tk visualization also. Note that this must be run via wish.

source "../scripts/nocsim_load.tcl"
source "../nocviz/nocviz_color.tcl"

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

proc rgb2hex {r g b} {
	return [format "#%02x%02x%02x" $r $g $b]
}

proc on_route {origin dest flitno spawnedat injectedat hops fromnode tonode} {

	# don't update colors for PEs, since we don't show them
	if {[nodeinfo $tonode type] == [type2int "PE"]} {return}

	# update maximum # of routed flits for any node
	if {[nodeinfo $tonode routed] > $::maxrouted} {
		set ::maxrouted [nodeinfo $tonode routed]
	}

	# compute fraction for HSL interpolation
	set frac [expr (1.0 * [nodeinfo $tonode routed]) / $::maxrouted]

	# compute HSL interpolated color
	set color [rgb2hex {*}[rgb_interp $frac {*}$::cstart {*}$::cend]]

	# apply color to node
	visualize_node .canframe.can $tonode [nodeinfo $tonode row] [nodeinfo $tonode col] $color
}

registerinstrument arrive on_arrive
registerinstrument route on_route

# create a node, or update it's color
proc visualize_node {path id row col color} {

        # try to get the node
        set node [$path find withtag $id]

        if { "" eq [$path find withtag $id] } {
                # the node does not exist yet, so it must be created

		# corner position
		set cx [expr $::hoffset + $::pitch * $col]
		set cy [expr $::voffset + $::pitch * $row]

		set node [$path create rectangle \
			$cx \
			$cy \
			[expr $cx + $::radius] \
			[expr $cy + $::radius] \
			-fill $color -outline black -tag $id]

		$path create text [expr $cy + $::radius / 2] [expr $cx + $::radius / 2] -text $id
		$path lower $node
        }


        $path itemconfigure $node -fill $color
}

proc visualize_link {path node1 node2 color} {
	set link [$path find withtag "$node1-$node2"]

	if { "" eq [$path find withtag "$node1-$node2"] } {

		# get the center positions of both nodes
		set n1row [nodeinfo $node1 row]
		set n1col [nodeinfo $node1 col]
		set n2row [nodeinfo $node2 row]
		set n2col [nodeinfo $node2 col]
		set n1cx [expr $::hoffset + $::pitch * $n1col + $::radius / 2]
		set n1cy [expr $::voffset + $::pitch * $n1row + $::radius / 2]
		set n2cx [expr $::hoffset + $::pitch * $n2col + $::radius / 2]
		set n2cy [expr $::voffset + $::pitch * $n2row + $::radius / 2]

		if {[expr abs($n1row - $n2row) + abs($n1col - $n2col)] > 2} {
		}

		set link [ $path create line \
			$n1cx $n1cy $n2cx $n2cy -width 2 ]

		$path lower $link
	}

	$path itemconfigure $link -fill $color

}

proc on_link {origin dest existing} {

	# All the links in this simulation are bidirectional, and we visualize
	# them as such as soon as they are created. If $existing is 1, then
	# this is the second time we've seen this like and it's been converted
	# to bidirectional internally within the simulation state.
	if {1 == $existing} {
		return
	}

	visualize_link .canframe.can $origin $dest black
}

proc on_create {nodeid type row col behavior} {
	# don't show PEs
	if {$type == [type2int "PE"]} {return}
	visualize_node .canframe.can $nodeid $row $col white
}

proc on_tick {} {
	set ::display_tick "[expr $::nocsim::nocsim_tick + 1]"

}

registerinstrument node on_create
registerinstrument link on_link
registerinstrument tick on_tick

# configuration for node visualizations
set ::rows 7
set ::cols 7
set ::pitch 150
set ::voffset 50
set ::hoffset 50
set ::radius 50
set ::interval 50
set ::display_tick 0
set ::maxrouted 1
set ::cend [list 250 250 110]
set ::cstart [list 42 72 88]

# create the canvas
labelframe .canframe -text "visualization"
canvas .canframe.can -background "gray85" -width [expr $::rows * $::pitch] -height [expr $::cols * $::pitch]
pack .canframe.can
grid .canframe -row 0 -column 0

labelframe .ctlframe      -text "simulation control"
button .ctlframe.step     -text "step 1"             -command {step 1}
button .ctlframe.step10   -text "step 10"            -command {step 10}
button .ctlframe.step100  -text "step 100"           -command {step 100}
label .ctlframe.ticknolbl -text "tick#"
label .ctlframe.tickno    -textvariable display_tick
grid .ctlframe.step       -row 0                     -column 0
grid .ctlframe.step10     -row 0                     -column 1
grid .ctlframe.step100    -row 0                     -column 2
grid .ctlframe.ticknolbl  -row 0                     -column 3
grid .ctlframe.tickno     -row 0                     -column 4
grid .ctlframe            -row 1                     -column 0


create_mesh $::rows $::cols simpleinject simpleDOR


# set ::age_on_arrive {}
#
# step 500
#
# conswrite "routed $::nocsim::nocsim_routed flits"
# conswrite "$::nocsim::nocsim_arrived flits arrived"
# conswrite "$::nocsim::nocsim_spawned flits spawned"
# conswrite "$::nocsim::nocsim_injected flits injected"
# conswrite "throughput = [expr (1.0 * $::nocsim::nocsim_routed /  $::nocsim::nocsim_num_PE) / $::nocsim::nocsim_tick ] flits per PE per cycle"
# conswrite "age on arrival... "
# conswrite "\tmean    : [::math::statistics::mean $age_on_arrive]"
# conswrite "\tmedian  : [::math::statistics::median $age_on_arrive]"
# conswrite "\tstdev   : [::math::statistics::stdev $age_on_arrive]"
# conswrite "\tvariance: [::math::statistics::var $age_on_arrive]"
