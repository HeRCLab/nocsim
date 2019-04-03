#include "nocsim.h"

/**
 * @brief Perform DOR routing.
 *
 * @param node
 */
void nocsim_behavior_DOR(nocsim_node* node) {
#define iflink(dir) if (((node->incoming[dir] == NULL) ? 0 : (node->incoming[dir]->flit != NULL)))
	dbprintf("DOR behavior for ");
	dbprint_node(node);
	drprintf("\n");

	iflink(N) {
		nocsim_DOR_one(node, node->incoming[N]->flit);
	}
	
	iflink(S) {
		nocsim_DOR_one(node, node->incoming[S]->flit);
	}
	
	iflink(E) {
		nocsim_DOR_one(node, node->incoming[E]->flit);
	}
	
	iflink(W) {
		nocsim_DOR_one(node, node->incoming[W]->flit);
	}

	iflink(P)  {
		nocsim_DOR_one(node, node->incoming[P]->flit);
	}
#undef iflink
}

/**
 * @brief DOR route one flit
 *
 * @param node
 * @param flit
 */
void nocsim_DOR_one(nocsim_node* node, nocsim_flit* flit) {
	dbprintf("\tDOR routing for flit %lu\n", flit->flit_no);

#define iflink(dir) (node->outgoing[dir] == NULL) ? 0 : (node->outgoing[dir]->flit_next == NULL)
#define routelink(dir) if (iflink(dir)) { node->outgoing[dir]->flit_next = flit; \
	printf("route %lu %s to %s->%s via %s\n", \
		flit->flit_no, NOCSIM_DIRECTION_TO_STR(dir), \
		node->id, node->outgoing[dir]->to->id, node->id); \
		return; }

	if (flit->to->row > node->row) {
		     routelink(S)
		else routelink(E)
		else routelink(W)
		else routelink(N)

	} else if (flit->to->row < node->row) {
		     routelink(N)
		else routelink(E)
		else routelink(W)
		else routelink(S)

	} else if (flit->to->col > node->col) {
		     routelink(E)
		else routelink(S)
		else routelink(N)
		else routelink(W)

	} else if (flit->to->col < node->col) {
		     routelink(W)
		else routelink(S)
		else routelink(N)
		else routelink(E)

	} else {
		     routelink(P)
		else routelink(W)
		else routelink(S)
		else routelink(N)
		else routelink(E)
	}

	/* PE should have the same row,col as the router */
	if ((flit->from->col == node->col) && (flit->from->row == node->row)) {
		routelink(P);
	}

	err(1, "cannot route flit %lu from node %s, no available links",
			flit->flit_no, node->id);

#undef routelink
#undef iflink
}
