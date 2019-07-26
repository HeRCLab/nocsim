package require tcltest

tcltest::test 001 {lremove removes a single instance} -body {
	set mylist {a b c d}
	set mylist [lremove mylist b]

	if { [lindex $mylist 0] != "a" } { return 100; }
	if { [lindex $mylist 1] != "c" } { return 200; }
	if { [lindex $mylist 2] != "d" } { return 300; }

	return 0;
} -result {0}

tcltest::test 002 {lremove remove only the first instance of several} -body {
	set mylist {a b c d b e b}
	set mylist [lremove mylist b]

	if { [lindex $mylist 0] != "a" } { return 100; }
	if { [lindex $mylist 1] != "c" } { return 200; }
	if { [lindex $mylist 2] != "d" } { return 300; }
	if { [lindex $mylist 3] != "b" } { return 400; }
	if { [lindex $mylist 4] != "e" } { return 500; }
	if { [lindex $mylist 5] != "b" } { return 600; }
}
