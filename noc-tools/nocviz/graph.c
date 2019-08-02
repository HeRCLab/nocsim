#include "graph.h"

nocviz_graph* nocviz_graph_init(void) {
	nocviz_graph* g;

	g = noctools_malloc(sizeof(nocviz_graph));

	g->nodes = kh_init(mstrnode);
	g->ds = nocviz_ds_init();

	return g;
}

void nocviz_graph_free(nocviz_graph* g) {
	nocviz_node* node;

	nocviz_graph_foreach_node(g, node, nocviz_graph_free_node(g, node));

	kh_destroy(mstrnode, g->nodes);

	nocviz_ds_free(g->ds);

	free(g);
}

nocviz_node* nocviz_graph_new_node(nocviz_graph* g, char* id) {
	nocviz_node* n;
	khint_t iter;
	int r;

	/* cannot create a node over top of an existing node */
	if (nocviz_graph_get_node(g, id) != NULL) { return NULL; }

	n = noctools_malloc(sizeof(nocviz_node));
	n->adjacent = kh_init(mstrlink);
	n->id = strdup(id);
	n->ds = nocviz_ds_init();

	iter = kh_put(mstrnode, g->nodes, n->id, &r);
	kh_val(g->nodes, iter) = n;

	return n;
}

nocviz_link* nocviz_graph_new_link(nocviz_graph* g, char* from, char* to, nocviz_link_type type) {
	nocviz_node* from_node;
	nocviz_node* to_node;
	nocviz_link* lnk;
	khint_t iter;
	int r;

	from_node = nocviz_graph_get_node(g, from);
	to_node = nocviz_graph_get_node(g, to);

	dbprintf("creating link from '%s' (%p) to '%s' (%p)\n",
			from, (void*) from_node, to, (void*) to_node);

	if (nocviz_graph_get_link(g, from_node->id, to_node->id) != NULL) {
		dbprintf("\tduplicate link found, not proceeding\n");
		return NULL;
	}

	/* both endpoints must exist */
	if ((from_node == NULL) || (to_node == NULL)) { return NULL; }

	/* create the link itself */
	lnk = noctools_malloc(sizeof(nocviz_link));
	lnk->from = from_node;
	lnk->to = to_node;
	lnk->type = type;
	lnk->ds = nocviz_ds_init();

	/* if the link is undirected, we are adjacent to the to node */
	if (type != NOCVIZ_LINK_DIRECTED) {
		iter = kh_put(mstrlink, to_node->adjacent, from_node->id, &r);
		kh_val(to_node->adjacent, iter) = lnk;
	}

	/* always adjacent to the from node */
	iter = kh_put(mstrlink, from_node->adjacent, to_node->id, &r);
	kh_val(from_node->adjacent, iter) = lnk;

	return lnk;
}

nocviz_node* nocviz_graph_get_node(nocviz_graph* g, char* id) {
	khint_t iter;

	iter = kh_get(mstrnode, g->nodes, id);

	/* no such node */
	if (iter == kh_end(g->nodes)) { return NULL; }

	return kh_val(g->nodes, iter);
}

nocviz_link* nocviz_graph_get_link(nocviz_graph* g, char* id1, char* id2) {
	nocviz_node* node1;
	nocviz_node* node2;
	khint_t iter;

	node1 = nocviz_graph_get_node(g, id1);
	node2 = nocviz_graph_get_node(g, id2);

	if ((node1 == NULL) || (node2 == NULL)) { return NULL; }

	/* adjacent to node 1 */
	iter = kh_get(mstrlink, node1->adjacent, id2);
	if (iter != kh_end(node1->adjacent)) {
		return kh_val(node1->adjacent, iter);
	}

	/* adjacent to node 2 */
	iter = kh_get(mstrlink, node2->adjacent, id1);
	if (iter != kh_end(node2->adjacent)) {
		return kh_val(node2->adjacent, iter);
	}

	/* nodes exist, but aren't linked */
	return NULL;
}

void nocviz_graph_free_node(nocviz_graph* g, nocviz_node* node) {
	nocviz_link* link;
	nocviz_node* n;
	khint_t iter;

	dbprintf("freeing node %s (%p)\n", node->id, (void*) node);

	/* free all links, we have to iterate over every link and node because
	 * there may be incoming links we don't have handles to from this node
	 * */
	nocviz_graph_foreach_node(g, n,
		nocviz_graph_node_foreach_link(n, link,
			if (link->to == node) {
				nocviz_graph_free_link(g, link);
			} else if (link->from == node) {
				nocviz_graph_free_link(g, link);
			}
		);
	);

	/* remove from node list */
	iter = kh_get(mstrnode, g->nodes, node->id);
	if (iter != kh_end(g->nodes)) {
		kh_del(mstrnode, g->nodes, iter);
	}

	/* clean up data structures */
	nocviz_ds_free(node->ds);
	free(node->id);
	kh_destroy(mstrlink, node->adjacent);
	free(node);

}

void nocviz_graph_free_link(nocviz_graph* g, nocviz_link* link) {
	khint_t iter;

	UNUSED(g);

	dbprintf("freeing link %p from %s (%p) to %s (%p)\n",
		(void*) link, link->from->id, (void*) link->from, link->to->id, (void*) link->to);

	/* remove link on from side */
	iter = kh_get(mstrlink, link->from->adjacent, link->to->id);
	if (iter != kh_end(link->from->adjacent)) {
		kh_del(mstrlink, link->from->adjacent, iter);
	}

	/* remove link on to side */
	if (link->type != NOCVIZ_LINK_DIRECTED) {
		iter = kh_get(mstrlink, link->to->adjacent, link->from->id);
		if (iter != kh_end(link->to->adjacent)) {
			kh_del(mstrlink, link->to->adjacent, iter);
		}
	}

	nocviz_ds_free(link->ds);
	free(link);
}
