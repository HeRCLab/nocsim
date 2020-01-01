# basic unit test that makes sure the interpreter is working / isn't crashing

source ../../scripts/nocsim_load.tcl
namespace import ::nocsim::*

package require tcltest

tcltest::test 001 {basic sanity check} {
	expr 2 * 3
} 6

namespace delete nocsim

