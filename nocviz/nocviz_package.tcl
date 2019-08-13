namespace eval ::nocviz {

	set version 2.0.0
	set MyDescription "nocviz supporting TCL libraries"

	variable home [file join [pwd] [file dirname [info script]]]

	source [file join $home nocviz_color.tcl]

	namespace export node
	namespace export link
	namespace export launch_gui
	namespace export rgb_interp

}

package provide nocviz $nocviz::version
