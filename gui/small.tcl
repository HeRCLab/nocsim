package require math

proc simpletest {} {
	conswrite "simpletest behavior from node [current]"
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

PE p1 0 0 simpletest
PE p2 0 1 simpletest
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


