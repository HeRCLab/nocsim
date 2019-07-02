# set tcl_library "/usr/share/tcltk/tcl8.6"
# source /usr/share/tcltk/tcl8.6/init.tcl
source /home/cad/Downloads/TclProDebug/lib/tcldebugger/initdebug.tcl
debugger_init
debugger_eval {
	debugger_break
	router foo 0 0 todo
	router bar 0 1 todo
	router baz 1 0 todo
	router quux 1 1 todo
	PE first 0 0 todo
	PE second 0 1 todo
	link first foo
	link foo first
	link second bar
	link bar second
	link foo bar
	link bar baz
	link baz quux
	link quux foo
	puts "find all nodes: [findnode]"
	puts "find node at 0 1 [findnode 0 1]"
	graphviz
	puts "type of foo is: [nodeinfo foo type]"
	puts "row of foo is: [nodeinfo foo row]"
	puts "col of foo is: [nodeinfo foo col]"
	puts "behavior of foo is: [nodeinfo foo behavior]"
	behavior foo different
	puts "behavior of foo is: [nodeinfo foo behavior]"
	puts "random node 1: [randnode]"
	puts "random node 2: [randnode]"
	puts "random node 3: [randnode]"
	step
	step
	step
}
