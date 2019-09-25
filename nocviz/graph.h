#ifndef NOCVIZ_GRAPH_H
#define NOCVIZ_GRAPH_H

/* threading primitives */
#include <agar/core.h>
#include <agar/gui.h>

#include <stdbool.h>

#include "datastore.h"
#include "../common/util.h"


/******************************************************************************
 *
 * Graph-representation library, where the graph itself, as well as each node
 * and link are annotated with datastores. Links may be directed or undirected.
 *
 * Each node stores a table associating adjacent node IDs with the links to
 * those nodes. Such links are always either undirected or outgoing.
 *
 * NOTE: there can only ever be zero or one links between two adjacent nodes.
 *
 *****************************************************************************/

/* mapping of strings to nodes */
KHASH_MAP_INIT_STR(mstrnode, struct nocviz_node_t*)

/* strings to links */
KHASH_MAP_INIT_STR(mstrlink, struct nocviz_link_t*)

/* list of links */
typedef vec_t(struct nocviz_link_t*) linkvec;

typedef struct nocviz_graph_t {
	khash_t(mstrnode)* nodes;
	nocviz_ds* ds;
	AG_Mutex* mutex;
	bool dirty;
	bool color_dirty;
	linkvec* links;
	struct nocviz_graph_widget* gw;

} nocviz_graph;

typedef enum nocviz_link_type_t {NOCVIZ_LINK_DIRECTED, NOCVIZ_LINK_UNDIRECTED} nocviz_link_type;

typedef struct nocviz_link_t {
	struct nocviz_node_t* from;
	struct nocviz_node_t* to;
	nocviz_link_type type;
	nocviz_ds* ds;
	char* title;

	/***** values used for GUI ******/
	AG_Color c;
	int curve;
	int label_surface;
	unsigned char surface_dirty;
	int hx;		/* hover detection */
	int hy;
	unsigned int flags;
#define NOCVIZ_LINK_SELECTED 0x1
#define NOCVIZ_LINK_HOVER 0x2
} nocviz_link;

typedef struct nocviz_node_t {
	/* keys are target node IDs, values are links */
	khash_t(mstrlink)* adjacent;
	char* id;
	char* title;
	int row;
	int col;
	nocviz_ds* ds;

	/***** values used for GUI ******/
	AG_Color c;
	int x;
	int y;
	int h;
	int w;
	unsigned int flags;
#define NOCVIZ_NODE_SELECTED 0x1
#define NOCVIZ_NODE_HOVER 0x2
	int label_surface;
	unsigned char surface_dirty;
} nocviz_node;

/* create a new graph */
nocviz_graph* nocviz_graph_init(void);

/* destroy a graph, de-allocating all member nodes and datastores */
void nocviz_graph_free(nocviz_graph* g);

/* create a new node */
nocviz_node* nocviz_graph_new_node(nocviz_graph* g, char* id);

/* create a new link */
nocviz_link* nocviz_graph_new_link(nocviz_graph* g, char* from, char* to, nocviz_link_type type);

/* retrieve a graph node by it's ID */
nocviz_node* nocviz_graph_get_node(nocviz_graph* g, char* id);

/* retrieve a link by it's endpoints */
nocviz_link* nocviz_graph_get_link(nocviz_graph* g, char* id1, char* id2);

/* reverse the src and dest of a link */
void nocviz_graph_reverse_link(nocviz_graph* g, nocviz_link* link);

/* Destroy a node, and any links attached to it.*/
void nocviz_graph_free_node(nocviz_graph* g, nocviz_node* node);

/* destroy a link */
void nocviz_graph_free_link(nocviz_graph* g, nocviz_link* link);

/* check if the graph is marked as dirty */
bool nocviz_graph_is_dirty(nocviz_graph* g);

/* update the graph's dirty state */
void nocviz_graph_set_dirty(nocviz_graph* g, bool dirty);

bool nocviz_graph_color_is_dirty(nocviz_graph* g);

void nocviz_graph_color_set_dirty(nocviz_graph* g, bool dirty);

/* ensure that the adjacency tables on either end of the link are setup
 * correctly */
void nocviz_graph_fix_link_adjacency(nocviz_graph* g, nocviz_link* link);

/* iterators */

/* every node in the graph */
#define nocviz_graph_foreach_node(g, nodevar, code) do { \
		const char* __k; \
		UNUSED(__k); \
		kh_foreach(g->nodes, __k, nodevar, code); \
	} while(0)

/* every link in the graph */
#define nocviz_graph_foreach_link(g, linkvar, code) do { \
		unsigned int __i; \
		vec_foreach(g->links, linkvar, __i) { \
			code \
		} \
	} while(0) \

/* every outgoing or undirected link originating at the given node */
#define nocviz_graph_node_foreach_link(node, linkvar, code) do { \
		const char* __k; \
		UNUSED(__k); \
		kh_foreach(node->adjacent, __k, linkvar, code); \
	} while(0)

/* every adjacent (outgoing or undirected) node relative to the given node */
#define nocviz_graph_node_foreach_adj(g, node, nodevar, code) do { \
		char* __k; \
		nocviz_link* __l; \
		kh_foreach(node->adjacent, __k, __l, \
			if (__l->to == node) { \
				nodevar = l->from; \
			} else { \
				nodevar = l->to; \
			} \
			code \
		); \
	} while(0)

/* internal non-thread-safe functions */

nocviz_node* __nocviz_graph_new_node(nocviz_graph* g, char* id);
nocviz_link* __nocviz_graph_new_link(nocviz_graph* g, char* from, char* to, nocviz_link_type type);
nocviz_node* __nocviz_graph_get_node(nocviz_graph* g, char* id);
nocviz_link* __nocviz_graph_get_link(nocviz_graph* g, char* id1, char* id2);
void __nocviz_graph_free_node(nocviz_graph* g, nocviz_node* node);
void __nocviz_graph_free_link(nocviz_graph* g, nocviz_link* link);
void __nocviz_graph_fix_link_adjacency(nocviz_graph* g, nocviz_link* link);

#endif
