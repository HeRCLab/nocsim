#include "nocsim.h"
#include "map.h"

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

void nocsim_behavior_ADOR(nocsim_node* node) {
	dbprintf("ADOR for node ");
	dbprint_node(node);
	drprintf("starting\n");

/* note that ADOR will never select the flit coming out of the PE, even if it
 * is in fact the oldest, as the PE can then no longer guarantee that it will
 * have enough outgoing links to satisfy every incoming flit without
 * backrouting one into the PE (and we only allow backrouting flits into their
 * originating PE) */

/* the foundit flags accounts for the case where the PE is the only incoming
 * flit, in which case the assign map will never assign oldest. To that end,
 * if the assign macro never assigns anything, we short out the frompe check */

#define iflink(dir) if (((node->incoming[dir] == NULL) ? 0 : (node->incoming[dir]->flit != NULL)))
#define frompe(flit) ((flit->from->col == node->col) && (flit->from->row == node->row))
#define assign(dir) iflink(dir) { if ( ! frompe(node->incoming[dir]->flit)) { oldest = dir; foundit = 1;} }
#define search(dir) iflink(dir) { \
	if ((node->incoming[dir]->flit->spawned_at < node->incoming[oldest]->flit->spawned_at) && ! \
frompe(node->incoming[dir]->flit)) \
	{oldest = dir;} }


	nocsim_direction oldest;
	unsigned char foundit;
	foundit = 0;

	MAP(assign, N, S, E, W, P);
#undef frompe
#define frompe(flit) ((flit->from->col == node->col) && (flit->from->row == node->row) && (foundit == 1))
	MAP(assign, N, S, E, W, P); /* should guarantee that we found
				       _something_ if there is anything to find
				       */
	if (foundit == 0) { /* there are no incoming flits */
		return;
	}

	MAP(search, N, S, E, W, P);

	dbprintf("ADOR selected oldest %lu from %s\n",
			node->incoming[oldest]->flit->flit_no, 
			NOCSIM_DIRECTION_TO_STR(oldest));

	nocsim_DOR_one(node, node->incoming[oldest]->flit);

#undef iflink
#define iflink(dir) if ((((node->incoming[dir] == NULL) ? 0 : (node->incoming[dir]->flit != NULL))) && dir != oldest)

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

	dbprintf("ADOR for node ");
	dbprint_node(node);
	drprintf(" done\n");


#undef iflink
#undef assign
#undef search
#undef frompe
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
		else routelink(N)
		else routelink(E)
		else routelink(W)

	} else if (flit->to->row < node->row) {
		     routelink(N)
		else routelink(S)
		else routelink(E)
		else routelink(W)

	} else if (flit->to->col > node->col) {
		     routelink(E)
		else routelink(W)
		else routelink(S)
		else routelink(N)

	} else if (flit->to->col < node->col) {
		     routelink(W)
		else routelink(E)
		else routelink(S)
		else routelink(N)

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
