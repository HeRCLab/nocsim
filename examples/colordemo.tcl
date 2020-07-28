source "../scripts/noc_tools_load.tcl"

# This simple program demonstrates animated colors being displayed with 
# nocviz.

nocviz::node create n1
nocviz::node create n2
nocviz::node create n3
nocviz::node create n4

nocviz::link create n1 n2
nocviz::link create n2 n3
nocviz::link create n3 n4
nocviz::link create n4 n1

nocviz::node row n1 0
nocviz::node col n1 0
nocviz::node row n2 0
nocviz::node col n2 2
nocviz::node row n3 2
nocviz::node col n3 2
nocviz::node row n4 2
nocviz::node col n4 0

nocviz::link data set n1 n2 foo bar
nocviz::link data show n1 n2 example foo

nocviz::node data set n1 foo bar
nocviz::node data show n1 example foo

proc next_state {val dir} {
    if { $val > 254 && $dir == 1} {
        set dir 0;
    }

    if { $val < 1 && $dir == 0} {
        set dir 1;
    }

    if {$dir == 1} {
        return [list [expr $val + 1] $dir]
    } else {
        return [list [expr $val - 1] $dir]
    }
}

proc ascolor1 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int($pct * 50.0 + 50)] [expr int($pct * 70.0 + 20)] [expr int((1 - $pct) * 100.0 + 80)]]
}

proc ascolor2 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int( (1 - $pct) * 120.0 + 100)] [expr int($pct * 60.0 + 50)] [expr int($pct * 70.0 + 20)]]
}

proc ascolor3 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int($pct * 160.0 + 90)] [expr int($pct * 100.0 + 90)] [expr int($pct * 30.0 + 40)]]
}

proc ascolor4 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int($pct * 50.0 + 50)] [expr int((1 - $pct) * 200.0 + 10)] [expr int($pct * 80.0 + 120)]]
}

proc ascolor5 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int($pct * 50.0 + 150)] [expr int($pct * 70.0 + 20)] [expr int($pct * 100.0 + 80)]]
}

proc ascolor6 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int($pct * 120.0 + 30)] [expr int($pct * 20.0 + 200)] [expr int( (1 - $pct) * 70.0 + 20)]]
}

proc ascolor7 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int( ( 1 - $pct ) * 160.0 + 50)] [expr int($pct * 210.0 + 30)] [expr int($pct * 30.0 + 40)]]
}

proc ascolor8 {val dir} {
    set pct [expr $val / 255.0]
    return [list [expr int($pct * 50.0 + 50)] [expr int($pct * 30.0 + 10)] [expr int($pct * 80.0 + 120)]]
}

nocviz::launch_gui

set n1state {0 1}
set n2state {30 1}
set n3state {60 1}
set n4state {90 1}
set l1state {200 1}
set l2state {150 1}
set l3state {100 1}
set l4state {50 1}


while {1} {
    after 5
    nocviz::node color n1 {*}[ascolor1 {*}$n1state]
    nocviz::node color n2 {*}[ascolor2 {*}$n2state]
    nocviz::node color n3 {*}[ascolor3 {*}$n3state]
    nocviz::node color n4 {*}[ascolor4 {*}$n4state]
    nocviz::link color n1 n2 {*}[ascolor5 {*}$l1state]
    nocviz::link color n2 n3 {*}[ascolor6 {*}$l2state]
    nocviz::link color n3 n4 {*}[ascolor7 {*}$l3state]
    nocviz::link color n4 n1 {*}[ascolor8 {*}$l4state]

    set n1state [next_state {*}$n1state]
    set n2state [next_state {*}$n2state]
    set n3state [next_state {*}$n3state]
    set n4state [next_state {*}$n4state]
    set l1state [next_state {*}$l1state]
    set l2state [next_state {*}$l2state]
    set l3state [next_state {*}$l3state]
    set l4state [next_state {*}$l4state]
}
