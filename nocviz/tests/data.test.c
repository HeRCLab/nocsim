/* test that the "data" command group is working as it should */

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
	nocviz_node* n1;
	nocviz_node* n2;
	nocviz_link* l;
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

	tcl_should_eval(interp, "%s", "nocviz::data set foo bar");
	str_should_equal(nocviz_ds_get_kvp(g->ds, "foo"), "bar");
	tcl_str_result_should_equal(interp, "bar", "%s", "nocviz::data get foo");
	tcl_str_result_should_equal(interp, "foo", "%s", "nocviz::data keys");
	tcl_str_result_should_equal(interp, "bar", "%s", "nocviz::data delete foo");
	tcl_should_not_eval(interp, "%s", "nocviz::data get foo");

	Tcl_DeleteInterp(interp);
}
