# test case to ensure linking works as it should

package require tcltest

source ../../scripts/noc_tools_load.tcl
namespace import ::nocsim::*

proc nop {} {}

tcltest::test 001 {link direction inference should work correctly} -body {
	router middle 1 1 nop
	router north 0 1 nop
	router south 2 1 nop
	router east 1 2 nop
	router west 1 0 nop
	PE p1 1 1 nop

	link middle north
	link middle south
	link middle east
	link middle west
	link north middle
	link south middle
	link east middle
	link west middle
	link p1 middle
	link middle p1

	conswrite "linkinfo middle north from_dir: [linkinfo middle north from_dir]"
	conswrite "linkinfo middle north to_dir  : [linkinfo middle north to_dir  ]"
	conswrite "linkinfo middle south from_dir: [linkinfo middle south from_dir]"
	conswrite "linkinfo middle south to_dir  : [linkinfo middle south to_dir  ]"
	conswrite "linkinfo middle east  from_dir: [linkinfo middle east  from_dir]"
	conswrite "linkinfo middle east  to_dir  : [linkinfo middle east  to_dir  ]"
	conswrite "linkinfo middle west  from_dir: [linkinfo middle west  from_dir]"
	conswrite "linkinfo middle west  to_dir  : [linkinfo middle west  to_dir  ]"
	conswrite "linkinfo middle p1    from_dir: [linkinfo middle p1    from_dir]"
	conswrite "linkinfo middle p1    to_dir  : [linkinfo middle p1    to_dir  ]"
	conswrite "linkinfo north middle from_dir: [linkinfo north middle from_dir]"
	conswrite "linkinfo north middle to_dir  : [linkinfo north middle to_dir  ]"
	conswrite "linkinfo south middle from_dir: [linkinfo south middle from_dir]"
	conswrite "linkinfo south middle to_dir  : [linkinfo south middle to_dir  ]"
	conswrite "linkinfo east  middle from_dir: [linkinfo east  middle from_dir]"
	conswrite "linkinfo east  middle to_dir  : [linkinfo east  middle to_dir  ]"
	conswrite "linkinfo west  middle from_dir: [linkinfo west  middle from_dir]"
	conswrite "linkinfo west  middle to_dir  : [linkinfo west  middle to_dir  ]"
	conswrite "linkinfo p1    middle from_dir: [linkinfo p1    middle from_dir]"
	conswrite "linkinfo p1    middle to_dir  : [linkinfo p1    middle to_dir  ]"

	if { [linkinfo middle north from_dir] != [dir2int north] } { return 100;   }
	if { [linkinfo middle north to_dir  ] != [dir2int south] } { return 200;   }
	if { [linkinfo middle south from_dir] != [dir2int south] } { return 300;   }
	if { [linkinfo middle south to_dir  ] != [dir2int north] } { return 400;   }
	if { [linkinfo middle east  from_dir] != [dir2int east ] } { return 500;   }
	if { [linkinfo middle east  to_dir  ] != [dir2int west ] } { return 600;   }
	if { [linkinfo middle west  from_dir] != [dir2int west ] } { return 700;   }
	if { [linkinfo middle west  to_dir  ] != [dir2int east ] } { return 800;   }
	if { [linkinfo middle p1    from_dir] != [dir2int pe   ] } { return 900;   }
	if { [linkinfo middle p1    to_dir  ] != [dir2int pe   ] } { return 1000;  }

	if { [linkinfo north middle from_dir] != [dir2int south] } { return 1100;  }
	if { [linkinfo north middle to_dir  ] != [dir2int north] } { return 1200;  }
	if { [linkinfo south middle from_dir] != [dir2int north] } { return 1300;  }
	if { [linkinfo south middle to_dir  ] != [dir2int south] } { return 1400;  }
	if { [linkinfo east  middle from_dir] != [dir2int west ] } { return 1500;  }
	if { [linkinfo east  middle to_dir  ] != [dir2int east ] } { return 1600;  }
	if { [linkinfo west  middle from_dir] != [dir2int east ] } { return 1700;  }
	if { [linkinfo west  middle to_dir  ] != [dir2int west ] } { return 1800;  }
	if { [linkinfo p1    middle from_dir] != [dir2int pe   ] } { return 1900;  }
	if { [linkinfo p1    middle to_dir  ] != [dir2int pe   ] } { return 2000;  }

	# diagonal links should error
	router northeast 0 2 nop
	if { ! [catch {link middle northeast} err] } { return 2100; }

	return 0
} -result 0

tcltest::test 002 {explicit to link should work correctly} -body {

	router t2_1 3 3 nop
	router t2_2 3 4 nop

	link t2_1 t2_2 [dir2int north]

	# XXX: should probably be more exhaustive

	if { [linkinfo t2_1 t2_2 to_dir   ] != [dir2int north] } { return 2200; }
	if { [linkinfo t2_1 t2_2 from_dir ] != [dir2int south] } { return 2300; }

	return 0
} -result 0

tcltest::test 003 {explicit to and from link should work correctly} -body {

	router t3_1 6 6 nop
	router t3_2 6 7 nop

	link t3_1 t3_2 [dir2int north] [dir2int west]

	if { [linkinfo t3_1 t3_2 to_dir   ] != [dir2int west ] } { return 2200; }
	if { [linkinfo t3_1 t3_2 from_dir ] != [dir2int north] } { return 2300; }

	return 0
} -result 0

tcltest::test 0004 {should be impossible to link a router to itself} -body {

	router t4_1 9 9 nop

	if { ! [ catch {link t4_1 t4_1} err ] } { return 100; }
	if { ! [ catch {link t4_1 t4_1 [dir2int north]} err ] } { return 200; }
	if { ! [ catch {link t4_1 t4_1 [dir2int north] [dir2int south]} err ] } { return 300; }

	return 0;

} -result 0


namespace delete nocsim
namespace delete nocviz
