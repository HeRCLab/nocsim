source ../scripts/noc_tools_load.tcl

package require math::statistics

nocviz::launch_gui

proc onnode {id type row col behavior} {
    nocviz::node create $id
    if { $type == [nocsim::type2int PE]} {
        nocviz::node row $id [ expr $row * 2 + 1]
        nocviz::node col $id [ expr $col * 2 + 1]
    } else {
        nocviz::node row $id [ expr $row * 2]
        nocviz::node col $id [ expr $col * 2]
    }
    nocviz::node data set $id meta_id $id
    nocviz::node data set $id meta_row $row
    nocviz::node data set $id meta_col $col
    nocviz::node data set $id meta_behavior $behavior
    nocviz::node data set $id meta_type [nocsim::int2type $type]
    nocviz::node data show $id metadata meta_id meta_row meta_col meta_behavior meta_type
}

proc onlink {from to bidir} {
    if {$bidir == 0} {
        nocviz::link create $from $to
        nocviz::link directed $from $to
    } else {
        nocviz::link undirected $from $to
    }
}

nocsim::registerinstrument node onnode
nocsim::registerinstrument link onlink

namespace import nocsim::*

# simple DOR implementation based on examples/mesh_DOR.tcl

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
    if {[with_P 0.2]} { inject [randnode [current]] }
}

proc on_arrive {origin dest flitno hops spawned_at injected_at} {
    lappend age_on_arrive [expr $::nocsim::nocsim_tick - $spawned_at]
    nocviz::node data set $dest routed [nodeinfo $dest routed]
    nocviz::node data set $dest spawned [nodeinfo $dest spawned]
    nocviz::node data set $dest injected [nodeinfo $dest injected]
    nocviz::node data set $dest dequeued [nodeinfo $dest dequeued]
    nocviz::node data set $dest backrouted [nodeinfo $dest backrouted]
    nocviz::node data set $dest arrived [nodeinfo $dest arrived]
    nocviz::node data show $dest "performance counters" routed spawned injected dequeued backrouted arrived
}

proc on_route {origin dest flitno spawnedat injectedat hops fromnode tonode} {
    nocviz::node data set [nocsim::current] routed [nodeinfo [nocsim::current] routed]
    nocviz::node data set [nocsim::current] spawned [nodeinfo [nocsim::current] spawned]
    nocviz::node data set [nocsim::current] injected [nodeinfo [nocsim::current] injected]
    nocviz::node data set [nocsim::current] dequeued [nodeinfo [nocsim::current] dequeued]
    nocviz::node data set [nocsim::current] backrouted [nodeinfo [nocsim::current] backrouted]
    nocviz::node data set [nocsim::current] arrived [nodeinfo [nocsim::current] arrived]
    nocviz::node data show [nocsim::current] "performance counters" routed spawned injected dequeued backrouted arrived

    set routed [nodeinfo [nocsim::current] routed]
    if { $routed > $::max_routed} {
        set ::max_routed $routed
    }

    set rchannel [expr int( ( 1.0 * $routed ) / (1.0 * $::max_routed) * 255)]
    set gchannel [expr int( (1 - ( 1.0 * $routed ) / (1.0 * $::max_routed)) * 192) + 63]
    set bchannel [expr int( (1 - ( 1.0 * $routed ) / (1.0 * $::max_routed)) * 192) + 63]
    nocviz::node data set [nocsim::current] rchannel $rchannel
    nocviz::node data set [nocsim::current] gchannel $bchannel
    nocviz::node data set [nocsim::current] bchannel $gchannel
    nocviz::node data show [nocsim::current] colors rchannel gchannel bchannel
    nocviz::node color [nocsim::current] $rchannel $gchannel $bchannel 128
}

registerinstrument arrive on_arrive
registerinstrument route on_route

create_mesh 10 10 simpleinject simpleDOR

set ::max_routed 1
set ::min_routed 0

after 5000

#step 1000

vwait forever
