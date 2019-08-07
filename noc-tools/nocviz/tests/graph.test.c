#include "../graph.h"
#include "test_util.h"
#include "../../common/util.h"

int main() {
	nocviz_graph* g;
	nocviz_node* n;
	nocviz_node* n2;
	nocviz_node* nv;
	nocviz_link* l;
	nocviz_link* l2;
	int idx;

	/* make sure we can allocate and free a graph */
	g = nocviz_graph_init();
	nocviz_graph_free(g);

	/* should free nodes */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	should_not_be_null(n);
	should_equal(n, nocviz_graph_get_node(g, "node1"));
	nocviz_graph_free(g);

	/* create one node and then delete it */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	should_not_be_null(n);
	should_equal(n, nocviz_graph_get_node(g, "node1"));
	nocviz_graph_free_node(g, n);
	nocviz_graph_free(g);

	/* should not be able to create duplicate nodes */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	should_not_be_null(n);
	should_equal(n, nocviz_graph_get_node(g, "node1"));
	n2 = nocviz_graph_new_node(g, "node1");
	should_be_null(n2);
	should_not_equal(n2, nocviz_graph_get_node(g, "node1"));
	should_equal(n, nocviz_graph_get_node(g, "node1"));
	nocviz_graph_free(g);


	/* create two nodes a and a link */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	n2 = nocviz_graph_new_node(g, "node2");
	should_not_be_null(g);
	should_not_be_null(n);
	should_not_be_null(n2);
	l = nocviz_graph_new_link(g, "node1", "node2", NOCVIZ_LINK_UNDIRECTED);
	should_not_be_null(l);
	should_equal(l->from, nocviz_graph_get_node(g, "node1"));
	should_equal(l->to, nocviz_graph_get_node(g, "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node1", "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node2", "node1"));
	nocviz_graph_free(g);

	/* create two nodes a and a directed link */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	n2 = nocviz_graph_new_node(g, "node2");
	should_not_be_null(g);
	should_not_be_null(n);
	should_not_be_null(n2);
	l = nocviz_graph_new_link(g, "node1", "node2", NOCVIZ_LINK_DIRECTED);
	should_not_be_null(l);
	should_equal(l->from, nocviz_graph_get_node(g, "node1"));
	should_equal(l->to, nocviz_graph_get_node(g, "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node1", "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node2", "node1"));
	vec_find(g->links, l, idx);
	should_not_equal(idx, -1);
	nocviz_graph_foreach_link(g, l2, should_equal(l, l2););
	nocviz_graph_free(g);

	/* create two nodes a and a link, and free before global free */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	n2 = nocviz_graph_new_node(g, "node2");
	should_not_be_null(g);
	should_not_be_null(n);
	should_not_be_null(n2);
	l = nocviz_graph_new_link(g, "node1", "node2", NOCVIZ_LINK_UNDIRECTED);
	should_not_be_null(l);
	should_equal(l->from, nocviz_graph_get_node(g, "node1"));
	should_equal(l->to, nocviz_graph_get_node(g, "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node1", "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node2", "node1"));
	nocviz_graph_free_link(g, l);
	should_not_be_null(n);
	should_not_be_null(n2);
	nocviz_graph_free(g);


	/* check that link directionality works as it should */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	n2 = nocviz_graph_new_node(g, "node2");
	should_not_be_null(g);
	should_not_be_null(n);
	should_not_be_null(n2);
	l = nocviz_graph_new_link(g, "node1", "node2", NOCVIZ_LINK_DIRECTED);
	should_not_be_null(l);
	should_equal(l->from, nocviz_graph_get_node(g, "node1"));
	should_equal(l->to, nocviz_graph_get_node(g, "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node1", "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node2", "node1"));
	nocviz_graph_node_foreach_adj(g, n, nv,
			should_equal(nv, n2););

	/* incoming side should not have outgoing side as adjacent */
	nocviz_graph_node_foreach_adj(g, n2, nv,
			should_not_equal(nv, n););
	nocviz_graph_free(g);

	/* should not be able to create duplicate link */
	g = nocviz_graph_init();
	n = nocviz_graph_new_node(g, "node1");
	n2 = nocviz_graph_new_node(g, "node2");
	should_not_be_null(g);
	should_not_be_null(n);
	should_not_be_null(n2);
	l = nocviz_graph_new_link(g, "node1", "node2", NOCVIZ_LINK_UNDIRECTED);
	should_not_be_null(l);
	should_equal(l->from, nocviz_graph_get_node(g, "node1"));
	should_equal(l->to, nocviz_graph_get_node(g, "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node1", "node2"));
	should_equal(l, nocviz_graph_get_link(g, "node2", "node1"));
	should_be_null(nocviz_graph_new_link(g, "node1", "node2", NOCVIZ_LINK_UNDIRECTED));
	nocviz_graph_free(g);

}
