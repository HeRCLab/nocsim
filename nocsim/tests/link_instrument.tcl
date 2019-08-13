package require tcltest

source ../../scripts/noc_tools_load.tcl
namespace import ::nocsim::*

proc testinstrument {from to bidir} {
	set ::fromlink $from
	set ::tolink $to
	set ::isbidir $bidir
}

tcltest::test 001 {link instrument should run when link is instantiated} -body {
	set ::fromlink "ERROR"
	set ::tolink "ERROR"
	set ::isbidir "ERROR"
	registerinstrument link testinstrument
	router r001a 0 1 dummy
	router r001b 1 1 dummy
	link r001a r001b
	
	puts "$fromlink, $tolink, $isbidir"

	if { ! [string equal $::fromlink "r001a"] } {return 100}
	if { ! [string equal $::tolink "r001b"] } {return 200}
	if { ! [string equal $::isbidir "0"] } {return 300}

	set ::fromlink "ERROR"
	set ::tolink "ERROR"
	set ::isbidir "ERROR"

	link r001b r001a

	puts "$fromlink, $tolink, $isbidir"

	if { ! [string equal $::tolink "r001a"] } {return 400}
	if { ! [string equal $::fromlink "r001b"] } {return 500}
	if { ! [string equal $::isbidir "1"] } {return 600}


	return 0

} -result {0}

namespace delete nocsim
namespace delete nocviz
