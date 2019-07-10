source /opt/TclProDebug/lib/tcldebugger/initdebug.tcl
debugger_init
debugger_eval {
	debugger_break
	set x 5
	set y 10
	set z 11
	puts "value of z is $z"
}
debugger_break

