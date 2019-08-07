#include "commands.h"

int nocviz_subcmd_link_create(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link create SRC_ID DEST_ID");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;

}

int nocviz_subcmd_link_destroy(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link destroy ID1 ID2");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;

}

int nocviz_subcmd_link_undirected(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link undirected ID1 ID2");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;

}

int nocviz_subcmd_link_directed(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link directed ID1 ID2");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;

}

int nocviz_subcmd_link_reverse(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link reverse ID1 ID2");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;

}

int nocviz_subcmd_link_direction(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link direction ID1 ID2");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;
}

int nocviz_subcmd_link_find(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "link find ID / link find ID1 ID2");
		return TCL_ERROR;
	}

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;

}

int nocviz_subcmd_link_match(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	Tcl_RequireArgs(interp, 4, "link direction KEY PATTERN");

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;
}

int nocviz_subcmd_link_data(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "link data subcmd...");
		return TCL_ERROR;
	}

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;
}

int nocviz_subcmd_link_op(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g = cdata;
	UNUSED(g);

	if (objc < 3) {
		Tcl_WrongNumArgs(interp, 0, objv, "link op subcmd...");
		return TCL_ERROR;
	}

	Tcl_SetResult(interp, "not yet implemented", NULL);

	return TCL_ERROR;
}
