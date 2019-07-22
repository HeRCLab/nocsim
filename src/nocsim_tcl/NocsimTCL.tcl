
# http://www.tutorialspoint.com/tcl-tk/tcl_packages.htm

namespace eval ::NocsimTCL {

	set version 1.0
	set MyDescription "HelloWorld"

	variable home [file join [pwd] [file dirname [info script]]]

	namespace export MyProcedure

}

# Definition of the procedure MyProcedure
proc ::NocsimTCL::MyProcedure {} {
   puts $NocsimTCL::MyDescription
}

package provide NocsimTCL $NocsimTCL::version
