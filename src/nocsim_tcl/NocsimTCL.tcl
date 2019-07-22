
# http://www.tutorialspoint.com/tcl-tk/tcl_packages.htm

namespace eval ::NocsimTCL {

	set version 1.0
	set MyDescription "nocsim supporting TCL libraries"

	variable home [file join [pwd] [file dirname [info script]]]

	namespace export PrintNocsimInfo

}

# display information about nocsim
proc ::NocsimTCL::PrintNocsimInfo {} {
	upvar #0 nocsim_version nocsim_version
	upvar #0 tcl_version tcl_version
	puts "TCL Version: $tcl_version"
	puts "nocsim version: [join $nocsim_version "."]"
}

package provide NocsimTCL $NocsimTCL::version
