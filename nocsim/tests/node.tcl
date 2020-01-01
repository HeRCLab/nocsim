# test node-related functionality

package require tcltest

source ../../scripts/nocsim_load.tcl
namespace import ::nocsim::*

tcltest::test 001 {should be able to instantiate a router} -body {
	router ident001 0 0 dummy
}

tcltest::test 002 {should be able to query router row} -body {
	router ident002 1 2 dummy
	nodeinfo ident002 row
} -result {1}

tcltest::test 003 {should be able to query router col} -body {
	router ident003 1 2 dummy
	nodeinfo ident003 col
} -result {2}

tcltest::test 004 {should be able to query router col} -body {
	router ident004 1 2 dummy
	nodeinfo ident004 col
} -result {2}

tcltest::test 005 {should be able to query router behavior} -body {
	router ident005 1 2 dummy
	nodeinfo ident005 behavior
} -result {dummy}

tcltest::test 006 {should be able to instantiate a PE} -body {
	PE ident006 0 0 dummy
}

tcltest::test 007 {should be able to query PE row} -body {
	PE ident007 1 2 dummy
	nodeinfo ident007 row
} -result {1}

tcltest::test 008 {should be able to query PE col} -body {
	PE ident008 1 2 dummy
	nodeinfo ident008 col
} -result {2}

tcltest::test 009 {should be able to query PE col} -body {
	PE ident009 1 2 dummy
	nodeinfo ident009 col
} -result {2}

tcltest::test 010 {should be able to query PE behavior} -body {
	PE ident010 1 2 dummy
	nodeinfo ident010 behavior
} -result {dummy}

tcltest::test 011 {should be able to create a link } -body {
	router r011a 0 0 dummy
	router r011b 0 1 dummy
	link r011a r011b
	linkinfo r011a r011b current_load
} -result {0}

tcltest::test 012 {should be able to update router behavior} -body {
	router ident012 1 2 dummy
	behavior ident012 different
	nodeinfo ident012 behavior
} -result {different}


namespace delete nocsim

