#include "nocviz.h"
#include "nocviz_commands.h"
#include "graph.h"

/*
 * Hello_Init -- Called when Tcl loads your extension.
 */
int DLLEXPORT
Nocviz_Init(Tcl_Interp *interp)
{
	Tcl_Namespace *nsPtr; /* pointer to hold our own new namespace */
	nocviz_graph* g;
	g = nocviz_graph_init();

	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
		return TCL_ERROR;
	}

	nsPtr = Tcl_CreateNamespace(interp, "nocviz", NULL, NULL);
	if (nsPtr == NULL) {
		return TCL_ERROR;
	}

	Tcl_CreateObjCommand(interp, "nocviz::node", nocviz_command_node, g, NULL);
	Tcl_PkgProvide(interp, "nocviz", "1.0");
	return TCL_OK;
}
