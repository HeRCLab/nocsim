#include "graph.h"

nocviz_graph* nocviz_graph_init(void) {
	nocviz_graph* g;

	g = noctools_malloc(sizeof(nocviz_graph));

	g->nodes = kh_init(mstrnode);
	g->ds = nocviz_ds_init();
	g->mutex = noctools_malloc(sizeof(AG_Mutex));
	AG_MutexInit(g->mutex);
	g->dirty = true;
	g->color_dirty = false;
	g->links = noctools_malloc(sizeof(linkvec));
	g->gw = NULL;
	vec_init(g->links);

	return g;
}

void nocviz_graph_free(nocviz_graph* g) {
	nocviz_node* node;

	noctools_mutex_lock(g->mutex);

	nocviz_graph_foreach_node(g, node, __nocviz_graph_free_node(g, node));

	kh_destroy(mstrnode, g->nodes);

	nocviz_ds_free(g->ds);

	noctools_mutex_unlock(g->mutex);
	free(g->mutex);
	vec_deinit(g->links);
	free(g->links);

	free(g);
}

inline nocviz_node* nocviz_graph_new_node(nocviz_graph* g, char* id) {
	nocviz_node* result;
	noctools_mutex_lock(g->mutex);
	result = __nocviz_graph_new_node(g, id);
	noctools_mutex_unlock(g->mutex);
	return result;
}

nocviz_node* __nocviz_graph_new_node(nocviz_graph* g, char* id) {
	nocviz_node* n;
	khint_t iter;
	int r;

	/* cannot create a node over top of an existing node */
	if (__nocviz_graph_get_node(g, id) != NULL) { return NULL; }

	n = noctools_malloc(sizeof(nocviz_node));
	n->adjacent = kh_init(mstrlink);
	n->id = strdup(id);
	n->ds = nocviz_ds_init();
	n->title = strdup(id);
	n->row = 0;
	n->col = 0;
	n->x = 0;
	n->y = 0;
	n->h = 32;	/* XXX: should be macro-ed out */
	n->w = 32;
	n->flags = 0;
	n->label_surface = -1;
	AG_ColorRGBA_8(&n->c, 128,128,128, 255);

	g->dirty = true;

	iter = kh_put(mstrnode, g->nodes, n->id, &r);
	kh_val(g->nodes, iter) = n;

	return n;
}


inline nocviz_link* nocviz_graph_new_link(nocviz_graph* g, char* from, char* to, nocviz_link_type type) {
	nocviz_link* result;
	noctools_mutex_lock(g->mutex);
	result = __nocviz_graph_new_link(g, from, to, type);
	noctools_mutex_unlock(g->mutex);
	return result;

}

nocviz_link* __nocviz_graph_new_link(nocviz_graph* g, char* from, char* to, nocviz_link_type type) {
	nocviz_node* from_node;
	nocviz_node* to_node;
	nocviz_link* link;

	from_node = __nocviz_graph_get_node(g, from);
	to_node = __nocviz_graph_get_node(g, to);

	dbprintf("creating link from '%s' (%p) to '%s' (%p)\n",
			from, (void*) from_node, to, (void*) to_node);

	if (__nocviz_graph_get_link(g, from_node->id, to_node->id) != NULL) {
		dbprintf("\tduplicate link found, not proceeding\n");
		return NULL;
	}

	/* both endpoints must exist */
	if ((from_node == NULL) || (to_node == NULL)) { return NULL; }

	g->dirty = true;

	/* create the link itself */
	link = noctools_malloc(sizeof(nocviz_link));
	link->from = from_node;
	link->to = to_node;
	link->type = type;
	link->ds = nocviz_ds_init();
	link->curve = 0;
	link->label_surface = -1;
	if (asprintf(&(link->title), "%s -> %s", from, to) < 0) {
		warn("asprintf failure!");
	}
	AG_ColorRGBA_8(&link->c, 0,0,0, 0);

	__nocviz_graph_fix_link_adjacency(g, link);

	vec_push(g->links, link);

	return link;
}

inline nocviz_node* nocviz_graph_get_node(nocviz_graph* g, char* id) {
	nocviz_node* result;
	noctools_mutex_lock(g->mutex);
	result = __nocviz_graph_get_node(g, id);
	noctools_mutex_unlock(g->mutex);
	return result;
}

