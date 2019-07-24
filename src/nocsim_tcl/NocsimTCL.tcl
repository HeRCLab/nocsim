
# http://www.tutorialspoint.com/tcl-tk/tcl_packages.htm

namespace eval ::NocsimTCL {

	set version 1.0
	set MyDescription "nocsim supporting TCL libraries"

	variable home [file join [pwd] [file dirname [info script]]]

	namespace export PrintNocsimInfo
	namespace export create_mesh
	namespace export all_adjacent

}

# display information about nocsim
proc ::NocsimTCL::PrintNocsimInfo {} {
	upvar #0 nocsim_version nocsim_version
	upvar #0 tcl_version tcl_version
	conswrite "TCL Version: $tcl_version"
	conswrite "nocsim version: [join $nocsim_version "."]"
}

proc ::NocsimTCL::all_adjacent {id} {
	set row [nodeinfo $id row]
	set col [nodeinfo $id col]
	return [concat \
		[findnode [expr $row - 1] $col] \
		[findnode [expr $row + 1] $col] \
		[findnode $row [expr $col - 1]] \
		[findnode $row [expr $col + 1]] ]
}

proc ::NocsimTCL::create_mesh {width height inject_behavior route_behavior} {
	set routers {}
	set PEs {}
	for {set row 0} {$row < $height} {incr row} {
		for {set col 0} {$col < $width} {incr col} {
			set router_id "R.$row.$col"
			set PE_id "PE.$row.$col"
			router $router_id $row $col $route_behavior
			PE $PE_id $row $col $inject_behavior
			link $PE_id $router_id
			link $router_id $PE_id
			lappend routers $router_id
			lappend PEs $PE_id
		}
	}

	foreach router $routers {
		foreach adj [all_adjacent $router] {

			# don't create links to PEs
			if { [nodeinfo $adj type] == [type2int pe] } {
				continue
			}

			link $router $adj
			link $adj $router
		}
	}

	return [concat $routers $PEs]

}

package provide NocsimTCL $NocsimTCL::version