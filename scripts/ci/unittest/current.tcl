# test that the current command works

package require tcltest

proc r1 {} {
	upvar #0 current current
	set current [current]
}

tcltest::test 001 {current should return the current node ID} -body {
	set current ""
	router r1 0 0 r1
	step
	return $current
} -result {r1}
