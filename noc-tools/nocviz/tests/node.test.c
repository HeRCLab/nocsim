/* test that the node command group is working as it should */

#include "../datastore.h"
#include "../nocviz.h"
#include "test_util.h"
#include "../../common/util.h"
#include "../../3rdparty/vec.h"

#include <tcl.h>

int main() {
	Tcl_Interp* interp;
	Tcl_Namespace* nsPtr;
	nocviz_graph* g;
	nocviz_node* n;

	interp = Tcl_CreateInterp();
	Nocviz_Init(interp);
	nsPtr = Tcl_FindNamespace(interp, "nocviz", NULL, 0);
	should_not_be_null(nsPtr);
	should_not_be_null(nsPtr->clientData);
	g = nsPtr->clientData;


	/* node command on it's own should error */
	tcl_should_not_eval(interp, "%s", "nocviz::node");


	/* create subcommand should error without arg */
	tcl_should_not_eval(interp, "%s", "nocviz::node create");


	/* destroy subcommand should error without arg */
	tcl_should_not_eval(interp, "%s", "nocviz::node destroy");


	/* create then destroy should not memory leak*/
	tcl_should_eval(interp, "nocviz::node create %s", "test1");
	should_not_be_null(nocviz_graph_get_node(g, "test1"));
	tcl_should_eval(interp, "nocviz::node destroy %s", "test1");

	/* should not be able to create twice */
	tcl_should_eval(interp, "nocviz::node create %s", "test1");
	tcl_should_not_eval(interp, "nocviz::node create %s", "test1");
	tcl_should_eval(interp, "nocviz::node destroy %s", "test1");

	/* or destroy twice */
	tcl_should_not_eval(interp, "nocviz::node destroy %s", "test1");


	/* node row and col */
	tcl_should_eval(interp, "nocviz::node create %s", "test1");

	/* missing parameter */
	tcl_should_not_eval(interp, "nocviz::node row %s", "test1");
	tcl_should_not_eval(interp, "nocviz::node col %s", "test1");

	/* invalid parameter */
	tcl_should_not_eval(interp, "nocviz::node row %s abcdef", "test1");
	tcl_should_not_eval(interp, "nocviz::node col %s abcdef", "test1");

	/* actually set the values */
	tcl_should_eval(interp, "nocviz::node row %s 5", "test1");
	tcl_should_eval(interp, "nocviz::node col %s 7", "test1");
	n = nocviz_graph_get_node(g, "test1");
	should_not_be_null(n);
	should_equal(n->row, 5);
	should_equal(n->col, 7);
	tcl_should_eval(interp, "nocviz::node destroy %s", "test1");


	/* node title subcommand */
	tcl_should_eval(interp, "nocviz::node create %s", "test1");
	n = nocviz_graph_get_node(g, "test1");
	should_not_be_null(n);
	str_should_equal(n->title, "test1");
	tcl_should_eval(interp, "nocviz::node title %s NEWTITLE", "test1");
	str_should_equal(n->title, "NEWTITLE");
	tcl_should_eval(interp, "nocviz::node destroy %s", "test1");


	/* node data get / set subcommand */
	tcl_should_eval(interp, "nocviz::node create %s", "test1");
	n = nocviz_graph_get_node(g, "test1");
	should_not_be_null(n);

	/* set first time */
	tcl_should_eval(interp, "%s", "nocviz::node data set test1 foo bar");
	should_not_be_null(nocviz_ds_get_kvp(n->ds, "foo"));
	str_should_equal(nocviz_ds_get_kvp(n->ds, "foo"), "bar");

	/* retrieve first time */
	tcl_should_eval(interp, "%s", "nocviz::node data get test1 foo");
	str_should_equal("bar", Tcl_GetStringResult(interp));

	/* overwrite */
	tcl_should_eval(interp, "%s", "nocviz::node data set test1 foo baz");
	should_not_be_null(nocviz_ds_get_kvp(n->ds, "foo"));
	str_should_equal(nocviz_ds_get_kvp(n->ds, "foo"), "baz");

	/* retrieve overwritten value */
	tcl_should_eval(interp, "%s", "nocviz::node data get test1 foo");
	str_should_equal("baz", Tcl_GetStringResult(interp));

	/* retrieve nonexistant value */
	tcl_should_not_eval(interp, "%s", "nocviz::node data get test1 quux");
	tcl_should_eval(interp, "nocviz::node destroy %s", "test1");


	/* node data fmt subcommand */
	tcl_should_eval(interp, "nocviz::node create %s", "test1");
	n = nocviz_graph_get_node(g, "test1");
	should_not_be_null(n);

	/* set first time */
	tcl_should_eval(interp, "%s", "nocviz::node data fmt test1 abc xyz");
	should_not_be_null(nocviz_ds_get_fmt(n->ds, "abc"));
	str_should_equal(nocviz_ds_get_fmt(n->ds, "abc"), "xyz");

	/* overwrite */
	tcl_should_eval(interp, "%s", "nocviz::node data fmt test1 abc def");
	should_not_be_null(nocviz_ds_get_fmt(n->ds, "abc"));
	str_should_equal(nocviz_ds_get_fmt(n->ds, "abc"), "def");

	tcl_should_eval(interp, "nocviz::node destroy %s", "test1");

	Tcl_DeleteInterp(interp);

}