nocviz_node* __nocviz_graph_get_node(nocviz_graph* g, char* id) {
	khint_t iter;

	iter = kh_get(mstrnode, g->nodes, id);

	/* no such node */
	if (iter == kh_end(g->nodes)) { return NULL; }

	return kh_val(g->nodes, iter);
}



inline nocviz_link* nocviz_graph_get_link(nocviz_graph* g, char* id1, char* id2) {
	nocviz_link* result;
	noctools_mutex_lock(g->mutex);
	result = __nocviz_graph_get_link(g, id1, id2);
	noctools_mutex_unlock(g->mutex);
	return result;
}

nocviz_link* __nocviz_graph_get_link(nocviz_graph* g, char* id1, char* id2) {
	nocviz_node* node1;
	nocviz_node* node2;
	khint_t iter;

	node1 = __nocviz_graph_get_node(g, id1);
	node2 = __nocviz_graph_get_node(g, id2);

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

inline void nocviz_graph_free_node(nocviz_graph* g, nocviz_node* node) {
	noctools_mutex_lock(g->mutex);
	__nocviz_graph_free_node(g, node);
	noctools_mutex_unlock(g->mutex);
}

void __nocviz_graph_free_node(nocviz_graph* g, nocviz_node* node) {
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
				__nocviz_graph_free_link(g, link);
			} else if (link->from == node) {
				__nocviz_graph_free_link(g, link);
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
	free(node->title);
	kh_destroy(mstrlink, node->adjacent);
	free(node);

	g->dirty = true;


}

inline void nocviz_graph_free_link(nocviz_graph* g, nocviz_link* link) {
	noctools_mutex_lock(g->mutex);
	__nocviz_graph_free_link(g, link);
	noctools_mutex_unlock(g->mutex);
}

void __nocviz_graph_free_link(nocviz_graph* g, nocviz_link* link) {
	khint_t iter;

	dbprintf("freeing link %p from %s (%p) to %s (%p)\n",
		(void*) link, link->from->id, (void*) link->from, link->to->id, (void*) link->to);

	vec_remove(g->links, link);

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

bool nocviz_graph_is_dirty(nocviz_graph* g) {
	bool result;
	noctools_mutex_lock(g->mutex);
	result = g->dirty;
	noctools_mutex_unlock(g->mutex);
	return result;
}

void nocviz_graph_set_dirty(nocviz_graph* g, bool dirty) {
	noctools_mutex_lock(g->mutex);
	g->dirty = dirty;
	noctools_mutex_unlock(g->mutex);
}

void nocviz_graph_reverse_link(nocviz_graph* g, nocviz_link* link) {

	noctools_mutex_lock(g->mutex);

	/* swap to and from */
	nocviz_node* from = link->from;
	nocviz_node* to = link->to;
	link->from = to;
	link->to = from;

	/* make sure the adjacent tables are accurate */
	__nocviz_graph_fix_link_adjacency(g, link);

	noctools_mutex_unlock(g->mutex);
}

inline void nocviz_graph_fix_link_adjacency(nocviz_graph* g, nocviz_link* link) {
	noctools_mutex_lock(g->mutex);
	__nocviz_graph_fix_link_adjacency(g, link);
	noctools_mutex_unlock(g->mutex);
}

void __nocviz_graph_fix_link_adjacency(nocviz_graph* g, nocviz_link* link) {
	khint_t iter;
	int r;
	nocviz_node* from = link->from;
	nocviz_node* to= link->to;
	UNUSED(g);

	/* should always have a link in the from node */
	iter = kh_put(mstrlink, from->adjacent, to->id, &r);
	kh_val(from->adjacent, iter) = link;

	if (link->type != NOCVIZ_LINK_DIRECTED) {
		/* directed links will have a link in the to node also */
		iter = kh_put(mstrlink, to->adjacent, from->id, &r);
		kh_val(to->adjacent, iter) = link;
	} else {
		/* undirected links should not */
		iter = kh_get(mstrlink, to->adjacent, from->id);
		if (iter != kh_end(to->adjacent)) {
			kh_del(mstrlink, to->adjacent, iter);
		}
	}

}

bool nocviz_graph_color_is_dirty(nocviz_graph* g) {
	bool result;
	noctools_mutex_lock(g->mutex);
	result = g->color_dirty;
	noctools_mutex_unlock(g->mutex);
	return result;
}

void nocviz_graph_color_set_dirty(nocviz_graph* g, bool dirty) {
	noctools_mutex_lock(g->mutex);
	g->color_dirty = dirty;
	noctools_mutex_unlock(g->mutex);
}

