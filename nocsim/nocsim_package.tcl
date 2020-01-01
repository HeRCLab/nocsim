namespace eval ::nocsim {

	set version 2.0.1
	set MyDescription "nocsim supporting TCL libraries"

	variable home [file join [pwd] [file dirname [info script]]]

	source [file join $home nocsim_tclutils.tcl]

	namespace export router
	namespace export PE
	namespace export link
	namespace export current
	namespace export step
	namespace export nodeinfo
	namespace export linkinfo
	namespace export findnode
	namespace export behavior
	namespace export randnode
	namespace export registerinstrument
	namespace export conswrite
	namespace export errwrite
	namespace export route
	namespace export route_priority
	namespace export drop
	namespace export peek
	namespace export avail
	namespace export incoming
	namespace export allincoming
	namespace export spawn
	namespace export inject
	namespace export dir2int
	namespace export dir2list
	namespace export int2dir
	namespace export type2int
	namespace export int2type
	namespace export all_adjacent
	namespace export lshift
	namespace export lremove
	namespace export create_mesh

	namespace export nocsim_RNG_seed
	namespace export nocsim_num_PE
	namespace export nocsim_num_router
	namespace export nocsim_num_node
	namespace export nocsim_flit_no
	namespace export nocsim_tick
	namespace export nocsim_injected
	namespace export nocsim_spawned
	namespace export nocsim_dequeued
	namespace export nocsim_backrouted
	namespace export nocsim_routed
	namespace export nocsim_arrived
	namespace export nocsim_title
	namespace export nocsim_version

}

package provide nocsim $nocsim::version
