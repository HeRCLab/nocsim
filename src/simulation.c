#include "nocsim.h"

void next_state(nocsim_state* state, Tcl_Interp* interp) {
	unsigned int i;
	nocsim_node* cursor;

	vec_foreach(state->nodes, state->current, i) {
		if (Tcl_Eval(interp, state->current->behavior) != TCL_OK) {
			print_tcl_error(interp);
			err(1, "unable to proceed, exiting with failure state");
		}
	}

	/* PEs send packets into links */
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->type != node_PE) { continue; }

		if (cursor->pending->length > 0) {

			if (cursor->outgoing[P] == NULL) {
				err(1, "PE %s does not have an outgoing link", cursor->id);
			}

			cursor->outgoing[P]->flit_next = \
				vec_dequeue(cursor->pending);

			cursor->outgoing[P]->flit_next->injected_at = state->tick;

			printf("pop %lu from %s to %s->%s\n",
				cursor->outgoing[P]->flit_next->flit_no,
				cursor->id,
				cursor->id,
				cursor->outgoing[P]->to->id);
		}
	}

}

void flip_state(nocsim_state* state) {
	unsigned int i;
	nocsim_node* cursor;

	vec_foreach(state->nodes, cursor, i) {
		for (nocsim_direction dir = N ; dir <= P ; dir++) {
			if (cursor->incoming[dir] != NULL) {
				if (cursor->incoming[dir]->flit != NULL) {
					err(1, "invalid state: router %s has unhandled incoming flits after behavior execution",
						cursor->id);
				}

				cursor->incoming[dir]->flit = \
					cursor->incoming[dir]->flit_next;
				cursor->incoming[dir]->flit_next = NULL;
			}
		}
	}

	/* check if packet arrived */
	vec_foreach(state->nodes, cursor, i) {
		for (nocsim_direction dir = N ; dir <= P ; dir++) {
			if (cursor->incoming[dir] != NULL) {
				nocsim_handle_arrival(state, cursor, dir);
			}

		}
	}

}

void nocsim_step(nocsim_state* state, Tcl_Interp* interp) {

	printf("tick %lu\n", state->tick);

	if (state->instruments[INSTRUMENT_TICK] != NULL) {
		if (Tcl_Eval(interp, state->instruments[INSTRUMENT_TICK]) != TCL_OK) {
			print_tcl_error(interp);
			err(1, "unable to proceed, exiting with failure state");
		}
	}


	next_state(state, interp);
	flip_state(state);

	state->tick++;
}

/**
 * @brief Handle a single node's incoming flits for a specific direction.
 *
 * @param cursor
 * @param dir
 */
void nocsim_handle_arrival(nocsim_state* state, nocsim_node* cursor, nocsim_direction dir) {
	nocsim_flit* flit;

	// do nothing if there isn't anything coming from this direction
	if (cursor->incoming[dir]->flit == NULL) {
		return;

	} else if (cursor->incoming[dir]->flit->to == cursor) {
		if (cursor->type == node_router) {
			err(1, "router %s received flit %lu destined for it, but routers may not be the destination for flits",
				cursor->id,
				cursor->incoming[dir]->flit->flit_no);
		}

		flit = cursor->incoming[dir]->flit;

		if (state->instruments[INSTRUMENT_ARRIVE] != NULL) {
			if (Tcl_Evalf(state->interp, "%s \"%s\" \"%s\" %lu %lu %lu %lu",
						state->instruments[INSTRUMENT_ARRIVE],
						flit->from->id, flit->to->id,
						flit->flit_no,
						flit->hops, flit->spawned_at,
						flit->injected_at
						)) {
				print_tcl_error(state->interp);
				err(1, "unable to proceed, exiting with failure state");
			}
		}

		/* delete the flit */
		free((cursor->incoming[dir]->flit));
		cursor->incoming[dir]->flit = NULL;

	} else if (dir == P) {
		/* insert into top of FIFO, as this should always be going back
		 * into the PE that injected it */

		if (cursor->type == node_router) {
			/* not applicable to routers */
			return;
		}

		vec_insert(cursor->pending, 0, cursor->incoming[P]->flit);

		flit = cursor->incoming[dir]->flit;

		if (state->instruments[INSTRUMENT_BACKROUTE] != NULL) {
			if (Tcl_Evalf(state->interp, "%s \"%s\" \"%s\" %lu %lu %lu %lu",
						state->instruments[INSTRUMENT_BACKROUTE],
						flit->from->id, flit->to->id,
						flit->flit_no,
						flit->hops, flit->spawned_at,
						flit->injected_at
						)) {
				print_tcl_error(state->interp);
				err(1, "unable to proceed, exiting with failure state");
			}
		}


		/* remove the flit from the incoming list */
		cursor->incoming[P]->flit = NULL;
	}
}

void nocsim_inject(nocsim_state* state, nocsim_node* from, nocsim_node* to) {
	nocsim_flit* flit;

	alloc(sizeof(nocsim_flit), flit);

	if (from == NULL) {
		dbprintf("called on null from! (flit=%p)\n", (void*) flit);
	}

	flit->from = from;
	flit->to = to;
	flit->spawned_at = state->tick;
	flit->injected_at = 0;
	flit->flit_no = state->flit_no;
	flit->hops = 0;
	state->flit_no ++;

	/* insert into FIFO */
	vec_push(from->pending, flit);

	if (state->instruments[INSTRUMENT_INJECT] != NULL) {
		if (Tcl_Evalf(state->interp, "%s \"%s\" \"%s\" %lu",
					state->instruments[INSTRUMENT_INJECT],
					from->id, to->id, flit->flit_no)) {
			print_tcl_error(state->interp);
			err(1, "unable to proceed, exiting with failure state");
		}
	}
}
