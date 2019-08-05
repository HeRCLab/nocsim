#include "nocviz.h"

/* this is required to ensure the namespace and backing datastore is properly
 * cleaned up when the interpreter exits */
void nocviz_namespace_delete(ClientData cdata) {
	nocviz_graph* g = cdata;
	nocviz_graph_free(g);
}

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

	nsPtr = Tcl_CreateNamespace(interp, "nocviz", g, nocviz_namespace_delete);
	if (nsPtr == NULL) {
		return TCL_ERROR;
	}

	Tcl_CreateObjCommand(interp, "nocviz::node", nocviz_command_node, g, NULL);
	Tcl_PkgProvide(interp, "nocviz", "1.0");
	return TCL_OK;
}
