#include "nocviz.h"

/* this is required to ensure the namespace and backing datastore is properly
 * cleaned up when the interpreter exits */
void nocviz_namespace_delete(ClientData cdata) {
	nocviz_gui_handle* h = cdata;
	nocviz_graph* g = h->graph;

	if (h->initialized) {
		AG_ThreadKill(h->thread, SIGKILL);
	}

	nocviz_graph_free(g);
}

/*
 * Hello_Init -- Called when Tcl loads your extension.
 */
int DLLEXPORT
Nocviz_Init(Tcl_Interp *interp)
{
	Tcl_Namespace *nsPtr; /* pointer to hold our own new namespace */
	nocviz_gui_handle* h;
	nocviz_graph* g;

	h = noctools_malloc(sizeof(nocviz_gui_handle));
	g = nocviz_graph_init();
	h->graph = g;
	h->initialized = false;

	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
		return TCL_ERROR;
	}

	nsPtr = Tcl_CreateNamespace(interp, "nocviz", h, nocviz_namespace_delete);
	if (nsPtr == NULL) {
		return TCL_ERROR;
	}

	Tcl_CreateObjCommand(interp, "nocviz::node", nocviz_command_node, g, NULL);
	Tcl_CreateObjCommand(interp, "nocviz::launch_gui", nocviz_command_launch_gui, h, NULL);
	Tcl_PkgProvide(interp, "nocviz", "1.0");
	return TCL_OK;
}
