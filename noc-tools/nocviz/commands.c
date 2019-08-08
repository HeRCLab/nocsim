#include "commands.h"

/* https://wiki.tcl-lang.org/page/Tcl+Handles */

int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "node subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);

	if (string_equals(subcmd, "create")) {
		return nocviz_subcmd_node_create(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "destroy")) {
		return nocviz_subcmd_node_destroy(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "row")) {
		return nocviz_subcmd_node_row(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "col")) {
		return nocviz_subcmd_node_col(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "title")) {
		return nocviz_subcmd_node_title(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "data")) {
		return nocviz_subcmd_node_data(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "list")) {
		return nocviz_subcmd_node_list(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "match")) {
		return nocviz_subcmd_node_list(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "op")) {
		return nocviz_subcmd_node_op(cdata, interp, objc, objv);



	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}
}

int nocviz_command_link(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "link subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);

	if (string_equals(subcmd, "create")) {
		return nocviz_subcmd_link_create(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "destroy")) {
		return nocviz_subcmd_link_destroy(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "undirected")) {
		return nocviz_subcmd_link_undirected(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "directed")) {
		return nocviz_subcmd_link_directed(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "direction")) {
		return nocviz_subcmd_link_direction(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "reverse")) {
		return nocviz_subcmd_link_reverse(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "find")) {
		return nocviz_subcmd_link_find(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "match")) {
		return nocviz_subcmd_link_match(cdata, interp, objc, objv);

	} else if (string_equals(subcmd, "data")) {
		return nocviz_subcmd_link_data(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}
}

int nocviz_command_launch_gui(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_gui_handle* h = cdata;

	Tcl_RequireArgs(interp, 1, "launch_gui");

	gui_init(h, interp, h->graph);

	return TCL_OK;

}
