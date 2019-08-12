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

	/* link directionality */
	tcl_should_eval(interp, "%s", "nocviz::node create node1");
	tcl_should_eval(interp, "%s", "nocviz::node create node2");
	tcl_should_eval(interp, "%s", "nocviz::node create node3");
	tcl_should_eval(interp, "%s", "nocviz::link create node1 node2");
	l =nocviz_graph_get_link(g, "node1", "node2");
	should_not_be_null(l);
	should_equal(l->type, NOCVIZ_LINK_DIRECTED);
	tcl_should_eval(interp, "%s", "nocviz::link undirected node1 node2");
	should_equal(l->type, NOCVIZ_LINK_UNDIRECTED);
	tcl_should_eval(interp, "%s", "nocviz::link directed node1 node2");
	should_equal(l->type, NOCVIZ_LINK_DIRECTED);
	n1 = nocviz_graph_get_node(g, "node1");
	n2 = nocviz_graph_get_node(g, "node2");
	should_not_be_null(n1);
	should_not_be_null(n2);
	str_should_equal(n1->id, l->from->id);
	str_should_equal(n2->id, l->to->id);
	tcl_should_eval(interp, "%s", "nocviz::link reverse node1 node2");
	str_should_equal(n2->id, l->from->id);
	str_should_equal(n1->id, l->to->id);
	tcl_str_result_should_equal(interp, "node2 node1",
		"%s", "nocviz::link direction node1 node2");
	tcl_str_result_should_equal(interp, "node2 node1",
		"%s", "nocviz::link direction node2 node1");
	/* directed link, should show up searching for node2, but not node1,
	 * since the link is outgoing from node 2, which we already asserted */
	tcl_result_list_should_contain(interp, "node1", "%s", "nocviz::link find node2");
	tcl_result_list_should_contain(interp, "node2", "%s", "nocviz::link find node2");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node2");
	tcl_result_list_should_not_contain(interp, "node1", "%s", "nocviz::link find node1");
	tcl_result_list_should_not_contain(interp, "node2", "%s", "nocviz::link find node1");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node1");
	/* should still be able to find it by both ends */
	tcl_result_list_should_contain(interp, "node1", "%s", "nocviz::link find node1 node2");
	tcl_result_list_should_contain(interp, "node2", "%s", "nocviz::link find node1 node2");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node1 node2");
	tcl_result_list_should_contain(interp, "node1", "%s", "nocviz::link find node2 node1");
	tcl_result_list_should_contain(interp, "node2", "%s", "nocviz::link find node2 node1");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node2 node1");
	/* now it's undirected, so the link should appear from both directions */
	tcl_should_eval(interp, "%s", "nocviz::link undirected node1 node2");
	tcl_result_list_should_contain(interp, "node1", "%s", "nocviz::link find node1");
	tcl_result_list_should_contain(interp, "node2", "%s", "nocviz::link find node1");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node1");
	tcl_result_list_should_contain(interp, "node1", "%s", "nocviz::link find node2");
	tcl_result_list_should_contain(interp, "node2", "%s", "nocviz::link find node2");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node2");
	/* no links to node 3 */
	tcl_result_list_should_not_contain(interp, "node1", "%s", "nocviz::link find node3");
	tcl_result_list_should_not_contain(interp, "node2", "%s", "nocviz::link find node3");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link find node3");
	tcl_should_eval(interp, "%s", "nocviz::node destroy node1");
	tcl_should_eval(interp, "%s", "nocviz::node destroy node2");
	tcl_should_eval(interp, "%s", "nocviz::node destroy node3");

	/* test link data handling */
	tcl_should_eval(interp, "%s", "nocviz::node create node1");
	tcl_should_eval(interp, "%s", "nocviz::node create node2");
	tcl_should_eval(interp, "%s", "nocviz::node create node3");
	tcl_should_eval(interp, "%s", "nocviz::node create node4");
	tcl_should_eval(interp, "%s", "nocviz::link create node1 node2");
	tcl_should_eval(interp, "%s", "nocviz::link create node3 node4");
	tcl_should_not_eval(interp, "%s", "nocviz::link data set node1 node3 foo bar");
	l = nocviz_graph_get_link(g, "node1", "node2");
	tcl_should_eval(interp, "%s", "nocviz::link data set node1 node2 foo bar");
	tcl_should_eval(interp, "%s", "nocviz::link data set node3 node4 foo baz");
	should_not_be_null(nocviz_ds_get_kvp(l->ds, "foo"));
	str_should_equal(nocviz_ds_get_kvp(l->ds, "foo"), "bar");
	tcl_result_list_should_contain(interp, "node1", "%s", "nocviz::link match foo bar");
	tcl_result_list_should_contain(interp, "node2", "%s", "nocviz::link match foo bar");
	tcl_result_list_should_not_contain(interp, "node3", "%s", "nocviz::link match foo bar");
	tcl_result_list_should_not_contain(interp, "node4", "%s", "nocviz::link match foo bar");
	tcl_should_eval(interp, "%s", "nocviz::node destroy node1");
	tcl_should_eval(interp, "%s", "nocviz::node destroy node2");


	Tcl_DeleteInterp(interp);
}
