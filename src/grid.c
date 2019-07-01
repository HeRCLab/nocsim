#include "nocsim.h"

/* This file contains methods relating to the "grid" of routers, PEs, and links
 * */

#include <stdio.h>
#include <tcl.h>

/**
 * @brief Create a new router instance
 *
 * @param state
 * @param id
 * @param row
 * @param col
 */
void nocsim_grid_create_router(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior) {
	nocsim_node* router;

	alloc(sizeof(nocsim_node), router);

	nocsim_init_node(router, node_router, row, col, id);

	router->node_number = state->num_node;
	router->type_number = state->num_router;
	state->num_node++;
	state->num_router++;
	router->behavior = behavior;

	vec_push(state->nodes, router);
}

void nocsim_grid_create_PE(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior) {
	nocsim_node* PE;
	flitlist* pending;

	nocsim_init_node(PE, node_PE, row, col, id);

	alloc(sizeof(flitlist), pending);
	vec_init(pending);
	PE->pending = pending;
	PE->node_number = state->num_node;
	state->num_node++;
	PE->type_number = state->num_PE;
	state->num_PE++;
	PE->behavior = behavior;

	vec_push(state->nodes, PE);
}

void nocsim_grid_create_link(nocsim_state* state, char* from_id, char* to_id) {
	nocsim_node* from;
	nocsim_node* to;
	nocsim_link* link;
	unsigned int i;
	nocsim_node* cursor;

	alloc(sizeof(nocsim_link), link);

	from = NULL; to=NULL;
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->id == NULL) {
			err(1, "node@0x%p missing ID", (void*) cursor);
		}

		if (!strncmp(to_id, cursor->id, NOCSIM_GRID_LINELEN)) {
			to = cursor;
		}
		if (!strncmp(from_id, cursor->id, NOCSIM_GRID_LINELEN)) {
			from = cursor;
		}
	}

	dbprintf("from node=");
	dbprint_node(from);
	drprintf("\n");

	dbprintf("to node=");
	dbprint_node(to);
	drprintf("\n");

	if (from == NULL) {
		err(1, "could not link from unknown node '%s'", from_id);
	}

	if (to == NULL) {
		err(1, "could not link to unknown node '%s'", to_id);
	}

	link->to = to;
	link->from = from;
	link->flit = NULL;
	link->flit_next = NULL;

	if ((from->type == node_PE) && (to->type == node_PE)) {
		err(1, "cannot create illegal link from PE '%s' to PE '%s'",
			from_id, to_id);

	} else if (((from->type == node_PE) && (to->type == node_router)) ||
		((from->type == node_router) && (to->type == node_PE))) {
		dbprintf("link direction: PE\n");
		from->outgoing[P] = link;
		to->incoming[P] = link;

	} else {
		if (from->row < to->row) {
			dbprintf("link direction: S->N (down)\n");
			from->outgoing[S] = link;
			to->incoming[N] = link;

		} else if (from->row > to->row) {
			dbprintf("link direction: N->S (up)\n");
			from->outgoing[N] = link;
			to->incoming[S] = link;

		} else if (from->col < to->col) {
			dbprintf("link direction: E->W (right)\n");
			from->outgoing[E] = link;
			to->incoming[W] = link;

		} else if (from->col > to->col) {
			dbprintf("link direction: W->E (left)\n");
			from->outgoing[W] = link;
			to->incoming[E] = link;

		} else {
			fprintf(stderr, "nodes: ");
			nocsim_print_node(stderr, from);
			fprintf(stderr, " and ");
			nocsim_print_node(stderr, to);
			fprintf(stderr, " cannot be linked because they have overlapping coordinates\n");
			err(1, NULL);

		}
	}
}

void nocsim_grid_config(nocsim_state* state, char* key, char* val) {
	unsigned int seed;
	unsigned int max_ticks;
	char* errstr;

	if (!strncmp(key, "RNG_seed", NOCSIM_GRID_LINELEN)) {
#ifdef __OpenBSD__
		seed = (unsigned int) strtonum(val, 0, UINT_MAX, &errstr);
		if (errstr != NULL) {
			err(1, "could not parse RNG seed '%s'", val);
		}
#else
		seed = (unsigned int) strtoul(val, &errstr, 10);
#endif
		state->RNG_seed = seed;
		dbprintf("state->RNG_seed=%u\n", seed);
		free(val);

	} else if (!strncmp(key, "max_ticks", NOCSIM_GRID_LINELEN)) {
#ifdef __OpenBSD__
		max_ticks = (unsigned int) strtonum(val, 0, UINT_MAX, &errstr);
		if (errstr != NULL) {
			err(1, "could not parse max_ticks '%s'", val);
		}
#else
		max_ticks = (unsigned int) strtoul(val, &errstr, 10);
#endif
		state->max_ticks = max_ticks;
		dbprintf("state->max_ticks=%u\n", max_ticks);
		free(val);

	} else if (!strncmp(key, "title", NOCSIM_GRID_LINELEN)) {
		state->title = val;
		dbprintf("state->title=%s\n", val);

	} else {
		err(1, "invalid config definition: unknown key '%s'", key);
	}
}
