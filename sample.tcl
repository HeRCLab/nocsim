set tcl_library "/usr/share/tcltk/tcl8.6"
source /usr/share/tcltk/tcl8.6/init.tcl
source /home/cad/Downloads/TclProDebug/lib/tcldebugger/initdebug.tcl
debugger_init
debugger_break
debugger_eval {
	debugger_break
	router foo 0 0 todo
	router bar 0 1 todo
	router baz 1 0 todo
	router quux 1 1 todo
	link foo bar
	link bar baz
	link baz quux
	link quux foo
	graphviz
}
debugger_break
