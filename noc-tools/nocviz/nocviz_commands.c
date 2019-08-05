#include "nocviz_commands.h"
#include "graph.h"

/* https://wiki.tcl-lang.org/page/Tcl+Handles */


static int nocviz_command_node(ClientData cdata, Tcl_Interp* interp, int objc, Tcl_Obj *const objv[]) {
	nocviz_graph* g;

	g = (nocviz_graph*) cdata;

	Tcl_SetResult(interp, "hello from nocviz!", NULL);
	return TCL_OK;
}
