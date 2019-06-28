#include "nocsim.h"

void nocsim_step(nocsim_state* state) {
	nocsim_node* cursor;
	unsigned int i;

	dbprintf("beginning tick number %lu\n", state->tick);
	printf("tick %lu\n", state->tick);

	/* PEs generate packets */
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->type == node_PE) {
			if (with_P(cursor->P_inject)) {
				nocsim_inject(state, cursor);
			}
		}
	}

	/* PEs send packets into links */
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->type != node_PE) { continue; }

		if (cursor->pending->length > 0) {

			cursor->outgoing[P]->flit = \
				vec_dequeue(cursor->pending);

			printf("pop %lu from %s to %s->%s\n",
				cursor->outgoing[P]->flit->flit_no,
				cursor->id,
				cursor->id,
				cursor->outgoing[P]->to->id);
		}
	}

	/* calculate next state */
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->type != node_router) {
			continue;
		} else if (cursor->behavior == NULL) {
			err(1, "%s %s has no defined behavior\n",
				NOCSIM_NODE_TYPE_TO_STR(cursor->type),
				cursor->id);
		} else {
			cursor->behavior(cursor);
		}
	}

	/* flip states */
	vec_foreach(state->nodes, cursor, i) {
		for (nocsim_direction dir = N ; dir <= P ; dir++) {
			if (cursor->incoming[dir] != NULL) {
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
				nocsim_handle_arrival(cursor, dir);
			}

		}
	}

	state->tick++;
}

/**
 * @brief Handle a single node's incoming flits for a specific direction.
 *
 * @param cursor
 * @param dir
 */
void nocsim_handle_arrival(nocsim_node* cursor, nocsim_direction dir) {
	// do nothing if there isn't anything coming from this direction
	if (cursor->incoming[dir]->flit == NULL) {
		return;
	} else if (cursor->incoming[dir]->flit->to == cursor) {
		printf("arrived %lu at %s\n",
			cursor->incoming[dir]->flit->flit_no,
			cursor->id);

		/* delete the flit */
		free((cursor->incoming[dir]->flit));
		cursor->incoming[dir]->flit = NULL;

	} else if (dir == P) {
		/* insert into top of FIFO, as this should always be going back
		 * into the PE that injected it */

		vec_insert(cursor->pending, 0, cursor->incoming[P]->flit);

		printf("backrouted %lu at %s\n",
			cursor->incoming[dir]->flit->flit_no,
			cursor->id);

		printf("push %lu into %s\n",
				cursor->incoming[dir]->flit->flit_no,
				cursor->id);

		/* remove the flit from the incoming list */
		cursor->incoming[P]->flit = NULL;
	}
}

/**
 * @brief Unconditionally inject a flit
 *
 * @param head
 * @param from
 */
void nocsim_inject(nocsim_state* state, nocsim_node* from) {
	unsigned int target_PE_num;
	unsigned int counter;
	nocsim_node* cursor;
	nocsim_node* to;
	nocsim_flit* flit;
	unsigned int i;

	to = NULL;

	/* Find a target PE that isn't us */
	do {
		target_PE_num = randrange(0,
			state->num_PE);
	} while (target_PE_num == from->type_number);

	/* find the target */
	counter = 0;
	vec_foreach(state->nodes, cursor, i) {
		if (cursor->type != node_PE) {continue;}

		if (counter == target_PE_num) {
			to = cursor;
			break;
		}
		counter ++;
	}

	if (to == NULL) {
		err(1, "could not find PE# %u", target_PE_num);
	}

	alloc(sizeof(nocsim_flit), flit);

	if (from == NULL) {
		dbprintf("called on null from! (flit=%p)\n", (void*) flit);
	}

	flit->from = from;
	flit->to = to;
	flit->spawned_at = state->tick;
	flit->flit_no = state->flit_no;
	state->flit_no ++;

	/* insert into FIFO */
	vec_push(from->pending, flit);

	printf("inject %lu from %s %u %u to %s %u %u\n",
			flit->flit_no,
			from->id, from->row, from->col,
			to->id, to->row, to->col);
	printf("push %lu into %s\n", flit->flit_no, from->id);

}
