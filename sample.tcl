# set tcl_library "/usr/share/tcltk/tcl8.6"
# source /usr/share/tcltk/tcl8.6/init.tcl

proc simpletest {} {
	upvar 1 tick tick
	upvar 1 type_PE type_PE
	set current [current]
	puts "===== simpletest ===="
	puts "current is: $current"
	puts "type is: [nodeinfo $current type]"
	puts "row is: [nodeinfo $current row]"
	puts "col is: [nodeinfo $current col]"
	puts "tick is: $tick"
	if { [nodeinfo $current type] == $type_PE } {
		inject [randnode [current]]
	}
}


source /home/cad/Downloads/TclProDebug/lib/tcldebugger/initdebug.tcl
debugger_init
debugger_eval {
	debugger_break
	router foo 0 0 simpletest
	router bar 0 1 simpletest
	router baz 1 0 simpletest
	router quux 1 1 simpletest
	PE first 0 0 simpletest
	PE second 0 1 simpletest
	link first foo
	link foo first
	link second bar
	link bar second
	link foo bar
	link bar baz
	link baz quux
	link quux foo
	graphviz
	step
	step
	step
}
