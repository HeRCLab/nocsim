namespace eval ::nocsim {
}

proc ::nocsim::all_adjacent {id} {
	set row [nodeinfo $id row]
	set col [nodeinfo $id col]
	return [concat \
		[findnode [expr $row - 1] $col] \
		[findnode [expr $row + 1] $col] \
		[findnode $row [expr $col - 1]] \
		[findnode $row [expr $col + 1]] ]
}

proc ::nocsim::create_mesh {width height inject_behavior route_behavior} {
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
		}
	}

	return [concat $routers $PEs]

}

# as described here: https://wiki.tcl-lang.org/page/lshift
proc ::nocsim::lshift {args} {
	lassign $args listVar count
	upvar 1 $listVar var
	if { ! [info exists var] } {
		return -level 1 -code error \
			"can't read \"$listVar\": no such variable"
	}
	switch -exact -- [llength $args] {
		1 {
			set var [lassign $var value]
		}
		2 {
			set value [lrange $var 0 $count-1]
			set var [lrange $var $count end]
		}
		default {
			return -level 1 -code error \
				"wrong # args: should be \"lshift listVar ?count?\""
				# error-args "lshift listVar ?count?"
			}
	}
	return $value
}

proc ::nocsim::dir2list args {
	set result {}
	foreach arg $args {
		lappend result [dir2int $arg]
	}
	return $result
}

proc ::nocsim::route_priority args {
	set from_dir [lshift args]

	# nothing to do
	if { [incoming $from_dir] != 1} {return;}

	foreach arg $args {
		if { [avail $arg] == 0 } {
			route $from_dir $arg
			return $arg;
		}
	}

	return;
}

proc ::nocsim::lremove {listname val} {
	upvar 1 $listname target_list

	set idx [lsearch $target_list $val]
	return [lreplace $target_list $idx $idx]
}

proc ::nocsim::mapcolor {r1 g1 b1 a1 r2 g2 b2 a2 lower upper val} {

	if { $upper < $lower } { return [::nocsim::mapcolor $r1 $g1 $b1 $a1 $r2 $g2 $b2 $a2 $upper $lower $val] }

	set d [ expr (1.0 * $val - 1.0 * $lower) / (1.0 * $upper - 1.0 * $lower) ]
	set r [ expr abs($r2 - $r1) * $d ]
	set g [ expr abs($g2 - $g1) * $d ]
	set b [ expr abs($b2 - $b1) * $d ]
	set a [ expr abs($a2 - $a1) * $d ]

	return [ list $r $g $b $a ]
}
