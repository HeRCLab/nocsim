#include "nocsim.h"

/* This file contains methods relating to the "grid" of routers, PEs, and links
 * */

#include <stdio.h>
#include <tcl.h>

/* note that the caller must verify that the ID is unique */
void nocsim_grid_create_router(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior) {
	nocsim_node* router;

	alloc(sizeof(nocsim_node)*2, router);

	nocsim_init_node(router, node_router, row, col, id);

	router->node_number = state->num_node;
	router->type_number = state->num_router;
	state->num_node++;
	state->num_router++;
	router->behavior = behavior;

	if (row > state->max_row) { state->max_row = row; }
	if (col > state->max_col) { state->max_col = col; }

	vec_push(state->nodes, router);
	ez_kv_insert(state->node_map, id, router);

	if (state->instruments[INSTRUMENT_NODE] != NULL) {
		if (Tcl_Evalf(state->interp, "%s {%s} {%u} {%u} {%u} {%s}",
					state->instruments[INSTRUMENT_NODE],
					router->id,
					router->type,
					router->row,
					router->col,
					router->behavior)) {
			print_tcl_error(state->interp);
			err(1, "unable to proceed, exiting with failure state");
		}
	}
}

/* note that the caller must verify that the ID is unique */
void nocsim_grid_create_PE(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior) {
	nocsim_node* PE;
	flitlist* pending;

	alloc(sizeof(nocsim_node), PE);

	nocsim_init_node(PE, node_PE, row, col, id);

	alloc(sizeof(flitlist), pending);
	vec_init(pending);
	PE->pending = pending;
	PE->node_number = state->num_node;
	state->num_node++;
	PE->type_number = state->num_PE;
	state->num_PE++;
	PE->behavior = behavior;
	if (row > state->max_row) { state->max_row = row; }
	if (col > state->max_col) { state->max_col = col; }

	vec_push(state->nodes, PE);
	ez_kv_insert(state->node_map, id, PE);

	if (state->instruments[INSTRUMENT_NODE] != NULL) {
		if (Tcl_Evalf(state->interp, "%s {%s} {%u} {%u} {%u} {%s}",
					state->instruments[INSTRUMENT_NODE],
					PE->id,
					PE->type,
					PE->row,
					PE->col,
					PE->behavior)) {
			print_tcl_error(state->interp);
			err(1, "unable to proceed, exiting with failure state");
		}
	}
}

nocsim_result nocsim_grid_create_link(nocsim_state* state, char* from_id, char* to_id, nocsim_direction from_dir, nocsim_direction to_dir) {

	// #lizard forgives the complexity

	/* if d is DIR_UNDEF, infer the direction, otherwise it is assume
	 * that d is the direction the link is pointing */

	nocsim_node* from;
	nocsim_node* to;
	nocsim_link* link;
	int bidir;

	if (state->instruments[INSTRUMENT_LINK] != NULL) {
		bidir = ((nocsim_link_by_nodes(state, from_id, to_id) != NULL) ||
				(nocsim_link_by_nodes(state, to_id, from_id) != NULL));
	}

	alloc(sizeof(nocsim_link), link);

	from = nocsim_node_by_id(state, from_id);
	to = nocsim_node_by_id(state, to_id);

	dbprintf("from node=");
	dbprint_node(from);
	drprintf("\n");

	dbprintf("to node=");
	dbprint_node(to);
	drprintf("\n");

	if (from == NULL) {
		free(link);
		nocsim_return_error(state, "could not link from unknown node '%s'", from_id);
	}

	if (to == NULL) {
		free(link);
		nocsim_return_error(state, "could not link to unknown node '%s'", to_id);
	}

	if (from == to) {
		free(link);
		nocsim_return_error(state, "could not link node '%s' to node '%s' which is the same node", from_id, to_id);
	}

	link->to = to;
	link->from = from;
	link->flit = NULL;
	link->flit_next = NULL;
	link->load = 0;

	nocsim_direction selected_to_dir;
	nocsim_direction selected_from_dir;

	if (to_dir == DIR_UNDEF && from_dir == DIR_UNDEF) {
		selected_to_dir = infer_direction(state, from_id, to_id);
		selected_from_dir = invert_direction(selected_to_dir);

	} else if (to_dir == DIR_UNDEF) {
		selected_from_dir = from_dir;
		selected_to_dir = invert_direction(from_dir);

	} else if (from_dir == DIR_UNDEF) {
		selected_to_dir = to_dir;
		selected_from_dir = invert_direction(to_dir);
	} else {
		selected_to_dir = to_dir;
		selected_from_dir = from_dir;
	}

	dbprintf("from dir=%s to dir=%s\n",
			NOCSIM_DIRECTION_TO_STR(selected_from_dir),
			NOCSIM_DIRECTION_TO_STR(selected_to_dir));


	if ( (!NOCSIM_DIRECTION_VALID(selected_to_dir)) ||
		( (!NOCSIM_DIRECTION_VALID(selected_from_dir) ))) {

		free(link);
		nocsim_return_error(state,
			"inferred invalid to/from directions %s, %s from nodes %s and %s and directions %s and %s",
			NOCSIM_DIRECTION_TO_STR(selected_from_dir),
			NOCSIM_DIRECTION_TO_STR(selected_to_dir),
			from_id, to_id,
			NOCSIM_DIRECTION_TO_STR(from_dir),
			NOCSIM_DIRECTION_TO_STR(to_dir));
	}

	if ((from->type == node_PE) && (to->type == node_PE)) {

		free(link);
		nocsim_return_error(state,
				"cannot create illegal link from PE '%s' to PE '%s'",
			from_id, to_id);
	}  else if (from->outgoing[selected_from_dir] != NULL) {

		free(link);
		nocsim_return_error(state,
				"cannot create link from %s to %s, would overwrite existing link from %s to %s",
				from_id, to_id,
				from->outgoing[selected_from_dir]->from->id,
				from->outgoing[selected_from_dir]->to->id);
	}  else if (to->incoming[selected_to_dir] != NULL) {

		free(link);
		nocsim_return_error(state,
				"cannot create link from %s to %s, would overwrite existing link from %s to %s",
				from_id, to_id,
				to->incoming[selected_to_dir]->from->id,
				to->incoming[selected_to_dir]->to->id);
	} else {

		from->outgoing[selected_from_dir] = link;
		to->incoming[selected_to_dir] = link;
	}

	vec_push(state->links, link);

	if (state->instruments[INSTRUMENT_LINK] != NULL) {
		if (Tcl_Evalf(state->interp, "%s {%s} {%s} {%i}",
					state->instruments[INSTRUMENT_LINK],
					link->from->id,
					link->to->id,
					bidir)) {
			print_tcl_error(state->interp);
			nocsim_return_error(state, "error while evaluating link instrument '%s'",
				state->instruments[INSTRUMENT_LINK]);
		}
	}

	return NOCSIM_RESULT_OK;

}
