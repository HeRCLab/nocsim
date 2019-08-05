#include "commands.h"

/* https://wiki.tcl-lang.org/page/Tcl+Handles */


int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	char* subcmd;

	if (objc < 2) {
		Tcl_WrongNumArgs(interp, 0, objv, "node subcmd ...");
		return TCL_ERROR;
	}

	subcmd = Tcl_GetString(objv[1]);

	dbprintf("node calling subcmd %s\n", subcmd);

	if (string_equals(subcmd, "create")) {
		nocviz_subcmd_node_create(cdata, interp, objc, objv);

	} else {
		Tcl_SetResultf(interp, "no such subcommand: %s", subcmd);
		return TCL_ERROR;
	}

	return TCL_OK;
}
