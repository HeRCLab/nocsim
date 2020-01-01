# should be source-ed from a TCL interpreter

set frameinfo "[info frame [info frame]]"
lappend auto_path "[file dirname [file dirname [file normalize [dict get $frameinfo file]]]][file separator]nocsim"
package require nocsim

unset frameinfo
