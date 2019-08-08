/* test that the link command group is working as it should */

#include "../datastore.h"
#include "../nocviz.h"
#include "../gui.h"
#include "test_util.h"
#include "../../common/util.h"
#include "../../3rdparty/vec.h"

#include <tcl.h>

int main() {
	Tcl_Interp* interp;
	Tcl_Namespace* nsPtr;
	nocviz_graph* g;
	nocviz_node* n;
	nocviz_gui_handle* h;
	int status;
	int result;

	interp = Tcl_CreateInterp();
	Nocviz_Init(interp);
	nsPtr = Tcl_FindNamespace(interp, "nocviz", NULL, 0);
	should_not_be_null(nsPtr);
	should_not_be_null(nsPtr->clientData);
	h = nsPtr->clientData;
	g = h->graph;

	/* link command on it's own should error */
	tcl_should_not_eval(interp, "%s", "nocviz::link");

	/* should be able to link two nodes */
	tcl_should_eval(interp, "%s", "nocviz::node create node1");
	tcl_should_eval(interp, "%s", "nocviz::node create node2");
	tcl_should_eval(interp, "%s", "nocviz::link create node1 node2");
	should_not_be_null(nocviz_graph_get_link(g, "node1", "node2"));
	should_not_be_null(nocviz_graph_get_link(g, "node2", "node1"));
	/* should not be able to create duplicate link */
	tcl_should_not_eval(interp, "%s", "nocviz::link create node1 node2");
	tcl_should_not_eval(interp, "%s", "nocviz::link create node2 node1");
	tcl_should_eval(interp, "%s", "nocviz::link destroy node2 node1");
	should_be_null(nocviz_graph_get_link(g, "node1", "node2"));
	should_be_null(nocviz_graph_get_link(g, "node2", "node1"));
	tcl_should_eval(interp, "%s", "nocviz::node destroy node1");
	tcl_should_eval(interp, "%s", "nocviz::node destroy node2");

	Tcl_DeleteInterp(interp);
}
