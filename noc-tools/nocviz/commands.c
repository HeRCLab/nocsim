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

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}
}
